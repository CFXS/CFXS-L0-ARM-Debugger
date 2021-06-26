#pragma once
#include "ELF.hpp"

// This is not a general purpose ELF library. This is HWD specific stuff.

namespace HWD::ELF32 {

    using Word       = uint32_t;
    using SignedWord = int32_t;
    using HalfWord   = uint16_t;
    using Address    = Word;
    using Offset     = Word;

    /////////////////////////////////////////////////////////////////////////////////////////
    // Docs/elf_spec.pdf Figure 1-3 ELF32 Header
    enum class FileType : HalfWord {
        NONE        = 0, // No file type
        RELOCATABLE = 1, // Relocatable file
        EXECUTABLE  = 2, // Executable file
        DYNAMIC     = 3, // Shared object file
        CORE        = 4  // Core file
    };

#pragma pack(1)
    static constexpr uint32_t MAGIC_NUMBER = (0x7F << 0) | ((uint32_t)'E' << 8) | ((uint32_t)'L' << 16) | ((uint32_t)'F' << 24);
    struct Header {
        union { // File identification
            struct {
                uint32_t magicNumber;
                ELF::FileClass fileClass;
                ELF::DataEncoding dataEncoding;
                uint8_t version; // must be 1
                uint8_t padding[8];
                uint8_t identifierSize; // sizeof(identifier)
            };
            uint8_t value[16];
        } identifier;
        FileType type;              // Object file type
        HalfWord machine;           // Architecture [in this case, only important value is 0 (ET_NONE - No machine)]
        Word version;               // Version 0 = invalid, Version 1 = current, Version > 1 = extension
        Address entryPoint;         // Initial PC value
        Offset programHeaderOffset; // Program header table offset in file [0 = no table]
        Offset sectionHeaderOffset; // Section header offset in file [0 = no table]
        Word flags;                 // Processor specific flags
        HalfWord headerSize;        // ELF header size in bytes
        HalfWord programTableEntrySize;
        HalfWord programTableEntryCount;
        HalfWord sectionTableEntrySize;
        HalfWord sectionTableEntryCount;
        HalfWord sectionTableNameEntryIndex; // Section table entry index for table containing section names
    };
#pragma pack()

    /////////////////////////////////////////////////////////////////////////////////////////
    // Docs/elf_spec.pdf Figure 1-8 Section Header

    enum class SectionType : Word {
        _NULL = 0,             // No associated section
        PROGBITS,              // Defined by the program
        SYMBOL_TABLE,          // Symbol table
        STRING_TABLE,          // String table
        RELA,                  // Relocation table entries with explicit addends
        HASH,                  // Symbol hash table
        DYNAMIC,               // Dynamic linking info
        NOTE,                  // Information that marks the file in some way
        NOBITS,                // The section occupies no space, but SectionTable::offset specifies a conceptual file offset
        REL,                   // Relocation table entries without explicit addends
        SHLIB,                 // Reserved
        DYNNAMIC_SYMBOL_TABLE, // Dynamic symbol table
        // User and processor specific values are ignored for this project
    };

    enum class SectionFlags : Word {
        WRITE                   = 0x01,
        ALLOC                   = 0x02,
        EXECUTABLE_INSTRUCTIONS = 0x04,
    };
    HWD_OVERLOAD_ENUM_BITWISE_OPERATORS(SectionFlags, Word);

#pragma pack(1)
    struct SectionHeader {
        Word nameOffset; // Section name is an index into the section header name string table [Header::sectionTableNameEntryIndex]
        SectionType type;
        SectionFlags flags;
        Address address;
        Offset offsetInFile; // Offset from beginning of the ELF file to section data
        Word size;           // Section size in bytes
        Word link;           // Header table link index (interpretation based on section type)
        Word info;           // Info depends on section type
        Word addressAlignment;
        Word entrySize;
    };
#pragma pack()

    /////////////////////////////////////////////////////////////////////////////////////////
    // Docs/elf_spec.pdf Figure 1-15 Symbol Table Entry

#pragma pack(1)
    struct SymbolEntry {
        Word nameOffset; // index in symbol name table
        Address value;   // in this project, this is the address of the symbol in memory
        Word size;
        struct {
            uint8_t value;
            inline ELF::SymbolBinding GetBinding() const {
                return static_cast<ELF::SymbolBinding>(value >> 4);
            }
            inline ELF::SymbolType GetType() const {
                return static_cast<ELF::SymbolType>(value & 0x0F);
            }
        } info;
        uint8_t other;
        HalfWord nameTableSectionIndex; // section index of table containing symbol names for this symbol entry

        inline bool IsFunction() const {
            return info.GetType() == ELF::SymbolType::FUNCTION;
        }

        inline bool IsObject() const {
            return info.GetType() == ELF::SymbolType::OBJECT;
        }

        inline bool IsFile() const {
            return info.GetType() == ELF::SymbolType::FILE;
        }

        inline bool IsSection() const {
            return info.GetType() == ELF::SymbolType::SECTION;
        }

        inline bool IsNone() const {
            return info.GetType() == ELF::SymbolType::NONE;
        }
    };
#pragma pack()

} // namespace HWD::ELF32