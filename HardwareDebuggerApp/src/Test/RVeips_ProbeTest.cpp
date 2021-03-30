// [source]
#include "RVeips_ProbeTest.hpp"

namespace HWD::Test {

    static Probe::IProbe* s_TestProbe;

    void RVeips_ProbeTest::Run() {
        s_TestProbe = new Probe::JLink;
    }

} // namespace HWD::Test