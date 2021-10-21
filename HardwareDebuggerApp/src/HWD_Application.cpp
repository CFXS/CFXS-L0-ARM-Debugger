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
#include "HWD_Application.hpp"
#include <QString>
#include <QStringLiteral>
#include <QStandardPaths>
#include <Probe/JLink/JLink.hpp>

namespace HWD {

    HWD_Application::HWD_Application(int argc, char** argv) : Application(argc, argv, CFXS_HWD_PROGRAM_NAME) {
        qApp->setOrganizationName("CFXS");
        qApp->setApplicationDisplayName(CFXS_HWD_PROGRAM_NAME);
        qApp->setWindowIcon(QPixmap(":/HWD_Icon.png"));
    }
    void HWD_Application::OnCreate() {
        Load_Probe();
    }

    void HWD_Application::OnDestroy() {
        Unload_Probe();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    void HWD_Application::Load_Probe() {
        HWDLOG_CORE_TRACE("Load probes");
        Probe::JLink::HWD_Load();
    }

    void HWD_Application::Unload_Probe() {
        HWDLOG_CORE_TRACE("Unload probes");
        Probe::JLink::HWD_Unload();
    }

} // namespace HWD