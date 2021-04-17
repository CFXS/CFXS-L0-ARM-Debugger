// [source]
#pragma once

#include "JLink.hpp"

namespace HWD::Probe {

    ///////////////////////////////////////////////////////////////////////////////////////////////
    static constexpr bool LOG_ENABLED       = false;
    static constexpr bool WARN_LOG_ENABLED  = true;
    static constexpr bool ERROR_LOG_ENABLED = true;
    ///////////////////////////////////////////////////////////////////////////////////////////////
    using namespace Driver::JLink_Types;
    ///////////////////////////////////////////////////////////////////////////////////////////////
    std::array<JLink*, JLink::MAX_DISCOVERABLE_PROBE_COUNT> JLink::s_Probes = {};

    struct ProbeCallbackEntry {
        JLink::MessageCallback log;
        JLink::MessageCallback warning;
        JLink::MessageCallback error;
        JLink::FlashProgressCallback flashProgress;
    };
    static std::array<ProbeCallbackEntry, JLink::MAX_DISCOVERABLE_PROBE_COUNT> s_ProbeCallbackEntries;
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

    void JLink::s_InitializeProbeCallbackArray() {
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
    ///////////////////////////////////////////////////////////////////////////////////////////////

    JLink::JLink(const Driver::JLink_Types::ProbeInfo& probeInfo, int probeCallbackIndex) {
        HWDLOG_PROBE_TRACE("[JLink@{0}] Constructor", fmt::ptr(this));

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
            HWDLOG_PROBE_CRITICAL("[JLink@{0}] Driver library not loaded", fmt::ptr(this));
        }
    }

    JLink::~JLink() {
        HWDLOG_PROBE_TRACE("[JLink@{0}] Destructor", fmt::ptr(this));

        if (Probe_IsConnected())
            Probe_Disconnect();
        s_Probes[m_ProbeIndex] = nullptr;
    }

    //////////////////////////////////////////////////////////////////////

    bool JLink::Probe_IsReady() const {
        return m_Driver->IsLoaded();
    }

    /////////////////////////////////////////////////////////////
    // Library

    void JLink::Probe_LogCallback(const char* message) {
        if constexpr (LOG_ENABLED) {
            HWDLOG_PROBE_TRACE("[JLink@{0}][LOG] {1}", fmt::ptr(this), message ? message : "-");
        }
    }

    void JLink::Probe_WarningCallback(const char* message) {
        if constexpr (WARN_LOG_ENABLED) {
            HWDLOG_PROBE_WARN("[JLink@{0}][WARN] >> {1}", fmt::ptr(this), message ? message : "-");
        }
    }

    void JLink::Probe_ErrorCallback(const char* message) {
        if constexpr (ERROR_LOG_ENABLED) {
            HWDLOG_PROBE_ERROR("[JLink@{0}][ERROR] >> {1}", fmt::ptr(this), message ? message : "-");
        }
    }

    void JLink::Probe_FlashProgressCallback(const char* action, const char* prog, int percentage) {
        //HWDLOG_PROBE_INFO("[JLink@{0}][PROG] >> {1} {2} {3}%", fmt::ptr(this), action ? action : "", prog ? prog : "", percentage);
        m_FlashProgress = percentage / 100.0f;
    }

    /////////////////////////////////////////////////////////////
    // Probe

    void JLink::Probe_DisableFlashProgressPopup() {
        char charBuf[256];
        auto ret =
            m_Driver->probe_ExecuteCommand(Driver::JLink_Types::Commands::DISABLE_FLASH_PROGRESS_POPUP, charBuf, sizeof(charBuf) - 1);

        if (ret != ErrorCode::OK)
            HWDLOG_PROBE_WARN("[JLink@{0}] Failed to disable internal flash progress popup: {1}", fmt::ptr(this), charBuf);
    }

    bool JLink::Probe_Connect() {
        if (Probe_IsConnected()) {
            HWDLOG_PROBE_WARN("[JLink@{0}] Not connecting to probe - already connected", fmt::ptr(this));
            return true;
        }

        ErrorCode ec = m_Driver->probe_SelectBySerialNumber_USB(GetRawSerialNumber());
        if (ec != ErrorCode::OK) {
            HWDLOG_PROBE_ERROR("[JLink@{0}] Failed to select probe by serial number - {1}", fmt::ptr(this), ErrorCodeToString(ec));
            return false;
        }

        const char* openStatus =
            m_Driver->probe_ConnectEx(s_ProbeCallbackEntries[m_ProbeIndex].log, s_ProbeCallbackEntries[m_ProbeIndex].error);

        if (openStatus) {
            HWDLOG_PROBE_ERROR("[JLink@{0}] Failed to connect to probe - {1}", fmt::ptr(this), openStatus);
            return false;
        }

        m_Driver->probe_SetWarningCallback(s_ProbeCallbackEntries[m_ProbeIndex].warning);

        Probe_DisableFlashProgressPopup();
        m_Driver->probe_SetFlashProgProgressCallback(s_ProbeCallbackEntries[m_ProbeIndex].flashProgress);

        UpdateProbeInfo();

        // temporary
        m_Driver->target_SetInterfaceSpeed(50000);

        return true;
    }

    bool JLink::Probe_Disconnect() {
        if (Probe_IsConnected()) {
            HWDLOG_PROBE_TRACE("[JLink@{0}] Disconnect from probe", fmt::ptr(this));

            // need to check if nullptr is a valid no callback value
            m_Driver->probe_SetLogCallback([](const char*) {
            });
            m_Driver->probe_SetErrorCallback([](const char*) {
            });
            m_Driver->probe_SetWarningCallback([](const char*) {
            });

            m_Driver->probe_Disconnect();
        } else {
            HWDLOG_PROBE_WARN("[JLink@{0}] Not disconnecting - already disconnected", fmt::ptr(this));
        }
        return true;
    }

    bool JLink::Probe_IsConnected() const {
        return m_Driver->probe_IsConnectionOpen();
    }

    ////////////////////////////////////////////////////////////////////
    // Target

    // TODO: Implement error reason callback for higher level callers
    bool JLink::Target_SelectDevice(const TargetDeviceDescription& device) {
        char resp[256];
        ErrorCode ec = m_Driver->probe_ExecuteCommand(fmt::format("Device = {0}\n", device.GetName()).c_str(), resp, 256);

        if (ec == ErrorCode::OK) {
            HWDLOG_PROBE_TRACE("[JLink@{0}] Selected target device \"{1}\"", fmt::ptr(this), device.GetName());
            HWDLOG_PROBE_TRACE("[JLink@{0}] Target memory regions:", fmt::ptr(this));
            for (auto& reg : device.GetMemoryRegions()) {
                HWDLOG_PROBE_TRACE("[JLink@{0}] - {1}\t{2:6}\t0x{3:08X} - 0x{4:08X}",
                                   fmt::ptr(this),
                                   To_C_String(reg.GetAccessPermissions()),
                                   reg.GetName(),
                                   reg.GetAddress(),
                                   reg.GetAddress() + reg.GetSize());
            }
            return true;
        } else {
            HWDLOG_PROBE_ERROR(
                "[JLink@{0}] Failed to select target device \"{1}\" - {2}", fmt::ptr(this), device.GetName(), ErrorCodeToString(ec));
            return false;
        }
    }

    // TODO: Implement error reason callback for higher level callers
    bool JLink::Target_SelectDebugInterface(DebugInterface interface) {
        switch (interface) {
            case DebugInterface::SWD: {
                TargetInterfaceMask supportedInterfaces;
                m_Driver->target_GetAvailableInterfaces(&supportedInterfaces);

                if (supportedInterfaces & TargetInterfaceMask::SWD) {
                    m_Driver->target_SelectInterface(TargetInterface::SWD);
                    HWDLOG_PROBE_TRACE("[JLink@{0}] Selected SWD target interface", fmt::ptr(this));
                    return true;
                } else {
                    HWDLOG_PROBE_ERROR("[JLink@{0}] Failed to select target interface \"SWD\" - not supported", fmt::ptr(this));
                    return false;
                }
            }
            default:
                HWDLOG_PROBE_ERROR("[JLink@{0}] Failed to select target interface \"{1}\" - unknown interface",
                                   fmt::ptr(this),
                                   IProbe::TargetInterfaceToString(interface));
                return false;
        }
    }

    bool JLink::Target_IsConnected() const {
        return m_Driver->target_IsConnected();
    }

    // TODO: Implement error reason callback for higher level callers
    bool JLink::Target_Connect() {
        ErrorCode ec = m_Driver->target_Connect();

        if (ec == ErrorCode::OK) {
            HWDLOG_PROBE_TRACE("[JLink@{0}] Connected to target", fmt::ptr(this));
            PrepareTarget();
            return true;
        } else {
            if ((int)ec == -1) { // Unspecified error
                HWDLOG_PROBE_ERROR("[JLink@{0}] Could not connect to target - unspecified error", fmt::ptr(this));
            } else { // ErrorCode
                HWDLOG_PROBE_ERROR("[JLink@{0}] Could not connect to target - {1}", fmt::ptr(this), ErrorCodeToString(ec));
            }

            return false;
        }
    }

    // TODO: Implement error reason callback for higher level callers
    bool JLink::Target_Erase() {
        ErrorCode ec = m_Driver->target_Erase();

        if (ec == ErrorCode::OK) {
            HWDLOG_PROBE_TRACE("[JLink@{0}] Target program erased", fmt::ptr(this));
            return true;
        } else {
            HWDLOG_PROBE_ERROR("[JLink@{0}] Failed to erase target program - {1}", fmt::ptr(this), ErrorCodeToString(ec));
            return false;
        }
    }

    // TODO: Implement error reason callback for higher level callers
    bool JLink::Target_WriteProgram(const uint8_t* data, uint32_t size) {
        m_Driver->target_BeginDownload(DownloadFlags::ALLOW_FLASH | DownloadFlags::ALLOW_BUFFERED_RAM);

        ErrorCode ret = m_Driver->target_WriteProgram(0, size, (void*)data);
        if ((int)ret >= 0) {
            ret = m_Driver->target_EndDownload();

            if ((int)ret >= 0 || (int)ret == -2) { // return code -2 is error code for target program matching requested program
                HWDLOG_PROBE_TRACE("[JLink@{0}] Target programmed", fmt::ptr(this));
                return true;
            } else {
                const char* reason;

                switch ((int)ret) {
                    case -3: reason = "program/erase failed";
                    case -4: reason = "verification failed";
                    default: reason = "error";
                }

                HWDLOG_PROBE_ERROR("[JLink@{0}] Failed to program target - {1}", fmt::ptr(this), reason);
                return false;
            }
        } else {
            HWDLOG_PROBE_ERROR("[JLink@{0}] Failed to program target - {1}", fmt::ptr(this), ErrorCodeToString(ret));
            return false;
        }
    }

    // TODO: Implement error reason callback for higher level callers
    bool JLink::Target_Reset(bool haltAfterReset) {
        ErrorCode ec;

        if (haltAfterReset) {
            if (m_ProbeCapabilities & ProbeCapabilities::RESET_STOP_TIMED) { // can halt immediately after reset
                ec = m_Driver->target_Reset();
                if (ec != ErrorCode::OK) {
                    HWDLOG_PROBE_ERROR("[JLink@{0}] Failed to reset target - {1}", fmt::ptr(this), ErrorCodeToString(ec));
                }
            } else {
                HWDLOG_PROBE_WARN("[JLink@{0}] Immediate reset and halt not supported - resetting and halting seperately", fmt::ptr(this));
                m_Driver->target_ResetAndRun();
                m_Driver->target_Halt();
            }
        } else {
            m_Driver->target_ResetAndRun();
        }

        return true;
    }

    bool JLink::Target_StartTerminal(void* params) {
        // TODO: Implement error reason callback for higher level callers
        if (!Probe_IsConnected()) {
            HWDLOG_PROBE_ERROR("[JLink@{0}] Failed to start terminal - probe not connected", fmt::ptr(this));
            m_TerminalEnabled = false;
            return false;
        }

        if (params) {
            HWDLOG_PROBE_ERROR("[JLink@{0}] Start terminal with params not implemented", fmt::ptr(this));
            m_TerminalEnabled = false;
            return false;
        } else {
            ErrorCode ec = m_Driver->target_RTT_Control(Driver::JLink_Types::RTT_Command::START, nullptr);
            if (ec == ErrorCode::OK) {
                HWDLOG_PROBE_TRACE("[JLink@{0}] Terminal started", fmt::ptr(this));
                m_TerminalEnabled = true;
                m_TerminalBuffer.reserve(1024 * 1024 * 16);
                return true;
            } else {
                HWDLOG_PROBE_ERROR("[JLink@{0}] Failed to start terminal - {1}", fmt::ptr(this), ErrorCodeToString(ec));

                m_TerminalEnabled = false;
                return true;
            }
        }
    }

    bool JLink::Target_ReadMemory_32(uint32_t address, uint32_t* readTo) {
        return m_Driver->target_ReadMemory_32(address, 1, readTo, nullptr) == 1;
    }

    bool JLink::Target_ReadMemory_64(uint32_t address, uint64_t* readTo) {
        return m_Driver->target_ReadMemory_64(address, 1, readTo, nullptr) == 1;
    }

    bool JLink::Target_WriteMemory_32(uint32_t address, uint32_t value) {
        return m_Driver->target_WriteMemory_32(address, value) == 0;
    }

    bool JLink::Target_Halt() {
        return m_Driver->target_Halt();
    }
    bool JLink::Target_Run() {
        m_Driver->target_Run();
        return true;
    }
    bool JLink::Target_IsRunning() {
        return m_Driver->target_IsHalted();
    }

    const char* JLink::Target_GetTerminalBuffer() {
        return m_TerminalBuffer.data();
    }

    float JLink::Target_GetFlashProgress() {
        return m_FlashProgress;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // Process
    void JLink::Process() {
        if (m_TerminalEnabled) {
            char str[1024];
            int bytesRead = m_Driver->target_RTT_Read(0, str, sizeof(str) - 2);
            for (int i = 0; i < bytesRead; i++) {
                m_TerminalBuffer.push_back(str[i]);
            }
        }
    }

} // namespace HWD::Probe
