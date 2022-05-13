// ---------------------------------------------------------------------
// CFXS L0 ARM Debugger <https://github.com/CFXS/CFXS-L0-ARM-Debugger>
// Copyright (C) 2022 | CFXS
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>
// ---------------------------------------------------------------------
// [CFXS] //
#pragma once

#include "JLink.hpp"

#include <set>
#include <QString>

namespace L0::Probe {

    ///////////////////////////////////////////////////////////////////////////////////////////////
    static constexpr bool LOG_ENABLED       = true;
    static constexpr bool WARN_LOG_ENABLED  = true;
    static constexpr bool ERROR_LOG_ENABLED = true;
    ///////////////////////////////////////////////////////////////////////////////////////////////
    using namespace Driver::JLink_Types;
    ///////////////////////////////////////////////////////////////////////////////////////////////
    Driver::JLink_Driver* JLink::s_Driver = nullptr;
    ///////////////////////////////////////////////////////////////////////////////////////////////

    void JLink::L0_Load() {
        s_Driver = new Driver::JLink_Driver;

        if (!s_Driver->IsLoaded()) {
            delete s_Driver;
            s_Driver = nullptr;
        }
    }

    void JLink::L0_Unload() {
        if (s_Driver)
            delete s_Driver;
    }

    JLink::JLink() {
        LOG_PROBE_TRACE("[JLink@{0}] Constructor", fmt::ptr(this));
        m_DeviceAssigned = false;
    }

    JLink::~JLink() {
        LOG_PROBE_TRACE("[JLink@{0}] Destructor", fmt::ptr(this));

        if (Probe_IsConnected())
            Probe_Disconnect();
    }

    //////////////////////////////////////////////////////////////////////

    // Select working device by serial number
    void JLink::L0_SelectDevice(uint32_t serialNumber) {
        if (Probe_IsConnected())
            Probe_Disconnect();

        ErrorCode ec = GetDriver()->probe_SelectBySerialNumber_USB(serialNumber);

        GetDriver()->probe_SetWarningCallback(Probe_WarningCallback);
        GetDriver()->probe_SetErrorCallback(Probe_ErrorCallback);
        GetDriver()->probe_SetLogCallback(Probe_LogCallback);

        if (ec != ErrorCode::OK) {
            LOG_PROBE_ERROR("[JLink@{0}] Failed to select probe by serial number {1} - {2}", fmt::ptr(this), serialNumber, ec);
            m_DeviceAssigned = false;
        } else {
            LOG_PROBE_TRACE("[JLink@{0}] Probe {1} selected", fmt::ptr(this), serialNumber);

            m_SerialNumberString = std::to_string(serialNumber);

            m_DeviceAssigned = true;
        }
    }

    bool JLink::Probe_IsReady() const {
        return m_DeviceAssigned;
    }

    /////////////////////////////////////////////////////////////
    // Library

    void JLink::Probe_LogCallback(const char* message) {
        if constexpr (LOG_ENABLED) {
            LOG_PROBE_TRACE("[LOG] {}", message ? message : "-");
        }
    }

    void JLink::Probe_WarningCallback(const char* message) {
        if constexpr (WARN_LOG_ENABLED) {
            LOG_PROBE_WARN("[WARN] >> {}", message ? message : "-");
        }
    }

    void JLink::Probe_ErrorCallback(const char* message) {
        if constexpr (ERROR_LOG_ENABLED) {
            LOG_PROBE_ERROR("[ERROR] >> {}", message ? message : "-");
        }
    }

    void JLink::Probe_FlashProgressCallback(const char* action, const char* prog, int percentage) {
        LOG_PROBE_TRACE("[PROG] >> {} {} {}%", action ? action : "", prog ? prog : "", percentage);
        //m_FlashProgress = percentage / 100.0f;
    }

    /////////////////////////////////////////////////////////////
    // Probe

    const std::string& JLink::GetModelName() const {
        return m_ModelName;
    }

    const std::string& JLink::GetSerialNumberString() const {
        return m_SerialNumberString;
    }

    void JLink::Probe_DisableFlashProgressPopup() {
        char charBuf[256];
        auto ret =
            GetDriver()->probe_ExecuteCommand(Driver::JLink_Types::Commands::DISABLE_FLASH_PROGRESS_POPUP, charBuf, sizeof(charBuf) - 1);

        if (ret != ErrorCode::OK)
            LOG_PROBE_WARN("[JLink@{0}] Failed to disable internal flash progress popup: {1}", fmt::ptr(this), charBuf);
    }

    bool JLink::Probe_Connect() {
        if (!Probe_IsReady()) {
            LOG_PROBE_WARN("[JLink@{0}] Not ready", fmt::ptr(this));
            return false;
        }

        if (Probe_IsConnected()) {
            LOG_PROBE_WARN("[JLink@{0}] Not connecting to probe - already connected", fmt::ptr(this));
            return true;
        }

        const char* openStatus = GetDriver()->probe_Connect();
        //probe_ConnectEx(s_ProbeCallbackEntries[m_ProbeIndex].log, s_ProbeCallbackEntries[m_ProbeIndex].error);

        if (openStatus) {
            LOG_PROBE_ERROR("[JLink@{0}] Failed to connect to probe - {1}", fmt::ptr(this), openStatus);
            return false;
        }

        Probe_DisableFlashProgressPopup();
        GetDriver()->probe_SetFlashProgProgressCallback(Probe_FlashProgressCallback);
        //GetDriver()->probe_SetWarningCallback(s_ProbeCallbackEntries[m_ProbeIndex].warning);

        UpdateProbeInfo();

        // temporary
        GetDriver()->target_SetInterfaceSpeed(50000);

        LOG_PROBE_TRACE("[JLink@{0}] Connected", fmt::ptr(this));

        return true;
    }

    bool JLink::Probe_Disconnect() {
        if (!Probe_IsReady()) {
            LOG_PROBE_WARN("[JLink@{0}] Not ready", fmt::ptr(this));
            return false;
        }

        if (Probe_IsConnected()) {
            LOG_PROBE_TRACE("[JLink@{0}] Disconnect from probe", fmt::ptr(this));

            // need to check if nullptr is a valid no callback value
            GetDriver()->probe_SetLogCallback([](const char*) {
            });
            GetDriver()->probe_SetErrorCallback([](const char*) {
            });
            GetDriver()->probe_SetWarningCallback([](const char*) {
            });

            GetDriver()->probe_Disconnect();
        } else {
            LOG_PROBE_WARN("[JLink@{0}] Not disconnecting - already disconnected", fmt::ptr(this));
        }
        return true;
    }

    bool JLink::Probe_IsConnected() const {
        return GetDriver()->probe_IsConnectionOpen();
    }

    ////////////////////////////////////////////////////////////////////
    // Target

    // TODO: Implement error reason callback for higher level callers
    bool JLink::Target_SelectDevice(const Target::DeviceDescription& device) {
        char resp[256];
        ErrorCode ec = GetDriver()->probe_ExecuteCommand(fmt::format("Device = {0}\n", device.GetName()).c_str(), resp, 256);

        LOG_PROBE_TRACE("[JLink@{0}]:", fmt::ptr(this));
        if (ec == ErrorCode::OK) {
            LOG_PROBE_TRACE("Selected target device \"{0}\"", device.GetName());
            LOG_PROBE_TRACE("Target memory regions:");
            for (auto& reg : device.GetMemoryRegions()) {
                LOG_PROBE_TRACE(" - {0:3}\t{1:6}\t0x{2:08X} - 0x{3:08X}",
                                To_C_String(reg.GetAccessPermissions()),
                                reg.GetName(),
                                reg.GetAddress(),
                                reg.GetAddress() + reg.GetSize());
            }
            return true;
        } else {
            LOG_PROBE_ERROR("Failed to select target device \"{0}\" - {1}", device.GetName(), ec);
            return false;
        }
    }

    // TODO: Implement error reason callback for higher level callers
    bool JLink::Target_SelectDebugInterface(DebugInterface interface) {
        switch (interface) {
            case DebugInterface::SWD: {
                TargetInterfaceMask supportedInterfaces;
                GetDriver()->target_GetAvailableInterfaces(&supportedInterfaces);

                if (supportedInterfaces & TargetInterfaceMask::SWD) {
                    GetDriver()->target_SelectInterface(TargetInterface::SWD);
                    LOG_PROBE_TRACE("[JLink@{0}] Selected SWD target interface", fmt::ptr(this));
                    return true;
                } else {
                    LOG_PROBE_ERROR("[JLink@{0}] Failed to select target interface \"SWD\" - not supported", fmt::ptr(this));
                    return false;
                }
            }
            default:
                LOG_PROBE_ERROR("[JLink@{0}] Failed to select target interface \"{1}\" - unknown interface", fmt::ptr(this), interface);
                return false;
        }
    }

    bool JLink::Target_IsConnected() const {
        return GetDriver()->target_IsConnected();
    }

    // TODO: Implement error reason callback for higher level callers
    bool JLink::Target_Connect() {
        ErrorCode ec = GetDriver()->target_Connect();

        if (ec == ErrorCode::OK) {
            LOG_PROBE_TRACE("[JLink@{0}] Connected to target", fmt::ptr(this));
            PrepareTarget();
            return true;
        } else {
            if ((int)ec == -1) { // Unspecified error
                LOG_PROBE_ERROR("[JLink@{0}] Could not connect to target - unspecified error", fmt::ptr(this));
            } else { // ErrorCode
                LOG_PROBE_ERROR("[JLink@{0}] Could not connect to target - {1}", fmt::ptr(this), ec);
            }

            return false;
        }
    }

    // TODO: Implement error reason callback for higher level callers
    bool JLink::Target_Erase() {
        ErrorCode ec = GetDriver()->target_Erase();

        if (ec == ErrorCode::OK) {
            LOG_PROBE_TRACE("[JLink@{0}] Target program erased", fmt::ptr(this));
            return true;
        } else {
            LOG_PROBE_ERROR("[JLink@{0}] Failed to erase target program - {1}", fmt::ptr(this), ec);
            return false;
        }
    }

    // TODO: Implement error reason callback for higher level callers
    bool JLink::Target_WriteProgram(const uint8_t* data, uint32_t size) {
        GetDriver()->target_BeginDownload(DownloadFlags::ALLOW_FLASH | DownloadFlags::ALLOW_BUFFERED_RAM);

        ErrorCode ret = GetDriver()->target_WriteProgram(0, size, (void*)data);
        if ((int)ret >= 0) {
            ret = GetDriver()->target_EndDownload();

            if ((int)ret >= 0 || (int)ret == -2) { // return code -2 is error code for target program matching requested program
                LOG_PROBE_TRACE("[JLink@{0}] Target programmed", fmt::ptr(this));
                return true;
            } else {
                const char* reason;

                switch ((int)ret) {
                    case -3: reason = "program/erase failed";
                    case -4: reason = "verification failed";
                    default: reason = "error";
                }

                LOG_PROBE_ERROR("[JLink@{0}] Failed to program target - {1}", fmt::ptr(this), reason);
                return false;
            }
        } else {
            LOG_PROBE_ERROR("[JLink@{0}] Failed to program target - {1}", fmt::ptr(this), ret);
            return false;
        }
    }

    // TODO: Implement error reason callback for higher level callers
    bool JLink::Target_Reset(bool haltAfterReset) {
        ErrorCode ec;

        if (haltAfterReset) {
            if (m_ProbeCapabilities & ProbeCapabilities::RESET_STOP_TIMED) { // can halt immediately after reset
                ec = GetDriver()->target_Reset();
                if (ec != ErrorCode::OK) {
                    LOG_PROBE_ERROR("[JLink@{0}] Failed to reset target - {1}", fmt::ptr(this), ec);
                }
            } else {
                LOG_PROBE_WARN("[JLink@{0}] Immediate reset and halt not supported - resetting and halting seperately", fmt::ptr(this));
                GetDriver()->target_ResetAndRun();
                GetDriver()->target_Halt();
            }
        } else {
            GetDriver()->target_ResetAndRun();
        }

        return true;
    }

    uint8_t JLink::Target_ReadMemory_8(uint32_t address, bool* success) {
        uint8_t tmp;

        if (success) {
            *success = GetDriver()->target_ReadMemory_8(address, 1, &tmp, nullptr) == 1;
        } else {
            GetDriver()->target_ReadMemory_8(address, 1, &tmp, nullptr);
        }

        return tmp;
    }

    uint16_t JLink::Target_ReadMemory_16(uint32_t address, bool* success) {
        uint16_t tmp;

        if (success) {
            *success = GetDriver()->target_ReadMemory_16(address, 1, &tmp, nullptr) == 1;
        } else {
            GetDriver()->target_ReadMemory_16(address, 1, &tmp, nullptr);
        }

        return tmp;
    }

    uint32_t JLink::Target_ReadMemory_32(uint32_t address, bool* success) {
        uint32_t tmp;

        if (success) {
            *success = GetDriver()->target_ReadMemory_32(address, 1, &tmp, nullptr) == 1;
        } else {
            GetDriver()->target_ReadMemory_32(address, 1, &tmp, nullptr);
        }

        return tmp;
    }

    uint64_t JLink::Target_ReadMemory_64(uint32_t address, bool* success) {
        uint64_t tmp;

        if (success) {
            *success = GetDriver()->target_ReadMemory_64(address, 1, &tmp, nullptr) == 1;
        } else {
            GetDriver()->target_ReadMemory_64(address, 1, &tmp, nullptr);
        }

        return tmp;
    }

    int JLink::Target_ReadMemoryTo(uint32_t address, void* to, uint32_t bytesToRead, AccessWidth accessWidth) {
        return GetDriver()->target_ReadMemoryEx(address, bytesToRead, to, (int)accessWidth);
    }

    bool JLink::Target_WriteMemory_32(uint32_t address, uint32_t value) {
        return GetDriver()->target_WriteMemory_32(address, value) == 0;
    }

    bool JLink::Target_Halt() {
        return GetDriver()->target_Halt();
    }
    bool JLink::Target_Run() {
        Target_Halt();
        GetDriver()->target_Run();
        return true;
    }
    bool JLink::Target_IsRunning() {
        return GetDriver()->target_IsHalted();
    }

    float JLink::Target_GetFlashProgress() {
        return 0;
        //return m_FlashProgress;
    }

    uint64_t JLink::Target_ReadPC(bool* success) {
        return GetDriver()->target_ReadRegister((Driver::JLink_Types::CPU_Registers::ARM)Driver::JLink_Types::Cortex_M4::R15);
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
    static bool s_Synced                = false;
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
                //LOG_PROBE_TRACE("[SWO Packet] OVERFLOW");
                break;
            case SWO_PacketType::TIMESTAMP:
                s_SWO_Stats.Counters.Timestamp++; //
                //LOG_PROBE_TRACE(
                //    "[SWO Packet] TIMESTAMP: {0} C = {1}", s_SWO_Current_Timestamp_Value, s_SWO_Packet_Timestamp_ContinuationCount);
                break;
            case SWO_PacketType::RESERVED:
                s_SWO_Stats.Counters.Reserved++; //
                LOG_PROBE_TRACE("[SWO Packet] RESERVED");
                break;
            case SWO_PacketType::SWIT:
                s_SWO_Stats.Counters.SWIT++; //
                //LOG_PROBE_TRACE("[SWO Packet] SWIT {0:#X} [{1} bytes] - PADDRDBG {2:#X}",
                //                   s_SWO_SWIT_Payload,
                //                   s_SWO_SWIT_PayloadLength,
                //                   s_SWO_SWIT_PADDRDBG);
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
    char* s_Console;
    bool s_ConsoleUpdated;
    static void SWO_ARMv7M_ProcessData(SWO_PacketType type, uint8_t val) {
        switch (type) {
            case SWO_PacketType::EVENT_COUNTER: { // no payload
                LOG_PROBE_TRACE("[SWO ARMv7-M Packet] EVENT_COUNTER {0:#X}", val);
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

                    //LOG_PROBE_ERROR("[SWO ARMv7-M Packet] EXCEPTION_TRACE {1} {0}",
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
                    //LOG_PROBE_WARN("[SWO ARMv7-M Packet] PC_SAMPLE PC = {0}", pcs);
                    SWO_ResetDecodeState(); // packet processed
                }
                break;
            }
            case SWO_PacketType::PC_SAMPLE_SLEEP: { // 1 byte payload (ignore)
                if (s_SWO_ARMv7M_ProgramCounterSleep_PayloadRead == false) {
                    s_SWO_ARMv7M_ProgramCounterSleep_PayloadRead = true;
                } else {
                    LOG_PROBE_TRACE("[SWO ARMv7-M Packet] PC_SAMPLE_SLEEP");
                    SWO_ResetDecodeState(); // packet processed
                }
                break;
            }
            default: LOG_PROBE_ERROR("UNKNOWN ARMv7 PACKET TYPE");
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
                        //LOG_PROBE_TRACE("SWO OVERFLOW");
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
                        LOG_PROBE_CRITICAL("UNKNOWN SWO HEADER {0:#X}", val);
                        //s_StopProcess = true;
                        SWO_ResetDecodeState();
                        s_Synced = false;
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
                                    LOG_PROBE_ERROR("SWO too many timestamp continuations");
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
                LOG_PROBE_WARN("SWO SYNC");
                s_Synced = true;
            }

            if (val == 0) {
                s_Sync_ZeroCount++;
            } else {
                s_Sync_ZeroCount = 0;
            }
        }
    }

    static uint8_t s_SWO_Buffer[4096];
    void JLink::Process() {
        uint32_t bytesRead = sizeof(s_SWO_Buffer); // bytes to read
        GetDriver()->target_SWO_Read(s_SWO_Buffer, 0, &bytesRead);
        // bytesRead now contains amount of bytes that were read

        if (bytesRead) {
            // have to manually flush SWO buffer - JLink DLL does not do this automatically
            // flush [bytesRead] number of bytes from internal buffers
            GetDriver()->target_SWO_Control(SWO_Command::FLUSH, &bytesRead);

            SWO_Process(s_SWO_Buffer, bytesRead);
        }
    }

} // namespace L0::Probe
