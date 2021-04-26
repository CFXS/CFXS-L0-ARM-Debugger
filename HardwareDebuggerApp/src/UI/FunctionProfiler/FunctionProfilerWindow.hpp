#pragma once
#include <QDialog>

namespace Ui {
    class FunctionProfilerWindow;
}

namespace HWD::UI {

    class FunctionProfilerWindow : public QDialog {
        Q_OBJECT

    public:
        explicit FunctionProfilerWindow(QWidget *parent = nullptr);
        ~FunctionProfilerWindow();

    private:
        Ui::FunctionProfilerWindow *ui;
    };

} // namespace HWD::UI
