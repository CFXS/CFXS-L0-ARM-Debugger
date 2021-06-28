#include "ELF_Reader.hpp"

#include <llvm/Demangle/Demangle.h>

#include <fstream>

#include "ELF32.hpp"

namespace HWD::ELF {

    ELF_Reader::ELF_Reader(const std::string& path) : m_Path(path) {
    }

    bool ELF_Reader::LoadFile() {
        HWDLOG_CORE_TRACE("Load ELF file {0}", m_Path);

        std::ifstream fileStream(m_Path, std::ios::in | std::ios::binary);
        m_DataVector = std::vector<uint8_t>(std::istreambuf_iterator<char>(fileStream), std::istreambuf_iterator<char>());
        m_RawData    = m_DataVector.data();

        m_ELF_Header.ptr = reinterpret_cast<const void*>(m_RawData);

        if (m_RawData == nullptr) {
            HWDLOG_CORE_ERROR(" - Failed to open file ({0})", strerror(errno));
            return false;
        }

        HWDLOG_CORE_TRACE(" - File size: {0} bytes", m_DataVector.size());

        auto header = m_ELF_Header.elf32;

        // check if header is good
        if (header->identifier.magicNumber == ELF32::MAGIC_NUMBER) {
            HWDLOG_CORE_TRACE(" - Valid ELF header");
            if (ParseFile()) {
                m_Valid = true;
            } else {
                HWDLOG_CORE_ERROR(" - Failed to parse ELF file");
                m_Valid = false;
            }
        } else {
            HWDLOG_CORE_ERROR(
                " - File is not a valid ELF file. magicNumber[0x{0:4X} != 0x{1:4X}]", ELF32::MAGIC_NUMBER, header->identifier.magicNumber);
            m_Valid = false;
        }

        return m_Valid;
    }

    bool ELF_Reader::ParseFile() {
        m_SymbolMap.clear();
        m_SectionData.clear();

        auto header = m_ELF_Header.elf32;

        // check identifier version
        if (header->identifier.version != 1) {
            HWDLOG_CORE_ERROR(" - Invalid identifier version ({0})", header->identifier.version);
            return false;
        }

        // check data encoding
        m_DataEncoding = header->identifier.dataEncoding;

        if (m_DataEncoding != ELF::DataEncoding::LSB && m_DataEncoding != ELF::DataEncoding::MSB) {
            HWDLOG_CORE_ERROR(" - Invalid data encoding ({0})", (uint8_t)m_DataEncoding);
            return false;
        }

        if (m_DataEncoding == ELF::DataEncoding::LSB) {
            HWDLOG_CORE_TRACE(" - Little endian");
        } else {
            HWDLOG_CORE_TRACE(" - Big endian");
        }

        // check if this is a 32bit or a 64bit file
        m_FileClass = header->identifier.fileClass;
        if (m_FileClass == ELF::FileClass::_32) {
            HWDLOG_CORE_TRACE(" - 32bit file");
            return ParseFile32();
        } else if (m_FileClass == ELF::FileClass::_64) {
            HWDLOG_CORE_TRACE(" - 64bit file");
            return ParseFile64();
        } else {
            HWDLOG_CORE_ERROR(" - Invalid file class ({0})", (uint8_t)m_FileClass);
            return false;
        }
    }

    bool ELF_Reader::ParseFile32() {
        auto header = m_ELF_Header.elf32;

        // check if file type is executable
        if (header->type != ELF32::FileType::EXECUTABLE) {
            HWDLOG_CORE_ERROR(" - Not an executable file");
            return false;
        }

        // check if machine is AARCH32 (Arm 32bit architecture)
        if (header->machine != 0x0028) {
            HWDLOG_CORE_ERROR(" - ELF files for machine types other than ARM32 are not supported (0x{0:04X})", header->machine);
            return false;
        }

        // check if version is current or above
        if (header->version != 1) {
            HWDLOG_CORE_ERROR(" - Invalid ELF file version ({0})", header->version);
            return false;
        }

        HWDLOG_CORE_TRACE(" - Program Table: {0} entries of size {1} at offset 0x{2:X}",
                          header->programTableEntryCount,
                          header->programTableEntrySize,
                          header->programHeaderOffset);
        HWDLOG_CORE_TRACE(" - Section Table: {0} entries of size {1} at offset 0x{2:X}",
                          header->sectionTableEntryCount,
                          header->sectionTableEntrySize,
                          header->sectionHeaderOffset);

        // section name table
        if (header->sectionTableNameEntryIndex) {
            auto sectionHeader = GetSection<ELF32::SectionHeader>(header->sectionTableNameEntryIndex);
            m_SectionNameTable = (const char*)(m_RawData + sectionHeader->offsetInFile);
        } else {
            m_SectionNameTable = nullptr;
        }

        // sections
        // 1 - load all section data offsets to vector
        m_SectionData.reserve(header->sectionTableEntryCount);

        for (int section_index = 0; section_index < header->sectionTableEntryCount; section_index++) {
            auto sectionHeader = GetSection<ELF32::SectionHeader>(section_index);
            m_SectionData.push_back(m_RawData + sectionHeader->offsetInFile);
        }

        // 2 - process sections
        if (header->sectionHeaderOffset) {
            for (int section_index = 0; section_index < header->sectionTableEntryCount; section_index++) {
                auto sectionHeader = GetSection<ELF32::SectionHeader>(section_index);

                HWDLOG_CORE_TRACE("Section {0} \"{1}\" at offset {2:X}",
                                  section_index,
                                  GetSectionName(sectionHeader->nameOffset),
                                  sectionHeader->offsetInFile);

                if (sectionHeader->type == ELF32::SectionType::SYMBOL_TABLE) {
                    if (!LoadSymbols32(sectionHeader)) {
                        HWDLOG_CORE_ERROR(" - Failed to load symbols");
                        return false;
                    }
                }
            }
        }

        if (header->programHeaderOffset) {
        }

        return true;
    }

    bool ELF_Reader::LoadSymbols32(const ELF32::SectionHeader* section) {
        HWDLOG_CORE_TRACE(" - Load symbol table");
        size_t entryCount = section->size / section->entrySize;

        HWDLOG_CORE_TRACE("   > Symbol table contains {0} symbols", entryCount);

        for (int symbol_index = 0; symbol_index < entryCount; symbol_index++) {
            auto symbolEntry =
                reinterpret_cast<const ELF32::SymbolEntry*>(m_RawData + section->offsetInFile + symbol_index * section->entrySize);

            // section->link is section index for string table
            auto symbolNameMangled = GetSymbolName(section->link, symbolEntry->nameOffset);

            char symName[512];
            char mangleWorkBuf[512];
            size_t mangleBufSize = 512;

            // only demangle object and function names
            if ((symbolEntry->IsFunction() || symbolEntry->IsObject())) {
                if (char* itanium = llvm::itaniumDemangle(symbolNameMangled, mangleWorkBuf, &mangleBufSize, NULL)) {
                    snprintf(symName, sizeof(symName), itanium);
                } else {
                    // forgot to add this else
                    // this caused way too many duplicate symbols, because symName was not updating when this failed
                    // demangle fails for some C functions where the result is supposed to be the same as the input
                    snprintf(symName, sizeof(symName), symbolNameMangled);
                }
            } else {
                snprintf(symName, sizeof(symName), symbolNameMangled);
            }

            char* type;
            switch (symbolEntry->info.GetBinding()) {
                case ELF::SymbolBinding::GLOBAL: type = "GLOBAL"; break;
                case ELF::SymbolBinding::LOCAL: type = "LOCAL"; break;
                case ELF::SymbolBinding::WEAK: type = "WEAK"; break;
            }

            /*HWDLOG_CORE_INFO("   > Symbol {0:5} {1:8} size {2:6} name {3} at 0x{4:08X} type {5}",
                                 symbol_index,
                                 symbolEntry->info.GetType() == ELF::SymbolType::FUNCTION ? "Function" : "Object",
                                 symbolEntry->size,
                                 symName,
                                 symbolEntry->value,
                                 type);*/

            if (m_SymbolMap.find(symName) == m_SymbolMap.end()) {
                SymbolInfo symInfo;
                symInfo.fullName = symName;

                char* firstSpace = symName;
                /*size_t symLen    = strlen(symName);
                for (size_t i = 0; i < symLen; i++) {
                    if (symName[i] == '(') {
                        symName[i] = 0;
                        break;
                    }
                }*/

                if (strcmp(
                        firstSpace,
                        "std::__detail::_Map_base<unsigned long, std::pair<unsigned long const, CFXS::TaskArrayEntry>, std::allocator<std::pair<unsigned long const, CFXS::TaskArrayEntry> >, std::__detail::_Select1st, std::equal_to<unsigned long>, std::hash<unsigned long>, std::__detail::_Mod_range_hashing, std::__detail::_Default_ranged_hash, std::__detail::_Prime_rehash_policy, std::__detail::_Hashtable_traits<false, false, true>, true>::operator[](unsigned long const&)") ==
                    0) {
                    firstSpace = "std::map<unsigned long, CFXS::TaskArrayEntry>::operator[](unsigned long const&)";
                }

                symInfo.name         = firstSpace;
                symInfo.address      = symbolEntry->value - 1;
                symInfo.size         = symbolEntry->size;
                m_SymbolMap[symName] = symInfo;
            }
        }

        HWDLOG_CORE_TRACE("   > {0} symbols loaded", m_SymbolMap.size());

        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////

    bool ELF_Reader::ParseFile64() {
        HWDLOG_CORE_ERROR(" - 64bit files are not supported yet");
        return false;
    }

    //////////////////////////////////////////////////////////////////////////

    const char* ELF_Reader::GetSectionName(size_t sectionNameTableOffset) const {
        if (sectionNameTableOffset == 0) {
            return "[no name]";
        }

        if (m_SectionNameTable == nullptr) {
            return "[null]";
        }

        return m_SectionNameTable + sectionNameTableOffset;
    }

    const char* ELF_Reader::GetSymbolName(size_t sectionIndex, size_t nameOffset) const {
        return GetSectionData<char>(sectionIndex) + nameOffset;
    }

    const ELF_Reader::SymbolInfo* ELF_Reader::AddressToSymbol(uint64_t addr) {
        for (auto& [name, sym] : GetSymbolMap()) {
            if (addr == sym.address)
                return &sym;
            if (addr >= sym.address && addr < (sym.address + sym.size))
                return &sym;
        }

        return nullptr;
    }

} // namespace HWD::ELF