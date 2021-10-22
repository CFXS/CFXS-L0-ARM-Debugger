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

    FileBrowserModel::FileBrowserModel(QTreeView* workingTreeView, QObject* parent) :
        QFileSystemModel(parent), m_WorkingTreeView(workingTreeView) {
        setIconProvider(&m_IconProvider);
    }

    QVariant FileBrowserModel::data(const QModelIndex& index, int role) const {
        if (role == QFileSystemModel::FileIconRole && m_WorkingTreeView->isVisible()) {
            auto info = fileInfo(index);
            if (info.isDir()) {
                return m_IconProvider.icon(info, m_WorkingTreeView->isExpanded(index));
            } else {
                return m_IconProvider.icon(info, false);
            }
        }

        return QFileSystemModel::data(index, role);
    }

} // namespace HWD::UI