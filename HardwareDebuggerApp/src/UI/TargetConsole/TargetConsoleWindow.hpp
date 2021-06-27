#pragma once

#include <KDDockWidgets/DockWidget.h>

namespace Ui {
    class TargetConsoleWindow;
}

namespace HWD::UI {

    class TargetConsoleWindow : public KDDockWidgets::DockWidget {
        Q_OBJECT

    public:
        TargetConsoleWindow();
        ~TargetConsoleWindow();

    private:
        std::unique_ptr<Ui::TargetConsoleWindow> ui;
    };

} // namespace HWD::UI
