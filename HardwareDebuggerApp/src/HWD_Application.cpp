// [source]
#include "HWD_Application.hpp"

#include "main.hpp"

#include <GL/glew.h>
#include <imgui.h>

namespace HWD {

    HWD_Application::HWD_Application(int argc, char** argv) : Application(argc, argv, "CFXS Hardware Debugger " CFXS_HWD_VERSION_STRING) {
    }

    HWD_Application::~HWD_Application() {
    }

    void HWD_Application::OnCreate() {
        (new std::thread([=]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            m_RihardsTest = std::make_unique<Test::RVeips_ProbeTest>();
        }))->detach();
    }

    void HWD_Application::OnDestroy() {
    }

    void HWD_Application::OnUpdate() {
        glClearColor(0.1f, 0.1f, 0.1f, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    float val[4] = {0, 0, 0, 0};
    void HWD_Application::OnImGuiRender() {
        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
    }

    void HWD_Application::OnEvent() {
    }

} // namespace HWD