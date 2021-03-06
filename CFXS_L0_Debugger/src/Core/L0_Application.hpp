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

#include <QSettings>

#include <Application.hpp>

namespace L0 {

    class L0_Application : public Application {
    public:
        L0_Application(int argc, char** argv);
        virtual ~L0_Application() = default;

        void EarlyLoad() override;
        void OnCreate() override;
        void OnDestroy() override;

    private:
        void LoadGlobalSettings(); // load user settings
        void LoadWindowState();    // load state of window and panels

        void Load_Probe();   // load probe stuff
        void Unload_Probe(); // unload probe stuff

        void Init_Window(); // initialize window stuff

    private:
        static void SaveWindowState(QSettings* stateData);
    };

} // namespace L0