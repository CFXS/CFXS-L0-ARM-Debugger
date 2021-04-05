// [source]
#include "HWD_Application.hpp"

#include "main.hpp"

#include <GL/glew.h>
#include <imgui.h>

namespace HWD {
    ///////////////////////////////////////////////////////////////////////////////// !Bodge alert! - SDL SetWindowTitle implementation "no prefix" tag
    HWD_Application::HWD_Application(int argc, char** argv) :
        Application(argc, argv, CFXS_HWD_WINDOW_TITLE_NO_PREFIX CFXS_HWD_PROGRAM_NAME) {
    }

    HWD_Application::~HWD_Application() {
    }

    namespace Test {
        extern RVeips_ProbeTest* m_RV;
    }
    static ImFont* font_SCP;
    static ImFont* font_OS;
    void HWD_Application::OnCreate() {
        (new std::thread([=]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            m_RihardsTest = std::make_unique<Test::RVeips_ProbeTest>();
        }))->detach();

        auto& io = ImGui::GetIO();
        font_SCP = io.Fonts->AddFontFromFileTTF("C:/CFXS/SourceCodePro-Regular.ttf", 24);
        font_SCP = io.Fonts->AddFontFromFileTTF("C:/CFXS/SourceCodePro-Regular.ttf", 24);

        io.ConfigViewportsNoDecoration = false;

        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4{0.1f, 0.105f, 0.11f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4{0.2f, 0.205f, 0.21f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4{0.3f, 0.305f, 0.31f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4{0.15f, 0.1505f, 0.151f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.205f, 0.21f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.3f, 0.305f, 0.31f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.15f, 0.1505f, 0.151f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4{0.2f, 0.205f, 0.21f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4{0.3f, 0.305f, 0.31f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4{0.15f, 0.1505f, 0.151f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_Tab, ImVec4{0.15f, 0.1505f, 0.151f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_TabHovered, ImVec4{0.38f, 0.3805f, 0.381f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_TabActive, ImVec4{0.28f, 0.2805f, 0.281f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_TabUnfocused, ImVec4{0.15f, 0.1505f, 0.151f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_TabUnfocusedActive, ImVec4{0.2f, 0.205f, 0.21f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4{0.15f, 0.1505f, 0.151f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4{0.15f, 0.1505f, 0.151f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, ImVec4{0.15f, 0.1505f, 0.151f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, {0.0f, 0.4f, 1.0f, 1.0f});
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {8.0, 8.0});
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, {8.0, 8.0});
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, {4.0, 4.0});
    }

    void HWD_Application::OnDestroy() {
    }

    void HWD_Application::OnUpdate() {
        glClearColor(0.1f, 0.1f, 0.1f, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    float val[4] = {0, 0, 0, 0};
    void HWD_Application::OnImGuiRender() {
        ImGuiWindowClass window_class;
        // flags to remove dropdown at left side of docked tab bar
        window_class.DockNodeFlagsOverrideSet   = 1 << 14;
        window_class.DockNodeFlagsOverrideClear = 1 << 12;

        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

        ImGui::BeginMainMenuBar();
        {
            if (ImGui::BeginMenu("File")) {
                ImGui::MenuItem("Exit");
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Flash")) {
                ImGui::MenuItem("Select file");
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
        ImGui::EndMainMenuBar();

        ImGui::SetNextWindowClass(&window_class);
        ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_HorizontalScrollbar);
        {
            ImGui::Text("Flash progress");
            ImGui::ProgressBar(Test::m_RV ? Test::m_RV->FlashProgress() : 0);

            //int currentPeriod = m_RihardsTest ? m_RihardsTest->Read32(536871532) : 0;

            //ImGui::Text("CFXS::Time::ms = %llu", m_RihardsTest ? m_RihardsTest->ReadMilliseconds() : 0);
            //ImGui::Text("PORT_N_BASE    = 0x%X", m_RihardsTest ? m_RihardsTest->Read32(0x40025004) : 0);
            //ImGui::Text("FLASH_PERIOD");
            //ImGui::SameLine(0, 16);
            //if (ImGui::InputInt("", &currentPeriod, 10, 100, ImGuiInputTextFlags_EnterReturnsTrue)) {
            //    m_RihardsTest->Write32(536871532, currentPeriod);
            //}
        }
        ImGui::End();

        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4{0.05f, 0.05f, 0.05f, 1.0f});
        ImGui::SetNextWindowClass(&window_class);
        ImGui::Begin("Terminal", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_HorizontalScrollbar);
        {
            //ImGui::Text(m_RihardsTest ? m_RihardsTest->GetTerminalText() : "Waiting for debug session");
            //ImGui::SetScrollHereY(1.0f);
        }
        ImGui::End();
        ImGui::PopStyleColor();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0.0f, 0.0f});
        ImGui::SetNextWindowClass(&window_class);
        ImGui::Begin("Symbols", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_HorizontalScrollbar);
        {
            if (ImGui::BeginTable("Symbols", 3, ImGuiTableFlags_Resizable | ImGuiTableFlags_Borders)) {
                ImGui::TableSetupColumn("Symbol Name");
                ImGui::TableSetupColumn("Location");
                ImGui::TableSetupColumn("Data Type");
                ImGui::TableHeadersRow();
                for (int i = 0; i < 64; i++) {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("sym %d", i);
                    ImGui::TableNextColumn();
                    ImGui::Text("0x%08X", i);
                    ImGui::TableNextColumn();
                    ImGui::Text("uint32_t");
                }

                ImGui::EndTable();
            }
        }
        ImGui::End();
        ImGui::PopStyleVar();

        ImGui::SetNextWindowClass(&window_class);
        ImGui::Begin("Workspace", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_HorizontalScrollbar);
        {
            if (ImGui::CollapsingHeader("DigiNet MK2")) {
            }
        }
        ImGui::End();
    }

    void HWD_Application::OnEvent() {
    }

} // namespace HWD