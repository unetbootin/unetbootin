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

#ifndef QGRAPHICSSCENEEVENT_H
#define QGRAPHICSSCENEEVENT_H

#include <QtCore/qcoreevent.h>
#include <QtCore/qpoint.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

#if !defined(QT_NO_GRAPHICSVIEW) || (QT_EDITION & QT_MODULE_GRAPHICSVIEW) != QT_MODULE_GRAPHICSVIEW

class QMimeData;
class QPointF;
class QSizeF;
class QWidget;

class QGraphicsSceneEventPrivate;
class Q_GUI_EXPORT QGraphicsSceneEvent : public QEvent
{
public:
    QGraphicsSceneEvent(Type type);
    ~QGraphicsSceneEvent();

    QWidget *widget() const;
    void setWidget(QWidget *widget);

protected:
    QGraphicsSceneEvent(QGraphicsSceneEventPrivate &dd, Type type = None);
    QGraphicsSceneEventPrivate *d_ptr;
    Q_DECLARE_PRIVATE(QGraphicsSceneEvent)
};

class QGraphicsSceneMouseEventPrivate;
class Q_GUI_EXPORT QGraphicsSceneMouseEvent : public QGraphicsSceneEvent
{
public:
    QGraphicsSceneMouseEvent(Type type = None);
    ~QGraphicsSceneMouseEvent();

    QPointF pos() const;
    void setPos(const QPointF &pos);

    QPointF scenePos() const;
    void setScenePos(const QPointF &pos);

    QPoint screenPos() const;
    void setScreenPos(const QPoint &pos);

    QPointF buttonDownPos(Qt::MouseButton button) const;
    void setButtonDownPos(Qt::MouseButton button, const QPointF &pos);

    QPointF buttonDownScenePos(Qt::MouseButton button) const;
    void setButtonDownScenePos(Qt::MouseButton button, const QPointF &pos);

    QPoint buttonDownScreenPos(Qt::MouseButton button) const;
    void setButtonDownScreenPos(Qt::MouseButton button, const QPoint &pos);

    QPointF lastPos() const;
    void setLastPos(const QPointF &pos);

    QPointF lastScenePos() const;
    void setLastScenePos(const QPointF &pos);

    QPoint lastScreenPos() const;
    void setLastScreenPos(const QPoint &pos);

    Qt::MouseButtons buttons() const;
    void setButtons(Qt::MouseButtons buttons);

    Qt::MouseButton button() const;
    void setButton(Qt::MouseButton button);

    Qt::KeyboardModifiers modifiers() const;
    void setModifiers(Qt::KeyboardModifiers modifiers);

private:
    Q_DECLARE_PRIVATE(QGraphicsSceneMouseEvent)
};

class QGraphicsSceneWheelEventPrivate;
class Q_GUI_EXPORT QGraphicsSceneWheelEvent : public QGraphicsSceneEvent
{
public:
    QGraphicsSceneWheelEvent(Type type = None);
    ~QGraphicsSceneWheelEvent();

    QPointF pos() const;
    void setPos(const QPointF &pos);

    QPointF scenePos() const;
    void setScenePos(const QPointF &pos);

    QPoint screenPos() const;
    void setScreenPos(const QPoint &pos);

    Qt::MouseButtons buttons() const;
    void setButtons(Qt::MouseButtons buttons);

    Qt::KeyboardModifiers modifiers() const;
    void setModifiers(Qt::KeyboardModifiers modifiers);

    int delta() const;
    void setDelta(int delta);

    Qt::Orientation orientation() const;
    void setOrientation(Qt::Orientation orientation);

private:
    Q_DECLARE_PRIVATE(QGraphicsSceneWheelEvent)
};

class QGraphicsSceneContextMenuEventPrivate;
class Q_GUI_EXPORT QGraphicsSceneContextMenuEvent : public QGraphicsSceneEvent
{
public:
    enum Reason { Mouse, Keyboard, Other };

    QGraphicsSceneContextMenuEvent(Type type = None);
    ~QGraphicsSceneContextMenuEvent();

    QPointF pos() const;
    void setPos(const QPointF &pos);

    QPointF scenePos() const;
    void setScenePos(const QPointF &pos);

    QPoint screenPos() const;
    void setScreenPos(const QPoint &pos);

    Qt::KeyboardModifiers modifiers() const;
    void setModifiers(Qt::KeyboardModifiers modifiers);

    Reason reason() const;
    void setReason(Reason reason);

private:
    Q_DECLARE_PRIVATE(QGraphicsSceneContextMenuEvent)
};

class QGraphicsSceneHoverEventPrivate;
class Q_GUI_EXPORT QGraphicsSceneHoverEvent : public QGraphicsSceneEvent
{
public:
    QGraphicsSceneHoverEvent(Type type = None);
    ~QGraphicsSceneHoverEvent();

    QPointF pos() const;
    void setPos(const QPointF &pos);

    QPointF scenePos() const;
    void setScenePos(const QPointF &pos);

    QPoint screenPos() const;
    void setScreenPos(const QPoint &pos);

    QPointF lastPos() const;
    void setLastPos(const QPointF &pos);

    QPointF lastScenePos() const;
    void setLastScenePos(const QPointF &pos);

    QPoint lastScreenPos() const;
    void setLastScreenPos(const QPoint &pos);

    Qt::KeyboardModifiers modifiers() const;
    void setModifiers(Qt::KeyboardModifiers modifiers);

private:
    Q_DECLARE_PRIVATE(QGraphicsSceneHoverEvent)
};

class QGraphicsSceneHelpEventPrivate;
class Q_GUI_EXPORT QGraphicsSceneHelpEvent : public QGraphicsSceneEvent
{
public:
    QGraphicsSceneHelpEvent(Type type = None);
    ~QGraphicsSceneHelpEvent();

    QPointF scenePos() const;
    void setScenePos(const QPointF &pos);

    QPoint screenPos() const;
    void setScreenPos(const QPoint &pos);

private:
    Q_DECLARE_PRIVATE(QGraphicsSceneHelpEvent)
};

class QGraphicsSceneDragDropEventPrivate;
class Q_GUI_EXPORT QGraphicsSceneDragDropEvent : public QGraphicsSceneEvent
{
public:
    QGraphicsSceneDragDropEvent(Type type = None);
    ~QGraphicsSceneDragDropEvent();

    QPointF pos() const;
    void setPos(const QPointF &pos);

    QPointF scenePos() const;
    void setScenePos(const QPointF &pos);

    QPoint screenPos() const;
    void setScreenPos(const QPoint &pos);

    Qt::MouseButtons buttons() const;
    void setButtons(Qt::MouseButtons buttons);

    Qt::KeyboardModifiers modifiers() const;
    void setModifiers(Qt::KeyboardModifiers modifiers);

    Qt::DropActions possibleActions() const;
    void setPossibleActions(Qt::DropActions actions);

    Qt::DropAction proposedAction() const;
    void setProposedAction(Qt::DropAction action);
    void acceptProposedAction();

    Qt::DropAction dropAction() const;
    void setDropAction(Qt::DropAction action);

    QWidget *source() const;
    void setSource(QWidget *source);

    const QMimeData *mimeData() const;
    void setMimeData(const QMimeData *data);

private:
    Q_DECLARE_PRIVATE(QGraphicsSceneDragDropEvent)
};

class QGraphicsSceneResizeEventPrivate;
class Q_GUI_EXPORT QGraphicsSceneResizeEvent : public QGraphicsSceneEvent
{
    Q_DECLARE_PRIVATE(QGraphicsSceneResizeEvent)
public:
    QGraphicsSceneResizeEvent();
    ~QGraphicsSceneResizeEvent();

    QSizeF oldSize() const;
    void setOldSize(const QSizeF &size);

    QSizeF newSize() const;
    void setNewSize(const QSizeF &size);
};

class QGraphicsSceneMoveEventPrivate;
class Q_GUI_EXPORT QGraphicsSceneMoveEvent : public QGraphicsSceneEvent
{
    Q_DECLARE_PRIVATE(QGraphicsSceneMoveEvent)
public:
    QGraphicsSceneMoveEvent();
    ~QGraphicsSceneMoveEvent();

    QPointF oldPos() const;
    void setOldPos(const QPointF &pos);

    QPointF newPos() const;
    void setNewPos(const QPointF &pos);
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QT_NO_GRAPHICSVIEW
#endif
