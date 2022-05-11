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

#include <Core/Project/ProjectManager.hpp>
#include <QHexEdit2/QHexEdit.hpp>
#include <QBoxLayout>
#include <QDir>
#include <QFileInfo>
#include <QPixmap>
#include <QPoint>
#include <QScrollBar>
#include <QTimer>

#include "ui_TargetBinaryPanel.h"

#include <Core/ELF/ELF_Reader.hpp>

namespace L0::ELF {
    extern ELF_Reader* g_Test_ELF_Reader;
}
using L0::ELF::g_Test_ELF_Reader;

namespace L0::UI {

    TargetBinaryPanel::TargetBinaryPanel() : ads::CDockWidget(GetPanelBaseName()), ui(std::make_unique<Ui::TargetBinaryPanel>()) {
        LOG_UI_TRACE("Create {}", GetPanelBaseName());
        ui->setupUi(this);
        setWindowTitle("Target Binary");

        m_HexEditor = new QHexEdit;
        ui->content->layout()->addWidget(m_HexEditor);

        ui->searchTextBar->setPlaceholderText("Jump To Address...");
        ui->searchTextBar->setStyleSheet("QLineEdit{color: gray;}");
        ui->searchTextBar->setObjectName("monospaceTextObject");
        connect(ui->searchTextBar, qOverload<const QString&>(&QLineEdit::textChanged), [=](const QString& text) {
            if (text.isEmpty()) {
                ui->searchTextBar->setStyleSheet("QLineEdit{color: gray;}");
            } else {
                ui->searchTextBar->setStyleSheet("QLineEdit{color: white;}");

                if (g_Test_ELF_Reader && text == QSL("e")) {
                    m_HexEditor->setData(g_Test_ELF_Reader->GetRawFileData());
                } else if (g_Test_ELF_Reader && text == QSL("b")) {
                    m_HexEditor->setData(g_Test_ELF_Reader->GetTargetBinary());
                } else {
                    m_HexEditor->setData(QByteArray{});
                }
            }
        });

        m_HexEditor->setObjectName("monospaceTextObject");
        m_HexEditor->setHexFontColor(QColor{220, 220, 220});
        m_HexEditor->setAddressFontColor(QColor{160, 160, 160});
        m_HexEditor->setAsciiFontColor(QColor{240, 240, 240});
        m_HexEditor->setAsciiAreaColor(QColor{22, 22, 22});
        m_HexEditor->setAddressAreaColor(QColor{77, 77, 77});
        m_HexEditor->setStyleSheet("QFrame{background-color: rgb(22, 22, 22);}");
        m_HexEditor->setBytesPerLine(16);
        m_HexEditor->setHexCaps(true);
        m_HexEditor->setReadOnly(true);
        m_HexEditor->SetFontSize(14);

        setWidget(ui->root);
    }

    void TargetBinaryPanel::SavePanelState(QSettings* cfg) {
    }

    void TargetBinaryPanel::LoadPanelState(QSettings* cfg) {
    }

} // namespace L0::UI