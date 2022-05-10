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

#include <DockWidget.h>
#include <QFile>
#include <QFileInfo>
#include <QTextEdit>
#include <UI/Panels/I_Panel.hpp>

namespace Ui {
    class TargetBinaryPanel;
}

namespace L0::UI {

    class SymbolTableModel;

    class TargetBinaryPanel : public ads::CDockWidget, public I_Panel {
        Q_OBJECT
    public:
        TargetBinaryPanel();
        virtual ~TargetBinaryPanel() = default;

        /// Get panel base name (for type matching from string)
        static const QString& GetPanelBaseName() {
            static const QString name = QSL("TargetBinaryPanel");
            return name;
        }

        void SavePanelState(QSettings* cfg) override;
        void LoadPanelState(QSettings* cfg) override;

    private:
        SymbolTableModel* m_SymbolTableModel = nullptr;

    private:
        std::unique_ptr<Ui::TargetBinaryPanel> ui;
    };

} // namespace L0::UI
