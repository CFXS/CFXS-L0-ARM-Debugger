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

namespace HWD {

    class ProjectManager;
    class _ProjectManagerNotifier : public QObject {
        friend class ProjectManager;
        Q_OBJECT

    public:
        _ProjectManagerNotifier() = default;

    signals:
        void ProjectOpened(); // Emitted when a new project has been opened
    };

    ///////////////////////////////////////////////////////////////////////////

    class ProjectManager {
    public:
        using Notifier = _ProjectManagerNotifier;
        enum class Path { WINDOW_STATE };

    public:
        // Get instance for connections
        static _ProjectManagerNotifier* GetNotifier();

        /// Call when loading app
        static void HWD_Load();

        /// Call when unloading app
        static void HWD_Unload();

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
    };

} // namespace HWD