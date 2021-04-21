// [source]
#include "HWD_Application.hpp"

#include <GL/glew.h>
#include <imgui.h>
#include <imgui_internal.h>

#include <set>

#include "main.hpp"

namespace HWD {
    namespace Probe {
        extern std::map<uint32_t, uint64_t> s_PC_Map;
        extern std::map<uint32_t, uint64_t> s_ExecMap;
    } // namespace Probe
    using namespace Probe;

    // TM4C
    std::map<uint32_t, const char*> s_DecodeEx = {
        {0, "MainLoop"},   //
        {15, "SysTick"},   //
        {16, "GPIO A"},    //
        {21, "UART 0"},    //
        {22, "UART 1"},    //
        {39, "Timer 2A"},  //
        {49, "UART 2"},    //
        {51, "Timer 3A"},  //
        {52, "Timer 3B"},  //
        {72, "UART 3"},    //
        {73, "UART 4"},    //
        {74, "UART 5"},    //
        {76, "UART 7"},    //
        {79, "Timer 4A"},  //
        {80, "Timer 4B"},  //
        {81, "Timer 5A"},  //
        {82, "Timer 5B"},  //
        {114, "Timer 6A"}, //
        {115, "Timer 6B"}, //
        {116, "Timer 7A"}, //
    };

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
        //font_OS  = io.Fonts->AddFontFromFileTTF("C:/CFXS/OpenSans-Regular.ttf", 24);
        font_SCP = io.Fonts->AddFontFromFileTTF("C:/CFXS/SourceCodePro-Regular.ttf", 24);

        io.ConfigViewportsNoDecoration = false;

        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4{0.125f, 0.125f, 0.125f, 1.0f});
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

    struct comp {
        template<typename T>

        // Comparator function
        bool operator()(const T& l, const T& r) const {
            if (l.second != r.second) {
                return l.second > r.second;
            }
            return l.first > r.first;
        }
    };

    float val[4] = {0, 0, 0, 0};
    void HWD_Application::OnImGuiRender() {
        ImGuiWindowClass window_class;
        // flags to remove dropdown at left side of docked tab bar
        window_class.DockNodeFlagsOverrideSet   = ImGuiDockNodeFlags_NoWindowMenuButton;
        window_class.DockNodeFlagsOverrideClear = ImGuiDockNodeFlags_NoTabBar;

        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                ImGui::MenuItem("Exit");
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Flash")) {
                ImGui::MenuItem("Select file");
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

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
            ImGui::Text(m_RihardsTest ? m_RihardsTest->GetTerminalText() : "Waiting for debug session");
            ImGui::SetScrollHereY(1.0f);
        }
        ImGui::End();
        ImGui::PopStyleColor();

        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4{0.1f, 0.1f, 0.1f, 1.0f});
        ImGui::SetNextWindowClass(&window_class);
        ImGui::Begin("main.cpp", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_HorizontalScrollbar);
        {
            ImGui::PushFont(font_SCP);
            ImGui::Text(R"(// [source]
#include <CFXS/Base/Task.hpp>

using CFXS::Task;

static constexpr uint32_t TASK_MAINLOOP = 0;

const size_t CFXS::CPU::CLOCK_FREQ = 120e6;
CFXS::Time_t CFXS::Time::ms        = 0;

////////////////////////////////////////////////////////////////////////////////////////////////////////

static constexpr uint32_t Ports[] = {GPIO_PORTN_BASE, GPIO_PORTN_BASE, GPIO_PORTF_BASE, GPIO_PORTF_BASE};
static constexpr uint32_t Pins[]  = {GPIO_PIN_1, GPIO_PIN_0, GPIO_PIN_4, GPIO_PIN_0};
#define LED(id, state) GPIOPinWrite(Ports[id], Pins[id], (state) ? Pins[id] : 0)
#define TOGGLE_LED(id) GPIOPinWrite(Ports[id], Pins[id], !GPIOPinRead(Ports[id], Pins[id]) ? Pins[id] : 0)

volatile __attribute__((used)) uint32_t FLASH_PERIOD = 500;

void InitializeSysTick() {
    CFXS_println(" - Initialize SysTick");

    ROM_SysTickEnable();
    ROM_SysTickPeriodSet(CFXS::CPU::CLOCK_FREQ / 1000); // 1ms
    SysTickIntRegister([]() {
        CFXS::Time::ms++;
        LED(3, (CFXS::Time::ms / FLASH_PERIOD) & 1);
    });
    ROM_SysTickIntEnable();
}

void InitializeLEDs() {
    CFXS_println(" - Initialize LEDs");

    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    while (!ROM_SysCtlPeripheralReady(SYSCTL_PERIPH_GPION) || !ROM_SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF)) {}
    for (int i = 0; i < 4; i++) {
        ROM_GPIOPinTypeGPIOOutput(Ports[i], Pins[i]);
        LED(i, 0);
    }

    Task::Create(
        TASK_MAINLOOP,
        [](auto) {
            TOGGLE_LED(0);
            CFXS_println("Toggle LED - Timestamp: %llu", CFXS::Time::ms);
        },
        500)
        ->Start();
}

int main() {
    CFXS_println("[TestProject] main");

    Task::AddGroup(TASK_MAINLOOP, 64);

    InitializeSysTick();
    InitializeLEDs();

    Task::EnableProcessing();
    while (1) {
        Task::ProcessGroup(TASK_MAINLOOP);
    }
})");
            ImGui::PopFont();
        }
        ImGui::End();
        ImGui::PopStyleColor();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0.0f, 0.0f});
        ImGui::SetNextWindowClass(&window_class);
        ImGui::Begin("PC Samples", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_HorizontalScrollbar);
        {
            if (ImGui::BeginTable("Function Profiler", 3, ImGuiTableFlags_Resizable | ImGuiTableFlags_Borders)) {
                ImGui::TableSetupColumn("PC");
                ImGui::TableSetupColumn("Samples");
                ImGui::TableSetupColumn("Samples/Total %");
                ImGui::TableHeadersRow();

                uint64_t tot = 0;
                for (auto [pc, count] : s_PC_Map) {
                    tot += count;
                }

                std::set<std::pair<uint32_t, uint64_t>, comp> sortedCalls(s_PC_Map.begin(), s_PC_Map.end());

                for (auto [pc, count] : sortedCalls) {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("0x%08X", pc);
                    ImGui::TableNextColumn();
                    ImGui::Text("%llu", count);
                    ImGui::TableNextColumn();
                    ImGui::Text("%.3f%%", 100.0f / tot * count);
                }

                ImGui::EndTable();
            }
        }
        ImGui::End();

        ImGui::Begin("Exception Log", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_HorizontalScrollbar);
        {
            if (ImGui::BeginTable("Interrupt Entry Log", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_Borders)) {
                ImGui::TableSetupColumn("Interrupt Name [ID]");
                ImGui::TableSetupColumn("Times Entered");
                ImGui::TableHeadersRow();

                std::set<std::pair<uint32_t, uint64_t>, comp> sortedExecs(s_ExecMap.begin(), s_ExecMap.end());
                for (auto [num, count] : sortedExecs) {
                    if (num) {
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        ImGui::Text("%s [%u]", s_DecodeEx[num], num);
                        ImGui::TableNextColumn();
                        ImGui::Text("%llu", count / 2);
                    }
                }

                ImGui::EndTable();
            }
        }
        ImGui::End();
        ImGui::PopStyleVar();

        ImGui::SetNextWindowClass(&window_class);
        ImGui::Begin("Workspace", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_HorizontalScrollbar);
        {
            if (ImGui::CollapsingHeader("TestProject")) {
            }
        }
        ImGui::End();
    }

    void HWD_Application::OnEvent() {
    }

} // namespace HWD