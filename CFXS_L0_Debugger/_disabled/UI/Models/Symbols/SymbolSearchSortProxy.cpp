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
#include "SymbolSearchSortProxy.hpp"
#include <Utils/FuzzyMatcher.hpp>

namespace L0::UI {

    SymbolSearchSortProxy::SymbolSearchSortProxy(QObject *parent) : QSortFilterProxyModel(parent) {
    }

    void SymbolSearchSortProxy::SetNameFilter(const QString &filterString) {
        beginResetModel();
        m_FilterString = filterString;
        if (m_FilterString.length())
            m_NameFilterExpression = Utils::FuzzyMatcher::CreateRegularExpression(filterString, Qt::CaseSensitivity::CaseInsensitive);
        endResetModel();
    }

    ///////////////////////////////////////////////////////////////////////////////////

    bool SymbolSearchSortProxy::lessThan(const QModelIndex &left, const QModelIndex &right) const {
        QVariant leftData  = sourceModel()->data(left);
        QVariant rightData = sourceModel()->data(right);

        QString leftString  = leftData.toString();
        QString rightString = rightData.toString();
        switch (left.column()) {
            case 0: { // sort by name
                return QString::localeAwareCompare(leftString, rightString) < 0;
            }
            case 1: { // sort by address
                return leftString.toULongLong(nullptr, 16) < rightString.toULongLong(nullptr, 16);
            }
            case 2: { // sort by size
                return leftString.toULongLong(nullptr, 10) < rightString.toULongLong(nullptr, 10);
            }
            default: return true;
        }

        return true;
    }

    bool SymbolSearchSortProxy::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
        if (m_FilterString.length()) {
            bool isAddressQuery = m_FilterString.at(0) == '&';

            if (isAddressQuery && m_FilterString.size() >= 2) {
                QModelIndex nameIndex = sourceModel()->index(sourceRow, 0, sourceParent);
                QModelIndex addrIndex = sourceModel()->index(sourceRow, 1, sourceParent);
                QModelIndex sizeIndex = sourceModel()->index(sourceRow, 2, sourceParent);
                uint64_t address      = addrIndex.data(Qt::DisplayRole).toString().toULongLong(nullptr, 16);
                uint64_t size         = sizeIndex.data(Qt::DisplayRole).toString().toULongLong(nullptr, 10);
                bool validQuery;

                bool skip2 = (m_FilterString.at(1) == '>') || (m_FilterString.at(1) == '<') || (m_FilterString.at(1) == '=');

                uint64_t queryAddr = m_FilterString.mid(skip2 ? 2 : 1).toULongLong(&validQuery, 16);
                if (!validQuery) {
                    return false;
                }

                if (m_FilterString.at(1) == '>') {
                    return address >= queryAddr;
                } else if (m_FilterString.at(1) == '<') {
                    return address <= queryAddr;
                } else if (m_FilterString.at(1) == '=') {
                    return address == queryAddr;
                } else {
                    if (size == 0) {
                        size = 128;
                    }
                    return (queryAddr >= address) && (queryAddr <= (address + size));
                }
            } else {
                QModelIndex nameIndex = sourceModel()->index(sourceRow, 0, sourceParent);
                return m_NameFilterExpression.match(nameIndex.data(Qt::DisplayRole).toString()).hasMatch();
            }

        } else {
            return true;
        }
    }

} // namespace L0::UI