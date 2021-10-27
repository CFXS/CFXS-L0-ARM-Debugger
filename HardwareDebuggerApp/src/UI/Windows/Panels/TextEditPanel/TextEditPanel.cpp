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
#include "TextEditPanel.hpp"
#include "ui_TextEditPanel.h"
#include <Core/Project/ProjectManager.hpp>
#include <UI/Models/FileBrowser/FileBrowserIconProvider.hpp>
#include <QFileInfo>
#include <QDir>
#include <QPixmap>
#include <DockWidgetTab.h>
#include <QBoxLayout>
#include <QScrollBar>

namespace HWD::UI {

    //////////////////////////////////////////////
    static FileBrowserIconProvider* GetIconProvider() {
        static FileBrowserIconProvider fbip;
        return &fbip;
    };
    //////////////////////////////////////////////

    TextEditPanel::TextEditPanel() : ads::CDockWidget(QStringLiteral("TextEditPanel")), ui(std::make_unique<Ui::TextEditPanel>()) {
        HWDLOG_CORE_TRACE("Create text edit panel");
        ui->setupUi(this);

        auto layout = new QBoxLayout(QBoxLayout::LeftToRight);
        layout->setContentsMargins({0, 0, 0, 0});
        layout->setSpacing(1);
        ui->root->setLayout(layout);

        m_TextEdit = new QTextEdit;
        m_TextEdit->setTextBackgroundColor(QColor{0, 0, 0, 0});
        m_TextEdit->setTextColor(QColor{255, 255, 255});
        m_TextEdit->setStyleSheet("background-color: rgb(28, 28, 28);");
        m_TextEdit->setLineWrapMode(QTextEdit::NoWrap);
        m_TextEdit->setWordWrapMode(QTextOption::NoWrap);
        m_TextEdit->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

        m_LineWidget = new QTextEdit;
        m_LineWidget->setTextBackgroundColor(QColor{0, 0, 0, 0});
        m_LineWidget->setTextColor(QColor{128, 128, 128});
        m_LineWidget->setStyleSheet("background-color: rgb(28, 28, 28);");
        m_LineWidget->setLineWrapMode(QTextEdit::NoWrap);
        m_LineWidget->setWordWrapMode(QTextOption::NoWrap);
        m_LineWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        m_LineWidget->setFixedWidth(58);
        m_LineWidget->verticalScrollBar()->setVisible(false);
        m_LineWidget->horizontalScrollBar()->setVisible(false);
        m_LineWidget->setReadOnly(true);

        m_TextEdit->setObjectName("codeField");
        m_LineWidget->setObjectName("codeField");

        layout->addWidget(m_LineWidget);
        layout->addWidget(m_TextEdit);

        // Link scrollbars
        connect(m_TextEdit->verticalScrollBar(), SIGNAL(valueChanged(int)), m_LineWidget->verticalScrollBar(), SLOT(setValue(int)));
        connect(m_LineWidget->verticalScrollBar(), SIGNAL(valueChanged(int)), m_TextEdit->verticalScrollBar(), SLOT(setValue(int)));

        setWidget(ui->root);
    }

    const QString& TextEditPanel::GetFilePath() const {
        return m_FilePath;
    }

    void TextEditPanel::SetFilePath(const QString& filePath) {
        QString relPath = QDir(ProjectManager::GetWorkspacePath()).relativeFilePath(filePath);

        if (relPath != filePath) {
            setObjectName(QStringLiteral("TextEditPanel|./") + relPath);
        } else {
            setObjectName(QStringLiteral("TextEditPanel|") + filePath);
        }

        setProperty("absoluteFilePath", filePath);

        if (m_File.isOpen()) {
            m_File.close();
        }

        m_File.setFileName(filePath);

        if (m_File.exists()) {
            m_FileInfo.setFile(m_File);

            setWindowTitle(m_FileInfo.fileName());

            // TODO: low res on CFXS dev left monitor (move 4k to 1080)
            auto icon = GetIconProvider()->icon(m_FileInfo);
            setIcon(icon);

            if (relPath != filePath) {
                tabWidget()->setToolTip(QStringLiteral("./") + relPath);
            } else {
                tabWidget()->setToolTip(filePath);
            }
        } else {
            HWDLOG_UI_ERROR("TextEditPanel deleting panel: file does not exit - {}", filePath);
            deleteLater();
        }

        UpdateContent();
    }

    void TextEditPanel::UpdateContent() {
        m_File.open(QFile::ReadOnly | QFile::Text);
        QString lines = m_File.readAll();

        auto lineCount  = lines.count('\n');
        QString lineBar = "";
        for (int i = 0; i <= lineCount; i++) {
            if (i < 9) {
                lineBar = lineBar + "   ";
            } else if (i < 99) {
                lineBar = lineBar + "  ";
            } else {
                lineBar = lineBar + " ";
            }

            if (i < lineCount) {
                lineBar = lineBar + QString::number(i + 1) + "\n";
            } else {
                lineBar = lineBar + QString::number(i + 1);
            }
        }

        m_LineWidget->setText(lineBar);
        m_TextEdit->setText(lines);
        m_File.close();
    }

} // namespace HWD::UI