// [source]
#include "RVeips_ProbeTest.hpp"
#include <Probe/JLink/JLink_Probe.hpp>

#include "TestFirmware.h"
static constexpr uint32_t addr_FLASH_PERIOD = 0x2000026C; // uint32_t

namespace HWD::Test {

    using namespace Probe;

    RVeips_ProbeTest::RVeips_ProbeTest() {
        TargetDevices::LoadSupportedDevices();

        auto connectedProbes = JLink_Probe::s_GetConnectedProbes();
        for (IProbe* probe : connectedProbes) {
            probe->Probe_Connect();

            probe->Target_SelectDebugInterface(IProbe::DebugInterface::SWD);
            probe->Target_SelectDevice(TargetDevices::GetSupportedDevices().at("TM4C1294NCPDT"));
            probe->Target_Connect();

            if (probe->Target_IsConnected()) {
                probe->Target_WriteProgram(firmware, sizeof(firmware));
                probe->Target_Reset(false);

                probe->Target_StartTerminal();
            }
        }
    }

    RVeips_ProbeTest::~RVeips_ProbeTest() {
        auto connectedProbes = JLink_Probe::s_GetConnectedProbes();
        for (IProbe* probe : connectedProbes) {
            delete probe;
        }
    }

} // namespace HWD::Test