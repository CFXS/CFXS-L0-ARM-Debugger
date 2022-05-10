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
#include "../TargetBinaryPanel.hpp"

#include <DockWidgetTab.h>

#include <UI/Models/Symbols/SymbolTableModel.hpp>
#include <Core/Project/ProjectManager.hpp>
#include <QBoxLayout>
#include <QDir>
#include <QFileInfo>
#include <QPixmap>
#include <QPoint>
#include <QScrollBar>
#include <QTimer>

#include "ui_TargetBinaryPanel.h"

namespace L0::UI {

    TargetBinaryPanel::TargetBinaryPanel() : ads::CDockWidget(GetPanelBaseName()), ui(std::make_unique<Ui::TargetBinaryPanel>()) {
        LOG_UI_TRACE("Create {}", GetPanelBaseName());
        ui->setupUi(this);
        setWindowTitle("Target Binary");

        m_SymbolTableModel = new SymbolTableModel(this);

        ui->searchTextBar->setPlaceholderText("Search...");
        ui->searchTextBar->setStyleSheet("QLineEdit{color: gray;}");
        ui->searchTextBar->setObjectName("monospaceTextObject");
        connect(ui->searchTextBar, &QLineEdit::textChanged, [=] {
            if (ui->searchTextBar->text().isEmpty()) {
                ui->searchTextBar->setStyleSheet("QLineEdit{color: gray;}");
            } else {
                ui->searchTextBar->setStyleSheet("QLineEdit{color: white;}");
            }
        });

        ui->symbolTable->setObjectName("monospaceTextObject");
        ui->symbolTable->setSelectionMode(QAbstractItemView::SelectionMode::NoSelection);
        ui->symbolTable->setFocusPolicy(Qt::NoFocus);
        ui->symbolTable->setStyleSheet(
            "QTreeView{font-size: 14px;} QTreeView::item {margin: 0px; border: 1px; border-style: thin; border-color: rgba(255,255,255,32);}");
        ui->symbolTable->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
        ui->symbolTable->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
        ui->symbolTable->setUniformRowHeights(true);
        ui->symbolTable->setModel(m_SymbolTableModel);

        setWidget(ui->root);
    }

    void TargetBinaryPanel::SavePanelState(QSettings* cfg) {
    }

    void TargetBinaryPanel::LoadPanelState(QSettings* cfg) {
    }

} // namespace L0::UI