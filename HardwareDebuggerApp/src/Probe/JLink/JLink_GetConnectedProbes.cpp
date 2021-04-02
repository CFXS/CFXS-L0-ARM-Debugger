// [source]
#include "JLink_Probe.hpp"

namespace HWD::Probe {

    /////////////////////////////////////////////////////
    using namespace Driver::JLink_Types;
    /////////////////////////////////////////////////////
    static constexpr auto MAX_DISCOVERABLE_PROBE_COUNT = 32; // limit max discoverable probe count

    static std::vector<JLink_Probe*> s_Probes;
    /////////////////////////////////////////////////////

    const std::vector<JLink_Probe*>& JLink_Probe::GetConnectedProbes() {
        std::shared_ptr<Driver::JLink_Driver> driver;

        // create new driver or get driver from existing probe
        if (!s_Probes.empty()) {
            driver = s_Probes[0]->GetDriver();
        } else {
            driver = std::make_shared<Driver::JLink_Driver>();
        }

        if (driver->IsLoaded()) {
            ProbeInfo probeInfoBuffer[MAX_DISCOVERABLE_PROBE_COUNT]; // limit max discoverable probes to MAX_DISCOVERABLE_PROBE_COUNT

            auto connectedProbeCount =
                driver->GetProbeList(HostInterfaceType::USB | HostInterfaceType::ETHERNET, probeInfoBuffer, MAX_DISCOVERABLE_PROBE_COUNT);

            for (int i = 0; i < connectedProbeCount; i++) {
                // add if not already in vector
                if (std::find_if(s_Probes.begin(), s_Probes.end(), [&](JLink_Probe* a) {
                        return probeInfoBuffer[i].serialNumber == a->GetRawSerialNumber();
                    }) == s_Probes.end()) {
                    HWDLOG_PROBE_INFO("found new probe on {0} interface: \"{1}\" - S/N: {1}",
                                      probeInfoBuffer[i].interface == HostInterfaceType::USB ? "USB" : "ETHERNET",
                                      probeInfoBuffer[i].modelName,
                                      probeInfoBuffer[i].serialNumber);
                    s_Probes.push_back(new JLink_Probe(probeInfoBuffer[i]));
                }
            }
        }

        return s_Probes;
    }

} // namespace HWD::Probe