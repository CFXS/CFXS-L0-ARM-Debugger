#include "FunctionProfilerWindow.hpp"

#include <QTimer>
#include <set>

#include "ui_FunctionProfilerWindow.h"
#include <Debugger/ELF/ELF_Reader.hpp>
#include <QFileDialog>

namespace HWD {

    namespace Probe {
        extern std::map<uint32_t, uint64_t> s_PC_Map;
        extern std::map<uint32_t, uint64_t> s_ExecMap;
    } // namespace Probe
    using namespace Probe;

    // TM4C
    std::map<uint32_t, const char*> s_DecodeEx = {
        {0, "MainLoop"},   //
        {15, "SysTick"},   //
        {16, "GPIO A"},    //
        {21, "UART 0"},    //
        {22, "UART 1"},    //
        {39, "Timer 2A"},  //
        {49, "UART 2"},    //
        {51, "Timer 3A"},  //
        {52, "Timer 3B"},  //
        {72, "UART 3"},    //
        {73, "UART 4"},    //
        {74, "UART 5"},    //
        {76, "UART 7"},    //
        {79, "Timer 4A"},  //
        {80, "Timer 4B"},  //
        {81, "Timer 5A"},  //
        {82, "Timer 5B"},  //
        {114, "Timer 6A"}, //
        {115, "Timer 6B"}, //
        {116, "Timer 7A"}, //
    };

} // namespace HWD

struct comp {
    template<typename T>

    // Comparator function
    bool operator()(const T& l, const T& r) const {
        if (l.second != r.second) {
            return l.second > r.second;
        }
        return l.first > r.first;
    }
};

namespace HWD::UI {

    FunctionProfilerWindow::FunctionProfilerWindow() :
        KDDockWidgets::DockWidget(QStringLiteral("FunctionProfilerWindow"), KDDockWidgets::DockWidgetBase::Option_DeleteOnClose),
        ui(std::make_unique<Ui::FunctionProfilerWindow>()) {
        ui->setupUi(this);

        ui->table_PC->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
        ui->table_PC->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Interactive);
        ui->table_PC->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
        ui->table_PC->verticalHeader()->setDefaultSectionSize(16);

        QString filePath = QFileDialog::getOpenFileName(
            this, "ELF File", R"(X:\CPP\GCC_ARM_Cortex_M\Dev_TM4C1294\build)", ("Executable File (*.elf *.out)"));
        ELF::ELF_Reader* elfReader = new ELF::ELF_Reader(filePath.toStdString());

        setWidget(ui->RootWidget);

        QTimer* rt = new QTimer(this);
        rt->setInterval(33);
        rt->setSingleShot(false);
        QObject::connect(rt, &QTimer::timeout, [=]() {
            if (static_cast<int>(s_PC_Map.size()) != ui->table_PC->rowCount()) {
                ui->table_PC->setRowCount(static_cast<int>(s_PC_Map.size()));
            }

            std::map<std::string, uint64_t> calls;
            for (auto& [pc, callCount] : s_PC_Map) {
                auto sym = elfReader->AddressToSymbol(pc);
                if (sym) {
                    if (calls.find(sym->name) == calls.end()) {
                        calls[sym->name] += callCount;
                    }
                } else {
                    char name[24];
                    snprintf(name, 24, "0x%08X", pc);
                    if (calls.find(name) == calls.end()) {
                        calls[name] += callCount;
                    }
                }
            }

            std::set<std::pair<std::string, uint64_t>, comp> sortedCalls(calls.begin(), calls.end());

            uint64_t totalSamples = 0;
            for (auto& [pc, sampleCount] : sortedCalls) {
                totalSamples += sampleCount;
            }

            int i = 0;
            for (auto& [pc, sampleCount] : sortedCalls) {
                for (int c = 0; c < 3; c++) {
                    QTableWidgetItem* cell = ui->table_PC->item(i, c);
                    if (!cell) {
                        cell = new QTableWidgetItem;
                        ui->table_PC->setItem(i, c, cell);
                    }

                    switch (c) {
                        case 0: {
                            cell->setText(pc.c_str());

                            break;
                        }
                        case 1: {
                            char str[24];
                            snprintf(str, 24, "%llu", sampleCount);
                            cell->setText(str);
                            break;
                        }
                        case 2: {
                            char str[24];
                            snprintf(str, 24, "%.1f%%", 100.0f / totalSamples * sampleCount);
                            cell->setText(str);
                            break;
                        }
                    }
                }
                i++;
            }
        });
        rt->start();
    }

    FunctionProfilerWindow::~FunctionProfilerWindow() {
    }

} // namespace HWD::UI