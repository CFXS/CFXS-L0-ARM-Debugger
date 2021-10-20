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
#include "JLink_Driver.hpp"

namespace HWD::Probe::Driver {

    JLink_Driver::JLink_Driver() {
        m_Library = new QLibrary("C:/CFXS/JLink_x64.dll");
        m_Library->load();

        if (m_Library && m_Library->isLoaded()) {
            LoadFunctionPointers();
        } else {
            HWDLOG_PROBE_ERROR("JLink_Driver[{0}] Driver initialization failed", fmt::ptr(this));
        }
    }

    JLink_Driver::~JLink_Driver() {
        if (m_Library)
            delete m_Library;
    }

} // namespace HWD::Probe::Driver