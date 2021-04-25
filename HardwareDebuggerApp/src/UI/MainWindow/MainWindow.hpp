#pragma once

#include <QMainWindow>

namespace Ui {
    class MainWindow;
}

namespace HWD::UI {

    class MainWindow : public QMainWindow {
        Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = nullptr);
        ~MainWindow();

    signals:
        void Closed();

    protected:
        void closeEvent(QCloseEvent *event);

    private:
        Ui::MainWindow *ui;
    };

} // namespace HWD::UI