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
#include "ELF.hpp"
#include "ELF32.hpp"

namespace L0::ELF {

    class ELF_Reader {
    public:
        struct SymbolInfo {
            std::string fullName;
            std::string name;
            uint64_t address; // symbol address in target memory - 64bit for future 64bit core support
            uint64_t size;    // symbol size in bytes - 64bit for future 64bit core support
        };

    public:
        ELF_Reader(const std::string& path);
        ~ELF_Reader() = default;

        inline void SetFilePath(const std::string& path) {
            m_Path = path;
        }

        inline bool IsValid() const {
            return m_Valid;
        }

        inline bool IsLittleEndian() {
            return m_DataEncoding == ELF::DataEncoding::LSB;
        }

        inline bool IsBigEndian() {
            return m_DataEncoding == ELF::DataEncoding::MSB;
        }

        bool LoadFile();

        const std::map<std::string, SymbolInfo>& GetSymbolMap() const {
            return m_SymbolMap;
        }

        const SymbolInfo* AddressToSymbol(uint64_t addr);

        const SymbolInfo* NameToSymbol(const char* symbolName) {
            for (auto& [name, info] : m_SymbolMap) {
                if (strcmp(info.name.c_str(), symbolName) == 0) {
                    return &info;
                }
            }

            return nullptr;
        }

    private:
        bool ParseFile();

        bool ParseFile32();
        bool LoadSymbols32(const ELF32::SectionHeader* section);

        bool ParseFile64();

        const char* GetSectionName(size_t sectionNameTableOffset) const;
        const char* GetSymbolName(size_t sectionIndex, size_t nameOffset) const;

        template<typename T>
        inline const T* GetSection(size_t sectionIndex) const {
            if (m_FileClass == ELF::FileClass::_32) {
                return reinterpret_cast<const T*>(m_RawData + m_ELF_Header.elf32->sectionHeaderOffset +
                                                  sectionIndex * m_ELF_Header.elf32->sectionTableEntrySize);
            } else {
                return nullptr;
            }
        }

        template<typename T>
        inline const T* GetSectionData(size_t sectionIndex) const {
            if (sectionIndex >= m_SectionData.size()) {
                LOG_CORE_CRITICAL("Section index out of range ({0} >= {1})", sectionIndex, m_SectionData.size());
                return nullptr;
            } else {
                return reinterpret_cast<const T*>(m_SectionData[sectionIndex]);
            }
        }

    private:
        std::string m_Path;                // path to source elf
        std::vector<uint8_t> m_DataVector; // source elf data
        const uint8_t* m_RawData;          // raw data pointer
        bool m_Valid = false;              // is file valid

        // ELF stuff
        union {
            const ELF32::Header* elf32;
            const void* ptr;
        } m_ELF_Header;
        ELF::DataEncoding m_DataEncoding; // Little/big endian
        ELF::FileClass m_FileClass;       // 32bit/64bit
        std::vector<const uint8_t*> m_SectionData;
        const char* m_SectionNameTable;

        //
        std::map<std::string, SymbolInfo> m_SymbolMap;
    };

} // namespace L0::ELF