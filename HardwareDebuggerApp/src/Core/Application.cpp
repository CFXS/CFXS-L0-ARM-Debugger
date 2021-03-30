// [source]
#include "Application.hpp"

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
        m_Running = false;
    }

    void Application::Run() {
        while (m_Running) {
            m_Window->OnUpdate();
        }
    }

} // namespace HWD