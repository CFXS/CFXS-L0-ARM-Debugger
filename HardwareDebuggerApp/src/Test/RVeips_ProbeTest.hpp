#pragma once

#include <Probe/IProbe.hpp>

namespace HWD::Test {

    class RVeips_ProbeTest {
    public:
        RVeips_ProbeTest();
        ~RVeips_ProbeTest();

        const char* GetTerminalText();

    private:
        Probe::IProbe* m_Probe;
    };

} // namespace HWD::Test