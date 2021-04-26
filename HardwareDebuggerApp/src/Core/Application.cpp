// [source]
#include "Application.hpp"

#include <KDDockWidgets/Config.h>

#include <QStyleFactory>

namespace HWD {

    Application* Application::s_Instance = nullptr;

    static void SetQtAttributes() {
        QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
        QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    }

    Application::Application(int argc, char** argv, const std::string& name) {
        HWD_ASSERT(!s_Instance, "Application already exists!");
        s_Instance = this;

        SetQtAttributes();
        m_QtApplication = std::make_unique<QApplication>(argc, argv);
        m_QtApplication->setOrganizationName(QStringLiteral("CFXS"));
        m_QtApplication->setApplicationName(QStringLiteral(CFXS_HWD_PROGRAM_NAME));
        m_QtApplication->setStyle(QStyleFactory::create(QStringLiteral("Fusion")));

        auto flags = KDDockWidgets::Config::self().flags();
        flags |= KDDockWidgets::Config::Flag_NativeTitleBar;
        flags |= KDDockWidgets::Config::Flag_AlwaysShowTabs;
        flags |= KDDockWidgets::Config::Flag_AllowReorderTabs;
        flags |= KDDockWidgets::Config::Flag_TabsHaveCloseButton;
        flags |= KDDockWidgets::Config::Flag_TitleBarHasMaximizeButton;
        flags |= KDDockWidgets::Config::Flag_AutoHideSupport;
        flags |= KDDockWidgets::Config::Flag_CloseOnlyCurrentTab;
        KDDockWidgets::Config::self().setFlags(flags);

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