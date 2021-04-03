// [source]
#pragma once

#include "JLink_Probe.hpp"

namespace HWD::Probe {

    /////////////////////////////////////////////////////////
    using namespace Driver::JLink_Types;
    /////////////////////////////////////////////////////////
    std::array<JLink_Probe*, JLink_Probe::MAX_DISCOVERABLE_PROBE_COUNT> JLink_Probe::s_Probes = {};

    struct ProbeCallbackEntry {
        JLink_Probe::MessageCallback log;
        JLink_Probe::MessageCallback warning;
        JLink_Probe::MessageCallback error;
        JLink_Probe::FlashProgressCallback flashProgress;
    };
    static std::array<ProbeCallbackEntry, JLink_Probe::MAX_DISCOVERABLE_PROBE_COUNT> s_ProbeCallbackEntries;
    static bool s_ProbeCallbackArrayInitialized = false;

#define DEF_LIBRARY_CALLBACK_ENTRY(index)                                                                    \
    s_ProbeCallbackEntries[index].log = [](const char* str) {                                                \
        if (s_Probes[index])                                                                                 \
            s_Probes[index]->Probe_LogCallback(str);                                                         \
    };                                                                                                       \
    s_ProbeCallbackEntries[index].warning = [](const char* str) {                                            \
        if (s_Probes[index])                                                                                 \
            s_Probes[index]->Probe_WarningCallback(str);                                                     \
    };                                                                                                       \
    s_ProbeCallbackEntries[index].error = [](const char* str) {                                              \
        if (s_Probes[index])                                                                                 \
            s_Probes[index]->Probe_ErrorCallback(str);                                                       \
    };                                                                                                       \
    s_ProbeCallbackEntries[index].flashProgress = [](const char* action, const char* prog, int percentage) { \
        if (s_Probes[index])                                                                                 \
            s_Probes[index]->Probe_FlashProgressCallback(action, prog, percentage);                          \
    };

    void JLink_Probe::s_InitializeProbeCallbackArray() {
        // Max probe count is set to 8
        DEF_LIBRARY_CALLBACK_ENTRY(0);
        DEF_LIBRARY_CALLBACK_ENTRY(1);
        DEF_LIBRARY_CALLBACK_ENTRY(2);
        DEF_LIBRARY_CALLBACK_ENTRY(3);
        DEF_LIBRARY_CALLBACK_ENTRY(4);
        DEF_LIBRARY_CALLBACK_ENTRY(5);
        DEF_LIBRARY_CALLBACK_ENTRY(6);
        DEF_LIBRARY_CALLBACK_ENTRY(7);
    }
    /////////////////////////////////////////////////////////

    JLink_Probe::JLink_Probe(const Driver::JLink_Types::ProbeInfo& probeInfo, int probeCallbackIndex) {
        HWDLOG_PROBE_TRACE("[JLink_Probe@{0}] constructor", fmt::ptr(this));

        if (!s_ProbeCallbackArrayInitialized) {
            s_ProbeCallbackArrayInitialized = true;
            s_InitializeProbeCallbackArray();
        }

        m_Driver     = std::make_shared<Driver::JLink_Driver>();
        m_ProbeIndex = probeCallbackIndex;

        m_ModelName          = probeInfo.modelName;
        m_SerialNumberString = std::to_string(probeInfo.serialNumber);
        m_RawSerialNumber    = probeInfo.serialNumber;

        if (!m_Driver->IsLoaded()) {
            HWDLOG_PROBE_CRITICAL("[JLink_Probe@{0}] driver library not loaded", fmt::ptr(this));
        }
    }

    JLink_Probe::~JLink_Probe() {
        HWDLOG_PROBE_TRACE("[JLink_Probe@{0}] destructor", fmt::ptr(this));

        if (Probe_IsConnected())
            Probe_Disconnect();
        s_Probes[m_ProbeIndex] = nullptr;
    }

    //////////////////////////////////////////////////////////////////////

    bool JLink_Probe::Probe_IsReady() const {
        return m_Driver->IsLoaded();
    }

    /////////////////////////////////////////////////////////////
    // Library

    void JLink_Probe::Probe_LogCallback(const char* message) {
        //HWDLOG_PROBE_TRACE("[JLink_Probe@{0}][LOG] {1}", fmt::ptr(this), message ? message : "-");
    }

    void JLink_Probe::Probe_WarningCallback(const char* message) {
        HWDLOG_PROBE_WARN("[JLink_Probe@{0}][WARN] >> {1}", fmt::ptr(this), message ? message : "-");
    }

    void JLink_Probe::Probe_ErrorCallback(const char* message) {
        HWDLOG_PROBE_ERROR("[JLink_Probe@{0}][ERROR] >> {1}", fmt::ptr(this), message ? message : "-");
    }

    void JLink_Probe::Probe_FlashProgressCallback(const char* action, const char* prog, int percentage) {
        HWDLOG_PROBE_INFO("[JLink_Probe@{0}][PROG] >> {1} {2} {3}%", fmt::ptr(this), action ? action : "", prog ? prog : "", percentage);
    }

    /////////////////////////////////////////////////////////////
    // Probe

    void JLink_Probe::Probe_DisableFlashProgressPopup() {
        char charBuf[256];
        auto ret =
            m_Driver->probe_ExecuteCommand(Driver::JLink_Types::Commands::DISABLE_FLASH_PROGRESS_POPUP, charBuf, sizeof(charBuf) - 1);

        if (ret != ErrorCode::OK)
            HWDLOG_PROBE_WARN("[JLink_Probe@{0}] failed to disable internal flash progress popup: {1}", fmt::ptr(this), charBuf);
    }

    bool JLink_Probe::Probe_Connect() {
        if (Probe_IsConnected()) {
            HWDLOG_PROBE_WARN("[JLink_Probe@{0}] not connecting to probe - already connected", fmt::ptr(this));
            return true;
        }

        const char* openStatus =
            m_Driver->probe_ConnectEx(s_ProbeCallbackEntries[m_ProbeIndex].log, s_ProbeCallbackEntries[m_ProbeIndex].error);

        if (openStatus) {
            HWDLOG_PROBE_ERROR("[JLink_Probe@{0}] failed to connect to probe - {1}", fmt::ptr(this), openStatus);
            return false;
        }

        m_Driver->probe_SetWarningCallback(s_ProbeCallbackEntries[m_ProbeIndex].warning);

        Probe_DisableFlashProgressPopup();
        m_Driver->probe_SetFlashProgProgressCallback(s_ProbeCallbackEntries[m_ProbeIndex].flashProgress);

        // temporary
        m_Driver->target_SetInterfaceSpeed(50000);

        return true;
    }
    bool JLink_Probe::Probe_Disconnect() {
        if (Probe_IsConnected()) {
            HWDLOG_PROBE_TRACE("[JLink_Probe@{0}] disconnect from probe", fmt::ptr(this));

            // need to check if nullptr is a valid no callback value
            m_Driver->probe_SetLogCallback([](const char*) {
            });
            m_Driver->probe_SetErrorCallback([](const char*) {
            });
            m_Driver->probe_SetWarningCallback([](const char*) {
            });

            m_Driver->probe_Disconnect();
        } else {
            HWDLOG_PROBE_WARN("[JLink_Probe@{0}] not disconnecting - already disconnected", fmt::ptr(this));
        }
        return true;
    }

    bool JLink_Probe::Probe_IsConnected() const {
        return m_Driver->probe_IsConnectionOpen();
    }

    ////////////////////////////////////////////////////////////////////
    // Target

    // TODO: Implement error reason callback for higher level callers
    bool JLink_Probe::Target_SelectDevice(const TargetDeviceDescription& device) {
        char resp[256];
        ErrorCode ec = m_Driver->probe_ExecuteCommand(fmt::format("Device = {0}\n", device.name).c_str(), resp, 256);

        if (ec == ErrorCode::OK) {
            HWDLOG_PROBE_TRACE("[JLink_Probe@{0}] selected target device \"{1}\"", fmt::ptr(this), device.name);
            return true;
        } else {
            HWDLOG_PROBE_ERROR(
                "[JLink_Probe@{0}] failed to select target device \"{1}\" - {2}", fmt::ptr(this), device.name, ErrorCodeToString(ec));
            return false;
        }
    }

    // TODO: Implement error reason callback for higher level callers
    bool JLink_Probe::Target_SelectDebugInterface(DebugInterface interface) {
        switch (interface) {
            case DebugInterface::SWD: {
                TargetInterfaceMask supportedInterfaces;
                m_Driver->target_GetAvailableInterfaces(&supportedInterfaces);

                if (supportedInterfaces & TargetInterfaceMask::SWD) {
                    m_Driver->target_SelectInterface(TargetInterface::SWD);
                    HWDLOG_PROBE_TRACE("[JLink_Probe@{0}] selected SWD target interface", fmt::ptr(this));
                    return true;
                } else {
                    HWDLOG_PROBE_ERROR("[JLink_Probe@{0}] failed to select target interface \"SWD\" - not supported", fmt::ptr(this));
                    return false;
                }
            }
            default:
                HWDLOG_PROBE_ERROR("[JLink_Probe@{0}] failed to select target interface \"{1}\" - unknown interface",
                                   fmt::ptr(this),
                                   IProbe::TargetInterfaceToString(interface));
                return false;
        }
    }

    bool JLink_Probe::Target_IsConnected() const {
        return m_Driver->target_IsConnected();
    }

    // TODO: Implement error reason callback for higher level callers
    bool JLink_Probe::Target_Connect() {
        ErrorCode ec = m_Driver->target_Connect();

        if (ec == ErrorCode::OK) {
            HWDLOG_PROBE_TRACE("[JLink_Probe@{0}] connected to target", fmt::ptr(this));
            return true;
        } else {
            if ((int)ec == -1) { // Unspecified error
                HWDLOG_PROBE_ERROR("[JLink_Probe@{0}] could not connect to target - unspecified error", fmt::ptr(this));
            } else { // ErrorCode
                HWDLOG_PROBE_ERROR("[JLink_Probe@{0}] could not connect to target - {1}", fmt::ptr(this), ErrorCodeToString(ec));
            }

            return false;
        }
    }

    // TODO: Implement error reason callback for higher level callers
    bool JLink_Probe::Target_Erase() {
        ErrorCode ec = m_Driver->target_Erase();

        if (ec == ErrorCode::OK) {
            HWDLOG_PROBE_TRACE("[JLink_Probe@{0}] target program erased", fmt::ptr(this));
            return true;
        } else {
            HWDLOG_PROBE_ERROR("[JLink_Probe@{0}] failed to erase target program - {1}", fmt::ptr(this), ErrorCodeToString(ec));
            return false;
        }
    }

    // TODO: Implement error reason callback for higher level callers
    bool JLink_Probe::Target_WriteProgram(const uint8_t* data, uint32_t size) {
        m_Driver->target_BeginDownload(DownloadFlags::ALLOW_FLASH | DownloadFlags::ALLOW_BUFFERED_RAM);

        ErrorCode ret = m_Driver->target_WriteProgram(0, size, (void*)data);
        if ((int)ret >= 0) {
            ret = m_Driver->target_EndDownload();

            if ((int)ret >= 0 || (int)ret == -2) { // return code -2 is error code for target program matching requested program
                HWDLOG_PROBE_TRACE("[JLink_Probe@{0}] target programmed", fmt::ptr(this));
                return true;
            } else {
                const char* reason;

                switch ((int)ret) {
                    case -3: reason = "program/erase failed";
                    case -4: reason = "verification failed";
                    default: reason = "error";
                }

                HWDLOG_PROBE_ERROR("[JLink_Probe@{0}] failed to program target - {1}", fmt::ptr(this), reason);
                return false;
            }
        } else {
            HWDLOG_PROBE_ERROR("[JLink_Probe@{0}] failed to program target - {1}", fmt::ptr(this), ErrorCodeToString(ret));
            return false;
        }
    }

    // TODO: Implement error reason callback for higher level callers
    bool JLink_Probe::Target_Reset(bool haltAfterReset) {
        ErrorCode ec;

        if (haltAfterReset) {
            if (m_Driver->probe_GetCapabilities() & ProbeCapabilities::RESET_STOP_TIMED) {
                ec = m_Driver->target_Reset();
                // TODO: finish this
            } else {
                HWDLOG_PROBE_WARN("[JLink_Probe@{0}] immediate reset and halt not supported - resetting and halting seperately",
                                  fmt::ptr(this));
                m_Driver->target_ResetAndRun();
                m_Driver->target_Run();
            }
        } else {
            m_Driver->target_ResetAndRun();
        }

        return true;
    }

    // TODO: Implement error reason callback for higher level callers
    bool JLink_Probe::Target_StartTerminal(void* params) {
        if (!Probe_IsConnected()) {
            HWDLOG_PROBE_ERROR("[JLink_Probe@{0}] failed to start terminal - probe not connected", fmt::ptr(this));
            return false;
        }

        if (params) {
            HWDLOG_PROBE_ERROR("[JLink_Probe@{0}] start terminal with params not implemented", fmt::ptr(this));
            return false;
        } else {
            auto* terminalThread = new std::thread([=]() {
                ErrorCode ec = m_Driver->target_RTT_Control(Driver::JLink_Types::RTT_Command::START, nullptr);
                if (ec == ErrorCode::OK) {
                    HWDLOG_PROBE_TRACE("[JLink_Probe@{0}] terminal started", fmt::ptr(this));

                    char str[1024];
                    while (1 < 2) {
                        int bytesRead = m_Driver->target_RTT_Read(0, str, 1024);
                        if (bytesRead > 0) {
                            str[bytesRead] = 0;
                            std::cout << str;
                        }
                    }
                } else {
                    HWDLOG_PROBE_ERROR("[JLink_Probe@{0}] failed to start terminal - {1}", fmt::ptr(this), ErrorCodeToString(ec));
                }
            });
            terminalThread->detach();
            return true;
        }
    }

} // namespace HWD::Probe
