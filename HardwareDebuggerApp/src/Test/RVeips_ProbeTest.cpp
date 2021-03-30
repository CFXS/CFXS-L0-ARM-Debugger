// [source]
#include "RVeips_ProbeTest.hpp"

namespace HWD::Test {

    static Probe::IProbe* s_TestProbe;

    void RVeips_ProbeTest::Run() {
        HWDLOG_CORE_TRACE("RVeips_ProbeTest::Run()");

        s_TestProbe = new Probe::JLink;

        HWDLOG_CORE_INFO("s_TestProbe[{0}] Model: {1}", fmt::ptr(s_TestProbe), s_TestProbe->GetModel());
        HWDLOG_CORE_INFO("s_TestProbe[{0}] S/N:   {1}", fmt::ptr(s_TestProbe), s_TestProbe->GetSerialNumber());
    }

} // namespace HWD::Test