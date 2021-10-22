// [CFXS] //
#pragma once

#include <QString>

namespace HWD {

    class ProjectManager {
    public:
        enum class Path { WINDOW_STATE };

    public:
        static void HWD_Load();
        static void HWD_Unload();

        static void OpenProject(const QString& workspacePath);

        static bool IsProjectOpen();
        static const QString& GetWorkspacePath();
        static QString GetProjectFilePath(Path path);
    };

} // namespace HWD