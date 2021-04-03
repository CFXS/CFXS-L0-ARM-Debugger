// [source]
#include "TargetDevices.hpp"
#include <Utils.hpp>

namespace HWD {

    /////////////////////////////////////////////////////////////////////////////////
    using MemoryRegion = TargetDeviceDescription::MemoryRegion;
    /////////////////////////////////////////////////////////////////////////////////
    std::map<std::string, TargetDeviceDescription> TargetDevices::s_SupportedTargets;
    /////////////////////////////////////////////////////////////////////////////////

    bool TargetDevices::LoadSupportedDevices() {
        HWDLOG_CORE_INFO("Loading supported target devices");

        s_SupportedTargets["TM4C1294NC"] = TargetDeviceDescription{
            "TM4C1294NCPDT",
            {
                TargetDeviceDescription::MemoryRegion{"Flash", 0, 0x100000, MemoryRegion::AccessPermissions::READ},
                TargetDeviceDescription::MemoryRegion{"ROM", 0x1000000, 0x8C00, MemoryRegion::AccessPermissions::READ},
                TargetDeviceDescription::MemoryRegion{"SRAM", 0x20000000, 0x40000, MemoryRegion::AccessPermissions::READ_WRITE},
            }};

        HWDLOG_CORE_TRACE("Supported devices:");
        for (const auto& [name, desc] : s_SupportedTargets) {
            HWDLOG_CORE_TRACE(" - {0}", name);
        }

        return true;
    }

    const std::map<std::string, TargetDeviceDescription>& TargetDevices::GetSupportedDevices() {
        return s_SupportedTargets;
    }

} // namespace HWD