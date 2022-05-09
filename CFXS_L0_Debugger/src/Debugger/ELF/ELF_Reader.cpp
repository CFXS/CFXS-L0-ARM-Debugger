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
#include "ELF_Reader.hpp"
#include "ELF32.hpp"
#include <llvm/Demangle/Demangle.h>
#include <fstream>
#include <QFile>

namespace L0::ELF {

    ELF_Reader::ELF_Reader(const std::string& path) : m_Path(path) {
    }

    bool ELF_Reader::LoadFile() {
        LOG_CORE_TRACE("Load ELF file {0}", m_Path);

        std::ifstream fileStream(m_Path, std::ios::in | std::ios::binary);
        m_DataVector = std::vector<uint8_t>(std::istreambuf_iterator<char>(fileStream), std::istreambuf_iterator<char>());
        m_RawData    = m_DataVector.data();

        m_ELF_Header.ptr = reinterpret_cast<const void*>(m_RawData);

        if (m_RawData == nullptr) {
            LOG_CORE_ERROR(" - Failed to open file ({0})", strerror(errno));
            return false;
        }

        LOG_CORE_TRACE(" - File size: {0} bytes", m_DataVector.size());

        auto header = m_ELF_Header.elf32;

        // check if header is good
        if (header->identifier.magicNumber == ELF32::MAGIC_NUMBER) {
            LOG_CORE_TRACE(" - Valid ELF header");
            if (ParseFile()) {
                m_Valid = true;
            } else {
                LOG_CORE_ERROR(" - Failed to parse ELF file");
                m_Valid = false;
            }
        } else {
            LOG_CORE_ERROR(
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
            LOG_CORE_ERROR(" - Invalid identifier version ({0})", header->identifier.version);
            return false;
        }

        // check data encoding
        m_DataEncoding = header->identifier.dataEncoding;

        if (m_DataEncoding != ELF::DataEncoding::LSB && m_DataEncoding != ELF::DataEncoding::MSB) {
            LOG_CORE_ERROR(" - Invalid data encoding ({0})", (uint8_t)m_DataEncoding);
            return false;
        }

        if (m_DataEncoding == ELF::DataEncoding::LSB) {
            LOG_CORE_TRACE(" - Little endian");
        } else {
            LOG_CORE_TRACE(" - Big endian");
            LOG_CORE_ERROR(" - Big endian ELF files are not supported yet");
            return false;
        }

        // check if this is a 32bit or a 64bit file
        m_FileClass = header->identifier.fileClass;
        if (m_FileClass == ELF::FileClass::_32) {
            LOG_CORE_TRACE(" - 32bit file");
            return ParseFile32();
        } else if (m_FileClass == ELF::FileClass::_64) {
            LOG_CORE_TRACE(" - 64bit file");
            return ParseFile64();
        } else {
            LOG_CORE_ERROR(" - Invalid file class ({0})", (uint8_t)m_FileClass);
            return false;
        }
    }

    bool ELF_Reader::ParseFile32() {
        auto header = m_ELF_Header.elf32;

        // check if file type is executable
        if (header->type != ELF32::FileType::EXECUTABLE) {
            LOG_CORE_ERROR(" - Not an executable file");
            return false;
        }

        // check if machine is AARCH32 (Arm 32bit architecture)
        if (header->machine != 0x0028) {
            LOG_CORE_ERROR(" - ELF files for machine types other than ARM32 are not supported (0x{0:04X})", header->machine);
            return false;
        }

        // check if version is current or above
        if (header->version != 1) {
            LOG_CORE_ERROR(" - Invalid ELF file version ({0})", header->version);
            return false;
        }

        LOG_CORE_TRACE(" - Program Table: {0} entries of size {1} at offset 0x{2:X}",
                       header->programTableEntryCount,
                       header->programTableEntrySize,
                       header->programHeaderOffset);
        LOG_CORE_TRACE(" - Section Table: {0} entries of size {1} at offset 0x{2:X}",
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

        for (int sectionIndex = 0; sectionIndex < header->sectionTableEntryCount; sectionIndex++) {
            auto sectionHeader = GetSection<ELF32::SectionHeader>(sectionIndex);
            m_SectionData.push_back(m_RawData + sectionHeader->offsetInFile);
        }

        // 2 - process sections
        if (header->sectionHeaderOffset) {
            bool symbolsLoaded = false;
            ForEachSection([&](int sectionIndex, const ELF32::SectionHeader* section, const char* sectionName) {
                m_SectionNameIndexMap[sectionName] = sectionIndex;
                LOG_CORE_TRACE("Section {} \"{}\" ({}/{})", sectionIndex, sectionName, ToString(section->type), ToString(section->flags));

                if (section->type == ELF32::SectionType::SYMBOL_TABLE) {
                    symbolsLoaded = LoadSymbols32(section);
                }
            });

            if (!symbolsLoaded) {
                LOG_CORE_ERROR(" - Failed to load symbols");
                return false;
            }
        }

        // 3 - print target physical memory sections and prepare loadable file
        m_LoadableBinary.clear();
        m_LoadableBinary.shrink_to_fit();

        auto targetMemSections = GetTargetMemorySections();
        if (targetMemSections.empty()) {
            LOG_CORE_TRACE("Target physical memory sections not found");
        } else {
            LOG_CORE_TRACE("Target physical memory sections:");
            for (auto section : targetMemSections) {
                auto name          = GetSectionName(section);
                QString memTypeStr = [&]() {
                    switch (section->flags) {
                        case ELF32::SectionFlags::RX: return "[FLASH]";
                        case ELF32::SectionFlags::RW:
                        case ELF32::SectionFlags::RWX: return "[RAM]  ";
                        case ELF32::SectionFlags::R: return "[ROM]  ";
                        default: return ToString(section->flags);
                    }
                }();

                LOG_CORE_TRACE(" - {:8} {} 0x{:08X}-0x{:08X} ({} bytes)",
                               name,
                               memTypeStr,
                               section->address,
                               section->address + section->size,
                               section->size);

                // if section is a part of loadable firmware
                if ((section->type == ELF32::SectionType::PROGBITS) && section->size) {
                    try {
                        auto sectMaxSize = section->address + section->size;
                        if (sectMaxSize) {
                            if (sectMaxSize > m_LoadableBinary.size())
                                m_LoadableBinary.resize(sectMaxSize);

                            auto blockData = GetSectionData<uint8_t>(section);
                            if (!blockData) {
                                LOG_CORE_ERROR("Failed to get loadable binary section data ({})", name);
                                return false;
                            }

                            memcpy(m_LoadableBinary.data() + section->address, blockData, section->size);
                        } else {
                            LOG_CORE_WARN("Skip section {} - size is 0", name);
                        }
                    } catch (const std::exception& e) {
                        LOG_CORE_CRITICAL("Failed to resize ELF loadable binary buffer ({})", e.what());
                        return false;
                    }
                }
            }

            LOG_CORE_TRACE("Loadable binary size: {} bytes", m_LoadableBinary.size());
            LOG_CORE_TRACE("Saving temp .bin as {}.l0.bin", m_Path);

            QFile tempBin(QString(m_Path.c_str()) + ".l0.bin");
            tempBin.open(QIODevice::ReadWrite);
            tempBin.write((const char*)m_LoadableBinary.data(), m_LoadableBinary.size());
            tempBin.close();
        }

        if (header->programHeaderOffset) {
        }

        return true;
    }

    /// Iterate over each section [ELF32]
    void ELF_Reader::ForEachSection(const ForEachSectionSig32& fn) const {
        if (!m_ELF_Header.elf32) {
            LOG_CORE_ERROR("Can not iterate ELF32 sections - header is nullptr");
            return;
        }

        for (int sectionIndex = 0; sectionIndex < m_ELF_Header.elf32->sectionTableEntryCount; sectionIndex++) {
            auto sectionHeader = GetSection<ELF32::SectionHeader>(sectionIndex);
            auto sectionName   = GetSectionName(sectionHeader->nameOffset);
            fn(sectionIndex, sectionHeader, sectionName);
        }
    }

    bool ELF_Reader::LoadSymbols32(const ELF32::SectionHeader* section) {
        LOG_CORE_TRACE(" - Load symbol table");
        size_t entryCount = section->size / section->entrySize;

        LOG_CORE_TRACE("   > Symbol table contains {0} symbols", entryCount);

        for (int symbol_index = 0; symbol_index < entryCount; symbol_index++) {
            auto symbolEntry =
                reinterpret_cast<const ELF32::SymbolEntry*>(m_RawData + section->offsetInFile + symbol_index * section->entrySize);

            // section->link is section index for string table
            auto symbolNameMangled = GetSymbolName(section->link, symbolEntry->nameOffset);

            static char symName[4096];
            static char mangleWorkBuf[4096];
            size_t mangleBufSize = sizeof(mangleWorkBuf);

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
                continue;
                //snprintf(symName, sizeof(symName), symbolNameMangled);
            }

            const char* type;
            switch (symbolEntry->info.GetBinding()) {
                case ELF::SymbolBinding::GLOBAL: type = "GLOBAL"; break;
                case ELF::SymbolBinding::LOCAL: type = "LOCAL"; break;
                case ELF::SymbolBinding::WEAK: type = "WEAK"; break;
            }

            /*LOG_CORE_INFO("   > Symbol {0:5} {1:8} size {2:6} name {3} at 0x{4:08X} type {5}",
                                 symbol_index,
                                 symbolEntry->info.GetType() == ELF::SymbolType::FUNCTION ? "Function" : "Object",
                                 symbolEntry->size,
                                 symName,
                                 symbolEntry->value,
                                 type);*/

            if (m_SymbolMap.find(symName) == m_SymbolMap.end()) {
                SymbolInfo symInfo;
                symInfo.fullName = symName;

                const char* firstSpace = symName;
                /*size_t symLen    = strlen(symName);
                for (size_t i = 0; i < symLen; i++) {
                    if (symName[i] == '(') {
                        symName[i] = 0;
                        break;
                    }
                }*/

                //if (strcmp(
                //        firstSpace,
                //        "std::__detail::_Map_base<unsigned long, std::pair<unsigned long const, CFXS::TaskArrayEntry>, std::allocator<std::pair<unsigned long const, CFXS::TaskArrayEntry> >, std::__detail::_Select1st, std::equal_to<unsigned long>, std::hash<unsigned long>, std::__detail::_Mod_range_hashing, std::__detail::_Default_ranged_hash, std::__detail::_Prime_rehash_policy, std::__detail::_Hashtable_traits<false, false, true>, true>::operator[](unsigned long const&)") ==
                //    0) {
                //    firstSpace = "std::map<unsigned long, CFXS::TaskArrayEntry>::operator[](unsigned long const&)";
                //}

                symInfo.name         = firstSpace;
                symInfo.address      = symbolEntry->value;
                symInfo.size         = symbolEntry->size;
                m_SymbolMap[symName] = symInfo;
            }
        }

        LOG_CORE_TRACE("   > {0} symbols loaded", m_SymbolMap.size());

        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////

    bool ELF_Reader::ParseFile64() {
        LOG_CORE_ERROR(" - 64bit files are not supported yet");
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

    const char* ELF_Reader::GetSymbolName(int sectionIndex, size_t nameOffset) const {
        auto rawData = GetSectionData<char>(sectionIndex);
        if (!rawData) {
            return "[invalid section]";
        } else {
            return rawData + nameOffset;
        }
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

    /// Get list of sections (indexes) located in target memory
    std::vector<const ELF32::SectionHeader*> ELF_Reader::GetTargetMemorySections() const {
        std::vector<const ELF32::SectionHeader*> sections;

        if (m_FileClass == ELF::FileClass::_32) {
            ForEachSection([&](int idx, const ELF32::SectionHeader* section, const char* name) {
                // if section is PROGBITS(data) or NOBITS(space) and has access permission flags
                if ((section->type == ELF32::SectionType::PROGBITS || section->type == ELF32::SectionType::NOBITS) &&
                    (section->flags & ELF32::SectionFlags::ACCESS_PERMISSIONS_MASK)) {
                    sections.push_back(section);
                }
            });
        }

        return sections;
    }

} // namespace L0::ELF