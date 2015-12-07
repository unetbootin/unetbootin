/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#ifndef QCOREAPPLICATION_H
#define QCOREAPPLICATION_H

#include <QtCore/qobject.h>
#include <QtCore/qcoreevent.h>
#include <QtCore/qeventloop.h>

#ifdef QT_INCLUDE_COMPAT
#include <QtCore/qstringlist.h>
#endif

#if defined(Q_WS_WIN) && !defined(tagMSG)
typedef struct tagMSG MSG;
#endif

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Core)

class QCoreApplicationPrivate;
class QTextCodec;
class QTranslator;
class QPostEventList;
class QStringList;

#define qApp QCoreApplication::instance()

class Q_CORE_EXPORT QCoreApplication : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString applicationName READ applicationName WRITE setApplicationName)
    Q_PROPERTY(QString applicationVersion READ applicationVersion WRITE setApplicationVersion)
    Q_PROPERTY(QString organizationName READ organizationName WRITE setOrganizationName)
    Q_PROPERTY(QString organizationDomain READ organizationDomain WRITE setOrganizationDomain)

    Q_DECLARE_PRIVATE(QCoreApplication)
public:
    QCoreApplication(int &argc, char **argv);
    ~QCoreApplication();

#ifdef QT_DEPRECATED
    QT_DEPRECATED static int argc();
    QT_DEPRECATED static char **argv();
#endif
    static QStringList arguments();

    static void setAttribute(Qt::ApplicationAttribute attribute, bool on = true);
    static bool testAttribute(Qt::ApplicationAttribute attribute);

    static void setOrganizationDomain(const QString &orgDomain);
    static QString organizationDomain();
    static void setOrganizationName(const QString &orgName);
    static QString organizationName();
    static void setApplicationName(const QString &application);
    static QString applicationName();
    static void setApplicationVersion(const QString &version);
    static QString applicationVersion();

    static QCoreApplication *instance() { return self; }

    static int exec();
    static void processEvents(QEventLoop::ProcessEventsFlags flags = QEventLoop::AllEvents);
    static void processEvents(QEventLoop::ProcessEventsFlags flags, int maxtime);
    static void exit(int retcode=0);

    static bool sendEvent(QObject *receiver, QEvent *event);
    static void postEvent(QObject *receiver, QEvent *event);
    static void postEvent(QObject *receiver, QEvent *event, int priority);
    static void sendPostedEvents(QObject *receiver, int event_type);
    static void sendPostedEvents();
    static void removePostedEvents(QObject *receiver);
    static void removePostedEvents(QObject *receiver, int eventType);
    static bool hasPendingEvents();

    virtual bool notify(QObject *, QEvent *);

    static bool startingUp();
    static bool closingDown();

    static QString applicationDirPath();
    static QString applicationFilePath();
    static qint64 applicationPid();

#ifndef QT_NO_LIBRARY
    static void setLibraryPaths(const QStringList &);
    static QStringList libraryPaths();
    static void addLibraryPath(const QString &);
    static void removeLibraryPath(const QString &);
#endif // QT_NO_LIBRARY

#ifndef QT_NO_TRANSLATION
    static void installTranslator(QTranslator * messageFile);
    static void removeTranslator(QTranslator * messageFile);
#endif
    enum Encoding { CodecForTr, UnicodeUTF8, DefaultCodec = CodecForTr };
    // ### Qt 5: merge
    static QString translate(const char * context,
                             const char * key,
                             const char * comment = 0,
                             Encoding encoding = CodecForTr);
    static QString translate(const char * context,
                             const char * key,
                             const char * comment,
                             Encoding encoding, int n);

    static void flush();

#if defined(QT3_SUPPORT)
    inline QT3_SUPPORT void lock() {}
    inline QT3_SUPPORT void unlock(bool = true) {}
    inline QT3_SUPPORT bool locked() { return false; }
    inline QT3_SUPPORT bool tryLock() { return false; }

    static inline QT3_SUPPORT void processOneEvent()
    { processEvents(QEventLoop::WaitForMoreEvents); }
    static QT3_SUPPORT int enter_loop();
    static QT3_SUPPORT void exit_loop();
    static QT3_SUPPORT int loopLevel();
#endif

#if defined(Q_WS_WIN)
    virtual bool winEventFilter(MSG *message, long *result);
#endif

#ifdef Q_OS_UNIX
    static void watchUnixSignal(int signal, bool watch);
#endif

    typedef bool (*EventFilter)(void *message, long *result);
    EventFilter setEventFilter(EventFilter filter);
    bool filterEvent(void *message, long *result);

public Q_SLOTS:
    static void quit();

Q_SIGNALS:
    void aboutToQuit();
    void unixSignal(int);

protected:
    bool event(QEvent *);

    virtual bool compressEvent(QEvent *, QObject *receiver, QPostEventList *);

protected:
    QCoreApplication(QCoreApplicationPrivate &p);

private:
    static bool sendSpontaneousEvent(QObject *receiver, QEvent *event);
    bool notifyInternal(QObject *receiver, QEvent *event);

    void init();

    static QCoreApplication *self;

    friend class QEventDispatcherUNIXPrivate;
    friend class QApplication;
    friend class QApplicationPrivate;
    friend class QETWidget;
    friend class Q3AccelManager;
    friend class QShortcutMap;
    friend class QWidget;
    friend class QWidgetPrivate;
    friend bool qt_sendSpontaneousEvent(QObject*, QEvent*);
    friend Q_CORE_EXPORT QString qAppName();
    friend class QClassFactory;
};

inline bool QCoreApplication::sendEvent(QObject *receiver, QEvent *event)
{  if (event) event->spont = false; return self ? self->notifyInternal(receiver, event) : false; }

inline bool QCoreApplication::sendSpontaneousEvent(QObject *receiver, QEvent *event)
{ if (event) event->spont = true; return self ? self->notifyInternal(receiver, event) : false; }

inline void QCoreApplication::sendPostedEvents() { sendPostedEvents(0, 0); }

#ifdef QT_NO_TRANSLATION
// Simple versions
inline QString QCoreApplication::translate(const char *, const char *sourceText,
                                           const char *, Encoding encoding)
{
#ifndef QT_NO_TEXTCODEC
    if (encoding == UnicodeUTF8)
        return QString::fromUtf8(sourceText);
#else
    Q_UNUSED(encoding)
#endif
    return QString::fromLatin1(sourceText);
}

// Simple versions
inline QString QCoreApplication::translate(const char *, const char *sourceText,
                                           const char *, Encoding encoding, int)
{
#ifndef QT_NO_TEXTCODEC
    if (encoding == UnicodeUTF8)
        return QString::fromUtf8(sourceText);
#else
    Q_UNUSED(encoding)
#endif
    return QString::fromLatin1(sourceText);
}
#endif

// ### merge the four functions into two (using "int n = -1")
#define Q_DECLARE_TR_FUNCTIONS(context) \
public: \
    static inline QString tr(const char *sourceText, const char *comment = 0) \
        { return QCoreApplication::translate(#context, sourceText, comment); } \
    static inline QString trUtf8(const char *sourceText, const char *comment = 0) \
        { return QCoreApplication::translate(#context, sourceText, comment, \
                                             QCoreApplication::UnicodeUTF8); } \
    static inline QString tr(const char *sourceText, const char *comment, int n) \
        { return QCoreApplication::translate(#context, sourceText, comment, \
                                             QCoreApplication::CodecForTr, n); } \
    static inline QString trUtf8(const char *sourceText, const char *comment, int n) \
        { return QCoreApplication::translate(#context, sourceText, comment, \
                                             QCoreApplication::UnicodeUTF8, n); } \
private:

typedef void (*QtCleanUpFunction)();

Q_CORE_EXPORT void qAddPostRoutine(QtCleanUpFunction);
Q_CORE_EXPORT void qRemovePostRoutine(QtCleanUpFunction);
Q_CORE_EXPORT QString qAppName();                // get application name

#if defined(Q_WS_WIN) && !defined(QT_NO_DEBUG_STREAM)
Q_CORE_EXPORT QString decodeMSG(const MSG &);
Q_CORE_EXPORT QDebug operator<<(QDebug, const MSG &);
#endif

QT_END_NAMESPACE

QT_END_HEADER

#endif // QCOREAPPLICATION_H
