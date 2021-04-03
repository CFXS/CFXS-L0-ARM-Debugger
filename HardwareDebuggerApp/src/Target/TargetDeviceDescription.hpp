#pragma once

namespace HWD {

    class TargetDeviceDescription {
    public:
        class MemoryRegion {
        public:
            enum class AccessPermissions : uint8_t {
                READ       = 0x01,
                WRITE      = 0x02,
                READ_WRITE = 0x03,
            };

        public:
            MemoryRegion(const char* name, uint32_t address, uint32_t size, AccessPermissions accessPermissions) :
                m_Name(name), m_Address(address), m_Size(size), m_AccessPermissions(accessPermissions) {
            }

            const char* GetName() const {
                return m_Name;
            }
            uint32_t GetAddress() const {
                return m_Address;
            }
            uint32_t GetSize() const {
                return m_Size;
            }
            AccessPermissions GetAccessPermissions() const {
                return m_AccessPermissions;
            }

        private:
            const char* m_Name;
            uint32_t m_Address;
            uint32_t m_Size;
            AccessPermissions m_AccessPermissions;
        };

        TargetDeviceDescription() = default;
        TargetDeviceDescription(const char* name, const std::vector<MemoryRegion>& memoryRegions) :
            m_Name(name), m_MemoryRegions(memoryRegions) {
        }

        const char* GetName() const {
            return m_Name;
        }

        const std::vector<MemoryRegion>& GetMemoryRegions() const {
            return m_MemoryRegions;
        }

        inline void operator=(const TargetDeviceDescription& other) {
            m_Name          = other.m_Name;
            m_MemoryRegions = other.m_MemoryRegions;
        }

    private:
        const char* m_Name = "Unknown";
        std::vector<MemoryRegion> m_MemoryRegions;
    };

    inline const char* To_C_String(TargetDeviceDescription::MemoryRegion::AccessPermissions type) {
        switch (type) {
            case TargetDeviceDescription::MemoryRegion::AccessPermissions::READ: return "R";
            case TargetDeviceDescription::MemoryRegion::AccessPermissions::WRITE: return "W";
            case TargetDeviceDescription::MemoryRegion::AccessPermissions::READ_WRITE: return "R/W";
            default: return "Unknown";
        }
    }

} // namespace HWD