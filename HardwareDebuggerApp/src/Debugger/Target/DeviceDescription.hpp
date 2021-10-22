// ---------------------------------------------------------------------
// CFXS Hardware Debugger <https://github.com/CFXS/CFXS-Hardware-Debugger>
// Copyright (C) 2021 | CFXS
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

#include "Core.hpp"
#include "MemoryRegion.hpp"

namespace HWD::Target {

    class DeviceDescription {
    public:
        DeviceDescription() = default;
        DeviceDescription(const char* name, const std::vector<MemoryRegion>& memoryRegions) : m_Name(name), m_MemoryRegions(memoryRegions) {
        }

        const char* GetName() const {
            return m_Name;
        }

        const std::vector<MemoryRegion>& GetMemoryRegions() const {
            return m_MemoryRegions;
        }

        inline void operator=(const DeviceDescription& other) {
            m_Name          = other.m_Name;
            m_MemoryRegions = other.m_MemoryRegions;
        }

    private:
        TargetCore m_Core;
        const char* m_Name = "Unknown";
        std::vector<MemoryRegion> m_MemoryRegions;
    };

    inline const char* To_C_String(Target::MemoryRegion::AccessPermissions type) {
        switch (type) {
            case Target::MemoryRegion::AccessPermissions::READ: return "R";
            case Target::MemoryRegion::AccessPermissions::WRITE: return "W";
            case Target::MemoryRegion::AccessPermissions::READ_WRITE: return "R/W";
            default: return "Unknown";
        }
    }

} // namespace HWD::Target