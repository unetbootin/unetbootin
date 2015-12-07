/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License versions 2.0 or 3.0 as published by the Free Software
** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file.  Alternatively you may (at
** your option) use any later version of the GNU General Public
** License if such license has been publicly approved by Trolltech ASA
** (or its successors, if any) and the KDE Free Qt Foundation. In
** addition, as a special exception, Trolltech gives you certain
** additional rights. These rights are described in the Trolltech GPL
** Exception version 1.2, which can be found at
** http://www.trolltech.com/products/qt/gplexception/ and in the file
** GPL_EXCEPTION.txt in this package.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/. If
** you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** In addition, as a special exception, Trolltech, as the sole
** copyright holder for Qt Designer, grants users of the Qt/Eclipse
** Integration plug-in the right for the Qt/Eclipse Integration to
** link to functionality provided by Qt Designer and its related
** libraries.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not expressly
** granted herein.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "browserapplication.h"

#include "bookmarks.h"
#include "browsermainwindow.h"
#include "cookiejar.h"
#include "downloadmanager.h"
#include "history.h"
#include "networkaccessmanager.h"
#include "tabwidget.h"
#include "webview.h"

#include <QtCore/QBuffer>
#include <QtCore/QDir>
#include <QtCore/QLibraryInfo>
#include <QtCore/QSettings>
#include <QtCore/QTextStream>
#include <QtCore/QTranslator>

#include <QtGui/QDesktopServices>
#include <QtGui/QFileOpenEvent>

#include <QtNetwork/QLocalServer>
#include <QtNetwork/QLocalSocket>
#include <QtNetwork/QNetworkProxy>

#include <QtWebKit/QWebSettings>

#include <QtCore/QDebug>

DownloadManager *BrowserApplication::s_downloadManager = 0;
HistoryManager *BrowserApplication::s_historyManager = 0;
NetworkAccessManager *BrowserApplication::s_networkAccessManager = 0;
BookmarksManager *BrowserApplication::s_bookmarksManager = 0;

BrowserApplication::BrowserApplication(int &argc, char **argv)
    : QApplication(argc, argv)
    , m_localServer(0)
{
    QCoreApplication::setOrganizationName(QLatin1String("Trolltech"));
    QCoreApplication::setApplicationName(QLatin1String("demobrowser"));
    QCoreApplication::setApplicationVersion(QLatin1String("0.1"));
#ifdef Q_WS_QWS
    // Use a different server name for QWS so we can run an X11
    // browser and a QWS browser in parallel on the same machine for
    // debugging
    QString serverName = QCoreApplication::applicationName() + QLatin1String("_qws");
#else
    QString serverName = QCoreApplication::applicationName();
#endif
    QLocalSocket socket;
    socket.connectToServer(serverName);
    if (socket.waitForConnected(500)) {
        QTextStream stream(&socket);
        QStringList args = QCoreApplication::arguments();
        if (args.count() > 1)
            stream << args.last();
        else
            stream << QString();
        stream.flush();
        socket.waitForBytesWritten();
        return;
    }

#if defined(Q_WS_MAC)
    QApplication::setQuitOnLastWindowClosed(false);
#else
    QApplication::setQuitOnLastWindowClosed(true);
#endif

    m_localServer = new QLocalServer(this);
    connect(m_localServer, SIGNAL(newConnection()),
            this, SLOT(newLocalSocketConnection()));
    if (!m_localServer->listen(serverName)) {
        if (m_localServer->serverError() == QAbstractSocket::AddressInUseError
            && QFile::exists(m_localServer->serverName())) {
            QFile::remove(m_localServer->serverName());
            m_localServer->listen(serverName);
        }
    }

    QDesktopServices::setUrlHandler(QLatin1String("http"), this, "openUrl");
    QString localSysName = QLocale::system().name();

    installTranslator(QLatin1String("qt_") + localSysName);

    QSettings settings;
    settings.beginGroup(QLatin1String("sessions"));
    m_lastSession = settings.value(QLatin1String("lastSession")).toByteArray();
    settings.endGroup();

#if defined(Q_WS_MAC)
    connect(this, SIGNAL(lastWindowClosed()),
            this, SLOT(lastWindowClosed()));
#endif

    QTimer::singleShot(0, this, SLOT(postLaunch()));
}

BrowserApplication::~BrowserApplication()
{
    delete s_downloadManager;
    qDeleteAll(m_mainWindows);
    delete s_networkAccessManager;
    delete s_bookmarksManager;
}

#if defined(Q_WS_MAC)
void BrowserApplication::lastWindowClosed()
{
    clean();
    BrowserMainWindow *mw = new BrowserMainWindow;
    mw->slotHome();
    m_mainWindows.prepend(mw);
}
#endif

BrowserApplication *BrowserApplication::instance()
{
    return (static_cast<BrowserApplication *>(QCoreApplication::instance()));
}

#if defined(Q_WS_MAC)
#include <QtGui/QMessageBox>
void BrowserApplication::quitBrowser()
{
    clean();
    int tabCount = 0;
    for (int i = 0; i < m_mainWindows.count(); ++i) {
        tabCount =+ m_mainWindows.at(i)->tabWidget()->count();
    }

    if (tabCount > 1) {
        int ret = QMessageBox::warning(mainWindow(), QString(),
                           tr("There are %1 windows and %2 tabs open\n"
                              "Do you want to quit anyway?").arg(m_mainWindows.count()).arg(tabCount),
                           QMessageBox::Yes | QMessageBox::No,
                           QMessageBox::No);
        if (ret == QMessageBox::No)
            return;
    }

    exit(0);
}
#endif

/*!
    Any actions that can be delayed until the window is visible
 */
void BrowserApplication::postLaunch()
{
    QString directory = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
    if (directory.isEmpty())
        directory = QDir::homePath() + QLatin1String("/.") + QCoreApplication::applicationName();
    QWebSettings::setIconDatabasePath(directory);

    setWindowIcon(QIcon(QLatin1String(":browser.svg")));

    loadSettings();

    // newMainWindow() needs to be called in main() for this to happen
    if (m_mainWindows.count() > 0) {
        QStringList args = QCoreApplication::arguments();
        if (args.count() > 1)
            mainWindow()->loadPage(args.last());
        else
            mainWindow()->slotHome();
    }
    BrowserApplication::historyManager();
}

void BrowserApplication::loadSettings()
{
    QSettings settings;
    settings.beginGroup(QLatin1String("websettings"));

    QWebSettings *defaultSettings = QWebSettings::globalSettings();
    QString standardFontFamily = defaultSettings->fontFamily(QWebSettings::StandardFont);
    int standardFontSize = defaultSettings->fontSize(QWebSettings::DefaultFontSize);
    QFont standardFont = QFont(standardFontFamily, standardFontSize);
    standardFont = qVariantValue<QFont>(settings.value(QLatin1String("standardFont"), standardFont));
    defaultSettings->setFontFamily(QWebSettings::StandardFont, standardFont.family());
    defaultSettings->setFontSize(QWebSettings::DefaultFontSize, standardFont.pointSize());

    QString fixedFontFamily = defaultSettings->fontFamily(QWebSettings::FixedFont);
    int fixedFontSize = defaultSettings->fontSize(QWebSettings::DefaultFixedFontSize);
    QFont fixedFont = QFont(fixedFontFamily, fixedFontSize);
    fixedFont = qVariantValue<QFont>(settings.value(QLatin1String("fixedFont"), fixedFont));
    defaultSettings->setFontFamily(QWebSettings::FixedFont, fixedFont.family());
    defaultSettings->setFontSize(QWebSettings::DefaultFixedFontSize, fixedFont.pointSize());

    defaultSettings->setAttribute(QWebSettings::JavascriptEnabled, settings.value(QLatin1String("enableJavascript"), true).toBool());
    defaultSettings->setAttribute(QWebSettings::PluginsEnabled, settings.value(QLatin1String("enablePlugins"), true).toBool());

    QUrl url = settings.value(QLatin1String("userStyleSheet")).toUrl();
    defaultSettings->setUserStyleSheetUrl(url);

    settings.endGroup();
}

QList<BrowserMainWindow*> BrowserApplication::mainWindows()
{
    clean();
    QList<BrowserMainWindow*> list;
    for (int i = 0; i < m_mainWindows.count(); ++i)
        list.append(m_mainWindows.at(i));
    return list;
}

void BrowserApplication::clean()
{
    // cleanup any deleted main windows first
    for (int i = m_mainWindows.count() - 1; i >= 0; --i)
        if (m_mainWindows.at(i).isNull())
            m_mainWindows.removeAt(i);
}

void BrowserApplication::saveSession()
{
    QWebSettings *globalSettings = QWebSettings::globalSettings();
    if (globalSettings->testAttribute(QWebSettings::PrivateBrowsingEnabled))
        return;

    clean();

    QSettings settings;
    settings.beginGroup(QLatin1String("sessions"));

    QByteArray data;
    QBuffer buffer(&data);
    QDataStream stream(&buffer);
    buffer.open(QIODevice::ReadWrite);

    stream << m_mainWindows.count();
    for (int i = 0; i < m_mainWindows.count(); ++i)
        stream << m_mainWindows.at(i)->saveState();
    settings.setValue(QLatin1String("lastSession"), data);
    settings.endGroup();
}

bool BrowserApplication::canRestoreSession() const
{
    return !m_lastSession.isEmpty();
}

void BrowserApplication::restoreLastSession()
{
    QList<QByteArray> windows;
    QBuffer buffer(&m_lastSession);
    QDataStream stream(&buffer);
    buffer.open(QIODevice::ReadOnly);
    int windowCount;
    stream >> windowCount;
    for (int i = 0; i < windowCount; ++i) {
        QByteArray windowState;
        stream >> windowState;
        windows.append(windowState);
    }
    for (int i = 0; i < windows.count(); ++i) {
        BrowserMainWindow *newWindow = 0;
        if (m_mainWindows.count() == 1
            && mainWindow()->tabWidget()->count() == 1
            && mainWindow()->currentTab()->url() == QUrl()) {
            newWindow = mainWindow();
        } else {
            newWindow = newMainWindow();
        }
        newWindow->restoreState(windows.at(i));
    }
}

bool BrowserApplication::isTheOnlyBrowser() const
{
    return (m_localServer != 0);
}

void BrowserApplication::installTranslator(const QString &name)
{
    QTranslator *translator = new QTranslator(this);
    translator->load(name, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    QApplication::installTranslator(translator);
}

#if defined(Q_WS_MAC)
bool BrowserApplication::event(QEvent* event)
{
    switch (event->type()) {
    case QEvent::ApplicationActivate: {
        clean();
        if (!m_mainWindows.isEmpty()) {
            BrowserMainWindow *mw = mainWindow();
            if (mw && !mw->isMinimized()) {
                mainWindow()->show();
            }
            return true;
        }
    }
    case QEvent::FileOpen:
        if (!m_mainWindows.isEmpty()) {
            mainWindow()->loadPage(static_cast<QFileOpenEvent *>(event)->file());
            return true;
        }
    default:
        break;
    }
    return QApplication::event(event);
}
#endif

void BrowserApplication::openUrl(const QUrl &url)
{
    mainWindow()->loadPage(url.toString());
}

BrowserMainWindow *BrowserApplication::newMainWindow()
{
    BrowserMainWindow *browser = new BrowserMainWindow();
    m_mainWindows.prepend(browser);
    browser->show();
    return browser;
}

BrowserMainWindow *BrowserApplication::mainWindow()
{
    clean();
    if (m_mainWindows.isEmpty())
        newMainWindow();
    return m_mainWindows[0];
}

void BrowserApplication::newLocalSocketConnection()
{
    QLocalSocket *socket = m_localServer->nextPendingConnection();
    if (!socket)
        return;
    socket->waitForReadyRead(1000);
    QTextStream stream(socket);
    QString url;
    stream >> url;
    if (!url.isEmpty()) {
        QSettings settings;
        settings.beginGroup(QLatin1String("general"));
        int openLinksIn = settings.value(QLatin1String("openLinksIn"), 0).toInt();
        settings.endGroup();
        if (openLinksIn == 1)
            newMainWindow();
        else
            mainWindow()->tabWidget()->newTab();
        openUrl(url);
    }
    delete socket;
    mainWindow()->raise();
    mainWindow()->activateWindow();
}

CookieJar *BrowserApplication::cookieJar()
{
    return (CookieJar*)networkAccessManager()->cookieJar();
}

DownloadManager *BrowserApplication::downloadManager()
{
    if (!s_downloadManager) {
        s_downloadManager = new DownloadManager();
    }
    return s_downloadManager;
}

NetworkAccessManager *BrowserApplication::networkAccessManager()
{
    if (!s_networkAccessManager) {
        s_networkAccessManager = new NetworkAccessManager();
        s_networkAccessManager->setCookieJar(new CookieJar);
    }
    return s_networkAccessManager;
}

HistoryManager *BrowserApplication::historyManager()
{
    if (!s_historyManager) {
        s_historyManager = new HistoryManager();
        QWebHistoryInterface::setDefaultInterface(s_historyManager);
    }
    return s_historyManager;
}

BookmarksManager *BrowserApplication::bookmarksManager()
{
    if (!s_bookmarksManager) {
        s_bookmarksManager = new BookmarksManager;
    }
    return s_bookmarksManager;
}

QIcon BrowserApplication::icon(const QUrl &url) const
{
    QIcon icon = QWebSettings::iconForUrl(url);
    if (!icon.isNull())
        return icon.pixmap(16, 16);
    if (m_defaultIcon.isNull())
        m_defaultIcon = QIcon(QLatin1String(":defaulticon.png"));
    return m_defaultIcon.pixmap(16, 16);
}

