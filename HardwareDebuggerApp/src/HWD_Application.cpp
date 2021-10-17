#include "HWD_Application.hpp"

#include <KDDockWidgets/DockWidget.h>

#include <Probe/JLink/JLink.hpp>

namespace HWD {

    HWD_Application::HWD_Application(int argc, char** argv) : Application(argc, argv, CFXS_HWD_PROGRAM_NAME) {
    }

    namespace Test {
        extern RVeips_ProbeTest* m_RV;
    }

    void HWD_Application::OnCreate() {
        HWDLOG_CORE_INFO("Loading probes");
        Probe::JLink::HWD_Load();

#ifdef DEV_PC_CFXS
        HWDLOG_CORE_INFO("DEV_PC_CFXS init");
        (new std::thread([=]() {
            m_RihardsTest = std::make_unique<Test::RVeips_ProbeTest>();
        }))->detach();
#endif

        //auto fpWindow = new UI::FunctionProfilerWindow;
        //fpWindow->show();

        //auto tcWindow = new UI::TargetConsoleWindow;
        //tcWindow->show();

        //GetMainWindow()->addDockWidgetAsTab(tcWindow);
        //GetMainWindow()->addDockWidgetAsTab(fpWindow);
    }

    void HWD_Application::OnDestroy() {
        HWDLOG_CORE_INFO("Destroying App");
        Probe::JLink::HWD_Unload();
    }

} // namespace HWD