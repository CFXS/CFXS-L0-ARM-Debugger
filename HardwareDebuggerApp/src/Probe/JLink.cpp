// [source]
#pragma once

#include "JLink.hpp"
#include "JLink_Platform.hpp"

namespace HWD::Probe {

    static bool s_JLink_Link_Initialized = false;

    static void InitializeLibrary() {
        if (s_JLink_Link_Initialized)
            return;

        s_JLink_Link_Initialized = true;
        auto lib                 = _JLink_LoadLibrary();

        if (lib)
            HWDLOG_PROBE_INFO("Loaded JLink_x64 library");
        else
            HWDLOG_PROBE_CRITICAL("Failed to load JLink_x64 library");
    }

    JLink::JLink() {
        InitializeLibrary();
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