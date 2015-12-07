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

#include "mainwindow.h"
#include "helpdialog.h"
#include "config.h"

#include <QTcpServer>
#include <QTcpSocket>
#include <QApplication>
#include <QPixmap>
#include <QStringList>
#include <QDir>
#include <QMessageBox>
#include <QPointer>
#include <QTranslator>
#include <QLibraryInfo>
#include <QLocale>
#include <stdlib.h>
#include <stdio.h>

#if defined(USE_STATIC_JPEG_PLUGIN)
  #include <QtPlugin>
  Q_IMPORT_PLUGIN(qjpeg)
#endif

#define INDEX_CHECK( text ) if( i+1 >= argc ) { fprintf(stderr, "%s\n", text); return 1; }

QT_BEGIN_NAMESPACE

#if !defined(QT_NO_DBUS) && defined(Q_OS_UNIX)
QT_BEGIN_INCLUDE_NAMESPACE
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusAbstractAdaptor>
#include <QtDBus/QDBusObjectPath>
#include "tabbedbrowser.h"
QT_END_INCLUDE_NAMESPACE

class HelpWindowAdaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.trolltech.Assistant.HelpWindow")

    Q_PROPERTY(QString source READ source WRITE setSource)

public:
    HelpWindowAdaptor(HelpWindow *w) : QDBusAbstractAdaptor(w), helpWindow(w)
    {
        setAutoRelaySignals(true);
    }

public Q_SLOTS:
    inline QString source() const { return helpWindow->source().toString(); }
    inline void setSource(const QString &src) { helpWindow->setSource(src); }

    inline void clearHistory() { helpWindow->clearHistory(); }
    inline void backward() { helpWindow->backward(); }
    inline void forward() { helpWindow->forward(); }
    inline void reload() { helpWindow->reload(); }
    inline void home() { helpWindow->home(); }

private:
    HelpWindow *helpWindow;
};

class AssistantAdaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.trolltech.Assistant.HelpViewer")

public:
    AssistantAdaptor(MainWindow *mw) : QDBusAbstractAdaptor(mw), mw(mw)
    {
        QDBusConnection connection = QDBusConnection::sessionBus();
        connection.registerService(QLatin1String("com.trolltech.Assistant"));
        connection.registerObject(QLatin1String("/Assistant"), mw);
    }

public slots:
    void showLink(const QString &link) { mw->showLink(link); }
    QDBusObjectPath createNewTab();
    QDBusObjectPath currentTab();

private:
    QDBusObjectPath pathForBrowser(HelpWindow *window);
    MainWindow *mw;
};

QDBusObjectPath AssistantAdaptor::createNewTab()
{
    HelpWindow *window = mw->browsers()->newBackgroundTab();
    return pathForBrowser(window);
}

QDBusObjectPath AssistantAdaptor::currentTab()
{
    HelpWindow *window = mw->browsers()->currentBrowser();
    return pathForBrowser(window);
}

QDBusObjectPath AssistantAdaptor::pathForBrowser(HelpWindow *window)
{
    int index = mw->browsers()->browsers().indexOf(window);
    if (index == -1)
        return QDBusObjectPath();

    QString name(QLatin1String("/Assistant/Tabs/"));
    name += QString::number(index);
    QDBusObjectPath path(name);

    if (!window->findChild<HelpWindowAdaptor *>()) {
        (void)new HelpWindowAdaptor(window);
        QDBusConnection::sessionBus().registerObject(name, window);
    }

    return path;
}

#endif // QT_NO_DBUS

class AssistantSocket : public QTcpSocket
{
    Q_OBJECT
public:
    AssistantSocket( int sock, QObject *parent = 0 );
    ~AssistantSocket() {}

signals:
    void showLinkRequest( const QString& );

private slots:
    void readClient();
    void connectionClosed();
};


class AssistantServer : public QTcpServer
{
    Q_OBJECT
public:
    AssistantServer( QObject* parent = 0 );
    quint16 getPort() const;

signals:
    void showLinkRequest( const QString& );
    void newConnect();

public slots:
    virtual void incomingConnection( int socket );

private:
    quint16 p;
};

AssistantSocket::AssistantSocket( int sock, QObject *parent )
    : QTcpSocket( parent )
{
    connect( this, SIGNAL(readyRead()), SLOT(readClient()) );
    connect( this, SIGNAL(disconnected()), SLOT(connectionClosed()) );
    setSocketDescriptor( sock );
}

void AssistantSocket::readClient()
{
    QString link = QString();
    while ( canReadLine() )
        link = QLatin1String(readLine());
    if ( !link.isNull() ) {
        link = link.replace(QLatin1String("\n"), QLatin1String(""));
        link = link.replace(QLatin1String("\r"), QLatin1String(""));
        QFileInfo fi(link);
        link = fi.absoluteFilePath();
        emit showLinkRequest( link );
    }
}

void AssistantSocket::connectionClosed()
{
    deleteLater();
}

AssistantServer::AssistantServer( QObject *parent )
    : QTcpServer( parent )
{
    listen(QHostAddress::LocalHost, 0);
    if ( !isListening() ) {
        QMessageBox::critical( 0, tr( "Qt Assistant" ),
                tr( "Failed to bind to port %1" ).arg( serverPort() ) );
        exit( 1 );
    }
    p = serverPort();
}

quint16 AssistantServer::getPort() const
{
    return p;
}

void AssistantServer::incomingConnection( int socket )
{
    AssistantSocket *as = new AssistantSocket( socket, this );
    connect( as, SIGNAL(showLinkRequest(QString)),
             this, SIGNAL(showLinkRequest(QString)) );
    emit newConnect();
}

int runAssistant( int argc, char ** argv )
{
    bool withGUI = true;
#ifndef Q_WS_WIN
    if ( argc > 1 ) {
        QString arg = QString::fromLocal8Bit(argv[1]);
        arg = arg.toLower();
        if ( arg == QLatin1String("-addcontentfile")
            || arg == QLatin1String("-removecontentfile")
            || arg == QLatin1String("-help")
            || arg == QLatin1String("/?")
            )
            withGUI = false;
    }
#endif
    QApplication a(argc, argv, withGUI);
    a.setOrganizationName(QLatin1String("Trolltech"));
    a.setApplicationName(QLatin1String("Assistant"));

    QString resourceDir;
    AssistantServer *as = 0;
    QStringList catlist;
    QString file, profileName, aDocPath;
    bool server = false;
    bool hideSidebar = false;
    bool configLoaded = false;
    if ( argc == 2 ) {
        file = QString::fromLocal8Bit(argv[1]);
        if (file.startsWith(QLatin1String("-")) || file == QLatin1String("/?")) {
            file.clear();
        } else {
            QFileInfo fi(file);
            file = fi.absoluteFilePath();
            file = MainWindow::urlifyFileName(file);
        }
    }
    if ( file.isEmpty() ) {
        for ( int i = 1; i < argc; i++ ) {
            QString opt = QString::fromLocal8Bit(argv[i]).toLower();
            if ( opt == QLatin1String("-file") ) {
                INDEX_CHECK( "Missing file argument!" );
                i++;
                file = QFile::decodeName(argv[i]);
            } else if ( opt == QLatin1String("-server") ) {
                server = true;
            } else if ( opt == QLatin1String("-profile") ) {
                INDEX_CHECK( "Missing profile argument!" );
                profileName = QFile::decodeName(argv[++i]);
            } else if ( opt == QLatin1String("-addcontentfile") ) {
                INDEX_CHECK( "Missing content file!" );
                Config *c = Config::loadConfig(QString());
                QFileInfo file( QFile::decodeName(argv[i+1]) );
                if( !file.exists() ) {
                    fprintf(stderr, "Could not locate content file: %s\n", qPrintable(file.absoluteFilePath()));
                    return 1;
                }
                DocuParser *parser = DocuParser::createParser( file.absoluteFilePath() );
                if( parser ) {
                    QFile f( QFile::decodeName(argv[i+1]) );
                    if( !parser->parse( &f ) ) {
                        fprintf(stderr, "Failed to parse file: %s\n", qPrintable(file.absoluteFilePath()));
                        return 1;
                    }
                    parser->addTo( c->profile() );
                    c->setDocRebuild( true );
                    c->save();
                }
                return 0;
            } else if ( opt == QLatin1String("-removecontentfile") ) {
                INDEX_CHECK("Missing content file!");
                Config *c = Config::loadConfig(QString());
                Profile *profile = c->profile();
                QString contentFile = QString::fromLocal8Bit(argv[i+i]);
                QStringList entries;
#ifdef Q_WS_WIN
                contentFile.replace(QLatin1Char('\\'), QLatin1Char('/'));
                entries = profile->docs.filter(contentFile, Qt::CaseInsensitive);
#else
                entries = profile->docs.filter(contentFile);
#endif
                if (entries.count() == 0) {
                    fprintf(stderr, "Could not locate content file: %s\n", qPrintable(contentFile));
                    return 1;
                } else if (entries.count() > 1) {
                    fprintf(stderr, "More than one entry matching file name found, "
                        "please specify full path to file");
                    return 1;
                } else {
                    QFileInfo file(entries[0]);
                    if( !file.exists() ) {
                        fprintf(stderr, "Could not locate content file: %s\n", qPrintable(file.absoluteFilePath()));
                        return 1;
                    }
                    profile->removeDocFileEntry( file.absoluteFilePath() );
                    c->setDocRebuild( true );
                    c->save();
                }
                return 0;
            } else if ( QString( QLatin1String(argv[i]) ).toLower() == QLatin1String("-docpath") ) {
                INDEX_CHECK( "Missing path!" );
                QDir dir(QString::fromLocal8Bit(argv[i+1]));
                if ( dir.exists() ) {
                    Config *c = Config::loadConfig(QString());
                    c->saveProfile(Profile::createDefaultProfile(dir.absolutePath()));
                    c->loadDefaultProfile();
                    c->setDocRebuild(true);
                    c->save();
                    configLoaded = true;
                    ++i;
                } else {
                    fprintf(stderr, "The specified path does not exist!\n");
                    return 1;
                }
            } else if ( opt == QLatin1String("-hidesidebar") ) {
                hideSidebar = true;
            } else if ( opt == QLatin1String("-help") || opt == QLatin1String("/?") ) {
                QString helpText = QLatin1String( "Usage: assistant [option]\n"
                                  "Options:\n"
                                  " -file Filename             assistant opens the specified file\n"
                                  " -server                    reads commands from a socket after\n"
                                  "                            assistant has started\n"
                                  " -profile fileName          starts assistant and displays the\n"
                                  "                            profile specified in the file fileName.\n"
                                  " -addContentFile file       adds the content file 'file' to the set of\n"
                                  "                            documentation available by default\n"
                                  " -removeContentFile file    removes the content file 'file' from the\n"
                                  "                            documentation available by default\n"
                                  " -docPath path              sets the Qt documentation root path to\n"
                                  "                            'path' and starts assistant\n"
                                  " -hideSidebar               assistant will hide the sidebar.\n"
                                  " -resourceDir               assistant will load translations from\n"
                                  "                            this directory.\n"
                                  " -help                      shows this help.");
#ifdef Q_WS_WIN
                QMessageBox::information( 0, QLatin1String("Qt Assistant"),
                    QLatin1String("<pre>") + helpText + QLatin1String("</pre>") );
#else
                fprintf(stdout, "%s\n", qPrintable(helpText));
#endif
                exit( 0 );
            } else if ( opt == QLatin1String("-resourcedir") ) {
                INDEX_CHECK( "Missing resource directory argument!" );
                resourceDir = QFile::decodeName( argv[++i] );
            } else {
                fprintf(stderr, "Unrecognized option %s. Try -help to get help.\n", qPrintable(opt));
                return 1;
            }
        }
    }

    if( resourceDir.isNull() )
        resourceDir = QLibraryInfo::location(QLibraryInfo::TranslationsPath);

    QTranslator translator( 0 );
    translator.load( QLatin1String("assistant_adp_") + QLocale::system().name(), resourceDir );
    a.installTranslator( &translator );

    QTranslator qtTranslator( 0 );
    qtTranslator.load( QLatin1String("qt_") + QLocale::system().name(), resourceDir );
    a.installTranslator( &qtTranslator );

    Config *conf = 0;
    if (configLoaded)
        conf = Config::configuration();
    else
        conf = Config::loadConfig( profileName );
    if (!conf) {
        fprintf( stderr, "Profile '%s' does not exist!\n", profileName.toLatin1().constData() );
        fflush( stderr );
        return -1;
    }

    QStringList links = conf->source();
    conf->hideSideBar( hideSidebar );

    QPointer<MainWindow> mw = new MainWindow();
    mw->setObjectName(QLatin1String("Assistant"));

    if ( server ) {
        as = new AssistantServer();
        printf("%d\n", as->serverPort() );
        fflush( stdout );
        as->connect( as, SIGNAL(showLinkRequest(QString)),
                     mw, SLOT(showLinkFromClient(QString)) );
    }

#if !defined(QT_NO_DBUS) && defined(Q_OS_UNIX)
    new AssistantAdaptor(mw);
#endif // QT_NO_DBUS

    FontSettings settings = conf->fontSettings();
    if (mw->font() != settings.windowFont)
        a.setFont(settings.windowFont, "QWidget");

#ifdef Q_WS_MAC
    // Make sure AssitantClient shows the window in front.
    mw->raise();
#endif
    mw->show();

    if (!file.isEmpty())
        mw->showLink( MainWindow::urlifyFileName(file) );
    else if (file.isEmpty())
        mw->showLinks( links );

    a.connect( &a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()) );
    
    int appExec = a.exec();
    delete (MainWindow*)mw;
    return appExec;
}

QT_END_NAMESPACE

int main( int argc, char ** argv )
{
    Q_INIT_RESOURCE(assistant);
    return QT_PREPEND_NAMESPACE(runAssistant)(argc, argv);
}

#include "main.moc"
