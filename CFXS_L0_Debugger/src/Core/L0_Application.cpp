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
#include "L0_Application.hpp"
#include <QString>
#include <QStandardPaths>
#include <Debugger/Probe/JLink/JLink.hpp>
#include "Project/ProjectManager.hpp"
#include <QFontDatabase>
#include <QFile>

namespace L0 {

    L0_Application::L0_Application(int argc, char** argv) : Application(argc, argv, CFXS_PROGRAM_NAME) {
        qApp->setWindowIcon(QPixmap(":/L0_Icon"));
    }

    void L0_Application::OnCreate() {
        QFontDatabase::addApplicationFont(":/Font/Code");

        ProjectManager::L0_Load();
        Load_Probe();
        Load_Window();
    }

    void L0_Application::OnDestroy() {
        Unload_Probe();
        ProjectManager::L0_Unload();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    void L0_Application::Load_Window() {
        LoadWindowState();

        QObject::connect(GetMainWindow(), &UI::MainWindow::StateDataReady, SaveWindowState);
        QObject::connect(ProjectManager::GetNotifier(), &ProjectManager::Notifier::ProjectOpened, [=]() {
            LoadWindowState();
        });
    }

    void L0_Application::Load_Probe() {
        LOG_CORE_INFO("Load probes");
        Probe::JLink::L0_Load();
    }

    void L0_Application::Unload_Probe() {
        LOG_CORE_INFO("Unload probes");
        Probe::JLink::L0_Unload();
    }

    void L0_Application::LoadWindowState() {
        if (ProjectManager::IsProjectOpen()) {
            LOG_CORE_INFO("Load window state");
            QByteArray windowStateData;
            QFile windowStateFile(ProjectManager::GetProjectFilePath(ProjectManager::Path::WINDOW_STATE));
            if (windowStateFile.exists()) {
                QSettings stateData(ProjectManager::GetProjectFilePath(ProjectManager::Path::WINDOW_STATE), QSettings::IniFormat);
                if (stateData.status() == QSettings::NoError) {
                    GetMainWindow()->HideAllPanels();
                    GetMainWindow()->LoadState(stateData);
                } else {
                    LOG_CORE_ERROR("Failed to open window state file");
                }
            } else {
                GetMainWindow()->HideAllPanels();
                LOG_CORE_WARN("Window state file not found");
            }
        } else {
            LOG_CORE_INFO("Not loading window state - project not open");
        }
    }

    void L0_Application::SaveWindowState(QSettings* stateData) {
        if (ProjectManager::IsProjectOpen()) {
            LOG_CORE_INFO("Saving window state");
            QSettings localCopy(ProjectManager::GetProjectFilePath(ProjectManager::Path::WINDOW_STATE), QSettings::IniFormat);

            for (auto key : stateData->allKeys()) {
                localCopy.setValue(key, stateData->value(key));
            }

            localCopy.sync();

            if (localCopy.status() == QSettings::NoError) {
                LOG_CORE_INFO("Window state saved");
            } else {
                LOG_CORE_ERROR("Failed to save window state");
            }
        } else {
            LOG_CORE_INFO("Not saving window state - no project open");
        }
    }

} // namespace L0