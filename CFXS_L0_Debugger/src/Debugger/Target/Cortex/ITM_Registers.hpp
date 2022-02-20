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

#include <Debugger/Probe/I_Probe.hpp>

#include "CoreSight.hpp"

namespace L0::Cortex {

    namespace M4::ITM {

        static constexpr uint32_t OFFSET_STIMn = 0x000; // Stimulus Port Registers [STIM0 - STIM255]
        static constexpr uint32_t OFFSET_TERn  = 0xE00; // Trace Enable Registers [TER0 - TER7]
        static constexpr uint32_t OFFSET_TPR   = 0xE40; // Trace Privilege Register
        static constexpr uint32_t OFFSET_TCR   = 0xE80; // Trace Control Register

        static constexpr uint32_t OFFSET_STIM(uint8_t n) {
            return OFFSET_STIMn + 4 * n;
        }

        static constexpr uint32_t OFFSET_TER(uint8_t n) {
            return OFFSET_TERn + 4 * n;
        }

        class REG_TER {
        public:
            DEF_REG_CONSTRUCTOR(REG_TER);
            DEF_REG_OPERATORS();

            DEF_REG_FIELD(STIMENA, 31, 0); // Bit field for stimulus ports

            void Set_Enabled(uint32_t mask, bool state) {
                if (state) {
                    *this = mask;
                } else {
                    *this = GetRaw() & ~mask;
                }
            }

        private:
            uint32_t m_Address;
        };

        class REG_TCR {
        public:
            DEF_REG_CONSTRUCTOR(REG_TCR);
            DEF_REG_OPERATORS();

            DEF_REG_FIELD(reserved_31_24, 31, 24); // Reserved
            DEF_REG_FIELD(BUSY, 23, 23);           // Indicates whether the ITM is currently processing events
            DEF_REG_FIELD(TRACE_BUS_ID, 22, 16);   // Identifier for multi-source trace stream formatting
            DEF_REG_FIELD(reserved_15_12, 15, 12); // Reserved
            DEF_REG_FIELD(GTSFREQ, 11, 10);        // Global timestamp frequency
            DEF_REG_FIELD(TSPRESCALE, 9, 8);       // Local timestamp prescaler, used with the trace packet reference clock
            DEF_REG_FIELD(reserved_7_5, 7, 5);     // Reserved
            DEF_REG_FIELD(SWOENA, 4, 4);           // Enables asynchronous clocking of the timestamp counter
            DEF_REG_FIELD(TXENA, 3, 3);   // Enables forwarding of hardware event packet from the DWT unit to the ITM for output to the TPIU
            DEF_REG_FIELD(SYNCENA, 2, 2); // Enables Synchronization packet transmission for a synchronous TPIU
            DEF_REG_FIELD(TSENA, 1, 1);   // Enables Local timestamp generation
            DEF_REG_FIELD(ITMENA, 0, 0);  // Enables the ITM

            static constexpr uint8_t VAL_GTSFREQ_DISABLED     = 0b00; // Disable generation of global timestamps
            static constexpr uint8_t VAL_GTSFREQ_128_CYCLES   = 0b01; // Approximately every 128 cycles
            static constexpr uint8_t VAL_GTSFREQ_8192_CYCLES  = 0b10; // Approximately every 8192 cycles
            static constexpr uint8_t VAL_GTSFREQ_EVERY_PACKET = 0b11; // Timestamp after every packet, if the output FIFO empty

            static constexpr uint8_t VAL_TSPRESCALE_1  = 0b00; // No prescaling
            static constexpr uint8_t VAL_TSPRESCALE_4  = 0b01; // div 4
            static constexpr uint8_t VAL_TSPRESCALE_16 = 0b10; // div 16
            static constexpr uint8_t VAL_TSPRESCALE_64 = 0b11; // div 64

            enum class GlobalTimestampFrequency : uint8_t {
                DISABLED          = VAL_GTSFREQ_DISABLED,
                EVERY_128_CYCLES  = VAL_GTSFREQ_128_CYCLES,
                EVERY_8192_CYCLES = VAL_GTSFREQ_8192_CYCLES,
                EVERY_PACKET      = VAL_GTSFREQ_EVERY_PACKET,
            };

            enum class LocalTimestampPrescaler : uint8_t {
                DIV_1  = VAL_TSPRESCALE_1,
                DIV_4  = VAL_TSPRESCALE_4,
                DIV_16 = VAL_TSPRESCALE_16,
                DIV_64 = VAL_TSPRESCALE_64,
            };

        private:
            uint32_t m_Address;
        };

    } // namespace M4::ITM

} // namespace L0::Cortex