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

    static ImFont* font_SCP;
    void HWD_Application::OnCreate() {
        (new std::thread([=]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            m_RihardsTest = std::make_unique<Test::RVeips_ProbeTest>();
        }))->detach();

        auto& io = ImGui::GetIO();
        font_SCP = io.Fonts->AddFontFromFileTTF("C:/CFXS/SourceCodePro-Regular.ttf", 24);
    }

    void HWD_Application::OnDestroy() {
    }

    void HWD_Application::OnUpdate() {
        glClearColor(0.1f, 0.1f, 0.1f, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    float val[4] = {0, 0, 0, 0};
    void HWD_Application::OnImGuiRender() {
        ImGui::Begin("JLink", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_HorizontalScrollbar);
        ImGui::ProgressBar(0.25f);

        ImGui::BeginChild("Terminal");
        ImGui::Text("CFXS::Time::ms = %llu", 0);
        ImGui::Text("");
        ImGui::Text(m_RihardsTest ? m_RihardsTest->GetTerminalText() : "Waiting for debug session");
        ImGui::SetScrollHereY(1.0f);
        ImGui::EndChild();

        ImGui::End();
    }

    void HWD_Application::OnEvent() {
    }

} // namespace HWD