// [source]
#include "Application.hpp"

#include <backends/imgui_impl_sdl.h>
#include <backends/imgui_impl_opengl3.h>

namespace HWD {

    static void InitializeImGui() {
        //ImGui_ImplSDL2_InitForOpenGL(m_Window->GetNativeWindow(), m_Window->m_Context->m_ContextHandle);
        //ImGui_ImplOpenGL3_Init("#version 450");
    }

    Application* Application::s_Instance = nullptr;

    Application::Application(int argc, char** argv, const std::string& name) {
        HWD_ASSERT(!s_Instance, "Application already exists!");
        s_Instance = this;

        m_Window = std::make_unique<Window>(WindowProps(name));

        InitializeImGui();
    }

    Application::~Application() {
    }

    void Application::Close() {
        HWDLOG_CORE_INFO("Closing application");
        m_Running = false;
        OnDestroy();
    }

    void Application::Run() {
        HWDLOG_CORE_INFO("Running application");
        OnCreate();

        /*        static Uint64 frequency = SDL_GetPerformanceFrequency();
        Uint64 current_time     = SDL_GetPerformanceCounter();
        ((current_time - g_Time) / frequency);*/

        while (m_Running) {
            OnUpdate();
            OnImGuiRender();
            Window::OnUpdate();
            m_Window->SwapBuffers();
        }
    }

} // namespace HWD