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
#include "TargetConsoleWindow.hpp"

#include <QTimer>
#include <set>

#include "ui_TargetConsoleWindow.h"
#include <Debugger/ELF/ELF_Reader.hpp>
#include <QFileDialog>
#include <QScrollBar>
#include <QFont>

namespace HWD {

    namespace Probe {
        extern char* s_Console;
        extern bool s_ConsoleUpdated;
    } // namespace Probe
    using namespace Probe;

} // namespace HWD

namespace HWD::UI {

    TargetConsoleWindow::TargetConsoleWindow() :
        KDDockWidgets::DockWidget(QStringLiteral("TargetConsoleWindow"), KDDockWidgets::DockWidgetBase::Option_DeleteOnClose),
        ui(std::make_unique<Ui::TargetConsoleWindow>()) {
        ui->setupUi(this);

        ui->text_Console->setStyleSheet("background-color: #111111; font-size: 16px;");

        QFont font("Source Code Pro");
        font.setStyleHint(QFont::Monospace);
        ui->text_Console->setFont(font);

        setWidget(ui->RootWidget);

        QTimer* rt = new QTimer(this);
        rt->setInterval(33);
        rt->setSingleShot(false);
        QObject::connect(rt, &QTimer::timeout, [=]() {
            if (s_ConsoleUpdated) {
                s_ConsoleUpdated = false;
                bool atMax       = ui->text_Console->verticalScrollBar()->value() == ui->text_Console->verticalScrollBar()->maximum();
                auto current     = ui->text_Console->verticalScrollBar()->value();

                ui->text_Console->setPlainText(s_Console);

                if (atMax) {
                    ui->text_Console->verticalScrollBar()->setValue(ui->text_Console->verticalScrollBar()->maximum());
                } else {
                    ui->text_Console->verticalScrollBar()->setValue(current);
                }
            }
        });
        rt->start();
    }

    TargetConsoleWindow::~TargetConsoleWindow() {
    }

} // namespace HWD::UI