#pragma once

#include "CoreSight.hpp"

namespace HWD::Cortex {

    namespace M4 {

        struct ROM_Table_Entry {
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

        union ROM_Table_Offsets {
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

        union PeripheralID4 {
            struct {
                uint8_t jep106 : 4;
                uint8_t log2BlockCount : 4; // number of 4kB blocks this peripheral uses
            };
            uint8_t _val;

            constexpr uint32_t GetBlockCount() const {
                return 1 << log2BlockCount;
            }
        };

        constexpr const char* ROM_Table_EntryName[] = {"SCS", "DWT", "FPB", "ITM", "TPIU", "ETM"};

        class ROM_Table {};

    } // namespace M4

} // namespace HWD::Cortex