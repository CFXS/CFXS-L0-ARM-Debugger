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
        Ui::FunctionProfilerWindow *ui;
    };

} // namespace HWD::UI
