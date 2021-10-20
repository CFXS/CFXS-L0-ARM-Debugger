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
#include "MainWindow.hpp"

#include <QCloseEvent>
#include <QFileDialog>
#include <Debugger/ELF/ELF_Reader.hpp>

#include <QLabel>
#include <QTableWidget>

#include <QTreeView>
#include <UI/Models/FileBrowser/FileBrowserModel.hpp>

using ads::CDockManager;
using ads::CDockWidget;
using ads::DockWidgetArea;

namespace HWD::UI {

    MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
        setWindowTitle(QStringLiteral(CFXS_HWD_PROGRAM_NAME));
        resize(640, 480);

        m_DockManager = new ads::CDockManager(this);

        auto fileTree = new QTreeView();

        auto rootPath = "C:/CFXS_Projects/CFXS-RTOS-Test";

        auto m_FB_Model = new FileBrowserModel(fileTree, this);
        //m_FB_Model->setFilter(QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files);
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

        // Create a dock widget with the title Label 1 and set the created label
        // as the dock widget content
        ads::CDockWidget* dockWidget = new ads::CDockWidget("Project Files");
        dockWidget->setWidget(fileTree);

        m_DockManager->addDockWidget(ads::TopDockWidgetArea, dockWidget);
    }

    MainWindow::~MainWindow() {
    }

    void MainWindow::closeEvent(QCloseEvent* event) {
        emit Closed();
        event->accept();
    }

} // namespace HWD::UI