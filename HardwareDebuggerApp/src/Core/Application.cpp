// [source]
#include "Application.hpp"

#include <Test/RVeips_ProbeTest.hpp>

namespace HWD {

    Application* Application::s_Instance = nullptr;

    Application::Application(int argc, char** argv, const std::string& name) {
        HWD_ASSERT(!s_Instance, "Application already exists!");
        s_Instance = this;

        m_Window = std::make_unique<Window>(WindowProps(name));
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

        while (m_Running) {
            OnUpdate();
            OnImGuiRender();
            Window::OnUpdate();
            m_Window->SwapBuffers();
        }
    }

} // namespace HWD