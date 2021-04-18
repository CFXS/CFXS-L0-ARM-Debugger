// [source]
#include "SupportedDevices.hpp"
#include <Utils.hpp>

namespace HWD {

    /////////////////////////////////////////////////////////////////////////////////
    using Target::MemoryRegion;
    using Access = MemoryRegion::AccessPermissions;
    /////////////////////////////////////////////////////////////////////////////////
    std::map<std::string, Target::DeviceDescription> SupportedDevices::s_SupportedTargets;
    /////////////////////////////////////////////////////////////////////////////////

    bool SupportedDevices::LoadSupportedDevices() {
        HWDLOG_CORE_INFO("Loading supported target devices");

        s_SupportedTargets["TM4C1294NC"] = Target::DeviceDescription{"TM4C1294NCPDT",
                                                                     {
                                                                         MemoryRegion{"Flash", 0, 0x100000, Access::READ},
                                                                         MemoryRegion{"ROM", 0x1000000, 0x8C00, Access::READ},
                                                                         MemoryRegion{"SRAM", 0x20000000, 0x40000, Access::READ_WRITE},
                                                                     }};

        HWDLOG_CORE_TRACE("Supported devices:");
        for (const auto& [name, desc] : s_SupportedTargets) {
            HWDLOG_CORE_TRACE(" - {0}", name);
        }

        return true;
    }

    const std::map<std::string, Target::DeviceDescription>& SupportedDevices::GetSupportedDevices() {
        return s_SupportedTargets;
    }

} // namespace HWD