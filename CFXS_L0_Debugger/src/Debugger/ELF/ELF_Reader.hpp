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
#include <functional>

namespace L0::ELF {

    class ELF_Reader {
    public:
        struct SymbolTableEntry {
            std::string fullName;
            std::string name;
            uint64_t address; // symbol address in target memory - 64bit for future 64bit core support
            uint64_t size;    // symbol size in bytes - 64bit for future 64bit core support
        };

    public:
        ELF_Reader(const std::string& path);
        ~ELF_Reader() = default;

        void SetFilePath(const std::string& path);
        bool IsValid() const;
        bool IsLittleEndian();
        bool IsBigEndian();
        bool Is32bit() const;
        bool Is64bit() const;

        /// Load file from path and do initial parsing
        /// \returns true on success
        bool LoadFile();

        /// Load basic symbols from .symtab
        bool LoadBasicSymbols();

        /// Get basic symbol table
        const std::unordered_map<std::string, SymbolTableEntry>& GetBasicSymbolTable() const {
            return m_BasicSymbolTable;
        }

    private:
        bool ParseFile();

        bool ParseFile32();
        bool ParseFile64();

        bool LoadBasicSymbols32();

        /// Get symbol name from string section at offset
        const char* GetSymbolName(int sectionIndex, size_t nameOffset) const;

        /// Get section name from section name table offset
        const char* GetSectionName(size_t sectionNameTableOffset) const;

        /// Get section name from section name from section header
        const char* GetSectionName(const ELF32::SectionHeader* sectionHeader) const {
            return GetSectionName(sectionHeader->nameOffset);
        }

        /// Get section index by name
        /// \param sectionName name of section to find (case sensitive)
        /// \returns section index if found, -1 if not found
        inline int GetSectionIndex(const std::string& sectionName) const {
            auto it = m_SectionNameIndexMap.find(sectionName);
            if (it != std::end(m_SectionNameIndexMap)) {
                return it->second;
            } else {
                return -1;
            }
        }

        /// Get section from section index
        template<typename T>
        inline const T* GetSection(int sectionIndex) const {
            if (sectionIndex == -1) {
                LOG_CORE_ERROR("Invalid section index\n");
                return nullptr;
            }
            if (m_FileClass == ELF::FileClass::_32) {
                return reinterpret_cast<const T*>(m_RawData + m_ELF_Header.elf32->sectionHeaderOffset +
                                                  sectionIndex * m_ELF_Header.elf32->sectionTableEntrySize);
            } else {
                return nullptr;
            }
        }

        /// Get section from section name
        template<typename T>
        inline const T* GetSection(const std::string& sectionName) const {
            if (m_FileClass == ELF::FileClass::_32) {
                auto index = GetSectionIndex(sectionName);
                if (index != -1) {
                    return GetSection<T>(index);
                } else {
                    return nullptr;
                }
            } else {
                return nullptr;
            }
        }

        /// Get section data from section index
        template<typename T>
        inline const T* GetSectionData(int sectionIndex) const {
            if (sectionIndex == -1) {
                LOG_CORE_ERROR("Invalid section index\n");
                return nullptr;
            }

            if (sectionIndex >= m_SectionData.size()) {
                LOG_CORE_CRITICAL("Section index out of range ({0} >= {1})", sectionIndex, m_SectionData.size());
                return nullptr;
            } else {
                return reinterpret_cast<const T*>(m_SectionData[sectionIndex]);
            }
        }

        /// Get section data from section (ELF32)
        template<typename T>
        inline const T* GetSectionData(const ELF32::SectionHeader* section) const {
            if (!section) {
                LOG_CORE_ERROR("Invalid section\n");
                return nullptr;
            }

            return reinterpret_cast<const T*>(m_RawData + section->offsetInFile);
        }

        /// Get section data from section name
        template<typename T>
        inline const T* GetSectionData(const std::string& sectionName) const {
            auto index = GetSectionIndex(sectionName);
            if (index != -1) {
                return GetSectionData<T>(index);
            } else {
                return nullptr;
            }
        }

        /// Get list of sections located in physical target memory
        std::vector<const ELF32::SectionHeader*> GetTargetMemorySections() const;

        /// Iterate over each section
        using ForEachSectionSig32 =
            std::function<void(int sectionIndex, const ELF32::SectionHeader* sectionHeader, const char* sectionName)>;
        void ForEachSection(const ForEachSectionSig32& fn) const;

    private:
        std::string m_Path;                // path to source elf
        std::vector<uint8_t> m_DataVector; // source elf data
        const uint8_t* m_RawData;          // raw data pointer
        bool m_Valid = false;              // is file valid

        // ELF Stuff
        union {
            const ELF32::Header* elf32;
            const void* ptr;
        } m_ELF_Header;
        ELF::DataEncoding m_DataEncoding;          // Little/big endian
        ELF::FileClass m_FileClass;                // 32bit/64bit
        std::vector<const uint8_t*> m_SectionData; // Pointers to section data blocks
        const char* m_SectionNameTable;            // Pointer to section name table

        // Content
        std::unordered_map<std::string, int> m_SectionNameIndexMap;           // Section names to section indexes
        std::unordered_map<std::string, SymbolTableEntry> m_BasicSymbolTable; // All symbols from .symtab

        std::vector<uint8_t> m_LoadableBinary; // Target firmware file
    };

} // namespace L0::ELF