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

namespace L0::ELF {

    enum class FileClass : uint8_t {
        NONE = 0, // Invalid
        _32  = 1, // 32bit objects
        _64  = 2  // 64bit objects
    };

    enum class DataEncoding : uint8_t {
        NONE = 0, // Invalid
        LSB  = 1,
        MSB  = 2
    };

    enum class SymbolBinding : uint8_t {
        LOCAL  = 0, // C/C++ static symbols (not visible from other files)
        GLOBAL = 1, // Global symbols visible from other files
        WEAK   = 2, // Same as global, but with lower precedence
        // processor specific values are ignored for this project
    };

    enum class SymbolType : uint8_t {
        NONE     = 0, // Type not specified
        OBJECT   = 1, // The symbol is associated with a data object (variable, array, ...)
        FUNCTION = 2, // The symbol is associated with a function/executable code
        SECTION  = 3, // The symbol is associated with a section (primarily used for relocation)
        FILE     = 4,
        // processor specific values are ignored for this project
    };

} // namespace L0::ELF