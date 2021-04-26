#include "HWD_Application.hpp"
#include "Log/Log.hpp"

int main(int argc, char** argv) {
    HWD::Log::Initialize();
    HWDLOG_CORE_INFO("Initialized Logging");

    HWDLOG_CORE_INFO("Creating Application");
    auto app = new HWD::HWD_Application(argc, argv);

    app->Run();
    delete app;

    return 0;
}