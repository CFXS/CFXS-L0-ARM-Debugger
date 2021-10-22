// ---------------------------------------------------------------------
// CFXS Hardware Debugger <https://github.com/CFXS/CFXS-Hardware-Debugger>
// Copyright (C) 2021 | CFXS
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>
// ---------------------------------------------------------------------
// [CFXS] //
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