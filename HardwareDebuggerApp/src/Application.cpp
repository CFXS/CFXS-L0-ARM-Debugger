// [source]
#include "Application.hpp"

#include "Test/RVeips_ProbeTest.hpp"

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include "main.hpp"

namespace HWD {

    Application* Application::s_Instance = nullptr;

    Application::Application(int argc, char** argv, const std::string& name) {
        HWD_ASSERT(!s_Instance, "Application already exists!");
        s_Instance = this;

        //m_Window = Window::Create(WindowProps(name));
        //m_Window->SetEventCallback(HZ_BIND_EVENT_FN(Application::OnEvent));
    }

    Application::~Application() {
    }

    void Application::Close() {
        m_Running = false;
    }

    void Application::Run() {
        Test::RVeips_ProbeTest::Run();

        while (m_Running) {
            //float time        = (float)glfwGetTime();
            //Timestep timestep = time - m_LastFrameTime;
            //m_LastFrameTime   = time;

            //for (Layer* layer : m_LayerStack)
            //    layer->OnUpdate(timestep);

            if (!m_Minimized) {
                //m_ImGuiLayer->Begin();

                //for (Layer* layer : m_LayerStack)
                //    layer->OnImGuiRender();
                //m_ImGuiLayer->End();
            }

            //m_Window->OnUpdate();
        }
    }

} // namespace HWD