// [source]
#pragma once

#include "JLink_Probe.hpp"

#include "TestFirmware.h"
static constexpr uint32_t addr_FLASH_PERIOD = 0x2000026C; // uint32_t

namespace HWD::Probe {

    /////////////////////////////////////////////////////////
    using namespace Driver::JLink_Types;
    /////////////////////////////////////////////////////////

    JLink_Probe::JLink_Probe(const Driver::JLink_Types::ProbeInfo& probeInfo) {
        m_Driver = std::make_shared<Driver::JLink_Driver>();

        m_ModelName          = probeInfo.modelName;
        m_SerialNumberString = std::to_string(probeInfo.serialNumber);
        m_RawSerialNumber    = probeInfo.serialNumber;
    }

    JLink_Probe::~JLink_Probe() {
        HWDLOG_PROBE_TRACE("JLink_Probe::~JLink_Probe() [{0}]", fmt::ptr(this));
    }

    //////////////////////////////////////////////////////////////////////

    bool JLink_Probe::IsInitialized() const {
        return m_Driver->IsLoaded();
    }

    void JLink_Probe::DisableFlashProgressPopup() {
        char charBuf[256];
        auto ret = m_Driver->ExecuteCommand(Driver::JLink_Types::Commands::DISABLE_FLASH_PROGRESS_POPUP, charBuf, sizeof(charBuf) - 1);
        if (ret)
            HWDLOG_PROBE_WARN("JLink_Probe[{0}] failed to disable internal flash progress popup: {1}", fmt::ptr(this), charBuf);
    }

    bool JLink_Probe::OpenConnection() {
        if (IsConnectionOpen()) {
            HWDLOG_PROBE_WARN("JLinkProbe[{0}]->OpenConnection connection already open", fmt::ptr(this));
            return false;
        }

        const char* openStatus = m_Driver->Open();

        if (openStatus) {
            HWDLOG_PROBE_ERROR("JLinkProbe[{0}]->OpenConnection error: {1}", fmt::ptr(this), openStatus);
            return false;
        }

        DisableFlashProgressPopup();

        HWDLOG_PROBE_TRACE("JLinkProbe[{0}]->OpenConnection connected to probe", fmt::ptr(this));

        char resp[1024] = {0};

        m_Driver->SetSpeed(50000);

        m_Driver->SetErrorOutHandler([](const char* str) {
            HWDLOG_PROBE_ERROR("JLink Error: {0}", str ? str : "Unknown");
        });

        m_Driver->SetWarnOutHandler([](const char* str) {
            HWDLOG_PROBE_WARN("JLink Warning: {0}", str ? str : "Unknown");
        });

        m_Driver->SetFlashProgProgressCallback([](const char* action, const char* prog, int percentage) {
            HWDLOG_PROBE_ERROR("JLink flash progress {0} - {1}: {2}%", action ? action : "-", prog ? prog : "-", percentage);
        });

        //m_Driver->EnableLog([](const char* str) {
        //    HWDLOG_PROBE_TRACE("JLink log: {0}", str ? str : "Unknown");
        //});
        //m_Driver->EnableLogCom([](const char* str) {
        //    HWDLOG_PROBE_TRACE("JLink log com: {0}", str ? str : "Unknown");
        //});

        resp[0]  = 0;
        auto ret = m_Driver->ExecuteCommand("Device = TM4C129ENCPDT\n", resp, 1024);
        HWDLOG_PROBE_TRACE("Device select return: {0} - resp: {1}", ret, resp);

        ret = m_Driver->TIF_Select(TargetInterface::SWD);
        HWDLOG_PROBE_TRACE("SWD select return: {0}", ret);

        ret = m_Driver->Reset();
        HWDLOG_PROBE_TRACE("Reset return: {0}", ret);

        ret = m_Driver->EraseChip();
        HWDLOG_PROBE_TRACE("EraseChip return: {0}", ret);

        ret = m_Driver->Reset();
        HWDLOG_PROBE_TRACE("Reset return: {0}", ret);

        m_Driver->BeginDownload(DownloadFlags::ALLOW_FLASH);
        HWDLOG_PROBE_TRACE("BeginDownload");

        m_Driver->WriteMem(0, sizeof(firmware), (void*)firmware);
        HWDLOG_PROBE_TRACE("WriteMem return: {0}", ret);

        ret = m_Driver->EndDownload();
        HWDLOG_PROBE_TRACE("EndDownload return: {0}", ret);

        ret = m_Driver->Reset();
        HWDLOG_PROBE_TRACE("Reset return: {0}", ret);

        m_Driver->Go();

        std::thread t([=]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            uint32_t fp    = 0;
            uint8_t status = 0;
            auto ret       = m_Driver->ReadMemU32(addr_FLASH_PERIOD, 1, &fp, &status);
            HWDLOG_PROBE_TRACE("ReadMemU32 return: {0} - status {1}", ret, status);

            HWDLOG_PROBE_ERROR("Current FLASH_PERIOD: {0}", fp);
        });
        t.join();

        std::thread t1([=]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            auto ret = m_Driver->WriteU32(addr_FLASH_PERIOD, 100);
            HWDLOG_PROBE_TRACE("WriteU32 set flash period to 100ms return: {0}", ret);
        });
        t1.join();

        std::thread t2([=]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            auto ret = m_Driver->WriteU32(addr_FLASH_PERIOD, 50);
            HWDLOG_PROBE_TRACE("WriteU32 set flash period to 50ms return: {0}", ret);
        });
        t2.join();

        std::thread t3([=]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            auto ret = m_Driver->WriteU32(addr_FLASH_PERIOD, 1000);
            HWDLOG_PROBE_TRACE("WriteU32 set flash period to 1000ms return: {0}", ret);
        });
        t3.join();

        return true;
    }

    bool JLink_Probe::CloseConnection() {
        if (IsConnectionOpen()) {
            HWDLOG_PROBE_WARN("JLinkProbe[{0}]->CloseConnection", fmt::ptr(this));
            m_Driver->Close();
        }
        return true;
    }

    bool JLink_Probe::IsConnectionOpen() const {
        return m_Driver->IsOpen();
    }

    const std::string& JLink_Probe::GetModelName() const {
        return m_ModelName;
    }

    const std::string& JLink_Probe::GetSerialNumberString() const {
        return m_SerialNumberString;
    }

    std::shared_ptr<Driver::JLink_Driver> JLink_Probe::GetDriver() const {
        return m_Driver;
    }

    uint32_t JLink_Probe::GetRawSerialNumber() const {
        return m_RawSerialNumber;
    }

    // Target

    TargetStatus JLink_Probe::GetTargetStatus() const {
        HardwareStatus hwStatus;

        m_Driver->Connect();
        m_Driver->GetHWStatus(&hwStatus);

        HWDLOG_PROBE_TRACE("VCC: {0}", hwStatus.targetVoltage);

        return TargetStatus::CONNECTION_ERROR;
    }

    //

} // namespace HWD::Probe
