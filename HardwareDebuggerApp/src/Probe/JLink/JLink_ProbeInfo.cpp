#include "JLink.hpp"

namespace HWD::Probe {

    using namespace Driver::JLink_Types;

    void JLink::UpdateProbeInfo() {
        HWDLOG_PROBE_TRACE("[JLink@{0}] UpdateProbeInfo:", fmt::ptr(this));

        m_ProbeCapabilities = m_Driver->probe_GetCapabilities();

        HWDLOG_PROBE_TRACE("{0} Capabilities:", GetModelName());
        if (m_ProbeCapabilities & ProbeCapabilities::ADAPTIVE_CLOCKING)
            HWDLOG_PROBE_TRACE(" - Adaptive clocking");
        if (m_ProbeCapabilities & ProbeCapabilities::RESET_STOP_TIMED)
            HWDLOG_PROBE_TRACE(" - Halt after reset");
        if (m_ProbeCapabilities & ProbeCapabilities::SWO)
            HWDLOG_PROBE_TRACE(" - SWO");
    }

} // namespace HWD::Probe