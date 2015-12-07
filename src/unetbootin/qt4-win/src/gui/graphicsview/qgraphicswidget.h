/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#ifndef QGRAPHICSWIDGET_H
#define QGRAPHICSWIDGET_H

#include <QtGui/qfont.h>
#include <QtGui/qgraphicslayoutitem.h>
#include <QtGui/qgraphicsitem.h>
#include <QtGui/qpalette.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

class QFont;
class QFontMetrics;
class QGraphicsLayout;
class QGraphicsSceneMoveEvent;
class QGraphicsWidgetPrivate;
class QGraphicsSceneResizeEvent;
class QStyle;
class QStyleOption;

#if !defined(QT_NO_GRAPHICSVIEW) || (QT_EDITION & QT_MODULE_GRAPHICSVIEW) != QT_MODULE_GRAPHICSVIEW

class QGraphicsWidgetPrivate;

class Q_GUI_EXPORT QGraphicsWidget : public QObject, public QGraphicsItem, public QGraphicsLayoutItem
{
    Q_OBJECT
    Q_PROPERTY(QPalette palette READ palette WRITE setPalette)
    Q_PROPERTY(QFont font READ font WRITE setFont)
    Q_PROPERTY(Qt::LayoutDirection layoutDirection READ layoutDirection WRITE setLayoutDirection RESET unsetLayoutDirection)
    Q_PROPERTY(QSizeF size READ size WRITE resize)
    Q_PROPERTY(Qt::FocusPolicy focusPolicy READ focusPolicy WRITE setFocusPolicy)
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled)
    Q_PROPERTY(bool visible READ isVisible WRITE setVisible)
    Q_PROPERTY(Qt::WindowFlags windowFlags READ windowFlags WRITE setWindowFlags)
    Q_PROPERTY(QString windowTitle READ windowTitle WRITE setWindowTitle)

public:
    QGraphicsWidget(QGraphicsItem *parent = 0, Qt::WindowFlags wFlags = 0);
    ~QGraphicsWidget();

    QGraphicsLayout *layout() const;
    void setLayout(QGraphicsLayout *layout);
    void adjustSize();

    Qt::LayoutDirection layoutDirection() const;
    void setLayoutDirection(Qt::LayoutDirection direction);
    void unsetLayoutDirection();

    QStyle *style() const;
    void setStyle(QStyle *style);

    QFont font() const;
    void setFont(const QFont &font);

    QPalette palette() const;
    void setPalette(const QPalette &palette);

    void resize(const QSizeF &size);
    inline void resize(qreal w, qreal h) { resize(QSizeF(w, h)); }
    QSizeF size() const;

    void setGeometry(const QRectF &rect);
    inline void setGeometry(qreal x, qreal y, qreal w, qreal h);
    inline QRectF rect() const { return QRectF(QPointF(), size()); }

    void setContentsMargins(qreal left, qreal top, qreal right, qreal bottom);
    void getContentsMargins(qreal *left, qreal *top, qreal *right, qreal *bottom) const;

    void setWindowFrameMargins(qreal left, qreal top, qreal right, qreal bottom);
    void getWindowFrameMargins(qreal *left, qreal *top, qreal *right, qreal *bottom) const;
    void unsetWindowFrameMargins();
    QRectF windowFrameGeometry() const;
    QRectF windowFrameRect() const;

    // Window handling
    Qt::WindowFlags windowFlags() const;
    Qt::WindowType windowType() const;
    void setWindowFlags(Qt::WindowFlags wFlags);
    bool isActiveWindow() const;
    void setWindowTitle(const QString &title);
    QString windowTitle() const;

    // Focus handling
    Qt::FocusPolicy focusPolicy() const;
    void setFocusPolicy(Qt::FocusPolicy policy);
    static void setTabOrder(QGraphicsWidget *first, QGraphicsWidget *second);
    QGraphicsWidget *focusWidget() const;

    void setAttribute(Qt::WidgetAttribute attribute, bool on = true);
    bool testAttribute(Qt::WidgetAttribute attribute) const;

    enum {
        Type = 11
    };
    int type() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
    virtual void paintWindowFrame(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
    QRectF boundingRect() const;
    QPainterPath shape() const;

#if 0
    static void dumpFocusChain(QGraphicsWidget *widget);
#endif

    // ### Qt 5: Disambiguate
#ifdef Q_NO_USING_KEYWORD
    const QObjectList &children() const { return QObject::children(); }
#else
    using QObject::children;
#endif

public Q_SLOTS:
    bool close();

protected:
    virtual void initStyleOption(QStyleOption *option) const;

    QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint = QSizeF()) const;
    void updateGeometry();

    // Notification
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    virtual QVariant propertyChange(const QString &propertyName, const QVariant &value);

    // Scene events
    bool sceneEvent(QEvent *event);
    virtual bool windowFrameEvent(QEvent *e);
    virtual Qt::WindowFrameSection windowFrameSectionAt(const QPointF& pos) const;

    // Base event handlers
    bool event(QEvent *event);
    //virtual void actionEvent(QActionEvent *event);
    virtual void changeEvent(QEvent *event);
    virtual void closeEvent(QCloseEvent *event);
    //void create(WId window = 0, bool initializeWindow = true, bool destroyOldWindow = true);
    //void destroy(bool destroyWindow = true, bool destroySubWindows = true);
    void focusInEvent(QFocusEvent *event);
    virtual bool focusNextPrevChild(bool next);
    void focusOutEvent(QFocusEvent *event);
    virtual void hideEvent(QHideEvent *event);
    //virtual bool macEvent(EventHandlerCallRef caller, EventRef event);
    //virtual int metric(PaintDeviceMetric m ) const;
    virtual void moveEvent(QGraphicsSceneMoveEvent *event);
    virtual void polishEvent();
    //virtual bool qwsEvent(QWSEvent *event);
    //void resetInputContext ();
    virtual void resizeEvent(QGraphicsSceneResizeEvent *event);
    virtual void showEvent(QShowEvent *event);
    //virtual void tabletEvent(QTabletEvent *event);
    //virtual bool winEvent(MSG *message, long *result);
    //virtual bool x11Event(XEvent *event);
    virtual void hoverMoveEvent(QGraphicsSceneHoverEvent *event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    virtual void grabMouseEvent(QEvent *event);
    virtual void ungrabMouseEvent(QEvent *event);
    virtual void grabKeyboardEvent(QEvent *event);
    virtual void ungrabKeyboardEvent(QEvent *event);
    QGraphicsWidget(QGraphicsWidgetPrivate &, QGraphicsItem *parent, QGraphicsScene *, Qt::WindowFlags wFlags = 0);

private:
    Q_DISABLE_COPY(QGraphicsWidget)
    Q_DECLARE_PRIVATE_D(QGraphicsItem::d_ptr, QGraphicsWidget)
    friend class QGraphicsScene;
    friend class QGraphicsScenePrivate;
    friend class QGraphicsView;
    friend class QGraphicsLayout;
    friend class QWidget;
    friend class QApplication;
};

inline void QGraphicsWidget::setGeometry(qreal ax, qreal ay, qreal aw, qreal ah)
{ setGeometry(QRectF(ax, ay, aw, ah)); }

#endif

QT_END_NAMESPACE

QT_END_HEADER

#endif

