// [source]
#include "Application.hpp"

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include "main.hpp"

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
        lua_State* ls = luaL_newstate();

        int r = luaL_dostring(ls, "a = 0xAABBCCDDEEFF1122");
        lua_getglobal(ls, "a");

        printf("Lua a = 0x%llx\n", (long long)lua_tointeger(ls, -1));

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

        m_MainWindow = SDL_CreateWindow("CFXS Hardware Debugger",
                                        SDL_WINDOWPOS_CENTERED,
                                        SDL_WINDOWPOS_CENTERED,
                                        640,
                                        480,
                                        SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
        if (m_MainWindow == NULL) {
            fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
            return false;
        }

        return true;
    }

} // namespace HWD