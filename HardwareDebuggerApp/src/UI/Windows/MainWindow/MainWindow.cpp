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
#include <QDesktopServices>
#include <QFileDialog>

#include <DockAreaWidget.h>
#include "CFXS_Center_Widget.hpp"

#include <UI/Windows/AboutWindow/AboutWindow.hpp>
#include <Core/Project/ProjectManager.hpp>

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
    static const QString KEY_WINDOW_DATA               = QStringLiteral("windowStateData");
    //////////////////////////////////////////////////////////////////////////////////////////////////

    MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(std::make_unique<Ui::MainWindow>()) {
        ui->setupUi(this);
        UpdateTitle();
        resize(1280, 720); // default size

        auto menuFont = ui->menubar->font();
        menuFont.setPointSize(menuFont.pointSize() + 1);
        ui->menubar->setFont(menuFont);

        for (auto action : ui->menubar->actions()) {
            auto actionFont = action->font();
            actionFont.setPointSize(actionFont.pointSize() + 1);
            action->setFont(actionFont);
        }

        auto centralFrame = new CFXS_Center_Widget;
        centralFrame->setObjectName("MainWindowCentralFrame");

        CDockWidget* centralDockWidget = new CDockWidget(CENTRAL_WIDGET_NAME);
        centralDockWidget->setWidget(centralFrame);
        centralDockWidget->layout()->setContentsMargins({0, 0, 0, 0});

        centralDockWidget->setFeature(CDockWidget::NoTab, true);
        auto centralDockArea = GetDockManager()->setCentralWidget(centralDockWidget);
        centralDockArea->setAllowedAreas(DockWidgetArea::AllDockAreas);

        RegisterActions();

        // Update title if project dir changed
        connect(ProjectManager::GetNotifier(), &ProjectManager::Notifier::ProjectOpened, this, [=]() {
            UpdateTitle();
        });

        // Update recent projects if recent project list changed
        connect(ProjectManager::GetNotifier(), &ProjectManager::Notifier::RecentProjectsChanged, this, [=]() {
            UpdateRecentProjects();
        });
        UpdateRecentProjects();
    }

    MainWindow::~MainWindow() {
    }

    ads::CDockManager* MainWindow::GetDockManager() {
        return ui->dockManager;
    }

    void MainWindow::closeEvent(QCloseEvent* event) {
        HWDLOG_UI_TRACE("Close MainWindow");
        SaveState();
        emit Closed();
        event->accept();
    }

    ///////////////////////////////////////////////////////////////////

    void MainWindow::UpdateTitle() {
        if (ProjectManager::IsProjectOpen()) {
            auto path           = ProjectManager::GetWorkspacePath();
            QString lastDirName = path.mid(path.lastIndexOf("/") + 1);
            setWindowTitle(QStringLiteral(CFXS_HWD_PROGRAM_NAME) + " - " + lastDirName);
        } else {
            setWindowTitle(QStringLiteral(CFXS_HWD_PROGRAM_NAME) + " - No project open");
        }
    }

    void MainWindow::UpdateRecentProjects() {
        ui->menuOpen_Recent->clear();

        auto& recentList = ProjectManager::GetRecentProjectPaths();

        if (recentList.isEmpty()) {
            ui->menuOpen_Recent->addAction("No Recent Projects");
        } else {
            for (auto& path : recentList) {
                ui->menuOpen_Recent->addAction(path, [=]() {
                    ProjectManager::OpenProject(path);
                });
            }
            ui->menuOpen_Recent->addSeparator();
            ui->menuOpen_Recent->addAction("Clear Recent Projects", [=]() {
                ProjectManager::ClearRecentProjects();
            });
        }
    }

    void MainWindow::SaveState() {
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

        s.setValue(KEY_WINDOW_DATA, state);

        emit StateDataReady(&s);
    }

    void MainWindow::HideAllPanels() {
        for (auto panel : GetDockManager()->dockWidgetsMap()) {
            if (panel->objectName() != CENTRAL_WIDGET_NAME) {
                panel->toggleView(false);
            }
        }
    }

    void MainWindow::LoadState(QSettings& stateData) {
        if (stateData.status() != QSettings::Status::NoError) {
            HWDLOG_UI_ERROR("MainWindow failed to load state");
        } else {
            QMap<QString, QVariant> state = stateData.value(KEY_WINDOW_DATA).toMap();

            if (!state.isEmpty()) {
                if (!state.contains(KEY_VERSION)) {
                    HWDLOG_UI_ERROR("MainWindow failed to load state - no version field");
                    return;
                }

                switch (state[KEY_VERSION].toInt()) {
                    case 1: {
                        HWDLOG_UI_TRACE("MainWindow load state [version = {0}]", state[KEY_VERSION].toInt());
                        if (!state.contains(KEY_WINDOW_GEOMETRY) || !state.contains(KEY_WINDOW_STATE) || !state.contains(KEY_DOCK_STATE) ||
                            !state.contains(KEY_OPEN_PANELS)) {
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
    }

    ///////////////////////////////////////////////////////////////

    void MainWindow::RegisterActions() {
        RegisterActions_File();
        RegisterActions_Edit();
        RegisterActions_View();
        RegisterActions_Help();
    }

    void MainWindow::RegisterActions_File() {
        // Open Project...
        connect(ui->actionOpen_Project, &QAction::triggered, this, [=]() {
            QString dir = QFileDialog::getExistingDirectory(
                this, QStringLiteral("Open Project"), "", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

            if (dir.isEmpty()) {
                HWDLOG_CORE_TRACE("Not opening project - no directory selected");
            } else {
                HWDLOG_CORE_TRACE("Selected project directory - {}", dir);

                if (ProjectManager::IsProjectOpen()) {
                    SaveState();
                }

                ProjectManager::OpenProject(dir);
            }
        });

        // Exit
        connect(ui->actionExit, &QAction::triggered, this, [=]() {
            this->close();
        });
    }

    void MainWindow::RegisterActions_Edit() {
    }

    void MainWindow::RegisterActions_View() {
        // View > Workspace
        connect(ui->actionWorkspace, &QAction::triggered, this, [=]() {
            OpenPanel_Workspace();
        });
    }

    void MainWindow::RegisterActions_Help() {
        // CFXS HWD Github
        connect(ui->actionCFXS_HWD_Github_Page, &QAction::triggered, this, [=]() {
            QDesktopServices::openUrl(QStringLiteral("https://github.com/CFXS/CFXS-Hardware-Debugger"));
        });

        // CFXS Discord
        connect(ui->actionCFXS_Discord, &QAction::triggered, this, [=]() {
            QDesktopServices::openUrl(QStringLiteral("https://discord.gg/ZdhfzQAC4S"));
        });

        // About
        connect(ui->actionAbout, &QAction::triggered, this, [=]() {
            auto aboutWindow = new AboutWindow;
            aboutWindow->show();
        });
    }

    ////////////////////////////////////////////////////////////////////
    // Panel opening

    void MainWindow::OpenPanel_Workspace() {
        if (!m_Panel_Workspace) {
            m_Panel_Workspace = new WorkspacePanel;
            GetDockManager()->addDockWidgetFloating(m_Panel_Workspace);
        } else {
            m_Panel_Workspace->toggleView();
            m_Panel_Workspace->raise();
        }
    }

} // namespace HWD::UI