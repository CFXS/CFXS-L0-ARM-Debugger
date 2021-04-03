// [source]
#include "HWD_Application.hpp"

#include "main.hpp"

namespace HWD {

    HWD_Application::HWD_Application(int argc, char** argv) : Application(argc, argv, "CFXS Hardware Debugger " CFXS_HWD_VERSION_STRING) {
    }

    HWD_Application::~HWD_Application() {
    }

    void HWD_Application::OnCreate() {
        m_RihardsTest = std::make_unique<Test::RVeips_ProbeTest>();
    }

    void HWD_Application::OnDestroy() {
    }

    void HWD_Application::OnUpdate() {
    }

    void HWD_Application::OnImGuiRender() {
    }

    void HWD_Application::OnEvent() {
    }

} // namespace HWD