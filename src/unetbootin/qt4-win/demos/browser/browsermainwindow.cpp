/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the demonstration applications of the Qt Toolkit.
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

#include "browsermainwindow.h"

#include "autosaver.h"
#include "bookmarks.h"
#include "browserapplication.h"
#include "chasewidget.h"
#include "downloadmanager.h"
#include "history.h"
#include "settings.h"
#include "tabwidget.h"
#include "toolbarsearch.h"
#include "ui_passworddialog.h"
#include "webview.h"

#include <QtCore/QSettings>

#include <QtGui/QDesktopWidget>
#include <QtGui/QFileDialog>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QPrintDialog>
#include <QtGui/QPrintPreviewDialog>
#include <QtGui/QPrinter>
#include <QtGui/QMenuBar>
#include <QtGui/QMessageBox>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QInputDialog>

#include <QtWebKit/QWebFrame>
#include <QtWebKit/QWebHistory>

#include <QtCore/QDebug>

BrowserMainWindow::BrowserMainWindow(QWidget *parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags)
    , m_tabWidget(new TabWidget(this))
    , m_autoSaver(new AutoSaver(this))
    , m_historyBack(0)
    , m_historyForward(0)
    , m_stop(0)
    , m_reload(0)
{
    setAttribute(Qt::WA_DeleteOnClose, true);
    statusBar()->setSizeGripEnabled(true);
    setupMenu();
    setupToolBar();

    QWidget *centralWidget = new QWidget(this);
    BookmarksModel *boomarksModel = BrowserApplication::bookmarksManager()->bookmarksModel();
    m_bookmarksToolbar = new BookmarksToolBar(boomarksModel, this);
    connect(m_bookmarksToolbar, SIGNAL(openUrl(const QUrl&)),
            m_tabWidget, SLOT(loadUrlInCurrentTab(const QUrl&)));
    connect(m_bookmarksToolbar->toggleViewAction(), SIGNAL(toggled(bool)),
            this, SLOT(updateBookmarksToolbarActionText(bool)));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setSpacing(0);
    layout->setMargin(0);
#if defined(Q_WS_MAC)
    layout->addWidget(m_bookmarksToolbar);
    layout->addWidget(new QWidget); // <- OS X tab widget style bug
#else
    addToolBarBreak();
    addToolBar(m_bookmarksToolbar);
#endif
    layout->addWidget(m_tabWidget);
    centralWidget->setLayout(layout);
	setCentralWidget(centralWidget);

    connect(m_tabWidget, SIGNAL(loadPage(const QString &)),
        this, SLOT(loadPage(const QString &)));
    connect(m_tabWidget, SIGNAL(setCurrentTitle(const QString &)),
        this, SLOT(slotUpdateWindowTitle(const QString &)));
    connect(m_tabWidget, SIGNAL(showStatusBarMessage(const QString&)),
            statusBar(), SLOT(showMessage(const QString&)));
    connect(m_tabWidget, SIGNAL(linkHovered(const QString&)),
            statusBar(), SLOT(showMessage(const QString&)));
    connect(m_tabWidget, SIGNAL(loadProgress(int)),
            this, SLOT(slotLoadProgress(int)));
    connect(m_tabWidget, SIGNAL(tabsChanged()),
            m_autoSaver, SLOT(changeOccurred()));
    connect(m_tabWidget, SIGNAL(geometryChangeRequested(const QRect &)),
            this, SLOT(geometryChangeRequested(const QRect &)));
    connect(m_tabWidget, SIGNAL(printRequested(QWebFrame *)),
            this, SLOT(printRequested(QWebFrame *)));
    connect(m_tabWidget, SIGNAL(menuBarVisibilityChangeRequested(bool)),
            menuBar(), SLOT(setVisible(bool)));
    connect(m_tabWidget, SIGNAL(statusBarVisibilityChangeRequested(bool)),
            statusBar(), SLOT(setVisible(bool)));
    connect(m_tabWidget, SIGNAL(toolBarVisibilityChangeRequested(bool)),
            m_navigationBar, SLOT(setVisible(bool)));
    connect(m_tabWidget, SIGNAL(toolBarVisibilityChangeRequested(bool)),
            m_bookmarksToolbar, SLOT(setVisible(bool)));
#if defined(Q_WS_MAC)
    connect(m_tabWidget, SIGNAL(lastTabClosed()),
            this, SLOT(close()));
#else
    connect(m_tabWidget, SIGNAL(lastTabClosed()),
            m_tabWidget, SLOT(newTab()));
#endif

    slotUpdateWindowTitle();
    loadDefaultState();
    m_tabWidget->newTab();

    int size = m_tabWidget->lineEditStack()->sizeHint().height();
    m_navigationBar->setIconSize(QSize(size, size));
}

BrowserMainWindow::~BrowserMainWindow()
{
    m_autoSaver->changeOccurred();
    m_autoSaver->saveIfNeccessary();
}

void BrowserMainWindow::loadDefaultState()
{
    QSettings settings;
    settings.beginGroup(QLatin1String("BrowserMainWindow"));
    QByteArray data = settings.value(QLatin1String("defaultState")).toByteArray();
    restoreState(data);
    settings.endGroup();
}

QSize BrowserMainWindow::sizeHint() const
{
    QRect desktopRect = QApplication::desktop()->screenGeometry();
    QSize size = desktopRect.size() * 0.9;
    return size;
}

void BrowserMainWindow::save()
{
    BrowserApplication::instance()->saveSession();

    QSettings settings;
    settings.beginGroup(QLatin1String("BrowserMainWindow"));
    QByteArray data = saveState(false);
    settings.setValue(QLatin1String("defaultState"), data);
    settings.endGroup();
}

static const qint32 BrowserMainWindowMagic = 0xba;

QByteArray BrowserMainWindow::saveState(bool withTabs) const
{
    int version = 2;
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);

    stream << qint32(BrowserMainWindowMagic);
    stream << qint32(version);

    stream << size();
    stream << !m_navigationBar->isHidden();
    stream << !m_bookmarksToolbar->isHidden();
    stream << !statusBar()->isHidden();
    if (withTabs)
        stream << tabWidget()->saveState();
    else
        stream << QByteArray();
    return data;
}

bool BrowserMainWindow::restoreState(const QByteArray &state)
{
    int version = 2;
    QByteArray sd = state;
    QDataStream stream(&sd, QIODevice::ReadOnly);
    if (stream.atEnd())
        return false;

    qint32 marker;
    qint32 v;
    stream >> marker;
    stream >> v;
    if (marker != BrowserMainWindowMagic || v != version)
        return false;

    QSize size;
    bool showToolbar;
    bool showBookmarksBar;
    bool showStatusbar;
    QByteArray tabState;

    stream >> size;
    stream >> showToolbar;
    stream >> showBookmarksBar;
    stream >> showStatusbar;
    stream >> tabState;

    resize(size);

    m_navigationBar->setVisible(showToolbar);
    updateToolbarActionText(showToolbar);

    m_bookmarksToolbar->setVisible(showBookmarksBar);
    updateBookmarksToolbarActionText(showBookmarksBar);

    statusBar()->setVisible(showStatusbar);
    updateStatusbarActionText(showStatusbar);

    if (!tabWidget()->restoreState(tabState))
        return false;

    return true;
}

void BrowserMainWindow::setupMenu()
{
    new QShortcut(QKeySequence(Qt::Key_F6), this, SLOT(slotSwapFocus()));

    // File
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));

    fileMenu->addAction(tr("&New Window"), this, SLOT(slotFileNew()), QKeySequence::New);
    fileMenu->addAction(m_tabWidget->newTabAction());
    fileMenu->addAction(tr("&Open File..."), this, SLOT(slotFileOpen()), QKeySequence::Open);
    fileMenu->addAction(tr("Open &Location..."), this,
                SLOT(slotSelectLineEdit()), QKeySequence(Qt::ControlModifier + Qt::Key_L));
    fileMenu->addSeparator();
    fileMenu->addAction(m_tabWidget->closeTabAction());
    fileMenu->addSeparator();
    fileMenu->addAction(tr("&Save As..."), this,
                SLOT(slotFileSaveAs()), QKeySequence(QKeySequence::Save));
    fileMenu->addSeparator();
    BookmarksManager *bookmarksManager = BrowserApplication::bookmarksManager();
    fileMenu->addAction(tr("&Import Bookmarks..."), bookmarksManager, SLOT(importBookmarks()));
    fileMenu->addAction(tr("&Export Bookmarks..."), bookmarksManager, SLOT(exportBookmarks()));
    fileMenu->addSeparator();
    fileMenu->addAction(tr("P&rint Preview..."), this, SLOT(slotFilePrintPreview()));
    fileMenu->addAction(tr("&Print..."), this, SLOT(slotFilePrint()), QKeySequence::Print);
    fileMenu->addSeparator();
    QAction *action = fileMenu->addAction(tr("Private &Browsing..."), this, SLOT(slotPrivateBrowsing()));
    action->setCheckable(true);
    fileMenu->addSeparator();

#if defined(Q_WS_MAC)
    fileMenu->addAction(tr("&Quit"), BrowserApplication::instance(), SLOT(quitBrowser()), QKeySequence(Qt::CTRL | Qt::Key_Q));
#else
    fileMenu->addAction(tr("&Quit"), this, SLOT(close()), QKeySequence(Qt::CTRL | Qt::Key_Q));
#endif

    // Edit
    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
    QAction *m_undo = editMenu->addAction(tr("&Undo"));
    m_undo->setShortcuts(QKeySequence::Undo);
    m_tabWidget->addWebAction(m_undo, QWebPage::Undo);
    QAction *m_redo = editMenu->addAction(tr("&Redo"));
    m_redo->setShortcuts(QKeySequence::Redo);
    m_tabWidget->addWebAction(m_redo, QWebPage::Redo);
    editMenu->addSeparator();
    QAction *m_cut = editMenu->addAction(tr("Cu&t"));
    m_cut->setShortcuts(QKeySequence::Cut);
    m_tabWidget->addWebAction(m_cut, QWebPage::Cut);
    QAction *m_copy = editMenu->addAction(tr("&Copy"));
    m_copy->setShortcuts(QKeySequence::Copy);
    m_tabWidget->addWebAction(m_copy, QWebPage::Copy);
    QAction *m_paste = editMenu->addAction(tr("&Paste"));
    m_paste->setShortcuts(QKeySequence::Paste);
    m_tabWidget->addWebAction(m_paste, QWebPage::Paste);
    editMenu->addSeparator();

    QAction *m_find = editMenu->addAction(tr("&Find"));
    m_find->setShortcuts(QKeySequence::Find);
    connect(m_find, SIGNAL(triggered()), this, SLOT(slotEditFind()));
    new QShortcut(QKeySequence(Qt::Key_Slash), this, SLOT(slotEditFind()));

    QAction *m_findNext = editMenu->addAction(tr("&Find Next"));
    m_findNext->setShortcuts(QKeySequence::FindNext);
    connect(m_findNext, SIGNAL(triggered()), this, SLOT(slotEditFindNext()));

    QAction *m_findPrevious = editMenu->addAction(tr("&Find Previous"));
    m_findPrevious->setShortcuts(QKeySequence::FindPrevious);
    connect(m_findPrevious, SIGNAL(triggered()), this, SLOT(slotEditFindPrevious()));

    editMenu->addSeparator();
    editMenu->addAction(tr("&Preferences"), this, SLOT(slotPreferences()), tr("Ctrl+,"));

    // View
    QMenu *viewMenu = menuBar()->addMenu(tr("&View"));

    m_viewBookmarkBar = new QAction(this);
    updateBookmarksToolbarActionText(true);
    m_viewBookmarkBar->setShortcut(tr("Shift+Ctrl+B"));
    connect(m_viewBookmarkBar, SIGNAL(triggered()), this, SLOT(slotViewBookmarksBar()));
    viewMenu->addAction(m_viewBookmarkBar);

    m_viewToolbar = new QAction(this);
    updateToolbarActionText(true);
    m_viewToolbar->setShortcut(tr("Ctrl+|"));
    connect(m_viewToolbar, SIGNAL(triggered()), this, SLOT(slotViewToolbar()));
    viewMenu->addAction(m_viewToolbar);

    m_viewStatusbar = new QAction(this);
    updateStatusbarActionText(true);
    m_viewStatusbar->setShortcut(tr("Ctrl+/"));
    connect(m_viewStatusbar, SIGNAL(triggered()), this, SLOT(slotViewStatusbar()));
    viewMenu->addAction(m_viewStatusbar);

    viewMenu->addSeparator();

    m_stop = viewMenu->addAction(tr("&Stop"));
    QList<QKeySequence> shortcuts;
    shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_Period));
    shortcuts.append(Qt::Key_Escape);
    m_stop->setShortcuts(shortcuts);
    m_tabWidget->addWebAction(m_stop, QWebPage::Stop);

    m_reload = viewMenu->addAction(tr("Reload Page"));
    m_reload->setShortcuts(QKeySequence::Refresh);
    m_tabWidget->addWebAction(m_reload, QWebPage::Reload);

    viewMenu->addAction(tr("&Make Text Bigger"), this, SLOT(slotViewTextBigger()), QKeySequence(Qt::CTRL | Qt::Key_Plus));
    viewMenu->addAction(tr("&Make Text Normal"), this, SLOT(slotViewTextNormal()), QKeySequence(Qt::CTRL | Qt::Key_0));
    viewMenu->addAction(tr("&Make Text Smaller"), this, SLOT(slotViewTextSmaller()), QKeySequence(Qt::CTRL | Qt::Key_Minus));

    viewMenu->addSeparator();
    viewMenu->addAction(tr("Page S&ource"), this, SLOT(slotViewPageSource()), tr("Ctrl+Alt+U"));
    QAction *a = viewMenu->addAction(tr("&Full Screen"), this, SLOT(slotViewFullScreen(bool)),  Qt::Key_F11);
    a->setCheckable(true);

    // History
    HistoryMenu *historyMenu = new HistoryMenu(this);
    connect(historyMenu, SIGNAL(openUrl(const QUrl&)),
            m_tabWidget, SLOT(loadUrlInCurrentTab(const QUrl&)));
    connect(historyMenu, SIGNAL(hovered(const QString&)), this,
            SLOT(slotUpdateStatusbar(const QString&)));
    historyMenu->setTitle(tr("Hi&story"));
    menuBar()->addMenu(historyMenu);
    QList<QAction*> historyActions;

    m_historyBack = new QAction(tr("Back"), this);
    m_tabWidget->addWebAction(m_historyBack, QWebPage::Back);
    m_historyBack->setShortcuts(QKeySequence::Back);
    m_historyBack->setIconVisibleInMenu(false);

    m_historyForward = new QAction(tr("Forward"), this);
    m_tabWidget->addWebAction(m_historyForward, QWebPage::Forward);
    m_historyForward->setShortcuts(QKeySequence::Forward);
    m_historyForward->setIconVisibleInMenu(false);

    QAction *m_historyHome = new QAction(tr("Home"), this);
    connect(m_historyHome, SIGNAL(triggered()), this, SLOT(slotHome()));
    m_historyHome->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_H));

    m_restoreLastSession = new QAction(tr("Restore Last Session"), this);
    connect(m_restoreLastSession, SIGNAL(triggered()), BrowserApplication::instance(), SLOT(restoreLastSession()));
    m_restoreLastSession->setEnabled(BrowserApplication::instance()->canRestoreSession());

    historyActions.append(m_historyBack);
    historyActions.append(m_historyForward);
    historyActions.append(m_historyHome);
    historyActions.append(m_tabWidget->recentlyClosedTabsAction());
    historyActions.append(m_restoreLastSession);
    historyMenu->setInitialActions(historyActions);

    // Bookmarks
    BookmarksMenu *bookmarksMenu = new BookmarksMenu(this);
    connect(bookmarksMenu, SIGNAL(openUrl(const QUrl&)),
            m_tabWidget, SLOT(loadUrlInCurrentTab(const QUrl&)));
    connect(bookmarksMenu, SIGNAL(hovered(const QString&)),
            this, SLOT(slotUpdateStatusbar(const QString&)));
    bookmarksMenu->setTitle(tr("&Bookmarks"));
    menuBar()->addMenu(bookmarksMenu);

    QList<QAction*> bookmarksActions;

    QAction *showAllBookmarksAction = new QAction(tr("Show All Bookmarks"), this);
    connect(showAllBookmarksAction, SIGNAL(triggered()), this, SLOT(slotShowBookmarksDialog()));
    m_addBookmark = new QAction(QIcon(QLatin1String(":addbookmark.png")), tr("Add Bookmark..."), this);
    m_addBookmark->setIconVisibleInMenu(false);

    connect(m_addBookmark, SIGNAL(triggered()), this, SLOT(slotAddBookmark()));
    m_addBookmark->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_D));

    bookmarksActions.append(showAllBookmarksAction);
    bookmarksActions.append(m_addBookmark);
    bookmarksMenu->setInitialActions(bookmarksActions);

    // Window
    m_windowMenu = menuBar()->addMenu(tr("&Window"));
    connect(m_windowMenu, SIGNAL(aboutToShow()),
            this, SLOT(slotAboutToShowWindowMenu()));
    slotAboutToShowWindowMenu();

    QMenu *toolsMenu = menuBar()->addMenu(tr("&Tools"));
    toolsMenu->addAction(tr("Web &Search"), this, SLOT(slotWebSearch()), QKeySequence(tr("Ctrl+K", "Web Search")));
#ifndef Q_CC_MINGW
    a = toolsMenu->addAction(tr("Enable Web &Inspector"), this, SLOT(slotToggleInspector(bool)));
    a->setCheckable(true);
#endif

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(tr("About &Qt"), qApp, SLOT(aboutQt()));
    helpMenu->addAction(tr("About &Demo Browser"), this, SLOT(slotAboutApplication()));
}

void BrowserMainWindow::setupToolBar()
{
    setUnifiedTitleAndToolBarOnMac(true);
    m_navigationBar = addToolBar(tr("Navigation"));
    connect(m_navigationBar->toggleViewAction(), SIGNAL(toggled(bool)),
            this, SLOT(updateToolbarActionText(bool)));

    m_historyBack->setIcon(style()->standardIcon(QStyle::SP_ArrowBack, 0, this));
    m_historyBackMenu = new QMenu(this);
    m_historyBack->setMenu(m_historyBackMenu);
    connect(m_historyBackMenu, SIGNAL(aboutToShow()),
            this, SLOT(slotAboutToShowBackMenu()));
    connect(m_historyBackMenu, SIGNAL(triggered(QAction *)),
            this, SLOT(slotOpenActionUrl(QAction *)));
    m_navigationBar->addAction(m_historyBack);

    m_historyForward->setIcon(style()->standardIcon(QStyle::SP_ArrowForward, 0, this));
    m_historyForwardMenu = new QMenu(this);
    connect(m_historyForwardMenu, SIGNAL(aboutToShow()),
            this, SLOT(slotAboutToShowForwardMenu()));
    connect(m_historyForwardMenu, SIGNAL(triggered(QAction *)),
            this, SLOT(slotOpenActionUrl(QAction *)));
    m_historyForward->setMenu(m_historyForwardMenu);
    m_navigationBar->addAction(m_historyForward);

    m_stopReload = new QAction(this);
    m_reloadIcon = style()->standardIcon(QStyle::SP_BrowserReload);
    m_stopReload->setIcon(m_reloadIcon);

    m_navigationBar->addAction(m_stopReload);

    m_navigationBar->addWidget(m_tabWidget->lineEditStack());

    m_toolbarSearch = new ToolbarSearch(m_navigationBar);
    m_navigationBar->addWidget(m_toolbarSearch);
    connect(m_toolbarSearch, SIGNAL(search(const QUrl&)), SLOT(loadUrl(const QUrl&)));

    m_chaseWidget = new ChaseWidget(this);
    m_navigationBar->addWidget(m_chaseWidget);
}

void BrowserMainWindow::slotShowBookmarksDialog()
{
    BookmarksDialog *dialog = new BookmarksDialog(this);
    connect(dialog, SIGNAL(openUrl(const QUrl&)),
            m_tabWidget, SLOT(loadUrlInCurrentTab(const QUrl&)));
    dialog->show();
}

void BrowserMainWindow::slotAddBookmark()
{
    WebView *webView = currentTab();
    QString url = webView->url().toString();
    QString title = webView->title();
    AddBookmarkDialog dialog(url, title);
    dialog.exec();
}

void BrowserMainWindow::slotViewToolbar()
{
    if (m_navigationBar->isVisible()) {
        updateToolbarActionText(false);
        m_navigationBar->close();
    } else {
        updateToolbarActionText(true);
        m_navigationBar->show();
    }
    m_autoSaver->changeOccurred();
}

void BrowserMainWindow::slotViewBookmarksBar()
{
    if (m_bookmarksToolbar->isVisible()) {
        updateBookmarksToolbarActionText(false);
        m_bookmarksToolbar->close();
    } else {
        updateBookmarksToolbarActionText(true);
        m_bookmarksToolbar->show();
    }
    m_autoSaver->changeOccurred();
}

void BrowserMainWindow::updateStatusbarActionText(bool visible)
{
    m_viewStatusbar->setText(!visible ? tr("Show Status Bar") : tr("Hide Status Bar"));
}

void BrowserMainWindow::updateToolbarActionText(bool visible)
{
    m_viewToolbar->setText(!visible ? tr("Show Toolbar") : tr("Hide Toolbar"));
}

void BrowserMainWindow::updateBookmarksToolbarActionText(bool visible)
{
    m_viewBookmarkBar->setText(!visible ? tr("Show Bookmarks bar") : tr("Hide Bookmarks bar"));
}

void BrowserMainWindow::slotViewStatusbar()
{
    if (statusBar()->isVisible()) {
        updateStatusbarActionText(false);
        statusBar()->close();
    } else {
        updateStatusbarActionText(true);
        statusBar()->show();
    }
    m_autoSaver->changeOccurred();
}

QUrl BrowserMainWindow::guessUrlFromString(const QString &string)
{
    QString urlStr = string.trimmed();
    QRegExp test(QLatin1String("^[a-zA-Z]+\\:.*"));

    // Check if it looks like a qualified URL. Try parsing it and see.
    bool hasSchema = test.exactMatch(urlStr);
    if (hasSchema) {
        QUrl url(urlStr, QUrl::TolerantMode);
        if (url.isValid())
            return url;
    }

    // Might be a file.
    if (QFile::exists(urlStr)) {
        QFileInfo info(urlStr);
        return QUrl::fromLocalFile(info.absoluteFilePath());
    }

    // Might be a shorturl - try to detect the schema.
    if (!hasSchema) {
        int dotIndex = urlStr.indexOf(QLatin1Char('.'));
        if (dotIndex != -1) {
            QString prefix = urlStr.left(dotIndex).toLower();
            QString schema = (prefix == QLatin1String("ftp")) ? prefix : QLatin1String("http");
            QUrl url(schema + QLatin1String("://") + urlStr, QUrl::TolerantMode);
            if (url.isValid())
                return url;
        }
    }

    // Fall back to QUrl's own tolerant parser.
    QUrl url = QUrl(string, QUrl::TolerantMode);

    // finally for cases where the user just types in a hostname add http
    if (url.scheme().isEmpty())
        url = QUrl(QLatin1String("http://") + string, QUrl::TolerantMode);
    return url;
}

void BrowserMainWindow::loadUrl(const QUrl &url)
{
    loadPage(url.toString());
}

void BrowserMainWindow::slotDownloadManager()
{
    BrowserApplication::downloadManager()->show();
}

void BrowserMainWindow::slotSelectLineEdit()
{
    m_tabWidget->currentLineEdit()->selectAll();
    m_tabWidget->currentLineEdit()->setFocus();
}

void BrowserMainWindow::slotFileSaveAs()
{
    BrowserApplication::downloadManager()->download(currentTab()->url(), true);
}

void BrowserMainWindow::slotPreferences()
{
    SettingsDialog *s = new SettingsDialog(this);
    s->show();
}

void BrowserMainWindow::slotUpdateStatusbar(const QString &string)
{
    statusBar()->showMessage(string, 2000);
}

void BrowserMainWindow::slotUpdateWindowTitle(const QString &title)
{
    if (title.isEmpty()) {
        setWindowTitle(tr("Qt Demo Browser"));
    } else {
#if defined(Q_WS_MAC)
        setWindowTitle(title);
#else
        setWindowTitle(tr("%1 - Qt Demo Browser", "Page title and Browser name").arg(title));
#endif
    }
}

void BrowserMainWindow::slotAboutApplication()
{
    QMessageBox::about(this, tr("About"), tr(
        "Version %1"
        "<p>This demo demonstrates Qt's "
        "webkit facilities in action, providing an example "
        "browser for you to experiment with.<p>"
        "<p>QtWebKit is based on the Open Source WebKit Project developed at <a href=\"http://webkit.org/\">http://webkit.org/</a>."
        ).arg(QCoreApplication::applicationVersion()));
}

void BrowserMainWindow::slotFileNew()
{
    BrowserApplication::instance()->newMainWindow();
    BrowserMainWindow *mw = BrowserApplication::instance()->mainWindow();
    mw->slotHome();
}

void BrowserMainWindow::slotFileOpen()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Open Web Resource"), QString(),
            tr("Web Resources (*.html *.htm *.svg *.png *.gif *.svgz);;All files (*.*)"));

    if (file.isEmpty())
        return;

    loadPage(file);
}

void BrowserMainWindow::slotFilePrintPreview()
{
    if (!currentTab())
        return;
    QPrintPreviewDialog *dialog = new QPrintPreviewDialog(this);
    connect(dialog, SIGNAL(paintRequested(QPrinter *)),
            currentTab(), SLOT(print(QPrinter *)));
    dialog->exec();
}

void BrowserMainWindow::slotFilePrint()
{
    if (!currentTab())
        return;
    printRequested(currentTab()->page()->mainFrame());
}

void BrowserMainWindow::printRequested(QWebFrame *frame)
{
    QPrinter printer;
    QPrintDialog *dialog = new QPrintDialog(&printer, this);
    dialog->setWindowTitle(tr("Print Document"));
    if (dialog->exec() != QDialog::Accepted)
        return;
    frame->print(&printer);
}

void BrowserMainWindow::slotPrivateBrowsing()
{
    QWebSettings *settings = QWebSettings::globalSettings();
    bool pb = settings->testAttribute(QWebSettings::PrivateBrowsingEnabled);
    if (!pb) {
        QString title = tr("Are you sure you want to turn on private browsing?");
        QString text = tr("<b>%1</b><br><br>When private browsing in turned on,"
            " webpages are not added to the history,"
            " items are automatically removed from the Downloads window," \
            " new cookies are not stored, current cookies can't be accessed," \
            " site icons wont be stored, session wont be saved, " \
            " and searches are not addded to the pop-up menu in the Google search box." \
            "  Until you close the window, you can still click the Back and Forward buttons" \
            " to return to the webpages you have opened.").arg(title);

        QMessageBox::StandardButton button = QMessageBox::question(this, QString(), text,
                               QMessageBox::Ok | QMessageBox::Cancel,
                               QMessageBox::Ok);
        if (button == QMessageBox::Ok) {
            settings->setAttribute(QWebSettings::PrivateBrowsingEnabled, true);
        }
    } else {
        settings->setAttribute(QWebSettings::PrivateBrowsingEnabled, false);

        QList<BrowserMainWindow*> windows = BrowserApplication::instance()->mainWindows();
        for (int i = 0; i < windows.count(); ++i) {
            BrowserMainWindow *window = windows.at(i);
            window->m_lastSearch = QString::null;
            window->tabWidget()->clear();
        }
    }
}

void BrowserMainWindow::closeEvent(QCloseEvent *event)
{
    if (m_tabWidget->count() > 1) {
        int ret = QMessageBox::warning(this, QString(),
                           tr("Are you sure you want to close the window?"
                              "  There are %1 tab open").arg(m_tabWidget->count()),
                           QMessageBox::Yes | QMessageBox::No,
                           QMessageBox::No);
        if (ret == QMessageBox::No) {
            event->ignore();
            return;
        }
    }
    event->accept();
    deleteLater();
}

void BrowserMainWindow::slotEditFind()
{
    if (!currentTab())
        return;
    bool ok;
    QString search = QInputDialog::getText(this, tr("Find"),
                                          tr("Text:"), QLineEdit::Normal,
                                          m_lastSearch, &ok);
    if (ok && !search.isEmpty()) {
        m_lastSearch = search;
        if (!currentTab()->findText(m_lastSearch))
            slotUpdateStatusbar(tr("\"%1\" not found.").arg(m_lastSearch));
    }
}

void BrowserMainWindow::slotEditFindNext()
{
    if (!currentTab() && !m_lastSearch.isEmpty())
        return;
    currentTab()->findText(m_lastSearch);
}

void BrowserMainWindow::slotEditFindPrevious()
{
    if (!currentTab() && !m_lastSearch.isEmpty())
        return;
    currentTab()->findText(m_lastSearch, QWebPage::FindBackward);
}

void BrowserMainWindow::slotViewTextBigger()
{
    if (!currentTab())
        return;
    currentTab()->setTextSizeMultiplier(currentTab()->textSizeMultiplier() + 0.1);
}

void BrowserMainWindow::slotViewTextNormal()
{
    if (!currentTab())
        return;
    currentTab()->setTextSizeMultiplier(1.0);
}

void BrowserMainWindow::slotViewTextSmaller()
{
    if (!currentTab())
        return;
    currentTab()->setTextSizeMultiplier(currentTab()->textSizeMultiplier() - 0.1);
}

void BrowserMainWindow::slotViewFullScreen(bool makeFullScreen)
{
    if (makeFullScreen) {
        showFullScreen();
    } else {
        if (isMinimized())
            showMinimized();
        else if (isMaximized())
            showMaximized();
        else showNormal();
    }
}

void BrowserMainWindow::slotViewPageSource()
{
    if (!currentTab())
        return;

    QString markup = currentTab()->page()->mainFrame()->toHtml();
    QPlainTextEdit *view = new QPlainTextEdit(markup);
    view->setWindowTitle(tr("Page Source of %1").arg(currentTab()->title()));
    view->setMinimumWidth(640);
    view->setAttribute(Qt::WA_DeleteOnClose);
    view->show();
}

void BrowserMainWindow::slotHome()
{
    QSettings settings;
    settings.beginGroup(QLatin1String("MainWindow"));
    QString home = settings.value(QLatin1String("home"), QLatin1String("http://www.trolltech.com/")).toString();
    loadPage(home);
}

void BrowserMainWindow::slotWebSearch()
{
    m_toolbarSearch->lineEdit()->selectAll();
    m_toolbarSearch->lineEdit()->setFocus();
}

void BrowserMainWindow::slotToggleInspector(bool enable)
{
    QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, enable);
    if (enable) {
        int result = QMessageBox::question(this, tr("Web Inspector"),
                                           tr("The web inspector will only work correctly for pages that were loaded after enabling.\n"
                                           "Do you want to reload all pages?"),
                                           QMessageBox::Yes | QMessageBox::No);
        if (result == QMessageBox::Yes) {
            m_tabWidget->reloadAllTabs();
        }
    }
}

void BrowserMainWindow::slotSwapFocus()
{
    if (currentTab()->hasFocus())
        m_tabWidget->currentLineEdit()->setFocus();
    else
        currentTab()->setFocus();
}

void BrowserMainWindow::loadPage(const QString &page)
{
    if (!currentTab() || page.isEmpty())
        return;

    QUrl url = guessUrlFromString(page);
    m_tabWidget->currentLineEdit()->setText(url.toString());
    m_tabWidget->loadUrlInCurrentTab(url);
}

TabWidget *BrowserMainWindow::tabWidget() const
{
    return m_tabWidget;
}

WebView *BrowserMainWindow::currentTab() const
{
    return m_tabWidget->currentWebView();
}

void BrowserMainWindow::slotLoadProgress(int progress)
{
    if (progress < 100 && progress > 0) {
        m_chaseWidget->setAnimated(true);
        disconnect(m_stopReload, SIGNAL(triggered()), m_reload, SLOT(trigger()));
        if (m_stopIcon.isNull())
            m_stopIcon = style()->standardIcon(QStyle::SP_BrowserStop);
        m_stopReload->setIcon(m_stopIcon);
        connect(m_stopReload, SIGNAL(triggered()), m_stop, SLOT(trigger()));
        m_stopReload->setToolTip(tr("Stop loading the current page"));
    } else {
        m_chaseWidget->setAnimated(false);
        disconnect(m_stopReload, SIGNAL(triggered()), m_stop, SLOT(trigger()));
        m_stopReload->setIcon(m_reloadIcon);
        connect(m_stopReload, SIGNAL(triggered()), m_reload, SLOT(trigger()));
        m_stopReload->setToolTip(tr("Reload the current page"));
    }
}

void BrowserMainWindow::slotAboutToShowBackMenu()
{
    m_historyBackMenu->clear();
    if (!currentTab())
        return;
    QWebHistory *history = currentTab()->history();
    int historyCount = history->count();
    for (int i = history->backItems(historyCount).count() - 1; i >= 0; --i) {
        QWebHistoryItem item = history->backItems(history->count()).at(i);
        QAction *action = new QAction(this);
        action->setData(-1*(historyCount-i-1));
        QIcon icon = BrowserApplication::instance()->icon(item.url());
        action->setIcon(icon);
        action->setText(item.title());
        m_historyBackMenu->addAction(action);
    }
}

void BrowserMainWindow::slotAboutToShowForwardMenu()
{
    m_historyForwardMenu->clear();
    if (!currentTab())
        return;
    QWebHistory *history = currentTab()->history();
    int historyCount = history->count();
    for (int i = 0; i < history->forwardItems(history->count()).count(); ++i) {
        QWebHistoryItem item = history->forwardItems(historyCount).at(i);
        QAction *action = new QAction(this);
        action->setData(historyCount-i);
        QIcon icon = BrowserApplication::instance()->icon(item.url());
        action->setIcon(icon);
        action->setText(item.title());
        m_historyForwardMenu->addAction(action);
    }
}

void BrowserMainWindow::slotAboutToShowWindowMenu()
{
    m_windowMenu->clear();
    m_windowMenu->addAction(m_tabWidget->nextTabAction());
    m_windowMenu->addAction(m_tabWidget->previousTabAction());
    m_windowMenu->addSeparator();
    m_windowMenu->addAction(tr("Downloads"), this, SLOT(slotDownloadManager()), QKeySequence(tr("Alt+Ctrl+L", "Download Manager")));

    m_windowMenu->addSeparator();
    QList<BrowserMainWindow*> windows = BrowserApplication::instance()->mainWindows();
    for (int i = 0; i < windows.count(); ++i) {
        BrowserMainWindow *window = windows.at(i);
        QAction *action = m_windowMenu->addAction(window->windowTitle(), this, SLOT(slotShowWindow()));
        action->setData(i);
        action->setCheckable(true);
        if (window == this)
            action->setChecked(true);
    }
}

void BrowserMainWindow::slotShowWindow()
{
    if (QAction *action = qobject_cast<QAction*>(sender())) {
        QVariant v = action->data();
        if (v.canConvert<int>()) {
            int offset = qvariant_cast<int>(v);
            QList<BrowserMainWindow*> windows = BrowserApplication::instance()->mainWindows();
            windows.at(offset)->activateWindow();
            windows.at(offset)->currentTab()->setFocus();
        }
    }
}

void BrowserMainWindow::slotOpenActionUrl(QAction *action)
{
    int offset = action->data().toInt();
    QWebHistory *history = currentTab()->history();
    if (offset < 0)
        history->goToItem(history->backItems(-1*offset).first()); // back
    else if (offset > 0)
        history->goToItem(history->forwardItems(history->count() - offset + 1).back()); // forward
 }

void BrowserMainWindow::geometryChangeRequested(const QRect &geometry)
{
    setGeometry(geometry);
}

