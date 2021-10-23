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
#include "WorkspacePanel.hpp"
#include "ui_WorkspacePanel.h"
#include <QDesktopServices>
#include <Core/Project/ProjectManager.hpp>

#include <QDir>

namespace HWD::UI {

    ////////////////////////////////////////////////////////////
    const QStringList s_OpenWithExternalAppExtensionList = {
        QStringLiteral("pdf"),
        QStringLiteral("png"),
        QStringLiteral("bmp"),
        QStringLiteral("jpeg"),
        QStringLiteral("jpg"),
        QStringLiteral("webm"),
        QStringLiteral("gif"),
    };
    ////////////////////////////////////////////////////////////

    WorkspacePanel::WorkspacePanel() : ads::CDockWidget(QStringLiteral("Workspace")), ui(std::make_unique<Ui::WorkspacePanel>()) {
        HWDLOG_CORE_TRACE("Create workspace panel");
        ui->setupUi(this);

        setObjectName("WorkspacePanel");

        m_FB_Model = new FileBrowserModel(ui->tw_FileBrowser, this);
        m_FB_Model->setFilter(QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files);

        ui->tw_FileBrowser->setSelectionMode(QAbstractItemView::NoSelection); // No highlight
        ui->tw_FileBrowser->setFocusPolicy(Qt::NoFocus);                      // No focus box

        ui->tw_FileBrowser->setModel(m_FB_Model);

        for (int i = 1; i < ui->tw_FileBrowser->model()->columnCount(); i++) {
            ui->tw_FileBrowser->hideColumn(i); // hide all columns except name
        }

        ui->tw_FileBrowser->setHeaderHidden(true);
        ui->tw_FileBrowser->setUniformRowHeights(true);

        ui->tw_FileBrowser->setIndentation(16);

        connect(ui->tw_FileBrowser, &QTreeView::doubleClicked, this, [=](const QModelIndex& index) {
            auto info = m_FB_Model->fileInfo(index);
            if (info.isFile()) {
                // If extension matches external app list then open with external app
                if (s_OpenWithExternalAppExtensionList.contains(info.suffix())) {
                    QDesktopServices::openUrl(info.absoluteFilePath());
                }
            }
        });

        setWidget(ui->root);

        SetRootPath(ProjectManager::GetWorkspacePath());

        // Set root path if project dir changed
        connect(ProjectManager::GetNotifier(), &ProjectManager::Notifier::ProjectOpened, this, [=]() {
            SetRootPath(ProjectManager::GetWorkspacePath());
        });
    }

    WorkspacePanel::~WorkspacePanel() {
        HWDLOG_CORE_TRACE("Destroy workspace panel");
    };

    void WorkspacePanel::SetRootPath(const QString& path) {
        if (!path.isEmpty() && QDir().exists(path)) {
            m_FB_Model->setRootPath(path);
            ui->tw_FileBrowser->setRootIndex(m_FB_Model->index(path));
            ui->tw_FileBrowser->setVisible(true);
        } else {
            ui->tw_FileBrowser->setVisible(false);
        }
    }

} // namespace HWD::UI