#pragma once
#include "main.hpp"
#include <SDL.h>

namespace HWD {

    class Application {
    public:
        Application(int argc, char** argv);
        ~Application();

        void Run();

    private:
        bool Initialize_SDL();

    private:
        bool m_Running = true;

        SDL_Window* m_MainWindow = nullptr;
    };

} // namespace HWD