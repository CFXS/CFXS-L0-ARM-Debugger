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
#include "Assert.hpp"
#include "STLink.hpp"

namespace L0::Probe {

    void STLink::PrepareTarget() {
        LOG_PROBE_TRACE("[STLink@{0}] Prepare target", fmt::ptr(this));

        L0_NOT_IMPLEMENTED;

        UpdateTargetInfo();
    }

    void STLink::UpdateTargetInfo() {
        LOG_PROBE_TRACE("[STLink@{0}] UpdateTargetInfo:", fmt::ptr(this));

        //LOG_PROBE_TRACE("{0} Target info", GetModelName());
        //LOG_PROBE_TRACE("[CPU]");
        //LOG_PROBE_TRACE("\tCore:      {0}", core);
        //LOG_PROBE_TRACE("\tCPUID:     {0:#X}", cpu_id);
        //LOG_PROBE_TRACE("[Available Breakpoints]");
        //LOG_PROBE_TRACE("\tSW RAM:   {0}", avail_sw_ram_breakpoints);
        //LOG_PROBE_TRACE("\tSW FLASH: {0}", avail_sw_flash_breakpoints);
        //LOG_PROBE_TRACE("\tHW:       {0}", avail_hw_breakpoints);
    }

} // namespace L0::Probe