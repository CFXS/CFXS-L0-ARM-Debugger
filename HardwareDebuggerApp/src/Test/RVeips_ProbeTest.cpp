// [source]
#include "RVeips_ProbeTest.hpp"

#include <Probe/JLink/JLink_Probe.hpp>

namespace HWD::Test {

    void RVeips_ProbeTest::Run() {
        HWDLOG_CORE_TRACE("RVeips_ProbeTest::Run()");

        int i = 0;
        for (auto* probe : Probe::JLink_Probe::GetConnectedProbes()) {
            HWDLOG_CORE_INFO("JLink Probe #{0} - Name: {1} - S/N: {2}", i++, probe->GetModelName(), probe->GetSerialNumberString());
            probe->OpenConnection();
            probe->GetTargetStatus();
            probe->CloseConnection();
        }
    }

} // namespace HWD::Test