// [source]
#pragma once

#include "JLink_Probe.hpp"
#include "TestFirmware.h"

namespace HWD::Probe {

    /////////////////////////////////////////////////////////
    using namespace Driver::JLink_Types;
    /////////////////////////////////////////////////////////

    JLink_Probe::JLink_Probe(const Driver::JLink_Types::ProbeInfo& probeInfo) {
        HWDLOG_PROBE_TRACE("JLink_Probe::JLink_Probe() [{0}]", fmt::ptr(this));

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

    bool JLink_Probe::OpenConnection() {
        if (IsConnectionOpen()) {
            HWDLOG_PROBE_WARN("JLinkProbe[{0}]->OpenConnection connection already open", fmt::ptr(this));
            return false;
        }

        const char* openStatus = m_Driver->Open();

        if (openStatus)
            HWDLOG_PROBE_ERROR("JLinkProbe[{0}]->OpenConnection error: {1}", fmt::ptr(this), openStatus);
        else
            HWDLOG_PROBE_TRACE("JLinkProbe[{0}]->OpenConnection connected to probe", fmt::ptr(this));

        m_Driver->SetSpeed(50000);

        auto ret = m_Driver->ExecCommand("Device = TM4C129ENCPDT", nullptr, 0);
        HWDLOG_PROBE_TRACE("Device select return: {0}", ret);

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

        // nullptr = no error
        return openStatus == nullptr;
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
