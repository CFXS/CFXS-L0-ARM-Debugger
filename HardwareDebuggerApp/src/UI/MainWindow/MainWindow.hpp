#pragma once

#include <KDDockWidgets/MainWindow.h>

namespace HWD::UI {

    class MainWindow : public KDDockWidgets::MainWindow {
        Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = nullptr);
        ~MainWindow();

    signals:
        void Closed();

    protected:
        void closeEvent(QCloseEvent *event);
    };

} // namespace HWD::UI