/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt3Support module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
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
