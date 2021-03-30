// [source]
#pragma once

#include "JLink.hpp"
#include "JLink_Platform.hpp"

namespace HWD::Probe {

    JLink::JLink() {
    }

    JLink::~JLink() {
    }

    const std::string& JLink::GetModel() const {
        return "SEGGER J-Link Mini"; // RVeips test device
    }

    const std::string& JLink::GetSerialNumber() const {
        return "801022602"; // RVeips test device
    }

} // namespace HWD::Probe