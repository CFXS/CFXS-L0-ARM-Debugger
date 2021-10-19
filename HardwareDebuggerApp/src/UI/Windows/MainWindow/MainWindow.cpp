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

namespace HWD::UI {

    MainWindow::MainWindow(QWidget *parent) :
        KDDockWidgets::MainWindow(QStringLiteral(CFXS_HWD_PROGRAM_NAME), KDDockWidgets::MainWindowOption_HasCentralFrame, parent) {
        resize(640, 480);
    }

    MainWindow::~MainWindow() {
    }

    void MainWindow::closeEvent(QCloseEvent *event) {
        emit Closed();
        event->accept();
    }

} // namespace HWD::UI