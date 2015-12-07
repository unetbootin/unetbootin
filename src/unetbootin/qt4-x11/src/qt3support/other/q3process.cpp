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

#include "q3process.h"

#ifndef QT_NO_PROCESS

#include "qapplication.h"
#include "private/q3membuf_p.h"

#include <stdio.h>
#include <stdlib.h>

QT_BEGIN_NAMESPACE

//#define QT_Q3PROCESS_DEBUG


/*!
    \class Q3Process q3process.h

    \brief The Q3Process class is used to start external programs and
    to communicate with them.

    \compat

    You can write to the started program's standard input, and can
    read the program's standard output and standard error. You can
    pass command line arguments to the program either in the
    constructor or with setArguments() or addArgument(). The program's
    working directory can be set with setWorkingDirectory(). If you
    need to set up environment variables pass them to the start() or
    launch() functions (see below). The processExited() signal is
    emitted if the program exits. The program's exit status is
    available from exitStatus(), although you could simply call
    normalExit() to see if the program terminated normally.

    There are two different ways to start a process. If you just want
    to run a program, optionally passing data to its standard input at
    the beginning, use one of the launch() functions. If you want full
    control of the program's standard input (especially if you don't
    know all the data you want to send to standard input at the
    beginning), use the start() function.

    If you use start() you can write to the program's standard input
    using writeToStdin() and you can close the standard input with
    closeStdin(). The wroteToStdin() signal is emitted if the data
    sent to standard input has been written. You can read from the
    program's standard output using readStdout() or readLineStdout().
    These functions return an empty QByteArray if there is no data to
    read. The readyReadStdout() signal is emitted when there is data
    available to be read from standard output. Standard error has a
    set of functions that correspond to the standard output functions,
    i.e. readStderr(), readLineStderr() and readyReadStderr().

    If you use one of the launch() functions the data you pass will be
    sent to the program's standard input which will be closed once all
    the data has been written. You should \e not use writeToStdin() or
    closeStdin() if you use launch(). If you need to send data to the
    program's standard input after it has started running use start()
    instead of launch().

    Both start() and launch() can accept a string list of strings each
    of which has the format, key=value, where the keys are the names
    of environment variables.

    You can test to see if a program is running with isRunning(). The
    program's process identifier is available from
    processIdentifier(). If you want to terminate a running program
    use tryTerminate(), but note that the program may ignore this. If
    you \e really want to terminate the program, without it having any
    chance to clean up, you can use kill().

    Although you may need quotes for a file named on the command line
    (e.g. if it contains spaces) you shouldn't use extra quotes for
    arguments passed to addArgument() or setArguments().

    The readyReadStdout() signal is emitted when there is new data on
    standard output. This happens asynchronously: you don't know if
    more data will arrive later.

    In the above example you could connect the processExited() signal
    to the slot UicManager::readFromStdout() instead. If you do so,
    you will be certain that all the data is available when the slot
    is called. On the other hand, you must wait until the process has
    finished before doing any processing.

    Note that if you are expecting a lot of output from the process,
    you may hit platform-dependent limits to the pipe buffer size. The
    solution is to make sure you connect to the output, e.g. the
    readyReadStdout() and readyReadStderr() signals and read the data
    as soon as it becomes available.

    Please note that Q3Process does not emulate a shell. This means that
    Q3Process does not do any expansion of arguments: a '*' is passed as a '*'
    to the program and is \e not replaced by all the files, a '$HOME' is also
    passed literally and is \e not replaced by the environment variable HOME
    and the special characters for IO redirection ('>', '|', etc.) are also
    passed literally and do \e not have the special meaning as they have in a
    shell.

    Also note that Q3Process does not emulate a terminal. This means that
    certain programs which need direct terminal control, do not work as
    expected with Q3Process. Such programs include console email programs (like
    pine and mutt) but also programs which require the user to enter a password
    (like su and ssh).

    \section1 Notes for Windows users

    Some Windows commands, for example, \c dir, are not provided by
    separate applications, but by the command interpreter.
    If you attempt to use Q3Process to execute these commands directly
    it won't work. One possible solution is to execute the command
    interpreter itself (\c cmd.exe on some Windows systems), and ask
    the interpreter to execute the desired command.

    Under Windows there are certain problems starting 16-bit applications
    and capturing their output. Microsoft recommends using an intermediate
    application to start 16-bit applications.

    \sa Q3Socket
*/

/*!
    \enum Q3Process::Communication

    This enum type defines the communication channels connected to the
    process.

    \value Stdin  Data can be written to the process's standard input.

    \value Stdout  Data can be read from the process's standard
    output.

    \value Stderr  Data can be read from the process's standard error.

    \value DupStderr  Both the process's standard error output \e and
    its standard output are written to its standard output. (Like
    Unix's dup2().) This means that nothing is sent to the standard
    error output. This is especially useful if your application
    requires that the output on standard output and on standard error
    must be read in the same order that they are produced. This is a
    flag, so to activate it you must pass \c{Stdout|Stderr|DupStderr},
    or \c{Stdin|Stdout|Stderr|DupStderr} if you want to provide input,
    to the setCommunication() call.

    \sa setCommunication() communication()
*/

/*!
    Constructs a Q3Process object. The \a parent and \a name parameters
    are passed to the QObject constructor.

    \sa setArguments() addArgument() start()
*/
Q3Process::Q3Process( QObject *parent, const char *name )
    : QObject( parent, name ), ioRedirection( false ), notifyOnExit( false ),
    wroteToStdinConnected( false ),
    readStdoutCalled( false ), readStderrCalled( false ),
    comms( Stdin|Stdout|Stderr )
{
    init();
}

/*!
    Constructs a Q3Process with \a arg0 as the command to be executed.
    The \a parent and \a name parameters are passed to the QObject
    constructor.

    The process is not started. You must call start() or launch() to
    start the process.

    \sa setArguments() addArgument() start()
*/
Q3Process::Q3Process( const QString& arg0, QObject *parent, const char *name )
    : QObject( parent, name ), ioRedirection( false ), notifyOnExit( false ),
    wroteToStdinConnected( false ),
    readStdoutCalled( false ), readStderrCalled( false ),
    comms( Stdin|Stdout|Stderr )
{
    init();
    addArgument( arg0 );
}

/*!
    Constructs a Q3Process with \a args as the arguments of the
    process. The first element in the list is the command to be
    executed. The other elements in the list are the arguments to this
    command. The \a parent and \a name parameters are passed to the
    QObject constructor.

    The process is not started. You must call start() or launch() to
    start the process.

    \sa setArguments() addArgument() start()
*/
Q3Process::Q3Process( const QStringList& args, QObject *parent, const char *name )
    : QObject( parent, name ), ioRedirection( false ), notifyOnExit( false ),
    wroteToStdinConnected( false ),
    readStdoutCalled( false ), readStderrCalled( false ),
    comms( Stdin|Stdout|Stderr )
{
    init();
    setArguments( args );
}

/*!
    \fn Q3Process::~Q3Process()

    Destroys the instance.

    If the process is running, it is <b>not</b> terminated! The
    standard input, standard output and standard error of the process
    are closed.

    You can connect the destroyed() signal to the kill() slot, if you
    want the process to be terminated automatically when the instance
    is destroyed.

    \sa tryTerminate() kill()
*/

/*!
    Returns the list of arguments that are set for the process.
    Arguments can be specified with the constructor or with the
    functions setArguments() and addArgument().

    Note that if you want to iterate over the list, you should iterate
    over a copy, e.g.
    \snippet doc/src/snippets/code/src_qt3support_other_q3process.cpp 0

    \sa setArguments() addArgument()
*/
QStringList Q3Process::arguments() const
{
    return _arguments;
}

/*!
    Clears the list of arguments that are set for the process.

    \sa setArguments() addArgument()
*/
void Q3Process::clearArguments()
{
    _arguments.clear();
}

/*!
    Sets \a args as the arguments for the process. The first element
    in the list is the command to be executed. The other elements in
    the list are the arguments to the command. Any previous arguments
    are deleted.

    Q3Process does not perform argument substitutions; for example, if you
    specify "*" or "$DISPLAY", these values are passed to the process
    literally. If you want to have the same behavior as the shell
    provides, you must do the substitutions yourself; i.e. instead of
    specifying a "*" you must specify the list of all the filenames in
    the current directory, and instead of "$DISPLAY" you must specify
    the value of the environment variable \c DISPLAY.

    Note for Windows users. The standard Windows shells, e.g. \c
    command.com and \c cmd.exe, do not perform file globbing, i.e.
    they do not convert a "*" on the command line into a list of files
    in the current directory. For this reason most Windows
    applications implement their own file globbing, and as a result of
    this, specifying an argument of "*" for a Windows application is
    likely to result in the application performing a file glob and
    ending up with a list of filenames.

    \sa arguments() addArgument()
*/
void Q3Process::setArguments( const QStringList& args )
{
    _arguments = args;
}

/*!
    Adds \a arg to the end of the list of arguments.

    The first element in the list of arguments is the command to be
    executed; the following elements are the command's arguments.

    \sa arguments() setArguments()
*/
void Q3Process::addArgument( const QString& arg )
{
    _arguments.append( arg );
}

#ifndef QT_NO_DIR
/*!
    Returns the working directory that was set with
    setWorkingDirectory(), or the current directory if none has been
    explicitly set.

    \sa setWorkingDirectory() QDir::current()
*/
QDir Q3Process::workingDirectory() const
{
    return workingDir;
}

/*!
    Sets \a dir as the working directory for processes. This does not
    affect running processes; only processes that are started
    afterwards are affected.

    Setting the working directory is especially useful for processes
    that try to access files with relative paths.

    \sa workingDirectory() start()
*/
void Q3Process::setWorkingDirectory( const QDir& dir )
{
    workingDir = dir;
}
#endif //QT_NO_DIR

/*!
    Returns the communication required with the process, i.e. some
    combination of the \c Communication flags.

    \sa setCommunication()
*/
int Q3Process::communication() const
{
    return comms;
}

/*!
    Sets \a commFlags as the communication required with the process.

    \a commFlags is a bitwise OR of the flags defined by the \c
    Communication enum.

    The default is \c{Stdin|Stdout|Stderr}.

    \sa communication()
*/
void Q3Process::setCommunication( int commFlags )
{
    comms = commFlags;
}

/*!
    Returns true if the process has exited normally; otherwise returns
    false. This implies that this function returns false if the
    process is still running.

    \sa isRunning() exitStatus() processExited()
*/
bool Q3Process::normalExit() const
{
    // isRunning() has the side effect that it determines the exit status!
    if ( isRunning() )
	return false;
    else
	return exitNormal;
}

/*!
    Returns the exit status of the process or 0 if the process is
    still running. This function returns immediately and does not wait
    until the process is finished.

    If normalExit() is false (e.g. if the program was killed or
    crashed), this function returns 0, so you should check the return
    value of normalExit() before relying on this value.

    \sa normalExit() processExited()
*/
int Q3Process::exitStatus() const
{
    // isRunning() has the side effect that it determines the exit status!
    if ( isRunning() )
	return 0;
    else
	return exitStat;
}


/*!
    Reads the data that the process has written to standard output.
    When new data is written to standard output, the class emits the
    signal readyReadStdout().

    If there is no data to read, this function returns a QByteArray of
    size 0: it does not wait until there is something to read.

    \sa readyReadStdout() readLineStdout() readStderr() writeToStdin()
*/
QByteArray Q3Process::readStdout()
{
    if ( readStdoutCalled ) {
	return QByteArray();
    }
    readStdoutCalled = true;
    Q3Membuf *buf = membufStdout();
    readStdoutCalled = false;

    return buf->readAll();
}

/*!
    Reads the data that the process has written to standard error.
    When new data is written to standard error, the class emits the
    signal readyReadStderr().

    If there is no data to read, this function returns a QByteArray of
    size 0: it does not wait until there is something to read.

    \sa readyReadStderr() readLineStderr() readStdout() writeToStdin()
*/
QByteArray Q3Process::readStderr()
{
    if ( readStderrCalled ) {
	return QByteArray();
    }
    readStderrCalled = true;
    Q3Membuf *buf = membufStderr();
    readStderrCalled = false;

    return buf->readAll();
}

/*!
    Reads a line of text from standard output, excluding any trailing
    newline or carriage return characters, and returns it. Returns
    an empty string if canReadLineStdout() returns false.

    By default, the text is interpreted to be in Latin-1 encoding. If you need
    other codecs, you can set a different codec with
    QTextCodec::setCodecForCStrings().

    \sa canReadLineStdout() readyReadStdout() readStdout() readLineStderr()
*/
QString Q3Process::readLineStdout()
{
    QByteArray a( 256 );
    Q3Membuf *buf = membufStdout();
    if ( !buf->scanNewline( &a ) ) {
      if ( !canReadLineStdout() )
	return QString();

      if ( !buf->scanNewline( &a ) )
	return QString( QLatin1String(buf->readAll()) );
    }

    uint size = a.size();
    buf->consumeBytes( size, 0 );

    // get rid of terminating \n or \r\n
    if ( size>0 && a.at( size - 1 ) == '\n' ) {
      if ( size>1 && a.at( size - 2 ) == '\r' )
	a.chop(2);
      else
	a.chop(1);
    }
    return QString(QString::fromLatin1(a.constData()));
}

/*!
    Reads a line of text from standard error, excluding any trailing
    newline or carriage return characters and returns it. Returns
    an empty string if canReadLineStderr() returns false.

    By default, the text is interpreted to be in Latin-1 encoding. If you need
    other codecs, you can set a different codec with
    QTextCodec::setCodecForCStrings().

    \sa canReadLineStderr() readyReadStderr() readStderr() readLineStdout()
*/
QString Q3Process::readLineStderr()
{
    QByteArray a( 256 );
    Q3Membuf *buf = membufStderr();
    if ( !buf->scanNewline( &a ) ) {
      if ( !canReadLineStderr() )
	return QString();

      if ( !buf->scanNewline( &a ) )
	return QString( QString::fromLatin1( buf->readAll().constData() ) );
    }

    uint size = a.size();
    buf->consumeBytes( size, 0 );

    // get rid of terminating \n or \r\n
    if ( size>0 && a.at( size - 1 ) == '\n' ) {
      if ( size>1 && a.at( size - 2 ) == '\r' )
	a.chop(2);
      else
	a.chop(1);
    }
    return QString( QString::fromLatin1( a.constData() ) );
}

/*!
    \fn bool Q3Process::start( QStringList *env )

    Tries to run a process for the command and arguments that were
    specified with setArguments(), addArgument() or that were
    specified in the constructor. The command is searched for in the
    path for executable programs; you can also use an absolute path in
    the command itself.

    If \a env is null, then the process is started with the same
    environment as the starting process. If \a env is non-null, then
    the values in the stringlist are interpreted as environment
    setttings of the form \c {key=value} and the process is started in
    these environment settings. For convenience, there is a small
    exception to this rule: under Unix, if \a env does not contain any
    settings for the environment variable \c LD_LIBRARY_PATH, then
    this variable is inherited from the starting process; under
    Windows the same applies for the environment variable \c PATH.

    Returns true if the process could be started; otherwise returns
    false.

    You can write data to the process's standard input with
    writeToStdin(). You can close standard input with closeStdin() and
    you can terminate the process with tryTerminate(), or with kill().

    You can call this function even if you've used this instance to
    create a another process which is still running. In such cases,
    Q3Process closes the old process's standard input and deletes
    pending data, i.e., you lose all control over the old process, but
    the old process is not terminated. This applies also if the
    process could not be started. (On operating systems that have
    zombie processes, Qt will also wait() on the old process.)

    \sa launch() closeStdin()
*/

/*!
    \fn void Q3Process::tryTerminate() const

    Asks the process to terminate. Processes can ignore this if they
    wish. If you want to be certain that the process really
    terminates, you can use kill() instead.

    The slot returns immediately: it does not wait until the process
    has finished. When the process terminates, the processExited()
    signal is emitted.

    \sa kill() processExited()
*/

/*!
    \fn void Q3Process::kill() const

    Terminates the process. This is not a safe way to end a process
    since the process will not be able to do any cleanup.
    tryTerminate() is safer, but processes can ignore a
    tryTerminate().

    The nice way to end a process and to be sure that it is finished,
    is to do something like this:
    \snippet doc/src/snippets/code/src_qt3support_other_q3process_unix.cpp 0

    This tries to terminate the process the nice way. If the process
    is still running after 5 seconds, it terminates the process the
    hard way. The timeout should be chosen depending on the time the
    process needs to do all its cleanup: use a higher value if the
    process is likely to do a lot of computation or I/O on cleanup.

    The slot returns immediately: it does not wait until the process
    has finished. When the process terminates, the processExited()
    signal is emitted.

    \sa tryTerminate() processExited()
*/

/*!
    \fn bool Q3Process::isRunning() const

    Returns true if the process is running; otherwise returns false.

    \sa normalExit() exitStatus() processExited()
*/

/*!
    \fn bool Q3Process::canReadLineStdout() const

    Returns true if it's possible to read an entire line of text from
    standard output at this time; otherwise returns false.

    \sa readLineStdout() canReadLineStderr()
*/

/*!
    \fn bool Q3Process::canReadLineStderr() const

    Returns true if it's possible to read an entire line of text from
    standard error at this time; otherwise returns false.

    \sa readLineStderr() canReadLineStdout()
*/

/*!
    \fn void Q3Process::writeToStdin( const QByteArray& buf )

    Writes the data \a buf to the process's standard input. The
    process may or may not read this data.

    This function returns immediately; the Q3Process class might write
    the data at a later point (you must enter the event loop for this
    to occur). When all the data is written to the process, the signal
    wroteToStdin() is emitted. This does not mean that the process
    actually read the data, since this class only detects when it was
    able to write the data to the operating system.

    \sa wroteToStdin() closeStdin() readStdout() readStderr()
*/

/*!
    \fn void Q3Process::closeStdin()

    Closes the process's standard input.

    This function also deletes any pending data that has not been
    written to standard input.

    \sa wroteToStdin()
*/

/*!
    \fn Q3Process::PID Q3Process::processIdentifier()

    Returns platform dependent information about the process. This can
    be used together with platform specific system calls.

    Under Unix the return value is the PID of the process, or -1 if no
    process belongs to this object.

    Under Windows it is a pointer to the \c PROCESS_INFORMATION
    struct, or 0 if no process is belongs to this object.

    Use of this function's return value is likely to be non-portable.
*/

/*!
    \fn void Q3Process::launchFinished()

    This signal is emitted when the process was started with launch().
    If the start was successful, this signal is emitted after all the
    data has been written to standard input. If the start failed, then
    this signal is emitted immediately.

    This signal is especially useful if you want to know when you can
    safely delete the Q3Process object when you are not interested in
    reading from standard output or standard error.

    \sa launch() QObject::deleteLater()
*/

/*!
    Runs the process and writes the data \a buf to the process's
    standard input. If all the data is written to standard input,
    standard input is closed. The command is searched for in the path
    for executable programs; you can also use an absolute path in the
    command itself.

    If \a env is null, then the process is started with the same
    environment as the starting process. If \a env is non-null, then
    the values in the string list are interpreted as environment
    setttings of the form \c {key=value} and the process is started
    with these environment settings. For convenience, there is a small
    exception to this rule under Unix: if \a env does not contain any
    settings for the environment variable \c LD_LIBRARY_PATH, then
    this variable is inherited from the starting process.

    Returns true if the process could be started; otherwise returns
    false.

    Note that you should not use the slots writeToStdin() and
    closeStdin() on processes started with launch(), since the result
    is not well-defined. If you need these slots, use start() instead.

    The process may or may not read the \a buf data sent to its
    standard input.

    You can call this function even when a process that was started
    with this instance is still running. Be aware that if you do this
    the standard input of the process that was launched first will be
    closed, with any pending data being deleted, and the process will
    be left to run out of your control. Similarly, if the process
    could not be started the standard input will be closed and the
    pending data deleted. (On operating systems that have zombie
    processes, Qt will also wait() on the old process.)

    The object emits the signal launchFinished() when this function
    call is finished. If the start was successful, this signal is
    emitted after all the data has been written to standard input. If
    the start failed, then this signal is emitted immediately.

    \sa start() launchFinished()
*/
bool Q3Process::launch( const QByteArray& buf, QStringList *env )
{
    if ( start( env ) ) {
	if ( !buf.isEmpty() ) {
	    connect( this, SIGNAL(wroteToStdin()),
		    this, SLOT(closeStdinLaunch()) );
	    writeToStdin( buf );
	} else {
	    closeStdin();
	    emit launchFinished();
	}
	return true;
    } else {
	emit launchFinished();
	return false;
    }
}

/*!
    \overload

    The data \a buf is written to standard input with writeToStdin()
    using the QString::local8Bit() representation of the strings.
*/
bool Q3Process::launch( const QString& buf, QStringList *env )
{
    if ( start( env ) ) {
	if ( !buf.isEmpty() ) {
	    connect( this, SIGNAL(wroteToStdin()),
		    this, SLOT(closeStdinLaunch()) );
	    writeToStdin( buf );
	} else {
	    closeStdin();
	    emit launchFinished();
	}
	return true;
    } else {
	emit launchFinished();
	return false;
    }
}

/*
  This private slot is used by the launch() functions to close standard input.
*/
void Q3Process::closeStdinLaunch()
{
    disconnect( this, SIGNAL(wroteToStdin()),
	    this, SLOT(closeStdinLaunch()) );
    closeStdin();
    emit launchFinished();
}


/*!
    \fn void Q3Process::readyReadStdout()

    This signal is emitted when the process has written data to
    standard output. You can read the data with readStdout().

    Note that this signal is only emitted when there is new data and
    not when there is old, but unread data. In the slot connected to
    this signal, you should always read everything that is available
    at that moment to make sure that you don't lose any data.

    \sa readStdout() readLineStdout() readyReadStderr()
*/

/*!
    \fn void Q3Process::readyReadStderr()

    This signal is emitted when the process has written data to
    standard error. You can read the data with readStderr().

    Note that this signal is only emitted when there is new data and
    not when there is old, but unread data. In the slot connected to
    this signal, you should always read everything that is available
    at that moment to make sure that you don't lose any data.

    \sa readStderr() readLineStderr() readyReadStdout()
*/

/*!
    \fn void Q3Process::processExited()

    This signal is emitted when the process has exited.

    \sa isRunning() normalExit() exitStatus() start() launch()
*/

/*!
    \fn void Q3Process::wroteToStdin()

    This signal is emitted if the data sent to standard input (via
    writeToStdin()) was actually written to the process. This does not
    imply that the process really read the data, since this class only
    detects when it was able to write the data to the operating
    system. But it is now safe to close standard input without losing
    pending data.

    \sa writeToStdin() closeStdin()
*/


/*!
    \overload

    The string \a buf is handled as text using the
    QString::local8Bit() representation.
*/
void Q3Process::writeToStdin( const QString& buf )
{
    QByteArray tmp = buf.local8Bit();
    tmp.resize( buf.length() );
    writeToStdin( tmp );
}


/*
 * Under Windows the implementation is not so nice: it is not that easy to
 * detect when one of the signals should be emitted; therefore there are some
 * timers that query the information.
 * To keep it a little efficient, use the timers only when they are needed.
 * They are needed, if you are interested in the signals. So use
 * connectNotify() and disconnectNotify() to keep track of your interest.
 */
/*!  \reimp
*/
void Q3Process::connectNotify( const char * signal )
{
#if defined(QT_Q3PROCESS_DEBUG)
    qDebug( "Q3Process::connectNotify(): signal %s has been connected", signal );
#endif
    if ( !ioRedirection )
	if ( qstrcmp( signal, SIGNAL(readyReadStdout()) )==0 ||
		qstrcmp( signal, SIGNAL(readyReadStderr()) )==0
	   ) {
#if defined(QT_Q3PROCESS_DEBUG)
	    qDebug( "Q3Process::connectNotify(): set ioRedirection to true" );
#endif
	    setIoRedirection( true );
	    return;
	}
    if ( !notifyOnExit && qstrcmp( signal, SIGNAL(processExited()) )==0 ) {
#if defined(QT_Q3PROCESS_DEBUG)
	qDebug( "Q3Process::connectNotify(): set notifyOnExit to true" );
#endif
	setNotifyOnExit( true );
	return;
    }
    if ( !wroteToStdinConnected && qstrcmp( signal, SIGNAL(wroteToStdin()) )==0 ) {
#if defined(QT_Q3PROCESS_DEBUG)
	qDebug( "Q3Process::connectNotify(): set wroteToStdinConnected to true" );
#endif
	setWroteStdinConnected( true );
	return;
    }
}

/*!  \reimp
*/
void Q3Process::disconnectNotify( const char * )
{
    if ( ioRedirection &&
	    receivers( SIGNAL(readyReadStdout()) ) ==0 &&
	    receivers( SIGNAL(readyReadStderr()) ) ==0
	    ) {
#if defined(QT_Q3PROCESS_DEBUG)
	qDebug( "Q3Process::disconnectNotify(): set ioRedirection to false" );
#endif
	setIoRedirection( false );
    }
    if ( notifyOnExit && receivers( SIGNAL(processExited()) ) == 0 ) {
#if defined(QT_Q3PROCESS_DEBUG)
	qDebug( "Q3Process::disconnectNotify(): set notifyOnExit to false" );
#endif
	setNotifyOnExit( false );
    }
    if ( wroteToStdinConnected && receivers( SIGNAL(wroteToStdin()) ) == 0 ) {
#if defined(QT_Q3PROCESS_DEBUG)
	qDebug( "Q3Process::disconnectNotify(): set wroteToStdinConnected to false" );
#endif
	setWroteStdinConnected( false );
    }
}

QT_END_NAMESPACE

#endif // QT_NO_PROCESS
