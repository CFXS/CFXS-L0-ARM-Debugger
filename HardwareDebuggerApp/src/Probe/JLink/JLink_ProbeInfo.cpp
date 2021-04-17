// [source]
#include "JLink.hpp"

namespace HWD::Probe {

    using namespace Driver::JLink_Types;

    void JLink::UpdateProbeInfo() {
        // check capabilities
        m_ProbeCapabilities = m_Driver->probe_GetCapabilities();

        HWDLOG_PROBE_TRACE("[JLink@{0}] {1} Capabilities:", fmt::ptr(this), GetModelName());
        if (m_ProbeCapabilities & ProbeCapabilities::ADAPTIVE_CLOCKING)
            HWDLOG_PROBE_TRACE("[JLink@{0}]  - Adaptive clocking", fmt::ptr(this));
        if (m_ProbeCapabilities & ProbeCapabilities::RESET_STOP_TIMED)
            HWDLOG_PROBE_TRACE("[JLink@{0}]  - Halt after reset", fmt::ptr(this));
        if (m_ProbeCapabilities & ProbeCapabilities::SWO)
            HWDLOG_PROBE_TRACE("[JLink@{0}]  - SWO", fmt::ptr(this));
    }

} // namespace HWD::Probe