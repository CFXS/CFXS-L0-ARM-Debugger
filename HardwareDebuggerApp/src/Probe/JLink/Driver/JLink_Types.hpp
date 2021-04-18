#pragma once
#include <Utils.hpp>

namespace HWD::Probe::Driver::JLink_Types {

    using FlashProgressCallback = void (*)(const char* action, const char* prog, int percentage);
    using LogCallback           = void (*)(const char* str);
    using UnsecureDialogHook    = int (*)(const char* title, const char* message, uint32_t flags);
    using WaitFunction          = void (*)(void* context);

    namespace Commands {
        static constexpr const char* ENABLE_FLASH_PROGRESS_POPUP  = "EnableInfoWinFlashDL\n";
        static constexpr const char* DISABLE_FLASH_PROGRESS_POPUP = "DisableInfoWinFlashDL\n";
    } // namespace Commands

    enum class Emulator_PID {
        UNKNOWN            = 0x00,
        JLINK              = 0x01,
        JLINK_CF           = 0x02,
        JLINK_CE           = 0x03,
        JLINK_KS           = 0x04,
        DIGI_LINK          = 0x05,
        MIDAS              = 0x06,
        SAMICE             = 0x07,
        JTRACE             = 0x08,
        JTRACE_CS          = 0x09,
        FLASHER_ARM        = 0x0A,
        JLINK_PRO          = 0x0B,
        JLINK_EDU          = 0x0C,
        JLINK_ULTRA        = 0x0D,
        FLASHER_PPC        = 0x0E,
        FLASHER_RX         = 0x0F,
        JLINK_OB_RX200     = 0x10,
        JLINK_LITE         = 0x11,
        JLINK_OB_SAM3U128  = 0x12,
        JLINK_LITE_CORTEXM = 0x13,
        JLINK_LITE_LPC     = 0x14,
        JLINK_LITE_STM32   = 0x15,
        JLINK_LITE_FSL     = 0x16,
        JLINK_LITE_ADI     = 0x17,
        ENERGY_MICRO_EFM32 = 0x18,
        JLINK_LITE_XMC4000 = 0x19,
        JLINK_LITE_XMC4200 = 0x20,
        JLINK_LPC_LINK2    = 0x21,
        FLASHER_PRO        = 0x22
    };

    enum class ErrorCode : int {
        OK                           = 0,
        PROBE_NO_CONNECTION          = -256,
        PROBE_COM_ERROR              = -257,
        DLL_NOT_LOADED               = -258,
        TARGET_NOT_POWERED           = -259,
        INVALID_HANDLE               = -260,
        NO_CPU_FOUND                 = -261,
        PROBE_FEATURE_NOT_SUPPORTED  = -262,
        PROBE_NO_MEMORY              = -263,
        TIF_STATUS_ERROR             = -264,
        FLASH_PROG_COMPARE_FAILED    = -265,
        FLASH_PROG_PROGRAM_FAILED    = -266,
        FLASH_PROG_VERIFY_FAILED     = -267,
        OPEN_FILE_FAILED             = -268,
        UNKNOWN_FILE_FORMAT          = -269,
        WRITE_TARGET_MEMORY_FAILED   = -270,
        DEVICE_FEATURE_NOT_SUPPORTED = -271,
        BAD_USER_CONFIG              = -272,
        NO_TARGET_DEVICE_SELECTED    = -273,
        CPU_IN_LOW_POWER_MODE        = -274,
        NO_CPU_CLOCK                 = -2147483647,
        NO_CPU_POWER                 = -2147483646,
        GENERIC_ERROR                = -1,
        SPECIFIC_ERROR               = -2,
        NO_FLASH_BANK                = -3,
        PROGRAM_TOO_BIG              = -4,
    };

    enum class FlashErrorCode : int {
        BLOCK_VERIFICATION_ERROR = 1,
        ITEM_VERIFICATION_ERROR  = 2,
        TIMEOUT                  = 3,
        PROGRAM_ERROR            = 4,
        PROGRAM_1_OVER_0         = 5,
        SECTOR_LOCKED            = 6,
        ERASE_ERROR              = 7,
        NO_FLASH_MEMORY          = 8,
        ERR                      = 9,
    };

    inline const char* ErrorCodeToString(ErrorCode ec) {
        switch (ec) {
            case ErrorCode::OK: return "No error";
            case ErrorCode::PROBE_NO_CONNECTION: return "Probe not connected";
            case ErrorCode::GENERIC_ERROR: return "Error";
            case ErrorCode::PROBE_COM_ERROR: return "Probe communication error";
            case ErrorCode::DLL_NOT_LOADED: return "Library not laoded";
            case ErrorCode::TARGET_NOT_POWERED: return "Target not powered";
            case ErrorCode::INVALID_HANDLE: return "Invalid handle";
            case ErrorCode::NO_CPU_FOUND: return "No CPU found";
            case ErrorCode::PROBE_FEATURE_NOT_SUPPORTED: return "Probe feature not supported";
            case ErrorCode::PROBE_NO_MEMORY: return "Probe out of memory";
            case ErrorCode::TIF_STATUS_ERROR: return "Target interface status error";
            case ErrorCode::FLASH_PROG_COMPARE_FAILED: return "Flash program compare failed";
            case ErrorCode::FLASH_PROG_PROGRAM_FAILED: return "Flash program write failed";
            case ErrorCode::FLASH_PROG_VERIFY_FAILED: return "Flash program verify failed";
            case ErrorCode::OPEN_FILE_FAILED: return "Failed to open file";
            case ErrorCode::UNKNOWN_FILE_FORMAT: return "Unknown file format";
            case ErrorCode::WRITE_TARGET_MEMORY_FAILED: return "Failed to write target memory";
            case ErrorCode::DEVICE_FEATURE_NOT_SUPPORTED: return "Device feature not supported";
            case ErrorCode::BAD_USER_CONFIG: return "Bad user config";
            case ErrorCode::NO_TARGET_DEVICE_SELECTED: return "No target device selected";
            case ErrorCode::CPU_IN_LOW_POWER_MODE: return "CPU in low power mode";
            case ErrorCode::NO_CPU_CLOCK: return "No CPU clock";
            case ErrorCode::NO_CPU_POWER: return "No CPU power";
            case ErrorCode::SPECIFIC_ERROR: return "Error";
            case ErrorCode::NO_FLASH_BANK: return "No flash bank";
            case ErrorCode::PROGRAM_TOO_BIG: return "Program too big";
            default: return "Unknown error";
        };
    }

    inline const char* FlashErrorCodeToString(FlashErrorCode fec) {
        switch (fec) {
            case FlashErrorCode::BLOCK_VERIFICATION_ERROR: return "Block verification failed";
            case FlashErrorCode::ITEM_VERIFICATION_ERROR: return "Item verification failed";
            case FlashErrorCode::TIMEOUT: return "Timed out";
            case FlashErrorCode::PROGRAM_ERROR: return "Program error";
            case FlashErrorCode::PROGRAM_1_OVER_0: return "Program error 1/0";
            case FlashErrorCode::SECTOR_LOCKED: return "Sector locked";
            case FlashErrorCode::ERASE_ERROR: return "Erase failed";
            case FlashErrorCode::NO_FLASH_MEMORY: return "No flash memory";
            case FlashErrorCode::ERR: return "Error";
            default: return "Unknown error";
        };
    }

    enum class PinStatus {
        LOW     = 0,
        HIGH    = 1,
        UNKNOWN = 255,
    };

    enum class PinOverrideMode {
        RELEASE      = 0,
        PIO_IN       = 1,
        PIO_OUT_LOW  = 2,
        PIO_OUT_HIGH = 3,
        UART_TX      = 4,
        UART_RX      = 5,
        UART_RXTX    = 6,
    };

    enum class TargetInterface : uint32_t {
        JTAG = 0,
        SWD  = 1,
    };

    enum class TargetInterfaceMask : uint32_t {
        JTAG = 1 << 0,
        SWD  = 1 << 1,
    };
    HWD_OVERLOAD_ENUM_BITWISE_OPERATORS(TargetInterfaceMask, uint32_t)

    static constexpr auto HSS_FLAG_TIMESTAMP_US = 1UL << 0;

    enum class STRACE_Event {
        SET             = 0,
        CLEAR           = 1,
        CLEAR_ALL       = 2,
        SET_BUFFER_SIZE = 3,
    };

    enum class STRACE_EventType : uint8_t {
        CODE_FETCH = 0,
        DATA_ACC   = 1,
        DATA_LOAD  = 2,
        DATA_STORE = 3,
    };

    enum class STRACE_Operation : uint8_t {
        START         = 0,
        STOP          = 1,
        INCLUDE_RANGE = 2,
        EXCLUDE_RANGE = 3,
    };

    enum class DeviceFamily : int {
        AUTO      = 0,
        CM1       = 1,  // Cortex-M1
        CF        = 2,  // ColdFire
        CM3       = 3,  // Cortex-M3
        SIM       = 4,  // Simulator
        XSCALE    = 5,  // XScale
        CM0       = 6,  // Cortex-M0
        ARM7      = 7,  // ARM7
        CORTEX_A8 = 8,  // Cortex-A8/A9
        ARM9      = 9,  // ARM9
        ARM10     = 10, // ARM10
        ARM11     = 11, // ARM11
        CORTEX_R4 = 12, // Cortex-R4
        RX        = 13, // Renesas RX
        CM4       = 14, // Cortex-M4
        CORTEX_A5 = 15, // Cortex-A5
        POWER_PC  = 16,
        MIPS      = 17,
        EFM8      = 18,
        ANY       = 255,
    };

    enum class DeviceCore : uint32_t {
        NONE           = 0x00000000,
        ANY            = 0xFFFFFFFF,
        CORTEX_M1      = 0x010000FF,
        COLDFIRE       = 0x02FFFFFF,
        CORTEX_M3      = 0x030000FF,
        CORTEX_M3_R1P0 = 0x03000010,
        CORTEX_M3_R1P1 = 0x03000011,
        CORTEX_M3_R2P0 = 0x03000020,
        CORTEX_M3_R2P1 = 0x03000021,
        SIM            = 0x04FFFFFF,
        XSCALE         = 0x05FFFFFF,
        CORTEX_M0      = 0x060000FF,
        CORTEX_M23     = 0x060100FF,
        ARM7           = 0x07FFFFFF,
        ARM7TDMI       = 0x070000FF,
        ARM7TDMI_R3    = 0x0700003F,
        ARM7TDMI_R4    = 0x0700004F,
        ARM7TDMI_S     = 0x070001FF,
        ARM7TDMI_S_R3  = 0x0700013F,
        ARM7TDMI_S_R4  = 0x0700014F,
        CORTEX_A8      = 0x080000FF,
        CORTEX_A7      = 0x080800FF,
        CORTEX_A9      = 0x080900FF,
        CORTEX_A12     = 0x080A00FF,
        CORTEX_A15     = 0x080B00FF,
        CORTEX_A17     = 0x080C00FF,
        ARM9           = 0x09FFFFFF,
        ARM9TDMI_S     = 0x090001FF,
        ARM920T        = 0x092000FF,
        ARM922T        = 0x092200FF,
        ARM926EJ_S     = 0x092601FF,
        ARM946E_S      = 0x094601FF,
        ARM966E_S      = 0x096601FF,
        ARM968E_S      = 0x096801FF,
        ARM11          = 0x0BFFFFFF,
        ARM1136        = 0x0B36FFFF,
        ARM1136J       = 0x0B3602FF,
        ARM1136J_S     = 0x0B3603FF,
        ARM1136JF      = 0x0B3606FF,
        ARM1136JF_S    = 0x0B3607FF,
        ARM1156        = 0x0B56FFFF,
        ARM1176        = 0x0B76FFFF,
        ARM1176J       = 0x0B7602FF,
        ARM1176J_S     = 0x0B7603FF,
        ARM1176JF      = 0x0B7606FF,
        ARM1176JF_S    = 0x0B7607FF,
        CORTEX_R4      = 0x0C0000FF,
        CORTEX_R5      = 0x0C0100FF,
        CORTEX_R8      = 0x0C0200FF,
        CORTEX_M4      = 0x0E0000FF,
        CORTEX_M7      = 0x0E0100FF,
        CORTEX_M33     = 0x0E0200FF,
        CORTEX_A5      = 0x0F0000FF,
    };

    inline const char* DeviceCoreToString(DeviceCore core) {
        switch (core) {
            case DeviceCore::NONE: return "None";
            case DeviceCore::ANY: return "Any";
            case DeviceCore::CORTEX_M1: return "Cortex M1";
            case DeviceCore::COLDFIRE: return "ColdFire";
            case DeviceCore::CORTEX_M3: return "Cortex M3";
            case DeviceCore::CORTEX_M3_R1P0: return "Cortex M3 r1p0";
            case DeviceCore::CORTEX_M3_R1P1: return "Cortex M3 r1p1";
            case DeviceCore::CORTEX_M3_R2P0: return "Cortex M3 r2p0";
            case DeviceCore::CORTEX_M3_R2P1: return "Cortex M3 r2p1";
            case DeviceCore::SIM: return "Sim";
            case DeviceCore::XSCALE: return "XScale";
            case DeviceCore::CORTEX_M0: return "Cortex M0";
            case DeviceCore::CORTEX_M23: return "Cortex M23";
            case DeviceCore::ARM7: return "ARM7";
            case DeviceCore::ARM7TDMI: return "ARM7TDMI";
            case DeviceCore::ARM7TDMI_R3: return "ARM7TDMI R3";
            case DeviceCore::ARM7TDMI_R4: return "ARM7TDMI R4";
            case DeviceCore::ARM7TDMI_S: return "ARM7TDMI S";
            case DeviceCore::ARM7TDMI_S_R3: return "ARM7TDMI S R3";
            case DeviceCore::ARM7TDMI_S_R4: return "ARM7TDMI S R4";
            case DeviceCore::CORTEX_A8: return "Cortex A8";
            case DeviceCore::CORTEX_A7: return "Cortex A7";
            case DeviceCore::CORTEX_A9: return "Cortex A9";
            case DeviceCore::CORTEX_A12: return "Cortex A12";
            case DeviceCore::CORTEX_A15: return "Cortex A15";
            case DeviceCore::CORTEX_A17: return "Cortex A17";
            case DeviceCore::ARM9: return "ARM9";
            case DeviceCore::ARM9TDMI_S: return "ARM9TDMIS";
            case DeviceCore::ARM920T: return "ARM920T";
            case DeviceCore::ARM922T: return "ARM922T";
            case DeviceCore::ARM926EJ_S: return "ARM926EJ S";
            case DeviceCore::ARM946E_S: return "ARM946E S";
            case DeviceCore::ARM966E_S: return "ARM966E S";
            case DeviceCore::ARM968E_S: return "ARM968E S";
            case DeviceCore::ARM11: return "ARM11";
            case DeviceCore::ARM1136: return "ARM1136";
            case DeviceCore::ARM1136J: return "ARM1136J";
            case DeviceCore::ARM1136J_S: return "ARM1136J S";
            case DeviceCore::ARM1136JF: return "ARM1136JF";
            case DeviceCore::ARM1136JF_S: return "ARM1136JF S";
            case DeviceCore::ARM1156: return "ARM1156";
            case DeviceCore::ARM1176: return "ARM1176";
            case DeviceCore::ARM1176J: return "ARM1176J";
            case DeviceCore::ARM1176J_S: return "ARM1176J S";
            case DeviceCore::ARM1176JF: return "ARM1176JF";
            case DeviceCore::ARM1176JF_S: return "ARM1176JF S";
            case DeviceCore::CORTEX_R4: return "Cortex R4";
            case DeviceCore::CORTEX_R5: return "Cortex R5";
            case DeviceCore::CORTEX_R8: return "Cortex R8";
            case DeviceCore::CORTEX_M4: return "Cortex M4";
            case DeviceCore::CORTEX_M7: return "Cortex M7";
            case DeviceCore::CORTEX_M33: return "Cortex M33";
            case DeviceCore::CORTEX_A5: return "Cortex A5";
            default: return "Unknown";
        }
    }

    namespace CPU_Capacilities {
        static constexpr uint32_t RESERVED   = 1 << 0;
        static constexpr uint32_t READ_MEM   = 1 << 1;
        static constexpr uint32_t WRITE_MEM  = 1 << 2;
        static constexpr uint32_t READ_REGS  = 1 << 3;
        static constexpr uint32_t WRITE_REGS = 1 << 4;
        static constexpr uint32_t GO         = 1 << 5;
        static constexpr uint32_t STEP       = 1 << 6;
        static constexpr uint32_t HALT       = 1 << 7;
        static constexpr uint32_t IS_HALTED  = 1 << 8;
        static constexpr uint32_t RESET      = 1 << 9;
        static constexpr uint32_t RUN_STOP   = 1 << 10;
        static constexpr uint32_t TERMINAL   = 1 << 11;
        static constexpr uint32_t DCC        = 1 << 14;
        static constexpr uint32_t HSS        = 1 << 15;
    } // namespace CPU_Capacilities

    enum class ProbeCapabilities : uint32_t {
        RESERVED          = 1 << 0,
        GET_HW_VERSION    = 1 << 1,    // can get hardware version from host
        ADAPTIVE_CLOCKING = 1 << 3,    // adaptive clock mode
        TRACE_ARM79       = 1 << 6,    // supports ARM7/9 trace
        WRITE_MEM_ARM79   = 1 << 7,    // can auto write ARM7/9 memory
        READ_MEM_ARM79    = 1 << 8,    // can auto read ARM7/9 memory
        SPEED_INFO        = 1 << 9,    // can get target supported speeds
        RESET_STOP_TIMED  = 1 << 14,   // can reset and halt
        GET_LICENSE_INFO  = 1 << 15,   // can get license info
        SELECT_IF         = 1 << 17,   // can select target interface
        RW_MEM_ARM79      = 1 << 18,   // can auto read/write ARM7/9 memory
        SWO               = 1 << 23,   // supports SWO
        RAWTRACE          = 1 << 30,   // supports rawtrace
        GET_CAPS_EX       = 1ul << 31, // has extended capabilities
    };
    HWD_OVERLOAD_ENUM_BITWISE_OPERATORS(ProbeCapabilities, uint32_t)

    enum class ProbeExtendedCapabilities : uint8_t {
        RESERVED                  = 0,
        GET_HW_VERSION            = 1,
        WRITE_DCC                 = 2,
        ADAPTIVE_CLOCKING         = 3,
        READ_CONFIG               = 4,
        WRITE_CONFIG              = 5,
        TRACE_ARM79               = 6,
        WRITE_MEM                 = 7,
        READ_MEM                  = 8,
        SPEED_INFO                = 9,
        EXEC_CODE                 = 10,
        GET_MAX_BLOCK_SIZE        = 11,
        GET_HW_INFO               = 12,
        SET_KS_POWER              = 13,
        RESET_STOP_TIMED          = 14,
        GET_LICENSE_INFO          = 15,
        MEASURE_RTCK_REACT        = 16,
        SELECT_IF                 = 17,
        RW_MEM_ARM79              = 18,
        GET_COUNTERS              = 19,
        READ_DCC                  = 20,
        GET_CPU_CAPS              = 21,
        EXEC_CPU_CMD              = 22,
        SWO                       = 23,
        WRITE_DCC_EX              = 24,
        UPDATE_FIRMWARE_EX        = 25,
        FILE_IO                   = 26,
        REGISTER                  = 27,
        INDICATORS                = 28,
        TEST_NET_SPEED            = 29,
        RAWTRACE                  = 30,
        GET_CAPS_EX               = 31,
        HW_JTAG_WRITE             = 32,
        EMUCOM                    = 33,
        EXEC_CPU2_CMD             = 34,
        GET_CPU2_CAPS             = 35,
        TEST_NET                  = 36,
        POWERTRACE                = 37,
        ETHERNET                  = 38,
        HW_SET_SWDIO_DIR          = 39,
        HW_ENABLE_DISABLE_SWCLK   = 40,
        HW_ENABLE_DISABLE_JTAG_IF = 41,
        HW_SET_CLEAR_TCK          = 42,
        PCODE_EXEC                = 44,
        CDC_EXEC                  = 48,
        CDC_SET_HOOK_FUNCS        = 49,
        HANDLE_BMI                = 50,
        HANDLE_GPIO               = 51,
        HSS                       = 53,
        FILE_IO_EX                = 54,
        SPI                       = 56,
        SUPPORT_OTS_CONFIG_X      = 57,
        HANDLE_SECURITY           = 58,
        PIN_OVERRIDE              = 59,
        WINUSB                    = 61,
    };

    enum class HostInterfaceType : int {
        USB      = 1 << 0,
        ETHERNET = 1 << 1,
    };
    HWD_OVERLOAD_ENUM_BITWISE_OPERATORS(HostInterfaceType, int)

    namespace Breakpoint {

        namespace Mode {
            static constexpr uint32_t MODE_0 = 0;
            static constexpr uint32_t MODE_1 = 1;
            static constexpr uint32_t MODE_2 = 2;
            static constexpr uint32_t MODE_3 = 3;
            static constexpr uint32_t MASK   = 0x0F;
        } // namespace Mode

        enum Type : uint32_t {
            SW_RAM   = 1 << 4,
            SW_FLASH = 1 << 5,
            SW       = 0x000000F0,
            HW       = 0xFFFFFF00,
            ANY      = HW | SW,
            MASK     = ANY,
        };

        // Other cores - type does not matter, default to MODE_0 probably
        namespace Type_ARM_7_9_CORTEX_A {
            static constexpr auto ARM   = Mode::MODE_0;
            static constexpr auto THUMB = Mode::MODE_1;
        } // namespace Type_ARM_7_9_CORTEX_A

        enum ImplFlags : uint32_t {
            HARDWARE = 1 << 0,
            SOFTWARE = 1 << 1,
            PENDING  = 1 << 2,
            FLASH    = 1 << 4,
        };

    } // namespace Breakpoint

    namespace Event {

        namespace Type {
            static constexpr size_t DATA_BREAKPOINT = 1 << 0;
        }

        namespace Error {
            static constexpr uint32_t UNKNOWN             = 0x80000000;
            static constexpr uint32_t NO_MORE_EVENTS      = 0x80000001;
            static constexpr uint32_t NO_MORE_ADDR_COMP   = 0x80000002;
            static constexpr uint32_t NO_MORE_DATA_COMP   = 0x80000004;
            static constexpr uint32_t INVALID_ADDR_MASK   = 0x80000020;
            static constexpr uint32_t INVALID_DATA_MASK   = 0x80000040;
            static constexpr uint32_t INVALID_ACCESS_MASK = 0x80000080;
        } // namespace Error

        enum DataBreakpoint : uint32_t {
            READ       = 0 << 0,
            WRITE      = 1 << 0,
            PRIV       = 1 << 4,
            SIZE_8BIT  = 0 << 1,
            SIZE_16BIT = 1 << 1,
            SIZE_32BIT = 2 << 1,
        };

        enum class DataBreakpointTypeMask : uint32_t {
            SIZE = 3 << 1,
            DIR  = 1 << 0,
            PRIV = 1 << 4,
        };

    } // namespace Event

    namespace Watchpoint {
        static constexpr uint32_t DIR_RD     = 0 << 0;
        static constexpr uint32_t DIR_WR     = 1 << 0;
        static constexpr uint32_t PRIV       = 1 << 4;
        static constexpr uint32_t SIZE_8BIT  = 0 << 1;
        static constexpr uint32_t SIZE_16BIT = 1 << 1;
        static constexpr uint32_t SIZE_32BIT = 2 << 1;
    } // namespace Watchpoint

    namespace WatchpointTypeMask {
        static constexpr uint32_t SIZE = 3 << 1;
        static constexpr uint32_t DIR  = 1 << 0;
        static constexpr uint32_t PRIV = 1 << 4;
    } // namespace WatchpointTypeMask

    enum class Speed {
        AUTO     = 0,
        INVALID  = 0xFFFE,
        ADAPTIVE = 0xFFFF,
    };

    enum class Indicator {
        LED_GREEN = 0,
        LED_RED   = 1,
        BUSY      = 2,
        OK        = 3,
    };

    enum class Go_ExFlags : uint32_t {
        OVERSTEP_BREAKPOINT = 1 << 0, // overstep current instruction if it is on a breakpoint
        DEFAULT             = 0xFFFFFFFF,
    };

    enum DownloadFlags : int {
        ALLOW_FLASH        = 1 << 0,
        ALLOW_BUFFERED_RAM = 1 << 1,
    };

    enum class VectorCatchFlags_Cortex_M3 {
        CORERESET = 1 << 0,
        MMERR     = 1 << 4,
        NOCPERR   = 1 << 5,
        CHKERR    = 1 << 6,
        STATERR   = 1 << 7,
        BUSERR    = 1 << 8,
        INTERR    = 1 << 9,
        HARDERR   = 1 << 10,
    };

    enum class GPIO_State_Flags {
        UNCHANGE = 0,
        INPUT    = 1,
        CLR      = 2,
        SET      = 3,
    };

    enum class ExternalDialogFlags {
        BUTTON_YES      = 1 << 0,
        BUTTON_NO       = 1 << 1,
        BUTTON_OK       = 1 << 2,
        BUTTON_CANCEL   = 1 << 3,
        TYPE_PROT_READ  = 1 << 28,
        TYPE_PROT_WRITE = 2 << 28,
    };

    namespace CPU_Registers {

        enum class ARM : uint32_t {
            R0,
            R1,
            R2,
            R3,
            R4,
            R5,
            R6,
            R7,
            CPSR,
            R15,
            R8_USR,
            R9_USR,
            R10_USR,
            R11_USR,
            R12_USR,
            R13_USR,
            R14_USR,
            SPSR_FIQ,
            R8_FIQ,
            R9_FIQ,
            R10_FIQ,
            R11_FIQ,
            R12_FIQ,
            R13_FIQ,
            R14_FIQ,
            SPSR_SVC,
            R13_SVC,
            R14_SVC,
            SPSR_ABT,
            R13_ABT,
            R14_ABT,
            SPSR_IRQ,
            R13_IRQ,
            R14_IRQ,
            SPSR_UND,
            R13_UND,
            R14_UND,
            FPSID,
            FPSCR,
            FPEXC,
            FPS0,
            FPS1,
            FPS2,
            FPS3,
            FPS4,
            FPS5,
            FPS6,
            FPS7,
            FPS8,
            FPS9,
            FPS10,
            FPS11,
            FPS12,
            FPS13,
            FPS14,
            FPS15,
            FPS16,
            FPS17,
            FPS18,
            FPS19,
            FPS20,
            FPS21,
            FPS22,
            FPS23,
            FPS24,
            FPS25,
            FPS26,
            FPS27,
            FPS28,
            FPS29,
            FPS30,
            FPS31,
            R8,
            R9,
            R10,
            R11,
            R12,
            R13,
            R14,
            SPSR,
            COUNT,
        };

        enum class Cortex_M3 : uint32_t {
            R0,
            R1,
            R2,
            R3,
            R4,
            R5,
            R6,
            R7,
            R8,
            R9,
            R10,
            R11,
            R12,
            R13, // Pseudo (SP_MSP or SP_PSP) depending on control register:
            R14,
            R15,
            XPSR,
            MSP,
            PSP,
            RAZ,
            CFBP,
            APSR,        // Pseudo (XPSR)
            EPSR,        // Pseudo (XPSR)
            IPSR,        // Pseudo (XPSR)
            PRIMASK,     //   Pseudo(CFBP)
            BASEPRI,     //   Pseudo(CFBP)
            FAULTMASK,   //      Pseudo(CFBP)
            CONTROL,     //    Pseudo(CFBP)
            BASEPRI_MAX, //        Pseudo(CFBP)
            IAPSR,       //  Pseudo(XPSR)
            EAPSR,       //  Pseudo(XPSR)
            IEPSR,       //  Pseudo(XPSR)
            DWT_CYCCNT = 65,
            MSP_NS,
            PSP_NS,
            MSP_S,
            PSP_S,
            MSPLIM_S,
            PSPLIM_S,
            MSPLIM_NS,
            PSPLIM_NS,
            CFBP_S,
            CFBP_NS,
            PRIMASK_NS,     // Pseudo (CFBP)
            BASEPRI_NS,     // Pseudo (CFBP)
            FAULTMASK_NS,   // Pseudo (CFBP)
            CONTROL_NS,     // Pseudo (CFBP)
            BASEPRI_MAX_NS, // Pseudo (CFBP)
            PRIMASK_S,      // Pseudo (CFBP)
            BASEPRI_S,      // Pseudo (CFBP)
            FAULTMASK_S,    // Pseudo (CFBP)
            CONTROL_S,      // Pseudo (CFBP)
            BASEPRI_MAX_S,  // Pseudo (CFBP)
            MSPLIM,
            PSPLIM,
            COUNT,
        };
    } // namespace CPU_Registers

    enum class Cortex_M4 {
        R0,
        R1,
        R2,
        R3,
        R4,
        R5,
        R6,
        R7,
        R8,
        R9,
        R10,
        R11,
        R12,
        R13, // Pseudo (SP_MSP or SP_PSP) depending on control register:
        R14,
        R15,
        XPSR,
        MSP,
        PSP,
        RAZ,
        CFBP,
        APSR,        // Pseudo (XPSR)
        EPSR,        // Pseudo (XPSR)
        IPSR,        // Pseudo (XPSR)
        PRIMASK,     // Pseudo (CFBP)
        BASEPRI,     // Pseudo (CFBP)
        FAULTMASK,   // Pseudo (CFBP)
        CONTROL,     // Pseudo (CFBP)
        BASEPRI_MAX, // Pseudo (CFBP)
        IAPSR,       // Pseudo (XPSR)
        EAPSR,       // Pseudo (XPSR)
        IEPSR,       // Pseudo (XPSR)
        FPSCR,
        FPS0,
        FPS1,
        FPS2,
        FPS3,
        FPS4,
        FPS5,
        FPS6,
        FPS7,
        FPS8,
        FPS9,
        FPS10,
        FPS11,
        FPS12,
        FPS13,
        FPS14,
        FPS15,
        FPS16,
        FPS17,
        FPS18,
        FPS19,
        FPS20,
        FPS21,
        FPS22,
        FPS23,
        FPS24,
        FPS25,
        FPS26,
        FPS27,
        FPS28,
        FPS29,
        FPS30,
        FPS31,
        DWT_CYCCNT,
        MSP_NS,
        PSP_NS,
        MSP_S,
        PSP_S,
        MSPLIM_S,
        PSPLIM_S,
        MSPLIM_NS,
        PSPLIM_NS,
        CFBP_S,
        CFBP_NS,
        PRIMASK_NS,
        BASEPRI_NS,
        FAULTMASK_NS,
        CONTROL_NS,
        BASEPRI_MAX_NS,
        PRIMASK_S,
        BASEPRI_S,
        FAULTMASK_S,
        CONTROL_S,
        BASEPRI_MAX_S,
        MSPLIM,
        PSPLIM,
        COUNT
    };

#pragma pack(1)
    struct STRACE_EventInfo {
        uint32_t structSize;
        STRACE_EventType type;
        STRACE_Operation op;
        uint8_t accessSize;
        uint8_t _;
        uint64_t address;
        uint64_t data;
        uint64_t dataMask;
        uint32_t addressRangeSize;
    };
#pragma pack()

    struct STRACE_InstStats {
        uint64_t execCount;
    };

    enum class STRACE_InstCounters : uint32_t {
        FETCHED                  = 0,
        EXECUTED                 = 1,
        SKIPPED                  = 2,
        TOTAL_FETCHED            = 4,
        TOTAL_EXECECUTED         = 5,
        TOTAL_SKIPPED            = 6,
        TOTAL_COUNT              = 7,
        FETCHED_EXECUTED         = 8,
        FETCHED_SKIPPED          = 9,
        EXECUTED_SKIPPED         = 10,
        FETCHED_EXECUTED_SKIPPED = 11,
    };

    enum class DebugInfoIndex : uint32_t {
        ROM_TABLE_ADDRESS         = 0x100,
        ETM_ADDR                  = 0x101,
        MTB_ADDR                  = 0x102,
        TPIU_ADDR                 = 0x103,
        ITM_ADDR                  = 0x104,
        DWT_ADDR                  = 0x105,
        FPB_ADDR                  = 0x106,
        NVIC_ADDR                 = 0x107,
        TMC_ADDR                  = 0x108,
        TF_ADDR                   = 0x109,
        PTM_ADDR                  = 0x10A,
        ETB_ADDR                  = 0x10B,
        DBG_ADDR                  = 0x10C,
        APBAP                     = 0x10D,
        AHBAP                     = 0x10E,
        HAS_CORTEX_M_SECURITY_EXT = 0x10F,
    };

    enum class ARM_Endian {
        LITTLE = 0,
        BIG    = 1,
        BOTH   = 2,
    };

    //
    // ARM_RESET_TYPE
    //
    enum class ResetType : uint32_t {
        NORMAL = 0,  // Core + Peripherals
        BP0,         // Not recommended
        ADI,         // Not recommended
        NO_RESET,    // Not recommended
        HALT_WP,     // Not recommended
        HALT_DBGRQ,  // Not recommended
        SOFT,        // Not recommended
        HALT_DURING, // Not recommended
        SAM7,        // Not recommended
        LPC,         // Not recommended
        CORE = 100,
        RESET_PIN,
    };

    enum class SWO_ConversionMode {
        WINDOWS = 0, // \r\n
        NONE    = 1, // no change
    };

    struct IndicatorControl {
        uint16_t id;
        uint16_t overrideInternal;
        uint16_t initialOnTime; // 1ms multiplier
        uint16_t onTime;        // 1ms multiplier
        uint16_t offTime;       // 1ms multiplier
    };

    struct BreakpointInfo {
        uint32_t structSize;             // In
        uint32_t handle;                 // Out (also "In" if Index is invalid)
        uint32_t address;                // Out
        Breakpoint::Type type;           // Out
        Breakpoint::ImplFlags implFlags; // Out
        uint32_t useCount;               // Out
        uint8_t internal;                // Out
        uint8_t disabled;                // Out
    };

    struct WatchpointInfo {
        uint32_t structSize;
        uint32_t handle;
        uint32_t address;
        uint32_t addressMask;
        uint32_t data;
        uint32_t dataMask;
        uint32_t control;
        uint32_t controlMask;
        uint8_t watchpointUnit;
        uint8_t internal;
        uint8_t disabled;
        uint8_t _;
        uint32_t unitMask;
    };

    struct DataEvent {
        int size;
        int type;
        uint32_t address;
        uint32_t addressMask;
        uint32_t data;
        uint32_t dataMask;
        uint8_t access;
        uint8_t accessMask;
    };

    enum class HaltReason : uint32_t {
        DBGRQ           = 0,
        CODE_BREAKPOINT = 1,
        DATA_BREAKPOINT = 2,
        VECTOR_CATCH    = 3,
    };

    /// Method of debug entry
    struct MOE_Info {
        HaltReason reason;
        int index;
    };

    struct AreaInfo {
        uint32_t address;
        uint32_t size;
    };

    struct FlashBankInfo {
        const char* bankName;
        const char* algoFile;
        uint32_t algoType;
        uint32_t baseAddress;
        const void* blockInfo;
        const void* algoInfo;
    };

    struct PCODE_Info {
        const uint8_t* data; // Pointer to PCode.
        uint32_t length;     // Length of PCode in bytes.
    };

    struct DeviceInfo {
        uint32_t structSize;
        const char* name;
        uint32_t coreId;
        uint32_t flashAddress;
        uint32_t ramAddress;
        ARM_Endian endianness;
        uint32_t flashSize;
        uint32_t ramSize;
        const char* manufacturer;
        AreaInfo flashArea[32];
        AreaInfo ramArea[32];
        DeviceCore core;
        FlashBankInfo flashBank[16];
        PCODE_Info pcode[10];
    };

    struct SpeedInfo {
        uint32_t structSize;
        uint32_t baseClock;  // Clock frequency in Hz
        uint16_t minDivider; // Minimum divider.  Max JTAG speed is BaseFreq / MinDiv.
        uint16_t adaptiveSupported;
    };

    struct ProbeInfo {
        uint32_t serialNumber;
        HostInterfaceType interface;
        uint32_t usbAddress;
        uint8_t ipAddress[4];
        uint8_t _pad0[12];
        int udpDiscoverTime;
        uint64_t udpDiscoverTime_usec;
        uint32_t hardwareVersion;
        uint8_t macAddress[6];
        char modelName[32];
        char nickname[32];
        char firmwareString[112];
        char useDhcpAddress;
        char isDhcpAddressValid;
        char networkConnectionCount;
        char validNetworkConnectionCount;
        uint8_t _pad1[34];
    };

    struct HardwareStatus {
        uint16_t targetVoltage;
        uint8_t tck;
        uint8_t tdi;
        uint8_t tdo;
        uint8_t tms;
        uint8_t tres;
        uint8_t trst;
    };

    struct TraceData {
        uint8_t pipeStat;
        uint8_t sync;
        uint16_t packet;
    };

    struct TraceRegionProperties {
        uint32_t structSize;
        uint32_t regionIndex;
        uint32_t numberOfSamples;
        uint32_t offset;
    };

    struct TraceRegionExtendedProperties {
        uint32_t structSize;
        uint32_t regionIndex;
        uint32_t numberOfSamples;
        uint32_t offset;
        uint32_t regionCount;
        uint32_t _;
        uint64_t timestamp;
    };

    enum class SWO_Interface : uint32_t {
        UART       = 0,
        MANCHESTER = 1,
    };

    struct SWO_StartInfo {
        uint32_t structSize;
        SWO_Interface interface;
        uint32_t speed; // Hz (0 = auto)
    };

    struct SWO_SpeedInfo {
        uint32_t structSize;
        SWO_Interface interface;
        uint32_t baseClock;
        uint32_t minDivider;
        uint32_t maxDivider;
        uint32_t minPrescale;
        uint32_t maxPrescale;
    };

    struct WriteMemoryDescriptor {
        uint32_t address;
        uint32_t dataLength;
        uint8_t* sourceData;
        uint8_t* statusPtr;
        uint32_t accessWidth;
        uint32_t flags;
        int result;
    };

    struct RTT_Start {
        uint32_t configBlockAddress;
        uint32_t _0;
        uint32_t _1;
        uint32_t _2;
    };

    struct RTT_Stop {
        uint8_t invalidateTargetCB;
        uint8_t _0[3];
        uint32_t _1;
        uint32_t _2;
        uint32_t _3;
    };

    enum class RTT_BufferDirection : uint32_t {
        UP   = 0,
        DOWN = 1,
    };

    struct RTT_BufferDescriptor {
        int index;
        RTT_BufferDirection direction;
        char name[32];
        uint32_t bufferSize;
        uint32_t flags;
    };

    struct RTT_TerminalStatus {
        uint32_t totalBytesSent;
        uint32_t totalBytesRead;
        int hostOverflowCount;
        int running;
        int upBufferCount;
        int downBufferCount;
        uint32_t _0;
        uint32_t _1;
    };

    struct HSS_MemoryBlockDescriptor {
        uint32_t address;
        uint32_t length;
        uint32_t flags;
        uint32_t _;
    };

    struct HSS_Capabilities {
        uint32_t maxBlocks;
        uint32_t maxFrequency;
        uint32_t capabilities;
        uint32_t _[5];
    };

    enum class TraceSource : int {
        ETB = 0,
        ETM = 1,
        MTB = 2,
    };

    enum class TraceCommand : uint32_t {
        START               = 0,
        STOP                = 1,
        FLUSH               = 2,
        GET_NUM_SAMPLES     = 0x10,
        GET_CONF_CAPACITY   = 0x11,
        SET_CAPACITY        = 0x12,
        GET_MIN_CAPACITY    = 0x13,
        GET_MAX_CAPACITY    = 0x14,
        SET_FORMAT          = 0x20,
        GET_FORMAT          = 0x21,
        GET_NUM_REGIONS     = 0x30,
        GET_REGION_PROPS    = 0x31,
        GET_REGION_PROPS_EX = 0x32,
    };

    namespace TraceFormat {
        static constexpr uint32_t BITS_4        = 0x00000001ul;
        static constexpr uint32_t BITS_8        = 0x00000002ul;
        static constexpr uint32_t BITs_16       = 0x00000004ul;
        static constexpr uint32_t MULTIPLEXED   = 0x00000008ul;
        static constexpr uint32_t DEMULTIPLEXED = 0x00000010ul;
        static constexpr uint32_t DOUBLE_EDGE   = 0x00000020ul;
        static constexpr uint32_t ETM7_9        = 0x00000040ul;
        static constexpr uint32_t ETM10         = 0x00000080ul;
        static constexpr uint32_t BITS_1        = 0x00000100ul;
        static constexpr uint32_t BITS_2        = 0x00000200ul;
    }; // namespace TraceFormat

    enum class RawTraceCommand {
        START          = 0,
        STOP           = 1,
        GET_TRACE_FREQ = 2,
        SET_BUFF_SIZE  = 3,
        GET_CAPS       = 4,
    };

    namespace RawTraceFormat {
        static constexpr uint32_t BITS_1 = 0x00000003ul;
        static constexpr uint32_t BITS_2 = 0x0000000Cul;
        static constexpr uint32_t BITS_4 = 0x00000030ul;
    } // namespace RawTraceFormat

    namespace RawTraceCapabilities {
        static constexpr uint32_t RESERVED   = 1 << 0;
        static constexpr uint32_t START      = 1 << 1;
        static constexpr uint32_t STOP       = 1 << 2;
        static constexpr uint32_t READ       = 1 << 3;
        static constexpr uint32_t GET_FREQ   = 1 << 4;
        static constexpr uint32_t LTRACE     = 1 << 5;
        static constexpr uint32_t LTRACE_USB = 1 << 6;
    } // namespace RawTraceCapabilities

    enum class RTT_Command : uint32_t {
        START     = 0,
        STOP      = 1,
        GETDESC   = 2,
        GETNUMBUF = 3,
        GETSTAT   = 4,
    };

    enum class SWO_Command : uint32_t {
        START               = 0,
        STOP                = 1,
        FLUSH               = 2,
        GET_SPEED_INFO      = 3,
        GET_NUM_BYTES       = 10,
        SET_BUFFERSIZE_HOST = 20,
        SET_BUFFERSIZE_EMU  = 21,
    };

} // namespace HWD::Probe::Driver::JLink_Types

template<>
struct fmt::formatter<HWD::Probe::Driver::JLink_Types::ErrorCode> {
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(HWD::Probe::Driver::JLink_Types::ErrorCode const& ec, FormatContext& ctx) {
        return fmt::format_to(ctx.out(), "{0}", HWD::Probe::Driver::JLink_Types::ErrorCodeToString(ec));
    }
};

template<>
struct fmt::formatter<HWD::Probe::Driver::JLink_Types::DeviceCore> {
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(HWD::Probe::Driver::JLink_Types::DeviceCore const& core, FormatContext& ctx) {
        return fmt::format_to(ctx.out(), "{0}", HWD::Probe::Driver::JLink_Types::DeviceCoreToString(core));
    }
};
