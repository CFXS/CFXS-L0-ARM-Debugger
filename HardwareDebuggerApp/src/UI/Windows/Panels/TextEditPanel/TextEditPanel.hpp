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
#pragma once

#include <DockWidget.h>
#include <QFile>
#include <QFileInfo>
#include <QTextEdit>

namespace Ui {
    class TextEditPanel;
}

namespace HWD::UI {

    class TextEditPanel : public ads::CDockWidget {
    public:
        TextEditPanel();

        void SetFilePath(const QString& filePath);
        const QString& GetFilePath() const;

    private:
        void UpdateContent();

    private:
        std::unique_ptr<Ui::TextEditPanel> ui;
        QString m_FilePath;
        QFile m_File;
        QFileInfo m_FileInfo;
        QTextEdit* m_TextEdit;
        QTextEdit* m_LineWidget;
    };

} // namespace HWD::UI
