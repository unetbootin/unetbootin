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

#include "qplatformdefs.h"

// Solaris redefines connect -> __xnet_connect with _XOPEN_SOURCE_EXTENDED.
#if defined(connect)
#undef connect
#endif

#include "q3process.h"

#ifndef QT_NO_PROCESS

#include "qapplication.h"
#include "q3cstring.h"
#include "q3ptrqueue.h"
#include "q3ptrlist.h"
#include "qsocketnotifier.h"
#include "qtimer.h"
#include "q3cleanuphandler.h"
#include "qregexp.h"
#include "private/q3membuf_p.h"
#include "private/qobject_p.h"

#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>

QT_BEGIN_NAMESPACE

#ifdef __MIPSEL__
# ifndef SOCK_DGRAM
#  define SOCK_DGRAM 1
# endif
# ifndef SOCK_STREAM
#  define SOCK_STREAM 2
# endif
#endif

//#define QT_Q3PROCESS_DEBUG


#ifdef Q_C_CALLBACKS
extern "C" {
#endif // Q_C_CALLBACKS

    static QT_SIGNAL_RETTYPE qt_C_sigchldHnd(QT_SIGNAL_ARGS);

#ifdef Q_C_CALLBACKS
}
#endif // Q_C_CALLBACKS


class QProc;
class Q3ProcessManager;
class Q3ProcessPrivate
{
public:
    Q3ProcessPrivate();
    ~Q3ProcessPrivate();

    void closeOpenSocketsForChild();
    void newProc( pid_t pid, Q3Process *process );

    Q3Membuf bufStdout;
    Q3Membuf bufStderr;

    Q3PtrQueue<QByteArray> stdinBuf;

    QSocketNotifier *notifierStdin;
    QSocketNotifier *notifierStdout;
    QSocketNotifier *notifierStderr;

    ssize_t stdinBufRead;
    QProc *proc;

    bool exitValuesCalculated;
    bool socketReadCalled;

    static Q3ProcessManager *procManager;
};


/***********************************************************************
 *
 * QProc
 *
 **********************************************************************/
/*
  The class Q3Process does not necessarily map exactly to the running
  child processes: if the process is finished, the Q3Process class may still be
  there; furthermore a user can use Q3Process to start more than one process.

  The helper-class QProc has the semantics that one instance of this class maps
  directly to a running child process.
*/
class QProc
{
public:
    QProc( pid_t p, Q3Process *proc=0 ) : pid(p), process(proc)
    {
#if defined(QT_Q3PROCESS_DEBUG)
	qDebug( "QProc: Constructor for pid %d and Q3Process %p", pid, process );
#endif
	socketStdin = 0;
	socketStdout = 0;
	socketStderr = 0;
    }
    ~QProc()
    {
#if defined(QT_Q3PROCESS_DEBUG)
	qDebug( "QProc: Destructor for pid %d and Q3Process %p", pid, process );
#endif
	if ( process ) {
	    if ( process->d->notifierStdin )
		process->d->notifierStdin->setEnabled( false );
	    if ( process->d->notifierStdout )
		process->d->notifierStdout->setEnabled( false );
	    if ( process->d->notifierStderr )
		process->d->notifierStderr->setEnabled( false );
	    process->d->proc = 0;
	}
	if( socketStdin )
	    ::close( socketStdin );
	if( socketStdout )
	    ::close( socketStdout );
	if( socketStderr )
	    ::close( socketStderr );
    }

    pid_t pid;
    int socketStdin;
    int socketStdout;
    int socketStderr;
    Q3Process *process;
};

/***********************************************************************
 *
 * Q3ProcessManager
 *
 **********************************************************************/
class Q3ProcessManager : public QObject
{
    Q_OBJECT

public:
    Q3ProcessManager();
    ~Q3ProcessManager();

    void append( QProc *p );
    void remove( QProc *p );

    void cleanup();

public slots:
    void removeMe();
    void sigchldHnd( int );

public:
    struct sigaction oldactChld;
    struct sigaction oldactPipe;
    Q3PtrList<QProc> *procList;
    int sigchldFd[2];

private:
    QSocketNotifier *sn;
};

static void q3process_cleanup()
{
    delete Q3ProcessPrivate::procManager;
    Q3ProcessPrivate::procManager = 0;
}

#ifdef Q_OS_QNX6
#define BAILOUT close(tmpSocket);close(socketFD[1]);return -1;
int qnx6SocketPairReplacement (int socketFD[2]) {
    int tmpSocket;
    tmpSocket = socket (AF_INET, SOCK_STREAM, 0);
    if (tmpSocket == -1)
	return -1;
    socketFD[1] = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFD[1] == -1) { BAILOUT };

    sockaddr_in ipAddr;
    memset(&ipAddr, 0, sizeof(ipAddr));
    ipAddr.sin_family = AF_INET;
    ipAddr.sin_addr.s_addr = INADDR_ANY;

    int socketOptions = 1;
    setsockopt(tmpSocket, SOL_SOCKET, SO_REUSEADDR, &socketOptions, sizeof(int));

    bool found = false;
    for (int socketIP = 2000; (socketIP < 2500) && !(found); socketIP++) {
	ipAddr.sin_port = htons(socketIP);
	if (bind(tmpSocket, (struct sockaddr *)&ipAddr, sizeof(ipAddr)))
	    found = true;
    }

    if (listen(tmpSocket, 5)) { BAILOUT };

    // Select non-blocking mode
    int originalFlags = fcntl(socketFD[1], F_GETFL, 0);
    fcntl(socketFD[1], F_SETFL, originalFlags | O_NONBLOCK);

    // Request connection
    if (connect(socketFD[1], (struct sockaddr*)&ipAddr, sizeof(ipAddr)))
	if (errno != EINPROGRESS) { BAILOUT };

    // Accept connection
    socketFD[0] = accept(tmpSocket, (struct sockaddr *)NULL, (size_t *)NULL);
    if(socketFD[0] == -1) { BAILOUT };

    // We're done
    close(tmpSocket);

    // Restore original flags , ie return to blocking
    fcntl(socketFD[1], F_SETFL, originalFlags);
    return 0;
}
#undef BAILOUT
#endif

Q3ProcessManager::Q3ProcessManager() : sn(0)
{
    procList = new Q3PtrList<QProc>;
    procList->setAutoDelete( true );

    // The SIGCHLD handler writes to a socket to tell the manager that
    // something happened. This is done to get the processing in sync with the
    // event reporting.
#ifndef Q_OS_QNX6
    if ( ::socketpair( AF_UNIX, SOCK_STREAM, 0, sigchldFd ) ) {
#else
    if ( qnx6SocketPairReplacement (sigchldFd) ) {
#endif
	sigchldFd[0] = 0;
	sigchldFd[1] = 0;
    } else {
#if defined(QT_Q3PROCESS_DEBUG)
	qDebug( "Q3ProcessManager: install socket notifier (%d)", sigchldFd[1] );
#endif
	sn = new QSocketNotifier( sigchldFd[1],
		QSocketNotifier::Read, this );
	connect( sn, SIGNAL(activated(int)),
		this, SLOT(sigchldHnd(int)) );
	sn->setEnabled( true );
    }

    // install a SIGCHLD handler and ignore SIGPIPE
    struct sigaction act;

#if defined(QT_Q3PROCESS_DEBUG)
    qDebug( "Q3ProcessManager: install a SIGCHLD handler" );
#endif
    act.sa_handler = qt_C_sigchldHnd;
    sigemptyset( &(act.sa_mask) );
    sigaddset( &(act.sa_mask), SIGCHLD );
    act.sa_flags = SA_NOCLDSTOP;
#if defined(SA_RESTART)
    act.sa_flags |= SA_RESTART;
#endif
    if ( sigaction( SIGCHLD, &act, &oldactChld ) != 0 )
	qWarning( "Error installing SIGCHLD handler" );

#if defined(QT_Q3PROCESS_DEBUG)
    qDebug( "Q3ProcessManager: install a SIGPIPE handler (SIG_IGN)" );
#endif
    act.sa_handler = QT_SIGNAL_IGNORE;
    sigemptyset( &(act.sa_mask) );
    sigaddset( &(act.sa_mask), SIGPIPE );
    act.sa_flags = 0;
    if ( sigaction( SIGPIPE, &act, &oldactPipe ) != 0 )
	qWarning( "Error installing SIGPIPE handler" );
}

Q3ProcessManager::~Q3ProcessManager()
{
    delete procList;

    if ( sigchldFd[0] != 0 )
	::close( sigchldFd[0] );
    if ( sigchldFd[1] != 0 )
	::close( sigchldFd[1] );

    // restore SIGCHLD handler
#if defined(QT_Q3PROCESS_DEBUG)
    qDebug( "Q3ProcessManager: restore old sigchild handler" );
#endif
    if ( sigaction( SIGCHLD, &oldactChld, 0 ) != 0 )
	qWarning( "Error restoring SIGCHLD handler" );

#if defined(QT_Q3PROCESS_DEBUG)
    qDebug( "Q3ProcessManager: restore old sigpipe handler" );
#endif
    if ( sigaction( SIGPIPE, &oldactPipe, 0 ) != 0 )
	qWarning( "Error restoring SIGPIPE handler" );
}

void Q3ProcessManager::append( QProc *p )
{
    procList->append( p );
#if defined(QT_Q3PROCESS_DEBUG)
    qDebug( "Q3ProcessManager: append process (procList.count(): %d)", procList->count() );
#endif
}

void Q3ProcessManager::remove( QProc *p )
{
    procList->remove( p );
#if defined(QT_Q3PROCESS_DEBUG)
    qDebug( "Q3ProcessManager: remove process (procList.count(): %d)", procList->count() );
#endif
    cleanup();
}

void Q3ProcessManager::cleanup()
{
    if ( procList->count() == 0 ) {
	QTimer::singleShot( 0, this, SLOT(removeMe()) );
    }
}

void Q3ProcessManager::removeMe()
{
    if ( procList->count() == 0 ) {
	qRemovePostRoutine(q3process_cleanup);
	Q3ProcessPrivate::procManager = 0;
	delete this;
    }
}

void Q3ProcessManager::sigchldHnd( int fd )
{
    // Disable the socket notifier to make sure that this function is not
    // called recursively -- this can happen, if you enter the event loop in
    // the slot connected to the processExited() signal (e.g. by showing a
    // modal dialog) and there are more than one process which exited in the
    // meantime.
    if ( sn ) {
	if ( !sn->isEnabled() )
	    return;
	sn->setEnabled( false );
    }

    char tmp;
    ::read( fd, &tmp, sizeof(tmp) );
#if defined(QT_Q3PROCESS_DEBUG)
    qDebug( "Q3ProcessManager::sigchldHnd()" );
#endif
    QProc *proc;
    Q3Process *process;
    bool removeProc;
    proc = procList->first();
    while ( proc != 0 ) {
	removeProc = false;
	process = proc->process;
	if ( process != 0 ) {
	    if ( !process->isRunning() ) {
#if defined(QT_Q3PROCESS_DEBUG)
		qDebug( "Q3ProcessManager::sigchldHnd() (PID: %d): process exited (Q3Process available)", proc->pid );
#endif
		/*
		  Apparently, there is not consistency among different
		  operating systems on how to use FIONREAD.

		  FreeBSD, Linux and Solaris all expect the 3rd
		  argument to ioctl() to be an int, which is normally
		  32-bit even on 64-bit machines.

		  IRIX, on the other hand, expects a size_t, which is
		  64-bit on 64-bit machines.

		  So, the solution is to use size_t initialized to
		  zero to make sure all bits are set to zero,
		  preventing underflow with the FreeBSD/Linux/Solaris
		  ioctls.
		*/
		size_t nbytes = 0;
		// read pending data
		if ( proc->socketStdout && ::ioctl(proc->socketStdout, FIONREAD, (char*)&nbytes)==0 && nbytes>0 ) {
#if defined(QT_Q3PROCESS_DEBUG)
		    qDebug( "Q3ProcessManager::sigchldHnd() (PID: %d): reading %d bytes of pending data on stdout", proc->pid, nbytes );
#endif
		    process->socketRead( proc->socketStdout );
		}
		nbytes = 0;
		if ( proc->socketStderr && ::ioctl(proc->socketStderr, FIONREAD, (char*)&nbytes)==0 && nbytes>0 ) {
#if defined(QT_Q3PROCESS_DEBUG)
		    qDebug( "Q3ProcessManager::sigchldHnd() (PID: %d): reading %d bytes of pending data on stderr", proc->pid, nbytes );
#endif
		    process->socketRead( proc->socketStderr );
		}
		// close filedescriptors if open, and disable the
		// socket notifiers
		if ( proc->socketStdout ) {
		    ::close( proc->socketStdout );
		    proc->socketStdout = 0;
		    if (process->d->notifierStdout)
			process->d->notifierStdout->setEnabled(false);
		}
		if ( proc->socketStderr ) {
		    ::close( proc->socketStderr );
		    proc->socketStderr = 0;
		    if (process->d->notifierStderr)
			process->d->notifierStderr->setEnabled(false);
		}

		if ( process->notifyOnExit )
		    emit process->processExited();

		removeProc = true;
	    }
	} else {
	    int status;
	    if ( ::waitpid( proc->pid, &status, WNOHANG ) == proc->pid ) {
#if defined(QT_Q3PROCESS_DEBUG)
		qDebug( "Q3ProcessManager::sigchldHnd() (PID: %d): process exited (Q3Process not available)", proc->pid );
#endif
		removeProc = true;
	    }
	}
	if ( removeProc ) {
	    QProc *oldproc = proc;
	    proc = procList->next();
	    remove( oldproc );
	} else {
	    proc = procList->next();
	}
    }
    if ( sn )
	sn->setEnabled( true );
}

QT_BEGIN_INCLUDE_NAMESPACE
#include "q3process_unix.moc"
QT_END_INCLUDE_NAMESPACE


/***********************************************************************
 *
 * Q3ProcessPrivate
 *
 **********************************************************************/
Q3ProcessManager *Q3ProcessPrivate::procManager = 0;

Q3ProcessPrivate::Q3ProcessPrivate()
{
#if defined(QT_Q3PROCESS_DEBUG)
    qDebug( "Q3ProcessPrivate: Constructor" );
#endif
    stdinBufRead = 0;

    notifierStdin = 0;
    notifierStdout = 0;
    notifierStderr = 0;

    exitValuesCalculated = false;
    socketReadCalled = false;

    proc = 0;
}

Q3ProcessPrivate::~Q3ProcessPrivate()
{
#if defined(QT_Q3PROCESS_DEBUG)
    qDebug( "Q3ProcessPrivate: Destructor" );
#endif

    if ( proc != 0 ) {
	if ( proc->socketStdin != 0 ) {
	    ::close( proc->socketStdin );
	    proc->socketStdin = 0;
	}
	proc->process = 0;
    }

    while ( !stdinBuf.isEmpty() ) {
	delete stdinBuf.dequeue();
    }
    delete notifierStdin;
    delete notifierStdout;
    delete notifierStderr;
}

/*
  Closes all open sockets in the child process that are not needed by the child
  process. Otherwise one child may have an open socket on standard input, etc.
  of another child.
*/
void Q3ProcessPrivate::closeOpenSocketsForChild()
{
    if ( procManager != 0 ) {
	if ( procManager->sigchldFd[0] != 0 )
	    ::close( procManager->sigchldFd[0] );
	if ( procManager->sigchldFd[1] != 0 )
	    ::close( procManager->sigchldFd[1] );

	// close also the sockets from other Q3Process instances
	for ( QProc *p=procManager->procList->first(); p!=0; p=procManager->procList->next() ) {
	    ::close( p->socketStdin );
	    ::close( p->socketStdout );
	    ::close( p->socketStderr );
	}
    }
}

void Q3ProcessPrivate::newProc( pid_t pid, Q3Process *process )
{
    proc = new QProc( pid, process );
    if ( procManager == 0 ) {
	procManager = new Q3ProcessManager;
	qAddPostRoutine(q3process_cleanup);
    }
    // the Q3ProcessManager takes care of deleting the QProc instances
    procManager->append( proc );
}

/***********************************************************************
 *
 * sigchld handler callback
 *
 **********************************************************************/
static QT_SIGNAL_RETTYPE qt_C_sigchldHnd(QT_SIGNAL_ARGS)
{
    if ( Q3ProcessPrivate::procManager == 0 )
	return;
    if ( Q3ProcessPrivate::procManager->sigchldFd[0] == 0 )
	return;

    char a = 1;
    ::write( Q3ProcessPrivate::procManager->sigchldFd[0], &a, sizeof(a) );
}


/***********************************************************************
 *
 * Q3Process
 *
 **********************************************************************/
/*
  This private class does basic initialization.
*/
void Q3Process::init()
{
    d = new Q3ProcessPrivate();
    exitStat = 0;
    exitNormal = false;
}

/*
  This private class resets the process variables, etc. so that it can be used
  for another process to start.
*/
void Q3Process::reset()
{
    delete d;
    d = new Q3ProcessPrivate();
    exitStat = 0;
    exitNormal = false;
    d->bufStdout.clear();
    d->bufStderr.clear();
}

Q3Membuf* Q3Process::membufStdout()
{
    if ( d->proc && d->proc->socketStdout ) {
	/*
	  Apparently, there is not consistency among different
	  operating systems on how to use FIONREAD.

	  FreeBSD, Linux and Solaris all expect the 3rd argument to
	  ioctl() to be an int, which is normally 32-bit even on
	  64-bit machines.

	  IRIX, on the other hand, expects a size_t, which is 64-bit
	  on 64-bit machines.

	  So, the solution is to use size_t initialized to zero to
	  make sure all bits are set to zero, preventing underflow
	  with the FreeBSD/Linux/Solaris ioctls.
	*/
	size_t nbytes = 0;
	if ( ::ioctl(d->proc->socketStdout, FIONREAD, (char*)&nbytes)==0 && nbytes>0 )
	    socketRead( d->proc->socketStdout );
    }
    return &d->bufStdout;
}

Q3Membuf* Q3Process::membufStderr()
{
    if ( d->proc && d->proc->socketStderr ) {
	/*
	  Apparently, there is not consistency among different
	  operating systems on how to use FIONREAD.

	  FreeBSD, Linux and Solaris all expect the 3rd argument to
	  ioctl() to be an int, which is normally 32-bit even on
	  64-bit machines.

	  IRIX, on the other hand, expects a size_t, which is 64-bit
	  on 64-bit machines.

	  So, the solution is to use size_t initialized to zero to
	  make sure all bits are set to zero, preventing underflow
	  with the FreeBSD/Linux/Solaris ioctls.
	*/
	size_t nbytes = 0;
	if ( ::ioctl(d->proc->socketStderr, FIONREAD, (char*)&nbytes)==0 && nbytes>0 )
	    socketRead( d->proc->socketStderr );
    }
    return &d->bufStderr;
}

Q3Process::~Q3Process()
{
    delete d;
}

bool Q3Process::start( QStringList *env )
{
#if defined(QT_Q3PROCESS_DEBUG)
    qDebug( "Q3Process::start()" );
#endif
    reset();

    int sStdin[2];
    int sStdout[2];
    int sStderr[2];

    // open sockets for piping
#ifndef Q_OS_QNX6
    if ( (comms & Stdin) && ::socketpair( AF_UNIX, SOCK_STREAM, 0, sStdin ) == -1 ) {
#else
    if ( (comms & Stdin) && qnx6SocketPairReplacement(sStdin) == -1 ) {
#endif
	return false;
    }
#ifndef Q_OS_QNX6
    if ( (comms & Stderr) && ::socketpair( AF_UNIX, SOCK_STREAM, 0, sStderr ) == -1 ) {
#else
    if ( (comms & Stderr) && qnx6SocketPairReplacement(sStderr) == -1 ) {
#endif
	if ( comms & Stdin ) {
	    ::close( sStdin[0] );
	    ::close( sStdin[1] );
	}
	return false;
    }
#ifndef Q_OS_QNX6
    if ( (comms & Stdout) && ::socketpair( AF_UNIX, SOCK_STREAM, 0, sStdout ) == -1 ) {
#else
    if ( (comms & Stdout) && qnx6SocketPairReplacement(sStdout) == -1 ) {
#endif
	if ( comms & Stdin ) {
	    ::close( sStdin[0] );
	    ::close( sStdin[1] );
	}
	if ( comms & Stderr ) {
	    ::close( sStderr[0] );
	    ::close( sStderr[1] );
	}
	return false;
    }

    // the following pipe is only used to determine if the process could be
    // started
    int fd[2];
    if ( pipe( fd ) < 0 ) {
	// non critical error, go on
	fd[0] = 0;
	fd[1] = 0;
    }

    // construct the arguments for exec
    Q3CString *arglistQ = new Q3CString[ _arguments.count() + 1 ];
    const char** arglist = new const char*[ _arguments.count() + 1 ];
    int i = 0;
    for ( QStringList::Iterator it = _arguments.begin(); it != _arguments.end(); ++it ) {
	arglistQ[i] = (*it).local8Bit();
	arglist[i] = arglistQ[i];
#if defined(QT_Q3PROCESS_DEBUG)
	qDebug( "Q3Process::start(): arg %d = %s", i, arglist[i] );
#endif
	i++;
    }
#ifdef Q_OS_MACX
    if(i) {
	Q3CString arg_bundle = arglistQ[0];
	QFileInfo fi(QString::fromUtf8(arg_bundle.constData()));
	if(fi.exists() && fi.isDir() && arg_bundle.right(4) == ".app") {
	    Q3CString exe = arg_bundle;
	    int lslash = exe.findRev('/');
	    if(lslash != -1)
		exe = exe.mid(lslash+1);
	    exe = Q3CString(arg_bundle + "/Contents/MacOS/" + exe);
	    exe = exe.left(exe.length() - 4); //chop off the .app
	    if(QFile::exists(QString::fromLatin1(exe.constData()))) {
		arglistQ[0] = exe;
		arglist[0] = arglistQ[0];
	    }
	}
    }
#endif
    arglist[i] = 0;

    // Must make sure signal handlers are installed before exec'ing
    // in case the process exits quickly.
    if ( d->procManager == 0 ) {
	d->procManager = new Q3ProcessManager;
	qAddPostRoutine(q3process_cleanup);
    }

    // fork and exec
    QApplication::flushX();
    pid_t pid = fork();
    if ( pid == 0 ) {
	// child
	d->closeOpenSocketsForChild();
	if ( comms & Stdin ) {
	    ::close( sStdin[1] );
	    ::dup2( sStdin[0], STDIN_FILENO );
	}
	if ( comms & Stdout ) {
	    ::close( sStdout[0] );
	    ::dup2( sStdout[1], STDOUT_FILENO );
	}
	if ( comms & Stderr ) {
	    ::close( sStderr[0] );
	    ::dup2( sStderr[1], STDERR_FILENO );
	}
	if ( comms & DupStderr ) {
	    ::dup2( STDOUT_FILENO, STDERR_FILENO );
	}
#ifndef QT_NO_DIR
	::chdir( workingDir.absPath().latin1() );
#endif
	if ( fd[0] )
	    ::close( fd[0] );
	if ( fd[1] )
	    ::fcntl( fd[1], F_SETFD, FD_CLOEXEC ); // close on exec shows success

	if ( env == 0 ) { // inherit environment and start process
#ifndef Q_OS_QNX4
	    ::execvp( arglist[0], (char*const*)arglist ); // ### cast not nice
#else
	    ::execvp( arglist[0], (char const*const*)arglist ); // ### cast not nice
#endif
	} else { // start process with environment settins as specified in env
	    // construct the environment for exec
	    int numEntries = env->count();
#if defined(Q_OS_MACX)
	    QString ld_library_path(QLatin1String("DYLD_LIBRARY_PATH"));
#else
	    QString ld_library_path(QLatin1String("LD_LIBRARY_PATH"));
#endif
	    bool setLibraryPath =
		env->grep( QRegExp( QLatin1Char('^') + ld_library_path + QLatin1Char('=') ) ).empty() &&
		getenv( ld_library_path.local8Bit() ) != 0;
	    if ( setLibraryPath )
		numEntries++;
	    Q3CString *envlistQ = new Q3CString[ numEntries + 1 ];
	    const char** envlist = new const char*[ numEntries + 1 ];
	    int i = 0;
	    if ( setLibraryPath ) {
		envlistQ[i] = QString( ld_library_path + QLatin1String("=%1") ).arg( QString::fromLocal8Bit(getenv( ld_library_path.local8Bit() )) ).local8Bit();
		envlist[i] = envlistQ[i];
		i++;
	    }
	    for ( QStringList::Iterator it = env->begin(); it != env->end(); ++it ) {
		envlistQ[i] = (*it).local8Bit();
		envlist[i] = envlistQ[i];
		i++;
	    }
	    envlist[i] = 0;

	    // look for the executable in the search path
	    if ( _arguments.count()>0 && getenv("PATH")!=0 ) {
		QString command = _arguments[0];
		if ( !command.contains( QLatin1Char('/') ) ) {
		    QStringList pathList = QStringList::split( QLatin1Char(':'), QString::fromLocal8Bit(getenv( "PATH" )) );
		    for (QStringList::Iterator it = pathList.begin(); it != pathList.end(); ++it ) {
			QString dir = *it;
#if defined(Q_OS_MACX) //look in a bundle
			if(!QFile::exists(dir + QLatin1Char('/') + command) && QFile::exists(dir + QLatin1Char('/') + command + QLatin1String(".app")))
			    dir += QLatin1Char('/') + command + QLatin1String(".app/Contents/MacOS");
#endif
#ifndef QT_NO_DIR
			QFileInfo fileInfo( dir, command );
#else
			QFileInfo fileInfo( dir + "/" + command );
#endif
			if ( fileInfo.isExecutable() ) {
#if defined(Q_OS_MACX)
			    arglistQ[0] = fileInfo.absFilePath().local8Bit();
#else
			    arglistQ[0] = fileInfo.filePath().local8Bit();
#endif
			    arglist[0] = arglistQ[0];
			    break;
			}
		    }
		}
	    }
#ifndef Q_OS_QNX4
	    ::execve( arglist[0], (char*const*)arglist, (char*const*)envlist ); // ### casts not nice
#else
	    ::execve( arglist[0], (char const*const*)arglist,(char const*const*)envlist ); // ### casts not nice
#endif
	}
	if ( fd[1] ) {
	    char buf = 0;
	    ::write( fd[1], &buf, 1 );
	    ::close( fd[1] );
	}
	::_exit( -1 );
    } else if ( pid == -1 ) {
	// error forking
	goto error;
    }

    // test if exec was successful
    if ( fd[1] )
	::close( fd[1] );
    if ( fd[0] ) {
	char buf;
	for ( ;; ) {
	    int n = ::read( fd[0], &buf, 1 );
	    if ( n==1 ) {
		// socket was not closed => error
		if ( ::waitpid( pid, 0, WNOHANG ) != pid ) {
		    // The wait did not succeed yet, so try again when we get
		    // the sigchild (to avoid zombies).
		    d->newProc( pid, 0 );
		}
		d->proc = 0;
		goto error;
	    } else if ( n==-1 ) {
		if ( errno==EAGAIN || errno==EINTR )
		    // try it again
		    continue;
	    }
	    break;
	}
	::close( fd[0] );
    }

    d->newProc( pid, this );

    if ( comms & Stdin ) {
	::close( sStdin[0] );
	d->proc->socketStdin = sStdin[1];

	// Select non-blocking mode
	int originalFlags = fcntl(d->proc->socketStdin, F_GETFL, 0);
	fcntl(d->proc->socketStdin, F_SETFL, originalFlags | O_NONBLOCK);

	d->notifierStdin = new QSocketNotifier( sStdin[1], QSocketNotifier::Write );
	connect( d->notifierStdin, SIGNAL(activated(int)),
		this, SLOT(socketWrite(int)) );
	// setup notifiers for the sockets
	if ( !d->stdinBuf.isEmpty() ) {
	    d->notifierStdin->setEnabled( true );
	}
    }
    if ( comms & Stdout ) {
	::close( sStdout[1] );
	d->proc->socketStdout = sStdout[0];
	d->notifierStdout = new QSocketNotifier( sStdout[0], QSocketNotifier::Read );
	connect( d->notifierStdout, SIGNAL(activated(int)),
		this, SLOT(socketRead(int)) );
	if ( ioRedirection )
	    d->notifierStdout->setEnabled( true );
    }
    if ( comms & Stderr ) {
	::close( sStderr[1] );
	d->proc->socketStderr = sStderr[0];
	d->notifierStderr = new QSocketNotifier( sStderr[0], QSocketNotifier::Read );
	connect( d->notifierStderr, SIGNAL(activated(int)),
		this, SLOT(socketRead(int)) );
	if ( ioRedirection )
	    d->notifierStderr->setEnabled( true );
    }

    // cleanup and return
    delete[] arglistQ;
    delete[] arglist;
    return true;

error:
#if defined(QT_Q3PROCESS_DEBUG)
    qDebug( "Q3Process::start(): error starting process" );
#endif
    if ( d->procManager )
	d->procManager->cleanup();
    if ( comms & Stdin ) {
	::close( sStdin[1] );
	::close( sStdin[0] );
    }
    if ( comms & Stdout ) {
	::close( sStdout[0] );
	::close( sStdout[1] );
    }
    if ( comms & Stderr ) {
	::close( sStderr[0] );
	::close( sStderr[1] );
    }
    ::close( fd[0] );
    ::close( fd[1] );
    delete[] arglistQ;
    delete[] arglist;
    return false;
}


void Q3Process::tryTerminate() const
{
    if ( d->proc != 0 )
	::kill( d->proc->pid, SIGTERM );
}

void Q3Process::kill() const
{
    if ( d->proc != 0 )
	::kill( d->proc->pid, SIGKILL );
}

bool Q3Process::isRunning() const
{
    if ( d->exitValuesCalculated ) {
#if defined(QT_Q3PROCESS_DEBUG)
	qDebug( "Q3Process::isRunning(): false (already computed)" );
#endif
	return false;
    }
    if ( d->proc == 0 )
	return false;
    int status;
    if ( ::waitpid( d->proc->pid, &status, WNOHANG ) == d->proc->pid ) {
	// compute the exit values
	Q3Process *that = (Q3Process*)this; // mutable
	that->exitNormal = WIFEXITED( status ) != 0;
	if ( exitNormal ) {
	    that->exitStat = (char)WEXITSTATUS( status );
	}
	d->exitValuesCalculated = true;

	// On heavy processing, the socket notifier for the sigchild might not
	// have found time to fire yet.
	if ( d->procManager && d->procManager->sigchldFd[1] < FD_SETSIZE ) {
	    fd_set fds;
	    struct timeval tv;
	    FD_ZERO( &fds );
	    FD_SET( d->procManager->sigchldFd[1], &fds );
	    tv.tv_sec = 0;
	    tv.tv_usec = 0;
	    if ( ::select( d->procManager->sigchldFd[1]+1, &fds, 0, 0, &tv ) > 0 )
		d->procManager->sigchldHnd( d->procManager->sigchldFd[1] );
	}

#if defined(QT_Q3PROCESS_DEBUG)
	qDebug( "Q3Process::isRunning() (PID: %d): false", d->proc->pid );
#endif
	return false;
    }
#if defined(QT_Q3PROCESS_DEBUG)
    qDebug( "Q3Process::isRunning() (PID: %d): true", d->proc->pid );
#endif
    return true;
}

bool Q3Process::canReadLineStdout() const
{
    if ( !d->proc || !d->proc->socketStdout )
	return d->bufStdout.size() != 0;

    Q3Process *that = (Q3Process*)this;
    return that->membufStdout()->scanNewline( 0 );
}

bool Q3Process::canReadLineStderr() const
{
    if ( !d->proc || !d->proc->socketStderr )
	return d->bufStderr.size() != 0;

    Q3Process *that = (Q3Process*)this;
    return that->membufStderr()->scanNewline( 0 );
}

void Q3Process::writeToStdin( const QByteArray& buf )
{
#if defined(QT_Q3PROCESS_DEBUG)
//    qDebug( "Q3Process::writeToStdin(): write to stdin (%d)", d->socketStdin );
#endif
    d->stdinBuf.enqueue( new QByteArray(buf) );
    if ( d->notifierStdin != 0 )
	d->notifierStdin->setEnabled( true );
}


void Q3Process::closeStdin()
{
    if ( d->proc == 0 )
	return;
    if ( d->proc->socketStdin !=0 ) {
	while ( !d->stdinBuf.isEmpty() ) {
	    delete d->stdinBuf.dequeue();
	}
	d->notifierStdin->setEnabled(false);
	qDeleteInEventHandler(d->notifierStdin);
	d->notifierStdin = 0;
	if ( ::close( d->proc->socketStdin ) != 0 ) {
	    qWarning( "Could not close stdin of child process" );
	}
#if defined(QT_Q3PROCESS_DEBUG)
	qDebug( "Q3Process::closeStdin(): stdin (%d) closed", d->proc->socketStdin );
#endif
	d->proc->socketStdin = 0;
    }
}


/*
  This private slot is called when the process has outputted data to either
  standard output or standard error.
*/
void Q3Process::socketRead( int fd )
{
    if ( d->socketReadCalled ) {
	// the slots that are connected to the readyRead...() signals might
	// trigger a recursive call of socketRead(). Avoid this since you get a
	// blocking read otherwise.
	return;
    }

#if defined(QT_Q3PROCESS_DEBUG)
    qDebug( "Q3Process::socketRead(): %d", fd );
#endif
    if ( fd == 0 )
	return;
    if ( !d->proc )
	return;
    Q3Membuf *buffer = 0;
    int n;
    if ( fd == d->proc->socketStdout ) {
	buffer = &d->bufStdout;
    } else if ( fd == d->proc->socketStderr ) {
	buffer = &d->bufStderr;
    } else {
	// this case should never happen, but just to be safe
	return;
    }
#if defined(QT_Q3PROCESS_DEBUG)
    uint oldSize = buffer->size();
#endif

    // try to read data first (if it fails, the filedescriptor was closed)
    const int basize = 4096;
    QByteArray *ba = new QByteArray( basize );
    n = ::read( fd, ba->data(), basize );
    if ( n > 0 ) {
	ba->resize( n );
	buffer->append( ba );
	ba = 0;
    } else {
	delete ba;
	ba = 0;
    }
    // eof or error?
    if ( n == 0 || n == -1 ) {
	if ( fd == d->proc->socketStdout ) {
#if defined(QT_Q3PROCESS_DEBUG)
	    qDebug( "Q3Process::socketRead(): stdout (%d) closed", fd );
#endif
	    d->notifierStdout->setEnabled( false );
	    qDeleteInEventHandler(d->notifierStdout);
	    d->notifierStdout = 0;
	    ::close( d->proc->socketStdout );
	    d->proc->socketStdout = 0;
	    return;
	} else if ( fd == d->proc->socketStderr ) {
#if defined(QT_Q3PROCESS_DEBUG)
	    qDebug( "Q3Process::socketRead(): stderr (%d) closed", fd );
#endif
	    d->notifierStderr->setEnabled( false );
	    qDeleteInEventHandler(d->notifierStderr);
	    d->notifierStderr = 0;
	    ::close( d->proc->socketStderr );
	    d->proc->socketStderr = 0;
	    return;
	}
    }

    if ( fd < FD_SETSIZE ) {
	fd_set fds;
	struct timeval tv;
	FD_ZERO( &fds );
	FD_SET( fd, &fds );
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	while ( ::select( fd+1, &fds, 0, 0, &tv ) > 0 ) {
	    // prepare for the next round
	    FD_ZERO( &fds );
	    FD_SET( fd, &fds );
	    // read data
	    ba = new QByteArray( basize );
	    n = ::read( fd, ba->data(), basize );
	    if ( n > 0 ) {
		ba->resize( n );
		buffer->append( ba );
		ba = 0;
	    } else {
		delete ba;
		ba = 0;
		break;
	    }
	}
    }

    d->socketReadCalled = true;
    if ( fd == d->proc->socketStdout ) {
#if defined(QT_Q3PROCESS_DEBUG)
	qDebug( "Q3Process::socketRead(): %d bytes read from stdout (%d)",
		buffer->size()-oldSize, fd );
#endif
	emit readyReadStdout();
    } else if ( fd == d->proc->socketStderr ) {
#if defined(QT_Q3PROCESS_DEBUG)
	qDebug( "Q3Process::socketRead(): %d bytes read from stderr (%d)",
		buffer->size()-oldSize, fd );
#endif
	emit readyReadStderr();
    }
    d->socketReadCalled = false;
}


/*
  This private slot is called when the process tries to read data from standard
  input.
*/
void Q3Process::socketWrite( int fd )
{
    while ( fd == d->proc->socketStdin && d->proc->socketStdin != 0 ) {
	if ( d->stdinBuf.isEmpty() ) {
	    d->notifierStdin->setEnabled( false );
	    return;
	}
	ssize_t ret = ::write( fd,
		d->stdinBuf.head()->data() + d->stdinBufRead,
		d->stdinBuf.head()->size() - d->stdinBufRead );
#if defined(QT_Q3PROCESS_DEBUG)
	qDebug( "Q3Process::socketWrite(): wrote %d bytes to stdin (%d)", ret, fd );
#endif
	if ( ret == -1 )
	    return;
	d->stdinBufRead += ret;
	if ( d->stdinBufRead == (ssize_t)d->stdinBuf.head()->size() ) {
	    d->stdinBufRead = 0;
	    delete d->stdinBuf.dequeue();
	    if ( wroteToStdinConnected && d->stdinBuf.isEmpty() )
		emit wroteToStdin();
	}
    }
}

/*!
  \internal
  Flushes standard input. This is useful if you want to use Q3Process in a
  synchronous manner.

  This function should probably go into the public API.
*/
void Q3Process::flushStdin()
{
    if (d->proc)
        socketWrite(d->proc->socketStdin);
}

/*
  This private slot is only used under Windows (but moc does not know about #if
  defined()).
*/
void Q3Process::timeout()
{
}


/*
  This private function is used by connectNotify() and disconnectNotify() to
  change the value of ioRedirection (and related behaviour)
*/
void Q3Process::setIoRedirection( bool value )
{
    ioRedirection = value;
    if ( ioRedirection ) {
	if ( d->notifierStdout )
	    d->notifierStdout->setEnabled( true );
	if ( d->notifierStderr )
	    d->notifierStderr->setEnabled( true );
    } else {
	if ( d->notifierStdout )
	    d->notifierStdout->setEnabled( false );
	if ( d->notifierStderr )
	    d->notifierStderr->setEnabled( false );
    }
}

/*
  This private function is used by connectNotify() and
  disconnectNotify() to change the value of notifyOnExit (and related
  behaviour)
*/
void Q3Process::setNotifyOnExit( bool value )
{
    notifyOnExit = value;
}

/*
  This private function is used by connectNotify() and disconnectNotify() to
  change the value of wroteToStdinConnected (and related behaviour)
*/
void Q3Process::setWroteStdinConnected( bool value )
{
    wroteToStdinConnected = value;
}

/*!
    \typedef Q3Process::PID
    \internal
*/

Q3Process::PID Q3Process::processIdentifier()
{
    if ( d->proc == 0 )
	return -1;
    return d->proc->pid;
}

QT_END_NAMESPACE

#endif // QT_NO_PROCESS
