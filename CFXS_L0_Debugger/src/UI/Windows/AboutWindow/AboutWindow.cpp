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
#include "AboutWindow.hpp"
#include "ui_AboutWindow.h"
#include "L0_Center_Widget.hpp"

namespace L0::UI {

    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////

    AboutWindow::AboutWindow(QWidget* parent) : QWidget(parent), ui(std::make_unique<Ui::AboutWindow>()) {
        ui->setupUi(this);
        setWindowTitle("About " + QSL(CFXS_PROGRAM_NAME));
        setFixedSize(size());
        setWindowFlag(Qt::WindowMinimizeButtonHint, false);
        setWindowFlag(Qt::WindowMaximizeButtonHint, false);
        setObjectName("AboutWindow");

        setStyleSheet("#AboutWindow{background-color: palette(base);}");

        new L0_Center_Widget(this);

        QFont labelFont = ui->label_Version->font();
        labelFont.setPointSize(labelFont.pointSize() * 1.5);
        ui->label_Version->setFont(labelFont);

        ui->label_Version->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

        char buildTime[32];
        snprintf(buildTime, 32, "%s @ %s", __DATE__, __TIME__);
        ui->label_Version->setText(QSL(CFXS_PROGRAM_NAME) + QSL(" ") + QSL(CFXS_VERSION_STRING) + QSL("\n") + buildTime);
    }

} // namespace L0::UI