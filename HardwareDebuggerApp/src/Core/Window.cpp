// [source]
#include "Window.hpp"

#include "Application.hpp"

namespace HWD {

    static uint8_t s_SDLWindowCount = 0;

    Window::Window(const WindowProps& props) : m_Data(props) {
        if (!s_SDLWindowCount) {
            HWDLOG_CORE_INFO("Initializing SDL2");
            if (SDL_Init(SDL_INIT_VIDEO)) {
                HWDLOG_CORE_CRITICAL("Failed to initialize SDL2 - {0}", SDL_GetError());
                HWD_DEBUGBREAK();
            }
        }

        HWDLOG_CORE_INFO("Creating window {0} ({1}, {2})", m_Data.Title, m_Data.Width, m_Data.Height);
        m_Window = SDL_CreateWindow(m_Data.Title.c_str(),
                                    SDL_WINDOWPOS_CENTERED,
                                    SDL_WINDOWPOS_CENTERED,
                                    m_Data.Width,
                                    m_Data.Height,
                                    SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
        s_SDLWindowCount++;
        if (!m_Window) {
            HWDLOG_CORE_CRITICAL("Failed to create window - {0}", SDL_GetError());
            HWD_DEBUGBREAK();
        }

        m_Context = std::make_unique<OpenGL_Context>(m_Window);
    }

    Window::~Window() {
        HWDLOG_CORE_INFO("Destroying window {0} ({1}, {2})", m_Data.Title, m_Data.Width, m_Data.Height);
        SDL_DestroyWindow(m_Window);
        s_SDLWindowCount--;

        if (!s_SDLWindowCount) {
            SDL_Quit();
        }
    }

    void Window::OnUpdate() {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            Application::Get().OnSDLEvent(e);
            switch (e.type) {
                case SDL_EventType::SDL_QUIT: Application::Get().Close(); break;
            }
        }
    }

    void Window::SwapBuffers() {
        m_Context->SwapBuffers();
    }

} // namespace HWD