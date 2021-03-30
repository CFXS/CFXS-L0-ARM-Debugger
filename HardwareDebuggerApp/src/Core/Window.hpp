#pragma once

#include <SDL.h>

namespace HWD {

    struct WindowProps {
        std::string Title;
        uint32_t Width;
        uint32_t Height;

        WindowProps(const std::string& title, uint32_t width = 1600, uint32_t height = 900) : Title(title), Width(width), Height(height) {
        }
    };

    class Window {
    public:
        Window(const WindowProps& props);
        virtual ~Window();

        void OnUpdate();

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

    private:
        SDL_Window* m_Window = nullptr;
        WindowProps m_Data;
    };

} // namespace HWD