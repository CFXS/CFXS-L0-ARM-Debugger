#pragma once
#include "ELF.hpp"
#include "ELF32.hpp"

namespace HWD::ELF {

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
                HWDLOG_CORE_CRITICAL("Section index out of range ({0} >= {1})", sectionIndex, m_SectionData.size());
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

} // namespace HWD::ELF