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

    namespace M4::TPIU {

        static constexpr uint32_t OFFSET_SSPSR = 0x000; // Supported Parallel Port Sizes
        static constexpr uint32_t OFFSET_CSPSR = 0x004; // Current Parallel Port Size Register
        static constexpr uint32_t OFFSET_ACPR  = 0x010; // Asynchronous Clock Prescaler Register
        static constexpr uint32_t OFFSET_SPPR  = 0x0F0; // Selected Pin Protocol Register
        static constexpr uint32_t OFFSET_TYPE  = 0xFC8; // TPIU Type register

        class REG_SPPR {
        public:
            DEF_REG_CONSTRUCTOR(REG_SPPR);
            DEF_REG_OPERATORS();

            DEF_REG_FIELD(reserved_31_2, 31, 2); // Reserved
            DEF_REG_FIELD(TXMODE, 1, 0);         // Specifies the protocol for trace output from the TPIU

            static constexpr uint8_t VAL_TXMODE_PARALLEL       = 0b00; // Parallel trace port mode
            static constexpr uint8_t VAL_TXMODE_SWO_MANCHESTER = 0b01; // Asynchronous SWO, using Manchester encoding
            static constexpr uint8_t VAL_TXMODE_SWO_NRZ        = 0b10; // Asynchronous SWO, using NRZ encoding

            enum class OutputInterface : uint8_t {
                PARALLEL       = VAL_TXMODE_PARALLEL,
                SWO_MANCHESTER = VAL_TXMODE_SWO_MANCHESTER,
                SWO_NRZ        = VAL_TXMODE_SWO_NRZ,
            };

            void SetOutputInterface(OutputInterface ifc) {
                *this = ((uint8_t)ifc) << SHIFT_TXMODE;
            }

        private:
            uint32_t m_Address;
        };

        class REG_ACPR {
        public:
            DEF_REG_CONSTRUCTOR(REG_ACPR);
            DEF_REG_OPERATORS();

            DEF_REG_FIELD(reserved_31_16, 31, 16); // Reserved
            DEF_REG_FIELD(SWOPRESCALER, 15, 0);    // Defines a prescaler value for the baud rate of the Serial Wire Output

            void Set_SWO_Prescaler(uint16_t prescale) {
                *this = prescale < SHIFT_SWOPRESCALER;
            }

        private:
            uint32_t m_Address;
        };

    } // namespace M4::TPIU

} // namespace HWD::Cortex