#pragma once

static const char* style_sheet = R"(QCheckBox::indicator {
    border-style: solid;
    border-width: 1px;
    border-color: rgb(128, 128, 128);
    background-color: rgb(45, 45, 45);
}
QCheckBox::indicator:enabled {
    border-color: rgb(0, 130, 255);
}
QCheckBox::indicator:checked {
    background-color: rgb(0, 100, 220);
}
QCheckBox::indicator:unchecked {
    background-color: rgb(45, 45, 45);
}
QComboBox {
    border-style: solid;
    border-color: rgb(0, 130, 255);
    border-width: 1px;
    background: none;
    background-color: rgb(45, 45, 45);
    border-image: none;
}
QComboBox:disabled {
    background-color: rgb(55, 55, 55);
    border-color: 1px solid gray;
}
QComboBox::down-arrow {
    padding: 3px 3px 3px 3px;
}
QComboBox::drop-down {
    subcontrol-origin: padding;
    subcontrol-position: top right;
    width: 20px;
    border: none;
}
QLineEdit {
    border-style: solid;
    border-color: rgb(0, 130, 255);
    border-width: 1px;
    selection-background-color: rgb(100, 100, 100);
 }
 QLineEdit:disabled {
    background-color: rgb(55, 55, 55);
    border-color: 1px solid gray;
 }
 QTextEdit {
   background-color: rgb(45, 45, 45);
   border-style: solid;
   border-color: rgb(0, 130, 255);
   border-width: 1px;
   selection-background-color: rgb(100, 100, 100);
}
QTextEdit:disabled {
   background-color: rgb(55, 55, 55);
   border-color: 1px solid gray;
}
QListWidget {
    background-color: rgb(50, 50, 50);
    border-color: rgb(44, 44, 44);
    border-style: solid;
    border-width: 2px;
    font-size: 12px;
    padding: 0px;
}
QListWidget::item {
    background-color: rgb(50, 50, 50);
    border: none;
    color: rgb(255, 255, 255);
    padding: 2px;
    padding-left: 6px;
}
QListWidget::item:selected {
    background-color: rgb(35, 35, 35);
    border-left-color: rgb(0, 130, 255);
    border-left-style: solid;
    border-left-width: 3px;
    color: rgb(255, 255, 255);
    padding-left: 3px;
}
*{
    background-color: rgb(55, 55, 55);
    alternate-background-color: rgb(55, 55, 55);
    color: rgb(255, 255, 255);
    font-family: Verdana, Geneva, Tahoma, sans-serif;
}
*:disabled {
    color: rgb(128, 128, 128);
}
QToolTip {
    background-color: rgb(55, 55, 55);
    border-width: 2px;
    border-style: solid;
    border-color: rgb(70, 70, 70);
}
QSlider::handle:horizontal {
    background: rgb(72, 72, 72);
    border-color: rgb(96, 96, 96);
    border-width: 1px;
    border-style: solid;
    border-radius: 2px;
}
QSlider::handle:horizontal:hover {
    background: rgb(102, 102, 102);
    border-color: rgb(126, 126, 126);
}
QSlider::sub-page:horizontal {
    background: rgb(0, 130, 230);
}
QSlider::add-page:horizontal {
    background: rgb(44, 44, 44);
}
QMenuBar::item {
    background: transparent;
    padding: 4px 10px;
    spacing: 3px;
}
QMenuBar::item:selected { /* when selected using mouse or keyboard */
    background: rgb(110, 110, 110);
}
QMenuBar::item:pressed { background: rgb(50, 50, 50); }
QMenu::item:selected { /* when selected using mouse or keyboard */
    background: rgb(110, 110, 110);
}
QMenu { border: 1px solid black; }
QPushButton {
    background-color: rgb(90, 90, 90);
    border-color: rgb(44, 44, 44);
    border-style: outset;
    border-width: 2px;
    padding: 3px 3px 3px 3px;
}
QPushButton:disabled {
    background-color: rgb(65, 65, 65);
    border-color: rgb(50, 50, 50);
    color: rgb(128, 128, 128);
}
QPushButton:pressed {
    border-style: inset;
    background-color: rgb(80, 80, 80);
}
QPushButton:hover {
    background-color: rgb(115, 115, 115);
}
QSpinBox {
    selection-background-color: rgb(100, 100, 100);
    background: none;
    border-image: none;
    border-style:solid;
    border-color: rgb(0, 130, 255);
    border-width: 1px;
    background-color: rgb(45, 45, 45);
}
QSpinBox:disabled {
    background-color: rgb(55, 55, 55);
    border-color: 1px solid gray;
}
QSpinBox::down-button {
    padding: 1px, 1px, 1px, 1px;
    background: rgb(70, 70, 70);
}
QSpinBox::up-button {
    padding: 1px, 1px, 1px, 1px;
    background: rgb(70, 70, 70);
}
QSpinBox::down-button:hover {
    background: rgb(100, 100, 100);
}
QSpinBox::up-button:hover {
    background: rgb(100, 100, 100);
}
QScrollBar {
    background-color: rgb(70, 70, 70);
}
QScrollBar:vertical {
    width: 16px;
}
QScrollBar:horizontal {
    height: 16px;
}
QScrollBar::handle:horizontal {
    background-color: rgb(90, 90, 90);
    border-color: rgb(120, 120, 120);
    border-style: solid;
    border-width: 1px;
    margin-left: 17px;
    margin-right: 17px;
    image: url(":/img/theme/Scroll_MID_H.png");
}
QScrollBar::handle:vertical {
    background-color: rgb(90, 90, 90);
    border-color: rgb(120, 120, 120);
    border-style: solid;
    border-width: 1px;
    margin-top: 17px;
    margin-bottom: 17px;
}
QScrollBar::add-line, QScrollBar::sub-line {
    background-color: rgb(90, 90, 90);
    border-style: solid;
    border-color: rgb(120, 120, 120);
    border-width: 1px;
}
QTabWidget::pane {
    border-color: rgb(44, 44, 44);
    border-style: inset;
    border-width: 1px;
    border-top-width: 3px;
    border-top-color: rgb(0, 130, 230);
}
QTabBar::tab {
    background-color: rgb(80, 80, 80);
    border-color: rgb(44, 44, 44);
    border-style: solid;
    border-width: 2px;
    border-bottom: none;
    font-size: 14px;
    height: 24px;
    padding-left: 8px;
    padding-right: 8px;
    border-top-style: solid;
    border-left-style: solid;
    border-right-style: solid;
    border-bottom-style: none;
}
QTabBar::tab:selected {
    background-color: rgb(0, 130, 230);
    border-color: rgb(0, 130, 230);
    border-top-style: solid;
    border-left-style: solid;
    border-right-style: solid;
    border-bottom-style: solid;
}
QTabBar::tab:hover:!selected {
    background-color: rgb(100, 100, 100);
    border-bottom-color: rgb(0, 130, 230);
}
QTableWidget {
    background-color: rgb(50, 50, 50);
    color: rgb(255, 255, 255);
    border-style: solid;
    border-width: 2px;
    border-color: rgb(44, 44, 44);
    padding: 0px;
    font-size: 10pt;
}
QToolBar {
    border: none;
    spacing: 0px; /* spacing between items in the tool bar */
}
QToolBar::separator {
    background-color: rgb(65, 65, 65);
    border: 2px solid rgb(75, 75, 75);
    border-radius: 1px;
    border-style: solid;
}
)";