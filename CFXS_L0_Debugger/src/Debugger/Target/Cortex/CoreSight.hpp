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

namespace L0::Cortex::CoreSight {

    static constexpr uint32_t OFFSET_PERIPHERAL_ID4 = 0xFD0;
    static constexpr uint32_t OFFSET_PERIPHERAL_ID5 = 0xFD4;
    static constexpr uint32_t OFFSET_PERIPHERAL_ID6 = 0xFD8;
    static constexpr uint32_t OFFSET_PERIPHERAL_ID7 = 0xFDC;
    static constexpr uint32_t OFFSET_PERIPHERAL_ID0 = 0xFE0;
    static constexpr uint32_t OFFSET_PERIPHERAL_ID1 = 0xFE4;
    static constexpr uint32_t OFFSET_PERIPHERAL_ID2 = 0xFE8;
    static constexpr uint32_t OFFSET_PERIPHERAL_ID3 = 0xFEC;

    static constexpr uint32_t OFFSET_COMPONENT_ID3 = 0xFFC; // CS Component ID register 3
    static constexpr uint32_t OFFSET_COMPONENT_ID2 = 0xFF8; // CS Component ID register 2
    static constexpr uint32_t OFFSET_COMPONENT_ID1 = 0xFF4; // CS Component ID register 1
    static constexpr uint32_t OFFSET_COMPONENT_ID0 = 0xFF0; // CS Component ID register 0

    static constexpr uint32_t OFFSET_DEVTYPE = 0xFCC; // CS Device Type register
    static constexpr uint32_t OFFSET_DEVID   = 0xFC8; // CS Device ID register

    static constexpr uint32_t OFFSET_DEBUGID                      = 0x000;
    static constexpr uint32_t VAL_DEBUGID_DEVID_IMPLEMENTED       = 0x00008000; // is DEVID implemented
    static constexpr uint32_t VAL_DEBUGID_PC_SAMPLING_IMPLEMENTED = 0x00002000; // is PC sampling implemented

    static constexpr uint32_t OFFSET_LOCK_ACCESS = 0xFB0;      // CS component Software Lock access register
    static constexpr uint32_t OFFSET_LOCK_STATUS = 0xFB4;      // CS component Software Lock status register
    static constexpr uint32_t VAL_UNLOCK_KEY     = 0xC5ACCE55; // CS component Software Lock - unlock key value

    static constexpr uint32_t CLASS_ROMTABLE  = 0x01; // CoreSight Class value for ROM table
    static constexpr uint32_t CLASS_CORESIGHT = 0x09; // CoreSight Class value for CoreSight component
    static constexpr uint32_t CLASS_GENERIC   = 0x0E; // CoreSight Class value for Generic component
    static constexpr uint32_t CLASS_PRIMECELL = 0x0F; // CoreSight Class value for Primecell component table

} // namespace L0::Cortex::CoreSight