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
#include <QWidgetAction>
#include <QLabel>

namespace L0::UI::Utils {

    inline QWidgetAction* CreateQMenuTextSeparator(const QString& text) {
        auto* label = new QLabel(text);
        label->setStyleSheet("color: rgb(220, 220, 220); font-weight: 500; background: rgba(255, 255, 255, 32);");
        label->setAlignment(Qt::AlignCenter);
        auto* separator = new QWidgetAction(nullptr);
        separator->setDefaultWidget(label);
        return separator;
    }

    inline QWidgetAction* CreateQMenuTextSeparator(const QString& text,
                                                   const QString& styleSheet,
                                                   Qt::Alignment alignment = Qt::AlignVCenter) {
        auto* label = new QLabel(text);
        label->setStyleSheet(styleSheet);
        label->setAlignment(alignment);
        auto* separator = new QWidgetAction(nullptr);
        separator->setDefaultWidget(label);
        return separator;
    }

} // namespace L0::UI::Utils