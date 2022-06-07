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
#include "../TextEditPanel.hpp"
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
#include <QMenu>
#include <QAction>
#include <QShortcut>

#include <QCXXHighlighter>
#include <QJSONHighlighter>
#include <QXMLHighlighter>
#include <QLDHighlighter>
#include <QCMakeHighlighter>
#include <QLuaHighlighter>
#include <QPythonHighlighter>

#include <iostream>
//#include <clang-c/Index.h>
//#include <clang/Lex/Lexer.h>
//#include <clang/Format/Format.h>

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
        m_Editor->setAutoParentheses(false);

        layout->addWidget(m_Editor);

        QShortcut* shortcut = new QShortcut(QKeySequence("Ctrl+W"), m_Editor);
        QObject::connect(shortcut, &QShortcut::activated, [this]() {
            if (m_File.isOpen())
                m_File.close();
            m_File.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text);
            m_File.write(QByteArray(m_Editor->toPlainText().toStdString().c_str()));
            m_File.close();
        });

        setWidget(ui->root);
    }

    const QString& TextEditPanel::GetFilePath() const {
        return m_FilePath;
    }

    void TextEditPanel::SetFilePath(const QString& filePath) {
        QString relPath = QDir(ProjectManager::GetWorkspacePath()).relativeFilePath(filePath);

        if (relPath != filePath) {
            setObjectName(QSL("TextEditPanel|./") + relPath);
        } else {
            setObjectName(QSL("TextEditPanel|") + filePath);
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
                tabWidget()->setToolTip(QSL("./") + relPath);
            } else {
                tabWidget()->setToolTip(filePath);
            }
        } else {
            LOG_UI_ERROR("TextEditPanel deleting panel: file does not exit - {}", filePath);
            deleteLater();
        }

        UpdateContent();
    }

    //std::string TakeTempString(CXString str) {
    //    auto cs = clang_getCString(str);
    //    if (cs) {
    //        std::string s{cs};
    //        clang_disposeString(str);
    //        return s;
    //    } else {
    //        return "???";
    //    }
    //}

    void TextEditPanel::UpdateContent() {
        m_File.open(QFile::ReadOnly | QFile::Text);
        QString lines = m_File.readAll();

        auto ext  = m_FileInfo.suffix().toLower();
        auto name = m_FileInfo.fileName().toLower();

        if (m_Editor->highlighter()) {
            delete m_Editor->highlighter();
            m_Editor->setHighlighter(nullptr);
        }

        if (ext == QSL("c") || ext == QSL("cc") || ext == QSL("cpp") || ext == QSL("cxx") || ext == QSL("c++") || ext == QSL("h") ||
            ext == QSL("hh") || ext == QSL("hpp") || ext == QSL("hxx") || ext == QSL("h++")) {
            m_Editor->setHighlighter(new QCXXHighlighter);
        } else if (ext == QSL("json")) {
            m_Editor->setHighlighter(new QJSONHighlighter);
        } else if (ext == QSL("xml") || ext == QSL("emproject")) {
            m_Editor->setHighlighter(new QXMLHighlighter);
        } else if (ext == QSL("ld")) {
            m_Editor->setHighlighter(new QLDHighlighter);
        } else if (ext == QSL("cmake") || name == QSL("cmakelists.txt")) {
            m_Editor->setHighlighter(new QCMakeHighlighter);
        } else if (ext == QSL("lua")) {
            m_Editor->setHighlighter(new QLuaHighlighter);
        } else if (ext == QSL("py")) {
            m_Editor->setHighlighter(new QPythonHighlighter);
        }

        m_Editor->setPlainText(lines);

        //using std::chrono::duration;
        //using std::chrono::duration_cast;
        //using std::chrono::high_resolution_clock;
        //using std::chrono::milliseconds;
        //
        //auto t1                = high_resolution_clock::now();
        //CXIndex index          = clang_createIndex(0, 0);
        //CXTranslationUnit unit = clang_parseTranslationUnit(
        //    index, m_FileInfo.absoluteFilePath().toStdString().c_str(), nullptr, 0, nullptr, 0, CXTranslationUnit_None);
        //auto t2 = high_resolution_clock::now();
        //
        //if (!unit) {
        //    LOG_CORE_ERROR("Parse failed");
        //} else {
        //    auto ms_int                            = duration_cast<milliseconds>(t2 - t1);
        //    duration<double, std::milli> ms_double = t2 - t1;
        //
        //    LOG_CORE_INFO("File parsed in {:.3f}ms", ms_double.count());
        //
        //    CXCursor cursor = clang_getTranslationUnitCursor(unit);
        //    clang_visitChildren(
        //        cursor,
        //        [](CXCursor c, CXCursor parent, CXClientData client_data) {
        //            CXFile file;
        //            uint32_t line, col, offset;
        //            clang_getSpellingLocation(clang_getCursorLocation(c), &file, &line, &col, &offset);
        //            auto fname = TakeTempString(clang_getFileName(file));
        //            if (fname == ((QFileInfo*)client_data)->absoluteFilePath().toStdString()) {
        //                LOG_CORE_TRACE("Cursor {}:{}:{}.{} {} {}",
        //                               fname,
        //                               line,
        //                               col,
        //                               offset,
        //                               TakeTempString(clang_getCursorSpelling(c)),
        //                               TakeTempString(clang_getCursorKindSpelling(clang_getCursorKind(c))));
        //            }
        //            return CXChildVisit_Continue;
        //        },
        //        &m_FileInfo);
        //
        //    clang_disposeTranslationUnit(unit);
        //    clang_disposeIndex(index);
        //    unit  = nullptr;
        //    index = nullptr;
        //}
        //
        //if (unit)
        //    clang_disposeTranslationUnit(unit);
        //if (index)
        //    clang_disposeIndex(index);

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