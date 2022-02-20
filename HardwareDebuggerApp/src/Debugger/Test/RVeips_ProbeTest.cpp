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
#include "RVeips_ProbeTest.hpp"

#include <Debugger/Probe/JLink/JLink.hpp>
#include <Debugger/Target/Cortex/DWT_Registers.hpp>
#include <Debugger/Target/Cortex/ITM_Registers.hpp>
#include <Debugger/Target/Cortex/ROM_Table.hpp>
#include <Debugger/Target/Cortex/TPIU_Registers.hpp>
#include <fstream>
#include <QFileDialog>

static bool s_Stopped = false;

namespace L0::Probe {
    I_Probe* s_Probe;
}

namespace L0::Test {

    using namespace Cortex::M4;
    using namespace Probe;

    RVeips_ProbeTest* m_RV = nullptr;

    std::vector<uint8_t> readFile(const char* filename) {
        // open the file:
        std::ifstream file(filename, std::ios::binary);

        // Stop eating new lines in binary mode!!!
        file.unsetf(std::ios::skipws);

        // get its size:
        std::streampos fileSize;

        file.seekg(0, std::ios::end);
        fileSize = file.tellg();
        file.seekg(0, std::ios::beg);

        // reserve capacity
        std::vector<BYTE> vec;
        vec.reserve(fileSize);

        // read the data:
        vec.insert(vec.begin(), std::istream_iterator<BYTE>(file), std::istream_iterator<BYTE>());

        return vec;
    }

    RVeips_ProbeTest::RVeips_ProbeTest() {
        LOG_CORE_INFO("RVeips_ProbeTest()");

        ProbeTest();
    }

    /////////////////////////////////////////////////////////////////////////////////////////
    // Probe

    void RVeips_ProbeTest::ProbeTest() {
        SupportedDevices::LoadSupportedDevices();
        auto& testDevice = SupportedDevices::GetSupportedDevices().at("TM4C1294NC");

        auto jl = new JLink;
        jl->L0_SelectDevice(801022602);
        jl->Probe_Connect();
        jl->Target_SelectDebugInterface(I_Probe::DebugInterface::SWD);
        jl->Target_SelectDevice(testDevice);
        jl->Target_Connect();

        /*auto connectedProbes = JLink::s_GetConnectedProbes();
        for (I_Probe* probe : connectedProbes) {
            if (probe->GetModelName() != "SAM-ICE") {
                I_Probe::SetCurrentProbe(probe);

                probe->Probe_Connect();

                Probe::s_Probe = probe;

                probe->Target_SelectDebugInterface(I_Probe::DebugInterface::SWD);
                probe->Target_SelectDevice(testDevice);
                probe->Target_Connect();

                if (probe->Target_IsConnected()) {
                    m_Probe = probe;
                    m_RV    = this;

                    probe->Target_Erase();

                    auto firmwareBin = readFile(R"(C:\Users\CFXS\Documents\GitHub\ARM-Cortex-Network-Test\build\CortexNetworkTestApp.bin)");

                    std::vector<uint8_t> firmware;
                    firmware.resize(0x10000, 0);

                    for (auto b : firmwareBin)
                        firmware.push_back(b);

                    //probe->Target_WriteProgram(firmware.data(), static_cast<uint32_t>(firmware.size()));

                    probe->Target_Reset(true);

                    //////////////////////////////////////////////////////////////////////////////////

                    uint32_t rom_table_address;
                    rom_table_address = 0xE00FF000;

                    LOG_PROBE_TRACE("Target [Debug]");
                    if (rom_table_address) {
                        LOG_PROBE_TRACE("\tROM Table: @ {0:#X}", rom_table_address);
                    } else {
                        LOG_PROBE_CRITICAL("\tROM Table: @ {0:#X}", rom_table_address);
                    }

                    int bytesRead;
                    if (rom_table_address) {
                        // ROM TABLE /////////////////////////////////////////////////////////////
                        // [ARM Debug Interface v5 Architecture Specification]
                        // - ROM Tables > ROM Table Overview > "A ROM Table always occupies 4kB of memory" (4000 or 4096?)
                        uint8_t rom_table_data[4096];
                        Cortex::M4::ROM_Table_Offsets rtOffsets;
                        memset(&rtOffsets, 0, sizeof(rtOffsets));

                        bytesRead =
                            probe->Target_ReadMemoryTo(rom_table_address, rom_table_data, sizeof(rom_table_data), I_Probe::AccessWidth::_4);
                        LOG_PROBE_INFO("rom table read {0} bytes", bytesRead);

                        if (bytesRead == 4096) {
                            Cortex::M4::ROM_Table_Entry* rtEntry = (Cortex::M4::ROM_Table_Entry*)rom_table_data;
                            int entryIndex                       = 0;
                            do {
                                if (rtEntry->Is8bit()) {
                                    LOG_PROBE_CRITICAL("8bit ROM Table entries not supported yet");
                                    break;
                                }

                                uint32_t componentAddress = rtEntry->GetComponentAddress(rom_table_address);
                                uint32_t componentBaseAddress;

                                LOG_PROBE_INFO(" - {0} {1}",
                                                  Cortex::M4::ROM_Table_EntryName[entryIndex],
                                                  rtEntry->IsPresent() ? "Present" : "Not present");

                                if (rtEntry->IsPresent()) {
                                    Cortex::M4::PeripheralID4 pid4;
                                    pid4._val = probe->Target_ReadMemory_8(componentAddress + Cortex::CoreSight::OFFSET_PERIPHERAL_ID4);
                                    componentBaseAddress         = componentAddress - (4096 * (pid4.GetBlockCount() - 1));
                                    rtOffsets._table[entryIndex] = componentBaseAddress;

                                    LOG_PROBE_INFO("\tComponent Base Address: {0:#X}", componentBaseAddress);

                                    uint32_t cs_class =
                                        (probe->Target_ReadMemory_32(componentBaseAddress + Cortex::CoreSight::OFFSET_COMPONENT_ID1) >> 4) &
                                        0x0F;
                                    static constexpr auto getClassString = [](uint32_t c) {
                                        switch (c) {
                                            case Cortex::CoreSight::CLASS_CORESIGHT: return "CoreSight";
                                            case Cortex::CoreSight::CLASS_GENERIC: return "Generic";
                                            case Cortex::CoreSight::CLASS_PRIMECELL: return "PrimeCell";
                                            case Cortex::CoreSight::CLASS_ROMTABLE: return "ROM Table";
                                            default: return "Unknown";
                                        }
                                    };
                                    LOG_PROBE_INFO("\tComponent Class: {0}", getClassString(cs_class));
                                }

                                rtEntry++;
                                entryIndex++;
                            } while (!rtEntry->IsTableEndMarker());

                            if (rtOffsets.SCS) {
                                if (probe->Target_WriteMemory_32(rtOffsets.SCS + Cortex::CoreSight::OFFSET_LOCK_ACCESS,
                                                                 Cortex::CoreSight::VAL_UNLOCK_KEY)) {
                                    LOG_PROBE_TRACE("Unlocked SCS");
                                } else {
                                    LOG_PROBE_ERROR("Failed to unlock SCS");
                                }
                            }
                            if (rtOffsets.DWT) {
                                if (probe->Target_WriteMemory_32(rtOffsets.DWT + Cortex::CoreSight::OFFSET_LOCK_ACCESS,
                                                                 Cortex::CoreSight::VAL_UNLOCK_KEY)) {
                                    LOG_PROBE_TRACE("Unlocked DWT");
                                } else {
                                    LOG_PROBE_ERROR("Failed to unlock DWT");
                                }
                            }
                            if (rtOffsets.ITM) {
                                if (probe->Target_WriteMemory_32(rtOffsets.ITM + Cortex::CoreSight::OFFSET_LOCK_ACCESS,
                                                                 Cortex::CoreSight::VAL_UNLOCK_KEY)) {
                                    LOG_PROBE_TRACE("Unlocked ITM");
                                } else {
                                    LOG_PROBE_ERROR("Failed to unlock ITM");
                                }
                            }
                            if (rtOffsets.TPIU) {
                                if (probe->Target_WriteMemory_32(rtOffsets.TPIU + Cortex::CoreSight::OFFSET_LOCK_ACCESS,
                                                                 Cortex::CoreSight::VAL_UNLOCK_KEY)) {
                                    LOG_PROBE_TRACE("Unlocked TPIU");
                                } else {
                                    LOG_PROBE_ERROR("Failed to unlock TPIU");
                                }
                            }

                            if (rtOffsets.SCS && rtOffsets.DWT && rtOffsets.ITM && rtOffsets.TPIU) {
                                // enable Trace/DWT/ITM
                                // CoreDebug offset = 0xDF0

                                if (probe->Target_WriteMemory_32(rtOffsets.SCS + 0xDF0 + 0x00C, (1 << 24))) {
                                    LOG_PROBE_TRACE("Enabled trace");
                                } else {
                                    LOG_PROBE_ERROR("Failed to enable trace");
                                }

                                // configure pc sampling
                                DWT::REG_CTRL dwt_ctrl(rtOffsets.DWT + DWT::OFFSET_CTRL);
                                ITM::REG_TER itm_ter(rtOffsets.ITM + ITM::OFFSET_TER(0));
                                ITM::REG_TCR itm_tcr(rtOffsets.ITM + ITM::OFFSET_TCR);
                                TPIU::REG_ACPR tpiu_acpr(rtOffsets.TPIU + TPIU::OFFSET_ACPR);
                                TPIU::REG_SPPR tpiu_sspr(rtOffsets.TPIU + TPIU::OFFSET_SPPR);

                                itm_tcr  = 0;
                                dwt_ctrl = 0;

                                // Configure TPIU [ do not configure manually - setting the prescaler does something weird and the probe cant receive data properly ]
                                //                  - setting prescaler while core halted... weird stuff
                                // TODO: check with logic analyzer - what happens to SWO data when setting prescaler
                                //tpiu_sspr.Set_Interface(TPIU::REG_SPPR::Interface::SWO_MANCHESTER);
                                //tpiu_acpr.Set_Prescaler(((int)(120000000.0 / (4000000.0 * 2) + 0.5)) - 1);

                                if (probe->Target_WriteMemory_32(rtOffsets.TPIU + TPIU::OFFSET_CSPSR, 1)) { // port size = 1bit
                                    LOG_PROBE_TRACE("Configured CSPSR");
                                } else {
                                    LOG_PROBE_ERROR("Failed to configure CSPSR");
                                }

                                if (probe->Target_WriteMemory_32(rtOffsets.TPIU + 0x304, 0x100)) {
                                    LOG_PROBE_TRACE("Configured TPIU FFCR");
                                } else {
                                    LOG_PROBE_ERROR("Failed to configure TPIU FFCR");
                                }

                                if (probe->Target_WriteMemory_32(rtOffsets.TPIU + 0xE40, 0x0F)) {
                                    LOG_PROBE_TRACE("Configured TPIU TPR");
                                } else {
                                    LOG_PROBE_ERROR("Failed to configure TPIU TPR");
                                }

                                auto sampleRateDivider = DWT::REG_CTRL::SampleRateDivider::_2048;
                                dwt_ctrl.Set_Cycle_Counter_Enabled(true);
                                dwt_ctrl.Set_Sampling_Divider(sampleRateDivider);
                                dwt_ctrl.Set_PC_Sampling_Enabled(true);
                                dwt_ctrl.Set_Exception_Trace_Enabled(true);
                                dwt_ctrl.Set_Sync(DWT::REG_CTRL::SyncInterval::MEDIUM);
                                LOG_PROBE_TRACE("DWT_CTRL = {0:#X}", dwt_ctrl.GetRaw());

                                LOG_PROBE_TRACE("Sampling frequency: {0}kHz",
                                                   decltype(dwt_ctrl)::SampleRateDividerToSampleRate(120e6, sampleRateDivider));

                                itm_tcr = ((0 << ITM::REG_TCR::SHIFT_TRACE_BUS_ID) |                            //
                                           (1 << ITM::REG_TCR::SHIFT_TXENA) |                                   //
                                           (1 << ITM::REG_TCR::SHIFT_TSENA) |                                   //
                                           (1 << ITM::REG_TCR::SHIFT_SYNCENA) |                                 //
                                           (1 << ITM::REG_TCR::SHIFT_ITMENA) |                                  //
                                           (ITM::REG_TCR::VAL_TSPRESCALE_1 << ITM::REG_TCR::SHIFT_TSPRESCALE)); //

                                LOG_PROBE_TRACE("ITM_TCR = {0:#X}", itm_tcr.GetRaw());

                                itm_ter.Set_Enabled(0xFFFFFFFF, true);
                                probe->Target_WriteMemory_32(rtOffsets.ITM + ITM::OFFSET_TPR, 0xFFFFFFFF);
                            }
                        } else {
                            LOG_PROBE_CRITICAL("Failed to read ROM Table");
                        }
                    }

                    //probe->Target_StartTerminal();
                    probe->Target_Run();

                    //////////////////////////////////////////////////////////////////////////////////
                }

                auto thread = new std::thread([=]() {
                    while (1 < 2) {
                        std::this_thread::sleep_for(std::chrono::milliseconds(1));
                        if (s_Stopped == false) {
                            probe->Process();
                        }
                    }
                });
                thread->detach();
            }
        }*/
    }

    RVeips_ProbeTest::~RVeips_ProbeTest() {
    }

} // namespace L0::Test