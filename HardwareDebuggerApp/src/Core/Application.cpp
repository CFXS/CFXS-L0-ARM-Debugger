// [source]
#include "Application.hpp"

#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_sdl.h>
#include <imgui.h>

namespace HWD {

    static void InitializeImGui(SDL_Window* window, SDL_GLContext context) {
        /*std::regex gl_verion_regex("OpenGL version: (\\d).(\\d).(\\d)");
        std::smatch matcher;
        std::string gl_vers = "#version ";

        if (std::regex_search("INPUT STRING", matcher, gl_verion_regex)) {
            gl_vers += matcher[1].str() + matcher[2].str() + matcher[3].str();
        }*/

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;   // Enable Multi-Viewport / Platform Windows
        //io.ConfigViewportsNoAutoMerge = true;
        //io.ConfigViewportsNoTaskBarIcon = true;

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsClassic();

        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            style.WindowRounding              = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        ImGui_ImplSDL2_InitForOpenGL(window, context);
        ImGui_ImplOpenGL3_Init("#version 400");
    }

    Application* Application::s_Instance = nullptr;

    Application::Application(int argc, char** argv, const std::string& name) {
        HWD_ASSERT(!s_Instance, "Application already exists!");
        s_Instance = this;

        m_Window = std::make_unique<Window>(WindowProps(name));

        InitializeImGui(m_Window->GetNativeWindow(), m_Window->m_Context->m_ContextHandle);
    }

    Application::~Application() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
    }

    void Application::Close() {
        HWDLOG_CORE_INFO("Closing application");
        m_Running = false;
    }

    void Application::Run() {
        HWDLOG_CORE_INFO("Running application");
        OnCreate();

        /*        static Uint64 frequency = SDL_GetPerformanceFrequency();
        Uint64 current_time     = SDL_GetPerformanceCounter();
        ((current_time - g_Time) / frequency);*/

        while (m_Running) {
            OnUpdate();

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplSDL2_NewFrame(m_Window->GetNativeWindow());
            ImGui::NewFrame();

            OnImGuiRender();

            ImGuiIO& io    = ImGui::GetIO();
            io.DisplaySize = ImVec2(static_cast<float>(m_Window->GetWidth()), static_cast<float>(m_Window->GetHeight()));
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
                m_Window->MakeContextCurrent();
            }

            Window::OnUpdate();
            m_Window->SwapBuffers();
        }

        OnDestroy();
    }

    void Application::OnSDLEvent(SDL_Event& e) {
        ImGui_ImplSDL2_ProcessEvent(&e);
    }

} // namespace HWD