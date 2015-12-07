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

#ifndef HELPVIEWER_H
#define HELPVIEWER_H

#include <QtCore/QUrl>
#include <QtCore/QVariant>
#include <QtGui/QTextBrowser>
#include <QtGui/QAction>

QT_BEGIN_NAMESPACE

class QHelpEngine;
class CentralWidget;

class QPoint;
class QString;
class QKeyEvent;
class QMouseEvent;
class QContextMenuEvent;

#if defined(USE_WEBKIT)

#include <QWebView>

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
    void zoomIn(int range = 1);
    void zoomOut(int range = 1);

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

private Q_SLOTS:
    void actionChanged();

private:
    QHelpEngine *helpEngine;
    CentralWidget* parentWidget;
    QUrl homeUrl;
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

    inline bool hasSelection() const
    { return textCursor().hasSelection(); }

private:
    QVariant loadResource(int type, const QUrl &name);    
    void openLinkInNewTab(const QString &link);
    bool hasAnchorAt(const QPoint& pos);
    void contextMenuEvent(QContextMenuEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

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
