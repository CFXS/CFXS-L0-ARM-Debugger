// [CFXS] //
#pragma once
#include <QFileIconProvider>

namespace HWD::UI {

    class FileBrowserIconProvider : public QFileIconProvider {
    public:
        FileBrowserIconProvider();

        QIcon icon(IconType type) const override;
        QIcon icon(const QFileInfo &info) const override;
    };

} // namespace HWD::UI