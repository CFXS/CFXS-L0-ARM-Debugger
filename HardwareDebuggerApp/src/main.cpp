// [source]
#include "main.hpp"
#include "Application.hpp"
#include "Log/Log.hpp"

int main(int argc, char** argv) {
    HWD::Log::Initialize();
    HWDLOG_CORE_INFO("Initialized Logging");

    HWDLOG_CORE_INFO("Creating Application");
    auto app = new HWD::Application(argc, argv, "CFXS Hardware Debugger");

    HWDLOG_CORE_INFO("Running Application");
    app->Run();
    delete app;

    return 0;
}