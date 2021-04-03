#pragma once

#include <Probe/IProbe.hpp>

namespace HWD::Test {

    class RVeips_ProbeTest {
    public:
        RVeips_ProbeTest();
        ~RVeips_ProbeTest();

        const char* GetTerminalText();
        uint64_t ReadMilliseconds();
        uint32_t Read32(uint32_t addr, bool halt = false);
        uint32_t Write32(uint32_t addr, uint32_t value);
        float FlashProgress();

        bool Running() {
            return m_Probe ? !m_Probe->Target_IsRunning() : false;
        }

        void Run() {
            std::cout << "run\n";
            m_Probe->Target_Run();
        }

        void Stop() {
            std::cout << "stop\n";
            m_Probe->Target_Halt();
        }

    private:
        Probe::IProbe* m_Probe;
        bool m_Programming = false;
    };

} // namespace HWD::Test