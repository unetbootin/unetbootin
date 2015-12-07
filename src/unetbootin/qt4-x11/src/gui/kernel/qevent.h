/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#ifndef QEVENT_H
#define QEVENT_H

#include <QtGui/qwindowdefs.h>
#include <QtCore/qobject.h>
#include <QtGui/qregion.h>
#include <QtCore/qnamespace.h>
#include <QtCore/qstring.h>
#include <QtGui/qkeysequence.h>
#include <QtCore/qcoreevent.h>
#include <QtGui/qmime.h>
#include <QtGui/qdrag.h>
#include <QtCore/qvariant.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

class QAction;

class Q_GUI_EXPORT QInputEvent : public QEvent
{
public:
    QInputEvent(Type type, Qt::KeyboardModifiers modifiers = Qt::NoModifier);
    ~QInputEvent();
    inline Qt::KeyboardModifiers modifiers() const { return modState; }
protected:
    Qt::KeyboardModifiers modState;
};

class Q_GUI_EXPORT QMouseEvent : public QInputEvent
{
public:
    QMouseEvent(Type type, const QPoint &pos, Qt::MouseButton button,
                Qt::MouseButtons buttons, Qt::KeyboardModifiers modifiers);
    QMouseEvent(Type type, const QPoint &pos, const QPoint &globalPos,
                Qt::MouseButton button, Qt::MouseButtons buttons,
                Qt::KeyboardModifiers modifiers);
    ~QMouseEvent();

    inline const QPoint &pos() const { return p; }
    inline const QPoint &globalPos() const { return g; }
    inline int x() const { return p.x(); }
    inline int y() const { return p.y(); }
    inline int globalX() const { return g.x(); }
    inline int globalY() const { return g.y(); }
    inline Qt::MouseButton button() const { return b; }
    inline Qt::MouseButtons buttons() const { return mouseState; }

    static QMouseEvent *createExtendedMouseEvent(Type type, const QPointF &pos,
                                                 const QPoint &globalPos, Qt::MouseButton button,
                                                 Qt::MouseButtons buttons, Qt::KeyboardModifiers modifiers);
    inline bool hasExtendedInfo() const { return reinterpret_cast<const QMouseEvent *>(d) == this; }
    QPointF posF() const;

#ifdef QT3_SUPPORT
    QT3_SUPPORT_CONSTRUCTOR QMouseEvent(Type type, const QPoint &pos, Qt::ButtonState button, int state);
    QT3_SUPPORT_CONSTRUCTOR QMouseEvent(Type type, const QPoint &pos, const QPoint &globalPos,
                                      Qt::ButtonState button, int state);
    inline QT3_SUPPORT Qt::ButtonState state() const
    { return Qt::ButtonState((mouseState^b)|int(modifiers())); }
    inline QT3_SUPPORT Qt::ButtonState stateAfter() const
    { return Qt::ButtonState(int(mouseState)|int(modifiers())); }
#endif
protected:
    QPoint p, g;
    Qt::MouseButton b;
    Qt::MouseButtons mouseState;
};

class Q_GUI_EXPORT QHoverEvent : public QEvent
{
public:
    QHoverEvent(Type type, const QPoint &pos, const QPoint &oldPos);
    ~QHoverEvent();

    inline const QPoint &pos() const { return p; }
    inline const QPoint &oldPos() const { return op; }

protected:
    QPoint p, op;
};

#ifndef QT_NO_WHEELEVENT
class Q_GUI_EXPORT QWheelEvent : public QInputEvent
{
public:
    QWheelEvent(const QPoint &pos, int delta,
                Qt::MouseButtons buttons, Qt::KeyboardModifiers modifiers,
                Qt::Orientation orient = Qt::Vertical);
    QWheelEvent(const QPoint &pos, const QPoint& globalPos, int delta,
                Qt::MouseButtons buttons, Qt::KeyboardModifiers modifiers,
                Qt::Orientation orient = Qt::Vertical);
    ~QWheelEvent();

    inline int delta() const { return d; }
    inline const QPoint &pos() const { return p; }
    inline const QPoint &globalPos()   const { return g; }
    inline int x() const { return p.x(); }
    inline int y() const { return p.y(); }
    inline int globalX() const { return g.x(); }
    inline int globalY() const { return g.y(); }

    inline Qt::MouseButtons buttons() const { return mouseState; }
    Qt::Orientation orientation() const { return o; }

#ifdef QT3_SUPPORT
    QT3_SUPPORT_CONSTRUCTOR QWheelEvent(const QPoint &pos, int delta, int state,
                                      Qt::Orientation orient = Qt::Vertical);
    QT3_SUPPORT_CONSTRUCTOR QWheelEvent(const QPoint &pos, const QPoint& globalPos, int delta, int state,
                                      Qt::Orientation orient = Qt::Vertical);
    inline QT3_SUPPORT Qt::ButtonState state() const
    { return static_cast<Qt::ButtonState>(int(buttons())|int(modifiers())); }
#endif
protected:
    QPoint p;
    QPoint g;
    int d;
    Qt::MouseButtons mouseState;
    Qt::Orientation o;
};
#endif

#ifndef QT_NO_TABLETEVENT
class Q_GUI_EXPORT QTabletEvent : public QInputEvent
{
public:
    enum TabletDevice { NoDevice, Puck, Stylus, Airbrush, FourDMouse,
                        XFreeEraser /*internal*/, RotationStylus };
    enum PointerType { UnknownPointer, Pen, Cursor, Eraser };
    QTabletEvent(Type t, const QPoint &pos, const QPoint &globalPos, const QPointF &hiResGlobalPos,
                 int device, int pointerType, qreal pressure, int xTilt, int yTilt,
                 qreal tangentialPressure, qreal rotation, int z,
                 Qt::KeyboardModifiers keyState, qint64 uniqueID);
    ~QTabletEvent();

    inline const QPoint &pos() const { return mPos; }
    inline const QPoint &globalPos() const { return mGPos; }
    inline const QPointF &hiResGlobalPos() const { return mHiResGlobalPos; }
    inline int x() const { return mPos.x(); }
    inline int y() const { return mPos.y(); }
    inline int globalX() const { return mGPos.x(); }
    inline int globalY() const { return mGPos.y(); }
    inline qreal hiResGlobalX() const { return mHiResGlobalPos.x(); }
    inline qreal hiResGlobalY() const { return mHiResGlobalPos.y(); }
    inline TabletDevice device() const { return TabletDevice(mDev); }
    inline PointerType pointerType() const { return PointerType(mPointerType); }
    inline qint64 uniqueId() const { return mUnique; }
    inline qreal pressure() const { return mPress; }
    inline int z() const { return mZ; }
    inline qreal tangentialPressure() const { return mTangential; }
    inline qreal rotation() const { return mRot; }
    inline int xTilt() const { return mXT; }
    inline int yTilt() const { return mYT; }

protected:
    QPoint mPos, mGPos;
    QPointF mHiResGlobalPos;
    int mDev, mPointerType, mXT, mYT, mZ;
    qreal mPress, mTangential, mRot;
    qint64 mUnique;

    // I don't know what the future holds for tablets but there could be some
    // new devices coming along, and there seem to be "holes" in the
    // OS-specific events for this.
    void *mExtra;
};
#endif // QT_NO_TABLETEVENT

class Q_GUI_EXPORT QKeyEvent : public QInputEvent
{
public:
    QKeyEvent(Type type, int key, Qt::KeyboardModifiers modifiers, const QString& text = QString(),
              bool autorep = false, ushort count = 1);
    ~QKeyEvent();

    int key() const { return k; }
#ifndef QT_NO_SHORTCUT
    bool matches(QKeySequence::StandardKey key) const;
#endif
    Qt::KeyboardModifiers modifiers() const;
    inline QString text() const { return txt; }
    inline bool isAutoRepeat() const { return autor; }
    inline int count() const { return int(c); }

    // Functions for the extended key event information
    static QKeyEvent *createExtendedKeyEvent(Type type, int key, Qt::KeyboardModifiers modifiers,
                                             quint32 nativeScanCode, quint32 nativeVirtualKey,
                                             quint32 nativeModifiers,
                                             const QString& text = QString(), bool autorep = false,
                                             ushort count = 1);
    inline bool hasExtendedInfo() const { return reinterpret_cast<const QKeyEvent*>(d) == this; }
    quint32 nativeScanCode() const;
    quint32 nativeVirtualKey() const;
    quint32 nativeModifiers() const;

#ifdef QT3_SUPPORT
    inline QT3_SUPPORT_CONSTRUCTOR QKeyEvent(Type type, int key, int /*ascii*/,
                                           int modifiers, const QString& text = QString(),
                                           bool autorep = false, ushort count = 1)
        : QInputEvent(type, (Qt::KeyboardModifiers)(modifiers & (int)Qt::KeyButtonMask)), txt(text), k(key),
          c(count), autor(autorep)
    {
        if (key >= Qt::Key_Back && key <= Qt::Key_MediaLast)
            ignore();
    }
    inline QT3_SUPPORT int ascii() const
    { return (txt.length() ? txt.unicode()->toLatin1() : 0); }
    inline QT3_SUPPORT Qt::ButtonState state() const { return Qt::ButtonState(QInputEvent::modifiers()); }
    inline QT3_SUPPORT Qt::ButtonState stateAfter() const { return Qt::ButtonState(modifiers()); }
#endif

protected:
    QString txt;
    int k;
    ushort c;
    uint autor:1;
};


class Q_GUI_EXPORT QFocusEvent : public QEvent
{
public:
    QFocusEvent(Type type, Qt::FocusReason reason=Qt::OtherFocusReason);
    ~QFocusEvent();

    inline bool gotFocus() const { return type() == FocusIn; }
    inline bool lostFocus() const { return type() == FocusOut; }

#ifdef QT3_SUPPORT
    enum Reason { Mouse=Qt::MouseFocusReason, Tab=Qt::TabFocusReason,
                  Backtab=Qt::BacktabFocusReason, MenuBar=Qt::MenuBarFocusReason,
                  ActiveWindow=Qt::ActiveWindowFocusReason, Other=Qt::OtherFocusReason,
                  Popup=Qt::PopupFocusReason, Shortcut=Qt::ShortcutFocusReason };
#endif
    Qt::FocusReason reason();
    Qt::FocusReason reason() const;

private:
    Qt::FocusReason m_reason;
};


class Q_GUI_EXPORT QPaintEvent : public QEvent
{
public:
    QPaintEvent(const QRegion& paintRegion);
    QPaintEvent(const QRect &paintRect);
    ~QPaintEvent();

    inline const QRect &rect() const { return m_rect; }
    inline const QRegion &region() const { return m_region; }

#ifdef QT3_SUPPORT
    QT3_SUPPORT_CONSTRUCTOR QPaintEvent(const QRegion &paintRegion, const QRect &paintRect);
    inline QT3_SUPPORT bool erased() const { return m_erased; }
    inline QT3_SUPPORT void setErased(bool b) { m_erased = b; }
#endif

protected:
    friend class QApplication;
    friend class QCoreApplication;
    QRect m_rect;
    QRegion m_region;
    bool m_erased;
};

class QUpdateLaterEvent : public QEvent
{
public:
    QUpdateLaterEvent(const QRegion& paintRegion);
    ~QUpdateLaterEvent();

    inline const QRegion &region() const { return m_region; }

protected:
    QRegion m_region;
};

class Q_GUI_EXPORT QMoveEvent : public QEvent
{
public:
    QMoveEvent(const QPoint &pos, const QPoint &oldPos);
    ~QMoveEvent();

    inline const QPoint &pos() const { return p; }
    inline const QPoint &oldPos() const { return oldp;}
protected:
    QPoint p, oldp;
    friend class QApplication;
    friend class QCoreApplication;
};


class Q_GUI_EXPORT QResizeEvent : public QEvent
{
public:
    QResizeEvent(const QSize &size, const QSize &oldSize);
    ~QResizeEvent();

    inline const QSize &size() const { return s; }
    inline const QSize &oldSize()const { return olds;}
protected:
    QSize s, olds;
    friend class QApplication;
    friend class QCoreApplication;
};


class Q_GUI_EXPORT QCloseEvent : public QEvent
{
public:
    QCloseEvent();
    ~QCloseEvent();
};


class Q_GUI_EXPORT QIconDragEvent : public QEvent
{
public:
    QIconDragEvent();
    ~QIconDragEvent();
};


class Q_GUI_EXPORT QShowEvent : public QEvent
{
public:
    QShowEvent();
    ~QShowEvent();
};


class Q_GUI_EXPORT QHideEvent : public QEvent
{
public:
    QHideEvent();
    ~QHideEvent();
};

#ifndef QT_NO_CONTEXTMENU
class Q_GUI_EXPORT QContextMenuEvent : public QInputEvent
{
public:
    enum Reason { Mouse, Keyboard, Other };

    QContextMenuEvent(Reason reason, const QPoint &pos, const QPoint &globalPos,
                      Qt::KeyboardModifiers modifiers);
    QContextMenuEvent(Reason reason, const QPoint &pos, const QPoint &globalPos);
    QContextMenuEvent(Reason reason, const QPoint &pos);
    ~QContextMenuEvent();

    inline int x() const { return p.x(); }
    inline int y() const { return p.y(); }
    inline int globalX() const { return gp.x(); }
    inline int globalY() const { return gp.y(); }

    inline const QPoint& pos() const { return p; }
    inline const QPoint& globalPos() const { return gp; }

    inline Reason reason() const { return Reason(reas); }

#ifdef QT3_SUPPORT
    QT3_SUPPORT_CONSTRUCTOR QContextMenuEvent(Reason reason, const QPoint &pos, const QPoint &globalPos, int);
    QT3_SUPPORT_CONSTRUCTOR QContextMenuEvent(Reason reason, const QPoint &pos, int);

    QT3_SUPPORT Qt::ButtonState state() const;
#endif
protected:
    QPoint p;
    QPoint gp;
    uint reas : 8;
};
#endif // QT_NO_CONTEXTMENU

#ifndef QT_NO_INPUTMETHOD
class Q_GUI_EXPORT QInputMethodEvent : public QEvent
{
public:
    enum AttributeType {
       TextFormat,
       Cursor,
       Language,
       Ruby
    };
    class Attribute {
    public:
        Attribute(AttributeType t, int s, int l, QVariant val) : type(t), start(s), length(l), value(val) {}
        AttributeType type;

        int start;
        int length;
        QVariant value;
    };
    QInputMethodEvent();
    QInputMethodEvent(const QString &preeditText, const QList<Attribute> &attributes);
    void setCommitString(const QString &commitString, int replaceFrom = 0, int replaceLength = 0);

    inline const QList<Attribute> &attributes() const { return attrs; }
    inline const QString &preeditString() const { return preedit; }

    inline const QString &commitString() const { return commit; }
    inline int replacementStart() const { return replace_from; }
    inline int replacementLength() const { return replace_length; }

    QInputMethodEvent(const QInputMethodEvent &other);

private:
    QString preedit;
    QList<Attribute> attrs;
    QString commit;
    int replace_from;
    int replace_length;
};
#endif // QT_NO_INPUTMETHOD

#ifndef QT_NO_DRAGANDDROP

class QMimeData;

class Q_GUI_EXPORT QDropEvent : public QEvent
// QT3_SUPPORT
                              , public QMimeSource
// END QT3_SUPPORT
{
public:
    QDropEvent(const QPoint& pos, Qt::DropActions actions, const QMimeData *data,
               Qt::MouseButtons buttons, Qt::KeyboardModifiers modifiers, Type type = Drop);
    ~QDropEvent();

    inline const QPoint &pos() const { return p; }
    inline Qt::MouseButtons mouseButtons() const { return mouseState; }
    inline Qt::KeyboardModifiers keyboardModifiers() const { return modState; }

    inline Qt::DropActions possibleActions() const { return act; }
    inline Qt::DropAction proposedAction() const { return default_action; }
    inline void acceptProposedAction() { drop_action = default_action; accept(); }

    inline Qt::DropAction dropAction() const { return drop_action; }
    void setDropAction(Qt::DropAction action);

    QWidget* source() const;
    inline const QMimeData *mimeData() const { return mdata; }

// QT3_SUPPORT
    const char* format(int n = 0) const;
    QByteArray encodedData(const char*) const;
    bool provides(const char*) const;
// END QT3_SUPPORT
#ifdef QT3_SUPPORT
    inline void accept() { QEvent::accept(); }
    inline QT3_SUPPORT void accept(bool y) { setAccepted(y); }
    inline QT3_SUPPORT QByteArray data(const char* f) const { return encodedData(f); }

    enum Action { Copy, Link, Move, Private, UserAction = Private };
    QT3_SUPPORT Action action() const;
    inline QT3_SUPPORT void acceptAction(bool y = true)  { if (y) { drop_action = default_action; accept(); } }
    inline QT3_SUPPORT void setPoint(const QPoint& np) { p = np; }
#endif


protected:
    friend class QApplication;
    QPoint p;
    Qt::MouseButtons mouseState;
    Qt::KeyboardModifiers modState;
    Qt::DropActions act;
    Qt::DropAction drop_action;
    Qt::DropAction default_action;
    const QMimeData *mdata;
    mutable QList<QByteArray> fmts; // only used for QT3_SUPPORT
};


class Q_GUI_EXPORT QDragMoveEvent : public QDropEvent
{
public:
    QDragMoveEvent(const QPoint &pos, Qt::DropActions actions, const QMimeData *data,
                   Qt::MouseButtons buttons, Qt::KeyboardModifiers modifiers, Type type = DragMove);
    ~QDragMoveEvent();

    inline QRect answerRect() const { return rect; }

    inline void accept() { QDropEvent::accept(); }
    inline void ignore() { QDropEvent::ignore(); }

    inline void accept(const QRect & r) { accept(); rect = r; }
    inline void ignore(const QRect & r) { ignore(); rect = r; }

#ifdef QT3_SUPPORT
    inline QT3_SUPPORT void accept(bool y) { setAccepted(y); }
#endif

protected:
    friend class QApplication;
    QRect rect;
};


class Q_GUI_EXPORT QDragEnterEvent : public QDragMoveEvent
{
public:
    QDragEnterEvent(const QPoint &pos, Qt::DropActions actions, const QMimeData *data,
                    Qt::MouseButtons buttons, Qt::KeyboardModifiers modifiers);
    ~QDragEnterEvent();
};


/* An internal class */
class Q_GUI_EXPORT QDragResponseEvent : public QEvent
{
public:
    QDragResponseEvent(bool accepted);
    ~QDragResponseEvent();

    inline bool dragAccepted() const { return a; }
protected:
    bool a;
};


class Q_GUI_EXPORT QDragLeaveEvent : public QEvent
{
public:
    QDragLeaveEvent();
    ~QDragLeaveEvent();
};
#endif // QT_NO_DRAGANDDROP


class Q_GUI_EXPORT QHelpEvent : public QEvent
{
public:
    QHelpEvent(Type type, const QPoint &pos, const QPoint &globalPos);
    ~QHelpEvent();

    inline int x() const { return p.x(); }
    inline int y() const { return p.y(); }
    inline int globalX() const { return gp.x(); }
    inline int globalY() const { return gp.y(); }

    inline const QPoint& pos()  const { return p; }
    inline const QPoint& globalPos() const { return gp; }

private:
    QPoint p;
    QPoint gp;
};

#ifndef QT_NO_STATUSTIP
class Q_GUI_EXPORT QStatusTipEvent : public QEvent
{
public:
    QStatusTipEvent(const QString &tip);
    ~QStatusTipEvent();

    inline QString tip() const { return s; }
private:
    QString s;
};
#endif

#ifndef QT_NO_WHATSTHIS
class Q_GUI_EXPORT QWhatsThisClickedEvent : public QEvent
{
public:
    QWhatsThisClickedEvent(const QString &href);
    ~QWhatsThisClickedEvent();

    inline QString href() const { return s; }
private:
    QString s;
};
#endif

#ifndef QT_NO_ACTION
class Q_GUI_EXPORT QActionEvent : public QEvent
{
    QAction *act, *bef;
public:
    QActionEvent(int type, QAction *action, QAction *before = 0);
    ~QActionEvent();

    inline QAction *action() const { return act; }
    inline QAction *before() const { return bef; }
};
#endif

class Q_GUI_EXPORT QFileOpenEvent : public QEvent
{
public:
    QFileOpenEvent(const QString &file);
    ~QFileOpenEvent();

    inline QString file() const { return f; }
private:
    QString f;
};

#ifndef QT_NO_TOOLBAR
class Q_GUI_EXPORT QToolBarChangeEvent : public QEvent
{
public:
    QToolBarChangeEvent(bool t);
    ~QToolBarChangeEvent();

    inline bool toggle() const { return tog; }
private:
    uint tog : 1;
};
#endif

#ifndef QT_NO_SHORTCUT
class Q_GUI_EXPORT QShortcutEvent : public QEvent
{
public:
    QShortcutEvent(const QKeySequence &key, int id, bool ambiguous = false);
    ~QShortcutEvent();

    inline const QKeySequence &key() { return sequence; }
    inline const QKeySequence &key() const { return sequence; }
    inline int shortcutId() { return sid; }
    inline int shortcutId() const { return sid; }
    inline bool isAmbiguous() { return ambig; }
    inline bool isAmbiguous() const { return ambig; }
protected:
    QKeySequence sequence;
    bool ambig;
    int  sid;
};
#endif

#ifndef QT_NO_CLIPBOARD
class Q_GUI_EXPORT QClipboardEvent : public QEvent
{
public:
    QClipboardEvent(QEventPrivate *data);
    ~QClipboardEvent();

    QEventPrivate *data() { return d; };
};
#endif

class Q_GUI_EXPORT QWindowStateChangeEvent: public QEvent
{
public:
    QWindowStateChangeEvent(Qt::WindowStates aOldState);
    QWindowStateChangeEvent(Qt::WindowStates aOldState, bool isOverride);
    ~QWindowStateChangeEvent();

    inline Qt::WindowStates oldState() const { return ostate; }
    bool isOverride() const;

private:
    Qt::WindowStates ostate;
};

#ifdef QT3_SUPPORT
class QMenuBar;
class Q_GUI_EXPORT QMenubarUpdatedEvent: public QEvent
{
public:
    QMenubarUpdatedEvent(QMenuBar * const menBar);
    inline QMenuBar *menuBar() { return m_menuBar; }
private:
    QMenuBar *m_menuBar;
};
#endif

#ifndef QT_NO_DEBUG_STREAM
Q_GUI_EXPORT QDebug operator<<(QDebug, const QEvent *);
#endif

#ifndef QT_NO_SHORTCUT
inline bool operator==(QKeyEvent *e, QKeySequence::StandardKey key){return (e ? e->matches(key) : false);}
inline bool operator==(QKeySequence::StandardKey key, QKeyEvent *e){return (e ? e->matches(key) : false);}
#endif // QT_NO_SHORTCUT

QT_END_NAMESPACE

QT_END_HEADER

#endif // QEVENT_H
