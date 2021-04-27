#include "HWD_Application.hpp"

#include <KDDockWidgets/DockWidget.h>
#include <QStyleFactory>
#include <UI/Test_StyleSheet.hpp>

#include <set>

namespace HWD {

    HWD_Application::HWD_Application(int argc, char** argv) : Application(argc, argv, CFXS_HWD_PROGRAM_NAME) {
    }

    HWD_Application::~HWD_Application() {
    }

    namespace Test {
        extern RVeips_ProbeTest* m_RV;
    }

    void HWD_Application::OnCreate() {
        qApp->setStyle(QStyleFactory::create("Fusion"));
        qApp->setStyleSheet(style_sheet);

#define RIHARDS_TEST
#ifdef RIHARDS_TEST
        (new std::thread([=]() {
            m_RihardsTest = std::make_unique<Test::RVeips_ProbeTest>();
        }))->detach();
#endif

        //auto testDock = new KDDockWidgets::DockWidget(QStringLiteral("TestDockWidget"));
        //GetMainWindow()->addDockWidgetAsTab(testDock);
    }

    void HWD_Application::OnDestroy() {
        HWDLOG_CORE_INFO("Destroying App");
    }

    // This function is called every ms
    void HWD_Application::OnUpdate() {
    }

} // namespace HWD