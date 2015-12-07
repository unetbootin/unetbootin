/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt3Support module of the Qt Toolkit.
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

#ifndef Q3PROCESS_H
#define Q3PROCESS_H

#include <QtCore/qobject.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qdir.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3SupportLight)

#ifndef QT_NO_PROCESS

class Q3ProcessPrivate;
class Q3Membuf;

class Q_COMPAT_EXPORT Q3Process : public QObject
{
    Q_OBJECT
public:
    Q3Process( QObject *parent=0, const char *name=0 );
    Q3Process( const QString& arg0, QObject *parent=0, const char *name=0 );
    Q3Process( const QStringList& args, QObject *parent=0, const char *name=0 );
    ~Q3Process();

    // set and get the arguments and working directory
    QStringList arguments() const;
    void clearArguments();
    virtual void setArguments( const QStringList& args );
    virtual void addArgument( const QString& arg );
#ifndef QT_NO_DIR
    QDir workingDirectory() const;
    virtual void setWorkingDirectory( const QDir& dir );
#endif

    // set and get the comms wanted
    enum Communication { Stdin=0x01, Stdout=0x02, Stderr=0x04, DupStderr=0x08 };
    void setCommunication( int c );
    int communication() const;

    // start the execution
    virtual bool start( QStringList *env=0 );
    virtual bool launch( const QString& buf, QStringList *env=0  );
    virtual bool launch( const QByteArray& buf, QStringList *env=0  );

    // inquire the status
    bool isRunning() const;
    bool normalExit() const;
    int exitStatus() const;

    // reading
    virtual QByteArray readStdout();
    virtual QByteArray readStderr();
    bool canReadLineStdout() const;
    bool canReadLineStderr() const;
    virtual QString readLineStdout();
    virtual QString readLineStderr();

    // get platform dependent process information
#if defined(Q_OS_WIN32) || defined(Q_OS_WINCE)
    typedef void* PID;
#else
    typedef Q_LONG PID;
#endif
    PID processIdentifier();

    void flushStdin();

Q_SIGNALS:
    void readyReadStdout();
    void readyReadStderr();
    void processExited();
    void wroteToStdin();
    void launchFinished();

public Q_SLOTS:
    // end the execution
    void tryTerminate() const;
    void kill() const;

    // input
    virtual void writeToStdin( const QByteArray& buf );
    virtual void writeToStdin( const QString& buf );
    virtual void closeStdin();

protected: // ### or private?
    void connectNotify( const char * signal );
    void disconnectNotify( const char * signal );
private:
    void setIoRedirection( bool value );
    void setNotifyOnExit( bool value );
    void setWroteStdinConnected( bool value );

    void init();
    void reset();
#if defined(Q_OS_WIN32) || defined(Q_OS_WINCE)
    uint readStddev( Qt::HANDLE dev, char *buf, uint bytes );
#endif
    Q3Membuf* membufStdout();
    Q3Membuf* membufStderr();

private Q_SLOTS:
    void socketRead( int fd );
    void socketWrite( int fd );
    void timeout();
    void closeStdinLaunch();

private:
    Q3ProcessPrivate *d;
#ifndef QT_NO_DIR
    QDir        workingDir;
#endif
    QStringList _arguments;

    int  exitStat; // exit status
    bool exitNormal; // normal exit?
    bool ioRedirection; // automatically set be (dis)connectNotify
    bool notifyOnExit; // automatically set be (dis)connectNotify
    bool wroteToStdinConnected; // automatically set be (dis)connectNotify

    bool readStdoutCalled;
    bool readStderrCalled;
    int comms;

    friend class Q3ProcessPrivate;
#if defined(Q_OS_UNIX)
    friend class Q3ProcessManager;
    friend class QProc;
#endif

#if defined(Q_DISABLE_COPY) // Disabled copy constructor and operator=
    Q3Process( const Q3Process & );
    Q3Process &operator=( const Q3Process & );
#endif
};

#endif // QT_NO_PROCESS

QT_END_NAMESPACE

QT_END_HEADER

#endif // Q3PROCESS_H
