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
#include "../AppLogPanel.hpp"

#include <DockWidgetTab.h>

#include <Core/Project/ProjectManager.hpp>
#include <Log/AppLogBufferSink.hpp>
#include <QBoxLayout>
#include <QDir>
#include <QFileInfo>
#include <QPixmap>
#include <QPoint>
#include <QScrollBar>
#include <QTimer>
#include <UI/Providers/FileIconProvider.hpp>

#include "ui_AppLogPanel.h"

// TODO: optimize everything
// TODO: add color escape support

namespace L0::UI {

    AppLogPanel::AppLogPanel() : ads::CDockWidget(GetPanelBaseName()), ui(std::make_unique<Ui::AppLogPanel>()) {
        LOG_UI_TRACE("Create {}", GetPanelBaseName());
        ui->setupUi(this);

        setWindowTitle("Application Log");

        ui->textEdit->setReadOnly(true);
        ui->textEdit->setWordWrapMode(QTextOption::NoWrap);
        ui->textEdit->setStyleSheet("QTextEdit{background-color: rgb(0, 0, 0);}");
        ui->textEdit->setObjectName("monospaceTextObject");

        QString currentBuf = "";
        for (auto& line : L0_AppLogBufferSink::GetLogBuffer()) {
            currentBuf.append(line);
        }

        FormatAndAppend(currentBuf);

        connect(_L0_AppLogBufferSinkNotifier::GetInstance(),
                &_L0_AppLogBufferSinkNotifier::Updated,
                [=](const std::vector<QString>& logBuffer, size_t totalSize) {
                    if (logBuffer.empty() || totalSize == 0) {
                        ui->textEdit->setText("");
                        ui->textEdit->verticalScrollBar()->setValue(0);
                    } else {
                        FormatAndAppend(logBuffer.back());
                        ui->textEdit->verticalScrollBar()->setValue(ui->textEdit->verticalScrollBar()->maximum());
                    }
                });

        // >0ms delay for widget layouts
        QTimer::singleShot(1, [=]() {
            ui->textEdit->verticalScrollBar()->setValue(ui->textEdit->verticalScrollBar()->maximum());
        });

        setWidget(ui->root);
    }

    void AppLogPanel::FormatAndAppend(const QString& str) {
        ui->textEdit->append(str.mid(0, str.length() - 2));
    }

    void AppLogPanel::SavePanelState(QSettings* cfg) {
    }

    void AppLogPanel::LoadPanelState(QSettings* cfg) {
    }

} // namespace L0::UI