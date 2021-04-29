#pragma once

#include <KDDockWidgets/DockWidget.h>

namespace Ui {
    class FunctionProfilerWindow;
}

namespace HWD::UI {

    class FunctionProfilerWindow : public KDDockWidgets::DockWidget {
        Q_OBJECT

    public:
        FunctionProfilerWindow();
        ~FunctionProfilerWindow();

    private:
        std::unique_ptr<Ui::FunctionProfilerWindow> ui;
    };

} // namespace HWD::UI
