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
#include "MainWindow.hpp"

#include <QCloseEvent>
#include <QFileDialog>
#include <Debugger/ELF/ELF_Reader.hpp>

#include <QLabel>
#include <QTableWidget>

using ads::CDockManager;
using ads::CDockWidget;
using ads::DockWidgetArea;

namespace HWD::UI {

    MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
        setWindowTitle(QStringLiteral(CFXS_HWD_PROGRAM_NAME));
        resize(640, 480);

        m_DockManager = new ads::CDockManager(this);

        // Set central widget
        QLabel* label = new QLabel();
        label->setText("");
        label->setAlignment(Qt::AlignCenter);
        CDockWidget* CentralDockWidget = new CDockWidget("CentralWidget");
        CentralDockWidget->setWidget(label);
        CentralDockWidget->setFeature(ads::CDockWidget::NoTab, true);
        auto* CentralDockArea = m_DockManager->setCentralWidget(CentralDockWidget);

        // create other dock widgets
        QTableWidget* table = new QTableWidget();
        table->setColumnCount(3);
        table->setRowCount(10);
        CDockWidget* TableDockWidget = new CDockWidget("Table 1");
        TableDockWidget->setWidget(table);
        TableDockWidget->setMinimumSizeHintMode(CDockWidget::MinimumSizeHintFromDockWidget);
        TableDockWidget->resize(250, 150);
        TableDockWidget->setMinimumSize(200, 150);
        m_DockManager->addDockWidgetTabToArea(TableDockWidget, CentralDockArea);
        auto TableArea = m_DockManager->addDockWidget(DockWidgetArea::LeftDockWidgetArea, TableDockWidget);

        table = new QTableWidget();
        table->setColumnCount(5);
        table->setRowCount(1020);
        TableDockWidget = new CDockWidget("Table 2");
        TableDockWidget->setWidget(table);
        TableDockWidget->setMinimumSizeHintMode(CDockWidget::MinimumSizeHintFromDockWidget);
        TableDockWidget->resize(250, 150);
        TableDockWidget->setMinimumSize(200, 150);
        m_DockManager->addDockWidget(DockWidgetArea::BottomDockWidgetArea, TableDockWidget, TableArea);

        QTableWidget* propertiesTable = new QTableWidget();
        propertiesTable->setColumnCount(3);
        propertiesTable->setRowCount(10);
        CDockWidget* PropertiesDockWidget = new CDockWidget("Properties");
        PropertiesDockWidget->setWidget(propertiesTable);
        PropertiesDockWidget->setMinimumSizeHintMode(CDockWidget::MinimumSizeHintFromDockWidget);
        PropertiesDockWidget->resize(250, 150);
        PropertiesDockWidget->setMinimumSize(200, 150);
        m_DockManager->addDockWidget(DockWidgetArea::RightDockWidgetArea, PropertiesDockWidget, CentralDockArea);
    }

    MainWindow::~MainWindow() {
    }

    void MainWindow::closeEvent(QCloseEvent* event) {
        emit Closed();
        event->accept();
    }

} // namespace HWD::UI