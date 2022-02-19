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

// State save/load of WorkspacePanel
// - Expansion state is stored in QSettings (I_Panel::SavePanelState and I_Panel::LoadPanelState)

#pragma once

#include <UI/Models/FileBrowser/FileBrowserModel.hpp>
#include <DockWidget.h>
#include <UI/Windows/Panels/I_Panel.hpp>

namespace Ui {
    class WorkspacePanel;
}

namespace HWD::UI {

    class WorkspacePanel : public ads::CDockWidget, public I_Panel {
        Q_OBJECT
    public:
        WorkspacePanel();
        virtual ~WorkspacePanel() = default;

        void SavePanelState(QSettings* cfg) override;
        void LoadPanelState(QSettings* cfg) override;

        void SetRootPath(const QString& path);

    signals:
        void RequestOpenFile(const QString& path);

    private:
        /// Called on file/folder rightclick
        void OpenEntryContextMenu(const QPoint& point, const QModelIndex& index, const QFileInfo& info);

    private:
        std::unique_ptr<Ui::WorkspacePanel> ui;
        FileBrowserModel* m_FB_Model;
    };

} // namespace HWD::UI
