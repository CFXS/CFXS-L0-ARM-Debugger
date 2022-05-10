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
#include "../SymbolListPanel.hpp"

#include <DockWidgetTab.h>

#include <UI/Models/Symbols/SymbolTableModel.hpp>
#include <UI/Models/Symbols/SymbolSearchSortProxy.hpp>
#include <Core/Project/ProjectManager.hpp>
#include <QBoxLayout>
#include <QDir>
#include <QFileInfo>
#include <QPixmap>
#include <QPoint>
#include <QScrollBar>
#include <QTimer>

#include "ui_SymbolListPanel.h"

namespace L0::UI {

    SymbolListPanel::SymbolListPanel() : ads::CDockWidget(GetPanelBaseName()), ui(std::make_unique<Ui::SymbolListPanel>()) {
        LOG_UI_TRACE("Create {}", GetPanelBaseName());
        ui->setupUi(this);
        setWindowTitle("Symbols");

        m_SymbolTableModel      = new SymbolTableModel(this);
        m_SymbolSearchSortProxy = new SymbolSearchSortProxy(this);

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
        ui->symbolTable->setStyleSheet("QTreeView{font-size: 14px;}"
                                       "QTreeView::item {margin: 0px; border: 1px; border-style: thin; border-color: rgba(255,255,255,32);}"
                                       "QTreeView::item:alternate{background-color:rgb(28,28,28);}");
        ui->symbolTable->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
        ui->symbolTable->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
        ui->symbolTable->setRootIsDecorated(false);
        ui->symbolTable->setUniformRowHeights(true);
        ui->symbolTable->setAlternatingRowColors(true);

        ui->symbolTable->setSortingEnabled(true);
        ui->symbolTable->sortByColumn(1, Qt::AscendingOrder);

        m_SymbolSearchSortProxy->setSourceModel(m_SymbolTableModel);
        ui->symbolTable->setModel(m_SymbolSearchSortProxy);

        connect(ui->searchTextBar,
                qOverload<const QString&>(&QLineEdit::textChanged),
                m_SymbolSearchSortProxy,
                &SymbolSearchSortProxy::SetNameFilter);

        setWidget(ui->root);
    }

    void SymbolListPanel::SavePanelState(QSettings* cfg) {
        QString cfgKey = objectName();
        cfgKey.replace('/', '\\'); // QSettings does not like '/' in keys
        LOG_UI_TRACE("SymbolListPanel save state - {}", cfgKey);

        cfg->beginGroup(cfgKey);
        cfg->setValue("version", 1);

        QStringList columnWidths;
        for (int i = 0; i < ui->symbolTable->model()->columnCount(); i++) {
            columnWidths.push_back(QString::number(ui->symbolTable->header()->sectionSize(i)));
        }
        cfg->setValue("columnWidths", columnWidths);

        cfg->endGroup();
    }

    void SymbolListPanel::LoadPanelState(QSettings* cfg) {
        QString cfgKey = objectName();
        cfgKey.replace('/', '\\'); // QSettings does not like '/' in keys
        LOG_UI_TRACE("SymbolListPanel load state - {}", objectName());

        if (!cfg->childGroups().contains(cfgKey)) {
            LOG_UI_WARN(" - No config entry for {}", cfgKey);
            return; // no cfg entry
        }

        cfg->beginGroup(cfgKey);
        auto version = cfg->value("version").toInt();
        if (version == 1) {
            auto widthEntries = cfg->value("columnWidths").toStringList();
            int localIndex    = 0;
            for (auto w : widthEntries) {
                ui->symbolTable->header()->resizeSection(localIndex, w.toInt());
                ui->symbolTable->setColumnWidth(localIndex, w.toInt());
                localIndex++;
            }
        } else {
            LOG_UI_ERROR(" - Unsupported SymbolListPanel state data version {}", version);
        }
        cfg->endGroup();
    }

} // namespace L0::UI