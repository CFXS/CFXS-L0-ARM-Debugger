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

        m_BottomLabel = new QLabel;
        ui->content->layout()->addWidget(m_BottomLabel);
        m_BottomLabel->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
        m_BottomLabel->setFixedHeight(18);
        m_BottomLabel->setAlignment(Qt::AlignVCenter);
        m_BottomLabel->setObjectName("monospaceTextObject");

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
            if (val.length() == 0) {
                return;
            }

            uint64_t addr;
            bool convOk;

            enum _local_OP { NONE, ADD, SUB, MUL, DIV } op = _local_OP::NONE;

            switch (val.at(0).toLatin1()) {
                case '+': op = _local_OP::ADD; break;
                case '-': op = _local_OP::SUB; break;
                case '*': op = _local_OP::MUL; break;
                case '/': op = _local_OP::DIV; break;
                default:
                    if (!val.at(0).isDigit()) {
                        ui->searchTextBar->setText(QSL("Invalid operator '") + val.at(0) + QSL("'"));
                        ui->searchTextBar->setStyleSheet("QLineEdit{color: red;}");
                        return;
                    }
            }

            if (op != _local_OP::NONE) {
                if (val.length() == 1) {
                    ui->searchTextBar->setStyleSheet("QLineEdit{color: red;}");
                    return;
                }
                val = val.mid(1);
            }

            if (val.startsWith("0x")) {
                addr = val.toULongLong(&convOk, 16);
            } else if (val.startsWith("0b")) {
                if (val.length() == 2) {
                    convOk = false;
                    addr   = 0; // unused variable warning
                } else {
                    addr = val.mid(2).toULongLong(&convOk, 2);
                }
            } else if (val.startsWith("0")) {
                addr = val.toULongLong(&convOk, 8);
            } else {
                addr = val.toULongLong(&convOk, 10);
            }

            if (!convOk) {
                ui->searchTextBar->setStyleSheet("QLineEdit{color: red;}");
                return;
            }

            switch (op) {
                case _local_OP::NONE: break;
                case _local_OP::ADD: addr = (m_HexEditor->cursorPosition() / 2) + addr; break;
                case _local_OP::SUB: addr = (m_HexEditor->cursorPosition() / 2) - addr; break;
                case _local_OP::MUL: addr = (m_HexEditor->cursorPosition() / 2) * addr; break;
                case _local_OP::DIV:
                    if (addr == 0) {
                        ui->searchTextBar->setText(QSL("Division by Zero :("));
                        ui->searchTextBar->setStyleSheet("QLineEdit{color: red;}");
                        return;
                    }
                    addr = (m_HexEditor->cursorPosition() / 2) / addr;
                    break;
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

        connect(m_HexEditor, &QHexEdit::currentAddressChanged, [=](qint64 cursorPos) {
            static char infoStr[256];
            static char valueStr[64];

            if (m_HexEditor->data().isEmpty()) {
                m_BottomLabel->setText("");
                return;
            }

            auto selSize = m_HexEditor->GetSelectionSize();
            if (selSize) {
                auto data = m_HexEditor->GetChunkData(m_HexEditor->GetSelectionStart(), m_HexEditor->GetSelectionSize());

                switch (selSize) {
                    case 2:
                        snprintf(valueStr,
                                 sizeof(valueStr),
                                 "| LE: 0x%X | BE: 0x%X",
                                 qToLittleEndian(*(uint16_t*)data.data()),
                                 qToBigEndian(*(uint16_t*)data.data()));
                        break;
                    case 4:
                        snprintf(valueStr,
                                 sizeof(valueStr),
                                 "| LE: 0x%X | BE: 0x%X",
                                 qToLittleEndian(*(uint32_t*)data.data()),
                                 qToBigEndian(*(uint32_t*)data.data()));
                        break;
                    case 8:
                        snprintf(valueStr,
                                 sizeof(valueStr),
                                 "| LE: 0x%llX | BE: 0x%llX",
                                 qToLittleEndian(*(uint64_t*)data.data()),
                                 qToBigEndian(*(uint64_t*)data.data()));
                        break;
                    default: valueStr[0] = 0; break;
                }

                snprintf(infoStr,
                         sizeof(infoStr),
                         "Cursor: %llu(0x%llX) | Selection: %llu byte%s [0x%llX - 0x%llX] %s",
                         cursorPos,
                         cursorPos,
                         selSize,
                         selSize == 1 ? "" : "s",
                         m_HexEditor->GetSelectionStart(),
                         m_HexEditor->GetSelectionEnd() - 1,
                         valueStr);
            } else {
                snprintf(infoStr, sizeof(infoStr), "Cursor: %llu(0x%llX)", cursorPos, cursorPos);
            }
            m_BottomLabel->setText(infoStr);
        });

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