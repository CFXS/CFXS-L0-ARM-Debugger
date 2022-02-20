#include "JLink_Driver.hpp"

namespace L0::Probe::Driver {

    void JLink_Driver::LoadFunctionPointers() {
        probe_GetAvailableLicense = reinterpret_cast<decltype(probe_GetAvailableLicense)>(m_Library->resolve("JLINK_GetAvailableLicense"));
        target_Erase              = reinterpret_cast<decltype(target_Erase)>(m_Library->resolve("JLINK_EraseChip"));
        target_BeginDownload      = reinterpret_cast<decltype(target_BeginDownload)>(m_Library->resolve("JLINKARM_BeginDownload"));
        probe_Disconnect          = reinterpret_cast<decltype(probe_Disconnect)>(m_Library->resolve("JLINKARM_Close"));
        target_ClearBreakpoint    = reinterpret_cast<decltype(target_ClearBreakpoint)>(m_Library->resolve("JLINKARM_ClrBP"));
        target_ClearBreakpointEx  = reinterpret_cast<decltype(target_ClearBreakpointEx)>(m_Library->resolve("JLINKARM_ClrBPEx"));
        target_ClearDataEvent     = reinterpret_cast<decltype(target_ClearDataEvent)>(m_Library->resolve("JLINKARM_ClrDataEvent"));
        library_ClearErrorState   = reinterpret_cast<decltype(library_ClearErrorState)>(m_Library->resolve("JLINKARM_ClrError"));
        target_ClearResetPin      = reinterpret_cast<decltype(target_ClearResetPin)>(m_Library->resolve("JLINKARM_ClrRESET"));
        target_Connect            = reinterpret_cast<decltype(target_Connect)>(m_Library->resolve("JLINKARM_Connect"));
        probe_SetLogCallback      = reinterpret_cast<decltype(probe_SetLogCallback)>(m_Library->resolve("JLINKARM_EnableLog"));
        probe_SetCommunicationLogCallback =
            reinterpret_cast<decltype(probe_SetCommunicationLogCallback)>(m_Library->resolve("JLINKARM_EnableLogCom"));
        probe_EnableSoftwareBreakpoints =
            reinterpret_cast<decltype(probe_EnableSoftwareBreakpoints)>(m_Library->resolve("JLINKARM_EnableSoftBPs"));
        target_EndDownload         = reinterpret_cast<decltype(target_EndDownload)>(m_Library->resolve("JLINKARM_EndDownload"));
        probe_ExecuteCommand       = reinterpret_cast<decltype(probe_ExecuteCommand)>(m_Library->resolve("JLINKARM_ExecCommand"));
        target_FindBreakpoint      = reinterpret_cast<decltype(target_FindBreakpoint)>(m_Library->resolve("JLINKARM_FindBP"));
        target_GetBreakpointInfo   = reinterpret_cast<decltype(target_GetBreakpointInfo)>(m_Library->resolve("JLINKARM_GetBPInfo"));
        target_GetBreakpointInfoEx = reinterpret_cast<decltype(target_GetBreakpointInfoEx)>(m_Library->resolve("JLINKARM_GetBPInfoEx"));
        target_GetDebugInfo        = reinterpret_cast<decltype(target_GetDebugInfo)>(m_Library->resolve("JLINKARM_GetDebugInfo"));
        target_GetDeviceFamily     = reinterpret_cast<decltype(target_GetDeviceFamily)>(m_Library->resolve("JLINKARM_GetDeviceFamily"));
        library_GetVersion         = reinterpret_cast<decltype(library_GetVersion)>(m_Library->resolve("JLINKARM_GetDLLVersion"));
        probe_GetCapabilities      = reinterpret_cast<decltype(probe_GetCapabilities)>(m_Library->resolve("JLINKARM_GetEmuCaps"));
        probe_GetCapabilitiesEx    = reinterpret_cast<decltype(probe_GetCapabilitiesEx)>(m_Library->resolve("JLINKARM_GetEmuCapsEx"));
        probe_GetFeatureString     = reinterpret_cast<decltype(probe_GetFeatureString)>(m_Library->resolve("JLINKARM_GetFeatureString"));
        probe_GetFirmwareString    = reinterpret_cast<decltype(probe_GetFirmwareString)>(m_Library->resolve("JLINKARM_GetFirmwareString"));
        probe_GetHardwareVersion = reinterpret_cast<decltype(probe_GetHardwareVersion)>(m_Library->resolve("JLINKARM_GetHardwareVersion"));
        target_GetHardwareInfo   = reinterpret_cast<decltype(target_GetHardwareInfo)>(m_Library->resolve("JLINKARM_GetHWInfo"));
        target_GetHardwareStatus = reinterpret_cast<decltype(target_GetHardwareStatus)>(m_Library->resolve("JLINKARM_GetHWStatus"));
        target_Get_CPU_ID        = reinterpret_cast<decltype(target_Get_CPU_ID)>(m_Library->resolve("JLINKARM_GetId"));
        target_GetDebugEntryReason = reinterpret_cast<decltype(target_GetDebugEntryReason)>(m_Library->resolve("JLINKARM_GetMOEs"));
        probe_GetBreakpointCount   = reinterpret_cast<decltype(probe_GetBreakpointCount)>(m_Library->resolve("JLINKARM_GetNumBPs"));
        probe_GetAvailableBreakpointCount =
            reinterpret_cast<decltype(probe_GetAvailableBreakpointCount)>(m_Library->resolve("JLINKARM_GetNumBPUnits"));
        probe_GetWatchpointCount = reinterpret_cast<decltype(probe_GetWatchpointCount)>(m_Library->resolve("JLINKARM_GetNumWPs"));
        probe_GetAvailableWatchpointCount =
            reinterpret_cast<decltype(probe_GetAvailableWatchpointCount)>(m_Library->resolve("JLINKARM_GetNumWPUnits"));
        target_GetRegisterList = reinterpret_cast<decltype(target_GetRegisterList)>(m_Library->resolve("JLINKARM_GetRegisterList"));
        target_GetRegisterName = reinterpret_cast<decltype(target_GetRegisterName)>(m_Library->resolve("JLINKARM_GetRegisterName"));
        target_GetResetTypeDescription =
            reinterpret_cast<decltype(target_GetResetTypeDescription)>(m_Library->resolve("JLINKARM_GetResetTypeDesc"));
        probe_GetSerialNumber          = reinterpret_cast<decltype(probe_GetSerialNumber)>(m_Library->resolve("JLINKARM_GetSN"));
        target_GetSpeed                = reinterpret_cast<decltype(target_GetSpeed)>(m_Library->resolve("JLINKARM_GetSpeed"));
        target_GetSpeedInfo            = reinterpret_cast<decltype(target_GetSpeedInfo)>(m_Library->resolve("JLINKARM_GetSpeedInfo"));
        target_GetWatchpointInfoEx     = reinterpret_cast<decltype(target_GetWatchpointInfoEx)>(m_Library->resolve("JLINKARM_GetWPInfoEx"));
        target_Run                     = reinterpret_cast<decltype(target_Run)>(m_Library->resolve("JLINKARM_Go"));
        target_RunAllowSimulation      = reinterpret_cast<decltype(target_RunAllowSimulation)>(m_Library->resolve("JLINKARM_GoAllowSim"));
        target_RunNumberOfInstructions = reinterpret_cast<decltype(target_RunNumberOfInstructions)>(m_Library->resolve("JLINKARM_GoEx"));
        target_RunNumberOfClocks       = reinterpret_cast<decltype(target_RunNumberOfClocks)>(m_Library->resolve("JLINKARM_GoHalt"));
        target_RunAndDisableInterrupts =
            reinterpret_cast<decltype(target_RunAndDisableInterrupts)>(m_Library->resolve("JLINKARM_GoIntDis"));
        target_Halt              = reinterpret_cast<decltype(target_Halt)>(m_Library->resolve("JLINKARM_Halt"));
        library_InErrorState     = reinterpret_cast<decltype(library_InErrorState)>(m_Library->resolve("JLINKARM_HasError"));
        target_IsConnected       = reinterpret_cast<decltype(target_IsConnected)>(m_Library->resolve("JLINKARM_IsConnected"));
        target_IsHalted          = reinterpret_cast<decltype(target_IsHalted)>(m_Library->resolve("JLINKARM_IsHalted"));
        probe_IsConnectionOpen   = reinterpret_cast<decltype(probe_IsConnectionOpen)>(m_Library->resolve("JLINKARM_IsOpen"));
        library_Lock             = reinterpret_cast<decltype(library_Lock)>(m_Library->resolve("JLINKARM_Lock"));
        target_Measure_CPU_Speed = reinterpret_cast<decltype(target_Measure_CPU_Speed)>(m_Library->resolve("JLINKARM_MeasureCPUSpeed"));
        target_Measure_CPU_Speed_Ex =
            reinterpret_cast<decltype(target_Measure_CPU_Speed_Ex)>(m_Library->resolve("JLINKARM_MeasureCPUSpeedEx"));
        probe_Connect              = reinterpret_cast<decltype(probe_Connect)>(m_Library->resolve("JLINKARM_Open"));
        probe_ConnectEx            = reinterpret_cast<decltype(probe_ConnectEx)>(m_Library->resolve("JLINKARM_OpenEx"));
        target_ReadCodeMemory      = reinterpret_cast<decltype(target_ReadCodeMemory)>(m_Library->resolve("JLINKARM_ReadCodeMem"));
        target_Read_ICE_Register   = reinterpret_cast<decltype(target_Read_ICE_Register)>(m_Library->resolve("JLINKARM_ReadICEReg"));
        target_ReadMemory          = reinterpret_cast<decltype(target_ReadMemory)>(m_Library->resolve("JLINKARM_ReadMem"));
        target_ReadMemoryEx        = reinterpret_cast<decltype(target_ReadMemoryEx)>(m_Library->resolve("JLINKARM_ReadMemEx"));
        target_ReadMemoryImmediate = reinterpret_cast<decltype(target_ReadMemoryImmediate)>(m_Library->resolve("JLINKARM_ReadMemHW"));
        target_ReadMemoryIndirect  = reinterpret_cast<decltype(target_ReadMemoryIndirect)>(m_Library->resolve("JLINKARM_ReadMemIndirect"));
        target_ReadMemory_8        = reinterpret_cast<decltype(target_ReadMemory_8)>(m_Library->resolve("JLINKARM_ReadMemU8"));
        target_ReadMemory_16       = reinterpret_cast<decltype(target_ReadMemory_16)>(m_Library->resolve("JLINKARM_ReadMemU16"));
        target_ReadMemory_32       = reinterpret_cast<decltype(target_ReadMemory_32)>(m_Library->resolve("JLINKARM_ReadMemU32"));
        target_ReadMemory_64       = reinterpret_cast<decltype(target_ReadMemory_64)>(m_Library->resolve("JLINKARM_ReadMemU64"));
        target_ReadMemoryZonedEx   = reinterpret_cast<decltype(target_ReadMemoryZonedEx)>(m_Library->resolve("JLINK_ReadMemZonedEx"));
        target_ReadRegister        = reinterpret_cast<decltype(target_ReadRegister)>(m_Library->resolve("JLINKARM_ReadReg"));
        target_ReadRegisters       = reinterpret_cast<decltype(target_ReadRegisters)>(m_Library->resolve("JLINKARM_ReadRegs"));
        target_Reset               = reinterpret_cast<decltype(target_Reset)>(m_Library->resolve("JLINKARM_Reset"));
        target_ResetAndRun         = reinterpret_cast<decltype(target_ResetAndRun)>(m_Library->resolve("JLINKARM_ResetNoHalt"));
        target_SelectTraceSource   = reinterpret_cast<decltype(target_SelectTraceSource)>(m_Library->resolve("JLINKARM_SelectTraceSource"));
        target_SetBreakpoint       = reinterpret_cast<decltype(target_SetBreakpoint)>(m_Library->resolve("JLINKARM_SetBP"));
        target_SetBreakpointEx     = reinterpret_cast<decltype(target_SetBreakpointEx)>(m_Library->resolve("JLINKARM_SetBPEx"));
        target_SetDataEvent        = reinterpret_cast<decltype(target_SetDataEvent)>(m_Library->resolve("JLINKARM_SetDataEvent"));
        target_SetEndiannes        = reinterpret_cast<decltype(target_SetEndiannes)>(m_Library->resolve("JLINKARM_SetEndian"));
        probe_SetErrorCallback     = reinterpret_cast<decltype(probe_SetErrorCallback)>(m_Library->resolve("JLINKARM_SetErrorOutHandler"));
        probe_SetHookUnsecureDialog =
            reinterpret_cast<decltype(probe_SetHookUnsecureDialog)>(m_Library->resolve("JLINK_SetHookUnsecureDialog"));
        target_SetInitializeRegistersOnReset =
            reinterpret_cast<decltype(target_SetInitializeRegistersOnReset)>(m_Library->resolve("JLINKARM_SetInitRegsOnReset"));
        target_SetMaxInterfaceSpeed = reinterpret_cast<decltype(target_SetMaxInterfaceSpeed)>(m_Library->resolve("JLINKARM_SetMaxSpeed"));
        target_SetResetPin          = reinterpret_cast<decltype(target_SetResetPin)>(m_Library->resolve("JLINKARM_SetRESET"));
        target_SetResetDelay        = reinterpret_cast<decltype(target_SetResetDelay)>(m_Library->resolve("JLINKARM_SetResetDelay"));
        target_SetResetType         = reinterpret_cast<decltype(target_SetResetType)>(m_Library->resolve("JLINKARM_SetResetType"));
        target_SetInterfaceSpeed    = reinterpret_cast<decltype(target_SetInterfaceSpeed)>(m_Library->resolve("JLINKARM_SetSpeed"));
        probe_SetWaitFunction       = reinterpret_cast<decltype(probe_SetWaitFunction)>(m_Library->resolve("JLINKARM_SetWaitFunction"));
        probe_SetWarningCallback = reinterpret_cast<decltype(probe_SetWarningCallback)>(m_Library->resolve("JLINKARM_SetWarnOutHandler"));
        target_SetWatchpoint     = reinterpret_cast<decltype(target_SetWatchpoint)>(m_Library->resolve("JLINKARM_SetWP"));
        target_SimulateInstruction =
            reinterpret_cast<decltype(target_SimulateInstruction)>(m_Library->resolve("JLINKARM_SimulateInstruction"));
        target_Step               = reinterpret_cast<decltype(target_Step)>(m_Library->resolve("JLINKARM_Step"));
        library_Unlock            = reinterpret_cast<decltype(library_Unlock)>(m_Library->resolve("JLINKARM_Unlock"));
        probe_UpdateProbeFirmware = reinterpret_cast<decltype(probe_UpdateProbeFirmware)>(m_Library->resolve("JLINKARM_UpdateFirmware"));
        probe_UpdateProbeFirmwareIfNewer =
            reinterpret_cast<decltype(probe_UpdateProbeFirmwareIfNewer)>(m_Library->resolve("JLINKARM_UpdateFirmwareIfNewer"));
        target_WaitForHalt          = reinterpret_cast<decltype(target_WaitForHalt)>(m_Library->resolve("JLINKARM_WaitForHalt"));
        target_Write_ICE_Register   = reinterpret_cast<decltype(target_Write_ICE_Register)>(m_Library->resolve("JLINKARM_WriteICEReg"));
        target_WriteProgram         = reinterpret_cast<decltype(target_WriteProgram)>(m_Library->resolve("JLINKARM_WriteMem"));
        target_WriteMemoryDelayed   = reinterpret_cast<decltype(target_WriteMemoryDelayed)>(m_Library->resolve("JLINKARM_WriteMemDelayed"));
        target_WriteMemoryEx        = reinterpret_cast<decltype(target_WriteMemoryEx)>(m_Library->resolve("JLINKARM_WriteMemEx"));
        target_WriteMemoryImmediate = reinterpret_cast<decltype(target_WriteMemoryImmediate)>(m_Library->resolve("JLINKARM_WriteMemHW"));
        target_WriteMemoryDescriptor =
            reinterpret_cast<decltype(target_WriteMemoryDescriptor)>(m_Library->resolve("JLINKARM_WriteMemMultiple"));
        target_WriteRegister      = reinterpret_cast<decltype(target_WriteRegister)>(m_Library->resolve("JLINKARM_WriteReg"));
        target_WriteRegisters     = reinterpret_cast<decltype(target_WriteRegisters)>(m_Library->resolve("JLINKARM_WriteRegs"));
        target_WriteMemory_8      = reinterpret_cast<decltype(target_WriteMemory_8)>(m_Library->resolve("JLINKARM_WriteU8"));
        target_WriteMemory_16     = reinterpret_cast<decltype(target_WriteMemory_16)>(m_Library->resolve("JLINKARM_WriteU16"));
        target_WriteMemory_32     = reinterpret_cast<decltype(target_WriteMemory_32)>(m_Library->resolve("JLINKARM_WriteU32"));
        target_WriteMemory_64     = reinterpret_cast<decltype(target_WriteMemory_64)>(m_Library->resolve("JLINKARM_WriteU64"));
        target_WriteMemoryZonedEx = reinterpret_cast<decltype(target_WriteMemoryZonedEx)>(m_Library->resolve("JLINK_WriteMemZonedEx"));
        target_WriteVectorCatch   = reinterpret_cast<decltype(target_WriteVectorCatch)>(m_Library->resolve("JLINKARM_WriteVectorCatch"));
        target_WriteVerifyMemory  = reinterpret_cast<decltype(target_WriteVerifyMemory)>(m_Library->resolve("JLINKARM_WriteVerifyMem"));
        target_GetDeviceCore      = reinterpret_cast<decltype(target_GetDeviceCore)>(m_Library->resolve("JLINKARM_CORE_GetFound"));
        target_CoreSight_Configure =
            reinterpret_cast<decltype(target_CoreSight_Configure)>(m_Library->resolve("JLINKARM_CORESIGHT_Configure"));
        target_CoreSight_ReadAPDPReg =
            reinterpret_cast<decltype(target_CoreSight_ReadAPDPReg)>(m_Library->resolve("JLINKARM_CORESIGHT_ReadAPDPReg"));
        target_CoreSight_WriteAPDP =
            reinterpret_cast<decltype(target_CoreSight_WriteAPDP)>(m_Library->resolve("JLINKARM_CORESIGHT_WriteAPDPReg"));
        target_CP15_IsPresent     = reinterpret_cast<decltype(target_CP15_IsPresent)>(m_Library->resolve("JLINKARM_CP15_IsPresent"));
        target_CP15_ReadEx        = reinterpret_cast<decltype(target_CP15_ReadEx)>(m_Library->resolve("JLINKARM_CP15_ReadEx"));
        target_CP15_ReadRegister  = reinterpret_cast<decltype(target_CP15_ReadRegister)>(m_Library->resolve("JLINKARM_CP15_ReadReg"));
        target_CP15_WriteEx       = reinterpret_cast<decltype(target_CP15_WriteEx)>(m_Library->resolve("JLINKARM_CP15_WriteEx"));
        target_CP15_WriteRegister = reinterpret_cast<decltype(target_CP15_WriteRegister)>(m_Library->resolve("JLINKARM_CP15_WriteReg"));
        probe_AddLicense          = reinterpret_cast<decltype(probe_AddLicense)>(m_Library->resolve("JLINK_EMU_AddLicense"));
        probe_EraseLicenses       = reinterpret_cast<decltype(probe_EraseLicenses)>(m_Library->resolve("JLINK_EMU_EraseLicenses"));
        probe_GetLicenses         = reinterpret_cast<decltype(probe_GetLicenses)>(m_Library->resolve("JLINK_EMU_GetLicenses"));
        probe_GetCounters         = reinterpret_cast<decltype(probe_GetCounters)>(m_Library->resolve("JLINKARM_EMU_GetCounters"));
        probe_GetDeviceList       = reinterpret_cast<decltype(probe_GetDeviceList)>(m_Library->resolve("JLINKARM_EMU_GetList"));
        probe_GetConnectedDeviceCount =
            reinterpret_cast<decltype(probe_GetConnectedDeviceCount)>(m_Library->resolve("JLINKARM_EMU_GetNumDevices"));
        probe_IsCapableOfEx = reinterpret_cast<decltype(probe_IsCapableOfEx)>(m_Library->resolve("JLINKARM_EMU_HasCapEx"));
        probe_IsConnected   = reinterpret_cast<decltype(probe_IsConnected)>(m_Library->resolve("JLINKARM_EMU_IsConnected"));
        probe_SelectBySerialNumber_USB =
            reinterpret_cast<decltype(probe_SelectBySerialNumber_USB)>(m_Library->resolve("JLINKARM_EMU_SelectByUSBSN"));
        probe_SelectByIP = reinterpret_cast<decltype(probe_SelectByIP)>(m_Library->resolve("JLINKARM_EMU_SelectIP"));
        probe_SelectBySerialNumber_Ethernet =
            reinterpret_cast<decltype(probe_SelectBySerialNumber_Ethernet)>(m_Library->resolve("JLINKARM_EMU_SelectIPBySN"));
        target_ETB_IsPresent      = reinterpret_cast<decltype(target_ETB_IsPresent)>(m_Library->resolve("JLINKARM_ETB_IsPresent"));
        target_ETB_ReadRegister   = reinterpret_cast<decltype(target_ETB_ReadRegister)>(m_Library->resolve("JLINKARM_ETB_ReadReg"));
        target_ETB_WriteRegister  = reinterpret_cast<decltype(target_ETB_WriteRegister)>(m_Library->resolve("JLINKARM_ETB_WriteReg"));
        target_ETM_IsPresent      = reinterpret_cast<decltype(target_ETM_IsPresent)>(m_Library->resolve("JLINKARM_ETM_IsPresent"));
        target_ETM_ReadRegister   = reinterpret_cast<decltype(target_ETM_ReadRegister)>(m_Library->resolve("JLINKARM_ETM_ReadReg"));
        target_ETM_StartTrace     = reinterpret_cast<decltype(target_ETM_StartTrace)>(m_Library->resolve("JLINKARM_ETM_StartTrace"));
        target_ETM_WriteRegister  = reinterpret_cast<decltype(target_ETM_WriteRegister)>(m_Library->resolve("JLINKARM_ETM_WriteReg"));
        probe_HSS_GetCapabilities = reinterpret_cast<decltype(probe_HSS_GetCapabilities)>(m_Library->resolve("JLINK_HSS_GetCaps"));
        target_HSS_Start          = reinterpret_cast<decltype(target_HSS_Start)>(m_Library->resolve("JLINK_HSS_Start"));
        target_HSS_Stop           = reinterpret_cast<decltype(target_HSS_Stop)>(m_Library->resolve("JLINK_HSS_Stop"));
        target_HSS_Read           = reinterpret_cast<decltype(target_HSS_Read)>(m_Library->resolve("JLINK_HSS_Read"));
        target_RawTrace_Control   = reinterpret_cast<decltype(target_RawTrace_Control)>(m_Library->resolve("JLINKARM_RAWTRACE_Control"));
        target_RawTrace_Read      = reinterpret_cast<decltype(target_RawTrace_Read)>(m_Library->resolve("JLINKARM_RAWTRACE_Read"));
        target_RTT_Control        = reinterpret_cast<decltype(target_RTT_Control)>(m_Library->resolve("JLINK_RTTERMINAL_Control"));
        target_RTT_Read           = reinterpret_cast<decltype(target_RTT_Read)>(m_Library->resolve("JLINK_RTTERMINAL_Read"));
        target_RTT_Write          = reinterpret_cast<decltype(target_RTT_Write)>(m_Library->resolve("JLINK_RTTERMINAL_Write"));
        target_STrace_Config      = reinterpret_cast<decltype(target_STrace_Config)>(m_Library->resolve("JLINK_STRACE_Config"));
        target_STrace_Control     = reinterpret_cast<decltype(target_STrace_Control)>(m_Library->resolve("JLINK_STRACE_Control"));
        target_STrace_Read        = reinterpret_cast<decltype(target_STrace_Read)>(m_Library->resolve("JLINK_STRACE_Read"));
        target_STrace_GetInstStats =
            reinterpret_cast<decltype(target_STrace_GetInstStats)>(m_Library->resolve("JLINK_STRACE_GetInstStats"));
        target_STrace_Start    = reinterpret_cast<decltype(target_STrace_Start)>(m_Library->resolve("JLINK_STRACE_Start"));
        target_STrace_Stop     = reinterpret_cast<decltype(target_STrace_Stop)>(m_Library->resolve("JLINK_STRACE_Stop"));
        target_SWD_GetData     = reinterpret_cast<decltype(target_SWD_GetData)>(m_Library->resolve("JLINK_SWD_GetData"));
        target_SWD_GetData_8   = reinterpret_cast<decltype(target_SWD_GetData_8)>(m_Library->resolve("JLINK_SWD_GetU8"));
        target_SWD_GetData_16  = reinterpret_cast<decltype(target_SWD_GetData_16)>(m_Library->resolve("JLINK_SWD_GetU16"));
        target_SWD_GetData_32  = reinterpret_cast<decltype(target_SWD_GetData_32)>(m_Library->resolve("JLINK_SWD_GetU32"));
        target_SWD_StoreGetRaw = reinterpret_cast<decltype(target_SWD_StoreGetRaw)>(m_Library->resolve("JLINK_SWD_StoreGetRaw"));
        target_SWD_StoreRaw    = reinterpret_cast<decltype(target_SWD_StoreRaw)>(m_Library->resolve("JLINK_SWD_StoreRaw"));
        target_SWD_SyncBits    = reinterpret_cast<decltype(target_SWD_SyncBits)>(m_Library->resolve("JLINK_SWD_SyncBits"));
        target_SWD_SyncBytes   = reinterpret_cast<decltype(target_SWD_SyncBytes)>(m_Library->resolve("JLINK_SWD_SyncBytes"));
        target_SWO_Control     = reinterpret_cast<decltype(target_SWO_Control)>(m_Library->resolve("JLINKARM_SWO_Control"));
        target_SWO_Disable     = reinterpret_cast<decltype(target_SWO_Disable)>(m_Library->resolve("JLINKARM_SWO_DisableTarget"));
        target_SWO_Enable      = reinterpret_cast<decltype(target_SWO_Enable)>(m_Library->resolve("JLINKARM_SWO_EnableTarget"));
        target_SWO_GetCompatibleSpeeds =
            reinterpret_cast<decltype(target_SWO_GetCompatibleSpeeds)>(m_Library->resolve("JLINKARM_SWO_GetCompatibleSpeeds"));
        target_SWO_Read         = reinterpret_cast<decltype(target_SWO_Read)>(m_Library->resolve("JLINKARM_SWO_Read"));
        target_SWO_ReadStimulus = reinterpret_cast<decltype(target_SWO_ReadStimulus)>(m_Library->resolve("JLINKARM_SWO_ReadStimulus"));
        target_GetAvailableInterfaces =
            reinterpret_cast<decltype(target_GetAvailableInterfaces)>(m_Library->resolve("JLINKARM_TIF_GetAvailable"));
        target_SelectInterface = reinterpret_cast<decltype(target_SelectInterface)>(m_Library->resolve("JLINKARM_TIF_Select"));
        target_Trace_Control   = reinterpret_cast<decltype(target_Trace_Control)>(m_Library->resolve("JLINKARM_TRACE_Control"));
        target_Trace_Read      = reinterpret_cast<decltype(target_Trace_Read)>(m_Library->resolve("JLINKARM_TRACE_Read"));
        probe_SetFlashProgProgressCallback =
            reinterpret_cast<decltype(probe_SetFlashProgProgressCallback)>(m_Library->resolve("JLINK_SetFlashProgProgressCallback"));
    }

} // namespace L0::Probe::Driver
