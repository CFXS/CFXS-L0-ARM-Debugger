#include "MainWindow.hpp"

#include <QCloseEvent>
#include <QFileDialog>
#include <Debugger/ELF/ELF_Reader.hpp>

namespace HWD::UI {

    MainWindow::MainWindow(QWidget *parent) :
        KDDockWidgets::MainWindow(QStringLiteral(CFXS_HWD_PROGRAM_NAME), KDDockWidgets::MainWindowOption_HasCentralFrame, parent) {
        resize(640, 480);
    }

    MainWindow::~MainWindow() {
    }

    void MainWindow::closeEvent(QCloseEvent *event) {
        emit Closed();
        event->accept();
    }

} // namespace HWD::UI