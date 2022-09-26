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
#include <QMenu>
#include <QClipboard>
#include <QAction>

extern void e_ClearApplicationLog(); // XXX: temp

///////////////////////////////////////////
// Test
#include <Core/Probe/JLink/JLink.hpp>
extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}
#include <Core/ELF/ELF_Reader.hpp>

namespace L0::ELF {
    extern ELF_Reader* g_Test_ELF_Reader;
}
extern const char* g_TargetDeviceModel;
extern uint32_t g_ProbeID;
extern L0::Probe::JLink* g_JLink;
extern bool g_CortexA;
extern void StartConnection();
///////////////////////////////////////////

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
        m_BottomLabel->setFixedHeight(18 * 2);
        m_BottomLabel->setAlignment(Qt::AlignVCenter);
        m_BottomLabel->setObjectName("monospaceTextObject");

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
            ProcessSearchExpression();
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

        m_HexEditor->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(m_HexEditor, &QHexEdit::customContextMenuRequested, [=](const QPoint& point) {
            OpenEditorContextMenu(point);
        });

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
                                 "\nLE: 0x%X | BE: 0x%X",
                                 qToLittleEndian(*(uint16_t*)data.data()),
                                 qToBigEndian(*(uint16_t*)data.data()));
                        break;
                    case 4:
                        snprintf(valueStr,
                                 sizeof(valueStr),
                                 "\nLE: 0x%X | BE: 0x%X",
                                 qToLittleEndian(*(uint32_t*)data.data()),
                                 qToBigEndian(*(uint32_t*)data.data()));
                        break;
                    case 8:
                        snprintf(valueStr,
                                 sizeof(valueStr),
                                 "\nLE: 0x%llX | BE: 0x%llX",
                                 qToLittleEndian(*(uint64_t*)data.data()),
                                 qToBigEndian(*(uint64_t*)data.data()));
                        break;
                    default:
                        valueStr[0] = '\n';
                        valueStr[1] = '\0';
                        break;
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
                snprintf(infoStr, sizeof(infoStr), "Cursor: %llu(0x%llX)\n ", cursorPos, cursorPos);
            }
            m_BottomLabel->setText(infoStr);
        });

        setWidget(ui->root);
    }

    void HexEditorPanel::SavePanelState(QSettings* cfg) {
        QString cfgKey = objectName();
        cfgKey.replace('/', '\\'); // QSettings does not like '/' in keys
        LOG_UI_TRACE("HexEditorPanel save state - {}", cfgKey);

        cfg->beginGroup(cfgKey);
        cfg->setValue("searchBarContent", ui->searchTextBar->text().trimmed());
        cfg->endGroup();
    }

    void HexEditorPanel::LoadPanelState(QSettings* cfg) {
        QString cfgKey = objectName();
        cfgKey.replace('/', '\\'); // QSettings does not like '/' in keys
        LOG_UI_TRACE("HexEditorPanel load state - {}", cfgKey);

        cfg->beginGroup(cfgKey);
        if (cfg->value("searchBarContent").isValid()) {
            ui->searchTextBar->setText(cfg->value("searchBarContent").toString());
        }
        cfg->endGroup();
    }

    bool HexEditorPanel::LoadFile(const QString& filePath) {
        if (filePath.startsWith("$FastMemoryView")) {
            ui->searchTextBar->setPlaceholderText("Load Range Expression or Lua Script ($path)");
            m_FastMemoryView = true;

            m_HexEditor->setData({});

            setObjectName(QSL("HexEditorPanel|") + filePath);
            tabWidget()->setToolTip(filePath);

            setWindowTitle(QSL("Fast Memory View (") + filePath + QSL(")"));

            setProperty("MULTI_INSTANCE", true);

            return true;
        } else {
            ui->searchTextBar->setPlaceholderText("Jump to Address...");
            m_FastMemoryView = false;
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
    }

    void HexEditorPanel::OpenEditorContextMenu(const QPoint& point) {
        auto menu    = new QMenu(this);
        auto selSize = m_HexEditor->GetSelectionSize();

        if (selSize == 2 || selSize == 4 || selSize == 8) {
            auto bdata = m_HexEditor->GetChunkData(m_HexEditor->GetSelectionStart(), m_HexEditor->GetSelectionSize());
            static char valueStr_LE[32];
            static char valueStr_BE[32];

            switch (selSize) {
                case 2:
                    snprintf(valueStr_LE, sizeof(valueStr_LE), "0x%X", qToLittleEndian(*(uint16_t*)bdata.data()));
                    snprintf(valueStr_BE, sizeof(valueStr_BE), "0x%X", qToBigEndian(*(uint16_t*)bdata.data()));
                    break;
                case 4:
                    snprintf(valueStr_LE, sizeof(valueStr_LE), "0x%X", qToLittleEndian(*(uint32_t*)bdata.data()));
                    snprintf(valueStr_BE, sizeof(valueStr_BE), "0x%X", qToBigEndian(*(uint32_t*)bdata.data()));
                    break;
                case 8:
                    snprintf(valueStr_LE, sizeof(valueStr_LE), "0x%llX", qToLittleEndian(*(uint64_t*)bdata.data()));
                    snprintf(valueStr_BE, sizeof(valueStr_BE), "0x%llX", qToBigEndian(*(uint64_t*)bdata.data()));
                    break;
                default: return;
            }

            auto copyValueLE = new QAction(QSL("Copy Value (%1)").arg(valueStr_LE), this);
            menu->addAction(copyValueLE);
            connect(copyValueLE, &QAction::triggered, this, [=]() {
                QApplication::clipboard()->setText(valueStr_LE);
            });

            auto copyValueBE = new QAction(QSL("Copy Value (%1)").arg(valueStr_BE), this);
            menu->addAction(copyValueBE);
            connect(copyValueBE, &QAction::triggered, this, [=]() {
                QApplication::clipboard()->setText(valueStr_BE);
            });
        }

        if (selSize) {
            auto copyInitializer = new QAction(QSL("Copy as C Array"), this);
            menu->addAction(copyInitializer);
            connect(copyInitializer, &QAction::triggered, this, [=]() {
                static char valueStr[8];
                auto bdata           = m_HexEditor->GetChunkData(m_HexEditor->GetSelectionStart(), m_HexEditor->GetSelectionSize());
                QString dataInitList = "";
                qsizetype byteIndex  = 0;
                for (auto b : bdata) {
                    snprintf(valueStr, sizeof(valueStr), (byteIndex < bdata.size() - 1) ? "0x%02X, " : "0x%02X", (uint8_t)b);
                    dataInitList += valueStr;
                    byteIndex++;
                }
                QApplication::clipboard()->setText(dataInitList);
            });
        }

        if (!menu->actions().isEmpty())
            menu->popup(m_HexEditor->viewport()->mapToGlobal(point));
    }

    void HexEditorPanel::ProcessSearchExpression() {
        auto searchText = ui->searchTextBar->text().trimmed();

        if (m_FastMemoryView) {
            if (!g_JLink) {
                LOG_CORE_CRITICAL("FastMemoryView Error: Probe/Device not selected");
                return;
            }

            if (!g_JLink->Probe_IsConnected() || !g_JLink->Target_IsConnected()) {
                LOG_CORE_CRITICAL("FastMemoryView Error: Probe/Device not connected");
                return;
            }

            if (!L0::ELF::g_Test_ELF_Reader) {
                LOG_CORE_CRITICAL("FastMemoryView Error: ELF file not loaded");
                return;
            }

            lua_State* L = luaL_newstate();
            luaL_openlibs(L);

            lua_pushcfunction(L, [](lua_State* L) -> int {
                auto name = luaL_checkstring(L, 1);
                auto it   = L0::ELF::g_Test_ELF_Reader->GetBasicSymbolTable().find(name);
                if (it != L0::ELF::g_Test_ELF_Reader->GetBasicSymbolTable().end()) {
                    lua_pushinteger(L, (*it).address);
                } else {
                    LOG_CORE_ERROR("SymbolNameToAddress: \"{}\" not found", name);
                    lua_pushnil(L);
                }
                return 1;
            });
            lua_setglobal(L, "__SymbolNameToAddress");
            lua_pushcfunction(L, [](lua_State* L) -> int {
                auto name = luaL_checkstring(L, 1);
                auto it   = L0::ELF::g_Test_ELF_Reader->GetBasicSymbolTable().find(name);
                if (it != L0::ELF::g_Test_ELF_Reader->GetBasicSymbolTable().end()) {
                    lua_pushinteger(L, (*it).size);
                } else {
                    LOG_CORE_ERROR("SymbolNameToSize: \"{}\" not found", name);
                    lua_pushnil(L);
                }
                return 1;
            });
            lua_setglobal(L, "__SymbolNameToSize");

            lua_pushcfunction(L, [](lua_State* L) -> int {
                auto addr = luaL_checkinteger(L, 1);
                if (!lua_isinteger(L, 1)) {
                    lua_pushnil(L);
                    LOG_CORE_ERROR("ReadMem8 invalid addr");
                    return 1;
                }

                bool s;
                auto val = g_JLink->Target_ReadMemory_8(addr, &s);
                //LOG_CORE_TRACE("FastMemoryView: ReadMem8 @ 0x{:X} == 0x{:X}", addr, val);
                if (s) {
                    lua_pushinteger(L, val);
                } else {
                    lua_pushnil(L);
                    LOG_CORE_ERROR("ReadMem8 read error (0x{:X} - 0x{:X})", addr, addr + 1);
                }

                return 1;
            });
            lua_setglobal(L, "ReadMem8");

            lua_pushcfunction(L, [](lua_State* L) -> int {
                auto addr  = luaL_checkinteger(L, 1);
                auto value = luaL_checkinteger(L, 2);
                if (!lua_isinteger(L, 1)) {
                    lua_pushnil(L);
                    LOG_CORE_ERROR("WriteMem8 invalid addr");
                    return 1;
                }
                if (!lua_isinteger(L, 2)) {
                    lua_pushnil(L);
                    LOG_CORE_ERROR("WriteMem8 invalid value");
                    return 1;
                }

                if (!g_JLink->Target_WriteMemory_8(addr, value)) {
                    LOG_CORE_ERROR("WriteMem8 write error (0x{:X} - 0x{:X})", addr, addr + 1);
                }

                return 0;
            });
            lua_setglobal(L, "WriteMem8");

            lua_pushcfunction(L, [](lua_State* L) -> int {
                auto addr = luaL_checkinteger(L, 1);
                if (!lua_isinteger(L, 1)) {
                    lua_pushnil(L);
                    LOG_CORE_ERROR("ReadMem16 invalid addr");
                    return 1;
                }

                bool s;
                auto val = g_JLink->Target_ReadMemory_16(addr, &s);
                //LOG_CORE_TRACE("FastMemoryView: ReadMem16 @ 0x{:X} == 0x{:X}", addr, val);
                if (s) {
                    lua_pushinteger(L, val);
                } else {
                    lua_pushnil(L);
                    LOG_CORE_ERROR("ReadMem16 read error (0x{:X} - 0x{:X})", addr, addr + 1);
                }

                return 1;
            });
            lua_setglobal(L, "ReadMem16");

            lua_pushcfunction(L, [](lua_State* L) -> int {
                auto addr = luaL_checkinteger(L, 1);
                if (!lua_isinteger(L, 1)) {
                    lua_pushnil(L);
                    LOG_CORE_ERROR("ReadMem32 invalid addr");
                    return 1;
                }

                bool s;
                auto val = g_JLink->Target_ReadMemory_32(addr, &s);
                //LOG_CORE_TRACE("FastMemoryView: ReadMem32 @ 0x{:X} == 0x{:X}", addr, val);
                if (s) {
                    lua_pushinteger(L, val);
                } else {
                    lua_pushnil(L);
                    LOG_CORE_ERROR("ReadMem32 read error (0x{:X} - 0x{:X})", addr, addr + 3);
                }

                return 1;
            });
            lua_setglobal(L, "ReadMem32");

            lua_pushcfunction(L, [](lua_State* L) -> int {
                auto addr = luaL_checkinteger(L, 1);
                if (!lua_isinteger(L, 1)) {
                    lua_pushnil(L);
                    LOG_CORE_ERROR("ReadMem64 invalid addr");
                    return 1;
                }

                bool s;
                auto val = g_JLink->Target_ReadMemory_64(addr, &s);
                //LOG_CORE_TRACE("FastMemoryView: ReadMem64 @ 0x{:X} == 0x{:X}", addr, val);
                if (s) {
                    lua_pushinteger(L, val);
                } else {
                    lua_pushnil(L);
                    LOG_CORE_ERROR("ReadMem64 read error (0x{:X} - 0x{:X})", addr, addr + 7);
                }

                return 1;
            });
            lua_setglobal(L, "ReadMem64");

            lua_pushcfunction(L, [](lua_State* L) -> int {
                auto rawVal = luaL_checkinteger(L, 1);
                if (!lua_isinteger(L, 1)) {
                    lua_pushnil(L);
                    return 1;
                }
                lua_pushnumber(L, *(float*)&rawVal);
                return 1;
            });
            lua_setglobal(L, "ParseRawFloat");

            lua_pushcfunction(L, [](lua_State* L) -> int {
                LOG_CORE_TRACE("> {}", luaL_checkstring(L, 1));
                return 0;
            });
            lua_setglobal(L, "__Print");

            lua_pushcfunction(L, [](lua_State* L) -> int {
                e_ClearApplicationLog();
                return 0;
            });
            lua_setglobal(L, "ClearApplicationLog");

            QFile libFile(":/Lua/CFXS_Lib.lua");
            libFile.open(QIODevice::OpenModeFlag::ReadOnly | QIODevice::OpenModeFlag::Text);
            auto libStat = luaL_dostring(L, libFile.readAll().data());
            libFile.close();

            if (libStat) {
                LOG_CORE_ERROR("FastMemoryView Lua Lib Error: {}", lua_tostring(L, -1));
                lua_close(L);
                return;
            }

            lua_pushcfunction(L, [](lua_State* L) -> int {
                QString path = luaL_checkstring(L, 1);
                if (!lua_isstring(L, 1)) {
                    lua_pushnil(L);
                    LOG_CORE_ERROR("include path not a string");
                    return 1;
                }

                QFile libFile(path);
                bool fileOpen = libFile.open(QIODevice::OpenModeFlag::ReadOnly | QIODevice::OpenModeFlag::Text);

                if (!fileOpen) {
                    LOG_CORE_ERROR("FastMemoryView Lua include not loaded \"{}\"", path);
                    return 0;
                }

                auto libStat = luaL_dostring(L, libFile.readAll().data());
                libFile.close();

                if (libStat) {
                    LOG_CORE_ERROR("FastMemoryView Lua include error: {}", lua_tostring(L, -1));
                    return 0;
                }

                return 0;
            });
            lua_setglobal(L, "include");

            LOG_CORE_TRACE("FastMemoryView Exec \"{}\"", searchText);

            if (g_CortexA) {
                g_JLink->Target_Halt();
                g_JLink->Target_WaitForHalt(1000);
            }

            if (searchText.at(0).toLatin1() == '$') {
                // load from file
                QFile f(searchText.mid(1));
                f.open(QIODevice::OpenModeFlag::Text | QIODevice::OpenModeFlag::ReadOnly);
                auto res = f.readAll();
                f.close();
                auto stat = luaL_dostring(L, res.data());
                if (stat) {
                    LOG_CORE_ERROR("FastMemoryView Lua Error: {}", lua_tostring(L, -1));
                    lua_close(L);
                    if (g_CortexA)
                        g_JLink->Target_Run();
                    return;
                }

            } else {
                // exec string
                auto stat = luaL_dostring(L, searchText.toStdString().c_str());
                if (stat) {
                    LOG_CORE_ERROR("FastMemoryView Lua Error: {}", lua_tostring(L, -1));
                    lua_close(L);
                    if (g_CortexA)
                        g_JLink->Target_Run();
                    return;
                }
            }

            lua_getglobal(L, "hex");
            if (!(lua_istable(L, 1) || lua_isnil(L, 1))) {
                LOG_CORE_ERROR("FastMemoryView Lua Error: Invalid _G.op");
                lua_close(L);
                if (g_CortexA)
                    g_JLink->Target_Run();
                return;
            }

            if (!lua_isnil(L, 1)) {
                lua_rawgeti(L, 1, 1);
                lua_rawgeti(L, 1, 2);

                if (!lua_isnumber(L, 2)) {
                    LOG_CORE_ERROR("FastMemoryView Lua Error: Address not a number");
                    lua_close(L);
                    if (g_CortexA)
                        g_JLink->Target_Run();
                    return;
                }
                if (!lua_isnumber(L, 3)) {
                    LOG_CORE_ERROR("FastMemoryView Lua Error: Size not a number");
                    lua_close(L);
                    if (g_CortexA)
                        g_JLink->Target_Run();
                    return;
                }

                auto addr = lua_tointeger(L, 2);
                auto size = lua_tointeger(L, 3);

                //LOG_CORE_TRACE("FastMemoryView Memory Read [0x{:X} - 0x{:X} ({} bytes)]", addr, addr + size, size, size);

                std::vector<char> readTemp;
                readTemp.resize(size);
                memset(readTemp.data(), 0, readTemp.size());
                g_JLink->Target_ReadMemoryTo(addr, readTemp.data(), size, L0::Probe::I_Probe::AccessWidth::_1);
                QByteArray readArray(readTemp.data(), readTemp.size());

                auto sel0 = m_HexEditor->GetSelectionStart();
                auto sel1 = m_HexEditor->GetSelectionEnd();
                m_HexEditor->setData(readArray);
                m_HexEditor->SetSelection(sel0, sel1);
            }

            lua_close(L);
            if (g_CortexA)
                g_JLink->Target_Run();
        } else {
            auto val = searchText;
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
        }
    }

} // namespace L0::UI