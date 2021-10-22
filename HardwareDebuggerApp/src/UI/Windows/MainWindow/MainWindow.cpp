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
#include <QSettings>

#include <DockAreaWidget.h>
#include "CFXS_Center_Widget.hpp"

using ads::CDockManager;
using ads::CDockWidget;
using ads::DockWidgetArea;

namespace HWD::UI {

    //////////////////////////////////////////////////////////////////////////////////////////////////
    static constexpr int STATE_SERIALIZER_VERSION      = 1; // Used to track state loading differences
    static const QString LAST_SESSION_PERSPECTIVE_NAME = QStringLiteral("__LastSession");
    static const QString CENTRAL_WIDGET_NAME           = QStringLiteral("__CentralWidget");
    static const QString KEY_VERSION                   = QStringLiteral("version");
    static const QString KEY_WINDOW_GEOMETRY           = QStringLiteral("windowGeometry");
    static const QString KEY_WINDOW_STATE              = QStringLiteral("windowState");
    static const QString KEY_DOCK_STATE                = QStringLiteral("dockState");
    static const QString KEY_OPEN_PANELS               = QStringLiteral("openPanels");
    //////////////////////////////////////////////////////////////////////////////////////////////////

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

        CDockWidget* centralDockWidget = new CDockWidget(CENTRAL_WIDGET_NAME);
        centralDockWidget->setWidget(centralFrame);
        centralDockWidget->layout()->setContentsMargins({0, 0, 0, 0});

        centralDockWidget->setFeature(CDockWidget::NoTab, true);
        auto centralDockArea = GetDockManager()->setCentralWidget(centralDockWidget);
        centralDockArea->setAllowedAreas(DockWidgetArea::AllDockAreas);

        RegisterActions();
    }

    MainWindow::~MainWindow() {
    }

    ads::CDockManager* MainWindow::GetDockManager() {
        return ui->dockManager;
    }

    void MainWindow::closeEvent(QCloseEvent* event) {
        HWDLOG_UI_TRACE("Close MainWindow");
        QMap<QString, QVariant> state;
        state[KEY_VERSION]         = STATE_SERIALIZER_VERSION;
        state[KEY_WINDOW_GEOMETRY] = saveGeometry();
        state[KEY_WINDOW_STATE]    = saveState();
        state[KEY_DOCK_STATE]      = GetDockManager()->saveState();

        QStringList openPanelList;

        GetDockManager()->removePerspective(LAST_SESSION_PERSPECTIVE_NAME);
        GetDockManager()->addPerspective(LAST_SESSION_PERSPECTIVE_NAME);
        QSettings s;
        GetDockManager()->savePerspectives(s);

        HWDLOG_UI_TRACE("Open panels:");

        auto dockWidgets = GetDockManager()->dockWidgetsMap();
        for (auto w : dockWidgets) {
            if (w->objectName() != CENTRAL_WIDGET_NAME) {
                HWDLOG_UI_TRACE(" - {}", w->objectName());
                openPanelList.append(w->objectName());
            }
        }

        state[KEY_OPEN_PANELS] = openPanelList;

        s.setValue("windowStateData", state);

        emit StateDataReady(&s);
        emit Closed();
        event->accept();
    }

    void MainWindow::LoadState(QSettings& stateData) {
        if (!m_StateLoaded) {
            m_StateLoaded = true; // This is set to true even if state load fails

            if (stateData.status() != QSettings::Status::NoError) {
                HWDLOG_UI_ERROR("MainWindow failed to load state");
            } else {
                QMap<QString, QVariant> state = stateData.value("windowStateData").toMap();

                if (!state.isEmpty()) {
                    if (!state.contains(KEY_VERSION)) {
                        HWDLOG_UI_ERROR("MainWindow failed to load state - no version field");
                        return;
                    }

                    switch (state[KEY_VERSION].toInt()) {
                        case 1: {
                            HWDLOG_UI_TRACE("MainWindow load state [version = {0}]", state[KEY_VERSION].toInt());
                            if (!state.contains(KEY_WINDOW_GEOMETRY) || !state.contains(KEY_WINDOW_STATE) ||
                                !state.contains(KEY_DOCK_STATE) || !state.contains(KEY_OPEN_PANELS)) {
                                HWDLOG_UI_ERROR("MainWindow failed to load state v{0} - invalid data", state[KEY_VERSION].toInt());
                                return;
                            }

                            // Open all stored panels
                            for (auto& panelName : state[KEY_OPEN_PANELS].toStringList()) {
                                bool known = true;
                                if (panelName == QStringLiteral("WorkspacePanel")) {
                                    OpenPanel_Workspace();
                                } else {
                                    known = false;
                                }

                                if (known) {
                                    HWDLOG_UI_TRACE(" - Open {}", panelName);
                                } else {
                                    HWDLOG_UI_ERROR(" - Unknown panel: {}", panelName);
                                }
                            }

                            // Load data
                            restoreGeometry(state[KEY_WINDOW_GEOMETRY].toByteArray());
                            restoreState(state[KEY_WINDOW_STATE].toByteArray());
                            //GetDockManager()->restoreState(state[KEY_DOCK_STATE].toByteArray());

                            GetDockManager()->loadPerspectives(stateData);
                            GetDockManager()->openPerspective(LAST_SESSION_PERSPECTIVE_NAME);

                            break;
                        }
                        default: HWDLOG_UI_ERROR("MainWindow failed to load state - unsupported version"); return;
                    }
                } else {
                    HWDLOG_UI_ERROR("MainWindow failed to load state - corrupted data");
                    return;
                }
            }
        } else {
            HWDLOG_UI_WARN("MainWindow state already loaded");
        }
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
            GetDockManager()->addDockWidgetFloating(m_Panel_Workspace);
            m_Panel_Workspace->SetRootPath("C:/CFXS_Projects/CFXS-RTOS-Test");
        } else {
            m_Panel_Workspace->toggleView();
            m_Panel_Workspace->raise();
        }
    }

} // namespace HWD::UI