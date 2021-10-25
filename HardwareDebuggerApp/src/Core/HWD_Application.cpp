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
#include <Debugger/Probe/JLink/JLink.hpp>
#include "Project/ProjectManager.hpp"
#include <QFontDatabase>

namespace HWD {

    HWD_Application::HWD_Application(int argc, char** argv) : Application(argc, argv, CFXS_HWD_PROGRAM_NAME) {
        qApp->setWindowIcon(QPixmap(":/HWD_Icon.png"));
    }

    void HWD_Application::OnCreate() {
        QFontDatabase::addApplicationFont(":/Font/Code");
        QFontDatabase::addApplicationFont(":/Font/OpenSans-Bold");
        QFontDatabase::addApplicationFont(":/Font/OpenSans-BoldItalic");
        QFontDatabase::addApplicationFont(":/Font/OpenSans-ExtraBold");
        QFontDatabase::addApplicationFont(":/Font/OpenSans-ExtraBoldItalic");
        QFontDatabase::addApplicationFont(":/Font/OpenSans-Italic");
        QFontDatabase::addApplicationFont(":/Font/OpenSans-Light");
        QFontDatabase::addApplicationFont(":/Font/OpenSans-LightItalic");
        QFontDatabase::addApplicationFont(":/Font/OpenSans-Regular");
        QFontDatabase::addApplicationFont(":/Font/OpenSans-SemiBold");
        QFontDatabase::addApplicationFont(":/Font/OpenSans-SemiBoldItalic");

        ProjectManager::HWD_Load();
        Load_Probe();
        Load_Window();
    }

    void HWD_Application::OnDestroy() {
        Unload_Probe();
        ProjectManager::HWD_Unload();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    void HWD_Application::Load_Window() {
        LoadWindowState();

        QObject::connect(GetMainWindow(), &UI::MainWindow::StateDataReady, SaveWindowState);
        QObject::connect(ProjectManager::GetNotifier(), &ProjectManager::Notifier::ProjectOpened, [=]() {
            LoadWindowState();
        });
    }

    void HWD_Application::Load_Probe() {
        HWDLOG_CORE_INFO("Load probes");
        Probe::JLink::HWD_Load();
    }

    void HWD_Application::Unload_Probe() {
        HWDLOG_CORE_INFO("Unload probes");
        Probe::JLink::HWD_Unload();
    }

    void HWD_Application::LoadWindowState() {
        if (ProjectManager::IsProjectOpen()) {
            HWDLOG_CORE_INFO("Load window state");
            QByteArray windowStateData;
            QFile windowStateFile(ProjectManager::GetProjectFilePath(ProjectManager::Path::WINDOW_STATE));
            if (windowStateFile.exists()) {
                QSettings stateData(ProjectManager::GetProjectFilePath(ProjectManager::Path::WINDOW_STATE), QSettings::IniFormat);
                if (stateData.status() == QSettings::NoError) {
                    GetMainWindow()->HideAllPanels();
                    GetMainWindow()->LoadState(stateData);
                } else {
                    HWDLOG_CORE_ERROR("Failed to open window state file");
                }
            } else {
                GetMainWindow()->HideAllPanels();
                HWDLOG_CORE_WARN("Window state file not found");
            }
        } else {
            HWDLOG_CORE_INFO("Not loading window state - project not open");
        }
    }

    void HWD_Application::SaveWindowState(QSettings* stateData) {
        if (ProjectManager::IsProjectOpen()) {
            HWDLOG_CORE_INFO("Saving window state");
            QSettings localCopy(ProjectManager::GetProjectFilePath(ProjectManager::Path::WINDOW_STATE), QSettings::IniFormat);

            for (auto key : stateData->allKeys()) {
                localCopy.setValue(key, stateData->value(key));
            }

            localCopy.sync();

            if (localCopy.status() == QSettings::NoError) {
                HWDLOG_CORE_INFO("Window state saved");
            } else {
                HWDLOG_CORE_ERROR("Failed to save window state");
            }
        } else {
            HWDLOG_CORE_INFO("Not saving window state - no project open");
        }
    }

} // namespace HWD