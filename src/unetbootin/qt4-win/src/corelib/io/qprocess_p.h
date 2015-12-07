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

#ifndef QPROCESS_P_H
#define QPROCESS_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "QtCore/qprocess.h"
#include "QtCore/qstringlist.h"
#include "private/qringbuffer_p.h"
#include "private/qiodevice_p.h"

#ifdef Q_OS_WIN
#include "QtCore/qt_windows.h"
typedef HANDLE Q_PIPE;
#define INVALID_Q_PIPE INVALID_HANDLE_VALUE
#else
typedef int Q_PIPE;
#define INVALID_Q_PIPE -1
#endif

#ifndef QT_NO_PROCESS

QT_BEGIN_NAMESPACE

class QSocketNotifier;
class QWindowsPipeWriter;
class QWinEventNotifier;
class QTimer;

class QProcessPrivate : public QIODevicePrivate
{
public:
    Q_DECLARE_PUBLIC(QProcess)

    struct Channel {
        enum ProcessChannelType {
            Normal = 0,
            PipeSource = 1,
            PipeSink = 2,
            Redirect = 3
            // if you add "= 4" here, increase the number of bits below
        };

        Channel() : process(0), notifier(0), type(Normal), closed(false), append(false)
        {
            pipe[0] = INVALID_Q_PIPE;
            pipe[1] = INVALID_Q_PIPE;
        }

        void clear();

        Channel &operator=(const QString &fileName)
        {
            clear();
            file = fileName;
            type = fileName.isEmpty() ? Normal : Redirect;
            return *this;
        }

        void pipeTo(QProcessPrivate *other)
        {
            clear();
            process = other;
            type = PipeSource;
        }

        void pipeFrom(QProcessPrivate *other)
        {
            clear();
            process = other;
            type = PipeSink;
        }

        QString file;
        QProcessPrivate *process;
        QSocketNotifier *notifier;
        Q_PIPE pipe[2];

        unsigned type : 2;
        bool closed : 1;
        bool append : 1;
    };

    QProcessPrivate();
    virtual ~QProcessPrivate();

    // private slots
    bool _q_canReadStandardOutput();
    bool _q_canReadStandardError();
    bool _q_canWrite();
    bool _q_startupNotification();
    bool _q_processDied();
    void _q_notified();

    QProcess::ProcessChannel processChannel;
    QProcess::ProcessChannelMode processChannelMode;
    QProcess::ProcessError processError;
    QProcess::ProcessState processState;
    QString workingDirectory;
    Q_PID pid;
    int sequenceNumber;

    bool dying;
    bool emittedReadyRead;
    bool emittedBytesWritten;

    Channel stdinChannel;
    Channel stdoutChannel;
    Channel stderrChannel;
    bool createChannel(Channel &channel);
    void closeWriteChannel();

    QString program;
    QStringList arguments;
    QStringList environment;

    QRingBuffer outputReadBuffer;
    QRingBuffer errorReadBuffer;
    QRingBuffer writeBuffer;

    Q_PIPE childStartedPipe[2];
    Q_PIPE deathPipe[2];
    void destroyPipe(Q_PIPE pipe[2]);

    QSocketNotifier *startupSocketNotifier;
    QSocketNotifier *deathNotifier;

    // the wonderful windows notifier
    QTimer *notifier;
    QWindowsPipeWriter *pipeWriter;
    QWinEventNotifier *processFinishedNotifier;

    void startProcess();
#ifdef Q_OS_UNIX
    void execChild(const char *workingDirectory, char **path, char **argv, char **envp);
#endif
    bool processStarted();
    void terminateProcess();
    void killProcess();
    void findExitCode();
#ifdef Q_OS_UNIX
    bool waitForDeadChild();
#endif
#ifdef Q_OS_WIN
    void flushPipeWriter();
    qint64 pipeWriterBytesToWrite() const;
#endif

    static bool startDetached(const QString &program, const QStringList &arguments, const QString &workingDirectory = QString(),
                              qint64 *pid = 0);

    int exitCode;
    QProcess::ExitStatus exitStatus;
    bool crashed;
#ifdef Q_OS_UNIX
    int serial;
#endif

    bool waitForStarted(int msecs = 30000);
    bool waitForReadyRead(int msecs = 30000);
    bool waitForBytesWritten(int msecs = 30000);
    bool waitForFinished(int msecs = 30000);
    bool waitForWrite(int msecs = 30000);

    qint64 bytesAvailableFromStdout() const;
    qint64 bytesAvailableFromStderr() const;
    qint64 readFromStdout(char *data, qint64 maxlen);
    qint64 readFromStderr(char *data, qint64 maxlen);
    qint64 writeToStdin(const char *data, qint64 maxlen);

    void cleanup();
#ifdef Q_OS_UNIX
    static void initializeProcessManager();
#endif
};

QT_END_NAMESPACE

#endif // QT_NO_PROCESS

#endif // QPROCESS_P_H
