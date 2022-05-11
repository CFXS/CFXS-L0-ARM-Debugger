// [CFXS] //
#include <QWidgetAction>
#include <QLabel>

namespace L0::UI::Utils {

    inline QWidgetAction* CreateQMenuTextSeparator(const QString& text) {
        auto* label = new QLabel(text);
        label->setStyleSheet("color: rgb(220, 220, 220); font-weight: 500; background: rgba(255, 255, 255, 32);");
        label->setAlignment(Qt::AlignCenter);
        auto* separator = new QWidgetAction(nullptr);
        separator->setDefaultWidget(label);
        return separator;
    }

    inline QWidgetAction* CreateQMenuTextSeparator(const QString& text,
                                                   const QString& styleSheet,
                                                   Qt::Alignment alignment = Qt::AlignVCenter) {
        auto* label = new QLabel(text);
        label->setStyleSheet(styleSheet);
        label->setAlignment(alignment);
        auto* separator = new QWidgetAction(nullptr);
        separator->setDefaultWidget(label);
        return separator;
    }

} // namespace L0::UI::Utils