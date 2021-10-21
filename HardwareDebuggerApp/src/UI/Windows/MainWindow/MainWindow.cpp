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
#include "ui_MainWindow.h"
#include <QCloseEvent>
#include <QTreeView>
#include <QAction>
#include <QLayout>

#include <DockAreaWidget.h>
#include "CFXS_Center_Widget.hpp"

using ads::CDockManager;
using ads::CDockWidget;
using ads::DockWidgetArea;

namespace HWD::UI {

    MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(std::make_unique<Ui::MainWindow>()) {
        ui->setupUi(this);
        setWindowTitle(QStringLiteral(CFXS_HWD_PROGRAM_NAME));
        resize(1280, 720);

        auto menuFont = ui->menubar->font();
        menuFont.setPointSize(menuFont.pointSize() + 1);
        ui->menubar->setFont(menuFont);

        for (auto action : ui->menubar->actions()) {
            auto actionFont = action->font();
            actionFont.setPointSize(actionFont.pointSize() + 1);
            action->setFont(actionFont);
        }

        CFXS_Center_Widget* centralFrame = new CFXS_Center_Widget;
        centralFrame->setObjectName("MainWindowCentralFrame");

        CDockWidget* centralDockWidget = new CDockWidget("CentralWidget");
        centralDockWidget->setWidget(centralFrame);
        centralDockWidget->layout()->setContentsMargins({0, 0, 0, 0});

        centralDockWidget->setFeature(ads::CDockWidget::NoTab, true);
        auto centralDockArea = ui->dockManager->setCentralWidget(centralDockWidget);
        centralDockArea->setAllowedAreas(DockWidgetArea::AllDockAreas);

        RegisterActions();
    }

    MainWindow::~MainWindow() {
    }

    void MainWindow::closeEvent(QCloseEvent* event) {
        emit Closed();
        event->accept();
    }

    ///////////////////////////////////////////////////////////////

    void MainWindow::RegisterActions() {
        // Exit
        connect(ui->actionExit, &QAction::triggered, this, [=]() {
            this->close();
        });

        // View > Workspace
        connect(ui->actionWorkspace, &QAction::triggered, this, [=]() {
            OpenPanel_Workspace();
        });
    }

    void MainWindow::OpenPanel_Workspace() {
        if (!m_Panel_Workspace) {
            m_Panel_Workspace = new WorkspacePanel;
            ui->dockManager->addDockWidget(ads::LeftDockWidgetArea, m_Panel_Workspace);
            m_Panel_Workspace->SetRootPath("C:/CFXS_Projects/CFXS-RTOS-Test");
        } else {
            m_Panel_Workspace->toggleView();
            if (m_Panel_Workspace->isFloating()) {
                m_Panel_Workspace->setFocus(Qt::FocusReason::ActiveWindowFocusReason);
                m_Panel_Workspace->raise();
            }
        }
    }

} // namespace HWD::UI