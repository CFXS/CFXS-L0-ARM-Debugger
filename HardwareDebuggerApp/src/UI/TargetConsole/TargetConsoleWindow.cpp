#include "TargetConsoleWindow.hpp"

#include <QTimer>
#include <set>

#include "ui_TargetConsoleWindow.h"
#include <Debugger/ELF/ELF_Reader.hpp>
#include <QFileDialog>
#include <QScrollBar>

namespace HWD {

    namespace Probe {
        extern char* s_Console;
        extern bool s_ConsoleUpdated;
    } // namespace Probe
    using namespace Probe;

} // namespace HWD

namespace HWD::UI {

    TargetConsoleWindow::TargetConsoleWindow() :
        KDDockWidgets::DockWidget(QStringLiteral("TargetConsoleWindow"), KDDockWidgets::DockWidgetBase::Option_DeleteOnClose),
        ui(std::make_unique<Ui::TargetConsoleWindow>()) {
        ui->setupUi(this);

        ui->text_Console->setStyleSheet("background-color: #111111;");

        setWidget(ui->RootWidget);

        QTimer* rt = new QTimer(this);
        rt->setInterval(33);
        rt->setSingleShot(false);
        QObject::connect(rt, &QTimer::timeout, [=]() {
            if (s_ConsoleUpdated) {
                s_ConsoleUpdated = false;
                bool atMax       = ui->text_Console->verticalScrollBar()->value() == ui->text_Console->verticalScrollBar()->maximum();
                auto current     = ui->text_Console->verticalScrollBar()->value();

                ui->text_Console->setPlainText(s_Console);

                if (atMax) {
                    ui->text_Console->verticalScrollBar()->setValue(ui->text_Console->verticalScrollBar()->maximum());
                } else {
                    ui->text_Console->verticalScrollBar()->setValue(current);
                }
            }
        });
        rt->start();
    }

    TargetConsoleWindow::~TargetConsoleWindow() {
    }

} // namespace HWD::UI