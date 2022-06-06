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
#pragma once
#include <QFileIconProvider>
#include <QTreeView>

namespace L0::UI {

    class FileIconProvider : public QFileIconProvider {
    public:
        enum Icon : int {
            FILE,
            DOC,
            ASM,
            BIN,
            C,
            CERT,
            CMAKE,
            COMPRESSED,
            CPP,
            EXE,
            FONT,
            GEAR,
            GIT,
            H,
            HPP,
            IMAGE,
            JSON,
            LIB,
            LUA,
            PDF,
            PY,
            XML,
            INFO,
            LOG,
            CFXS,
            SEGGER,
            LINK,
            L0,
            HEX,

            // Closed Folders
            FOLDER,
            FOLDER_VSCODE,
            FOLDER_GIT,
            FOLDER_BUILD,
            FOLDER_SRC,
            FOLDER_LIB,
            FOLDER_RESOURCE,
            FOLDER_CFXS,
            FOLDER_L0,
            // Open Folders
            FOLDER_OPEN,
            FOLDER_VSCODE_OPEN,
            FOLDER_GIT_OPEN,
            FOLDER_BUILD_OPEN,
            FOLDER_SRC_OPEN,
            FOLDER_LIB_OPEN,
            FOLDER_RESOURCE_OPEN,
            FOLDER_CFXS_OPEN,
            FOLDER_L0_OPEN,

            _COUNT
        };

    public:
        FileIconProvider();

        QIcon icon(IconType type) const override;
        QIcon icon(const QFileInfo& info, bool isExpanded) const;
        QIcon icon(const QFileInfo& info) const override {
            return icon(info, false);
        }

        QIcon icon(Icon icon) const;
    };

} // namespace L0::UI