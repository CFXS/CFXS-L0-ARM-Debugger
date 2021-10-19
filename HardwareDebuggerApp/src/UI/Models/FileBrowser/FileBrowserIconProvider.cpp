// [CFXS] //
#include "FileBrowserIconProvider.hpp"

namespace HWD::UI {

    //////////////////////////////////////////////////////////////
    static std::vector<QPixmap> s_IconTable;
    static bool s_Initialized = false;

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

        // Closed Folders
        FOLDER,
        FOLDER_VSCODE,
        FOLDER_GIT,
        FOLDER_BUILD,
        FOLDER_SRC,
        FOLDER_LIB,
        FOLDER_RESOURCE,
        FOLDER_CFXS,
        // Open Folders
        FOLDER_OPEN,
        FOLDER_VSCODE_OPEN,
        FOLDER_GIT_OPEN,
        FOLDER_BUILD_OPEN,
        FOLDER_SRC_OPEN,
        FOLDER_LIB_OPEN,
        FOLDER_RESOURCE_OPEN,
        FOLDER_CFXS_OPEN,

        _COUNT
    };

    static constexpr auto ICON_SIZE = 16;
    //////////////////////////////////////////////////////////////

    FileBrowserIconProvider::FileBrowserIconProvider() {
        if (!s_Initialized) {
            s_Initialized = true;
            s_IconTable   = {
                QPixmap(":/Icon/file.png"),
                QPixmap(":/Icon/doc.png"),
                QPixmap(":/Icon/asm.png"),
                QPixmap(":/Icon/bin.png"),
                QPixmap(":/Icon/c.png"),
                QPixmap(":/Icon/cert.png"),
                QPixmap(":/Icon/cmake.png"),
                QPixmap(":/Icon/compressed.png"),
                QPixmap(":/Icon/cpp.png"),
                QPixmap(":/Icon/exe.png"),
                QPixmap(":/Icon/font.png"),
                QPixmap(":/Icon/gear.png"),
                QPixmap(":/Icon/git.png"),
                QPixmap(":/Icon/h.png"),
                QPixmap(":/Icon/hpp.png"),
                QPixmap(":/Icon/image.png"),
                QPixmap(":/Icon/json.png"),
                QPixmap(":/Icon/lib.png"),
                QPixmap(":/Icon/lua.png"),
                QPixmap(":/Icon/pdf.png"),
                QPixmap(":/Icon/py.png"),
                QPixmap(":/Icon/xml.png"),
                QPixmap(":/Icon/info.png"),
                QPixmap(":/Icon/log.png"),
                QPixmap(":/Icon/cfxs.png"),
                QPixmap(":/Icon/segger.png"),
                QPixmap(":/Icon/link.png"),

                QPixmap(":/Icon/folder.png"),
                QPixmap(":/Icon/folder-vscode.png"),
                QPixmap(":/Icon/folder-git.png"),
                QPixmap(":/Icon/folder-build.png"),
                QPixmap(":/Icon/folder-src.png"),
                QPixmap(":/Icon/folder-lib.png"),
                QPixmap(":/Icon/folder-resource.png"),
                QPixmap(":/Icon/folder-cfxs.png"),

                QPixmap(":/Icon/folder-open.png"),
                QPixmap(":/Icon/folder-vscode-open.png"),
                QPixmap(":/Icon/folder-git-open.png"),
                QPixmap(":/Icon/folder-build-open.png"),
                QPixmap(":/Icon/folder-src-open.png"),
                QPixmap(":/Icon/folder-lib-open.png"),
                QPixmap(":/Icon/folder-resource-open.png"),
                QPixmap(":/Icon/folder-cfxs-open.png"),
            };

            if (s_IconTable.size() != _COUNT) {
                HWDLOG_CORE_CRITICAL("Table does not match enum");
                HWD_DEBUGBREAK();
            }
        }
    }

    QIcon FileBrowserIconProvider::icon(IconType type) const {
        switch (type) {
            case IconType::Folder: return s_IconTable[Icon::FOLDER];
            case IconType::File: return s_IconTable[Icon::FILE];
            default: return s_IconTable[Icon::FOLDER];
        }
    }

    QIcon FileBrowserIconProvider::icon(const QFileInfo& info, bool isExpanded) const {
        QString suffix = info.suffix().toLower();
        QString name   = info.baseName().toLower();

        if (info.isDir()) {
            if (name == "src" || name == "source") {
                return s_IconTable[isExpanded ? Icon::FOLDER_SRC_OPEN : Icon::FOLDER_SRC];
            } else if (name == "vendor" || name == "lib" || name == "library" || name == "extern") {
                return s_IconTable[isExpanded ? Icon::FOLDER_LIB_OPEN : Icon::FOLDER_LIB];
            } else if (name == "res" || name == "resource" || name == "resources") {
                return s_IconTable[isExpanded ? Icon::FOLDER_RESOURCE_OPEN : Icon::FOLDER_RESOURCE];
            } else if (suffix == "vscode") {
                return s_IconTable[isExpanded ? Icon::FOLDER_VSCODE_OPEN : Icon::FOLDER_VSCODE];
            } else if (name == "build" || name == "out") {
                return s_IconTable[isExpanded ? Icon::FOLDER_BUILD_OPEN : Icon::FOLDER_BUILD];
            } else if (suffix == "git") {
                return s_IconTable[isExpanded ? Icon::FOLDER_GIT_OPEN : Icon::FOLDER_GIT];
            } else if (suffix == "cfxs" || suffix == "hwd" || suffix == "cfxs_hwd") {
                return s_IconTable[isExpanded ? Icon::FOLDER_CFXS_OPEN : Icon::FOLDER_CFXS];
            } else {
                return s_IconTable[isExpanded ? Icon::FOLDER_OPEN : Icon::FOLDER];
            }
        } else {
            if (name == "license") {
                return s_IconTable[Icon::CERT];
            }

            if (suffix.contains("cfxs")) {
                return s_IconTable[Icon::CFXS];
            }

            if (suffix == "txt") {
                if (name == "cmakelists" || name == "cmakecache") {
                    return s_IconTable[Icon::CMAKE];
                } else {
                    return s_IconTable[Icon::DOC];
                }
            } else if (suffix == "c") {
                return s_IconTable[Icon::C];
            } else if (suffix == "cpp" || suffix == "cc" || suffix == "cxx" || suffix == "c++") {
                return s_IconTable[Icon::CPP];
            } else if (suffix == "h") {
                return s_IconTable[Icon::H];
            } else if (suffix == "hpp" || suffix == "hh" || suffix == "hxx" || suffix == "h++") {
                return s_IconTable[Icon::HPP];
            } else if (suffix == "cmake") {
                return s_IconTable[Icon::CMAKE];
            } else if (suffix == "s" || suffix == "asm" || suffix == "o" || suffix == "o") {
                return s_IconTable[Icon::ASM];
            } else if (suffix == "hex" || suffix == "bin" || suffix == "raw") {
                return s_IconTable[Icon::BIN];
            } else if (suffix == "lhg") {
                return s_IconTable[Icon::CERT];
            } else if (suffix == "zip" || suffix == "rar" || suffix == "gz" || suffix == "tar" || suffix == "7z") {
                return s_IconTable[Icon::COMPRESSED];
            } else if (suffix == "out" || suffix == "elf") {
                return s_IconTable[Icon::EXE];
            } else if (suffix == "ld" || suffix == "icf") {
                return s_IconTable[Icon::LINK];
            } else if (suffix == "fnt") {
                return s_IconTable[Icon::FONT];
            } else if (suffix == "cfg" || suffix == "config" || suffix == "clang-format" || suffix == "clang-tidy") {
                return s_IconTable[Icon::GEAR];
            } else if (suffix == "git" || suffix == "gitmodules" || suffix == "gitignore" || suffix == "gitattributes") {
                return s_IconTable[Icon::GIT];
            } else if (suffix == "png" || suffix == "jpg" || suffix == "jpeg" || suffix == "bmp" || suffix == "webm" || suffix == "gif") {
                return s_IconTable[Icon::IMAGE];
            } else if (suffix == "md") {
                return s_IconTable[Icon::INFO];
            } else if (suffix == "json") {
                return s_IconTable[Icon::JSON];
            } else if (suffix == "a" || suffix == "lib" || suffix == "dll") {
                return s_IconTable[Icon::LIB];
            } else if (suffix == "log") {
                return s_IconTable[Icon::LOG];
            } else if (suffix == "lua") {
                return s_IconTable[Icon::LUA];
            } else if (suffix == "pdf") {
                return s_IconTable[Icon::PDF];
            } else if (suffix == "py") {
                return s_IconTable[Icon::PY];
            } else if (suffix == "xml" || suffix == "htm" || suffix == "html" || suffix == "qrc") {
                return s_IconTable[Icon::XML];
            } else if (suffix == "emproject" || suffix == "emsession" || suffix == "jlink") {
                return s_IconTable[Icon::SEGGER];
            }

            return s_IconTable[Icon::FILE];
        }
    }

} // namespace HWD::UI