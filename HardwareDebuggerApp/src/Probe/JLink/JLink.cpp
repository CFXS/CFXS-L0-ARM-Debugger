// [source]
#pragma once

#include "JLink.hpp"

#include <set>

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
            HWDLOG_PROBE_CRITICAL("Driver library not loaded");
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
            HWDLOG_PROBE_ERROR("[JLink@{0}] Failed to select probe by serial number - {1}", fmt::ptr(this), ec);
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
    bool JLink::Target_SelectDevice(const Target::DeviceDescription& device) {
        char resp[256];
        ErrorCode ec = m_Driver->probe_ExecuteCommand(fmt::format("Device = {0}\n", device.GetName()).c_str(), resp, 256);

        HWDLOG_PROBE_TRACE("[JLink@{0}]:", fmt::ptr(this));
        if (ec == ErrorCode::OK) {
            HWDLOG_PROBE_TRACE("Selected target device \"{0}\"", device.GetName());
            HWDLOG_PROBE_TRACE("Target memory regions:");
            for (auto& reg : device.GetMemoryRegions()) {
                HWDLOG_PROBE_TRACE(" - {0}\t{1:6}\t0x{2:08X} - 0x{3:08X}",
                                   To_C_String(reg.GetAccessPermissions()),
                                   reg.GetName(),
                                   reg.GetAddress(),
                                   reg.GetAddress() + reg.GetSize());
            }
            return true;
        } else {
            HWDLOG_PROBE_ERROR("Failed to select target device \"{0}\" - {1}", device.GetName(), ec);
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
                HWDLOG_PROBE_ERROR("[JLink@{0}] Failed to select target interface \"{1}\" - unknown interface", fmt::ptr(this), interface);
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
                HWDLOG_PROBE_ERROR("[JLink@{0}] Could not connect to target - {1}", fmt::ptr(this), ec);
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
            HWDLOG_PROBE_ERROR("[JLink@{0}] Failed to erase target program - {1}", fmt::ptr(this), ec);
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
            HWDLOG_PROBE_ERROR("[JLink@{0}] Failed to program target - {1}", fmt::ptr(this), ret);
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
                    HWDLOG_PROBE_ERROR("[JLink@{0}] Failed to reset target - {1}", fmt::ptr(this), ec);
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
                HWDLOG_PROBE_ERROR("[JLink@{0}] Failed to start terminal - {1}", fmt::ptr(this), ec);

                m_TerminalEnabled = false;
                return true;
            }
        }
    }

    uint8_t JLink::Target_ReadMemory_8(uint32_t address, bool* success) {
        uint8_t tmp;

        if (success) {
            *success = m_Driver->target_ReadMemory_8(address, 1, &tmp, nullptr) == 1;
        } else {
            m_Driver->target_ReadMemory_8(address, 1, &tmp, nullptr);
        }

        return tmp;
    }

    uint16_t JLink::Target_ReadMemory_16(uint32_t address, bool* success) {
        uint16_t tmp;

        if (success) {
            *success = m_Driver->target_ReadMemory_16(address, 1, &tmp, nullptr) == 1;
        } else {
            m_Driver->target_ReadMemory_16(address, 1, &tmp, nullptr);
        }

        return tmp;
    }

    uint32_t JLink::Target_ReadMemory_32(uint32_t address, bool* success) {
        uint32_t tmp;

        if (success) {
            *success = m_Driver->target_ReadMemory_32(address, 1, &tmp, nullptr) == 1;
        } else {
            m_Driver->target_ReadMemory_32(address, 1, &tmp, nullptr);
        }

        return tmp;
    }

    uint64_t JLink::Target_ReadMemory_64(uint32_t address, bool* success) {
        uint64_t tmp;

        if (success) {
            *success = m_Driver->target_ReadMemory_64(address, 1, &tmp, nullptr) == 1;
        } else {
            m_Driver->target_ReadMemory_64(address, 1, &tmp, nullptr);
        }

        return tmp;
    }

    int JLink::Target_ReadMemoryTo(uint32_t address, void* to, uint32_t bytesToRead, AccessWidth accessWidth) {
        return m_Driver->target_ReadMemoryEx(address, bytesToRead, to, (int)accessWidth);
    }

    bool JLink::Target_WriteMemory_32(uint32_t address, uint32_t value) {
        return m_Driver->target_WriteMemory_32(address, value) == 0;
    }

    uint32_t JLink::Target_Get_ROM_Table_Address() {
        uint32_t rom_table_address;

        ErrorCode ec = m_Driver->target_GetDebugInfo(DebugInfoIndex::ROM_TABLE_ADDRESS, &rom_table_address); // ROM Table
        if (ec != ErrorCode::OK) {
            rom_table_address = 0;
            HWDLOG_PROBE_CRITICAL("Failed to get ROM table address - {0}", ec);
        }

        return rom_table_address;
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
        return m_TerminalBuffer.empty() ? "Waiting for data..." : m_TerminalBuffer.data();
    }

    float JLink::Target_GetFlashProgress() {
        return m_FlashProgress;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // Process

    ///////////////////////////////////////////////////////////////////
    struct {
        struct {
            uint64_t Overflow;
            uint64_t Timestamp;
            uint64_t Reserved;
            uint64_t SWIT;
        } Counters;
    } s_SWO_Stats;

    enum class SWO_PacketType : uint8_t {
        OVER_FLOW, // underscore because "OVERFLOW" is a Windows define somewhere
        TIMESTAMP,
        RESERVED,
        SWIT,
        SYNC,

        EVENT_COUNTER,   // ARMv7-M
        EXCEPTION_TRACE, // ARMv7-M
        PC_SAMPLE,       // ARMv7-M
        PC_SAMPLE_SLEEP, // ARMv7-M
    };
    ///////////////////////////////////////////////////////////////////
    static int s_Sync_ZeroCount         = 0;
    static bool s_Synced                = true;
    static bool s_DecodingPacket        = false;
    static bool s_LastPacketWasOverflow = false;
    static SWO_PacketType s_PacketTypeToDecode;

    ///////////////////////////////////////////////////////////////////
    // SWO Timestamp
    enum class SWO_Timestamp_Type : uint8_t {
        TIMESTAMP_EMITTED_SYNCHRONOUS_TO_ITM,
        TIMESTAMP_EMITTED_DELAYED_TO_ITM,
        PACKET_EMITTED_DELAYED,
        PACKET_AND_TIMESTAMP_EMITTED_DELAYED,
        OVERFLOW_ITM,
        RESERVED,
    };
    SWO_Timestamp_Type s_SWO_Current_Timestamp_Type;
    uint32_t s_SWO_Current_Timestamp_Value;
    static int s_SWO_Packet_Timestamp_ContinuationCount;
    static constexpr uint32_t MAX_SWO_TIMESTAMP_CONTINUATIONS = 4;
    ///////////////////////////////////////////////////////////////////
    // SWO SWIT
    uint32_t s_SWO_SWIT_Payload;
    uint8_t s_SWO_SWIT_PADDRDBG;
    uint8_t s_SWO_SWIT_PayloadLength;
    uint8_t s_SWO_SWIT_RemainingBytes;
    ///////////////////////////////////////////////////////////////////
    static void SWO_ResetDecodeState() {
        s_DecodingPacket = false;
    }

    static void SWO_ProcessPacket(SWO_PacketType type) {
        switch (type) {
            case SWO_PacketType::OVER_FLOW:
                s_SWO_Stats.Counters.Overflow++; //
                //HWDLOG_PROBE_TRACE("[SWO Packet] OVERFLOW");
                break;
            case SWO_PacketType::TIMESTAMP:
                s_SWO_Stats.Counters.Timestamp++; //
                //HWDLOG_PROBE_TRACE(
                //    "[SWO Packet] TIMESTAMP: {0} C = {1}", s_SWO_Current_Timestamp_Value, s_SWO_Packet_Timestamp_ContinuationCount);
                break;
            case SWO_PacketType::RESERVED:
                s_SWO_Stats.Counters.Reserved++; //
                HWDLOG_PROBE_TRACE("[SWO Packet] RESERVED");
                break;
            case SWO_PacketType::SWIT:
                s_SWO_Stats.Counters.SWIT++; //
                HWDLOG_PROBE_TRACE("[SWO Packet] SWIT {0:#X} [{1} bytes] - PADDRDBG {2:#X}",
                                   s_SWO_SWIT_Payload,
                                   s_SWO_SWIT_PayloadLength,
                                   s_SWO_SWIT_PADDRDBG);
                break;
        }

        SWO_ResetDecodeState();
    }

////////////////////////////////////////////////////////////////////////////////////
// Exception Trace
#define ARMv7M_EXCEPTION_TRACE_GET_EXCEPTION_NUMBER(x) (x & 0x1FF)
#define ARMv7M_EXCEPTION_TRACE_GET_FUNCTION(x)         ((x >> 12) & 0b11)
    uint16_t s_SWO_ARMv7M_ExceptionTraceData;
    uint8_t s_SWO_ARMv7M_ExceptionTraceData_ReadPos;
    // PC Sample
    uint32_t s_SWO_ARMv7M_ProgramCounter;
    uint8_t s_SWO_ARMv7M_ProgramCounter_ReadPos;
    bool s_SWO_ARMv7M_ProgramCounterSleep_PayloadRead;
    ////////////////////////////////////////////////////////////////////////////////////
    static void SWO_ARMv7M_InitializeDecode(SWO_PacketType type) {
        switch (type) {
            case SWO_PacketType::EXCEPTION_TRACE: {
                s_SWO_ARMv7M_ExceptionTraceData         = 0;
                s_SWO_ARMv7M_ExceptionTraceData_ReadPos = 0;
                break;
            }
            case SWO_PacketType::PC_SAMPLE: {
                s_SWO_ARMv7M_ProgramCounter         = 0;
                s_SWO_ARMv7M_ProgramCounter_ReadPos = 0;
                break;
            }
            case SWO_PacketType::PC_SAMPLE_SLEEP: {
                s_SWO_ARMv7M_ProgramCounterSleep_PayloadRead = false;
                break;
            }
        }
    }

    std::map<uint32_t, uint64_t> s_PC_Map;
    std::map<uint32_t, uint64_t> s_ExecMap;
    static void SWO_ARMv7M_ProcessData(SWO_PacketType type, uint8_t val) {
        switch (type) {
            case SWO_PacketType::EVENT_COUNTER: { // no payload
                HWDLOG_PROBE_TRACE("[SWO ARMv7-M Packet] EVENT_COUNTER {0:#X}", val);
                SWO_ResetDecodeState(); // packet processed
                break;
            }
            case SWO_PacketType::EXCEPTION_TRACE: { // 2 byte payload
                s_SWO_ARMv7M_ExceptionTraceData |= val << s_SWO_ARMv7M_ExceptionTraceData_ReadPos * 8;

                s_SWO_ARMv7M_ExceptionTraceData_ReadPos++;
                if (s_SWO_ARMv7M_ExceptionTraceData_ReadPos == 2) {
                    auto decodeFunc = [](uint8_t f) {
                        switch (f) {
                            case 0b01: return "Enter";
                            case 0b10: return "Exit";
                            case 0b11: return "Return to";
                            default: return "[Reserved]";
                        }
                    };

                    s_ExecMap[ARMv7M_EXCEPTION_TRACE_GET_EXCEPTION_NUMBER(s_SWO_ARMv7M_ExceptionTraceData)]++;

                    //auto exNum = ARMv7M_EXCEPTION_TRACE_GET_EXCEPTION_NUMBER(s_SWO_ARMv7M_ExceptionTraceData);
                    //char exDec[16];
                    //if (exNum == 0) {
                    //    snprintf(exDec, 16, "MainLoop");
                    //} else if (exNum == 15) {
                    //    snprintf(exDec, 16, "SysTick");
                    //} else {
                    //    snprintf(exDec, 16, "%u", exNum);
                    //}

                    //HWDLOG_PROBE_ERROR("[SWO ARMv7-M Packet] EXCEPTION_TRACE {1} {0}",
                    //                   exDec,
                    //                  decodeFunc(ARMv7M_EXCEPTION_TRACE_GET_FUNCTION(s_SWO_ARMv7M_ExceptionTraceData)));
                    SWO_ResetDecodeState(); // packet processed
                }
                break;
            }
            case SWO_PacketType::PC_SAMPLE: { // 4 byte payload
                s_SWO_ARMv7M_ProgramCounter |= val << s_SWO_ARMv7M_ProgramCounter_ReadPos * 8;

                s_SWO_ARMv7M_ProgramCounter_ReadPos++;
                if (s_SWO_ARMv7M_ProgramCounter_ReadPos == 4) {
                    //char pcs[16];
                    //snprintf(pcs, 16, "0x%08X", s_SWO_ARMv7M_ProgramCounter);
                    s_PC_Map[s_SWO_ARMv7M_ProgramCounter]++;
                    //HWDLOG_PROBE_WARN("[SWO ARMv7-M Packet] PC_SAMPLE PC = {0}", pcs);
                    SWO_ResetDecodeState(); // packet processed
                }
                break;
            }
            case SWO_PacketType::PC_SAMPLE_SLEEP: { // 1 byte payload (ignore)
                if (s_SWO_ARMv7M_ProgramCounterSleep_PayloadRead == false) {
                    s_SWO_ARMv7M_ProgramCounterSleep_PayloadRead = true;
                } else {
                    HWDLOG_PROBE_TRACE("[SWO ARMv7-M Packet] PC_SAMPLE_SLEEP");
                    SWO_ResetDecodeState(); // packet processed
                }
                break;
            }
            default: HWDLOG_PROBE_ERROR("UNKNOWN ARMv7 PACKET TYPE");
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////
    static bool s_StopProcess = false;
    static void SWO_Process(uint8_t* buf, uint32_t byteCount) {
        if (s_StopProcess)
            return;

        for (uint32_t b = 0; b < byteCount; b++) {
            auto val = buf[b];
            if (s_Synced) {
                if (!s_DecodingPacket) { /////////////////////// HEADER PROCESSING
                    // check headers
                    if (val == 0) {
                        s_PacketTypeToDecode = SWO_PacketType::SYNC;
                        s_DecodingPacket     = true;
                        s_Sync_ZeroCount     = 0;
                        continue;
                    }

                    // ARMv7-M trace packet decoding
                    // need to check if this is compatible with ARMv6 and ARMv8
                    if (1 < 2) {
                        if (val == 0b00010111) { // Periodic PC sample packets, discriminator ID2 [ARMv7-M Reference Manual D.3.3]
                            s_PacketTypeToDecode = SWO_PacketType::PC_SAMPLE;
                            s_DecodingPacket     = true;
                        } else if (val == 0b00001110) { // Exception trace packets, discriminator ID1 [ARMv7-M Reference Manual D.3.2]
                            s_PacketTypeToDecode = SWO_PacketType::EXCEPTION_TRACE;
                            s_DecodingPacket     = true;
                        } else if (val == 0b00000101) { // Event counter packet, discriminator ID0 [ARMv7-M Reference Manual D.3.1]
                            s_PacketTypeToDecode = SWO_PacketType::EVENT_COUNTER;
                            s_DecodingPacket     = true;
                        } else if (val ==
                                   0b00010101) { // Periodic PC sample sleep packets, discriminator ID2 [ARMv7-M Reference Manual D.3.3]
                            s_PacketTypeToDecode = SWO_PacketType::PC_SAMPLE_SLEEP;
                            s_DecodingPacket     = true;
                        }
                    }

                    if (s_DecodingPacket) {
                        SWO_ARMv7M_InitializeDecode(s_PacketTypeToDecode);
                        continue;
                    }

                    // Regular SWO decoding
                    if (val == 0b01110000) { // [Overflow] CoreSight Components PDF Table 12-3
                        // no payload - don't set decoding to true
                        //HWDLOG_PROBE_TRACE("SWO OVERFLOW");
                        SWO_ProcessPacket(SWO_PacketType::OVER_FLOW);
                    } else if ((val & 0b00001111) == 0 && (val & 0b01110000) != 0) { // [Timestamp] CoreSight Components PDF Table 12-4
                        bool continuation             = val & 0b10000000;
                        s_SWO_Current_Timestamp_Value = 0;
                        if (continuation) {
                            // [TC[2:0] field if C = 1] CoreSight Components PDF Table 12-4
                            switch ((val >> 4) & 0b111) {
                                case 0b100:
                                    s_SWO_Current_Timestamp_Type = SWO_Timestamp_Type::TIMESTAMP_EMITTED_SYNCHRONOUS_TO_ITM; //
                                    break;
                                case 0b101:
                                    s_SWO_Current_Timestamp_Type = SWO_Timestamp_Type::TIMESTAMP_EMITTED_DELAYED_TO_ITM; //
                                    break;
                                case 0b110:
                                    s_SWO_Current_Timestamp_Type = SWO_Timestamp_Type::PACKET_EMITTED_DELAYED; //
                                    break;
                                case 0b111:
                                    s_SWO_Current_Timestamp_Type = SWO_Timestamp_Type::PACKET_AND_TIMESTAMP_EMITTED_DELAYED; //
                                    break;
                                default:
                                    s_SWO_Current_Timestamp_Type = SWO_Timestamp_Type::RESERVED; //
                                    break;
                            }

                            s_SWO_Packet_Timestamp_ContinuationCount = 1;
                            s_PacketTypeToDecode                     = SWO_PacketType::TIMESTAMP;
                            s_DecodingPacket                         = true;
                        } else {
                            // [TC[2:0] field if C = 0] CoreSight Components PDF Table 12-4
                            switch ((val >> 4) & 0b111) {
                                case 0b000:
                                    s_SWO_Current_Timestamp_Type = SWO_Timestamp_Type::RESERVED; //
                                    break;
                                case 0b111:
                                    s_SWO_Current_Timestamp_Type = SWO_Timestamp_Type::OVERFLOW_ITM; //
                                    break;
                                default:
                                    s_SWO_Current_Timestamp_Type = SWO_Timestamp_Type::TIMESTAMP_EMITTED_SYNCHRONOUS_TO_ITM; //
                                    break;
                            }

                            s_SWO_Packet_Timestamp_ContinuationCount = 0;
                            SWO_ProcessPacket(SWO_PacketType::TIMESTAMP);
                        }
                    } else if ((val & 0b11) && !(val & 0b100)) { // SWIT CoreSight Components PDF Table 12-5
                        s_SWO_SWIT_PADDRDBG       = val >> 3;
                        s_SWO_SWIT_PayloadLength  = 1 << ((val & 0b11) - 1); // number of bytes
                        s_SWO_SWIT_RemainingBytes = s_SWO_SWIT_PayloadLength;
                        s_SWO_SWIT_Payload        = 0;
                        s_PacketTypeToDecode      = SWO_PacketType::SWIT;
                        s_DecodingPacket          = true;
                    } else if ((val & 0b00001111) == 0b0100) { // Reserved CoreSight Components PDF Table 12-1
                        bool continuation = val & 0b10000000;
                        if (continuation) {
                            s_PacketTypeToDecode = SWO_PacketType::RESERVED;
                            s_DecodingPacket     = true;
                        } else {
                            SWO_ProcessPacket(SWO_PacketType::RESERVED);
                        }
                    } else if (val & 0b00001000) {
                        // Extension Packet [Need to find documentation for this. No idea what it is]
                        if (!(val & 0x84)) {
                            SWO_ProcessPacket(SWO_PacketType::RESERVED);
                        } else {
                            s_PacketTypeToDecode = SWO_PacketType::RESERVED;
                            s_DecodingPacket     = true;
                        }
                    } else {
                        HWDLOG_PROBE_CRITICAL("UNKNOWN SWO HEADER {0:#X}", val);
                        s_StopProcess = true;
                        break;
                    }
                } else { /////////////////////////////////// DATA PROCESSING
                    switch (s_PacketTypeToDecode) {
                        case SWO_PacketType::SYNC: {
                            s_Sync_ZeroCount++;
                            if (val == 0x80) {
                                SWO_ResetDecodeState();
                            }
                            break;
                        }
                        case SWO_PacketType::SWIT: {
                            s_SWO_SWIT_Payload |= val << (s_SWO_SWIT_PayloadLength - s_SWO_SWIT_RemainingBytes) * 8;
                            s_SWO_SWIT_RemainingBytes--;
                            if (s_SWO_SWIT_RemainingBytes == 0) {
                                SWO_ProcessPacket(SWO_PacketType::SWIT);
                            }
                            break;
                        }
                        case SWO_PacketType::TIMESTAMP: {
                            bool continuation = val & 0b10000000;

                            s_SWO_Current_Timestamp_Value |= ((val & 0b01111111)) << (7 * (s_SWO_Packet_Timestamp_ContinuationCount - 1));

                            if (continuation) {
                                if (s_SWO_Packet_Timestamp_ContinuationCount > MAX_SWO_TIMESTAMP_CONTINUATIONS) {
                                    HWDLOG_PROBE_ERROR("SWO too many timestamp continuations");
                                    SWO_ResetDecodeState();
                                } else {
                                    s_SWO_Packet_Timestamp_ContinuationCount++;
                                }
                            } else {
                                SWO_ProcessPacket(SWO_PacketType::TIMESTAMP);
                            }
                            break;
                        }
                        case SWO_PacketType::RESERVED: {
                            // reserved packet is max 4 bytes of payload, ignore this for now
                            bool continuation = val & 0b10000000;

                            if (!continuation) {
                                SWO_ProcessPacket(SWO_PacketType::RESERVED);
                            }
                            break;
                        }
                        default: {
                            // ARMv7-M
                            SWO_ARMv7M_ProcessData(s_PacketTypeToDecode, val);
                        }
                    }
                }
            }

            // CoreSight Components PDF Table 12-1
            // Sync packet - 0x800000000000
            if (!s_Synced && val == 0x80 && s_Sync_ZeroCount == 5) {
                HWDLOG_PROBE_WARN("SWO SYNC");
                s_Synced = true;
            }

            if (val == 0) {
                s_Sync_ZeroCount++;
            } else {
                s_Sync_ZeroCount = 0;
            }
        }
    }

    void JLink::Process() {
        static uint8_t s_SWO_Buffer[4096];
        static bool s_FirstRead = true;

        if (s_FirstRead) {
            s_FirstRead = false;

            //return;
            uint8_t testStream[] = {
                0b00000000,
                0b00000000,
                0b00000000,
                0b00000000,
                0b00000000,
                0b10000000,
                // SYNC

                0b01110000,
                // OVERFLOW
                0b01110000,
                // OVERFLOW
                0b01110000,
                // OVERFLOW

                0b11111001,
                0b11111111,
                // SWIT, payload 0xFF

                0b11111011,
                0x50,
                0xA5,
                0xAD,
                0xDE,
                // SWIT, payload 0xDEADA550

                0b10000001,
                0b11111111,
                // SWIT, payload 0xFF

                0b10000000,
                0b11111111,
                0b00000000,
                // Timestamp

                0b10000000,
                // Timestamp

                0b11110000,
                0b11111111,
                0b11111111,
                0b11111111,
                0b01111111,
                // Timestamp
            };

            SWO_Process(testStream, sizeof(testStream));
            HWDLOG_CORE_CRITICAL("--------------------------");
        }

        uint32_t bytesRead = sizeof(s_SWO_Buffer); // bytes to read
        m_Driver->target_SWO_Read(s_SWO_Buffer, 0, &bytesRead);
        // bytesRead now contains amount of bytes that were read

        if (bytesRead) {
            // have to manually flush SWO buffer - JLink DLL does not do this automatically
            // flush [bytesRead] number of bytes from internal buffers
            m_Driver->target_SWO_Control(SWO_Command::FLUSH, &bytesRead);

            SWO_Process(s_SWO_Buffer, bytesRead);
        }

        if (m_TerminalEnabled) {
            char str[1024];
            int readCount = m_Driver->target_RTT_Read(0, str, sizeof(str) - 2);
            for (int i = 0; i < readCount; i++) {
                m_TerminalBuffer.push_back(str[i]);
            }
        }
    }

} // namespace HWD::Probe
