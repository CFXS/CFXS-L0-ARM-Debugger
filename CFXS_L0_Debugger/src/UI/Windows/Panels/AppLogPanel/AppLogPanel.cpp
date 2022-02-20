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
#include "AppLogPanel.hpp"
#include "ui_AppLogPanel.h"
#include <Log/AppLogBufferSink.hpp>
#include <Core/Project/ProjectManager.hpp>
#include <UI/Models/FileBrowser/FileIconProvider.hpp>
#include <QFileInfo>
#include <QDir>
#include <QPixmap>
#include <DockWidgetTab.h>
#include <QBoxLayout>
#include <QScrollBar>
#include <QPoint>
#include <QTimer>

// TODO: optimize everything
// TODO: add color escape support

namespace L0::UI {

    AppLogPanel::AppLogPanel() : ads::CDockWidget(GetPanelBaseName()), ui(std::make_unique<Ui::AppLogPanel>()) {
        LOG_UI_TRACE("Create {}", GetPanelBaseName());
        ui->setupUi(this);

        setWindowTitle("Application Log");

        ui->textEdit->setReadOnly(true);
        ui->textEdit->setStyleSheet("background-color: rgb(0, 0, 0);");
        ui->textEdit->setObjectName("monospaceTextObject");

        QString currentBuf = "";
        for (auto& line : L0_AppLogBufferSink::GetLogBuffer()) {
            currentBuf.append(line);
        }

        FormatAndAppend(currentBuf);

        connect(_L0_AppLogBufferSinkNotifier::GetInstance(),
                &_L0_AppLogBufferSinkNotifier::Updated,
                [=](const std::vector<QString>& logBuffer, size_t totalSize) {
                    FormatAndAppend(logBuffer.back());
                    ui->textEdit->verticalScrollBar()->setValue(ui->textEdit->verticalScrollBar()->maximum());
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