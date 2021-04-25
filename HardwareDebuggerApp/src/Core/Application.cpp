// [source]
#include "Application.hpp"

namespace HWD {

    Application* Application::s_Instance = nullptr;

    Application::Application(int argc, char** argv, const std::string& name) {
        HWD_ASSERT(!s_Instance, "Application already exists!");
        s_Instance = this;

        m_QtApplication = std::make_unique<QApplication>(argc, argv);

        m_MainWindow = std::make_unique<UI::MainWindow>();
        QObject::connect(m_MainWindow.get(), &UI::MainWindow::Closed, [&]() {
            Close();
        });
        m_MainWindow->show();
    }

    Application::~Application() {
    }

    void Application::Close() {
        HWDLOG_CORE_INFO("Closing application");
        m_Running = false;
    }

    void Application::Run() {
        HWDLOG_CORE_INFO("Running application");
        OnCreate();

        while (m_Running) {
            OnUpdate();
            QApplication::processEvents();
        }

        OnDestroy();
    }

} // namespace HWD