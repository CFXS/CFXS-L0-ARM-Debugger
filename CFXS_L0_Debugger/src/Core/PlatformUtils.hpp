// [CFXS] //
#pragma once
#include <QString>
#include <QDesktopServices>
#include <QUrl>
#include <QDir>
#include <QFileInfo>
#include <QProcess>

namespace L0::PlatformUtils {
    static void ShowInFileExplorer(const QString& path) {
        QFileInfo info(path);
#if defined(Q_OS_WIN)
        QStringList args;
        if (!info.isDir())
            args << "/select,";
        args << QDir::toNativeSeparators(path);
        if (QProcess::startDetached("explorer", args))
            return;
#endif
        QDesktopServices::openUrl(QUrl::fromLocalFile(info.isDir() ? path : info.path()));
    }
} // namespace L0::PlatformUtils
