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

#ifndef HELPVIEWER_H
#define HELPVIEWER_H

#include <QtCore/QUrl>
#include <QtCore/QVariant>
#include <QtGui/QTextBrowser>
#include <QtGui/QAction>

#if !defined(QT_NO_WEBKIT)
#include <QWebView>
#endif

QT_BEGIN_NAMESPACE

class QHelpEngine;
class CentralWidget;

class QPoint;
class QString;
class QKeyEvent;
class QMouseEvent;
class QContextMenuEvent;

#if !defined(QT_NO_WEBKIT)

class HelpViewer : public QWebView
{
    Q_OBJECT

public:
    HelpViewer(QHelpEngine *helpEngine, CentralWidget *parent);
    void setSource(const QUrl &url);

    inline QUrl source() const
    { return url(); }

    inline QString documentTitle() const
    { return title(); }

    inline bool hasSelection() const
    { return !selectedText().isEmpty(); } // ### this is suboptimal

    void resetZoom();
    void zoomIn(qreal range = 1);
    void zoomOut(qreal range = 1);

    inline void copy()
    { return triggerPageAction(QWebPage::Copy); }

    inline bool isForwardAvailable() const
    { return pageAction(QWebPage::Forward)->isEnabled(); }
    inline bool isBackwardAvailable() const
    { return pageAction(QWebPage::Back)->isEnabled(); }

public Q_SLOTS:
    void home();
    void backward() { back(); }

Q_SIGNALS:
    void copyAvailable(bool enabled);
    void forwardAvailable(bool enabled);
    void backwardAvailable(bool enabled);
    void highlighted(const QString &);
    void sourceChanged(const QUrl &);

protected:
    virtual void wheelEvent(QWheelEvent *);
    void mouseReleaseEvent(QMouseEvent *e);

private Q_SLOTS:
    void actionChanged();

private:
    QHelpEngine *helpEngine;
    CentralWidget* parentWidget;
};

#else

class HelpViewer : public QTextBrowser
{
    Q_OBJECT

public:
    HelpViewer(QHelpEngine *helpEngine, CentralWidget *parent);
    void setSource(const QUrl &url);

    void resetZoom();
    void zoomIn(int range = 1);
    void zoomOut(int range = 1);
    int zoom() const { return zoomCount; }
    void setZoom(int zoom) { zoomCount = zoom; }

    inline bool hasSelection() const
    { return textCursor().hasSelection(); }

    bool launchedWithExternalApp(const QUrl &url);

public Q_SLOTS:
    void home();

protected:
    void wheelEvent(QWheelEvent *e);

private:
    QVariant loadResource(int type, const QUrl &name);    
    void openLinkInNewTab(const QString &link);
    bool hasAnchorAt(const QPoint& pos);
    void contextMenuEvent(QContextMenuEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void keyPressEvent(QKeyEvent *e);

private slots:
    void openLinkInNewTab();

private:
    int zoomCount;
    bool controlPressed;
    QString lastAnchor;
    QHelpEngine *helpEngine;
    CentralWidget* parentWidget;
};

#endif

QT_END_NAMESPACE

#endif
