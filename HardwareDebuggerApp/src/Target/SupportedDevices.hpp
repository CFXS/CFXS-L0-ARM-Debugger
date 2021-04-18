#pragma once

#include "DeviceDescription.hpp"

namespace HWD {

    class SupportedDevices {
    public:
        static bool LoadSupportedDevices();
        static const std::map<std::string, Target::DeviceDescription>& GetSupportedDevices();

    private:
        static std::map<std::string, Target::DeviceDescription> s_SupportedTargets;
    };

} // namespace HWD