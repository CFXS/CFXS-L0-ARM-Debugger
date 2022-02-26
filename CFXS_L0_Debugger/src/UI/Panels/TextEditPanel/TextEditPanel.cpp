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
#include "TextEditPanel.hpp"
#include "ui_TextEditPanel.h"
#include <Core/Project/ProjectManager.hpp>
#include <UI/Providers/FileIconProvider.hpp>
#include <QFileInfo>
#include <QDir>
#include <QPixmap>
#include <DockWidgetTab.h>
#include <QBoxLayout>
#include <QScrollBar>
#include <QPoint>
#include <QSyntaxStyle>

#include <QCXXHighlighter>
#include <QJSONHighlighter>
#include <QXMLHighlighter>
#include <QLDHighlighter>
#include <QCMakeHighlighter>
#include <QLuaHighlighter>
#include <QPythonHighlighter>

namespace L0::UI {

    //////////////////////////////////////////////
    static FileIconProvider* GetIconProvider() {
        static FileIconProvider fbip;
        return &fbip;
    };
    //////////////////////////////////////////////

    TextEditPanel::TextEditPanel() : ads::CDockWidget(GetPanelBaseName()), ui(std::make_unique<Ui::TextEditPanel>()) {
        LOG_UI_TRACE("Create {}", GetPanelBaseName());
        ui->setupUi(this);

        auto layout = new QBoxLayout(QBoxLayout::LeftToRight);
        layout->setContentsMargins({0, 0, 0, 0});
        layout->setSpacing(0);
        ui->root->setLayout(layout);

        m_Editor = new QCodeEditor(this);
        m_Editor->setSyntaxStyle(QSyntaxStyle::defaultStyle());
        m_Editor->setWordWrapMode(QTextOption::NoWrap);
        m_Editor->setObjectName("monospaceTextObject");

        layout->addWidget(m_Editor);

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
            LOG_UI_ERROR("TextEditPanel deleting panel: file does not exit - {}", filePath);
            deleteLater();
        }

        UpdateContent();
    }

    void TextEditPanel::UpdateContent() {
        m_File.open(QFile::ReadOnly | QFile::Text);
        QString lines = m_File.readAll();

        auto ext  = m_FileInfo.suffix().toLower();
        auto name = m_FileInfo.fileName().toLower();

        if (m_Editor->highlighter()) {
            delete m_Editor->highlighter();
            m_Editor->setHighlighter(nullptr);
        }

        if (ext == QStringLiteral("c") || ext == QStringLiteral("cc") || ext == QStringLiteral("cpp") || ext == QStringLiteral("cxx") ||
            ext == QStringLiteral("c++") || ext == QStringLiteral("h") || ext == QStringLiteral("hh") || ext == QStringLiteral("hpp") ||
            ext == QStringLiteral("hxx") || ext == QStringLiteral("h++")) {
            m_Editor->setHighlighter(new QCXXHighlighter);
        } else if (ext == QStringLiteral("json")) {
            m_Editor->setHighlighter(new QJSONHighlighter);
        } else if (ext == QStringLiteral("xml") || ext == QStringLiteral("emproject")) {
            m_Editor->setHighlighter(new QXMLHighlighter);
        } else if (ext == QStringLiteral("ld")) {
            m_Editor->setHighlighter(new QLDHighlighter);
        } else if (ext == QStringLiteral("cmake") || name == QStringLiteral("cmakelists.txt")) {
            m_Editor->setHighlighter(new QCMakeHighlighter);
        } else if (ext == QStringLiteral("lua")) {
            m_Editor->setHighlighter(new QLuaHighlighter);
        } else if (ext == QStringLiteral("py")) {
            m_Editor->setHighlighter(new QPythonHighlighter);
        }
        m_Editor->setPlainText(lines);

        m_File.close();
    }

    void TextEditPanel::SavePanelState(QSettings* cfg) {
        QString cfgKey = objectName();
        cfgKey.replace('/', '\\'); // QSettings does not like '/' in keys
        LOG_UI_TRACE("TextEditPanel save state - {}", cfgKey);

        cfg->beginGroup(cfgKey);
        cfg->setValue("version", 1);
        cfg->setValue("scroll_x", m_Editor->horizontalScrollBar()->value());
        cfg->setValue("scroll_y", m_Editor->verticalScrollBar()->value());
        cfg->setValue("sel_start", m_Editor->textCursor().selectionStart());
        cfg->setValue("sel_end", m_Editor->textCursor().selectionEnd());
        cfg->endGroup();
    }

    void TextEditPanel::LoadPanelState(QSettings* cfg) {
        QString cfgKey = objectName();
        cfgKey.replace('/', '\\'); // QSettings does not like '/'
        LOG_UI_TRACE("TextEditPanel load state - {}", cfgKey);

        if (!cfg->childGroups().contains(cfgKey)) {
            LOG_UI_WARN(" - No config entry for {}", cfgKey);
            return; // no cfg entry
        }

        cfg->beginGroup(cfgKey);
        auto version = cfg->value("version").toInt();
        if (version == 1) {
            auto scroll_x  = cfg->value("scroll_x").toInt();
            auto scroll_y  = cfg->value("scroll_y").toInt();
            auto sel_start = cfg->value("sel_start").toInt();
            auto sel_end   = cfg->value("sel_end").toInt();

            auto cursor = m_Editor->textCursor();
            cursor.setPosition(sel_start, QTextCursor::MoveAnchor);
            cursor.setPosition(sel_end, QTextCursor::KeepAnchor);
            m_Editor->setTextCursor(cursor);

            m_Editor->horizontalScrollBar()->setValue(scroll_x);
            m_Editor->verticalScrollBar()->setValue(scroll_y);
        } else {
            LOG_UI_ERROR(" - Unsupported TextEditPanel state data version {}", version);
        }
        cfg->endGroup();
    }

} // namespace L0::UI