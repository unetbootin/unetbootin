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

#ifndef BROWSERAPPLICATION_H
#define BROWSERAPPLICATION_H

#include <QtGui/QApplication>

#include <QtCore/QUrl>
#include <QtCore/QPointer>

#include <QtGui/QIcon>

QT_BEGIN_NAMESPACE
class QLocalServer;
QT_END_NAMESPACE

class BookmarksManager;
class BrowserMainWindow;
class CookieJar;
class DownloadManager;
class HistoryManager;
class NetworkAccessManager;
class BrowserApplication : public QApplication
{
    Q_OBJECT

public:
    BrowserApplication(int &argc, char **argv);
    ~BrowserApplication();
    static BrowserApplication *instance();
    void loadSettings();

    bool isTheOnlyBrowser() const;
    BrowserMainWindow *mainWindow();
    QList<BrowserMainWindow*> mainWindows();
    QIcon icon(const QUrl &url) const;

    void saveSession();
    bool canRestoreSession() const;

    static HistoryManager *historyManager();
    static CookieJar *cookieJar();
    static DownloadManager *downloadManager();
    static NetworkAccessManager *networkAccessManager();
    static BookmarksManager *bookmarksManager();

#if defined(Q_WS_MAC)
    bool event(QEvent *event);
#endif

public slots:
    BrowserMainWindow *newMainWindow();
    void restoreLastSession();
#if defined(Q_WS_MAC)
    void lastWindowClosed();
    void quitBrowser();
#endif

private slots:
    void postLaunch();
    void openUrl(const QUrl &url);
    void newLocalSocketConnection();

private:
    void clean();
    void installTranslator(const QString &name);

    static HistoryManager *s_historyManager;
    static DownloadManager *s_downloadManager;
    static NetworkAccessManager *s_networkAccessManager;
    static BookmarksManager *s_bookmarksManager;

    QList<QPointer<BrowserMainWindow> > m_mainWindows;
    QLocalServer *m_localServer;
    QByteArray m_lastSession;
    mutable QIcon m_defaultIcon;
};

#endif // BROWSERAPPLICATION_H

