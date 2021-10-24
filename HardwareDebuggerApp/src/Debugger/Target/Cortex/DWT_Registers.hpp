// ---------------------------------------------------------------------
// CFXS Hardware Debugger <https://github.com/CFXS/CFXS-Hardware-Debugger>
// Copyright (C) 2021 | CFXS
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

namespace HWD::Cortex {

    namespace M4::DWT {

        // Number of comparators = CTRL.NUMCOMP - 1

        static constexpr uint32_t OFFSET_CTRL      = 0x000; // Control
        static constexpr uint32_t OFFSET_CYCCNT    = 0x04;  // Cycle Count
        static constexpr uint32_t OFFSET_CPICNT    = 0x08;  // CPI Count
        static constexpr uint32_t OFFSET_EXECCNT   = 0x0C;  // Exception Overhead Count
        static constexpr uint32_t OFFSET_SLEEPCNT  = 0x010; // Sleep Count
        static constexpr uint32_t OFFSET_LSUCNT    = 0x014; // LSU Count (Incremented by load/store addition instructions)
        static constexpr uint32_t OFFSET_FOLDCNT   = 0x018; // Folded-instruction Count
        static constexpr uint32_t OFFSET_PCSR      = 0x01C; // Program Counter Sample Register
        static constexpr uint32_t OFFSET_COMPn     = 0x020; // Comparator          (addr = offset + 16*n)
        static constexpr uint32_t OFFSET_MASKn     = 0x024; // Comparator Mask     (addr = offset + 16*n)
        static constexpr uint32_t OFFSET_FUNCTIONn = 0x028; // Comparator Function (addr = offset + 16*n)

        static constexpr uint32_t OFFSET_COMP(uint8_t n) {
            return OFFSET_COMPn + 16 * n;
        }

        static constexpr uint32_t OFFSET_MASK(uint8_t n) {
            return OFFSET_MASKn + 16 * n;
        }

        static constexpr uint32_t OFFSET_FUNCTION(uint8_t n) {
            return OFFSET_FUNCTIONn + 16 * n;
        }

        class REG_CTRL {
        public:
            DEF_REG_CONSTRUCTOR(REG_CTRL);
            DEF_REG_OPERATORS();

            DEF_REG_FIELD(NUMCOMP, 31, 28);        // Number of comparators - a value of zero indicates no comparator support
            DEF_REG_FIELD(NOTRCPKT, 27, 27);       // Shows whether the implementation supports trace sampling and exception tracing
            DEF_REG_FIELD(NOEXTTRIG, 26, 26);      // Shows whether the implementation includes external match signals, CMPMATCH[N]
            DEF_REG_FIELD(NOCYCCNT, 25, 25);       // Shows whether the implementation supports a cycle counter
            DEF_REG_FIELD(NOPRFCNT, 24, 24);       // Shows whether the implementation supports the profiling counters
            DEF_REG_FIELD(reserved_23, 23, 23);    // Reserved
            DEF_REG_FIELD(CYCEVTENA, 22, 22);      // Enables POSTCNT underflow Event counter packets generation
            DEF_REG_FIELD(FOLDEVTENA, 21, 21);     // Enables generation of the Folded-instruction counter overflow event
            DEF_REG_FIELD(LSUEVTENA, 20, 20);      // Enables generation of the LSU counter overflow event
            DEF_REG_FIELD(SLEEPEVTENA, 19, 19);    // Enables generation of the Sleep counter overflow event
            DEF_REG_FIELD(EXCEVTENA, 18, 18);      // Enables generation of the Exception overhead counter overflow event
            DEF_REG_FIELD(CPIEVTENA, 17, 17);      // Enables generation of the CPI counter overflow event
            DEF_REG_FIELD(EXCTRCENA, 16, 16);      // Enables generation of exception trace
            DEF_REG_FIELD(reserved_15_13, 15, 13); // Reserved
            DEF_REG_FIELD(PCSAMPLENA, 12, 12);     // Enables use of POSTCNT counter as a timer for Periodic PC sample packet generation
            DEF_REG_FIELD(SYNCTAP, 11, 10);        // Selects the position of the synchronization packet counter tap on the CYCCNT counter
            DEF_REG_FIELD(CYCTAP, 9, 9);           // Selects the position of the POSTCNT tap on the CYCCNT counter
            DEF_REG_FIELD(POSTINIT, 8, 5);         // Initial value for the POSTCNT counter [This field was previously called POSTCNT]
            DEF_REG_FIELD(POSTPRESET, 4, 1);       // Reload value for the POSTCNT counter
            DEF_REG_FIELD(CYCCNTENA, 0, 0);        // Enables CYCCNT

            static constexpr uint8_t VAL_SYNCTAP_DISABLED  = 0b00; // Disabled - no Synchronization packets
            static constexpr uint8_t VAL_SYNCTAP_CYCCNT_24 = 0b01; // Synchronization counter tap at CYCCNT[24]
            static constexpr uint8_t VAL_SYNCTAP_CYCCNT_26 = 0b10; // Synchronization counter tap at CYCCNT[26]
            static constexpr uint8_t VAL_SYNCTAP_CYCCNT_28 = 0b11; // Synchronization counter tap at CYCCNT[28]
            static constexpr uint8_t VAL_CYCTAP_CYCCNT_6   = 0;    // POSTCNT tap at CYCCNT[6]
            static constexpr uint8_t VAL_CYCTAP_CYCCNT_10  = 1;    // POSTCNT tap at CYCCNT[10]

            enum class SyncInterval : uint8_t {
                DISABLED = VAL_SYNCTAP_DISABLED,
                FAST     = VAL_SYNCTAP_CYCCNT_24,
                MEDIUM   = VAL_SYNCTAP_CYCCNT_26,
                SLOW     = VAL_SYNCTAP_CYCCNT_28,
            };

            enum class SampleRateDivider : uint16_t {   // 0x[POSTPRESET][CYCTAP]
                _64  = (0 << 8) | VAL_CYCTAP_CYCCNT_6,  // 1 * 64
                _128 = (1 << 8) | VAL_CYCTAP_CYCCNT_6,  // 2 * 64
                _192 = (2 << 8) | VAL_CYCTAP_CYCCNT_6,  // 3 * 64
                _256 = (3 << 8) | VAL_CYCTAP_CYCCNT_6,  // 4 * 64
                _320 = (4 << 8) | VAL_CYCTAP_CYCCNT_6,  // 5 * 64
                _384 = (5 << 8) | VAL_CYCTAP_CYCCNT_6,  // 6 * 64
                _448 = (6 << 8) | VAL_CYCTAP_CYCCNT_6,  // 7 * 64
                _512 = (7 << 8) | VAL_CYCTAP_CYCCNT_6,  // 8 * 64
                _576 = (8 << 8) | VAL_CYCTAP_CYCCNT_6,  // 9 * 64
                _640 = (9 << 8) | VAL_CYCTAP_CYCCNT_6,  // 10 * 64
                _704 = (10 << 8) | VAL_CYCTAP_CYCCNT_6, // 11 * 64
                _768 = (11 << 8) | VAL_CYCTAP_CYCCNT_6, // 12 * 64
                _832 = (12 << 8) | VAL_CYCTAP_CYCCNT_6, // 13 * 64
                _896 = (13 << 8) | VAL_CYCTAP_CYCCNT_6, // 14 * 64
                _960 = (14 << 8) | VAL_CYCTAP_CYCCNT_6, // 15 * 64
                //_1024  = (15 << 8) | VAL_CYCTAP_CYCCNT_6,  // 16 * 64
                _1024  = (0 << 8) | VAL_CYCTAP_CYCCNT_10,  // 1 * 1024
                _2048  = (1 << 8) | VAL_CYCTAP_CYCCNT_10,  // 2 * 1024
                _3072  = (2 << 8) | VAL_CYCTAP_CYCCNT_10,  // 3 * 1024
                _4096  = (3 << 8) | VAL_CYCTAP_CYCCNT_10,  // 4 * 1024
                _5120  = (4 << 8) | VAL_CYCTAP_CYCCNT_10,  // 5 * 1024
                _6144  = (5 << 8) | VAL_CYCTAP_CYCCNT_10,  // 6 * 1024
                _7168  = (6 << 8) | VAL_CYCTAP_CYCCNT_10,  // 7 * 1024
                _8192  = (7 << 8) | VAL_CYCTAP_CYCCNT_10,  // 8 * 1024
                _9216  = (8 << 8) | VAL_CYCTAP_CYCCNT_10,  // 9 * 1024
                _10240 = (9 << 8) | VAL_CYCTAP_CYCCNT_10,  // 10 * 1024
                _11264 = (10 << 8) | VAL_CYCTAP_CYCCNT_10, // 11 * 1024
                _12288 = (11 << 8) | VAL_CYCTAP_CYCCNT_10, // 12 * 1024
                _13312 = (12 << 8) | VAL_CYCTAP_CYCCNT_10, // 13 * 1024
                _14336 = (13 << 8) | VAL_CYCTAP_CYCCNT_10, // 14 * 1024
                _15360 = (14 << 8) | VAL_CYCTAP_CYCCNT_10, // 15 * 1024
                _16384 = (15 << 8) | VAL_CYCTAP_CYCCNT_10, // 16 * 1024
            };

            static float SampleRateDividerToSampleRate(float cpuFreq, SampleRateDivider div) {
                return cpuFreq / (((((uint16_t)div) >> 8) + 1) * (((uint16_t)div) ? 1024 : 64));
            }

            void Set_Sampling_Divider(SampleRateDivider srDiv) {
                *this &= ~MASK_POSTPRESET;                             // clear reset value
                *this &= ~MASK_POSTINIT;                               // clear current value
                *this |= (((uint16_t)srDiv) >> 8) << SHIFT_POSTPRESET; // set timer reset
                *this |= (((uint16_t)srDiv) & 0x01) << SHIFT_CYCTAP;   // set divider
            }

            void Set_Cycle_Counter_Enabled(bool state) {
                if (state) {
                    *this |= MASK_CYCCNTENA; // enable cycle counter
                } else {
                    *this &= ~MASK_CYCCNTENA; // disable cycle counter
                }
            }

            void Set_PC_Sampling_Enabled(bool state) {
                if (state)
                    *this |= MASK_PCSAMPLENA; // enable PC sampling
                else
                    *this &= ~MASK_PCSAMPLENA; // disable PC sampling
            }

            void Set_Exception_Trace_Enabled(bool state) {
                if (state)
                    *this |= MASK_EXCTRCENA; // enable exception trace
                else
                    *this &= ~MASK_EXCTRCENA; // disable exception trace
            }

            void Set_Sync(SyncInterval interval) {
                *this |= ((uint8_t)interval) << SHIFT_SYNCTAP; // set sync tap
            }

        private:
            uint32_t m_Address;
        };

    } // namespace M4::DWT

} // namespace HWD::Cortex