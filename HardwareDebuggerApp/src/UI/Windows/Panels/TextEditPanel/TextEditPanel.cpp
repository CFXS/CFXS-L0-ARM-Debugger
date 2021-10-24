// [CFXS] //
#include "TextEditPanel.hpp"
#include "ui_TextEditPanel.h"

namespace HWD::UI {

    TextEditPanel::TextEditPanel() : ads::CDockWidget(QStringLiteral("TextEditPanel")), ui(std::make_unique<Ui::TextEditPanel>()) {
        HWDLOG_CORE_TRACE("Create text edit panel");
        ui->setupUi(this);

        setWidget(ui->root);
    }

    const QString& TextEditPanel::GetFilePath() const {
        return m_FilePath;
    }

    void TextEditPanel::SetFilePath(const QString& filePath) {
        setObjectName(QStringLiteral("TextEditPanel|") + filePath);
    }

} // namespace HWD::UI