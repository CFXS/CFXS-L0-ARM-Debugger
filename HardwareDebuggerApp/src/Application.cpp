// [source]
#include "Application.hpp"

namespace HWD {

    Application::Application(int argc, char** argv) {
        printf("[CFXS Hardware Debugger %s]\n", CFXS_HWD_VERSION_STRING);
        Initialize_SDL();
    }

    void Application::Run() {
        while (m_Running) {
            SDL_Event e;
            while (SDL_PollEvent(&e)) {
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////

    void Application::Initialize_SDL() {
        if (SDL_Init(SDL_INIT_VIDEO) != 0) {
            fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
        }

        m_MainWindow = SDL_CreateWindow("CFXS Hardware Debugger", 100, 100, 620, 387, SDL_WINDOW_SHOWN);
        if (m_MainWindow == NULL) {
            fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
        }
    }

} // namespace HWD