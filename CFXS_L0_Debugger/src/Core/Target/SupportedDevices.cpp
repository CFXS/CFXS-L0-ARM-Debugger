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
#include "SupportedDevices.hpp"
#include <Utils.hpp>

namespace L0::Target {

    /////////////////////////////////////////////////////////////////////////////////
    using Target::MemoryRegion;
    using Access = MemoryRegion::AccessPermissions;
    /////////////////////////////////////////////////////////////////////////////////
    std::map<std::string, Target::DeviceDescription> SupportedDevices::s_SupportedTargets;
    /////////////////////////////////////////////////////////////////////////////////

    bool SupportedDevices::LoadSupportedDevices() {
        LOG_CORE_INFO("Loading supported target devices");

        s_SupportedTargets["TM4C1294NC"] = Target::DeviceDescription{"TM4C1294NCPDT",
                                                                     {
                                                                         MemoryRegion{"Flash", 0, 0x100000, Access::READ},
                                                                         MemoryRegion{"ROM", 0x1000000, 0x8C00, Access::READ},
                                                                         MemoryRegion{"SRAM", 0x20000000, 0x40000, Access::READ_WRITE},
                                                                     }};

        s_SupportedTargets["STM32H7A3ZI"] =
            Target::DeviceDescription{"STM32H7A3ZI",
                                      {
                                          MemoryRegion{"Flash", 0x08000000, 2_MB, Access::READ},
                                          MemoryRegion{"SRAM", 0x24000000, 1_MB, Access::READ_WRITE},
                                          // TODO: check if these can be cross accessed
                                          //MemoryRegion{"DTCM_SRAM1", 0x20000000, 64_kB, Access::READ_WRITE},
                                          //MemoryRegion{"DTCM_SRAM2", 0x20010000, 64_kB, Access::READ_WRITE},
                                          MemoryRegion{"DTCM_SRAM", 0x20000000, 128_kB, Access::READ_WRITE},
                                          MemoryRegion{"ITCM_SRAM", 0x00000000, 64_kB, Access::READ_WRITE},
                                          MemoryRegion{"SRAM_CD", 0x30000000, 128_kB, Access::READ_WRITE},
                                          MemoryRegion{"SRAM_SRD", 0x38000000, 32_kB, Access::READ_WRITE},
                                      }};

        s_SupportedTargets["STM32H753ZI"] = Target::DeviceDescription{"STM32H753ZI",
                                                                      {
                                                                          MemoryRegion{"Flash", 0x08000000, 2_MB, Access::READ},
                                                                          MemoryRegion{"SRAM_D1", 0x24000000, 512_kB, Access::READ_WRITE},
                                                                          MemoryRegion{"SRAM_D2", 0x30000000, 288_kB, Access::READ_WRITE},
                                                                          MemoryRegion{"SRAM_D3", 0x38000000, 64_kB, Access::READ_WRITE},
                                                                          MemoryRegion{"ITCM_SRAM", 0x00000000, 64_kB, Access::READ_WRITE},
                                                                          MemoryRegion{"DTCM_SRAM", 0x20000000, 128_kB, Access::READ_WRITE},
                                                                      }};

        s_SupportedTargets["ATSAMA5D36"] =
            Target::DeviceDescription{"ATSAMA5D36",
                                      {
                                          MemoryRegion{"SRAM", 0x00300000, 0x20000, Access::READ_WRITE},
                                          MemoryRegion{"RAM (Cache)", 0x20000000, 0x4000000, Access::READ_WRITE},
                                          MemoryRegion{"RAM (No Cache)", 0x24000000, 0x1000000, Access::READ_WRITE},
                                      }};

        LOG_CORE_TRACE("Supported devices:");
        for (const auto& [name, desc] : s_SupportedTargets) {
            LOG_CORE_TRACE(" - {0}", name);
        }

        return true;
    }

    const std::map<std::string, Target::DeviceDescription>& SupportedDevices::GetSupportedDevices() {
        return s_SupportedTargets;
    }

} // namespace L0::Target