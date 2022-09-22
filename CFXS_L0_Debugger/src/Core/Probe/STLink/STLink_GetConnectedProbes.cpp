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
#include "Log/Log.hpp"
#include "STLink.hpp"
#include <QFile>
#include <stlink/stlink.h>
#include <stlink-lib/usb.h>

extern "C" void process_chipfile_buffer(const char **fileBufferLines, int lineCount);

namespace L0::Probe {

    static constexpr auto MAX_STLINK_PROBES = 8;

    static const QVector<QString> s_ChipFiles = {
        ":stlink/F03x.chip",           ":stlink/F04x.chip",           ":stlink/F05x.chip",           ":stlink/F07x.chip",
        ":stlink/F09x.chip",           ":stlink/F1xx_CL.chip",        ":stlink/F1xx_HD.chip",        ":stlink/F1xx_LD.chip",
        ":stlink/F1xx_MD.chip",        ":stlink/F1xx_VL_HD.chip",     ":stlink/F1xx_VL_MD_LD.chip",  ":stlink/F1xx_XLD.chip",
        ":stlink/F2xx.chip",           ":stlink/F301_F302_F318.chip", ":stlink/F302_F303_F358.chip", ":stlink/F302_F303_F398_HD.chip",
        ":stlink/F303_F328_F334.chip", ":stlink/F37x.chip",           ":stlink/F401xB_xC.chip",      ":stlink/F401xD_xE.chip",
        ":stlink/F410.chip",           ":stlink/F411xC_xE.chip",      ":stlink/F412.chip",           ":stlink/F413_F423.chip",
        ":stlink/F42x_F43x.chip",      ":stlink/F446.chip",           ":stlink/F46x_F47x.chip",      ":stlink/F4x5_F4x7.chip",
        ":stlink/F72x_F73x.chip",      ":stlink/F74x_F75x.chip",      ":stlink/F76x_F77x.chip",      ":stlink/G03x_G04x.chip",
        ":stlink/G05x_G06x.chip",      ":stlink/G07x_G08x.chip",      ":stlink/G0Bx_G0Cx.chip",      ":stlink/G43x_G44x.chip",
        ":stlink/G47x_G48x.chip",      ":stlink/G49x_G4Ax.chip",      ":stlink/H72x_H73x.chip",      ":stlink/H74x_H75x.chip",
        ":stlink/H7Ax_H7Bx.chip",      ":stlink/L0xxx_Cat_1.chip",    ":stlink/L0xxx_Cat_2.chip",    ":stlink/L0xxx_Cat_3.chip",
        ":stlink/L0xxx_Cat_5.chip",    ":stlink/L1xx_Cat_1.chip",     ":stlink/L1xx_Cat_2.chip",     ":stlink/L1xx_Cat_3.chip",
        ":stlink/L1xx_Cat_4.chip",     ":stlink/L1xx_Cat_5.chip",     ":stlink/L41x_L42x.chip",      ":stlink/L43x_L44x.chip",
        ":stlink/L45x_L46x.chip",      ":stlink/L47x_L48x.chip",      ":stlink/L496x_L4A6x.chip",    ":stlink/L4Px.chip",
        ":stlink/L4Rx.chip",           ":stlink/unknown_device.chip", ":stlink/WBx0_WBx5.chip",      ":stlink/WLx5.chip",
    };

    void STLink::InitializeChipIDs() {
        LOG_PROBE_TRACE("Initialize ST-Link ChipIDs");
        for (auto &chipPath : s_ChipFiles) {
            QFile file(chipPath);
            file.open(QFile::ReadOnly);
            QString content(file.readAll());
            file.close();
            QStringList lines = content.split("\n");
            QVector<char *> lineList;
            for (auto &line : lines) {
                lineList.push_back(new char[256]);
                line = line.replace("\r", "");
                snprintf(lineList.last(), 256, "%s\n", line.toUtf8().data());
            }
            process_chipfile_buffer((const char **)lineList.data(), lineList.size());
            for (auto line : lineList) {
                delete line;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    std::vector<STLink::ProbeInfo> STLink::GetConnectedProbes() {
        std::vector<ProbeInfo> probes;

        for (int i = 0; i < MAX_STLINK_PROBES; i++) {
            auto st = stlink_open_usb(UERROR, CONNECT_NORMAL, 0, 0, i + 1);
            if (st) {
                probes.push_back({"ST-Link v" + QString::number(st->version.stlink_v), st->serial});
                stlink_close(st);
            }
        }

        return probes;
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace L0::Probe