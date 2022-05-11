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
#include "../HexEditorPanel.hpp"

#include <DockWidgetTab.h>

#include <Core/Project/ProjectManager.hpp>
#include <QHexEdit2/QHexEdit.hpp>
#include <QBoxLayout>
#include <QDir>
#include <QFileInfo>
#include <QPoint>
#include <QScrollBar>
#include <QTimer>
#include <QFile>

#include "ui_HexEditorPanel.h"

namespace L0::UI {

    HexEditorPanel::HexEditorPanel() : ads::CDockWidget(GetPanelBaseName()), ui(std::make_unique<Ui::HexEditorPanel>()) {
        LOG_UI_TRACE("Create {}", GetPanelBaseName());
        ui->setupUi(this);
        setWindowTitle(QSL("Hex Editor"));

        m_HexEditor = new QHexEdit;
        ui->content->layout()->addWidget(m_HexEditor);

        ui->searchTextBar->setPlaceholderText("Jump to Address...");
        ui->searchTextBar->setStyleSheet("QLineEdit{color: gray;}");
        ui->searchTextBar->setObjectName("monospaceTextObject");
        connect(ui->searchTextBar, qOverload<const QString&>(&QLineEdit::textChanged), [=](const QString& text) {
            if (text.isEmpty()) {
                ui->searchTextBar->setStyleSheet("QLineEdit{color: gray;}");
            } else {
                ui->searchTextBar->setStyleSheet("QLineEdit{color: white;}");
            }
        });

        connect(ui->searchTextBar, &QLineEdit::returnPressed, [=]() {
            auto val = ui->searchTextBar->text();
            uint64_t addr;
            bool convOk;

            if (val.contains("0x")) {
                addr = val.toULongLong(&convOk, 16);
            } else {
                addr = val.toULongLong(&convOk, 10);
            }

            if (convOk) {
                m_HexEditor->setCursorPosition(addr * 2); // x2 because of byte size (2 units/byte)
                m_HexEditor->ensureVisible();
            }
        });

        m_HexEditor->setObjectName("monospaceTextObject");
        m_HexEditor->setHexFontColor(QColor{255, 255, 255});
        m_HexEditor->setAddressFontColor(QColor{160, 160, 160});
        m_HexEditor->setAsciiFontColor(QColor{240, 240, 240});
        m_HexEditor->setAsciiAreaColor(QColor{22, 22, 22});
        m_HexEditor->setAddressAreaColor(QColor{66, 66, 66});
        m_HexEditor->setSelectionColor(QColor{0, 105, 220, 128});
        m_HexEditor->setStyleSheet("QFrame{background-color: rgb(22, 22, 22);}");
        m_HexEditor->setBytesPerLine(16);
        m_HexEditor->setHexCaps(true);
        m_HexEditor->setReadOnly(true);
        m_HexEditor->SetFontSize(14);

        setWidget(ui->root);
    }

    void HexEditorPanel::SavePanelState(QSettings* cfg) {
        QString cfgKey = objectName();
        cfgKey.replace('/', '\\'); // QSettings does not like '/' in keys
        LOG_UI_TRACE("HexEditorPanel save state - {}", cfgKey);
    }

    void HexEditorPanel::LoadPanelState(QSettings* cfg) {
        QString cfgKey = objectName();
        cfgKey.replace('/', '\\'); // QSettings does not like '/' in keys
        LOG_UI_TRACE("HexEditorPanel load state - {}", cfgKey);
    }

    bool HexEditorPanel::LoadFile(const QString& filePath) {
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly)) {
            LOG_UI_WARN("HexEditorPanel failed to open file {} ({})", filePath, file.errorString());
            setWindowTitle(QSL("Hex Editor - No file loaded"));
            return false;
        }

        m_HexEditor->setData(file.readAll());
        file.close();

        QString relPath = QDir(ProjectManager::GetWorkspacePath()).relativeFilePath(filePath);

        if (relPath != filePath) {
            setObjectName(QSL("HexEditorPanel|./") + relPath);
            tabWidget()->setToolTip(QSL("./") + relPath);
        } else {
            setObjectName(QSL("HexEditorPanel|") + filePath);
            tabWidget()->setToolTip(filePath);
        }

        setWindowTitle(QSL("Hex Editor - ") + QFileInfo(filePath).fileName());

        setProperty("absoluteFilePath", filePath);

        return true;
    }

} // namespace L0::UI