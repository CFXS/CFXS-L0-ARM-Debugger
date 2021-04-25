// [source]
#include "MainWindow.hpp"
#include "ui_MainWindow.h"

#include <QCloseEvent>

namespace HWD::UI {

    MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
        ui->setupUi(this);
    }

    MainWindow::~MainWindow() {
        delete ui;
    }

    void MainWindow::closeEvent(QCloseEvent *event) {
        emit Closed();
        event->accept();
    }

} // namespace HWD::UI