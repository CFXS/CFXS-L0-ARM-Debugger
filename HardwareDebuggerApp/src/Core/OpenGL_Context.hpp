#pragma once

#include <SDL.h>

namespace HWD {

    class OpenGL_Context {
        friend class Application;

    public:
        OpenGL_Context(SDL_Window* windowHandle);
        void SwapBuffers();

    private:
        SDL_Window* m_WindowHandle;
        SDL_GLContext m_ContextHandle = nullptr;
    };
} // namespace HWD