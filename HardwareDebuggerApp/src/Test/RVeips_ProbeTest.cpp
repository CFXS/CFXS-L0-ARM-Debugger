// [source]
#include "RVeips_ProbeTest.hpp"
#include <Probe/JLink/JLink_Probe.hpp>

#include "TestFirmware.h"

namespace HWD::Test {

    using namespace Probe;
    RVeips_ProbeTest* m_RV = nullptr;

    RVeips_ProbeTest::RVeips_ProbeTest() {
        TargetDevices::LoadSupportedDevices();
        auto& testDevice = TargetDevices::GetSupportedDevices().at("TM4C1294NC");

        auto connectedProbes = JLink_Probe::s_GetConnectedProbes();
        for (IProbe* probe : connectedProbes) {
            probe->Probe_Connect();

            probe->Target_SelectDebugInterface(IProbe::DebugInterface::SWD);
            probe->Target_SelectDevice(testDevice);
            probe->Target_Connect();

            if (probe->Target_IsConnected()) {
                m_Probe = probe;
                m_RV    = this;

                probe->Target_Erase();

                probe->Target_WriteProgram(firmware, sizeof(firmware));
                probe->Target_Reset(false);

                probe->Target_StartTerminal();
            }

            auto thread = new std::thread([=]() {
                while (1 < 2) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                    probe->Process();
                }
            });
            thread->detach();
        }
    }

    RVeips_ProbeTest::~RVeips_ProbeTest() {
        auto connectedProbes = JLink_Probe::s_GetConnectedProbes();
        for (IProbe* probe : connectedProbes) {
            delete probe;
        }
    }

    //

    uint64_t RVeips_ProbeTest::ReadMilliseconds() {
        uint64_t var = 0;

        if (m_Probe) {
            bool ret = m_Probe->Target_ReadMemory_64(536874024, &var);
            if (!ret)
                var = 0;
        }

        return var;
    }

    uint32_t RVeips_ProbeTest::Read32(uint32_t addr, bool halt) {
        uint32_t var = 0;

        if (m_Probe) {
            if (halt)
                m_Probe->Target_Halt();
            bool ret = m_Probe->Target_ReadMemory_32(addr, &var);
            if (halt)
                m_Probe->Target_Run();
            if (!ret)
                var = 0;
        }

        return var;
    }

    uint32_t RVeips_ProbeTest::Write32(uint32_t addr, uint32_t value) {
        m_Probe->Target_WriteMemory_32(addr, value);
        return 0;
    }

    float RVeips_ProbeTest::FlashProgress() {
        return m_Probe->Target_GetFlashProgress();
    }

    const char* RVeips_ProbeTest::GetTerminalText() {
        if (m_Probe)
            m_Probe->Target_GetTerminalBuffer();
        else
            return "Waiting for target...";
    }

} // namespace HWD::Test