#include "ProjectFileBrowser.hpp"

#include <Debugger/ELF/ELF_Reader.hpp>
#include <QFileDialog>
#include <QScrollBar>
#include <QTimer>
#include <set>
#include <Probe/I_Probe.hpp>

#include "ui_ProjectFileBrowser.h"

namespace HWD::UI {

    ProjectFileBrowser::ProjectFileBrowser() :
        KDDockWidgets::DockWidget(QStringLiteral("ProjectFileBrowser"), KDDockWidgets::DockWidgetBase::Option_DeleteOnClose),
        ui(std::make_unique<Ui::ProjectFileBrowser>()) {
        ui->setupUi(this);

        QString rootPath = "C:\\CFXS_Projects\\CFXS-RTOS-Test";

        m_FS_Model = new QFileSystemModel(this);

        m_FS_Model->setFilter(QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files);

        m_FS_Model->setRootPath(rootPath);

        ui->treeView->setModel(m_FS_Model);
        ui->treeView->setRootIndex(m_FS_Model->index(rootPath));

        ui->treeView->hideColumn(3);
        ui->treeView->hideColumn(2);
        ui->treeView->hideColumn(1);

        ui->treeView->setHeaderHidden(true);

        ui->treeView->setIndentation(16);

        setWidget(ui->RootWidget);
    }

    ProjectFileBrowser::~ProjectFileBrowser() {
    }

} // namespace HWD::UI