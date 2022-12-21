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
#include <Core/L0_Application.hpp>
#include <Log/Log.hpp>
#include <QWebEngineView>
#include <QWebEngineProfile>
#include <QWebEngineSettings>
#include <QWebEngineUrlSchemeHandler>
#include <QWebEngineUrlRequestJob>
#include <QWebEngineUrlScheme>
#include <QMessageBox>
#include <QCloseEvent>
#include <QFile>
#include <QTimer>
#include <QObject>

struct TimeExec {
    TimeExec(const char* n) {
        name = n;
        t.start();
    }
    ~TimeExec() {
        char str[128];
        snprintf(str, 128, "[%s] %.1f us", name, t.nsecsElapsed() / 1000.0f);
        qDebug() << str;
    }

    QElapsedTimer t;
    const char* name;
};

#define SCHEMENAME "L0"
static const QByteArray SCHEME_NAME = SCHEMENAME;
static const QUrl INDEX             = {SCHEMENAME ":index"};
class UI_Handler : public QWebEngineUrlSchemeHandler {
public:
    explicit UI_Handler(QObject* parent = nullptr) {
        qDebug() << "uih";
    }
    void requestStarted(QWebEngineUrlRequestJob* job) override {
        static const QByteArray GET(QByteArrayLiteral("GET"));

        QByteArray method = job->requestMethod();
        QUrl url          = job->requestUrl();
        QUrl initiator    = job->initiator();

        if (method == GET && url == INDEX) {
            QFile* file = new QFile(QStringLiteral(":/UI/index.html"), job);
            file->open(QIODevice::ReadOnly);
            job->reply(QByteArrayLiteral("text/html"), file);
        } else {
            LOG_CORE_ERROR("Failed to load URL: {}", url.toString());
            job->fail(QWebEngineUrlRequestJob::UrlNotFound);
        }
    }

    static void registerUrlScheme() {
        QWebEngineUrlScheme webUiScheme(SCHEME_NAME);
        webUiScheme.setFlags(QWebEngineUrlScheme::SecureScheme | QWebEngineUrlScheme::LocalScheme |
                             QWebEngineUrlScheme::LocalAccessAllowed);
        QWebEngineUrlScheme::registerScheme(webUiScheme);
    }
};

class UI_View : public QWebEngineView {
public:
    UI_View(QWidget* parent = nullptr) : QWebEngineView(parent) {
    }
    ~UI_View() = default;

protected:
    void closeEvent(QCloseEvent* event) {
        QMessageBox::StandardButton resBtn =
            QMessageBox::question(this, CFXS_PROGRAM_NAME, tr("Close window?\n"), QMessageBox::Cancel | QMessageBox::Yes, QMessageBox::Yes);
        if (resBtn != QMessageBox::Yes) {
            event->ignore();
        } else {
            event->accept();
        }
    }
};

int main(int argc, char** argv) {
    L0::Log::Initialize();

    // qputenv("QTWEBENGINE_CHROMIUM_FLAGS", "--disable-logging");

    UI_Handler::registerUrlScheme();

    QApplication app(argc, argv);

    auto profile = new QWebEngineProfile(QWebEngineProfile::defaultProfile());

    auto profileSettings = profile->settings();
    profileSettings->setAttribute(QWebEngineSettings::PluginsEnabled, false);
    profileSettings->setAttribute(QWebEngineSettings::DnsPrefetchEnabled, false);
    profileSettings->setAttribute(QWebEngineSettings::XSSAuditingEnabled, false);
    profileSettings->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, true);
    profile->installUrlSchemeHandler(SCHEME_NAME, new UI_Handler);
    // profileSettings->setAttribute(QWebEngineSettings::ScrollAnimatorEnabled, true);

    QFile ff{":/UI/index.html"};
    ff.open(QIODevice::ReadOnly);
    auto view = new UI_View;
    auto page = new QWebEnginePage(profile);
    view->setPage(page);
    page->load(INDEX);
    // view->resize(1920, 1080);
    view->showMaximized();

    return app.exec();
}