#pragma once

#include <KDDockWidgets/DockWidget.h>
#include <QFileSystemModel>

namespace Ui {
    class ProjectFileBrowser;
}

namespace HWD::UI {

    class ProjectFileBrowser : public KDDockWidgets::DockWidget {
        Q_OBJECT

    public:
        ProjectFileBrowser();
        ~ProjectFileBrowser();

    private:
        std::unique_ptr<Ui::ProjectFileBrowser> ui;
        QFileSystemModel* m_FS_Model;
    };

} // namespace HWD::UI
