// [CFXS] //
#pragma once

namespace L0::DWARF {

    class DIE {
    private:
#pragma pack(1)
        // Compilation Unit Header
        template<typename T>
        struct _CU_Header {
            uint16_t version;    // version of the DWARF information for the compilation unit
            T abbrevOffset;      // Associates the compilation unit with a particular set of debugging information entry abbreviations
            uint8_t addressSize; // size in bytes of an address on the target architecture
        };

        // Type Unit Header
        template<typename T>
        struct _TU_Header {
            uint16_t version;       // version of the DWARF information for the compilation unit
            T abbrevOffset;         // Associates the compilation unit with a particular set of debugging information entry abbreviations
            uint8_t addressSize;    // size in bytes of an address on the target architecture
            uint64_t typeSignature; // Unique signature of the type described in this type unit
            T typeOffset;           // Refers to the debugging information entry that describes the type
        };
#pragma pack()
    };

} // namespace L0::DWARF