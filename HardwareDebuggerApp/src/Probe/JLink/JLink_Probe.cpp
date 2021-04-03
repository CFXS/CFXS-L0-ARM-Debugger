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
        auto ret =
            m_Driver->probe_ExecuteCommand(Driver::JLink_Types::Commands::DISABLE_FLASH_PROGRESS_POPUP, charBuf, sizeof(charBuf) - 1);
        if (ret)
            HWDLOG_PROBE_WARN("JLink_Probe[{0}] failed to disable internal flash progress popup: {1}", fmt::ptr(this), charBuf);
    }

    bool JLink_Probe::OpenConnection() {
        if (IsConnectionOpen()) {
            HWDLOG_PROBE_WARN("JLinkProbe[{0}]->OpenConnection connection already open", fmt::ptr(this));
            return false;
        }

        const char* openStatus = m_Driver->session_Open();

        if (openStatus) {
            HWDLOG_PROBE_ERROR("JLinkProbe[{0}]->OpenConnection error: {1}", fmt::ptr(this), openStatus);
            return false;
        }

        DisableFlashProgressPopup();

        HWDLOG_PROBE_TRACE("JLinkProbe[{0}]->OpenConnection connected to probe", fmt::ptr(this));

        char resp[1024] = {0};

        m_Driver->probe_SetErrorCallback([](const char* str) {
            HWDLOG_PROBE_ERROR("JLink Error: {0}", str ? str : "Unknown");
        });

        m_Driver->probe_SetWarningCallback([](const char* str) {
            HWDLOG_PROBE_WARN("JLink Warning: {0}", str ? str : "Unknown");
        });

        m_Driver->probe_SetFlashProgProgressCallback([](const char* action, const char* prog, int percentage) {
            HWDLOG_PROBE_ERROR("JLink flash progress {0} - {1}: {2}%", action ? action : "-", prog ? prog : "-", percentage);
        });

        m_Driver->target_SetInterfaceSpeed(50000);

        resp[0]  = 0;
        auto ret = m_Driver->probe_ExecuteCommand("Device = TM4C129ENCPDT\n", resp, 1024);
        HWDLOG_PROBE_TRACE("Device select return: {0} - resp: {1}", ret, resp);

        ret = m_Driver->target_SelectInterface(TargetInterface::SWD);
        HWDLOG_PROBE_TRACE("SWD select return: {0}", ret);

        auto DecodeCore = [](auto core) -> const char* {
            switch ((uint32_t)core) {
                case 0x00000000: return "None";
                case 0xFFFFFFFF: return "Any";
                case 0x010000FF: return "Cortex-M1";
                case 0x02FFFFFF: return "ColdFire";
                case 0x030000FF: return "Cortex-M3";
                case 0x03000010: return "Cortex-M3_R1P";
                case 0x03000011: return "Cortex-M3_R1P";
                case 0x03000020: return "Cortex-M3_R2P";
                case 0x03000021: return "Cortex-M3_R2P";
                case 0x04FFFFFF: return "Simulator";
                case 0x05FFFFFF: return "XScale";
                case 0x060000FF: return "Cortex-M0";
                case 0x060100FF: return "Cortex-M23";
                case 0x07FFFFFF: return "ARM7";
                case 0x070000FF: return "ARM7TDMI";
                case 0x0700003F: return "ARM7TDMI_R3";
                case 0x0700004F: return "ARM7TDMI_R4";
                case 0x070001FF: return "ARM7TDMI_S";
                case 0x0700013F: return "ARM7TDMI_S_R3";
                case 0x0700014F: return "ARM7TDMI_S_R4";
                case 0x080000FF: return "Cortex-A8";
                case 0x080800FF: return "Cortex-A7";
                case 0x080900FF: return "Cortex-A9";
                case 0x080A00FF: return "Cortex-A12";
                case 0x080B00FF: return "Cortex-A15";
                case 0x080C00FF: return "Cortex-A17";
                case 0x09FFFFFF: return "ARM9";
                case 0x090001FF: return "ARM9TDMI_S";
                case 0x092000FF: return "ARM920T";
                case 0x092200FF: return "ARM922T";
                case 0x092601FF: return "ARM926EJ_S";
                case 0x094601FF: return "ARM946E_S";
                case 0x096601FF: return "ARM966E_S";
                case 0x096801FF: return "ARM968E_S";
                case 0x0BFFFFFF: return "ARM11";
                case 0x0B36FFFF: return "ARM1136";
                case 0x0B3602FF: return "ARM1136J";
                case 0x0B3603FF: return "ARM1136J_S";
                case 0x0B3606FF: return "ARM1136JF";
                case 0x0B3607FF: return "ARM1136JF_S";
                case 0x0B56FFFF: return "ARM1156";
                case 0x0B76FFFF: return "ARM1176";
                case 0x0B7602FF: return "ARM1176J";
                case 0x0B7603FF: return "ARM1176J_S";
                case 0x0B7606FF: return "ARM1176JF";
                case 0x0B7607FF: return "ARM1176JF_S";
                case 0x0C0000FF: return "Cortex-R4";
                case 0x0C0100FF: return "Cortex-R5";
                case 0x0C0200FF: return "Cortex-R8";
                case 0x0E0000FF: return "Cortex-M4";
                case 0x0E0100FF: return "Cortex-M7";
                case 0x0E0200FF: return "Cortex-M33";
                case 0x0F0000FF: return "Cortex-A5";
            }
        };

        HWDLOG_PROBE_CRITICAL("Detected core: {0}", DecodeCore(m_Driver->target_GetCore()));

        ret = m_Driver->target_Reset();
        HWDLOG_PROBE_TRACE("Reset return: {0}", ret);

        ret = m_Driver->target_Erase();
        HWDLOG_PROBE_TRACE("EraseChip return: {0}", ret);

        ret = m_Driver->target_Reset();
        HWDLOG_PROBE_TRACE("Reset return: {0}", ret);

        m_Driver->target_BeginDownload(DownloadFlags::ALLOW_FLASH);
        HWDLOG_PROBE_TRACE("BeginDownload");

        m_Driver->target_WriteMemory(0, sizeof(firmware), (void*)firmware);
        HWDLOG_PROBE_TRACE("WriteMem return: {0}", ret);

        ret = m_Driver->target_EndDownload();
        HWDLOG_PROBE_TRACE("EndDownload return: {0}", ret);

        ret = m_Driver->target_Reset();
        HWDLOG_PROBE_TRACE("Reset return: {0}", ret);

        // Delayed Go
        new std::thread([=]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            m_Driver->target_Go();
            while (1 < 2) {
            }
        });

        // RTT
        std::thread t([=]() {
            auto ret = m_Driver->target_RTT_Control(Driver::JLink_Types::RTT_Command::START, nullptr);
            HWDLOG_PROBE_TRACE("RTT Start return: {0}", ret);
            char str[1024];
            while (1 < 2) {
                int bytesRead = m_Driver->target_RTT_Read(0, str, 1024);
                if (bytesRead > 0) {
                    str[bytesRead] = 0;
                    std::cout << str;
                }
            }
        });
        t.join();

        return true;
    }

    bool JLink_Probe::CloseConnection() {
        if (IsConnectionOpen()) {
            HWDLOG_PROBE_WARN("JLinkProbe[{0}]->CloseConnection", fmt::ptr(this));
            m_Driver->session_Close();
        }
        return true;
    }

    bool JLink_Probe::IsConnectionOpen() const {
        return m_Driver->session_IsOpen();
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

        m_Driver->target_Connect();
        m_Driver->target_GetHardwareStatus(&hwStatus);

        HWDLOG_PROBE_TRACE("VCC: {0}", hwStatus.targetVoltage);

        return TargetStatus::CONNECTION_ERROR;
    }

    //

} // namespace HWD::Probe
