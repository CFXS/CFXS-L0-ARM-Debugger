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

    namespace Test {
        extern RVeips_ProbeTest* m_RV;
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
        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

        ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_HorizontalScrollbar);

        ImGui::Text("Flash progress");
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, {0.0f, 0.4f, 1.0f, 1.0f});
        ImGui::ProgressBar(Test::m_RV ? Test::m_RV->FlashProgress() : 0);

        int currentPeriod = m_RihardsTest ? m_RihardsTest->Read32(536871532) : 0;

        ImGui::Text("CFXS::Time::ms = %llu", m_RihardsTest ? m_RihardsTest->ReadMilliseconds() : 0);
        ImGui::Text("CPU_CYCLES     = %u", m_RihardsTest ? m_RihardsTest->Read32(0xE0001004, true) : 0);
        ImGui::Text("PORT_N_BASE    = 0x%X", m_RihardsTest ? m_RihardsTest->Read32(0x40025004) : 0);
        ImGui::Text("FLASH_PERIOD");
        if (ImGui::InputInt("", &currentPeriod, 10, 100, ImGuiInputTextFlags_EnterReturnsTrue)) {
            m_RihardsTest->Write32(536871532, currentPeriod);
        }
        ImGui::Text("");
        ImGui::BeginChild("Terminal");
        ImGui::Text(m_RihardsTest ? m_RihardsTest->GetTerminalText() : "Waiting for debug session");
        ImGui::SetScrollHereY(1.0f);
        ImGui::EndChild();

        ImGui::End();
    }

    void HWD_Application::OnEvent() {
    }

} // namespace HWD