#include "OpenGL_Context.hpp"

#include <GL/glew.h>

namespace HWD {

    OpenGL_Context::OpenGL_Context(SDL_Window* windowHandle) : m_WindowHandle(windowHandle) {
        HWDLOG_CORE_INFO("Creating OpenGL context");
        m_ContextHandle = SDL_GL_CreateContext(m_WindowHandle);
        if (!m_ContextHandle) {
            HWDLOG_CORE_CRITICAL("Failed to create OpenGL context");
            HWD_DEBUGBREAK();
        }
        SDL_GL_MakeCurrent(m_WindowHandle, m_ContextHandle);

        HWDLOG_CORE_INFO("Initializing GLEW");
        glewExperimental = GL_TRUE;
        auto ec          = glewInit();
        if (ec != GLEW_OK) {
            HWDLOG_CORE_ERROR("Failed to initialize GLEW - {0}", glewGetErrorString(ec));
            HWD_DEBUGBREAK();
        }

        SDL_GL_SetSwapInterval(-1);

        HWDLOG_CORE_INFO("OpenGL version: {0}", glGetString(GL_VERSION));
    }

    void OpenGL_Context::SwapBuffers() {
        SDL_GL_SwapWindow(m_WindowHandle);
    }

    void OpenGL_Context::MakeContextCurrent() {
        SDL_GL_MakeCurrent(m_WindowHandle, m_ContextHandle);
    }

} // namespace HWD