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

#ifndef CENTRALWIDGET_H
#define CENTRALWIDGET_H

#include <QtCore/QUrl>
#include <QtCore/QPoint>
#include <QtCore/QObject>

#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class QEvent;
class QLabel;
class QAction;
class QCheckBox;
class QLineEdit;
class QToolButton;

class HelpViewer;
class QTabWidget;
class QHelpEngine;
class CentralWidget;
class PrintHelper;
class MainWindow;

class SearchWidget;
class QHelpSearchEngine;

class FindWidget : public QWidget
{
    Q_OBJECT

public:
    FindWidget(QWidget *parent = 0);
    ~FindWidget();

signals:
    void findNext();
    void findPrevious();

private slots:
    void updateButtons();

private:
    QLineEdit *editFind;
    QCheckBox *checkCase;
    QLabel *labelWrapped;
    QToolButton *toolNext;
    QToolButton *toolClose;
    QToolButton *toolPrevious;
    QCheckBox *checkWholeWords;

    friend class CentralWidget;
};

class CentralWidget : public QWidget
{
    Q_OBJECT

public:
    CentralWidget(QHelpEngine *engine, MainWindow *parent);
    ~CentralWidget();

    void setLastShownPages();
    bool hasSelection() const;
    QUrl currentSource() const;
    QString currentTitle() const;
    bool isHomeAvailable() const;
    bool isForwardAvailable() const;
    bool isBackwardAvailable() const;
    QList<QAction*> globalActions() const;
    void setGlobalActions(const QList<QAction*> &actions);
    HelpViewer *currentHelpViewer() const;
    void activateTab(bool onlyHelpViewer = false);
    void activateSearch();
    void createSearchWidget(QHelpSearchEngine *searchEngine);
    void removeSearchWidget();

    static CentralWidget *instance();

public slots:
    void zoomIn();
    void zoomOut();
    void findNext();
    void nextPage();
    void resetZoom();
    void previousPage();
    void findPrevious();
    void copySelection();
    void showTextSearch();
    void print();
    void pageSetup();
    void printPreview();
    void updateBrowserFont();
    void setSource(const QUrl &url);
    void setSourceInNewTab(const QUrl &url);
    void findCurrentText(const QString &text);
    HelpViewer *newEmptyTab();
    void home();
    void forward();
    void backward();

signals:
    void currentViewerChanged();
    void copyAvailable(bool yes);
    void sourceChanged(const QUrl &url);
    void highlighted(const QString &link);
    void forwardAvailable(bool available);
    void backwardAvailable(bool available);
    void addNewBookmark(const QString &title, const QString &url);

protected:
    void keyPressEvent(QKeyEvent *);

private slots:
    void newTab();
    void closeTab();
    void setTabTitle(const QUrl& url);
    void currentPageChanged(int index);
    void showTabBarContextMenu(const QPoint &point);
    void printPreview(QPrinter *printer);

private:
    void connectSignals();    
    bool eventFilter(QObject *object, QEvent *e);
    void find(QString ttf, bool forward, bool backward);
    void initPrinter();

private:
    int lastTabPage;
    QString collectionFile;
    QList<QAction*> globalActionList;

    QWidget *findBar;
    QTabWidget* tabWidget;
    FindWidget *findWidget;
    QHelpEngine *helpEngine;
    QPrinter *printer;
    
    SearchWidget* m_searchWidget;
};

QT_END_NAMESPACE

#endif  // CENTRALWIDGET_H
