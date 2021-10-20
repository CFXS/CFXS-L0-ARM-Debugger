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
#include "ProjectFileBrowser.hpp"

#include <Debugger/ELF/ELF_Reader.hpp>
#include <QFileDialog>
#include <QScrollBar>
#include <QTimer>
#include <set>
#include <Probe/I_Probe.hpp>

#include "ui_ProjectFileBrowser.h"

namespace HWD::UI {

    ProjectFileBrowser::ProjectFileBrowser(const QString& rootPath) :
        KDDockWidgets::DockWidget(QStringLiteral("ProjectFileBrowser"), KDDockWidgets::DockWidgetBase::Option_DeleteOnClose),
        ui(std::make_unique<Ui::ProjectFileBrowser>()) {
        ui->setupUi(this);

        auto fileTree = ui->treeView;

        m_FB_Model = new FileBrowserModel(fileTree, this);
        m_FB_Model->setFilter(QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files | QDir::Hidden);
        m_FB_Model->setRootPath(rootPath);

        fileTree->setSelectionMode(QAbstractItemView::NoSelection); // No highlight
        fileTree->setFocusPolicy(Qt::NoFocus);                      // No focus box

        fileTree->setModel(m_FB_Model);
        fileTree->setRootIndex(m_FB_Model->index(rootPath));

        for (int i = 1; i < fileTree->model()->columnCount(); i++) {
            fileTree->hideColumn(i); // hide all columns except name
        }

        fileTree->setHeaderHidden(true);
        fileTree->setIndentation(16);

        setWidget(ui->RootWidget);
    }

    ProjectFileBrowser::~ProjectFileBrowser() {
    }

} // namespace HWD::UI