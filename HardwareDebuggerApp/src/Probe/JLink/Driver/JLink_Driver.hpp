#pragma once
#include <DynamicLibrary.hpp>
#include "JLink_Types.hpp"

namespace HWD::Probe::Driver {

    class JLink_Driver {
    public:
        //

    public:
        JLink_Driver();
        ~JLink_Driver() = default;

        bool IsLoaded() const {
            return m_Loaded;
        }

    private:
        void LoadFunctionPointers();

    private:
        std::unique_ptr<DynamicLibrary> m_Library;
        bool m_Loaded = false;

    public: // library functions
        int (*probe_GetAvailableLicense)(char* pBuffer, uint32_t BufferSize)                                              = nullptr;
        JLink_Types::ErrorCode (*target_Erase)(void)                                                                      = nullptr;
        void (*target_BeginDownload)(uint32_t Flags)                                                                      = nullptr;
        void (*probe_Disconnect)(void)                                                                                    = nullptr;
        int (*target_ClearBreakpoint)(unsigned BPIndex)                                                                   = nullptr;
        int (*target_ClearBreakpointEx)(int BPHandle)                                                                     = nullptr;
        int (*target_ClearDataEvent)(uint32_t Handle)                                                                     = nullptr;
        void (*library_ClearErrorState)(void)                                                                             = nullptr;
        void (*target_ClearResetPin)(void)                                                                                = nullptr;
        JLink_Types::ErrorCode (*target_Connect)(void)                                                                    = nullptr;
        void (*probe_SetLogCallback)(JLink_Types::LogCallback pfLog)                                                      = nullptr;
        void (*probe_SetCommunicationLogCallback)(JLink_Types::LogCallback pfLog)                                         = nullptr;
        void (*probe_EnableSoftwareBreakpoints)(char Enable)                                                              = nullptr;
        JLink_Types::ErrorCode (*target_EndDownload)(void)                                                                = nullptr;
        JLink_Types::ErrorCode (*probe_ExecuteCommand)(const char* inputBuffer, char* outputBuffer, int outputBufferSize) = nullptr;
        int (*target_FindBreakpoint)(uint32_t address)                                                                    = nullptr;
        uint32_t (*target_GetBreakpointInfo)(int handle)                                                                  = nullptr;
        int (*target_GetBreakpointInfoEx)(int index, JLink_Types::BreakpointInfo* pInfo)                                  = nullptr;
        int (*target_GetDebugInfo)(JLink_Types::DebugInfoIndex Index, uint32_t* pInfo)                                    = nullptr;
        JLink_Types::DeviceFamily (*target_GetDeviceFamily)(void)                                                         = nullptr;
        uint32_t (*library_GetVersion)(void)                                                                              = nullptr;
        JLink_Types::ProbeCapabilities (*probe_GetCapabilities)(void)                                                     = nullptr;
        void (*probe_GetCapabilitiesEx)(JLink_Types::ProbeExtendedCapabilities* capabilities, int bufferEntryCount)       = nullptr;
        void (*probe_GetFeatureString)(char* buffer_256)                                                                  = nullptr;
        void (*probe_GetFirmwareString)(char* buffer, int bufferSize)                                                     = nullptr;
        int (*probe_GetHardwareVersion)(void)                                                                             = nullptr;
        int (*target_GetHardwareInfo)(uint32_t BitMask, uint32_t* pHWInfo)                                                = nullptr;
        int (*target_GetHardwareStatus)(JLink_Types::HardwareStatus* pStat)                                               = nullptr;
        uint32_t (*target_Get_CPU_ID)(void)                                                                               = nullptr;
        int (*target_GetDebugEntryReason)(JLink_Types::MOE_Info* pInfo, int MaxNumMOEs)                                   = nullptr;
        uint32_t (*probe_GetBreakpointCount)(void)                                                                        = nullptr;
        int (*probe_GetAvailableBreakpointCount)(uint32_t Type)                                                           = nullptr;
        unsigned (*probe_GetWatchpointCount)(void)                                                                        = nullptr;
        int (*probe_GetAvailableWatchpointCount)(void)                                                                    = nullptr;
        int (*target_GetRegisterList)(uint32_t* paList, int MaxNumItems)                                                  = nullptr;
        const char* (*target_GetRegisterName)(uint32_t RegIndex)                                                          = nullptr;
        int (*target_GetResetTypeDescription)(JLink_Types::ResetType resetType,
                                              const char** psResetName,
                                              const char** psResetDesc)                                                   = nullptr;
        uint32_t (*probe_GetSerialNumber)(void)                                                                           = nullptr;
        uint16_t (*target_GetSpeed)(void)                                                                                 = nullptr;
        void (*target_GetSpeedInfo)(JLink_Types::SpeedInfo* pSpeedInfo)                                                   = nullptr;
        int (*target_GetWatchpointInfoEx)(int iWP, JLink_Types::WatchpointInfo* pInfo)                                    = nullptr;
        void (*target_Run)(void)                                                                                          = nullptr;
        void (*target_RunAllowSimulation)(uint32_t NumInsts)                                                              = nullptr;
        void (*target_RunNumberOfInstructions)(uint32_t MaxEmulInsts, uint32_t Flags)                                     = nullptr;
        int (*target_RunNumberOfClocks)(uint32_t NumClocks)                                                               = nullptr;
        void (*target_RunAndDisableInterrupts)(void)                                                                      = nullptr;
        char (*target_Halt)(void)                                                                                         = nullptr;
        int (*library_InErrorState)(void)                                                                                 = nullptr;
        char (*probe_IsConnected)(void)                                                                                   = nullptr;
        char (*target_IsHalted)(void)                                                                                     = nullptr;
        char (*probe_IsConnectionOpen)(void)                                                                              = nullptr;
        void (*library_Lock)(void)                                                                                        = nullptr;
        void (*library_Unlock)(void)                                                                                      = nullptr;
        int (*target_Measure_CPU_Speed)(uint32_t RAMAddr, int PreserveMem)                                                = nullptr;
        int (*target_Measure_CPU_Speed_Ex)(uint32_t RAMAddr, int PreserveMem, int AllowFail)                              = nullptr;
        const char* (*probe_Connect)(void)                                                                                = nullptr;
        const char* (*probe_ConnectEx)(JLink_Types::LogCallback pfLog, JLink_Types::LogCallback pfErrorOut)               = nullptr;
        int (*target_ReadCodeMemory)(uint32_t Addr, uint32_t NumBytes, void* pData)                                       = nullptr;
        uint32_t (*target_Read_ICE_Register)(int RegIndex)                                                                = nullptr;
        int (*target_ReadMemory)(uint32_t Addr, uint32_t NumBytes, void* pData)                                           = nullptr;
        int (*target_ReadMemoryEx)(uint32_t Addr, uint32_t NumBytes, void* pData, uint32_t Flags)                         = nullptr;
        int (*target_ReadMemoryImmediate)(uint32_t Addr, uint32_t Count, void* pData)                                     = nullptr;
        int (*target_ReadMemoryIndirect)(uint32_t Addr, uint32_t NumBytes, void* pData)                                   = nullptr;
        int (*target_ReadMemory_8)(uint32_t Addr, uint32_t NumItems, uint8_t* pData, uint8_t* pStatus)                    = nullptr;
        int (*target_ReadMemory_16)(uint32_t Addr, uint32_t NumItems, uint16_t* pData, uint8_t* pStatus)                  = nullptr;
        int (*target_ReadMemory_32)(uint32_t Addr, uint32_t NumItems, uint32_t* pData, uint8_t* pStatus)                  = nullptr;
        int (*target_ReadMemory_64)(uint32_t Addr, uint32_t NumItems, uint64_t* pData, uint8_t* pStatus)                  = nullptr;
        int (*target_ReadMemoryZonedEx)(uint32_t Addr, uint32_t NumBytes, void* pData, uint32_t Flags, const char* sZone) = nullptr;
        uint32_t (*target_ReadRegister)(JLink_Types::CPU_Registers::ARM RegIndex)                                         = nullptr;
        JLink_Types::ErrorCode (*target_ReadRegisters)(const uint32_t* paRegIndex,
                                                       uint32_t* paData,
                                                       uint8_t* paStatus,
                                                       uint32_t NumRegs)                                                  = nullptr;
        JLink_Types::ErrorCode (*target_Reset)(void)                                                                      = nullptr;
        void (*target_ResetAndRun)(void)                                                                                  = nullptr;
        void (*target_SelectTraceSource)(JLink_Types::TraceSource source)                                                 = nullptr;
        int (*target_SetBreakpoint)(uint32_t index, uint32_t address)                                                     = nullptr;
        int (*target_SetBreakpointEx)(uint32_t Addr, uint32_t TypeFlags)                                                  = nullptr;
        int (*target_SetDataEvent)(JLink_Types::DataEvent* pEvent, uint32_t* pHandle)                                     = nullptr;
        int (*target_SetEndiannes)(JLink_Types::ARM_Endian endianness)                                                    = nullptr;
        void (*probe_SetErrorCallback)(JLink_Types::LogCallback pfErrorOut)                                               = nullptr;
        int (*probe_SetHookUnsecureDialog)(JLink_Types::UnsecureDialogHook pfHook)                                        = nullptr;
        int (*target_SetInitializeRegistersOnReset)(bool state)                                                           = nullptr;
        void (*target_SetMaxInterfaceSpeed)(void)                                                                         = nullptr;
        void (*target_SetResetPin)(void)                                                                                  = nullptr;
        void (*target_SetResetDelay)(int ms)                                                                              = nullptr;
        JLink_Types::ResetType (*target_SetResetType)(JLink_Types::ResetType ResetType)                                   = nullptr;
        void (*target_SetInterfaceSpeed)(uint32_t Speed)                                                                  = nullptr;
        void (*probe_SetWaitFunction)(JLink_Types::WaitFunction, void* pContext)                                          = nullptr;
        void (*probe_SetWarningCallback)(JLink_Types::LogCallback pfWarnOut)                                              = nullptr;
        int (*target_SetWatchpoint)(uint32_t Addr, uint32_t AddrMask, uint32_t Data, uint32_t DataMask, uint8_t Ctrl, uint8_t CtrlMask) =
            nullptr;
        char (*target_SimulateInstruction)(uint32_t Inst)                                                                         = nullptr;
        char (*target_Step)(void)                                                                                                 = nullptr;
        uint16_t (*probe_UpdateProbeFirmware)(void)                                                                               = nullptr;
        uint32_t (*probe_UpdateProbeFirmwareIfNewer)(void)                                                                        = nullptr;
        int (*target_WaitForHalt)(int TimeOut)                                                                                    = nullptr;
        void (*target_Write_ICE_Register)(uint32_t RegIndex, uint32_t Value, int AllowDelay)                                      = nullptr;
        JLink_Types::ErrorCode (*target_WriteProgram)(uint32_t Addr, uint32_t Count, const void* pData)                           = nullptr;
        int (*target_WriteMemoryDelayed)(uint32_t Addr, uint32_t Count, const void* pData)                                        = nullptr;
        int (*target_WriteMemoryEx)(uint32_t Addr, uint32_t NumBytes, const void* p, uint32_t Flags)                              = nullptr;
        int (*target_WriteMemoryImmediate)(uint32_t Addr, uint32_t Count, const void* p)                                          = nullptr;
        int (*target_WriteMemoryDescriptor)(JLink_Types::WriteMemoryDescriptor* paDesc, uint32_t NumWrites)                       = nullptr;
        char (*target_WriteRegister)(JLink_Types::CPU_Registers::ARM RegIndex, uint32_t Data)                                     = nullptr;
        int (*target_WriteRegisters)(const uint32_t* paRegIndex, const uint32_t* paData, uint8_t* paStatus, uint32_t NumRegs)     = nullptr;
        int (*target_WriteMemory_8)(uint32_t Addr, uint8_t Data)                                                                  = nullptr;
        int (*target_WriteMemory_16)(uint32_t Addr, uint16_t Data)                                                                = nullptr;
        int (*target_WriteMemory_32)(uint32_t Addr, uint32_t Data)                                                                = nullptr;
        int (*target_WriteMemory_64)(uint32_t Addr, uint64_t Data)                                                                = nullptr;
        int (*target_WriteMemoryZonedEx)(uint32_t Addr, uint32_t NumBytes, const void* p, uint32_t Flags, const char* sZone)      = nullptr;
        int (*target_WriteVectorCatch)(uint32_t Value)                                                                            = nullptr;
        int (*target_WriteVerifyMemory)(uint32_t Addr, uint32_t NumBytes, const void* p, uint32_t Flags)                          = nullptr;
        uint32_t (*target_GetCore)(void)                                                                                          = nullptr;
        int (*target_CoreSight_Configure)(const char* sConfig)                                                                    = nullptr;
        int (*target_CoreSight_ReadAPDPReg)(uint8_t RegIndex, uint8_t APnDP, uint32_t* pData)                                     = nullptr;
        int (*target_CoreSight_WriteAPDP)(uint8_t RegIndex, uint8_t APnDP, uint32_t Data)                                         = nullptr;
        int (*target_CP15_IsPresent)(void)                                                                                        = nullptr;
        int (*target_CP15_ReadEx)(uint8_t CRn, uint8_t CRm, uint8_t op1, uint8_t op2, uint32_t* pData)                            = nullptr;
        int (*target_CP15_ReadRegister)(uint32_t RegIndex, uint32_t* pData)                                                       = nullptr;
        int (*target_CP15_WriteEx)(uint8_t CRn, uint8_t CRm, uint8_t op1, uint8_t op2, uint32_t Data)                             = nullptr;
        int (*target_CP15_WriteRegister)(uint32_t RegIndex, uint32_t Data)                                                        = nullptr;
        int (*probe_AddLicense)(const char* sLic)                                                                                 = nullptr;
        int (*probe_EraseLicenses)(void)                                                                                          = nullptr;
        int (*probe_GetLicenses)(char* pBuffer, uint32_t BufferSize)                                                              = nullptr;
        int (*probe_GetCounters)(uint32_t BitMask, uint32_t* pCounters)                                                           = nullptr;
        int (*probe_GetDeviceList)(JLink_Types::HostInterfaceType hostInterface,
                                   JLink_Types::ProbeInfo* paConnectInfo,
                                   size_t MaxInfos)                                                                               = nullptr;
        uint32_t (*probe_GetConnectedDeviceCount)(void)                                                                           = nullptr;
        int (*probe_IsCapableOfEx)(JLink_Types::ProbeExtendedCapabilities CapEx)                                                  = nullptr;
        char (*target_IsConnected)(void)                                                                                          = nullptr;
        JLink_Types::ErrorCode (*probe_SelectBySerialNumber_USB)(uint32_t SerialNo)                                               = nullptr;
        int (*probe_SelectByIP)(char* pIPAddr, int BufferSize, uint16_t* pPort)                                                   = nullptr;
        void (*probe_SelectBySerialNumber_Ethernet)(uint32_t SerialNo)                                                            = nullptr;
        char (*target_ETB_IsPresent)(void)                                                                                        = nullptr;
        uint32_t (*target_ETB_ReadRegister)(uint32_t RegIndex)                                                                    = nullptr;
        void (*target_ETB_WriteRegister)(uint32_t RegIndex, uint32_t Data, int AllowDelay)                                        = nullptr;
        char (*target_ETM_IsPresent)(void)                                                                                        = nullptr;
        uint32_t (*target_ETM_ReadRegister)(uint32_t Reg)                                                                         = nullptr;
        void (*target_ETM_StartTrace)(void)                                                                                       = nullptr;
        void (*target_ETM_WriteRegister)(uint32_t Reg, uint32_t Data, int AllowDelay)                                             = nullptr;
        int (*probe_HSS_GetCapabilities)(JLink_Types::HSS_Capabilities* capabilities)                                             = nullptr;
        int (*target_HSS_Start)(JLink_Types::HSS_MemoryBlockDescriptor* paDesc, int NumBlocks, int Period_us, int Flags)          = nullptr;
        int (*target_HSS_Stop)(void)                                                                                              = nullptr;
        int (*target_HSS_Read)(void* pBuffer, uint32_t BufferSize)                                                                = nullptr;
        int (*target_RawTrace_Control)(uint32_t Cmd, void* pData)                                                                 = nullptr;
        int (*target_RawTrace_Read)(uint8_t* pData, uint32_t NumBytes)                                                            = nullptr;
        JLink_Types::ErrorCode (*target_RTT_Control)(JLink_Types::RTT_Command cmd, void* p)                                       = nullptr;
        int (*target_RTT_Read)(uint32_t BufferIndex, char* sBuffer, uint32_t BufferSize)                                          = nullptr;
        int (*target_RTT_Write)(uint32_t BufferIndex, const char* sBuffer, uint32_t BufferSize)                                   = nullptr;
        int (*target_STrace_Config)(const char* sConfig)                                                                          = nullptr;
        int (*target_STrace_Control)(uint32_t Cmd, void* pData)                                                                   = nullptr;
        int (*target_STrace_Read)(uint32_t* paItem, uint32_t NumItems)                                                            = nullptr;
        int (*target_STrace_GetInstStats)(void* readTo,
                                          uint32_t addr,
                                          uint32_t count,
                                          uint32_t structSize,
                                          JLink_Types::STRACE_InstCounters counter)                                               = nullptr;
        int (*target_STrace_Start)(void)                                                                                          = nullptr;
        int (*target_STrace_Stop)(void)                                                                                           = nullptr;
        void (*target_SWD_GetData)(uint8_t* pOut, int BitPos, int NumBits)                                                        = nullptr;
        uint32_t (*target_SWD_GetData_8)(int BitPos)                                                                              = nullptr;
        uint32_t (*target_SWD_GetData_16)(int BitPos)                                                                             = nullptr;
        uint32_t (*target_SWD_GetData_32)(int BitPos)                                                                             = nullptr;
        void (*target_SWD_StoreGetRaw)(const uint8_t* pDir, const uint8_t* pIn, uint8_t* pOut, uint32_t NumBits)                  = nullptr;
        int (*target_SWD_StoreRaw)(const uint8_t* pDir, const uint8_t* pIn, uint32_t NumBits)                                     = nullptr;
        void (*target_SWD_SyncBits)(void)                                                                                         = nullptr;
        void (*target_SWD_SyncBytes)(void)                                                                                        = nullptr;
        int (*target_SWO_Control)(JLink_Types::SWO_Command Cmd, void* pData)                                                      = nullptr;
        int (*target_SWO_Disable)(uint32_t PortMask)                                                                              = nullptr;
        int (*target_SWO_Enable)(uint32_t CPUSpeed, uint32_t SWOSpeed, int Mode, uint32_t PortMask)                               = nullptr;
        int (*target_SWO_GetCompatibleSpeeds)(uint32_t CPUSpeed, uint32_t MaxSWOSpeed, uint32_t* paSWOSpeed, uint32_t NumEntries) = nullptr;
        void (*target_SWO_Read)(uint8_t* pData, uint32_t Offset, uint32_t* pNumBytes)                                             = nullptr;
        int (*target_SWO_ReadStimulus)(int Port, uint8_t* pData, uint32_t NumBytes)                                               = nullptr;
        void (*target_GetAvailableInterfaces)(JLink_Types::TargetInterfaceMask* pMask)                                            = nullptr;
        JLink_Types::ErrorCode (*target_SelectInterface)(JLink_Types::TargetInterface targetIf)                                   = nullptr;
        uint32_t (*target_Trace_Control)(uint32_t Cmd, uint32_t* p)                                                               = nullptr;
        uint32_t (*target_Trace_Read)(JLink_Types::TraceData* pData, uint32_t Off, uint32_t* pNumItems)                           = nullptr;
        void (*probe_SetFlashProgProgressCallback)(JLink_Types::FlashProgressCallback pfOnFlashProgess)                           = nullptr;
    };

} // namespace HWD::Probe::Driver