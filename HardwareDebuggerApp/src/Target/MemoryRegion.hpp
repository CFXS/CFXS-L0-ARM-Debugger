#pragma once

namespace HWD::Target {

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

} // namespace HWD::Target