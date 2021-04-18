// [source]
#include "JLink.hpp"

struct ARM_ROM_Table_Entry {
    // address_offset : 20; // The base address of the component, relative to the base address of this ROM Table
    // reserved_0 : 10;     // Reserved - should be 0
    // type : 1;            // 0 = 8bit, 1 = 32bit
    // is_present : 1;      // entry present
    uint32_t _val;

    constexpr bool IsTableEndMarker() const {
        return _val == 0;
    }

    constexpr bool IsPresent() const {
        return _val & 0b01;
    }

    constexpr bool Is32bit() const {
        return _val & 0b10;
    }

    constexpr bool Is8bit() const {
        return !(_val & 0b10);
    }

    constexpr uint32_t GetComponentAddress(uint32_t romTableBase) const {
        return romTableBase + (_val & 0xFFFFF000); // 31:12
    }
};

union ARM_ROM_Table_Offsets {
    struct {
        uint32_t SCS;
        uint32_t DWT; // debug/watchpoint/trace
        uint32_t FPB;
        uint32_t ITM;
        uint32_t TPIU;
        uint32_t ETM;
    };
    uint32_t _table[6];
};

struct ARM_PeripheralID4 {
    uint8_t jep106 : 4;
    uint8_t log2BlockCount : 4; // number of 4kB blocks this peripheral uses

    constexpr uint32_t GetBlockCount() const {
        return 1 << log2BlockCount;
    }
};

static constexpr uint32_t ARM_OFFSET_CS_PERIPHERAL_ID4 = 0xFD0;
static constexpr uint32_t ARM_OFFSET_CS_PERIPHERAL_ID5 = 0xFD4;
static constexpr uint32_t ARM_OFFSET_CS_PERIPHERAL_ID6 = 0xFD8;
static constexpr uint32_t ARM_OFFSET_CS_PERIPHERAL_ID7 = 0xFDC;
static constexpr uint32_t ARM_OFFSET_CS_PERIPHERAL_ID0 = 0xFE0;
static constexpr uint32_t ARM_OFFSET_CS_PERIPHERAL_ID1 = 0xFE4;
static constexpr uint32_t ARM_OFFSET_CS_PERIPHERAL_ID2 = 0xFE8;
static constexpr uint32_t ARM_OFFSET_CS_PERIPHERAL_ID3 = 0xFEC;

static constexpr uint32_t ARM_OFFSET_CS_COMPONENT_ID3 = 0xFFC; // CS Component ID register 3
static constexpr uint32_t ARM_OFFSET_CS_COMPONENT_ID2 = 0xFF8; // CS Component ID register 2
static constexpr uint32_t ARM_OFFSET_CS_COMPONENT_ID1 = 0xFF4; // CS Component ID register 1
static constexpr uint32_t ARM_OFFSET_CS_COMPONENT_ID0 = 0xFF0; // CS Component ID register 0

static constexpr uint32_t ARM_OFFSET_CS_DEVTYPE = 0xFCC; // CS Device Type register
static constexpr uint32_t ARM_OFFSET_CS_DEVID   = 0xFC8; // CS Device ID register

static constexpr uint32_t ARM_OFFSET_CS_DEBUGID                  = 0x000;
static constexpr uint32_t ARM_CS_DEBUGID_DEVID_IMPLEMENTED       = 0x00008000; // is DEVID implemented
static constexpr uint32_t ARM_CS_DEBUGID_PC_SAMPLING_IMPLEMENTED = 0x00002000; // is PC sampling implemented

static constexpr uint32_t ARM_OFFSET_CS_LOCK_ACCESS = 0xFB0;      // CS component Software Lock access register
static constexpr uint32_t ARM_OFFSET_CS_LOCK_STATUS = 0xFB4;      // CS component Software Lock status register
static constexpr uint32_t ARM_OFFSET_CS_UNLOCK_KEY  = 0xC5ACCE55; // CS component Software Lock - unlock key value

static constexpr uint32_t ARM_CS_CLASS_ROMTABLE  = 0x01; // CoreSight Class value for ROM table
static constexpr uint32_t ARM_CS_CLASS_CORESIGHT = 0x09; // CoreSight Class value for CoreSight component
static constexpr uint32_t ARM_CS_CLASS_GENERIC   = 0x0E; // CoreSight Class value for Generic component
static constexpr uint32_t ARM_CS_CLASS_PRIMECELL = 0x0F; // CoreSight Class value for Primecell component table

constexpr const char* ARM_ROM_Table_EntryName[] = {"SCS", "DWT", "FPB", "ITM", "TPIU", "ETM"};

namespace HWD::Probe {

    using namespace Driver::JLink_Types;

    void JLink::PrepareTarget() {
        HWDLOG_PROBE_TRACE("[JLink@{0}] Prepare target", fmt::ptr(this));
        ErrorCode ec;

        // Configure CoreSight
        ec = m_Driver->target_CoreSight_Configure("PerformTIFInit=1");
        if (ec != ErrorCode::OK) {
            HWDLOG_PROBE_ERROR("[JLink@{0}] Failed to configure CoreSight - {1}", fmt::ptr(this), ErrorCodeToString(ec));
        } else {
            HWDLOG_PROBE_TRACE("[JLink@{0}] CoreSight configured", fmt::ptr(this));
        }

        UpdateTargetInfo();
    }

    void JLink::UpdateTargetInfo() {
        HWDLOG_PROBE_TRACE("[JLink@{0}] UpdateTargetInfo:", fmt::ptr(this));
        ErrorCode ec;
        int bytesRead;

        uint32_t cpu_id                = m_Driver->target_Get_CPU_ID();
        DeviceCore core                = m_Driver->target_GetDeviceCore();
        int avail_sw_ram_breakpoints   = m_Driver->probe_GetAvailableBreakpointCount(Breakpoint::Type::SW_RAM);
        int avail_sw_flash_breakpoints = m_Driver->probe_GetAvailableBreakpointCount(Breakpoint::Type::SW_FLASH);
        int avail_hw_breakpoints       = m_Driver->probe_GetAvailableBreakpointCount(Breakpoint::Type::HW);
        uint32_t rom_table_address;

        ec = m_Driver->target_GetDebugInfo(DebugInfoIndex::ROM_TABLE_ADDRESS, &rom_table_address); // ROM table
        if (ec != ErrorCode::OK) {
            rom_table_address = 0;
            HWDLOG_PROBE_WARN("Failed to get ROM table address - {0}", ec);
        }

        HWDLOG_PROBE_TRACE("{0} Target info", GetModelName());
        HWDLOG_PROBE_TRACE("[CPU]");
        HWDLOG_PROBE_TRACE(" - Core:      {0}", core);
        HWDLOG_PROBE_TRACE(" - CPUID:     {0:#X}", cpu_id);
        HWDLOG_PROBE_TRACE(" - ROM Table: {0}", rom_table_address ? fmt::format("@ {0:#X}", rom_table_address) : "Error");
        HWDLOG_PROBE_TRACE("[Debug]");

        if (rom_table_address) {
            // ROM TABLE /////////////////////////////////////////////////////////////
            // [ARM Debug Interface v5 Architecture Specification]
            // - ROM Tables > ROM Table Overview > "A ROM Table always occupies 4kB of memory" (4000 or 4096?)
            uint8_t rom_table_data[4096];
            ARM_ROM_Table_Offsets rtOffsets;
            memset(rtOffsets, 0, sizeof(rtOffsets));

            bytesRead = m_Driver->target_ReadMemoryEx(rom_table_address, sizeof(rom_table_data), rom_table_data, 4); // access width 4
            HWDLOG_PROBE_INFO("rom table read {0} bytes", bytesRead);

            ARM_ROM_Table_Entry* rtEntry = (ARM_ROM_Table_Entry*)rom_table_data;
            int entryIndex               = 0;
            do {
                if (rtEntry->Is8bit()) {
                    HWDLOG_PROBE_CRITICAL("8bit ROM Table entries not supported yet");
                    break;
                }

                uint32_t componentAddress = rtEntry->GetComponentAddress(rom_table_address);
                uint32_t componentBaseAddress;

                HWDLOG_PROBE_INFO(" - {0} {1}", ARM_ROM_Table_EntryName[entryIndex], rtEntry->IsPresent() ? "Present" : "Not present");

                if (rtEntry->IsPresent()) {
                    ARM_PeripheralID4 pid4;
                    m_Driver->target_ReadMemory_8(componentAddress + ARM_OFFSET_CS_PERIPHERAL_ID4, 1, &pid4, nullptr);
                    componentBaseAddress         = componentAddress - (4096 * (pid4.GetBlockCount() - 1));
                    rtOffsets._table[entryIndex] = componentBaseAddress;

                    HWDLOG_PROBE_INFO("\tComponent Base Address: {0:#X}", componentBaseAddress);

                    uint32_t cs_class = (Target_ReadMemory_32(componentBaseAddress + ARM_OFFSET_CS_COMPONENT_ID1) >> 4) & 0x0F;
                    static constexpr auto getClassString = [](uint32_t c) {
                        switch (c) {
                            case ARM_CS_CLASS_CORESIGHT: return "CoreSight";
                            case ARM_CS_CLASS_GENERIC: return "Generic";
                            case ARM_CS_CLASS_PRIMECELL: return "PrimeCell";
                            case ARM_CS_CLASS_ROMTABLE: return "ROM Table";
                            default: return "Unknown";
                        }
                    };
                    HWDLOG_PROBE_INFO("\tComponent Class: {0}", getClassString(cs_class));
                }

                rtEntry++;
                entryIndex++;
            } while (!rtEntry->IsTableEndMarker());

            uint32_t cs_device_id   = Target_ReadMemory_32(rtOffsets.TPIU + ARM_OFFSET_CS_DEVID);
            uint32_t cs_device_type = Target_ReadMemory_32(rtOffsets.TPIU + ARM_OFFSET_CS_DEVTYPE);
            uint16_t cs_part_number = ((Target_ReadMemory_32(rtOffsets.TPIU + ARM_OFFSET_CS_PERIPHERAL_ID1) & 0xF) << 8) |
                                      Target_ReadMemory_32(rtOffsets.TPIU + ARM_OFFSET_CS_PERIPHERAL_ID0) & 0xFF;

            uint8_t cs_dt_minor = cs_device_type & 0x0F;
            uint8_t cs_dt_major = (cs_device_type >> 4) & 0x0F;
            bool locked         = (Target_ReadMemory_32(rtOffsets.TPIU + ARM_OFFSET_CS_LOCK_STATUS) & 0x11) != 1;

            HWDLOG_PROBE_INFO("\tDevice {0}.{1}, P/N {2:#X}, {3}, dt {4:#X}",
                              cs_dt_major,
                              cs_dt_minor,
                              cs_part_number,
                              locked ? "CoreSight Locked" : "CoreSight Unlocked",
                              cs_device_type);

            if (rtOffsets.ITM) {
                // configure pc sampling
            }
        }

        //////////////////////////////////////////////////////////////////////////
        HWDLOG_PROBE_TRACE("[Available Breakpoints]");
        HWDLOG_PROBE_TRACE(" - SW RAM:   {0}", avail_sw_ram_breakpoints);
        HWDLOG_PROBE_TRACE(" - SW FLASH: {0}", avail_sw_flash_breakpoints);
        HWDLOG_PROBE_TRACE(" - HW:       {0}", avail_hw_breakpoints);
    }

} // namespace HWD::Probe