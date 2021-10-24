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
#include "ProjectManager.hpp"
#include <QDir>
#include <QSettings>
#include <QStandardPaths>

namespace HWD {

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    static const QString CFXS_HWD_CONFIG_DIRECTORY = QStringLiteral("/.cfxs_hwd");
    static const QString KEY_RECENT_PATHS          = QStringLiteral("recentPaths");
    static constexpr int MAX_RECENT_PROJECTS       = 16;
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    static QStringList s_RecentProjectPaths;  // List of recently opened projects
    static bool s_ProjectOpen        = false; // true when project folder has been opened
    static bool s_ProjectInitialized = false; // true when CFXS_HWD_CONFIG_DIRECTORY contains a project file
    static QString s_WorkspacePath;
    ////////////////////////////////////////////////////////////////////////////////////////////////////////

    _ProjectManagerNotifier* ProjectManager::GetNotifier() {
        static _ProjectManagerNotifier n;
        return &n;
    }

    //////////////////////////////////////////////////////////////////////////////

    void ProjectManager::HWD_Load() {
        HWDLOG_PROJECT_INFO("Load ProjectManager");

        LoadRecentProjectList();

        if (!s_RecentProjectPaths.isEmpty()) {
            HWDLOG_PROJECT_TRACE("Open project from last session - {}", s_RecentProjectPaths.first());
            OpenProject(s_RecentProjectPaths.first());
        }

        QObject::connect(GetNotifier(), &Notifier::RecentProjectsChanged, []() {
            SaveRecentProjectList();
        });
    }

    void ProjectManager::SaveRecentProjectList() {
        QString location =
            QStandardPaths::standardLocations(QStandardPaths::StandardLocation::AppDataLocation).first() + "/RecentProjects.hwd";
        QSettings projectList(location, QSettings::IniFormat);
        projectList.setValue(KEY_RECENT_PATHS, s_RecentProjectPaths);
        projectList.sync();

        if (projectList.status() == QSettings::NoError) {
            HWDLOG_PROJECT_TRACE("Saved recent project list to {}", location);
        } else {
            HWDLOG_PROJECT_ERROR("Failed to save recent project list to {}", location);
        }
    }

    void ProjectManager::LoadRecentProjectList() {
        QString location =
            QStandardPaths::standardLocations(QStandardPaths::StandardLocation::AppDataLocation).first() + "/RecentProjects.hwd";
        QSettings projectList(
            QStandardPaths::standardLocations(QStandardPaths::StandardLocation::AppDataLocation).first() + "/RecentProjects.hwd",
            QSettings::IniFormat);

        if (projectList.status() == QSettings::NoError) {
            HWDLOG_PROJECT_TRACE("Loaded recent project list from {}", location);
            s_RecentProjectPaths = projectList.value(KEY_RECENT_PATHS).toStringList();
        } else {
            HWDLOG_PROJECT_ERROR("Failed to load recent project list from {}", location);
        }

        while (s_RecentProjectPaths.size() > MAX_RECENT_PROJECTS) {
            s_RecentProjectPaths.pop_back();
        }

        emit GetNotifier()->RecentProjectsChanged();
    }

    void ProjectManager::HWD_Unload() {
        HWDLOG_PROJECT_INFO("Unload ProjectManager");
    }

    //////////////////////////////////////////////////////////////////////////////

    const QStringList& ProjectManager::GetRecentProjectPaths() {
        return s_RecentProjectPaths;
    }

    void ProjectManager::ClearRecentProjects() {
        s_RecentProjectPaths.clear();
        if (IsProjectOpen()) {
            s_RecentProjectPaths.append(GetWorkspacePath());
        }

        emit GetNotifier()->RecentProjectsChanged();
    }

    void ProjectManager::AddRecentProjectPath(const QString& path) {
        if (s_RecentProjectPaths.contains(path)) {
            s_RecentProjectPaths.removeAll(path);
        }

        s_RecentProjectPaths.push_front(path);

        if (s_RecentProjectPaths.size() > MAX_RECENT_PROJECTS) {
            s_RecentProjectPaths.pop_back();
        }
    }

    void ProjectManager::OpenProject(const QString& path) {
        QString workspacePath = path;
        if (workspacePath.contains(CFXS_HWD_CONFIG_DIRECTORY)) {
            HWDLOG_PROJECT_INFO("Trying to open {} - moving back one directory", CFXS_HWD_CONFIG_DIRECTORY);
            workspacePath = workspacePath.mid(0, workspacePath.indexOf(CFXS_HWD_CONFIG_DIRECTORY));
        }

        if (s_WorkspacePath != workspacePath) {
            if (QDir().exists(workspacePath)) {
                HWDLOG_PROJECT_INFO("Open project {}", workspacePath);
                s_WorkspacePath = workspacePath;
                s_ProjectOpen   = true;

                AddRecentProjectPath(s_WorkspacePath);

                emit GetNotifier()->ProjectOpened();
                emit GetNotifier()->RecentProjectsChanged();
            } else {
                HWDLOG_PROJECT_ERROR("OpenProject(): Invalid path - {}", workspacePath);
            }
        } else {
            HWDLOG_PROJECT_INFO("Project already open - {}", workspacePath);
        }
    }

    bool ProjectManager::IsProjectOpen() {
        return s_ProjectOpen;
    }

    const QString& ProjectManager::GetWorkspacePath() {
        if (!IsProjectOpen()) {
            HWDLOG_PROJECT_CRITICAL("GetWorkspacePath(): Project not open");
        }
        return s_WorkspacePath;
    }

    QString ProjectManager::GetProjectFilePath(Path path) {
        switch (path) {
            case Path::WINDOW_STATE: return GetWorkspacePath() + CFXS_HWD_CONFIG_DIRECTORY + QStringLiteral("/WindowState.hwd");
        }

        HWDLOG_PROJECT_CRITICAL("Unknown path {}", (int)path);
        return s_WorkspacePath + "_error_temp.txt";
    }

} // namespace HWD