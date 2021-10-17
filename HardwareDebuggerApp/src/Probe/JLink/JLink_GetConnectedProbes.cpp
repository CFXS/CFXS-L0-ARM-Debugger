#include "JLink.hpp"

namespace HWD::Probe {

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    using namespace Driver::JLink_Types;
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    static constexpr auto MAX_DISCOVERABLE_PROBE_COUNT = 8;
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<JLink*> JLink::s_GetConnectedProbes() {
        ProbeInfo probeInfoBuffer[MAX_DISCOVERABLE_PROBE_COUNT]; // limit max discoverable probes to MAX_DISCOVERABLE_PROBE_COUNT

        auto connectedProbeCount = GetDriver()->probe_GetDeviceList(
            HostInterfaceType::USB | HostInterfaceType::ETHERNET, probeInfoBuffer, MAX_DISCOVERABLE_PROBE_COUNT);

        for (int i = 0; i < connectedProbeCount; i++) {
            HWDLOG_PROBE_INFO("JLink probe on {0} interface: \"{1}\" - S/N: {2}",
                              probeInfoBuffer[i].interface == HostInterfaceType::USB ? "USB" : "ETHERNET",
                              probeInfoBuffer[i].modelName,
                              probeInfoBuffer[i].serialNumber);
        }

        return {};
    }

} // namespace HWD::Probe