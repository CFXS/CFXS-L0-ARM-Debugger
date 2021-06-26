#pragma once

namespace HWD::ELF {

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

} // namespace HWD::ELF