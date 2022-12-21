// ---------------------------------------------------------------------
// CFXS L0 ARM Debugger <https://github.com/CFXS/CFXS-L0-ARM-Debugger>
// Copyright (C) 2022 | CFXS
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>
// ---------------------------------------------------------------------
// [CFXS] //
#pragma once

#include <QApplication>

#include "UI/Windows/MainWindow/MainWindow.hpp"

int main(int argc, char** argv);

namespace L0 {

    class Application {
    public:
        Application(int argc, char** argv, const std::string& name = "App");
        virtual ~Application();

        virtual void OnCreate()  = 0;
        virtual void EarlyLoad() = 0;
        virtual void OnDestroy() = 0;

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
        std::unique_ptr<UI::MainWindow> m_MainWindow;

    private:
        static Application* s_Instance;
        friend int ::main(int argc, char** argv);
    };

} // namespace L0