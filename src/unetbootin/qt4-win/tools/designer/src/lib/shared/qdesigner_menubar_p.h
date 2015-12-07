/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt Designer of the Qt Toolkit.
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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of Qt Designer.  This header
// file may change from version to version without notice, or even be removed.
//
// We mean it.
//

#ifndef QDESIGNER_MENUBAR_H
#define QDESIGNER_MENUBAR_H

#include "shared_global_p.h"

#include <QtGui/QAction>
#include <QtGui/QMenuBar>

#include <QtCore/QPointer>
#include <QtCore/QMimeData>

QT_BEGIN_NAMESPACE

class QDesignerFormWindowInterface;
class QDesignerActionProviderExtension;

class QLineEdit;
class QMimeData;

namespace qdesigner_internal {
class PromotionTaskMenu;

class SpecialMenuAction: public QAction
{
    Q_OBJECT
public:
    SpecialMenuAction(QObject *parent = 0);
    virtual ~SpecialMenuAction();
};

} // namespace qdesigner_internal

class QDESIGNER_SHARED_EXPORT QDesignerMenuBar: public QMenuBar
{
    Q_OBJECT
public:
    QDesignerMenuBar(QWidget *parent = 0);
    virtual ~QDesignerMenuBar();

    bool eventFilter(QObject *object, QEvent *event);

    QDesignerFormWindowInterface *formWindow() const;
    QDesignerActionProviderExtension *actionProvider();

    void adjustSpecialActions();
    bool interactive(bool i);
    bool dragging() const;

    void moveLeft(bool ctrl = false);
    void moveRight(bool ctrl = false);
    void moveUp();
    void moveDown();

private slots:
    void deleteMenu();
    void slotRemoveMenuBar();

protected:
    virtual void actionEvent(QActionEvent *event);
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dragMoveEvent(QDragMoveEvent *event);
    virtual void dragLeaveEvent(QDragLeaveEvent *event);
    virtual void dropEvent(QDropEvent *event);
    virtual void paintEvent(QPaintEvent *event);
    virtual void focusOutEvent(QFocusEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void keyReleaseEvent(QKeyEvent *event);

    bool handleEvent(QWidget *widget, QEvent *event);
    bool handleMouseDoubleClickEvent(QWidget *widget, QMouseEvent *event);
    bool handleMousePressEvent(QWidget *widget, QMouseEvent *event);
    bool handleMouseReleaseEvent(QWidget *widget, QMouseEvent *event);
    bool handleMouseMoveEvent(QWidget *widget, QMouseEvent *event);
    bool handleContextMenuEvent(QWidget *widget, QContextMenuEvent *event);
    bool handleKeyPressEvent(QWidget *widget, QKeyEvent *event);

    void startDrag(const QPoint &pos);

    enum ActionDragCheck { NoActionDrag, ActionDragOnSubMenu, AcceptActionDrag };
    ActionDragCheck checkAction(QAction *action) const;

    void adjustIndicator(const QPoint &pos);
    int findAction(const QPoint &pos) const;

    QAction *currentAction() const;
    int realActionCount() const;

    enum LeaveEditMode {
        Default = 0,
        ForceAccept
    };

    void enterEditMode();
    void leaveEditMode(LeaveEditMode mode);
    void showLineEdit();

    void showMenu(int index = -1);
    void hideMenu(int index = -1);

    QAction *safeActionAt(int index) const;

    bool swap(int a, int b);

private:
    void updateCurrentAction(bool selectAction);

    QAction *m_addMenu;
    QPointer<QMenu> m_activeMenu;
    QPoint m_startPosition;
    int m_currentIndex;
    bool m_interactive;
    QLineEdit *m_editor;
    bool m_dragging;
    int m_lastMenuActionIndex;
    QPointer<QWidget> m_lastFocusWidget;
    qdesigner_internal::PromotionTaskMenu* m_promotionTaskMenu;
};

QT_END_NAMESPACE

#endif // QDESIGNER_MENUBAR_H
