#include "Application.hpp"

#include <DockManager.h>
#include <QFile>
#include <QStyleFactory>

using ads::CDockManager;

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

        ads::CDockManager::setConfigFlag(ads::CDockManager::FocusHighlighting);
        ads::CDockManager::setConfigFlag(ads::CDockManager::AllTabsHaveCloseButton);
        ads::CDockManager::setConfigFlag(ads::CDockManager::DockAreaDynamicTabsMenuButtonVisibility);
        ads::CDockManager::setConfigFlag(ads::CDockManager::OpaqueSplitterResize);
        ads::CDockManager::setConfigFlag(ads::CDockManager::MiddleMouseButtonClosesTab);

        m_QtApplication->setStyle(QStyleFactory::create(QStringLiteral("fusion")));

        QPalette darkPalette;
        darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
        darkPalette.setColor(QPalette::WindowText, Qt::white);
        darkPalette.setColor(QPalette::Disabled, QPalette::WindowText, QColor(127, 127, 127));
        darkPalette.setColor(QPalette::Base, QColor(42, 42, 42));
        darkPalette.setColor(QPalette::AlternateBase, QColor(66, 66, 66));
        darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
        darkPalette.setColor(QPalette::ToolTipText, Qt::white);
        darkPalette.setColor(QPalette::Text, Qt::white);
        darkPalette.setColor(QPalette::Disabled, QPalette::Text, QColor(127, 127, 127));
        darkPalette.setColor(QPalette::Dark, QColor(35, 35, 35));
        darkPalette.setColor(QPalette::Shadow, QColor(20, 20, 20));
        darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
        darkPalette.setColor(QPalette::ButtonText, Qt::white);
        darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(127, 127, 127));
        darkPalette.setColor(QPalette::BrightText, Qt::red);
        darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
        darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
        darkPalette.setColor(QPalette::Disabled, QPalette::Highlight, QColor(80, 80, 80));
        darkPalette.setColor(QPalette::HighlightedText, Qt::white);
        darkPalette.setColor(QPalette::Disabled, QPalette::HighlightedText, QColor(127, 127, 127));

        m_QtApplication->setPalette(darkPalette);

        QFile styleFile(":/Style/DarkStyle.qss");
        styleFile.open(QFile::ReadOnly);
        m_QtApplication->setStyleSheet(styleFile.readAll());

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