// [CFXS] //
#include "ProjectManager.hpp"

namespace HWD {

    //////////////////////////////////////////////////////////////////////////////
    static bool s_ProjectOpen = false; // true when project folder has been opened
    static QString s_WorkspacePath;
    //////////////////////////////////////////////////////////////////////////////

    void ProjectManager::HWD_Load() {
        HWDLOG_PROJECT_INFO("ProjectManager load");
    }

    void ProjectManager::HWD_Unload() {
        HWDLOG_PROJECT_INFO("ProjectManager unload");
    }

    void ProjectManager::OpenProject(const QString& workspacePath) {
        HWDLOG_PROJECT_INFO("Open project {}", workspacePath);
        s_WorkspacePath = workspacePath;
    }

    bool ProjectManager::IsProjectOpen() {
        return s_ProjectOpen;
    }

    const QString& ProjectManager::GetWorkspacePath() {
        return s_WorkspacePath;
    }

    QString ProjectManager::GetProjectFilePath(Path path) {
        switch (path) {
            case Path::WINDOW_STATE: return GetWorkspacePath() + "/.cfxs_hwd/WindowState.hwd";
        }

        HWDLOG_PROJECT_CRITICAL("Unknown path {}", (int)path);
        return s_WorkspacePath + "_error_temp.txt";
    }

} // namespace HWD