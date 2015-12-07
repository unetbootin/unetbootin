/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt Assistant of the Qt Toolkit.
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

#include "qassistantclient.h"

#include <qtcpsocket.h>
#include <qtextstream.h>
#include <qtimer.h>
#include <qfileinfo.h>
#include <qmap.h>

QT_BEGIN_NAMESPACE

class QAssistantClientPrivate
{
    friend class QAssistantClient;
    QStringList arguments;
};

static QMap<const QAssistantClient*,QAssistantClientPrivate*> *dpointers = 0;

static QAssistantClientPrivate *data( const QAssistantClient *client, bool create=false )
{
    if( !dpointers )
        dpointers = new QMap<const QAssistantClient*,QAssistantClientPrivate*>;
    QAssistantClientPrivate *d = (*dpointers)[client];
    if( !d && create ) {
        d = new QAssistantClientPrivate;
        dpointers->insert( client, d );
    }
    return d;
}

/*!
    \class QAssistantClient
    \obsolete
    \brief The QAssistantClient class provides a means of using Qt
    Assistant as an application's help tool.

    \inmodule QtAssistant
    \ingroup helpsystem

    \bold{Note:} \e{This class is obsolete and only required when using
    the old Qt Assistant, now called assistant_adp. If you want to use
    the new Qt Assistant as a remote help viewer, simple create a
    QProcess instance and specify \tt{assistant} as its executable.
    The following code shows how to start Qt Assistant and request a
    certain page to be shown:}

    \snippet doc/src/snippets/code/tools_assistant_compat_lib_qassistantclient.cpp 0
    
    \e{For a complete example using the Qt Assistant remotely, see the \l
    {help/remotecontrol}{Remote Control} example.}
    
    In order to make Qt Assistant act as a customized help tool for
    your application, you must provide your application with a
    QAssistantClient object in addition to a \l
    {assistant-manual.html} {Qt Assistant Document Profile} (\c .adp
    file) and the associated documentation.

    Note that the QAssistantClient class is not included in the Qt
    library. To use it you must add the following line to your pro
    file:

    \snippet doc/src/snippets/code/tools_assistant_compat_lib_qassistantclient.cpp 1

    A QAssistantClient instance can open or close Qt Assistant
    whenever it is required.

    Once you have created a QAssistantClient instance, specifying the
    path to the Qt Assistant executable, using Qt Assistant is
    simple: You can either call the openAssistant() slot to show the
    defined start page of the documentation, or you can call the
    showPage() slot to show a particular help page. When you call
    openAssistant() and showPage(), Qt Assistant will be launched if
    it isn't already running. When Qt Assistant is running, the
    isOpen() function returns true.

    When calling showPage() the Qt Assistant instance will also be
    brought to the foreground if its hidden. The showPage() slot can
    be called multiple times, while calling openAssistant() several
    times without closing the application in between, will have no
    effect.

    You can close Qt Assistant at any time using the closeAssistant()
    slot. When you call openAssistant(), or you call showPage()
    without a previous call to openAssistant(), the assistantOpened()
    signal is emitted. Similarly when closeAssistant() is called,
    assistantClosed() is emitted. In either case, if an error occurs,
    error() is emitted.

    One QAssistantClient instance interacts with one Qt Assistant
    instance, so every time you call openAssistant(), showPage() or
    closeAssistant() they are applied to the particular Qt Assistant
    instance associated with the QAssistantClient.

    Qt Assistant's documentation set can be altered using the command
    line arguments that are passed to the application when it is
    launched. When started without any options, Qt Assistant displays
    a default set of documentation. When Qt is installed, the default
    documentation set in Qt Assistant contains the Qt reference
    documentation as well as the tools that come with Qt, such as \QD
    and \c qmake.

    Use the setArguments() function to specify the command line
    arguments. You can add or remove documentation from Qt Assistant
    by adding and removing the relevant content files: The command
    line arguments are \c {-addContentFile file.dcf} and \c
    {-removeContentFile file.dcf} respectively. You can make Qt
    Assistant run customized documentation sets that are separate from
    the Qt documentation, by specifying a profile: \c {-profile
    myapplication.adp}. The profile format can also be used to alter
    several of Qt Assistant's properties such as its title and
    startpage.

    The Documentation Content File (\c .dcf) and Qt Assistant
    Documentation Profile (\c .adp) formats are documented in the \l
    {assistant-manual.html}{Qt Assistant Manual}.

    For a complete example using the QAssistantClient class, see the
    \e{Simple Text Viewer} example. The example shows how you can make
    Qt Assistant act as a customized help tool for your application
    using the QAssistantClient class combined with a Qt Assistant
    Document Profile.

    \sa {Qt Assistant Manual}, {Simple Text Viewer Example}
*/

/*!
    \fn void QAssistantClient::assistantOpened()

    This signal is emitted when Qt Assistant is opened and the
    client-server communication is set up.

    \sa openAssistant(), showPage()
*/

/*!
    \fn void QAssistantClient::assistantClosed()

    This signal is emitted when the connection to Qt Assistant is
    closed. This happens when the user exits Qt Assistant, if an
    error in the server or client occurs, or if closeAssistant() is
    called.

    \sa closeAssistant()
*/

/*!
    \fn void QAssistantClient::error( const QString &message )

    This signal is emitted if Qt Assistant cannot be started, or if an
    error occurs during the initialization of the connection between
    Qt Assistant and the calling application. The \a message provides an
    explanation of the error.
*/

/*!
  Constructs an assistant client with the specified \a parent. For
  systems other than Mac OS, \a path specifies the path to the Qt
  Assistant executable. For Mac OS, \a path specifies a directory
  containing a valid assistant.app bundle. If \a path is the empty
  string, the system path (\c{%PATH%} or \c $PATH) is used.
*/
QAssistantClient::QAssistantClient( const QString &path, QObject *parent )
    : QObject( parent ), host ( QLatin1String("localhost") )
{
#if defined(Q_OS_MAC)
    const QString assistant = QLatin1String("Assistant_adp");
#else
    const QString assistant = QLatin1String("assistant_adp");
#endif

    if ( path.isEmpty() )
        assistantCommand = assistant;
    else {
        QFileInfo fi( path );
        if ( fi.isDir() )
            assistantCommand = path + QLatin1String("/") + assistant;
        else
            assistantCommand = path;
    }

#if defined(Q_OS_MAC)
    assistantCommand += QLatin1String(".app/Contents/MacOS/Assistant_adp");
#endif

    socket = new QTcpSocket( this );
    connect( socket, SIGNAL(connected()),
            SLOT(socketConnected()) );
    connect( socket, SIGNAL(disconnected()),
            SLOT(socketConnectionClosed()) );
    connect( socket, SIGNAL(error(QAbstractSocket::SocketError)),
             SLOT(socketError()) );
    opened = false;
    proc = new QProcess( this );
    port = 0;
    pageBuffer = QLatin1String("");
    connect( proc, SIGNAL(readyReadStandardError()),
             this, SLOT(readStdError()) );
    connect( proc, SIGNAL(error(QProcess::ProcessError)),
        this, SLOT(procError(QProcess::ProcessError)) );
}

/*!
    Destroys the assistant client object.
*/
QAssistantClient::~QAssistantClient()
{
    if ( proc->state() == QProcess::Running )
        proc->terminate();

    if( dpointers ) {
        QAssistantClientPrivate *d = (*dpointers)[ this ];
        if ( d ) {
            dpointers->remove(this);
            delete d;
            if( dpointers->isEmpty() ) {
                delete dpointers;
                dpointers = 0;
            }
        }
    }
}

/*!
    Opens Qt Assistant, i.e. sets up the client-server communication
    between the application and Qt Assistant, and shows the start page
    specified by the current \l {assistant-manual.html}
    {Qt Assistant Document Profile}. If there is no specfied profile,
    and Qt is installed, the default start page is the Qt Reference
    Documentation's index page.

    If the connection is already established, this function does
    nothing. Use the showPage() function to show another page. If an
    error occurs, the error() signal is emitted.

    \sa showPage(), assistantOpened()
*/
void QAssistantClient::openAssistant()
{
    if ( proc->state() == QProcess::Running )
        return;

    QStringList args;
    args.append(QLatin1String("-server"));
    if( !pageBuffer.isEmpty() ) {
        args.append( QLatin1String("-file") );
        args.append( pageBuffer );
    }

    QAssistantClientPrivate *d = data( this );
    if( d ) {
        QStringList::ConstIterator it = d->arguments.constBegin();
        while( it!=d->arguments.constEnd() ) {
            args.append( *it );
            ++it;
        }
    }

    connect( proc, SIGNAL(readyReadStandardOutput()),
        this, SLOT(readPort()) );

    proc->start(assistantCommand, args);
}

void QAssistantClient::procError(QProcess::ProcessError err)
{
    switch (err)
    {
    case QProcess::FailedToStart:
        emit error( tr( "Failed to start Qt Assistant." ) );
        break;
    case QProcess::Crashed:
        emit error( tr( "Qt Assistant crashed." ) );
        break;
    default:
        emit error( tr( "Error while running Qt Assistant." ) );
    }
}

void QAssistantClient::readPort()
{
    QString p(QString::fromLatin1(proc->readAllStandardOutput()));
    quint16 port = p.toUShort();
    if ( port == 0 ) {
        emit error( tr( "Cannot connect to Qt Assistant." ) );
        return;
    }
    socket->connectToHost( host, port );
    disconnect( proc, SIGNAL(readyReadStandardOutput()),
                this, SLOT(readPort()) );
}

/*!
    Closes the Qt Assistant instance.

    \sa openAssistant(), assistantClosed()
*/
void QAssistantClient::closeAssistant()
{
    if ( !opened )
        return;

    bool blocked = proc->blockSignals(true);
    proc->terminate();
    if (!proc->waitForFinished(2000)) {
        // If the process hasn't died after 2 seconds,
        // we kill it, causing it to exit immediately.
        proc->kill();
    }
    proc->blockSignals(blocked);
}

/*!
    Brings Qt Assistant to the foreground showing the given \a page.
    The \a page parameter is a path to an HTML file
    (e.g., QLatin1String("/home/pasquale/superproduct/docs/html/intro.html")).

    If Qt Assistant hasn't been opened yet, this function will call
    the openAssistant() slot with the specified page as the start
    page.

    \note The first time Qt Assistant is started, its window will open
    in front of the application's windows. Subsequent calls to this function
    will only load the specified pages in Qt Assistant and will not display
    its window in front of the application's windows.

    \sa openAssistant()
*/
void QAssistantClient::showPage( const QString &page )
{
    if (opened) {
        QTextStream os( socket );
        os << page << QLatin1String("\n");
    } else {
        pageBuffer = page;

        if (proc->state() == QProcess::NotRunning) {
            openAssistant();
            pageBuffer.clear();
            return;
        }
    }
}

/*!
    \property QAssistantClient::open
    \brief whether Qt Assistant is open

*/
bool QAssistantClient::isOpen() const
{
    return opened;
}

void QAssistantClient::socketConnected()
{
    opened = true;
    if ( !pageBuffer.isEmpty() )
        showPage( pageBuffer );
    emit assistantOpened();
}

void QAssistantClient::socketConnectionClosed()
{
    opened = false;
    emit assistantClosed();
}

void QAssistantClient::socketError()
{
    QAbstractSocket::SocketError err = socket->error();
    if (err == QTcpSocket::ConnectionRefusedError)
        emit error( tr( "Could not connect to Assistant: Connection refused" ) );
    else if (err == QTcpSocket::HostNotFoundError)
        emit error( tr( "Could not connect to Assistant: Host not found" ) );
    else if (err != QTcpSocket::RemoteHostClosedError)
        emit error( tr( "Communication error" ) );
}

void QAssistantClient::readStdError()
{
    QString errmsg = QString::fromLatin1(proc->readAllStandardError());

    if (!errmsg.isEmpty())
        emit error( errmsg.simplified() );
}

/*!
    \fn void QAssistantClient::setArguments(const QStringList &arguments)

    Sets the command line \a arguments that are passed to Qt Assistant
    when it is launched.

    The command line arguments can be used to alter Qt Assistant's
    documentation set. When started without any options, Qt Assistant
    displays a default set of documentation. When Qt is installed, the
    default documentation set in Qt Assistant contains the Qt
    reference documentation as well as the tools that come with Qt,
    such as Qt Designer and qmake.
*/
void QAssistantClient::setArguments( const QStringList &args )
{
    QAssistantClientPrivate *d = data( this, true );
    d->arguments = args;
}

QT_END_NAMESPACE
