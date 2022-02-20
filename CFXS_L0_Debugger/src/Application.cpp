// ---------------------------------------------------------------------
// CFXS L0 ARM Debugger <https://github.com/CFXS/CFXS-L0-ARM-Debugger>
// Copyright (C) 2022 | CFXS
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>
// ---------------------------------------------------------------------
// [CFXS] //
#include "Application.hpp"

#include <DockManager.h>
#include <QFile>
#include <QStyleFactory>
#include <QTextStream>
#include <QFontDatabase>

using ads::CDockManager;

namespace L0 {

    Application* Application::s_Instance = nullptr;

    static void SetQtAttributes() {
        QApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
    }

    Application::Application(int argc, char** argv, const std::string& name) {
        L0_ASSERT(!s_Instance, "Application already exists!");
        s_Instance = this;

        SetQtAttributes();
        m_QtApplication = std::make_unique<QApplication>(argc, argv);
        m_QtApplication->setOrganizationName(QStringLiteral("CFXS"));
        m_QtApplication->setApplicationName(QString::fromStdString(name));

        // DockManager disabled flags
        ads::CDockManager::setConfigFlag(ads::CDockManager::DockAreaHasCloseButton, false);
        ads::CDockManager::setConfigFlag(ads::CDockManager::DockAreaHasUndockButton, false);

        // DockManager enabled flags
        ads::CDockManager::setConfigFlag(ads::CDockManager::XmlCompressionEnabled, true);
        ads::CDockManager::setConfigFlag(ads::CDockManager::FocusHighlighting, true);
        ads::CDockManager::setConfigFlag(ads::CDockManager::DragPreviewShowsContentPixmap, true);
        ads::CDockManager::setConfigFlag(ads::CDockManager::AllTabsHaveCloseButton, true);
        ads::CDockManager::setConfigFlag(ads::CDockManager::DockAreaDynamicTabsMenuButtonVisibility, true);
        ads::CDockManager::setConfigFlag(ads::CDockManager::OpaqueSplitterResize, true);
        ads::CDockManager::setConfigFlag(ads::CDockManager::MiddleMouseButtonClosesTab, true);

        QPalette darkPalette;
        darkPalette.setColor(QPalette::Window, QColor(55, 55, 55));
        darkPalette.setColor(QPalette::WindowText, Qt::white);
        darkPalette.setColor(QPalette::Disabled, QPalette::WindowText, QColor(127, 127, 127));
        darkPalette.setColor(QPalette::Base, QColor(33, 33, 33));
        darkPalette.setColor(QPalette::Mid, QColor(66, 66, 66));
        darkPalette.setColor(QPalette::Midlight, QColor(105, 105, 105));
        darkPalette.setColor(QPalette::Light, QColor(164, 164, 164));
        darkPalette.setColor(QPalette::AlternateBase, QColor(66, 66, 66));
        darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
        darkPalette.setColor(QPalette::ToolTipText, Qt::white);
        darkPalette.setColor(QPalette::Text, Qt::white);
        darkPalette.setColor(QPalette::Disabled, QPalette::Text, QColor(127, 127, 127));
        darkPalette.setColor(QPalette::Dark, QColor(44, 44, 44));
        darkPalette.setColor(QPalette::Shadow, QColor(20, 20, 20));
        darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
        darkPalette.setColor(QPalette::ButtonText, Qt::white);
        darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(127, 127, 127));
        darkPalette.setColor(QPalette::BrightText, Qt::red);
        darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
        darkPalette.setColor(QPalette::Highlight, QColor(0, 105, 220));
        darkPalette.setColor(QPalette::Disabled, QPalette::Highlight, QColor(80, 80, 80));
        darkPalette.setColor(QPalette::HighlightedText, Qt::white);
        darkPalette.setColor(QPalette::Disabled, QPalette::HighlightedText, QColor(127, 127, 127));

        QApplication::setPalette(darkPalette);
        QApplication::setStyle(QStyleFactory::create(QStringLiteral("fusion")));

        QFile file(":/Style/Main");
        file.open(QFile::ReadOnly);
        m_QtApplication->setStyleSheet(QLatin1String(file.readAll()));
        file.close();

        m_MainWindow = std::make_unique<UI::MainWindow>();
        m_MainWindow->show();
    }

    Application::~Application() {
    }

    void Application::Run() {
        LOG_CORE_INFO("Running application");
        OnCreate();

        qApp->exec();

        LOG_CORE_INFO("Stopping application");
        OnDestroy();
    }

} // namespace L0