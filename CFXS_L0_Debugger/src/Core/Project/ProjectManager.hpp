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
#pragma once

#include <QObject>
#include <QString>

namespace L0 {

    class ProjectManager;
    class _ProjectManagerNotifier : public QObject {
        friend class ProjectManager;
        Q_OBJECT

    private:
        _ProjectManagerNotifier() = default;

    signals:
        void ProjectOpened();         // Emitted when a new project has been opened
        void RecentProjectsChanged(); // Emitted when recent project list order/content has changed
    };

    ///////////////////////////////////////////////////////////////////////////

    class L0_Application;
    class ProjectManager {
        friend class L0_Application;

    public:
        using Notifier = _ProjectManagerNotifier;
        enum class Path { WINDOW_STATE };

    public:
        // Get instance for connections
        static _ProjectManagerNotifier* GetNotifier();

        /// Get list of recently opened projects
        static const QStringList& GetRecentProjectPaths();

        /// Clears recently opened project list leaving only current project (if open)
        static void ClearRecentProjects();

        /// Set current project root directory
        static void OpenProject(const QString& workspacePath);

        /// True if a valid project directory is open
        static bool IsProjectOpen();

        /// True when current project directory contains a project file
        static bool IsProjectInitialized();

        /// Path to current project root folder
        static const QString& GetWorkspacePath();

        /// Get path to specific project files
        static QString GetProjectFilePath(Path path);

        /// Get full path to file that might or might not be a relative path
        static QString GetFullFilePath(const QString& path);

    private:
        /// Call when loading app
        static void L0_Load();

        /// Call when unloading app
        static void L0_Unload();

        /// Save recent project list to user storage
        static void SaveRecentProjectList();

        /// Load recent project list from user storage
        static void LoadRecentProjectList();

        /// Add recent project path
        static void AddRecentProjectPath(const QString& path);
    };

} // namespace L0