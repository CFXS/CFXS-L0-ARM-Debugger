// [CFXS] //
#pragma once

#include <DockWidget.h>

namespace Ui {
    class TextEditPanel;
}

namespace HWD::UI {

    class TextEditPanel : public ads::CDockWidget {
    public:
        TextEditPanel();

        void SetFilePath(const QString& filePath);
        const QString& GetFilePath() const;

    private:
        std::unique_ptr<Ui::TextEditPanel> ui;
        QString m_FilePath;
    };

} // namespace HWD::UI
