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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore/QUrl>
#include <QtGui/QMainWindow>

QT_BEGIN_NAMESPACE

class QAction;
class QLineEdit;
class QComboBox;
class QMenu;

class IndexWindow;
class QHelpEngineCore;
class QHelpEngine;
class CentralWidget;
class ContentWindow;
class BookmarkWidget;
class CmdLineParser;
class QtDocInstaller;

class SearchWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(CmdLineParser *cmdLine, QWidget *parent = 0);
    ~MainWindow();

    static void activateCurrentBrowser();
    static void activateCurrentCentralWidgetTab();

public:
    void hideContents();
    void hideIndex();
    void hideBookmarks();
    void hideSearch();
    void setIndexString(const QString &str);
    void expandTOC(int depth);

signals:
    void initDone();

public slots:
    void showContents();
    void showIndex();
    void showBookmarks();
    void showSearch();
    void syncContents();

private slots:
    void insertLastPages();
    void addBookmark();
    void gotoAddress();
    void showPreferences();
    void showNewAddress();
    void showAboutDialog();    
    void copyAvailable(bool yes);
    void updateNavigationItems();
    void showNewAddress(const QUrl &url);
    void saveBookmarks(const QByteArray &bookmarks);
    void addNewBookmark(const QString &title, const QString &url);
    void showTopicChooser(const QMap<QString, QUrl> &links, const QString &keyword);    
    void updateApplicationFont();
    void filterDocumentation(const QString &customFilter);
    void setupFilterCombo();
    void lookForNewQtDocumentation();
    void indexingStarted();
    void indexingFinished();
    void displayInstallationError(const QString &errorMessage);
    void qtDocumentationInstalled(bool newDocsInstalled);
    void checkInitState();

private:
    bool initHelpDB();
    void setupActions();
    void closeEvent(QCloseEvent *e);
    void activateDockWidget(QWidget *w);
    void updateAboutMenuText();
    void setupFilterToolbar();
    void setupAddressToolbar();
    QMenu *toolBarMenu();
    QWidget *setupBookmarkWidget();    

    QHelpEngine *m_helpEngine;
    CentralWidget *m_centralWidget;
    IndexWindow *m_indexWindow;
    ContentWindow *m_contentWindow;
    BookmarkWidget *m_bookmarkWidget;
    QLineEdit *m_addressLineEdit;
    QComboBox *m_filterCombo;

    QAction *m_backAction;
    QAction *m_nextAction;
    QAction *m_homeAction;
    QAction *m_syncAction;
    QAction *m_copyAction;
    QAction *m_findAction;
    QAction *m_printAction;
    QAction *m_printPreviewAction;
    QAction *m_pageSetupAction;
    QAction *m_zoomInAction;
    QAction *m_zoomOutAction;
    QAction *m_resetZoomAction;
    QAction *m_aboutAction;
    QAction *m_closeTabAction;
    QAction *m_newTabAction;

    QMenu *m_viewMenu;
    QMenu *m_toolBarMenu;

    CmdLineParser *m_cmdLine;
    SearchWidget *m_searchWidget;

    QWidget *m_progressWidget;
    QtDocInstaller *m_qtDocInstaller;

    bool m_connectedInitSignals;
};

QT_END_NAMESPACE

#endif // MAINWINDOW_H
