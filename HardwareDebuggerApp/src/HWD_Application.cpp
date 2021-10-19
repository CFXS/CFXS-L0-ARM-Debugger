#include "HWD_Application.hpp"

#include <KDDockWidgets/DockWidget.h>

#include <Probe/JLink/JLink.hpp>

#include <UI/ProjectFileBrowser/ProjectFileBrowser.hpp>

namespace HWD {

    HWD_Application::HWD_Application(int argc, char** argv) : Application(argc, argv, CFXS_HWD_PROGRAM_NAME) {
    }

    namespace Test {
        extern RVeips_ProbeTest* m_RV;
    }

    void HWD_Application::OnCreate() {
        HWDLOG_CORE_INFO("Loading probes");
        Probe::JLink::HWD_Load();

#ifdef DEV_PC_CFXS_n
        HWDLOG_CORE_INFO("DEV_PC_CFXS init");
        (new std::thread([=]() {
            m_RihardsTest = std::make_unique<Test::RVeips_ProbeTest>();
        }))->detach();
#endif

        //auto fpWindow = new UI::FunctionProfilerWindow;
        //fpWindow->show();
        //GetMainWindow()->addDockWidgetAsTab(fpWindow);

        auto pfbWindow = new UI::ProjectFileBrowser;
        pfbWindow->show();
        GetMainWindow()->addDockWidgetAsTab(pfbWindow);
    }

    void HWD_Application::OnDestroy() {
        HWDLOG_CORE_INFO("Destroying App");
        Probe::JLink::HWD_Unload();
    }

} // namespace HWD