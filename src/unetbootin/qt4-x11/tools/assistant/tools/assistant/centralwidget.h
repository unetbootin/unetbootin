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

    void closeTabAt(int index);
    QMap<int, QString> currentSourceFileList() const;

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
    void setSourceInNewTab(const QUrl &url, qreal zoom = 0.0);
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
    void setSourceFromSearch(const QUrl &url);
    void setSourceFromSearchInNewTab(const QUrl &url);

private:
    void connectSignals();    
    bool eventFilter(QObject *object, QEvent *e);
    void find(QString ttf, bool forward, bool backward);
    void initPrinter();
    QString quoteTabTitle(const QString &title) const;
    void highlightSearchTerms();

private:
    int lastTabPage;
    QString collectionFile;
    QList<QAction*> globalActionList;

    QWidget *findBar;
    QTabWidget* tabWidget;
    FindWidget *findWidget;
    QHelpEngine *helpEngine;
    QPrinter *printer;
    bool usesDefaultCollection;
    
    SearchWidget* m_searchWidget;
};

QT_END_NAMESPACE

#endif  // CENTRALWIDGET_H
