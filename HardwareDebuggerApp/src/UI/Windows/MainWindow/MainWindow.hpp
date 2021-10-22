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
#pragma once

#include <QMainWindow>
#include "DockManager.h"

#include <UI/Windows/WorkspacePanel/WorkspacePanel.hpp>

namespace Ui {
    class MainWindow;
}

namespace HWD::UI {

    class MainWindow : public QMainWindow {
        Q_OBJECT

    public:
        /// \param rawStateToLoad state data received from SaveState signal on window close
        explicit MainWindow(QWidget *parent = nullptr);
        ~MainWindow();

        /// Load window + docking state
        void LoadState(const QByteArray &rawState);

    private:
        /// getter for ui->dockManager
        ads::CDockManager *GetDockManager();

        /// connect actions to functions
        void RegisterActions();

        /// Open workspace panel
        void OpenPanel_Workspace();

    signals:
        void Closed();

        /// TODO: check if this has to be a copy or ref
        /// Emitted when window + docking state has been serialized
        void StateDataReady(const QByteArray &rawState);

    protected:
        void closeEvent(QCloseEvent *event);

    private:
        bool m_StateLoaded = false; // true when LoadState has been called
        std::unique_ptr<Ui::MainWindow> ui;
        ads::CDockManager *m_DockManager;

        // Panels

        WorkspacePanel *m_Panel_Workspace = nullptr;
    };

} // namespace HWD::UI