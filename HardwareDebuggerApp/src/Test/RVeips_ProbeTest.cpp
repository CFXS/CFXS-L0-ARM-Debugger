// [source]
#include "RVeips_ProbeTest.hpp"

#include <Probe/JLink/JLink_Probe.hpp>

#include "TestFirmware.h"
static constexpr uint32_t addr_FLASH_PERIOD = 0x2000026C; // uint32_t

namespace HWD::Test {

    using namespace Probe;

    void RVeips_ProbeTest::Run() {
        HWDLOG_CORE_TRACE("RVeips_ProbeTest::Run()");

        auto connectedProbes = JLink_Probe::s_GetConnectedProbes();
        for (IProbe* probe : connectedProbes) {
            probe->Probe_Connect();

            probe->Target_SelectDebugInterface(IProbe::DebugInterface::SWD);
            probe->Target_SelectDevice({"TM4C129ENCPDT"});
            probe->Target_Connect();

            if (probe->Target_IsConnected()) {
                probe->Target_WriteProgram(firmware, sizeof(firmware));
                probe->Target_Reset(false);

                probe->Target_StartTerminal();
            }

            //probe->Probe_Disconnect();
        }
    }

} // namespace HWD::Test