// [source]
#include "main.hpp"
#include "Application.hpp"

int main(int argc, char** argv) {
    auto app = new HWD::Application(argc, argv);

    app->Run();

    return 0;
}