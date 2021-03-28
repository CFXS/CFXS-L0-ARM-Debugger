// [source]
#include "Application.hpp"

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

namespace HWD {

    Application::Application(int argc, char** argv) {
        printf("[CFXS Hardware Debugger %s]\n", CFXS_HWD_VERSION_STRING);
        printf("[HWD] Application(%d, 0x%p)\n", argc, argv);

        bool sdlInitialized = Initialize_SDL();
        (void)sdlInitialized;
    }

    Application::~Application() {
        printf("[HWD] ~Application()\n");
    }

    void Application::Run() {
        while (m_Running) {
            SDL_Event e;
            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_QUIT) {
                    m_Running = false;
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Application::Initialize_SDL() {
        if (SDL_Init(SDL_INIT_VIDEO) != 0) {
            fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
            return false;
        }

        m_MainWindow = SDL_CreateWindow("CFXS Hardware Debugger", 100, 100, 640, 480, SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);
        if (m_MainWindow == NULL) {
            fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
            return false;
        }

        return true;
    }

} // namespace HWD