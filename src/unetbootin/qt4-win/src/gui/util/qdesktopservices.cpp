/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#include "qdesktopservices.h"

#ifndef QT_NO_DESKTOPSERVICES

#include <qdebug.h>

#if defined(Q_WS_QWS)
#include "qdesktopservices_qws.cpp"
#elif defined(Q_WS_X11)
#include "qdesktopservices_x11.cpp"
#elif defined(Q_WS_WIN)
#include "qdesktopservices_win.cpp"
#elif defined(Q_WS_MAC)
#include "qdesktopservices_mac.cpp"
#endif

#include <qhash.h>
#include <qobject.h>
#include <qcoreapplication.h>
#include <qurl.h>
#include <qmutex.h>

QT_BEGIN_NAMESPACE

class QOpenUrlHandlerRegistry : public QObject
{
    Q_OBJECT
public:
    inline QOpenUrlHandlerRegistry() : mutex(QMutex::Recursive) {}

    QMutex mutex;

    struct Handler
    {
        QObject *receiver;
        QByteArray name;
    };
    typedef QHash<QString, Handler> HandlerHash;
    HandlerHash handlers;

public Q_SLOTS:
    void handlerDestroyed(QObject *handler);

};

Q_GLOBAL_STATIC(QOpenUrlHandlerRegistry, handlerRegistry)

void QOpenUrlHandlerRegistry::handlerDestroyed(QObject *handler)
{
    HandlerHash::Iterator it = handlers.begin();
    while (it != handlers.end()) {
        if (it->receiver == handler) {
            it = handlers.erase(it);
        } else {
            ++it;
        }
    }
}

/*!
    \class QDesktopServices
    \brief The QDesktopServices class provides methods for accessing common desktop services.
    \since 4.2
    \ingroup desktop

    Many desktop environments provide services that can be used by applications to
    perform common tasks, such as opening a web page, in a way that is both consistent
    and takes into account the user's application preferences.

    This class contains functions that provide simple interfaces to these services
    that indicate whether they succeeded or failed.

    The openUrl() function is used to open files located at arbitrary URLs in external
    applications. For URLs that correspond to resources on the local filing system
    (where the URL scheme is "file"), a suitable application will be used to open the
    file; otherwise, a web browser will be used to fetch and display the file.

    The user's desktop settings control whether certain executable file types are
    opened for browsing, or if they are executed instead. Some desktop environments
    are configured to prevent users from executing files obtained from non-local URLs,
    or to ask the user's permission before doing so.

    \section1 URL Handlers

    The behavior of the openUrl() function can be customized for individual URL
    schemes to allow applications to override the default handling behavior for
    certain types of URLs.

    The dispatch mechanism allows only one custom handler to be used for each URL
    scheme; this is set using the setUrlHandler() function. Each handler is
    implemented as a slot which accepts only a single QUrl argument.

    The existing handlers for each scheme can be removed with the
    unsetUrlHandler() function. This returns the handling behavior for the given
    scheme to the default behavior.

    This system makes it easy to implement a help system, for example. Help could be
    provided in labels and text browsers using \gui{help://myapplication/mytopic}
    URLs, and by registering a handler it becomes possible to display the help text
    inside the application:

    \snippet doc/src/snippets/code/src.gui.util.qdesktopservices.cpp 0

    If inside the handler you decide that you can't open the requested
    URL, you can just call QDesktopServices::openUrl() again with the
    same argument, and it will try to open the URL using the
    appropriate mechanism for the user's desktop environment.

    \sa QSystemTrayIcon, QProcess
*/

/*!
    Opens the given \a url in the appropriate web browser for the user's desktop
    environment, and returns true if successful; otherwise returns false.

    If the URL is a reference to a local file (i.e. the URL scheme is "file") then
    it will be opened with a suitable application instead of a web browser.

    If a \c mailto URL is specified, the user's e-mail client will be used to open a
    composer window containing the options specified in the URL, similar to the way
    \c mailto links are handled by a web browser.

    For example, the following URL contains a recipient (\c{user@foo.com}), a
    subject (\c{Test}), and a message body (\c{Just a test}):

    \snippet doc/src/snippets/code/src.gui.util.qdesktopservices.cpp 1

    \warning Although many e-mail clients can send attachments and are
    Unicode-aware, the user may have configured their client without these features.
    Also, certain e-mail clients (e.g., Lotus Notes) have problems with long URLs.

    \sa setUrlHandler()
*/
bool QDesktopServices::openUrl(const QUrl &url)
{
    QOpenUrlHandlerRegistry *registry = handlerRegistry();
    QMutexLocker locker(&registry->mutex);
    static bool insideOpenUrlHandler = false;

    if (!insideOpenUrlHandler) {
        QOpenUrlHandlerRegistry::HandlerHash::ConstIterator handler = registry->handlers.constFind(url.scheme());
        if (handler != registry->handlers.constEnd()) {
            insideOpenUrlHandler = true;
            bool result = QMetaObject::invokeMethod(handler->receiver, handler->name, Qt::DirectConnection, Q_ARG(QUrl, url));
            insideOpenUrlHandler = false;
            return result; // ### support bool slot return type
        }
    }

    bool result;
    if (url.scheme() == QLatin1String("file"))
        result = openDocument(url);
    else
        result = launchWebBrowser(url);

    return result;
}

/*!
    Sets the handler for the given \a scheme to be the handler \a method provided by
    the \a receiver object.

    This function provides a way to customize the behavior of openUrl(). If openUrl()
    is called with a URL with the specified \a scheme then the given \a method on the
    \a receiver object is called instead of QDesktopServices launching an external
    application.

    The provided method must be implemented as a slot that only accepts a single QUrl
    argument.

    If setUrlHandler() is used to set a new handler for a scheme which already
    has a handler, the existing handler is simply replaced with the new one.
    Since QDesktopServices does not take ownership of handlers, no objects are
    deleted when a handler is replaced.

    Note that the handler will always be called from within the same thread that
    calls QDesktopServices::openUrl().

    \sa openUrl()
*/
void QDesktopServices::setUrlHandler(const QString &scheme, QObject *receiver, const char *method)
{
    QOpenUrlHandlerRegistry *registry = handlerRegistry();
    QMutexLocker locker(&registry->mutex);
    if (!receiver) {
        registry->handlers.remove(scheme);
        return;
    }
    QOpenUrlHandlerRegistry::Handler h;
    h.receiver = receiver;
    h.name = method;
    registry->handlers.insert(scheme, h);
    QObject::connect(receiver, SIGNAL(destroyed(QObject*)),
                     registry, SLOT(handlerDestroyed(QObject*)));
}

/*!
    Removes a previously set url handler for the specified \a scheme.
*/
void QDesktopServices::unsetUrlHandler(const QString &scheme)
{
    setUrlHandler(scheme, 0, 0);
}

QT_END_NAMESPACE

#include "qdesktopservices.moc"

#endif // QT_NO_DESKTOPSERVICES
