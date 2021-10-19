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

#include <KDDockWidgets/DockWidget.h>
#include <KDDockWidgets/LayoutSaver.h>

#include <Probe/JLink/JLink.hpp>

#include <UI/Windows/ProjectFileBrowser/ProjectFileBrowser.hpp>

namespace HWD {

    HWD_Application::HWD_Application(int argc, char** argv) : Application(argc, argv, CFXS_HWD_PROGRAM_NAME) {
        qApp->setOrganizationName("CFXS");
        qApp->setApplicationDisplayName(CFXS_HWD_PROGRAM_NAME);
    }

    namespace Test {
        extern RVeips_ProbeTest* m_RV;
    }

    void HWD_Application::OnCreate() {
        HWDLOG_CORE_INFO("Loading probes");
        Probe::JLink::HWD_Load();

#ifdef DEV_PC_CFXS_n
        HWDLOG_CORE_INFO("DEV_PC_CFXS init");
        (new std::thread([=]() {
            m_RihardsTest = std::make_unique<Test::RVeips_ProbeTest>();
        }))->detach();
#endif

        //auto fpWindow = new UI::FunctionProfilerWindow;
        //fpWindow->show();
        //GetMainWindow()->addDockWidgetAsTab(fpWindow);

        auto pfbWindow = new UI::ProjectFileBrowser;
        pfbWindow->show();
        GetMainWindow()->addDockWidgetAsTab(pfbWindow);
    }

    void HWD_Application::OnDestroy() {
        HWDLOG_CORE_INFO("Destroying App");
        Probe::JLink::HWD_Unload();
    }

} // namespace HWD