// ---------------------------------------------------------------------
// CFXS L0 ARM Debugger <https://github.com/CFXS/CFXS-L0-ARM-Debugger>
// Copyright (C) 2022 | CFXS
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

namespace L0::Probe {

    using namespace Driver::JLink_Types;

    void JLink::PrepareTarget() {
        LOG_PROBE_TRACE("[JLink@{0}] Prepare target", fmt::ptr(this));
        ErrorCode ec;

        // Configure CoreSight
        ec = GetDriver()->target_CoreSight_Configure("PerformTIFInit=1");
        if (ec != ErrorCode::OK) {
            LOG_PROBE_ERROR("[JLink@{0}] Failed to configure CoreSight - {1}", fmt::ptr(this), ErrorCodeToString(ec));
        } else {
            LOG_PROBE_TRACE("[JLink@{0}] CoreSight configured", fmt::ptr(this));
        }

        UpdateTargetInfo();
    }

    void JLink::UpdateTargetInfo() {
        LOG_PROBE_TRACE("[JLink@{0}] UpdateTargetInfo:", fmt::ptr(this));

        uint32_t cpu_id                = GetDriver()->target_Get_CPU_ID();
        DeviceCore core                = GetDriver()->target_GetDeviceCore();
        int avail_sw_ram_breakpoints   = GetDriver()->probe_GetAvailableBreakpointCount(Breakpoint::Type::SW_RAM);
        int avail_sw_flash_breakpoints = GetDriver()->probe_GetAvailableBreakpointCount(Breakpoint::Type::SW_FLASH);
        int avail_hw_breakpoints       = GetDriver()->probe_GetAvailableBreakpointCount(Breakpoint::Type::HW);

        LOG_PROBE_TRACE("{0} Target info", GetModelName());
        LOG_PROBE_TRACE("[CPU]");
        LOG_PROBE_TRACE("\tCore:      {0}", core);
        LOG_PROBE_TRACE("\tCPUID:     {0:#X}", cpu_id);
        LOG_PROBE_TRACE("[Available Breakpoints]");
        LOG_PROBE_TRACE("\tSW RAM:   {0}", avail_sw_ram_breakpoints);
        LOG_PROBE_TRACE("\tSW FLASH: {0}", avail_sw_flash_breakpoints);
        LOG_PROBE_TRACE("\tHW:       {0}", avail_hw_breakpoints);

        uint32_t maxSpeeds[64];
        auto maxSpeedCount = GetDriver()->target_SWO_GetCompatibleSpeeds(120000000, 0, maxSpeeds, 64);

        if (maxSpeedCount < 0) {
            LOG_PROBE_ERROR("Failed to get SWO speeds");
        } else {
            //for (auto i = 0; i < maxSpeedCount; i++) {
            //    LOG_PROBE_INFO("supported speed: {0}", maxSpeeds[i]);
            //}
            LOG_PROBE_TRACE("Max SWO speed: {0}", maxSpeeds[0]);
        }

        int cps = 120000000; //GetDriver()->target_Measure_CPU_Speed(0x20000000, true);
        LOG_CORE_CRITICAL("CPU SPEED {0}", cps);
        if (GetDriver()->target_SWO_Enable(cps, 4000000, SWO_Interface::MANCHESTER, 0x00000000) != ErrorCode::OK) {
            LOG_PROBE_ERROR("Failed to enable SWO");
        } else {
            LOG_PROBE_TRACE("SWO enabled");
        }
    }

} // namespace L0::Probe