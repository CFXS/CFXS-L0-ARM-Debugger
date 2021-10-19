// [CFXS] //
#pragma once
#include <QFileIconProvider>
#include <QTreeView>

namespace HWD::UI {

    class FileBrowserIconProvider : public QFileIconProvider {
    public:
        FileBrowserIconProvider();

        QIcon icon(IconType type) const override;
        QIcon icon(const QFileInfo& info, bool isExpanded) const;
        QIcon icon(const QFileInfo& info) const override {
            return icon(info, false);
        }
    };

} // namespace HWD::UI