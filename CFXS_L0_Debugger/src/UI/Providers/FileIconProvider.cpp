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
#include "FileIconProvider.hpp"

namespace L0::UI {

    //////////////////////////////////////////////////////////////
    static std::vector<QPixmap> s_IconTable;
    static bool s_Initialized = false;

    static constexpr auto ICON_SIZE = 16;
    //////////////////////////////////////////////////////////////

    FileIconProvider::FileIconProvider() {
        if (!s_Initialized) {
            s_Initialized = true;
            s_IconTable   = {
                // Files
                QPixmap(QSL(":/Icon/file")),
                QPixmap(QSL(":/Icon/doc")),
                QPixmap(QSL(":/Icon/asm")),
                QPixmap(QSL(":/Icon/bin")),
                QPixmap(QSL(":/Icon/c")),
                QPixmap(QSL(":/Icon/cert")),
                QPixmap(QSL(":/Icon/cmake")),
                QPixmap(QSL(":/Icon/compressed")),
                QPixmap(QSL(":/Icon/cpp")),
                QPixmap(QSL(":/Icon/exe")),
                QPixmap(QSL(":/Icon/font")),
                QPixmap(QSL(":/Icon/gear")),
                QPixmap(QSL(":/Icon/git")),
                QPixmap(QSL(":/Icon/h")),
                QPixmap(QSL(":/Icon/hpp")),
                QPixmap(QSL(":/Icon/image")),
                QPixmap(QSL(":/Icon/json")),
                QPixmap(QSL(":/Icon/lib")),
                QPixmap(QSL(":/Icon/lua")),
                QPixmap(QSL(":/Icon/pdf")),
                QPixmap(QSL(":/Icon/py")),
                QPixmap(QSL(":/Icon/xml")),
                QPixmap(QSL(":/Icon/info")),
                QPixmap(QSL(":/Icon/log")),
                QPixmap(QSL(":/Icon/cfxs")),
                QPixmap(QSL(":/Icon/segger")),
                QPixmap(QSL(":/Icon/link")),
                QPixmap(QSL(":/Icon/l0")),
                QPixmap(QSL(":/Icon/hex")),

                // Closed folders
                QPixmap(QSL(":/Icon/folder")),
                QPixmap(QSL(":/Icon/folder-vscode")),
                QPixmap(QSL(":/Icon/folder-git")),
                QPixmap(QSL(":/Icon/folder-build")),
                QPixmap(QSL(":/Icon/folder-src")),
                QPixmap(QSL(":/Icon/folder-lib")),
                QPixmap(QSL(":/Icon/folder-resource")),
                QPixmap(QSL(":/Icon/folder-cfxs")),
                QPixmap(QSL(":/Icon/folder-l0")),

                // Open folders
                QPixmap(QSL(":/Icon/folder-open")),
                QPixmap(QSL(":/Icon/folder-vscode-open")),
                QPixmap(QSL(":/Icon/folder-git-open")),
                QPixmap(QSL(":/Icon/folder-build-open")),
                QPixmap(QSL(":/Icon/folder-src-open")),
                QPixmap(QSL(":/Icon/folder-lib-open")),
                QPixmap(QSL(":/Icon/folder-resource-open")),
                QPixmap(QSL(":/Icon/folder-cfxs-open")),
                QPixmap(QSL(":/Icon/folder-l0-open")),
            };

            if (s_IconTable.size() != _COUNT) {
                LOG_CORE_CRITICAL("Table does not match enum");
                L0_DEBUGBREAK();
            }
        }
    }

    QIcon FileIconProvider::icon(IconType type) const {
        switch (type) {
            case IconType::Folder: return s_IconTable[Icon::FOLDER];
            case IconType::File: return s_IconTable[Icon::FILE];
            default: return s_IconTable[Icon::FOLDER];
        }
    }

    QIcon FileIconProvider::icon(Icon icon) const {
        return s_IconTable[icon];
    }

    QIcon FileIconProvider::icon(const QFileInfo& info, bool isExpanded) const {
        QString suffix = info.suffix().toLower();
        QString name   = info.baseName().toLower();

        if (info.isDir()) {
            if (name == QSL("src") || name == QSL("source")) {
                return s_IconTable[isExpanded ? Icon::FOLDER_SRC_OPEN : Icon::FOLDER_SRC];
            } else if (name == QSL("vendor") || name == QSL("lib") || name == QSL("library") || name == QSL("extern")) {
                return s_IconTable[isExpanded ? Icon::FOLDER_LIB_OPEN : Icon::FOLDER_LIB];
            } else if (name == QSL("res") || name == QSL("resource") || name == QSL("resources")) {
                return s_IconTable[isExpanded ? Icon::FOLDER_RESOURCE_OPEN : Icon::FOLDER_RESOURCE];
            } else if (suffix == QSL("vscode")) {
                return s_IconTable[isExpanded ? Icon::FOLDER_VSCODE_OPEN : Icon::FOLDER_VSCODE];
            } else if (name == QSL("build") || name == QSL("out")) {
                return s_IconTable[isExpanded ? Icon::FOLDER_BUILD_OPEN : Icon::FOLDER_BUILD];
            } else if (suffix == QSL("git")) {
                return s_IconTable[isExpanded ? Icon::FOLDER_GIT_OPEN : Icon::FOLDER_GIT];
            } else if (suffix == QSL("cfxs")) {
                return s_IconTable[isExpanded ? Icon::FOLDER_CFXS_OPEN : Icon::FOLDER_CFXS];
            } else if (name == QSL("l0") && info.absolutePath().toLower().contains(QSL(".cfxs"))) {
                return s_IconTable[isExpanded ? Icon::FOLDER_L0_OPEN : Icon::FOLDER_L0];
            } else {
                return s_IconTable[isExpanded ? Icon::FOLDER_OPEN : Icon::FOLDER];
            }
        } else {
            if (name == QSL("license")) {
                return s_IconTable[Icon::CERT];
            }

            if (suffix.contains(QSL("cfxs"))) {
                return s_IconTable[Icon::CFXS];
            } else if (suffix.contains(QSL("l0"))) {
                return s_IconTable[Icon::L0];
            }

            if (suffix == QSL("txt")) {
                if (name == QSL("cmakelists") || name == QSL("cmakecache")) {
                    return s_IconTable[Icon::CMAKE];
                } else {
                    return s_IconTable[Icon::DOC];
                }
            } else if (suffix == QSL("c")) {
                return s_IconTable[Icon::C];
            } else if (suffix == QSL("cpp") || suffix == QSL("cc") || suffix == QSL("cxx") || suffix == QSL("c++")) {
                return s_IconTable[Icon::CPP];
            } else if (suffix == QSL("h")) {
                return s_IconTable[Icon::H];
            } else if (suffix == QSL("hpp") || suffix == QSL("hh") || suffix == QSL("hxx") || suffix == QSL("h++")) {
                return s_IconTable[Icon::HPP];
            } else if (suffix == QSL("cmake")) {
                return s_IconTable[Icon::CMAKE];
            } else if (suffix == QSL("s") || suffix == QSL("asm") || suffix == QSL("a") || suffix == QSL("o") || suffix == QSL("raw")) {
                return s_IconTable[Icon::ASM];
            } else if (suffix == QSL("hex")) {
                return s_IconTable[Icon::HEX];
            } else if (suffix == QSL("bin")) {
                return s_IconTable[Icon::BIN];
            } else if (suffix == QSL("lhg")) {
                return s_IconTable[Icon::CERT];
            } else if (suffix == QSL("zip") || suffix == QSL("rar") || suffix == QSL("gz") || suffix == QSL("tar") || suffix == QSL("7z")) {
                return s_IconTable[Icon::COMPRESSED];
            } else if (suffix == QSL("out") || suffix == QSL("elf")) {
                return s_IconTable[Icon::EXE];
            } else if (suffix == QSL("ld") || suffix == QSL("icf")) {
                return s_IconTable[Icon::LINK];
            } else if (suffix == QSL("fnt")) {
                return s_IconTable[Icon::FONT];
            } else if (suffix == QSL("cfg") || suffix == QSL("config") || suffix == QSL("clang-format") || suffix == QSL("clang-tidy")) {
                return s_IconTable[Icon::GEAR];
            } else if (suffix == QSL("git") || suffix == QSL("gitmodules") || suffix == QSL("gitignore") ||
                       suffix == QSL("gitattributes")) {
                return s_IconTable[Icon::GIT];
            } else if (suffix == QSL("png") || suffix == QSL("jpg") || suffix == QSL("jpeg") || suffix == QSL("bmp") ||
                       suffix == QSL("webm") || suffix == QSL("gif")) {
                return s_IconTable[Icon::IMAGE];
            } else if (suffix == QSL("md")) {
                return s_IconTable[Icon::INFO];
            } else if (suffix == QSL("json")) {
                return s_IconTable[Icon::JSON];
            } else if (suffix == QSL("a") || suffix == QSL("lib") || suffix == QSL("dll")) {
                return s_IconTable[Icon::LIB];
            } else if (suffix == QSL("log")) {
                return s_IconTable[Icon::LOG];
            } else if (suffix == QSL("lua")) {
                return s_IconTable[Icon::LUA];
            } else if (suffix == QSL("pdf")) {
                return s_IconTable[Icon::PDF];
            } else if (suffix == QSL("py")) {
                return s_IconTable[Icon::PY];
            } else if (suffix == QSL("xml") || suffix == QSL("htm") || suffix == QSL("html") || suffix == QSL("qrc")) {
                return s_IconTable[Icon::XML];
            } else if (suffix == QSL("emproject") || suffix == QSL("emsession") || suffix == QSL("jlink")) {
                return s_IconTable[Icon::SEGGER];
            }

            return s_IconTable[Icon::FILE];
        }
    }

} // namespace L0::UI