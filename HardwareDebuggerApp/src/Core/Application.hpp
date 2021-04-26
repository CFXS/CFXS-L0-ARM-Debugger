#pragma once

#include <QApplication>
#include "UI/MainWindow/MainWindow.hpp"
#include "UI/FunctionProfiler/FunctionProfilerWindow.hpp"

int main(int argc, char** argv);

namespace HWD {

    class Application {
    public:
        Application(int argc, char** argv, const std::string& name = "App");
        virtual ~Application();

        virtual void OnCreate()  = 0;
        virtual void OnDestroy() = 0;
        virtual void OnUpdate()  = 0;

        void Close();

        inline UI::MainWindow* GetMainWindow() {
            return m_MainWindow.get();
        }

    public:
        static inline Application& Get() {
            return *s_Instance;
        }

    private:
        void Run();

    private:
        std::unique_ptr<QApplication> m_QtApplication;
        std::unique_ptr<QTimer> m_UpdateTimer;
        std::unique_ptr<UI::MainWindow> m_MainWindow;
        bool m_Running        = true;
        bool m_Minimized      = false;
        float m_LastFrameTime = 0.0f;

    private:
        static Application* s_Instance;
        friend int ::main(int argc, char** argv);
    };

} // namespace HWD