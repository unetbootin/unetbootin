/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt Assistant of the Qt Toolkit.
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

#include "centralwidget.h"
#include "helpviewer.h"
#include "searchwidget.h"
#include "mainwindow.h"

#include <QtCore/QDir>
#include <QtCore/QEvent>
#include <QtCore/QTimer>

#include <QtGui/QMenu>
#include <QtGui/QLabel>
#include <QtGui/QLayout>
#include <QtGui/QPrinter>
#include <QtGui/QLineEdit>
#include <QtGui/QCheckBox>
#include <QtGui/QTabBar>
#include <QtGui/QTabWidget>
#include <QtGui/QToolButton>
#include <QtGui/QMouseEvent>
#include <QtGui/QSpacerItem>
#include <QtGui/QTextCursor>
#include <QtGui/QPrintDialog>
#include <QtGui/QApplication>
#include <QtGui/QTextDocumentFragment>
#include <QtGui/QPrintPreviewDialog>
#include <QtGui/QPageSetupDialog>

#include <QtHelp/QHelpEngine>

QT_BEGIN_NAMESPACE

namespace {
    HelpViewer* helpViewerFromTabPosition(const QTabWidget *widget, const QPoint &point) 
    {
        QTabBar *tabBar = qFindChild<QTabBar*>(widget);
        for (int i = 0; i < tabBar->count(); ++i) {
            if (tabBar->tabRect(i).contains(point))
                return qobject_cast<HelpViewer*>(widget->widget(i));
        }
        return 0;
    }
    CentralWidget *staticCentralWidget = 0;
}

FindWidget::FindWidget(QWidget *parent)
    : QWidget(parent)
{
    QString system = QLatin1String("win");
    QHBoxLayout *hboxLayout = new QHBoxLayout(this);
#ifdef Q_OS_MAC
    system = QLatin1String("mac");
#else
    hboxLayout->setSpacing(6);
    hboxLayout->setMargin(0);
#endif

    toolClose = new QToolButton(this);    
    toolClose->setIcon(QIcon(QString::fromUtf8(":/trolltech/assistant/images/%1/closetab.png").arg(system)));
    toolClose->setAutoRaise(true);
    hboxLayout->addWidget(toolClose);

    editFind = new QLineEdit(this);
    editFind->setMinimumSize(QSize(150, 0));
    connect(editFind, SIGNAL(textChanged(const QString&)),
        this, SLOT(updateButtons()));
    hboxLayout->addWidget(editFind);

    toolPrevious = new QToolButton(this);
    toolPrevious->setAutoRaise(true);
    toolPrevious->setText(tr("Previous"));
    toolPrevious->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    toolPrevious->setIcon(QIcon(QString::fromUtf8(":/trolltech/assistant/images/%1/previous.png").arg(system)));
    hboxLayout->addWidget(toolPrevious);

    toolNext = new QToolButton(this);
    toolNext->setAutoRaise(true);
    toolNext->setText(tr("Next"));
    toolNext->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    toolNext->setIcon(QIcon(QString::fromUtf8(":/trolltech/assistant/images/%1/next.png").arg(system)));
    hboxLayout->addWidget(toolNext);

    checkCase = new QCheckBox(tr("Case Sensitive"), this);
    hboxLayout->addWidget(checkCase);

    checkWholeWords = new QCheckBox(tr("Whole words"), this);
    hboxLayout->addWidget(checkWholeWords);
#if defined(USE_WEBKIT)
    checkWholeWords->hide();
#endif

    labelWrapped = new QLabel(this);
    labelWrapped->setMinimumSize(QSize(0, 20));
    labelWrapped->setMaximumSize(QSize(105, 20));
    labelWrapped->setTextFormat(Qt::RichText);
    labelWrapped->setScaledContents(true);    
    labelWrapped->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
    labelWrapped->setText(tr("<img src=\":/trolltech/assistant/images/wrap.png\">&nbsp;Search wrapped"));
    hboxLayout->addWidget(labelWrapped);    

    QSpacerItem *spacerItem = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    hboxLayout->addItem(spacerItem);
    setMinimumWidth(minimumSizeHint().width());
    labelWrapped->hide();

    updateButtons();
}

FindWidget::~FindWidget()
{
}

void FindWidget::updateButtons()
{
    if (editFind->text().isEmpty()) {
        toolPrevious->setEnabled(false);
        toolNext->setEnabled(false);
    } else {
        toolPrevious->setEnabled(true);
        toolNext->setEnabled(true);
    }
}


CentralWidget::CentralWidget(QHelpEngine *engine, MainWindow *parent)
    : QWidget(parent)
    , findBar(0)
    , tabWidget(0)
    , helpEngine(engine)
    , printer(0)
    , m_searchWidget(0)
{
    lastTabPage = 0;
    globalActionList.clear();
    collectionFile = helpEngine->collectionFile();
    
    QString system = QLatin1String("win");
    QVBoxLayout *vboxLayout = new QVBoxLayout(this);
    
#ifdef Q_OS_MAC
    system = QLatin1String("mac");
#else
    vboxLayout->setMargin(0);
#endif

    tabWidget = new QTabWidget(this);
    connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(currentPageChanged(int)));

    QToolButton *newTabButton = new QToolButton(this);
    newTabButton->setAutoRaise(true);
    newTabButton->setToolTip(tr("Add new page"));
    newTabButton->setIcon(QIcon(QString::fromUtf8(":/trolltech/assistant/images/%1/addtab.png").arg(system)));

    tabWidget->setCornerWidget(newTabButton, Qt::TopLeftCorner);
    connect(newTabButton, SIGNAL(clicked()), this, SLOT(newTab()));

    QToolButton *closeTabButton = new QToolButton(this);
    closeTabButton->setEnabled(false);
    closeTabButton->setAutoRaise(true);
    closeTabButton->setToolTip(tr("Close current page"));
    closeTabButton->setIcon(QIcon(QString::fromUtf8(":/trolltech/assistant/images/%1/closetab.png").arg(system)));

    tabWidget->setCornerWidget(closeTabButton, Qt::TopRightCorner);
    connect(closeTabButton, SIGNAL(clicked()), this, SLOT(closeTab()));

    vboxLayout->addWidget(tabWidget);

    findBar = new QWidget(this);
    findWidget = new FindWidget(findBar);
    findBar->setMinimumHeight(findWidget->minimumSizeHint().height());
    findWidget->move(0, 0);
    vboxLayout->addWidget(findBar);
    findBar->hide();
    findWidget->editFind->installEventFilter(this);
    connect(findWidget->toolClose, SIGNAL(clicked()), findBar, SLOT(hide()));
    
    connect(findWidget->toolNext, SIGNAL(clicked()), this, SLOT(findNext()));
    connect(findWidget->editFind, SIGNAL(returnPressed()), this, SLOT(findNext()));
    connect(findWidget->editFind, SIGNAL(textChanged(const QString&)), this, SLOT(findCurrentText(const QString&)));
    connect(findWidget->toolPrevious, SIGNAL(clicked()), this, SLOT(findPrevious()));

    QTabBar *tabBar = qFindChild<QTabBar*>(tabWidget);
    if (tabBar) {
        tabBar->installEventFilter(this);
        tabBar->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(tabBar, SIGNAL(customContextMenuRequested(const QPoint&)), 
                this, SLOT(showTabBarContextMenu(const QPoint&)));
    }

    staticCentralWidget = this;
}

CentralWidget::~CentralWidget()
{
    QDir dir;
    QString currentPages;
    QHelpEngineCore engine(collectionFile, 0);

    if (engine.setupData()) {
        for (int i = 1; i < tabWidget->count(); ++i) {
            HelpViewer *viewer = qobject_cast<HelpViewer*>(tabWidget->widget(i));
            if (viewer && viewer->source().isValid())
                currentPages.append(viewer->source().toString()).append(QLatin1Char('|'));
        }
        engine.setCustomValue(QLatin1String("LastTabPage"), lastTabPage);
        engine.setCustomValue(QLatin1String("LastShownPages"), currentPages);
    }
}

CentralWidget *CentralWidget::instance()
{
    return staticCentralWidget;
}

void CentralWidget::newTab()
{
    HelpViewer* viewer = currentHelpViewer();
    if (viewer)
        setSourceInNewTab(viewer->source());
}

void CentralWidget::zoomIn()
{
    HelpViewer* viewer = currentHelpViewer();
    if (viewer)
        viewer->zoomIn();

    if (tabWidget->currentWidget() == m_searchWidget)
        m_searchWidget->zoomIn();
}

void CentralWidget::zoomOut()
{
    HelpViewer* viewer = currentHelpViewer();
    if (viewer)
        viewer->zoomOut();

    if (tabWidget->currentWidget() == m_searchWidget)
        m_searchWidget->zoomOut();
}

void CentralWidget::findNext()
{
	find(findWidget->editFind->text(), true, false);
}

void CentralWidget::nextPage()
{
    if(tabWidget->currentIndex() < tabWidget->count() -1)
        tabWidget->setCurrentIndex(tabWidget->currentIndex() +1);
    else
        tabWidget->setCurrentIndex(0);
}

void CentralWidget::resetZoom()
{
    HelpViewer* viewer = currentHelpViewer();
    if (viewer)
        viewer->resetZoom();

    if (tabWidget->currentWidget() == m_searchWidget)
        m_searchWidget->resetZoom();
}

void CentralWidget::previousPage()
{
    int index = tabWidget->currentIndex() -1;
    if(index >= 0)
        tabWidget->setCurrentIndex(index);
    else
        tabWidget->setCurrentIndex(tabWidget->count() -1);
}

void CentralWidget::findPrevious()
{
	find(findWidget->editFind->text(), false, true);
}

void CentralWidget::closeTab()
{
    HelpViewer* viewer = currentHelpViewer();
    if (!viewer|| tabWidget->count() == 1)
        return;

    tabWidget->removeTab(tabWidget->indexOf(viewer));
    QTimer::singleShot(0, viewer, SLOT(deleteLater()));
}

void CentralWidget::setSource(const QUrl &url)
{
    HelpViewer* viewer = currentHelpViewer();
    HelpViewer* lastViewer = qobject_cast<HelpViewer*>(tabWidget->widget(lastTabPage));

    if (!viewer && !lastViewer) {
        viewer = new HelpViewer(helpEngine, this);
        viewer->installEventFilter(this);
        lastTabPage = tabWidget->addTab(viewer, QString());
        tabWidget->setCurrentIndex(lastTabPage);
        connectSignals();
    } else
        viewer = lastViewer;

    viewer->setSource(url);
    currentPageChanged(lastTabPage);
    viewer->setFocus(Qt::OtherFocusReason);
    tabWidget->setCurrentIndex(lastTabPage);
    tabWidget->setTabText(lastTabPage, viewer->documentTitle());
}

void CentralWidget::setLastShownPages()
{
    const QStringList lastShownPageList = helpEngine->customValue(QLatin1String("LastShownPages")).
        toString().split(QLatin1Char('|'), QString::SkipEmptyParts);

    if (!lastShownPageList.isEmpty()) {
        foreach (const QString page, lastShownPageList)
            setSourceInNewTab(page);
        
        tabWidget->setCurrentIndex(helpEngine->customValue(QLatin1String("LastTabPage"), 1).toInt());
    } else {
        setSource(QUrl(QLatin1String("help")));
    }

    updateBrowserFont();
}

bool CentralWidget::hasSelection() const
{
    const HelpViewer* viewer = currentHelpViewer();
    return viewer ? viewer->hasSelection() : false;
}

QUrl CentralWidget::currentSource() const
{
    const HelpViewer* viewer = currentHelpViewer();
    if (viewer)
        return viewer->source();

    return QUrl();
}

QString CentralWidget::currentTitle() const
{
    const HelpViewer* viewer = currentHelpViewer();
    if (viewer)
        return viewer->documentTitle();

    return QString();
}

void CentralWidget::copySelection()
{
    HelpViewer* viewer = currentHelpViewer();
    if (viewer)
        viewer->copy();
}

void CentralWidget::showTextSearch()
{
    findBar->show();
    findWidget->editFind->selectAll();
    findWidget->editFind->setFocus(Qt::ShortcutFocusReason);
}

void CentralWidget::initPrinter()
{
#ifndef QT_NO_PRINTER
    if (!printer)
        printer = new QPrinter(QPrinter::HighResolution);
#endif
}

void CentralWidget::print()
{
#ifndef QT_NO_PRINTER
    HelpViewer* viewer = currentHelpViewer();
    if (!viewer)
        return;

    initPrinter();

    QPrintDialog *dlg = new QPrintDialog(printer, this);
#if !defined(USE_WEBKIT)
    if (viewer->textCursor().hasSelection())
        dlg->addEnabledOption(QAbstractPrintDialog::PrintSelection);
#endif
    dlg->addEnabledOption(QAbstractPrintDialog::PrintPageRange);
    dlg->addEnabledOption(QAbstractPrintDialog::PrintCollateCopies);
    dlg->setWindowTitle(tr("Print Document"));
    if (dlg->exec() == QDialog::Accepted) {
        viewer->print(printer);
    }
    delete dlg;
#endif
}

void CentralWidget::printPreview()
{
#ifndef QT_NO_PRINTER
    initPrinter();
    QPrintPreviewDialog preview(printer, this);
    connect(&preview, SIGNAL(paintRequested(QPrinter *)), SLOT(printPreview(QPrinter *)));
    preview.exec();    
#endif
}

void CentralWidget::printPreview(QPrinter *p)
{
#ifndef QT_NO_PRINTER
    HelpViewer *viewer = currentHelpViewer();
    if (viewer)
        viewer->print(p);
#endif
}

void CentralWidget::pageSetup()
{
#ifndef QT_NO_PRINTER
    initPrinter();    
    QPageSetupDialog dlg(printer);
    dlg.exec();
#endif
}

bool CentralWidget::isHomeAvailable() const
{
    return currentHelpViewer() ? true : false;
}

void CentralWidget::home()
{
    HelpViewer* viewer = currentHelpViewer();
    if (viewer)
        viewer->home();
}

bool CentralWidget::isForwardAvailable() const
{
    const HelpViewer* viewer = currentHelpViewer();
    if (viewer)
        return viewer->isForwardAvailable();

    return false;
}

void CentralWidget::forward()
{
    HelpViewer* viewer = currentHelpViewer();
    if (viewer)
        viewer->forward();
}

bool CentralWidget::isBackwardAvailable() const
{
    const HelpViewer* viewer = currentHelpViewer();
    if (viewer)
        return viewer->isBackwardAvailable();

    return false;
}

void CentralWidget::backward()
{
    HelpViewer* viewer = currentHelpViewer();
    if (viewer)
        viewer->backward();
}


QList<QAction*> CentralWidget::globalActions() const
{
    return globalActionList;
}

void CentralWidget::setGlobalActions(const QList<QAction*> &actions)
{
    globalActionList = actions;
}
void CentralWidget::setSourceInNewTab(const QUrl &url)
{
    HelpViewer* viewer = new HelpViewer(helpEngine, this);
    viewer->installEventFilter(this);
    viewer->setSource(url);
    viewer->setFocus(Qt::OtherFocusReason);
    tabWidget->setCurrentIndex(tabWidget->addTab(viewer, viewer->documentTitle()));

    QFont font = qApp->font();
    if (helpEngine->customValue(QLatin1String("useBrowserFont")).toBool())
        font = qVariantValue<QFont>(helpEngine->customValue(QLatin1String("browserFont")));

    viewer->setFont(font);

    connectSignals();
}

void CentralWidget::findCurrentText(const QString &text)
{
    find(text, false, false);
}

HelpViewer *CentralWidget::newEmptyTab()
{
    HelpViewer* viewer = new HelpViewer(helpEngine, this);
    viewer->installEventFilter(this);
    viewer->setFocus(Qt::OtherFocusReason);
#if !defined(USE_WEBKIT)
    viewer->setDocumentTitle(tr("unknown"));
#endif
    tabWidget->setCurrentIndex(tabWidget->addTab(viewer, tr("unknown")));
    
    connectSignals();
    return viewer;
}

void CentralWidget::connectSignals()
{
    const HelpViewer* viewer = currentHelpViewer();
    if (viewer) {
        connect(viewer, SIGNAL(copyAvailable(bool)), this, SIGNAL(copyAvailable(bool)));
        connect(viewer, SIGNAL(forwardAvailable(bool)), this, SIGNAL(forwardAvailable(bool)));
        connect(viewer, SIGNAL(backwardAvailable(bool)), this, SIGNAL(backwardAvailable(bool)));
        connect(viewer, SIGNAL(sourceChanged(const QUrl&)), this, SIGNAL(sourceChanged(const QUrl&)));
        connect(viewer, SIGNAL(highlighted(const QString&)), this, SIGNAL(highlighted(const QString&)));

        connect(viewer, SIGNAL(sourceChanged(const QUrl&)), this, SLOT(setTabTitle(const QUrl&)));
    }
}

HelpViewer *CentralWidget::currentHelpViewer() const
{
    return qobject_cast<HelpViewer*>(tabWidget->currentWidget());    
}

void CentralWidget::activateTab(bool onlyHelpViewer)
{
    if (currentHelpViewer()) {
        currentHelpViewer()->setFocus();
    } else {
        int idx = 0;
        if (onlyHelpViewer)
            idx = lastTabPage;        
        tabWidget->setCurrentIndex(idx);
        tabWidget->currentWidget()->setFocus();        
    }
}

void CentralWidget::setTabTitle(const QUrl& url)
{
    Q_UNUSED(url)
    const HelpViewer* viewer = currentHelpViewer();
    if (viewer)
        tabWidget->setTabText(lastTabPage, viewer->documentTitle().trimmed());
}

void CentralWidget::currentPageChanged(int index)
{
    const HelpViewer *viewer = currentHelpViewer();

    if (viewer || tabWidget->count() == 1)
        lastTabPage = index;

    bool enabled = false;
    if (viewer) {
        enabled = true;
        if (!m_searchWidget)
            enabled = tabWidget->count() > 1;
    }

    tabWidget->cornerWidget(Qt::TopRightCorner)->setEnabled(enabled);
    tabWidget->cornerWidget(Qt::TopLeftCorner)->setEnabled(m_searchWidget ? enabled : true);

     emit currentViewerChanged();
}

void CentralWidget::showTabBarContextMenu(const QPoint &point)
{
    HelpViewer* viewer = helpViewerFromTabPosition(tabWidget, point);
    if (!viewer)
        return;

    QTabBar *tabBar = qFindChild<QTabBar*>(tabWidget);

    QMenu menu(QLatin1String(""), tabBar);
    QAction *new_page = menu.addAction(tr("Add New Page"));
    QAction *close_page = menu.addAction(tr("Close This Page"));
    QAction *close_pages = menu.addAction(tr("Close Other Pages"));
    menu.addSeparator();
    QAction *newBookmark = menu.addAction(tr("Add Bookmark for this Page..."));

    if (tabBar->count() == 1) {
        close_page->setEnabled(false); 
        close_pages->setEnabled(false);
    } else if (m_searchWidget && tabBar->count() == 2) {
        close_pages->setEnabled(false);
    }

    QAction *picked_action = menu.exec(tabBar->mapToGlobal(point));
    if (!picked_action)
        return;

    if (picked_action == new_page)
        setSourceInNewTab(viewer->source());

    if (picked_action == close_page) {
        tabWidget->removeTab(tabWidget->indexOf(viewer));
        QTimer::singleShot(0, viewer, SLOT(deleteLater()));
    }

    if (picked_action == close_pages) {
        int currentPage = tabWidget->indexOf(viewer);
        for (int i = tabBar->count() -1; i >= 0; --i) {
            viewer = qobject_cast<HelpViewer*>(tabWidget->widget(i));
            if (i != currentPage && viewer) {
                tabWidget->removeTab(i);
                QTimer::singleShot(0, viewer, SLOT(deleteLater()));

                if (i < currentPage)
                    --currentPage;
            }
        }
    }

    if (picked_action == newBookmark)
        emit addNewBookmark(viewer->documentTitle(), viewer->source().toString());
}

bool CentralWidget::eventFilter(QObject *object, QEvent *e)
{
    if (currentHelpViewer() == object && e->type() == QEvent::KeyPress){
        QKeyEvent *ke = static_cast<QKeyEvent*>(e);
        if (ke->key() == Qt::Key_Backspace) {
            HelpViewer *viewer = currentHelpViewer();
            if (viewer && viewer->isBackwardAvailable())
                viewer->backward();
            return true;
        }
    }

    QTabBar *tabBar = qobject_cast<QTabBar*>(object);
    bool mousRel = e->type() == QEvent::MouseButtonRelease;
    bool dblClick = e->type() == QEvent::MouseButtonDblClick;

    if (tabBar && (mousRel || dblClick)) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(e);
        HelpViewer *viewer = helpViewerFromTabPosition(tabWidget, mouseEvent->pos());
        if (!m_searchWidget && tabWidget->count() <= 1)
            return QWidget::eventFilter(object, e);

        if (viewer && (mouseEvent->button() == Qt::MidButton || dblClick)) {
            tabWidget->removeTab(tabWidget->indexOf(viewer));
            QTimer::singleShot(0, viewer, SLOT(deleteLater()));
            currentPageChanged(tabWidget->currentIndex());
            return true;
        }
    } else if (object == findWidget->editFind && e->type() == QEvent::KeyPress) {
        QKeyEvent *ke = static_cast<QKeyEvent*>(e);
        if (ke->key() == Qt::Key_Escape) {
            findBar->hide();
            HelpViewer *hv = currentHelpViewer();
            if (hv)
                hv->setFocus();            
        }
    }
    return QWidget::eventFilter(object, e);
}

void CentralWidget::keyPressEvent(QKeyEvent *e)
{
    if (!findBar->isVisible()) {
        QString text = e->text();
        if (text.startsWith(QLatin1Char('/'))) {
            findBar->show();
            findWidget->editFind->clear();
            findWidget->editFind->setFocus();
            return;
        }
    }

    QWidget::keyPressEvent(e);
}

void CentralWidget::find(QString ttf, bool forward, bool backward)
{
    QTextCursor cursor;
    QTextDocument *doc = 0;
    QTextBrowser *browser = 0;

    HelpViewer* viewer = currentHelpViewer();
    QPalette p = findWidget->editFind->palette();
    p.setColor(QPalette::Active, QPalette::Base, Qt::white);

#if defined(USE_WEBKIT)
    if (viewer) {
        QWebPage::FindFlags options;
        if (!forward)
            options |= QWebPage::FindBackward;

        if (findWidget->checkCase->isChecked())
            options |= QWebPage::FindCaseSensitively;

        bool found = viewer->findText(ttf, options);
        findWidget->labelWrapped->hide();

        if (!found) {
            options |= QWebPage::FindWrapsAroundDocument;
            found = viewer->findText(ttf, options);

            if (!found) {
                p.setColor(QPalette::Active, QPalette::Base, QColor(255, 102, 102));
            } else {
                findWidget->labelWrapped->show();
            }
        }
    }
#else
    if (viewer) {
        doc = viewer->document();
        cursor = viewer->textCursor();
        browser = qobject_cast<QTextBrowser*>(viewer);
    }

    if (tabWidget->currentWidget() == m_searchWidget) {
        QTextBrowser* browser = qFindChild<QTextBrowser*>(m_searchWidget);
        if (browser) {
            doc = browser->document();
            cursor = browser->textCursor();
        }
    }

    if (!browser || !doc || cursor.isNull())
        return;

    QTextDocument::FindFlags options;

    if (cursor.hasSelection())
        cursor.setPosition(forward ? cursor.position() : cursor.anchor(), QTextCursor::MoveAnchor);

    QTextCursor newCursor = cursor;

    if (!ttf.isEmpty()) {
        if (backward)
            options |= QTextDocument::FindBackward;

        if (findWidget->checkCase->isChecked())
            options |= QTextDocument::FindCaseSensitively;

        if (findWidget->checkWholeWords->isChecked())
            options |= QTextDocument::FindWholeWords;

        newCursor = doc->find(ttf, cursor, options);
        findWidget->labelWrapped->hide();

        if (newCursor.isNull()) {
            QTextCursor ac(doc);
            ac.movePosition(options & QTextDocument::FindBackward
                    ? QTextCursor::End : QTextCursor::Start);
            newCursor = doc->find(ttf, ac, options);
            if (newCursor.isNull()) {
                p.setColor(QPalette::Active, QPalette::Base, QColor(255, 102, 102));
                newCursor = cursor;
            } else {
                findWidget->labelWrapped->show();
            }
        }
    }
#endif

    if (!findWidget->isVisible())
        findWidget->show();

#if !defined(USE_WEBKIT)
    if (browser)
        browser->setTextCursor(newCursor);
#endif
    findWidget->editFind->setPalette(p);
}

void CentralWidget::activateSearch()
{
    if (tabWidget->widget(0) != m_searchWidget)
        createSearchWidget(helpEngine->searchEngine());
    
    tabWidget->setCurrentWidget(m_searchWidget);
    m_searchWidget->setFocus();
}

void CentralWidget::updateBrowserFont()
{
    QFont font = qApp->font();
    if (helpEngine->customValue(QLatin1String("useBrowserFont")).toBool())
        font = qVariantValue<QFont>(helpEngine->customValue(QLatin1String("browserFont")));

    QWidget* widget = 0;
    for (int i = 0; i < tabWidget->count(); ++i) {
        widget = tabWidget->widget(i);
        if (widget->font() != font)
            widget->setFont(font);
    }
}

void CentralWidget::createSearchWidget(QHelpSearchEngine *searchEngine)
{
    if (!m_searchWidget) {
        m_searchWidget = new SearchWidget(searchEngine, this);
        connect(m_searchWidget, SIGNAL(requestShowLink(const QUrl&)), this, 
            SLOT(setSource(const QUrl&)));
        connect(m_searchWidget, SIGNAL(requestShowLinkInNewTab(const QUrl&)), this, 
            SLOT(setSourceInNewTab(const QUrl&)));
    }
    tabWidget->insertTab(0, m_searchWidget, tr("Search"));
}

void CentralWidget::removeSearchWidget()
{
    tabWidget->removeTab(0);    
}

QT_END_NAMESPACE
