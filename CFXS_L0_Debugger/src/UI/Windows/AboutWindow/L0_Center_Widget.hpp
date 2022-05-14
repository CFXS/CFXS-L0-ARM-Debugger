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
#pragma once

#include <QWidget>
#include <QPainter>
#include <QSizePolicy>
#include <QString>
#include <QPixmap>
#include <QIcon>
#include <QBitmap>

namespace L0::UI {

    class L0_Center_Widget : public QWidget {
        static constexpr auto SCALE      = 0.9;
        static constexpr auto IMG_ASPECT = 5.13597;

    public:
        L0_Center_Widget(QWidget* parent = nullptr) : QWidget(parent), m_Icon(QSL(":/Image/CFXS")) {
        }

    private:
        void GeneratePixmap(const QSize& size) {
            if (size != m_GeneratedPixmapSize) {
                m_GeneratedPixmapSize = size;

                m_Pixmap  = m_Icon.pixmap(QSize(size));
                auto mask = m_Pixmap.createMaskFromColor(QColor{255, 255, 255}, Qt::MaskOutColor);
                m_Pixmap.fill(QColor{0, 0, 0});
                m_Pixmap.setMask(mask);
            }
        }

    protected:
        void paintEvent(QPaintEvent* event) {
            QPainter painter(this);

            setFixedSize(parentWidget()->size());

            auto mdim   = std::min(width(), (int)(height() * IMG_ASPECT));
            QSize wsize = QSize{(int)(mdim * SCALE), (int)((mdim / IMG_ASPECT) * SCALE)};

            GeneratePixmap(wsize);
            painter.setOpacity(0.25);
            painter.drawPixmap(width() / 2 - wsize.width() / 2, height() / 2 - wsize.height() / 2, m_Pixmap);
        }

    private:
        QIcon m_Icon;
        QPixmap m_Pixmap;
        QSize m_GeneratedPixmapSize = {-1, -1};
    };

} // namespace L0::UI