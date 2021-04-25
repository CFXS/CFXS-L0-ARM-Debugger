#pragma once

#include <SDL.h>
#include "OpenGL_Context.hpp"

namespace HWD {

    struct WindowProps {
        std::string Title;
        uint32_t Width;
        uint32_t Height;

        WindowProps(const std::string& title, uint32_t width = 1600, uint32_t height = 900) : Title(title), Width(width), Height(height) {
        }
    };

    class Window {
        friend class Application;

    public:
        Window(const WindowProps& props);
        virtual ~Window();

        static void OnUpdate();
        void SwapBuffers();

        inline const std::string& WindowTitle() const {
            return m_Data.Title;
        }

        inline uint32_t GetWidth() const {
            return m_Data.Width;
        }

        inline uint32_t GetHeight() const {
            return m_Data.Height;
        }

        inline SDL_Window* GetNativeWindow() const {
            return m_Window;
        }

        inline void MakeContextCurrent() {
            m_Context->MakeContextCurrent();
        }

    private:
        WindowProps m_Data;
        SDL_Window* m_Window = nullptr;
        std::unique_ptr<OpenGL_Context> m_Context;
    };

} // namespace HWD