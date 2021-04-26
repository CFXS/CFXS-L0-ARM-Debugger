#include "FunctionProfilerWindow.hpp"
#include "ui_FunctionProfilerWindow.h"

namespace HWD::UI {

    FunctionProfilerWindow::FunctionProfilerWindow(QWidget *parent) : QDialog(parent), ui(new Ui::FunctionProfilerWindow) {
        ui->setupUi(this);

        setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
        setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);

        ui->table_PC->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
        ui->table_PC->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Interactive);
        ui->table_PC->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    }

    FunctionProfilerWindow::~FunctionProfilerWindow() {
        delete ui;
    }

} // namespace HWD::UI