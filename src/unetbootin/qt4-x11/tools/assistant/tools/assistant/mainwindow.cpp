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
#include "centralwidget.h"
#include "helpviewer.h"
#include "indexwindow.h"
#include "topicchooser.h"
#include "contentwindow.h"
#include "preferencesdialog.h"
#include "bookmarkmanager.h"
#include "remotecontrol.h"
#include "cmdlineparser.h"
#include "aboutdialog.h"
#include "searchwidget.h"
#include "qtdocinstaller.h"

#include <QtCore/QDir>
#include <QtCore/QTimer>
#include <QtCore/QDebug>
#include <QtCore/QResource>
#include <QtCore/QByteArray>
#include <QtCore/QTextStream>
#include <QtCore/QCoreApplication>

#include <QtGui/QMenuBar>
#include <QtGui/QAction>
#include <QtGui/QToolBar>
#include <QtGui/QStatusBar>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QLayout>
#include <QtGui/QDockWidget>
#include <QtGui/QTreeView>
#include <QtGui/QMessageBox>
#include <QtGui/QFontDatabase>
#include <QtGui/QComboBox>
#include <QtGui/QProgressBar>
#include <QtGui/QDesktopServices>
#include <QtGui/QToolButton>

#include <QtHelp/QHelpEngine>
#include <QtHelp/QHelpSearchEngine>
#include <QtHelp/QHelpContentModel>
#include <QtHelp/QHelpIndexModel>

QT_BEGIN_NAMESPACE

MainWindow::MainWindow(CmdLineParser *cmdLine, QWidget *parent)
    : QMainWindow(parent)
    , m_toolBarMenu(0)
    , m_cmdLine(cmdLine)
    , m_searchWidget(0)
    , m_progressWidget(0)
    , m_qtDocInstaller(0)
    , m_connectedInitSignals(false)
{
    if (usesDefaultCollection()) {
        MainWindow::collectionFileDirectory(true);
        m_helpEngine = new QHelpEngine(MainWindow::defaultHelpCollectionFileName(),
            this);
    } else {
        m_helpEngine = new QHelpEngine(cmdLine->collectionFile(), this);
    }

    m_centralWidget = new CentralWidget(m_helpEngine, this);
    setCentralWidget(m_centralWidget);

    m_indexWindow = new IndexWindow(m_helpEngine);
    QDockWidget *indexDock = new QDockWidget(tr("Index"), this);
    indexDock->setObjectName(QLatin1String("IndexWindow"));
    indexDock->setWidget(m_indexWindow);
    addDockWidget(Qt::LeftDockWidgetArea, indexDock);

    m_contentWindow = new ContentWindow(m_helpEngine);
    QDockWidget *contentDock = new QDockWidget(tr("Contents"), this);
    contentDock->setObjectName(QLatin1String("ContentWindow"));
    contentDock->setWidget(m_contentWindow);
    addDockWidget(Qt::LeftDockWidgetArea, contentDock);

    QDockWidget *bookmarkDock = new QDockWidget(tr("Bookmarks"), this);
    bookmarkDock->setObjectName(QLatin1String("BookmarkWindow"));
    bookmarkDock->setWidget(setupBookmarkWidget());
    addDockWidget(Qt::LeftDockWidgetArea, bookmarkDock);

    QHelpSearchEngine *searchEngine = m_helpEngine->searchEngine();
    connect(searchEngine, SIGNAL(indexingStarted()), this, SLOT(indexingStarted()));
    connect(searchEngine, SIGNAL(indexingFinished()), this, SLOT(indexingFinished()));

#ifdef QT_CLUCENE_SUPPORT
    m_centralWidget->createSearchWidget(searchEngine);
#else
    QDockWidget *dock = new QDockWidget(tr("Search"), this);
    dock->setObjectName(QLatin1String("SearchWindow"));
    m_searchWidget = new SearchWidget(searchEngine, this);
    dock->setWidget(m_searchWidget);
    addDockWidget(Qt::LeftDockWidgetArea, dock);

    connect(m_searchWidget, SIGNAL(requestShowLink(const QUrl&)),
        m_centralWidget, SLOT(setSource(const QUrl&)));
    connect(m_searchWidget, SIGNAL(requestShowLinkInNewTab(const QUrl&)),
        m_centralWidget, SLOT(setSourceInNewTab(const QUrl&)));
#endif

    QString defWindowTitle = tr("Qt Assistant");
    setWindowTitle(defWindowTitle);

    setupActions();
    statusBar()->show();

    if (initHelpDB()) {
        setupFilterToolbar();
        setupAddressToolbar();
        m_bookmarkManager->setupBookmarkModels();

        setWindowTitle(m_helpEngine->customValue(QLatin1String("WindowTitle"),
            defWindowTitle).toString());
        QByteArray iconArray = m_helpEngine->customValue(QLatin1String("ApplicationIcon"),
            QByteArray()).toByteArray();
        if (iconArray.size() > 0) {
            QPixmap pix;
            pix.loadFromData(iconArray);
            QIcon appIcon(pix);
            qApp->setWindowIcon(appIcon);
        } else {
            QIcon appIcon(QLatin1String(":/trolltech/assistant/images/assistant-128.png"));
            qApp->setWindowIcon(appIcon);
        }

        // Show the widget here, otherwise the restore geometry and state won't work
        // on x11.
        show();
        QByteArray ba(m_helpEngine->customValue(QLatin1String("MainWindow")).toByteArray());
        if (!ba.isEmpty())
            restoreState(ba);

        ba = m_helpEngine->customValue(QLatin1String("MainWindowGeometry")).toByteArray();
        if (!ba.isEmpty()) {
            restoreGeometry(ba);
        } else {
            tabifyDockWidget(contentDock, indexDock);
            tabifyDockWidget(indexDock, bookmarkDock);
            contentDock->raise();
            resize(QSize(800, 600));
        }

        if (!m_helpEngine->customValue(QLatin1String("useAppFont")).isValid()) {
            m_helpEngine->setCustomValue(QLatin1String("useAppFont"), false);
            m_helpEngine->setCustomValue(QLatin1String("useBrowserFont"), false);
            m_helpEngine->setCustomValue(QLatin1String("appFont"), qApp->font());
            m_helpEngine->setCustomValue(QLatin1String("appWritingSystem"), QFontDatabase::Latin);
            m_helpEngine->setCustomValue(QLatin1String("browserFont"), qApp->font());
            m_helpEngine->setCustomValue(QLatin1String("browserWritingSystem"), QFontDatabase::Latin);
        } else {
            updateApplicationFont();
        }

        updateAboutMenuText();

        QTimer::singleShot(0, this, SLOT(insertLastPages()));
        if (m_cmdLine->enableRemoteControl())
            (void)new RemoteControl(this, m_helpEngine);

        if (m_cmdLine->contents() == CmdLineParser::Show)
            showContents();
        else if (m_cmdLine->contents() == CmdLineParser::Hide)
            hideContents();

        if (m_cmdLine->index() == CmdLineParser::Show)
            showIndex();
        else if (m_cmdLine->index() == CmdLineParser::Hide)
            hideIndex();

        if (m_cmdLine->bookmarks() == CmdLineParser::Show)
            showBookmarks();
        else if (m_cmdLine->bookmarks() == CmdLineParser::Hide)
            hideBookmarks();

        if (m_cmdLine->search() == CmdLineParser::Show)
            showSearch();
        else if (m_cmdLine->search() == CmdLineParser::Hide)
            hideSearch();

        if (m_cmdLine->contents() == CmdLineParser::Activate)
            showContents();
        else if (m_cmdLine->index() == CmdLineParser::Activate)
            showIndex();
        else if (m_cmdLine->bookmarks() == CmdLineParser::Activate)
            showBookmarks();

        if (usesDefaultCollection())
            QTimer::singleShot(0, this, SLOT(lookForNewQtDocumentation()));
        else
            checkInitState();
    }
}

MainWindow::~MainWindow()
{
    if (m_qtDocInstaller)
        delete m_qtDocInstaller;
}

bool MainWindow::usesDefaultCollection() const
{
    return m_cmdLine->collectionFile().isEmpty();
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    m_bookmarkManager->saveBookmarks();
    m_helpEngine->setCustomValue(QLatin1String("MainWindow"), saveState());
    m_helpEngine->setCustomValue(QLatin1String("MainWindowGeometry"),
        saveGeometry());

    QMainWindow::closeEvent(e);
}

bool MainWindow::initHelpDB()
{
    if (!m_helpEngine->setupData())
        return false;

    bool assistantInternalDocRegistered = false;
    QString intern(QLatin1String("com.trolltech.com.assistantinternal_"));
    foreach (const QString &ns, m_helpEngine->registeredDocumentations()) {
        if (ns.startsWith(intern)) {
            intern = ns;
            assistantInternalDocRegistered = true;
            break;
        }
    }

    const QString &collectionFile = m_helpEngine->collectionFile();

    QFileInfo fi(collectionFile);
    QString helpFile;
    QTextStream(&helpFile) << fi.absolutePath() << QDir::separator()
        << QLatin1String("assistant.qch.") << (QT_VERSION >> 16)
        << QLatin1Char('.') << ((QT_VERSION >> 8) & 0xFF);

    bool needsSetup = false;
    if (!assistantInternalDocRegistered || !QFile::exists(helpFile)) {
        QFile file(helpFile);
        if (file.open(QIODevice::WriteOnly)) {
            QResource res(QLatin1String(":/trolltech/assistant/assistant.qch"));
            if (file.write((const char*)res.data(), res.size()) != res.size())
                qDebug() << QLatin1String("could not write assistant.qch...");

            file.close();
        }
        QHelpEngineCore hc(fi.absoluteFilePath());
        hc.setupData();
        hc.unregisterDocumentation(intern);
        hc.registerDocumentation(helpFile);
        needsSetup = true;
    }

    const QLatin1String unfiltered("UnfilteredFilterInserted");
    if (1 != m_helpEngine->customValue(unfiltered).toInt()) {
        {
            QHelpEngineCore hc(collectionFile);
            hc.setupData();
            hc.addCustomFilter(tr("Unfiltered"), QStringList());
            hc.setCustomValue(unfiltered, 1);
        }
        m_helpEngine->blockSignals(true);
        m_helpEngine->setCurrentFilter(tr("Unfiltered"));
        m_helpEngine->blockSignals(false);
        needsSetup = true;
    }

    if (needsSetup)
        m_helpEngine->setupData();
    return true;
}

void MainWindow::lookForNewQtDocumentation()
{
    m_qtDocInstaller = new QtDocInstaller(m_helpEngine->collectionFile());
    connect(m_qtDocInstaller, SIGNAL(errorMessage(const QString&)),
        this, SLOT(displayInstallationError(const QString&)));
    connect(m_qtDocInstaller, SIGNAL(docsInstalled(bool)),
        this, SLOT(qtDocumentationInstalled(bool)));

    QString versionKey = QString(QLatin1String("qtVersion%1$$$qt")).
        arg(QLatin1String(QT_VERSION_STR));
    if (m_helpEngine->customValue(versionKey, 0).toInt() != 1)
        statusBar()->showMessage(tr("Looking for Qt Documentation..."));
    m_qtDocInstaller->installDocs();
}

void MainWindow::displayInstallationError(const QString &errorMessage)
{
    QMessageBox::warning(this, tr("Qt Assistant"), errorMessage);
}

void MainWindow::qtDocumentationInstalled(bool newDocsInstalled)
{
    if (newDocsInstalled)
        m_helpEngine->setupData();
    statusBar()->clearMessage();
    checkInitState();
}

void MainWindow::checkInitState()
{
    if (!m_cmdLine->enableRemoteControl())
        return;

    if (m_helpEngine->contentModel()->isCreatingContents()
        || m_helpEngine->indexModel()->isCreatingIndex()) {
        if (!m_connectedInitSignals) {
            connect(m_helpEngine->contentModel(), SIGNAL(contentsCreated()),
                this, SLOT(checkInitState()));
            connect(m_helpEngine->indexModel(), SIGNAL(indexCreated()),
                this, SLOT(checkInitState()));
            m_connectedInitSignals = true;
        }
    } else {
        if (m_connectedInitSignals) {
            disconnect(m_helpEngine->contentModel(), 0, this, 0);
            disconnect(m_helpEngine->indexModel(), 0, this, 0);
        }
        emit initDone();
    }
}

void MainWindow::insertLastPages()
{
    if (m_cmdLine->url().isValid())
        m_centralWidget->setSource(m_cmdLine->url());
    else
        m_centralWidget->setLastShownPages();

    if (m_cmdLine->search() == CmdLineParser::Activate)
        showSearch();
}

void MainWindow::setupActions()
{
    QString system = QLatin1String("win");
#ifdef Q_OS_MAC
    system = QLatin1String("mac");
    setUnifiedTitleAndToolBarOnMac(true);
#endif

    QMenu *menu = menuBar()->addMenu(tr("&File"));

    m_pageSetupAction = menu->addAction(tr("Page Set&up..."), m_centralWidget, SLOT(pageSetup()));
    m_printPreviewAction = menu->addAction(tr("Print Preview..."), m_centralWidget, SLOT(printPreview()));
    m_printAction = menu->addAction(tr("&Print..."), m_centralWidget, SLOT(print()));
    m_printAction->setIcon(QIcon(
        QString::fromUtf8(":/trolltech/assistant/images/%1/print.png").arg(system)));
    m_printAction->setShortcut(tr("CTRL+P"));

    menu->addSeparator();

    m_newTabAction = menu->addAction(tr("New &Tab"), m_centralWidget, SLOT(newTab()));
    m_newTabAction->setShortcut(tr("CTRL+T"));
    m_closeTabAction = menu->addAction(tr("&Close Tab"), m_centralWidget, SLOT(closeTab()));
    m_closeTabAction->setShortcut(tr("CTRL+W"));

    QAction *tmp = menu->addAction(tr("&Quit"), this, SLOT(close()));
    tmp->setShortcut(tr("CTRL+Q"));
    tmp->setMenuRole(QAction::QuitRole);

    menu = menuBar()->addMenu(tr("&Edit"));
    m_copyAction = menu->addAction(tr("&Copy selected Text"),
        m_centralWidget, SLOT(copySelection()));
    m_copyAction->setIcon(QIcon(
        QString::fromUtf8(":/trolltech/assistant/images/%1/editcopy.png").arg(system)));
    m_copyAction->setShortcut(tr("Ctrl+C"));
    m_copyAction->setEnabled(false);

    m_findAction = menu->addAction(tr("&Find in Text..."),
        m_centralWidget, SLOT(showTextSearch()));
    m_findAction->setIcon(QIcon(
        QString::fromUtf8(":/trolltech/assistant/images/%1/find.png").arg(system)));
    m_findAction->setShortcut(tr("Ctrl+F"));
    m_findAction->setShortcut(QKeySequence::Find);

    QAction *findNextAction = menu->addAction(tr("Find &Next"),
        m_centralWidget, SLOT(findNext()));
    findNextAction->setShortcuts(QList<QKeySequence>() << QKeySequence(tr("F3"))
        << QKeySequence::FindNext);

    QAction *findPreviousAction = menu->addAction(tr("Find &Previous"),
        m_centralWidget, SLOT(findPrevious()));
    findPreviousAction->setShortcuts(QList<QKeySequence>() <<
        QKeySequence(tr("Shift+F3")) << QKeySequence::FindPrevious);

    menu->addSeparator();
    tmp = menu->addAction(tr("Preferences..."), this, SLOT(showPreferences()));
    tmp->setMenuRole(QAction::PreferencesRole);

    m_viewMenu = menuBar()->addMenu(tr("&View"));
    m_zoomInAction = m_viewMenu->addAction(tr("Zoom &in"),
        m_centralWidget, SLOT(zoomIn()));
    m_zoomInAction->setIcon(QIcon(
        QString::fromUtf8(":/trolltech/assistant/images/%1/zoomin.png").arg(system)));
    m_zoomInAction->setShortcut(tr("Ctrl++"));

    m_zoomOutAction = m_viewMenu->addAction(tr("Zoom &out"),
        m_centralWidget, SLOT(zoomOut()));
    m_zoomOutAction->setIcon(QIcon(
        QString::fromUtf8(":/trolltech/assistant/images/%1/zoomout.png").arg(system)));
    m_zoomOutAction->setShortcut(tr("Ctrl+-"));

    m_resetZoomAction = m_viewMenu->addAction(tr("Normal &Size"),
        m_centralWidget, SLOT(resetZoom()));
    m_resetZoomAction->setIcon(QIcon(
        QString::fromUtf8(":/trolltech/assistant/images/%1/resetzoom.png").arg(system)));
    m_resetZoomAction->setShortcut(tr("Ctrl+0"));

    m_viewMenu->addSeparator();

    m_viewMenu->addAction(tr("Contents"), this,
        SLOT(showContents()), QKeySequence(tr("ALT+C")));
    m_viewMenu->addAction(tr("Index"), this,
        SLOT(showIndex()), QKeySequence(tr("ALT+I")));
    m_viewMenu->addAction(tr("Bookmarks"), this,
        SLOT(showBookmarks()), QKeySequence(tr("ALT+O")));
    m_viewMenu->addAction(tr("Search"), this,
        SLOT(showSearch()), QKeySequence(tr("ALT+S")));

    menu = menuBar()->addMenu(tr("&Go"));
    m_homeAction = menu->addAction(tr("&Home"),
        m_centralWidget, SLOT(home()));
    m_homeAction->setShortcut(tr("Ctrl+Home"));
    m_homeAction->setIcon(QIcon(
        QString::fromUtf8(":/trolltech/assistant/images/%1/home.png").arg(system)));

    m_backAction = menu->addAction(tr("&Back"),
        m_centralWidget, SLOT(backward()));
    m_backAction->setEnabled(false);
    m_backAction->setShortcuts(QList<QKeySequence>()
        << QKeySequence(Qt::CTRL|Qt::Key_Left) << QKeySequence::Back);
    m_backAction->setIcon(QIcon(
        QString::fromUtf8(":/trolltech/assistant/images/%1/previous.png").arg(system)));

    m_nextAction = menu->addAction(tr("&Forward"),
        m_centralWidget, SLOT(forward()));
    m_nextAction->setEnabled(false);
    m_nextAction->setShortcuts(QList<QKeySequence>()
        << QKeySequence(Qt::CTRL|Qt::Key_Right) << QKeySequence::Forward);
    m_nextAction->setIcon(QIcon(
        QString::fromUtf8(":/trolltech/assistant/images/%1/next.png").arg(system)));

    m_syncAction = menu->addAction(tr("Sync with Table of Contents"),
        this, SLOT(syncContents()));
    m_syncAction->setIcon(QIcon(
        QString::fromUtf8(":/trolltech/assistant/images/%1/synctoc.png").arg(system)));

    menu->addSeparator();

    tmp = menu->addAction(tr("Next Page"), m_centralWidget, SLOT(nextPage()));
    tmp->setShortcuts(QList<QKeySequence>() << QKeySequence(tr("Ctrl+Alt+Right"))
        << QKeySequence(Qt::CTRL + Qt::Key_PageDown));

    tmp = menu->addAction(tr("Previous Page"),
        m_centralWidget, SLOT(previousPage()));
    tmp->setShortcuts(QList<QKeySequence>() << QKeySequence(tr("Ctrl+Alt+Left"))
        << QKeySequence(Qt::CTRL + Qt::Key_PageUp));

    menu = menuBar()->addMenu(tr("&Bookmarks"));
    tmp = menu->addAction(tr("Add Bookmark..."), this, SLOT(addBookmark()));
    tmp->setShortcut(tr("CTRL+D"));

    menu = menuBar()->addMenu(tr("&Help"));
    m_aboutAction = menu->addAction(tr("About..."), this, SLOT(showAboutDialog()));
    m_aboutAction->setMenuRole(QAction::AboutRole);

    QToolBar *navigationBar = addToolBar(tr("Navigation Toolbar"));
    navigationBar->setObjectName(QLatin1String("NavigationToolBar"));
    navigationBar->addAction(m_backAction);
    navigationBar->addAction(m_nextAction);
    navigationBar->addAction(m_homeAction);
    navigationBar->addAction(m_syncAction);
    QAction *sep = navigationBar->addSeparator();
    navigationBar->addAction(m_copyAction);
    navigationBar->addAction(m_printAction);
    navigationBar->addAction(m_findAction);
    QAction *sep2 = navigationBar->addSeparator();
    navigationBar->addAction(m_zoomInAction);
    navigationBar->addAction(m_zoomOutAction);
    navigationBar->addAction(m_resetZoomAction);

    QList<QAction*> actionList;
    actionList << m_backAction << m_nextAction << m_homeAction;
    actionList << sep << m_zoomInAction << m_zoomOutAction;
    actionList << sep2 << m_copyAction << m_printAction << m_findAction;
    m_centralWidget->setGlobalActions(actionList);

#if defined(Q_WS_MAC)
    QMenu *windowMenu = new QMenu(tr("&Window"), this);
    menuBar()->insertMenu(menu->menuAction(), windowMenu);
    windowMenu->addAction(tr("Minimize"), this,
        SLOT(showMinimized()), QKeySequence(tr("Ctrl+M")));
    windowMenu->addAction(tr("Zoom"), this,
        SLOT(showMaximized()));
#endif

    // content viewer connections
    connect(m_centralWidget, SIGNAL(copyAvailable(bool)),
        this, SLOT(copyAvailable(bool)));
    connect(m_centralWidget, SIGNAL(currentViewerChanged()),
        this, SLOT(updateNavigationItems()));
    connect(m_centralWidget, SIGNAL(forwardAvailable(bool)),
        this, SLOT(updateNavigationItems()));
    connect(m_centralWidget, SIGNAL(backwardAvailable(bool)),
        this, SLOT(updateNavigationItems()));
    connect(m_centralWidget, SIGNAL(highlighted(const QString&)),
        statusBar(), SLOT(showMessage(const QString&)));
    connect(m_centralWidget, SIGNAL(addNewBookmark(const QString&,
        const QString&)), this, SLOT(addNewBookmark(const QString&, const QString&)));

    // bookmarks
    connect(m_bookmarkWidget, SIGNAL(requestShowLink(const QUrl&)),
        m_centralWidget, SLOT(setSource(const QUrl&)));
    connect(m_bookmarkWidget, SIGNAL(escapePressed()),
        this, SLOT(activateCurrentCentralWidgetTab()));

    // index window
    connect(m_indexWindow, SIGNAL(linkActivated(const QUrl&)),
        m_centralWidget, SLOT(setSource(const QUrl&)));
    connect(m_indexWindow, SIGNAL(linksActivated(const QMap<QString, QUrl>&, const QString&)),
            this, SLOT(showTopicChooser(const QMap<QString, QUrl>&, const QString&)));
    connect(m_indexWindow, SIGNAL(escapePressed()),
        this, SLOT(activateCurrentCentralWidgetTab()));

    // content window
    connect(m_contentWindow, SIGNAL(linkActivated(const QUrl&)),
        m_centralWidget, SLOT(setSource(const QUrl&)));
    connect(m_contentWindow, SIGNAL(escapePressed()),
        this, SLOT(activateCurrentCentralWidgetTab()));

#if defined(QT_NO_PRINTER)
        m_pageSetupAction->setVisible(false);
        m_printPreviewAction->setVisible(false);
        m_printAction->setVisible(false);
#endif
}

QMenu *MainWindow::toolBarMenu()
{
    if (!m_toolBarMenu) {
        m_viewMenu->addSeparator();
        m_toolBarMenu = m_viewMenu->addMenu(tr("Toolbars"));
    }
    return m_toolBarMenu;
}

void MainWindow::setupFilterToolbar()
{
    if (!m_helpEngine->
            customValue(QLatin1String("EnableFilterFunctionality"), true).toBool())
        return;

    m_filterCombo = new QComboBox(this);
    m_filterCombo->setMinimumWidth(QFontMetrics(QFont()).
        width(QLatin1String("MakeTheComboBoxWidthEnough")));

    QToolBar *filterToolBar = addToolBar(tr("Filter Toolbar"));
    filterToolBar->setObjectName(QLatin1String("FilterToolBar"));
    filterToolBar->addWidget(new QLabel(tr("Filtered by:").append(QLatin1String(" ")), this));
    filterToolBar->addWidget(m_filterCombo);

    if (m_helpEngine->customValue(QLatin1String("HideFilterFunctionality"), true).toBool())
        filterToolBar->hide();
    toolBarMenu()->addAction(filterToolBar->toggleViewAction());

    connect(m_helpEngine, SIGNAL(setupFinished()),
        this, SLOT(setupFilterCombo()));
    connect(m_filterCombo, SIGNAL(activated(const QString&)),
        this, SLOT(filterDocumentation(const QString&)));

    setupFilterCombo();
}

void MainWindow::setupAddressToolbar()
{
    if (!m_helpEngine->customValue(QLatin1String("EnableAddressBar"), true).toBool())
        return;

    m_addressLineEdit = new QLineEdit(this);
    QToolBar *addressToolBar = addToolBar(tr("Address Toolbar"));
    addressToolBar->setObjectName(QLatin1String("AddressToolBar"));
    insertToolBarBreak(addressToolBar);

    addressToolBar->addWidget(new QLabel(tr("Address:").append(QLatin1String(" ")), this));
    addressToolBar->addWidget(m_addressLineEdit);

    if (m_helpEngine->customValue(QLatin1String("HideAddressBar"), true).toBool())
        addressToolBar->hide();
    toolBarMenu()->addAction(addressToolBar->toggleViewAction());

    // address lineedit
    connect(m_addressLineEdit, SIGNAL(returnPressed()),
        this, SLOT(gotoAddress()));
    connect(m_centralWidget, SIGNAL(currentViewerChanged()),
        this, SLOT(showNewAddress()));
    connect(m_centralWidget, SIGNAL(sourceChanged(const QUrl&)),
        this, SLOT(showNewAddress(const QUrl&)));
}

void MainWindow::updateAboutMenuText()
{
    if (m_helpEngine) {
        QByteArray ba = m_helpEngine->customValue(QLatin1String("AboutMenuTexts"),
            QByteArray()).toByteArray();
        if (ba.size() > 0) {
            QString lang;
            QString str;
            QString trStr;
            QString currentLang = QLocale::system().name();
            int i = currentLang.indexOf(QLatin1Char('_'));
            if (i > -1)
                currentLang = currentLang.left(i);
            QDataStream s(&ba, QIODevice::ReadOnly);
            while (!s.atEnd()) {
                s >> lang;
                s >> str;
                if (lang == QLatin1String("default") && trStr.isEmpty()) {
                    trStr = str;
                } else if (lang == currentLang) {
                    trStr = str;
                    break;
                }
            }
            if (!trStr.isEmpty())
                m_aboutAction->setText(trStr);
        }
    }
}

void MainWindow::showNewAddress()
{
    showNewAddress(m_centralWidget->currentSource());
}

void MainWindow::showNewAddress(const QUrl &url)
{
    m_addressLineEdit->setText(url.toString());
}

void MainWindow::addBookmark()
{
    addNewBookmark(m_centralWidget->currentTitle(), m_centralWidget->currentSource().toString());
}

void MainWindow::gotoAddress()
{
    m_centralWidget->setSource(m_addressLineEdit->text());
}

void MainWindow::updateNavigationItems()
{
    bool hasCurrentViewer = m_centralWidget->isHomeAvailable();
    m_copyAction->setEnabled(m_centralWidget->hasSelection());
    m_homeAction->setEnabled(hasCurrentViewer);
    m_syncAction->setEnabled(hasCurrentViewer);
    m_printPreviewAction->setEnabled(hasCurrentViewer);
    m_printAction->setEnabled(hasCurrentViewer);
    m_nextAction->setEnabled(m_centralWidget->isForwardAvailable());
    m_backAction->setEnabled(m_centralWidget->isBackwardAvailable());
    m_closeTabAction->setEnabled(hasCurrentViewer);
    m_newTabAction->setEnabled(hasCurrentViewer);
}

void MainWindow::showTopicChooser(const QMap<QString, QUrl> &links,
                                  const QString &keyword)
{
    TopicChooser tc(this, keyword, links);
    if (tc.exec() == QDialog::Accepted) {
        m_centralWidget->setSource(tc.link());
    }
}

void MainWindow::showPreferences()
{
    PreferencesDialog dia(m_helpEngine, this);

    connect(&dia, SIGNAL(updateApplicationFont()),
        this, SLOT(updateApplicationFont()));
    connect(&dia, SIGNAL(updateBrowserFont()),
        m_centralWidget, SLOT(updateBrowserFont()));

    dia.showDialog();
}

void MainWindow::syncContents()
{
    qApp->setOverrideCursor(QCursor(Qt::WaitCursor));
    const QUrl url = m_centralWidget->currentSource();
    showContents();
    if (!m_contentWindow->syncToContent(url))
        statusBar()->showMessage(
            tr("Could not find the associated content item."), 3000);
    qApp->restoreOverrideCursor();
}

void MainWindow::copyAvailable(bool yes)
{
    m_copyAction->setEnabled(yes);
}

void MainWindow::addNewBookmark(const QString &title, const QString &url)
{
    if (url.isEmpty())
        return;

    m_bookmarkManager->showBookmarkDialog(this, title, url);
}

void MainWindow::showAboutDialog()
{
    QByteArray contents;
    if (m_helpEngine) {
        QByteArray ba = m_helpEngine->customValue(QLatin1String("AboutTexts"),
            QByteArray()).toByteArray();
        if (!ba.isEmpty()) {
            QString lang;
            QByteArray cba;
            QString currentLang = QLocale::system().name();
            int i = currentLang.indexOf(QLatin1Char('_'));
            if (i > -1)
                currentLang = currentLang.left(i);
            QDataStream s(&ba, QIODevice::ReadOnly);
            while (!s.atEnd()) {
                s >> lang;
                s >> cba;
                if (lang == QLatin1String("default") && contents.isEmpty()) {
                    contents = cba;
                } else if (lang == currentLang) {
                    contents = cba;
                    break;
                }
            }
        }
    }

    AboutDialog aboutDia(this);

    QByteArray iconArray;
    if (!contents.isEmpty()) {
        iconArray = m_helpEngine->customValue(QLatin1String("AboutIcon"),
            QByteArray()).toByteArray();
        QByteArray resources = m_helpEngine->customValue(QLatin1String("AboutImages"),
            QByteArray()).toByteArray();
        QPixmap pix;
        pix.loadFromData(iconArray);
        aboutDia.setText(QString::fromUtf8(contents), resources);
        if (!pix.isNull())
            aboutDia.setPixmap(pix);
        aboutDia.setWindowTitle(aboutDia.documentTitle());
    } else {
#if QT_EDITION == QT_EDITION_OPENSOURCE
        QString edition = tr("Open Source Edition");
        QString info = tr("This version of Qt Assistant is part of the Qt Open Source Edition, for use "
            "in the development of Open Source applications. "
            "Qt is a comprehensive C++ framework for cross-platform application "
            "development.");
        QString moreInfo = tr("You need a commercial Qt license for development of proprietary (closed "
            "source) applications. Please see <a href=\"http://qtsoftware.com/company/about/businessmodel"
            "\">http://qtsoftware.com/company/about/businessmodel</a> for an overview of Qt licensing.");
#else
        QString edition;
        QString info;
        QString moreInfo(tr("This program is licensed to you under the terms of the "
            "Qt Commercial License Agreement. For details, see the file LICENSE "
            "that came with this software distribution."));

#endif
        QByteArray resources;
        aboutDia.setText(QString::fromLatin1("<center>"
            "<h3>%1</h3>"
            "<p>Version %2 %3</p></center>"
            "<p>%4</p>"
            "<p>%5</p>"
            "<p>Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).</p>"
            "<p>The program is provided AS IS with NO WARRANTY OF ANY KIND,"
            " INCLUDING THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A"
            " PARTICULAR PURPOSE.<p/>")
            .arg(tr("Qt Assistant")).arg(QLatin1String(QT_VERSION_STR))
            .arg(edition).arg(info).arg(moreInfo), resources);
        aboutDia.setPixmap(QString::fromLatin1(":/trolltech/assistant/images/assistant-128.png"));
    }
    if (aboutDia.windowTitle().isEmpty())
        aboutDia.setWindowTitle(tr("About %1").arg(windowTitle()));
    aboutDia.exec();
}

void MainWindow::showContents()
{
    activateDockWidget(m_contentWindow);
}

void MainWindow::showIndex()
{
    activateDockWidget(m_indexWindow);
}

void MainWindow::showBookmarks()
{
    activateDockWidget(m_bookmarkWidget);
}

void MainWindow::activateDockWidget(QWidget *w)
{
    w->parentWidget()->show();
    w->parentWidget()->raise();
    w->setFocus();
}

void MainWindow::hideContents()
{
    m_contentWindow->parentWidget()->hide();
}

void MainWindow::hideIndex()
{
    m_indexWindow->parentWidget()->hide();
}

void MainWindow::hideBookmarks()
{
    m_bookmarkWidget->parentWidget()->hide();
}

void MainWindow::setIndexString(const QString &str)
{
    m_indexWindow->setSearchLineEditText(str);
}

void MainWindow::activateCurrentBrowser()
{
    CentralWidget *cw = CentralWidget::instance();
    if (cw) {
        cw->activateTab(true);
    }
}

void MainWindow::activateCurrentCentralWidgetTab()
{
    m_centralWidget->activateTab();
}

void MainWindow::showSearch()
{
    if (m_searchWidget)
        activateDockWidget(m_searchWidget);
    else
        m_centralWidget->activateSearch();
}

void MainWindow::hideSearch()
{
    if (m_searchWidget) {
        m_searchWidget->parentWidget()->parentWidget()->hide();
    } else {
        m_centralWidget->removeSearchWidget();
    }
}

void MainWindow::updateApplicationFont()
{
    QFont font = qApp->font();
    if (m_helpEngine->customValue(QLatin1String("useAppFont")).toBool())
        font = qVariantValue<QFont>(m_helpEngine->customValue(QLatin1String("appFont")));

    qApp->setFont(font, "QWidget");
}

void MainWindow::setupFilterCombo()
{
    QString curFilter = m_filterCombo->currentText();
    if (curFilter.isEmpty())
        curFilter = m_helpEngine->currentFilter();
    m_filterCombo->clear();
    m_filterCombo->addItems(m_helpEngine->customFilters());
    int idx = m_filterCombo->findText(curFilter);
    if (idx < 0)
        idx = 0;
    m_filterCombo->setCurrentIndex(idx);
}

void MainWindow::filterDocumentation(const QString &customFilter)
{
    m_helpEngine->setCurrentFilter(customFilter);
}

void MainWindow::expandTOC(int depth)
{
    m_contentWindow->expandToDepth(depth);
}

void MainWindow::indexingStarted()
{
    m_progressWidget = new QWidget();
    QLayout* hlayout = new QHBoxLayout(m_progressWidget);

    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

    QLabel *label = new QLabel(tr("Updating search index"));
    label->setSizePolicy(sizePolicy);
    hlayout->addWidget(label);

    QProgressBar *progressBar = new QProgressBar();
    progressBar->setRange(0, 0);
    progressBar->setTextVisible(false);
    progressBar->setSizePolicy(sizePolicy);

    hlayout->setSpacing(6);
    hlayout->setMargin(0);
    hlayout->addWidget(progressBar);

    statusBar()->addPermanentWidget(m_progressWidget);
}

void MainWindow::indexingFinished()
{
    statusBar()->removeWidget(m_progressWidget);
    delete m_progressWidget;
    m_progressWidget = 0;
}

QWidget* MainWindow::setupBookmarkWidget()
{
    m_bookmarkManager = new BookmarkManager(m_helpEngine);
    m_bookmarkWidget = new BookmarkWidget(m_bookmarkManager, this);
    connect(m_bookmarkWidget, SIGNAL(addBookmark()),
        this, SLOT(addBookmark()));
    return m_bookmarkWidget;
}

QString MainWindow::collectionFileDirectory(bool createDir, const QString &cacheDir)
{
    QString collectionPath = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
    if (collectionPath.isEmpty()) {
        if (cacheDir.isEmpty())
            collectionPath = QDir::homePath() + QDir::separator()
                + QLatin1String(".assistant");
        else
            collectionPath = QDir::homePath() + QLatin1String("/.")
                + cacheDir;
    } else {
        if (cacheDir.isEmpty())
            collectionPath = collectionPath + QLatin1String("/Trolltech/Assistant");
        else
            collectionPath = collectionPath + QDir::separator()
                + cacheDir;
    }
    collectionPath = QDir::cleanPath(collectionPath);
    if (createDir) {
        QDir dir;
        if (!dir.exists(collectionPath))
            dir.mkpath(collectionPath);
    }
    return collectionPath;
}

QString MainWindow::defaultHelpCollectionFileName()
{
    return collectionFileDirectory() + QDir::separator() +
            QString(QLatin1String("qthelpcollection_%1.qhc")).
            arg(QLatin1String(QT_VERSION_STR));
}

QT_END_NAMESPACE
