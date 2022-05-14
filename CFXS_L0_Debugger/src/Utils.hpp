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

#define DEF_REG_OPERATORS()                                                                       \
    inline void operator=(uint32_t val) {                                                         \
        Probe::I_Probe::GetCurrentProbe()->Target_WriteMemory_32(m_Address, val);                 \
    }                                                                                             \
    inline void operator|=(uint32_t val) {                                                        \
        Probe::I_Probe::GetCurrentProbe()->Target_WriteMemory_32(                                 \
            m_Address, Probe::I_Probe::GetCurrentProbe()->Target_ReadMemory_32(m_Address) | val); \
    }                                                                                             \
    inline void operator&=(uint32_t val) {                                                        \
        Probe::I_Probe::GetCurrentProbe()->Target_WriteMemory_32(                                 \
            m_Address, Probe::I_Probe::GetCurrentProbe()->Target_ReadMemory_32(m_Address) & val); \
    }                                                                                             \
    inline uint32_t GetRaw() {                                                                    \
        return Probe::I_Probe::GetCurrentProbe()->Target_ReadMemory_32(m_Address);                \
    }

#define DEF_REG_CONSTRUCTOR(name)                 \
    name(uint32_t regAddr) : m_Address(regAddr) { \
    }                                             \
    constexpr void SetAddress(uint32_t addr) {    \
        m_Address = addr;                         \
    }

#define DEF_REG_FIELD(name, highBitIndex, lowBitIndex)                                                                                     \
    static constexpr uint32_t MASK_##name  = (((0xFFFFFFFF >> lowBitIndex) << lowBitIndex) << (31 - highBitIndex)) >> (31 - highBitIndex); \
    static constexpr uint32_t SHIFT_##name = lowBitIndex;                                                                                  \
    inline uint32_t Get_##name() const {                                                                                                   \
        return (Probe::I_Probe::GetCurrentProbe()->Target_ReadMemory_32(m_Address) & MASK_##name) >> SHIFT_##name;                         \
    }                                                                                                                                      \
    inline uint32_t GetRaw_##name(uint32_t val) const {                                                                                    \
        return (val & MASK_##name) >> SHIFT_##name;                                                                                        \
    }

#define L0_OVERLOAD_ENUM_BITWISE_OPERATORS(enumName)                                                                        \
    constexpr enumName operator|(enumName a, enumName b) {                                                                  \
        return static_cast<enumName>(static_cast<std::underlying_type<enumName>::type>(a) |                                 \
                                     static_cast<std::underlying_type<enumName>::type>(b));                                 \
    }                                                                                                                       \
    constexpr bool operator&(enumName a, enumName b) {                                                                      \
        return static_cast<std::underlying_type<enumName>::type>(a) & static_cast<std::underlying_type<enumName>::type>(b); \
    }