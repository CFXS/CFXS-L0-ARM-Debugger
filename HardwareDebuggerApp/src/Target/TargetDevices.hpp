#pragma once

#include "TargetDeviceDescription.hpp"

namespace HWD {

    class TargetDevices {
    public:
        static bool LoadSupportedDevices();
        static const std::map<std::string, TargetDeviceDescription>& GetSupportedDevices();

    private:
        static std::map<std::string, TargetDeviceDescription> s_SupportedTargets;
    };

} // namespace HWD