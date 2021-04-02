// [source]
#include "RVeips_ProbeTest.hpp"

#include <Probe/JLink/JLink_Probe.hpp>

namespace HWD::Test {

    void RVeips_ProbeTest::Run() {
        HWDLOG_CORE_TRACE("RVeips_ProbeTest::Run()");

        for (auto* probe : Probe::JLink_Probe::GetConnectedProbes()) {
            probe->OpenConnection();
            probe->GetTargetStatus();
            probe->CloseConnection();
        }
    }

} // namespace HWD::Test