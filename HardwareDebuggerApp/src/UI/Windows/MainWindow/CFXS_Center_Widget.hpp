// [CFXS] //
#pragma once

#include <QWidget>
#include <QPainter>
#include <QSizePolicy>
#include <QString>
#include <QPixmap>
#include <QIcon>

namespace HWD::UI {

    class CFXS_Center_Widget : public QWidget {
        static constexpr auto SCALE      = 0.8;
        static constexpr auto IMG_ASPECT = 5.13597;

    public:
        CFXS_Center_Widget(QWidget* parent = nullptr) : QWidget(parent), m_Icon(QStringLiteral(":/Image/CFXS")) {
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

} // namespace HWD::UI