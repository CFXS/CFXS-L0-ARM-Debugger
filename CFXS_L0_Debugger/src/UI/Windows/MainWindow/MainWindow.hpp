// ---------------------------------------------------------------------
// CFXS L0 ARM Debugger <https://github.com/CFXS/CFXS-L0-ARM-Debugger>
// Copyright (C) 2022 | CFXS
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
#pragma once

#include <QMainWindow>
#include "DockManager.h"

#include <UI/Windows/Panels/I_Panel.hpp>

namespace Ui {
    class MainWindow;
}

namespace L0::UI {

    class WorkspacePanel;
    class TextEditPanel;
    class AppLogPanel;

    class MainWindow : public QMainWindow {
        Q_OBJECT

        struct ActionEntryDefinition {
            bool isSeperator;
            QString name                               = QStringLiteral("");
            std::function<void(MainWindow *)> callback = nullptr;
            QIcon icon                                 = QIcon{};
        };

    public:
        /// \param rawStateToLoad state data received from SaveState signal on window close
        explicit MainWindow(QWidget *parent = nullptr);
        ~MainWindow();

        /// save window + dock state and emit ready signal
        void SaveState();

        /// Load window + docking state
        void LoadState(QSettings &stateData);

        /// Hide all panels - required for state load if new state contains less panels than current
        void HideAllPanels();

    private:
        /// getter for ui->dockManager
        ads::CDockManager *GetDockManager();

        /// Update window title
        void UpdateTitle();

        /// Update recent projects list
        void UpdateRecentProjects();

        /// connect actions to functions
        void InitializeActions();
        void InitializeActions_File();
        void InitializeActions_View();
        void InitializeActions_Help();

        // Normal panels
        WorkspacePanel *OpenPanel_Workspace();
        TextEditPanel *OpenPanel_TextEdit(const QString &path);

        // Dev panels
        AppLogPanel *OpenPanel_AppLog();

        /// Open/show file panel with file
        TextEditPanel *OpenFilePanel(const QString &path);

    signals:
        void Closed();

        /// Emitted when window + docking state has been serialized
        void StateDataReady(QSettings *stateData);

    protected:
        void closeEvent(QCloseEvent *event);

    private:
        std::unique_ptr<Ui::MainWindow> ui;
        ads::CDockManager *m_DockManager;

        static std::vector<ActionEntryDefinition> s_ActionDefinitions_View;

        // Panels
        WorkspacePanel *m_Panel_Workspace = nullptr;
        AppLogPanel *m_Panel_AppLog       = nullptr;
    };

} // namespace L0::UI