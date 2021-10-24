// [CFXS] //
#pragma once

#include <DockWidget.h>
#include <QFile>
#include <QFileInfo>
#include <QTextEdit>

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
        void UpdateContent();

    private:
        std::unique_ptr<Ui::TextEditPanel> ui;
        QString m_FilePath;
        QFile m_File;
        QFileInfo m_FileInfo;
        QTextEdit* m_TextEdit;
        QTextEdit* m_LineWidget;
    };

} // namespace HWD::UI
