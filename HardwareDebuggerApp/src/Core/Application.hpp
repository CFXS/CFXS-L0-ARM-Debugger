#pragma once

#include "Window.hpp"

int main(int argc, char** argv);

namespace HWD {

    class Application {
    public:
        Application(int argc, char** argv, const std::string& name = "HWD App");
        virtual ~Application();

        inline Window& GetWindow() {
            return *m_Window;
        }

        void Close();

        static inline Application& Get() {
            return *s_Instance;
        }

    private:
        void Run();

    private:
        std::unique_ptr<Window> m_Window;
        bool m_Running        = true;
        bool m_Minimized      = false;
        float m_LastFrameTime = 0.0f;

    private:
        static Application* s_Instance;
        friend int ::main(int argc, char** argv);
    };

} // namespace HWD