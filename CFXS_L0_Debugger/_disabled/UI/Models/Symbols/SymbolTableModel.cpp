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
#include "SymbolTableModel.hpp"
#include <Core/Project/ProjectManager.hpp>
#include <QTimer>

#include <Core/ELF/ELF_Reader.hpp>

namespace L0::ELF {
    extern ELF_Reader *g_Test_ELF_Reader;
}
using L0::ELF::g_Test_ELF_Reader;

namespace L0::UI {

    bool SymbolTableModel::IsDebugObjectLoaded() const {
        return g_Test_ELF_Reader;
    }

    SymbolTableModel::SymbolTableModel(QObject *parent) : QAbstractTableModel(parent) {
        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, [=]() {
            if (!m_ViewReady) {
                m_ViewReady = IsDebugObjectLoaded();
                if (m_ViewReady) {
                    beginResetModel();
                    endResetModel();
                    timer->deleteLater();
                }
            }
        });
        timer->start(100);
    }

    int SymbolTableModel::columnCount(const QModelIndex &parent) const {
        return Row::_COUNT;
    }

    int SymbolTableModel::rowCount(const QModelIndex &parent) const {
        return IsDebugObjectLoaded() ? (int)g_Test_ELF_Reader->GetBasicSymbolTable().size() : 0;
    }
    QVariant SymbolTableModel::headerData(int column, Qt::Orientation orientation, int role) const {
        if (role == Qt::DisplayRole) {
            if (orientation == Qt::Horizontal) {
                switch (column) {
                    case Row::NAME: return QSL("Name");
                    case Row::ADDRESS: return QSL("Address");
                    case Row::SIZE: return QSL("Size");
                    default: return QSL("");
                }
            }
        }

        return {};
    }

    QVariant SymbolTableModel::data(const QModelIndex &index, int role) const {
        if (!IsDebugObjectLoaded()) {
            return {};
        }

        if (role == Qt::DisplayRole) {
            auto symIt = g_Test_ELF_Reader->GetBasicSymbolTable().begin();
            std::advance(symIt, index.row());
            auto &sym = symIt.value();
            char tmp[32];
            switch (index.column()) {
                case Row::NAME: return sym.specialInfoString.length() ? sym.name + " (" + sym.specialInfoString + ")" : sym.name;
                case Row::ADDRESS: snprintf(tmp, 32, "0x%08llX", sym.address); return tmp;
                case Row::SIZE: snprintf(tmp, 32, "%llu", sym.size); return tmp;
            }
        } else if (role == SymbolTableModel::Role::SymbolMapName) {
            auto symIt = g_Test_ELF_Reader->GetBasicSymbolTable().begin();
            std::advance(symIt, index.row());
            auto &sym = symIt.value();
            switch (index.column()) {
                case Row::NAME: return sym.name;
            }
        }

        return {};
    }

} // namespace L0::UI