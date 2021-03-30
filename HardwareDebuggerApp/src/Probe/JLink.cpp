// [source]
#pragma once

#include "JLink.hpp"
#include "JLink_Platform.hpp"

namespace HWD::Probe {

    JLink::JLink() {
        HWDLOG_PROBE_TRACE("JLink::JLink() [{0}]", fmt::ptr(this));
    }

    JLink::~JLink() {
        HWDLOG_PROBE_TRACE("JLink::~JLink() [{0}]", fmt::ptr(this));
    }

    const std::string& JLink::GetModel() const {
        return m_Model; // RVeips test device
    }

    const std::string& JLink::GetSerialNumber() const {
        return m_SerialNumber; // RVeips test device
    }

} // namespace HWD::Probe