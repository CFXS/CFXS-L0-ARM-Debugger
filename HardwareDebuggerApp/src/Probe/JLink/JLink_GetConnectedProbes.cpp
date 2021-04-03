// [source]
#include "JLink_Probe.hpp"

namespace HWD::Probe {

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    using namespace Driver::JLink_Types;
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    /// Get vector of connected JLink probes
    // Not the best performing function. Will not be called a lot of times, so should be ok.
    // Maybe still optimize later
    std::vector<JLink_Probe*> JLink_Probe::s_GetConnectedProbes() {
        std::shared_ptr<Driver::JLink_Driver> driver;

        // create new driver or get driver from existing probe
        int probeArrayIndex = -1;
        for (int i = 0; i < MAX_DISCOVERABLE_PROBE_COUNT; i++) {
            if (s_Probes[i]) {
                probeArrayIndex = i;
                break;
            }
        }

        if (probeArrayIndex != -1) {
            driver = s_Probes[probeArrayIndex]->GetDriver();
        } else {
            driver = std::make_shared<Driver::JLink_Driver>();
        }

        if (driver->IsLoaded()) {
            ProbeInfo probeInfoBuffer[MAX_DISCOVERABLE_PROBE_COUNT]; // limit max discoverable probes to MAX_DISCOVERABLE_PROBE_COUNT

            auto connectedProbeCount = driver->probe_GetDeviceList(
                HostInterfaceType::USB | HostInterfaceType::ETHERNET, probeInfoBuffer, MAX_DISCOVERABLE_PROBE_COUNT);

            for (int i = 0; i < connectedProbeCount; i++) {
                // find free slot
                int freeIndex = -1;
                for (int j = 0; j < MAX_DISCOVERABLE_PROBE_COUNT; j++) {
                    if (s_Probes[i] == nullptr) {
                        freeIndex = i;
                        break;
                    }
                }

                // add if not already in array and array has free slot
                if (freeIndex >= 0 && std::find_if(s_Probes.begin(), s_Probes.end(), [&](JLink_Probe* a) {
                                          if (a == nullptr)
                                              return false;
                                          return probeInfoBuffer[i].serialNumber == a->GetRawSerialNumber();
                                      }) == s_Probes.end()) {
                    HWDLOG_PROBE_INFO("found new probe on {0} interface: \"{1}\" - S/N: {2}",
                                      probeInfoBuffer[i].interface == HostInterfaceType::USB ? "USB" : "ETHERNET",
                                      probeInfoBuffer[i].modelName,
                                      probeInfoBuffer[i].serialNumber);
                    s_Probes[freeIndex] = new JLink_Probe(probeInfoBuffer[i], freeIndex);
                }
            }
        }

        std::vector<JLink_Probe*> probes;
        probes.reserve(MAX_DISCOVERABLE_PROBE_COUNT);

        for (int i = 0; i < MAX_DISCOVERABLE_PROBE_COUNT; i++) {
            if (s_Probes[i])
                probes.push_back(s_Probes[i]);
        }
        return probes;
    }

} // namespace HWD::Probe