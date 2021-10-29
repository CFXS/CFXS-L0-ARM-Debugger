// [CFXS] //
#pragma once

#include <QSettings>

namespace HWD::UI {

    class I_Panel {
    public:
        I_Panel()          = default;
        virtual ~I_Panel() = default;

        /// Save state to QSettings instance
        virtual void SavePanelState(QSettings* cfg) = 0;

        /// Load state from QSettings instance
        virtual void LoadPanelState(QSettings* cfg) = 0;
    };

} // namespace HWD::UI
