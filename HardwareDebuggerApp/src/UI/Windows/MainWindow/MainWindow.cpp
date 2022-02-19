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
#include <UI/Windows/Panels/WorkspacePanel/WorkspacePanel.hpp>
#include <UI/Windows/Panels/TextEditPanel/TextEditPanel.hpp>
#include <Core/Project/ProjectManager.hpp>

using ads::CDockManager;
using ads::CDockWidget;
using ads::DockWidgetArea;

/////////////////////////////////////////////////////////////
////////////////// [ Window/Panel Stuff ] //////////////////
/////////////////////////////////////////////////////////////
// Panel/Window differences:
//     - Window: A window that is not a part of the docking system
//     - Panel: A dockable window that's state is saved/loaded when opening/closing projects
// Panels are the main dynamic component of the HWD UI
// Each main debugger function like MemoryView and LiveWatch will have it's own panel (including open source files - TextEditPanel)
//
// Panel names and descriptions are stored in it's objectName in this format: PanelName|PanelData
// This is stored to the /.cfxs_hwd/WindowState.hwd file and when loaded, the PanelData string is passed to the constructed panel if necessary
// Right now this is the only idea I have to not make loading panels in LoadState() too messy
// All panels with the same base panel name must have different PanelData values as the docking system uses a QMap<QString, ...> to store its dock widgets
// If multiple panels with the same name are constructed, then saving and loading panels will not work right because of map key overlaps
// Good panel objectName example:
//    Panel #1 - WorkspacePanel
//    Panel #2 - MemoryViewPanel|0
//    Panel #3 - MemoryViewPanel|1,"Custom Panel Title Label"
//    Panel #4 - TextEditPanel|./CFXS_RTOS_Test/src/main.cpp
//    Panel #5 - TextEditPanel|./CFXS_RTOS_Test/src/_TM4C129X_Startup_RTOS.cpp

namespace HWD::UI {

    //////////////////////////////////////////////////////////////////////////////////////////////////
    static constexpr int STATE_SERIALIZER_VERSION      = 1; // Used to track state loading differences
    static const QString LAST_SESSION_PERSPECTIVE_NAME = QStringLiteral("__LastSession");
    static const QString CENTRAL_WIDGET_NAME           = QStringLiteral("__CentralWidget");
    static const QString KEY_VERSION                   = QStringLiteral("version");
    static const QString KEY_WINDOW_GEOMETRY           = QStringLiteral("windowGeometry");
    static const QString KEY_WINDOW_STATE              = QStringLiteral("windowState");
    static const QString KEY_DOCK_STATE                = QStringLiteral("dockState");
    static const QString KEY_OPEN_PANEL_DESCRIPTION    = QStringLiteral("openPanels");
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
                    // TODO: Check if everything saved

                    if (ProjectManager::IsProjectOpen()) {
                        if (ProjectManager::GetWorkspacePath() == path) { // don't open same project again
                            HWDLOG_UI_TRACE("Trying to open same project");
                            return;
                        }

                        SaveState();
                    }
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
        QSettings cfg;
        GetDockManager()->savePerspectives(cfg);

        auto dockWidgets = GetDockManager()->dockWidgetsMap();
        for (auto w : dockWidgets) {
            // Don't store info about the central widget
            if (w->objectName() != CENTRAL_WIDGET_NAME) {
                openPanelList.append(w->objectName());
                auto panel = dynamic_cast<I_Panel*>(w);
                if (panel) {
                    panel->SavePanelState(&cfg);
                }
            }
        }

        state[KEY_OPEN_PANEL_DESCRIPTION] = openPanelList;

        cfg.setValue(KEY_WINDOW_DATA, state);

        emit StateDataReady(&cfg);
    }

    void MainWindow::HideAllPanels() {
        for (auto panel : GetDockManager()->dockWidgetsMap()) {
            if (panel->objectName() != CENTRAL_WIDGET_NAME) {
                if (panel->objectName().contains("TextEditPanel")) {
                    GetDockManager()->removeDockWidget(panel);
                    panel->deleteLater();
                } else {
                    panel->toggleView(false);
                }
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
                    case 1: { // Window state format version 1
                        HWDLOG_UI_TRACE("MainWindow load state [version = {0}]", state[KEY_VERSION].toInt());
                        if (!state.contains(KEY_WINDOW_GEOMETRY) || !state.contains(KEY_WINDOW_STATE) || !state.contains(KEY_DOCK_STATE) ||
                            !state.contains(KEY_OPEN_PANEL_DESCRIPTION)) {
                            HWDLOG_UI_ERROR("MainWindow failed to load state v{0} - invalid data", state[KEY_VERSION].toInt());
                            return;
                        }

                        // Open all stored panels
                        for (auto& panelDescription : state[KEY_OPEN_PANEL_DESCRIPTION].toStringList()) {
                            bool known = true;

                            // panelDescription format:
                            // PanelName|PanelData...
                            QString panelName;
                            QString panelData;
                            auto seperatorOffset = panelDescription.indexOf("|");

                            if (seperatorOffset > 0) {
                                panelName = panelDescription.mid(0, seperatorOffset);  // start to '|'
                                panelData = panelDescription.mid(seperatorOffset + 1); // '|' to end
                            } else {
                                panelName = panelDescription;
                            }

                            if (panelName == QStringLiteral("WorkspacePanel")) {
                                auto workspacePanel = OpenPanel_Workspace();

                                if (workspacePanel) {
                                    workspacePanel->LoadPanelState(&stateData);
                                }
                            } else if (panelName == QStringLiteral("TextEditPanel")) {
                                auto filePanel = OpenFilePanel(panelData); // panelData for TextEditPanel is relative file path
                                if (filePanel) {
                                    filePanel->LoadPanelState(&stateData);
                                }
                            } else {
                                known = false;
                            }

                            if (!known) {
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
                HWDLOG_UI_TRACE("Not opening project - no directory selected");
            } else {
                HWDLOG_UI_TRACE("Selected project directory - {}", dir);

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

    I_Panel* MainWindow::OpenPanel_Workspace() {
        if (!m_Panel_Workspace) {
            m_Panel_Workspace = new WorkspacePanel;
            GetDockManager()->addDockWidgetFloating(static_cast<WorkspacePanel*>(m_Panel_Workspace));

            connect(static_cast<WorkspacePanel*>(m_Panel_Workspace), &WorkspacePanel::RequestOpenFile, [=](const QString& path) {
                OpenFilePanel(path);
            });
        } else {
            static_cast<WorkspacePanel*>(m_Panel_Workspace)->toggleView();
            static_cast<WorkspacePanel*>(m_Panel_Workspace)->raise();
        }

        return m_Panel_Workspace;
    }

    I_Panel* MainWindow::OpenFilePanel(const QString& path) {
        QString newPath = QDir(ProjectManager::GetWorkspacePath()).relativeFilePath(path);
        auto fullPath   = ProjectManager::GetFullFilePath(newPath);
        QFileInfo fileInfo(fullPath);

        if (!fileInfo.exists()) {
            HWDLOG_UI_TRACE("OpenFile file does not exist - {}", path);
            return nullptr;
        }

        TextEditPanel* existingEditor = nullptr;
        for (auto panel : GetDockManager()->dockWidgetsMap()) {
            if (panel->objectName().contains(QStringLiteral("TextEditPanel")) &&
                panel->property("absoluteFilePath").toString() == fullPath) {
                existingEditor = static_cast<TextEditPanel*>(panel);
                break;
            }
        }

        // if exists, set as current tab
        if (existingEditor) {
            existingEditor->setAsCurrentTab();
            return existingEditor;
        } else {
            return OpenPanel_TextEdit(fullPath);
        }
    }

    I_Panel* MainWindow::OpenPanel_TextEdit(const QString& path) {
        auto textEditPanel = new TextEditPanel;
        textEditPanel->SetFilePath(path);
        GetDockManager()->addDockWidgetTabToArea(textEditPanel, GetDockManager()->centralWidget()->dockAreaWidget());
        textEditPanel->setFeature(CDockWidget::DockWidgetAlwaysCloseAndDelete, true); // delete on close

        return textEditPanel;
    }

} // namespace HWD::UI