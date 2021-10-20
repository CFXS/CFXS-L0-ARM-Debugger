#include "Application.hpp"

#include <KDDockWidgets/Config.h>

#include <QStyleFactory>
#include <QTimer>
#include <fstream>
#include <streambuf>

#include "UI/Test_StyleSheet.hpp"

namespace HWD {

    Application* Application::s_Instance = nullptr;

    static void SetQtAttributes() {
        QApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
    }

    Application::Application(int argc, char** argv, const std::string& name) {
        HWD_ASSERT(!s_Instance, "Application already exists!");
        s_Instance = this;

        SetQtAttributes();
        m_QtApplication = std::make_unique<QApplication>(argc, argv);
        m_QtApplication->setOrganizationName(QStringLiteral("CFXS"));
        m_QtApplication->setApplicationName(QString::fromStdString(name));
        m_QtApplication->setStyle(QStyleFactory::create(QStringLiteral("Fusion")));

        auto palette = QPalette();
        palette.setColor(QPalette::Window, QColor(53, 53, 53));
        palette.setColor(QPalette::WindowText, Qt::white);
        palette.setColor(QPalette::Base, QColor(33, 33, 33));
        palette.setColor(QPalette::AlternateBase, QColor(44, 44, 44));
        palette.setColor(QPalette::ToolTipBase, Qt::black);
        palette.setColor(QPalette::ToolTipText, Qt::white);
        palette.setColor(QPalette::Text, Qt::white);
        palette.setColor(QPalette::Button, QColor(53, 53, 53));
        palette.setColor(QPalette::ButtonText, Qt::white);
        palette.setColor(QPalette::BrightText, Qt::red);
        palette.setColor(QPalette::Link, QColor(42, 130, 218));
        palette.setColor(QPalette::Highlight, QColor(42, 130, 218));
        palette.setColor(QPalette::HighlightedText, Qt::black);
        m_QtApplication->setPalette(palette);

        //std::ifstream fileStream("C:\\CFXS\\dark.qss", std::ios::in | std::ios::binary);
        //std::vector<uint8_t> ssContent = std::vector<uint8_t>(std::istreambuf_iterator<char>(fileStream), std::istreambuf_iterator<char>());
        //m_QtApplication->setStyleSheet((char*)ssContent.data());

        auto flags = KDDockWidgets::Config::self().flags();
        flags |= KDDockWidgets::Config::Flag_NativeTitleBar;
        flags |= KDDockWidgets::Config::Flag_AllowReorderTabs;
        flags |= KDDockWidgets::Config::Flag_TabsHaveCloseButton;
        flags |= KDDockWidgets::Config::Flag_TitleBarHasMaximizeButton;
        flags |= KDDockWidgets::Config::Flag_AutoHideSupport;

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
            QApplication::processEvents(QEventLoop::AllEvents | QEventLoop::WaitForMoreEvents | QEventLoop::EventLoopExec);
        }

        OnDestroy();
    }

} // namespace HWD