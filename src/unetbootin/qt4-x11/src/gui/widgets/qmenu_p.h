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

#ifndef QMENU_P_H
#define QMENU_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "QtGui/qmenubar.h"
#include "QtGui/qstyleoption.h"
#include "QtCore/qdatetime.h"
#include "QtCore/qmap.h"
#include "QtCore/qhash.h"
#include "QtCore/qbasictimer.h"
#include "private/qwidget_p.h"

QT_BEGIN_NAMESPACE

#ifndef QT_NO_MENU

class QTornOffMenu;
class QEventLoop;

#ifdef Q_WS_MAC
#  ifdef __OBJC__
QT_END_NAMESPACE
@class NSMenuItem;
QT_BEGIN_NAMESPACE
#  else
typedef void NSMenuItem;
#  endif //__OBJC__
struct QMacMenuAction {
    QMacMenuAction() 
#ifndef QT_MAC_USE_COCOA
       : command(0)
#else
       : menuItem(0)
#endif
         , ignore_accel(0), merged(0), menu(0)
    {
    }
    ~QMacMenuAction();
#ifndef QT_MAC_USE_COCOA
    uint command;
#else
    NSMenuItem *menuItem;
#endif
    uchar ignore_accel : 1;
    uchar merged : 1;
    QPointer<QAction> action;
    OSMenuRef menu;
};

struct QMenuMergeItem
{
#ifndef QT_MAC_USE_COCOA
    inline QMenuMergeItem(MenuCommand c, QMacMenuAction *a) : command(c), action(a) { }
    MenuCommand command;
#else
    inline QMenuMergeItem(NSMenuItem *c, QMacMenuAction *a) : menuItem(c), action(a) { }
    NSMenuItem *menuItem;
#endif
    QMacMenuAction *action;
};
typedef QList<QMenuMergeItem> QMenuMergeList;
#endif

#ifdef Q_OS_WINCE
struct QWceMenuAction {
    uint command;    
    QPointer<QAction> action;
    HMENU menuHandle;
    QWceMenuAction() : menuHandle(0), command(0) {}
};
#endif

class QMenuPrivate : public QWidgetPrivate
{
    Q_DECLARE_PUBLIC(QMenu)
public:
    QMenuPrivate() : itemsDirty(0), maxIconWidth(0), tabWidth(0), ncols(0),
                      collapsibleSeparators(true), hasHadMouse(0), aboutToHide(0), motions(0),
                      currentAction(0), scroll(0), eventLoop(0), tearoff(0), tornoff(0), tearoffHighlighted(0),
                      hasCheckableItems(0), sloppyAction(0)
#ifdef Q_WS_MAC
                      ,mac_menu(0)
#endif
#if defined(Q_OS_WINCE) && !defined(QT_NO_MENUBAR)
                      ,wce_menu(0)
#endif
#ifdef QT3_SUPPORT
                      ,emitHighlighted(false)
#endif
    { }
    ~QMenuPrivate()
    {
        delete scroll;
#ifdef Q_WS_MAC
        delete mac_menu;
#endif
#if defined(Q_OS_WINCE) && !defined(QT_NO_MENUBAR)
        delete wce_menu;
#endif
    }
    void init();

    //item calculations
    mutable uint itemsDirty : 1;
    mutable uint maxIconWidth, tabWidth;
    QRect actionRect(QAction *) const;
    mutable QMap<QAction*, QRect> actionRects;
    mutable QList<QAction*> actionList;
    mutable QHash<QAction *, QWidget *> widgetItems;
    void calcActionRects(QMap<QAction*, QRect> &actionRects, QList<QAction*> &actionList) const;
    void updateActions();
    QRect popupGeometry(int screen=-1) const;
    QList<QAction *> filterActions(const QList<QAction *> &actions) const;
    uint ncols : 4; //4 bits is probably plenty
    uint collapsibleSeparators : 1;

    uint activationRecursionGuard : 1;

    //selection
    static QPointer<QMenu> mouseDown;
    QPoint mousePopupPos;
    uint hasHadMouse : 1;
    uint aboutToHide : 1;
    int motions;
    QAction *currentAction;
    static QBasicTimer menuDelayTimer;
    enum SelectionReason {
        SelectedFromKeyboard,
        SelectedFromElsewhere
    };
    QAction *actionAt(QPoint p) const;
    void setFirstActionActive();
    void setCurrentAction(QAction *, int popup = -1, SelectionReason reason = SelectedFromElsewhere, bool activateFirst = false);
    void popupAction(QAction *, int, bool);
    void setSyncAction();

    //scrolling support
    struct QMenuScroller {
        enum ScrollLocation { ScrollStay, ScrollBottom, ScrollTop, ScrollCenter };
        enum ScrollDirection { ScrollNone=0, ScrollUp=0x01, ScrollDown=0x02 };
        uint scrollFlags : 2, scrollDirection : 2;
        int scrollOffset;
        QBasicTimer *scrollTimer;

        QMenuScroller() : scrollFlags(ScrollNone), scrollDirection(ScrollNone), scrollOffset(0), scrollTimer(0) { }
        ~QMenuScroller() { delete scrollTimer; }
    } *scroll;
    void scrollMenu(QMenuScroller::ScrollLocation location, bool active=false);
    void scrollMenu(QMenuScroller::ScrollDirection direction, bool page=false, bool active=false);
    void scrollMenu(QAction *action, QMenuScroller::ScrollLocation location, bool active=false);

    //synchronous operation (ie exec())
    QEventLoop *eventLoop;
    QPointer<QAction> syncAction;

    //search buffer
    QString searchBuffer;
    QBasicTimer searchBufferTimer;

    //passing of mouse events up the parent heirarchy
    QPointer<QMenu> activeMenu;
    bool mouseEventTaken(QMouseEvent *);

    //used to walk up the popup list
    struct QMenuCaused {
        QPointer<QWidget> widget;
        QPointer<QAction> action;
    };
    virtual QList<QPointer<QWidget> > calcCausedStack() const;
    QMenuCaused causedPopup;
    void hideUpToMenuBar();
    void hideMenu(QMenu *menu);

    //index mappings
    inline QAction *actionAt(int i) const { return q_func()->actions().at(i); }
    inline int indexOf(QAction *act) const { return q_func()->actions().indexOf(act); }

    //tear off support
    uint tearoff : 1, tornoff : 1, tearoffHighlighted : 1;
    QPointer<QTornOffMenu> tornPopup;

    mutable bool hasCheckableItems;

    //sloppy selection
    static QBasicTimer sloppyDelayTimer;
    QAction *sloppyAction;
    QRegion sloppyRegion;

    //default action
    QPointer<QAction> defaultAction;

    QAction *menuAction;
    QAction *defaultMenuAction;

    void setOverrideMenuAction(QAction *);
    void _q_overrideMenuActionDestroyed();

    //firing of events
    void activateAction(QAction *, QAction::ActionEvent, bool self=true);
    void activateCausedStack(const QList<QPointer<QWidget> > &, QAction *, QAction::ActionEvent, bool);

    void _q_actionTriggered();
    void _q_actionHovered();

    bool hasMouseMoved(const QPoint &globalPos);

    //menu fading/scrolling effects
    bool doChildEffects;

#ifdef Q_WS_MAC
    //mac menu binding
    struct QMacMenuPrivate {
        QList<QMacMenuAction*> actionItems;
        OSMenuRef menu;
        QMacMenuPrivate();
        ~QMacMenuPrivate();

        bool merged(const QAction *action) const;
        void addAction(QAction *, QMacMenuAction* =0, QMenuPrivate *qmenu = 0);
        void addAction(QMacMenuAction *, QMacMenuAction* =0, QMenuPrivate *qmenu = 0);
        void syncAction(QMacMenuAction *);
        inline void syncAction(QAction *a) { syncAction(findAction(a)); }
        void removeAction(QMacMenuAction *);
        inline void removeAction(QAction *a) { removeAction(findAction(a)); }
        inline QMacMenuAction *findAction(QAction *a) {
            for(int i = 0; i < actionItems.size(); i++) {
                QMacMenuAction *act = actionItems[i];
                if(a == act->action)
                    return act;
            }
            return 0;
        }
    } *mac_menu;
    OSMenuRef macMenu(OSMenuRef merge);
    void setMacMenuEnabled(bool enable = true);
    static QHash<OSMenuRef, OSMenuRef> mergeMenuHash;
    static QHash<OSMenuRef, QMenuMergeList*> mergeMenuItemsHash;
#endif

    QPointer<QAction> actionAboutToTrigger;
#ifdef QT3_SUPPORT
    bool emitHighlighted;
#endif

#if defined(Q_OS_WINCE) && !defined(QT_NO_MENUBAR)
    struct QWceMenuPrivate {
        QList<QWceMenuAction*> actionItems;
        HMENU menuHandle;
        QWceMenuPrivate();
        ~QWceMenuPrivate();        
        void addAction(QAction *, QWceMenuAction* =0);
        void addAction(QWceMenuAction *, QWceMenuAction* =0);
        void syncAction(QWceMenuAction *);
        inline void syncAction(QAction *a) { syncAction(findAction(a)); }
        void removeAction(QWceMenuAction *);
        void rebuild(bool reCreate = false);
        inline void removeAction(QAction *a) { removeAction(findAction(a)); }
        inline QWceMenuAction *findAction(QAction *a) {
            for(int i = 0; i < actionItems.size(); i++) {
                QWceMenuAction *act = actionItems[i];
                if(a == act->action)
                    return act;
            }
            return 0;
        }
    } *wce_menu;
    HMENU wceMenu(bool create = false);
    QAction* wceCommands(uint command);
#endif

    QPointer<QWidget> noReplayFor;
};

#endif // QT_NO_MENU

QT_END_NAMESPACE

#endif // QMENU_P_H
