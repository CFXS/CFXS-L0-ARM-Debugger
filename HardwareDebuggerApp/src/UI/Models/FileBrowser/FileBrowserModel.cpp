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
#include "FileBrowserModel.hpp"
#include <QTreeView>

namespace HWD::UI {

    //////////////////////////////////////////////////////////////
    static std::vector<QPixmap> s_IconTable;
    static bool s_Initialized = false;

    enum Icon : int {
        FOLDER,
        VSCODE,
        GIT,
        BUILD,
        SRC,
        LIB,
        RESOURCE,
        //
        OPEN
    };

    static constexpr auto ICON_SIZE       = 16;
    static constexpr auto SCALE_ASPECT    = Qt::AspectRatioMode::IgnoreAspectRatio;
    static constexpr auto SCALE_TRANSFORM = Qt::TransformationMode::SmoothTransformation;
#define ICON_SCALE_OPTS ICON_SIZE, ICON_SIZE, SCALE_ASPECT, SCALE_TRANSFORM
    //////////////////////////////////////////////////////////////

    FileBrowserModel::FileBrowserModel(QTreeView* workingTreeView, QObject* parent) :
        QFileSystemModel(parent), m_WorkingTreeView(workingTreeView) {
        if (!s_Initialized) {
            s_Initialized = true;
            s_IconTable   = {
                // closed
                QPixmap(":/Icon/folder.png").scaled(ICON_SCALE_OPTS),          // Icon::FOLDER
                QPixmap(":/Icon/folder-vscode.png").scaled(ICON_SCALE_OPTS),   // Icon::VSCODE
                QPixmap(":/Icon/folder-git.png").scaled(ICON_SCALE_OPTS),      // Icon::GIT
                QPixmap(":/Icon/folder-build.png").scaled(ICON_SCALE_OPTS),    // Icon::BUILD
                QPixmap(":/Icon/folder-src.png").scaled(ICON_SCALE_OPTS),      // Icon::SRC
                QPixmap(":/Icon/folder-lib.png").scaled(ICON_SCALE_OPTS),      // Icon::LIB
                QPixmap(":/Icon/folder-resource.png").scaled(ICON_SCALE_OPTS), // Icon::RESOURCE

                // open
                QPixmap(":/Icon/folder-open.png").scaled(ICON_SCALE_OPTS),          // Icon::FOLDER
                QPixmap(":/Icon/folder-vscode-open.png").scaled(ICON_SCALE_OPTS),   // Icon::VSCODE
                QPixmap(":/Icon/folder-git-open.png").scaled(ICON_SCALE_OPTS),      // Icon::GIT
                QPixmap(":/Icon/folder-build-open.png").scaled(ICON_SCALE_OPTS),    // Icon::BUILD
                QPixmap(":/Icon/folder-src-open.png").scaled(ICON_SCALE_OPTS),      // Icon::SRC
                QPixmap(":/Icon/folder-lib-open.png").scaled(ICON_SCALE_OPTS),      // Icon::LIB
                QPixmap(":/Icon/folder-resource-open.png").scaled(ICON_SCALE_OPTS), // Icon::RESOURCE
            };
        }

        setIconProvider(&m_IconProvider);
    }

    QVariant FileBrowserModel::data(const QModelIndex& index, int role) const {
        if (role == QFileSystemModel::FileIconRole) {
            auto info = fileInfo(index);
            if (info.isDir()) {
                return m_IconProvider.icon(info, m_WorkingTreeView->isExpanded(index));
            }
        }

        return QFileSystemModel::data(index, role);
    }

} // namespace HWD::UI