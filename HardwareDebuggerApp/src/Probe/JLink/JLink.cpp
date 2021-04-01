// [source]
#pragma once

#include "JLink.hpp"
#include "JLink_Driver.hpp"

namespace HWD::Probe {

    /////////////////////////////////////////////////////////
    using Driver::JLink_Driver;
    using namespace Driver::JLink_Types;
    /////////////////////////////////////////////////////////

    //int JLINKARM_EMU_GetList(int HostIFs, JLINKARM_EMU_CONNECT_INFO* paConnectInfo, int MaxInfos);
    //using f_JLINKARM_EMU_GetList = decltype(&JLINKARM_EMU_GetList);

    JLink::JLink() {
        HWDLOG_PROBE_TRACE("JLink::JLink() [{0}]", fmt::ptr(this));

        /////////////////////////////////////////
        JLink_Driver jlinkDriver;
        /////////////////////////////////////////

        if (jlinkDriver.IsInitialized()) {
            std::vector<ProbeInfo> probes;

            probes.resize(jlinkDriver.GetConnectedProbeCount());

            auto foundProbesCount = jlinkDriver.GetProbeList(HostInterfaceType::USB, probes.data(), probes.size());

            if (foundProbesCount < 0) {
                HWDLOG_CORE_ERROR("Failed to load probes [{0}]", foundProbesCount);
            } else {
                HWDLOG_CORE_TRACE("Found {0} JLink probe{1}", foundProbesCount, foundProbesCount == 1 ? "" : "s");
            }

            // test
            for (int i = 0; i < foundProbesCount; i++) {
                HWDLOG_PROBE_INFO("> JLink Probe #{0} {1} - S/N {2}", i + 1, probes[i].productName, probes[i].serialNumber);
            }
        }
    }

    JLink::~JLink() {
        HWDLOG_PROBE_TRACE("JLink::~JLink() [{0}]", fmt::ptr(this));
    }

    const std::string& JLink::GetModel() const {
        return m_ModelName; // RVeips test device
    }

    const std::string& JLink::GetSerialNumber() const {
        return m_SerialNumber; // RVeips test device
    }

} // namespace HWD::Probe
