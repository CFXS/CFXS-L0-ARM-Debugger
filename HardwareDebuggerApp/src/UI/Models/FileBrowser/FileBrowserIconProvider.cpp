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
        HWD,

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
                // Files
                QPixmap(QStringLiteral(":/Icon/file.png")),
                QPixmap(QStringLiteral(":/Icon/doc.png")),
                QPixmap(QStringLiteral(":/Icon/asm.png")),
                QPixmap(QStringLiteral(":/Icon/bin.png")),
                QPixmap(QStringLiteral(":/Icon/c.png")),
                QPixmap(QStringLiteral(":/Icon/cert.png")),
                QPixmap(QStringLiteral(":/Icon/cmake.png")),
                QPixmap(QStringLiteral(":/Icon/compressed.png")),
                QPixmap(QStringLiteral(":/Icon/cpp.png")),
                QPixmap(QStringLiteral(":/Icon/exe.png")),
                QPixmap(QStringLiteral(":/Icon/font.png")),
                QPixmap(QStringLiteral(":/Icon/gear.png")),
                QPixmap(QStringLiteral(":/Icon/git.png")),
                QPixmap(QStringLiteral(":/Icon/h.png")),
                QPixmap(QStringLiteral(":/Icon/hpp.png")),
                QPixmap(QStringLiteral(":/Icon/image.png")),
                QPixmap(QStringLiteral(":/Icon/json.png")),
                QPixmap(QStringLiteral(":/Icon/lib.png")),
                QPixmap(QStringLiteral(":/Icon/lua.png")),
                QPixmap(QStringLiteral(":/Icon/pdf.png")),
                QPixmap(QStringLiteral(":/Icon/py.png")),
                QPixmap(QStringLiteral(":/Icon/xml.png")),
                QPixmap(QStringLiteral(":/Icon/info.png")),
                QPixmap(QStringLiteral(":/Icon/log.png")),
                QPixmap(QStringLiteral(":/Icon/cfxs.png")),
                QPixmap(QStringLiteral(":/Icon/segger.png")),
                QPixmap(QStringLiteral(":/Icon/link.png")),
                QPixmap(QStringLiteral(":/Icon/hwd.png")),

                // Closed folders
                QPixmap(QStringLiteral(":/Icon/folder.png")),
                QPixmap(QStringLiteral(":/Icon/folder-vscode.png")),
                QPixmap(QStringLiteral(":/Icon/folder-git.png")),
                QPixmap(QStringLiteral(":/Icon/folder-build.png")),
                QPixmap(QStringLiteral(":/Icon/folder-src.png")),
                QPixmap(QStringLiteral(":/Icon/folder-lib.png")),
                QPixmap(QStringLiteral(":/Icon/folder-resource.png")),
                QPixmap(QStringLiteral(":/Icon/folder-cfxs.png")),

                // Open folders
                QPixmap(QStringLiteral(":/Icon/folder-open.png")),
                QPixmap(QStringLiteral(":/Icon/folder-vscode-open.png")),
                QPixmap(QStringLiteral(":/Icon/folder-git-open.png")),
                QPixmap(QStringLiteral(":/Icon/folder-build-open.png")),
                QPixmap(QStringLiteral(":/Icon/folder-src-open.png")),
                QPixmap(QStringLiteral(":/Icon/folder-lib-open.png")),
                QPixmap(QStringLiteral(":/Icon/folder-resource-open.png")),
                QPixmap(QStringLiteral(":/Icon/folder-cfxs-open.png")),
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
            if (name == QStringLiteral("src") || name == QStringLiteral("source")) {
                return s_IconTable[isExpanded ? Icon::FOLDER_SRC_OPEN : Icon::FOLDER_SRC];
            } else if (name == QStringLiteral("vendor") || name == QStringLiteral("lib") || name == QStringLiteral("library") ||
                       name == QStringLiteral("extern")) {
                return s_IconTable[isExpanded ? Icon::FOLDER_LIB_OPEN : Icon::FOLDER_LIB];
            } else if (name == QStringLiteral("res") || name == QStringLiteral("resource") || name == QStringLiteral("resources")) {
                return s_IconTable[isExpanded ? Icon::FOLDER_RESOURCE_OPEN : Icon::FOLDER_RESOURCE];
            } else if (suffix == QStringLiteral("vscode")) {
                return s_IconTable[isExpanded ? Icon::FOLDER_VSCODE_OPEN : Icon::FOLDER_VSCODE];
            } else if (name == QStringLiteral("build") || name == QStringLiteral("out")) {
                return s_IconTable[isExpanded ? Icon::FOLDER_BUILD_OPEN : Icon::FOLDER_BUILD];
            } else if (suffix == QStringLiteral("git")) {
                return s_IconTable[isExpanded ? Icon::FOLDER_GIT_OPEN : Icon::FOLDER_GIT];
            } else if (suffix == QStringLiteral("cfxs") || suffix == QStringLiteral("hwd") || suffix == QStringLiteral("cfxs_hwd")) {
                return s_IconTable[isExpanded ? Icon::FOLDER_CFXS_OPEN : Icon::FOLDER_CFXS];
            } else {
                return s_IconTable[isExpanded ? Icon::FOLDER_OPEN : Icon::FOLDER];
            }
        } else {
            if (name == QStringLiteral("license")) {
                return s_IconTable[Icon::CERT];
            }

            if (suffix.contains(QStringLiteral("cfxs"))) {
                return s_IconTable[Icon::CFXS];
            } else if (suffix.contains(QStringLiteral("hwd"))) {
                return s_IconTable[Icon::HWD];
            }

            if (suffix == QStringLiteral("txt")) {
                if (name == QStringLiteral("cmakelists") || name == QStringLiteral("cmakecache")) {
                    return s_IconTable[Icon::CMAKE];
                } else {
                    return s_IconTable[Icon::DOC];
                }
            } else if (suffix == QStringLiteral("c")) {
                return s_IconTable[Icon::C];
            } else if (suffix == QStringLiteral("cpp") || suffix == QStringLiteral("cc") || suffix == QStringLiteral("cxx") ||
                       suffix == QStringLiteral("c++")) {
                return s_IconTable[Icon::CPP];
            } else if (suffix == QStringLiteral("h")) {
                return s_IconTable[Icon::H];
            } else if (suffix == QStringLiteral("hpp") || suffix == QStringLiteral("hh") || suffix == QStringLiteral("hxx") ||
                       suffix == QStringLiteral("h++")) {
                return s_IconTable[Icon::HPP];
            } else if (suffix == QStringLiteral("cmake")) {
                return s_IconTable[Icon::CMAKE];
            } else if (suffix == QStringLiteral("s") || suffix == QStringLiteral("asm") || suffix == QStringLiteral("o") ||
                       suffix == QStringLiteral("o")) {
                return s_IconTable[Icon::ASM];
            } else if (suffix == QStringLiteral("hex") || suffix == QStringLiteral("bin") || suffix == QStringLiteral("raw")) {
                return s_IconTable[Icon::BIN];
            } else if (suffix == QStringLiteral("lhg")) {
                return s_IconTable[Icon::CERT];
            } else if (suffix == QStringLiteral("zip") || suffix == QStringLiteral("rar") || suffix == QStringLiteral("gz") ||
                       suffix == QStringLiteral("tar") || suffix == QStringLiteral("7z")) {
                return s_IconTable[Icon::COMPRESSED];
            } else if (suffix == QStringLiteral("out") || suffix == QStringLiteral("elf")) {
                return s_IconTable[Icon::EXE];
            } else if (suffix == QStringLiteral("ld") || suffix == QStringLiteral("icf")) {
                return s_IconTable[Icon::LINK];
            } else if (suffix == QStringLiteral("fnt")) {
                return s_IconTable[Icon::FONT];
            } else if (suffix == QStringLiteral("cfg") || suffix == QStringLiteral("config") || suffix == QStringLiteral("clang-format") ||
                       suffix == QStringLiteral("clang-tidy")) {
                return s_IconTable[Icon::GEAR];
            } else if (suffix == QStringLiteral("git") || suffix == QStringLiteral("gitmodules") || suffix == QStringLiteral("gitignore") ||
                       suffix == QStringLiteral("gitattributes")) {
                return s_IconTable[Icon::GIT];
            } else if (suffix == QStringLiteral("png") || suffix == QStringLiteral("jpg") || suffix == QStringLiteral("jpeg") ||
                       suffix == QStringLiteral("bmp") || suffix == QStringLiteral("webm") || suffix == QStringLiteral("gif")) {
                return s_IconTable[Icon::IMAGE];
            } else if (suffix == QStringLiteral("md")) {
                return s_IconTable[Icon::INFO];
            } else if (suffix == QStringLiteral("json")) {
                return s_IconTable[Icon::JSON];
            } else if (suffix == QStringLiteral("a") || suffix == QStringLiteral("lib") || suffix == QStringLiteral("dll")) {
                return s_IconTable[Icon::LIB];
            } else if (suffix == QStringLiteral("log")) {
                return s_IconTable[Icon::LOG];
            } else if (suffix == QStringLiteral("lua")) {
                return s_IconTable[Icon::LUA];
            } else if (suffix == QStringLiteral("pdf")) {
                return s_IconTable[Icon::PDF];
            } else if (suffix == QStringLiteral("py")) {
                return s_IconTable[Icon::PY];
            } else if (suffix == QStringLiteral("xml") || suffix == QStringLiteral("htm") || suffix == QStringLiteral("html") ||
                       suffix == QStringLiteral("qrc")) {
                return s_IconTable[Icon::XML];
            } else if (suffix == QStringLiteral("emproject") || suffix == QStringLiteral("emsession") ||
                       suffix == QStringLiteral("jlink")) {
                return s_IconTable[Icon::SEGGER];
            }

            return s_IconTable[Icon::FILE];
        }
    }

} // namespace HWD::UI