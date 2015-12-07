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

#ifndef FORMWINDOWMANAGER_H
#define FORMWINDOWMANAGER_H

#include "formeditor_global.h"

#include <QtDesigner/QDesignerFormWindowManagerInterface>

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QPointer>
#include <QtCore/QMap>

QT_BEGIN_NAMESPACE

class QAction;
class QActionGroup;
class QUndoGroup;
class QDesignerFormEditorInterface;
class QDesignerWidgetBoxInterface;

namespace qdesigner_internal {
class FormWindow;

class QT_FORMEDITOR_EXPORT FormWindowManager: public QDesignerFormWindowManagerInterface
{
    Q_OBJECT
public:
    explicit FormWindowManager(QDesignerFormEditorInterface *core, QObject *parent = 0);
    virtual ~FormWindowManager();

    virtual QDesignerFormEditorInterface *core() const;

    inline QAction *actionCut() const { return m_actionCut; }
    inline QAction *actionCopy() const { return m_actionCopy; }
    inline QAction *actionPaste() const { return m_actionPaste; }
    inline QAction *actionDelete() const { return m_actionDelete; }
    inline QAction *actionSelectAll() const { return m_actionSelectAll; }
    inline QAction *actionLower() const { return m_actionLower; }
    inline QAction *actionRaise() const { return m_actionRaise; }
    QAction *actionUndo() const;
    QAction *actionRedo() const;

    inline QAction *actionHorizontalLayout() const { return m_actionHorizontalLayout; }
    inline QAction *actionVerticalLayout() const { return m_actionVerticalLayout; }
    inline QAction *actionSplitHorizontal() const { return m_actionSplitHorizontal; }
    inline QAction *actionSplitVertical() const { return m_actionSplitVertical; }
    inline QAction *actionGridLayout() const { return m_actionGridLayout; }
    inline QAction *actionBreakLayout() const { return m_actionBreakLayout; }
    inline QAction *actionAdjustSize() const { return m_actionAdjustSize; }

    QDesignerFormWindowInterface *activeFormWindow() const;

    int formWindowCount() const;
    QDesignerFormWindowInterface *formWindow(int index) const;

    QDesignerFormWindowInterface *createFormWindow(QWidget *parentWidget = 0, Qt::WindowFlags flags = 0);

    bool eventFilter(QObject *o, QEvent *e);

    void dragItems(const QList<QDesignerDnDItemInterface*> &item_list);

    QUndoGroup *undoGroup() const;

public slots:
    void addFormWindow(QDesignerFormWindowInterface *formWindow);
    void removeFormWindow(QDesignerFormWindowInterface *formWindow);
    void setActiveFormWindow(QDesignerFormWindowInterface *formWindow);

private slots:
    void slotActionCutActivated();
    void slotActionCopyActivated();
    void slotActionPasteActivated();
    void slotActionDeleteActivated();
    void slotActionSelectAllActivated();
    void slotActionLowerActivated();
    void slotActionRaiseActivated();
    void slotActionHorizontalLayoutActivated();
    void slotActionVerticalLayoutActivated();
    void slotActionSplitHorizontalActivated();
    void slotActionSplitVerticalActivated();
    void slotActionFormLayoutActivated();
    void slotActionGridLayoutActivated();
    void slotActionBreakLayoutActivated();
    void slotActionAdjustSizeActivated();
    void slotActionSimplifyLayoutActivated();

    void slotUpdateActions();

private:
    void setupActions();
    FormWindow *findFormWindow(QWidget *w);
    QWidget *findManagedWidget(FormWindow *fw, QWidget *w);

    void layoutContainerHorizontal();
    void layoutContainerVertical();
    void layoutContainerFormLayout();
    void layoutContainerGrid();

    void setCurrentUndoStack(QUndoStack *stack);

private:
    QDesignerFormEditorInterface *m_core;
    FormWindow *m_activeFormWindow;
    QList<FormWindow*> m_formWindows;

    bool m_layoutChilds;

    // edit actions
    QAction *m_actionCut;
    QAction *m_actionCopy;
    QAction *m_actionPaste;
    QAction *m_actionSelectAll;
    QAction *m_actionDelete;
    QAction *m_actionLower;
    QAction *m_actionRaise;
    // layout actions
    QAction *m_actionHorizontalLayout;
    QAction *m_actionVerticalLayout;
    QAction *m_actionSplitHorizontal;
    QAction *m_actionSplitVertical;
    QAction *m_actionGridLayout;
    QAction *m_actionBreakLayout;
    QAction *m_actionAdjustSize;

    QAction *m_actionUndo;
    QAction *m_actionRedo;

    QMap<QWidget *, bool> getUnsortedLayoutsToBeBroken(bool firstOnly) const;
    bool hasLayoutsToBeBroken() const;
    QList<QWidget *> layoutsToBeBroken(QWidget *w) const;
    QList<QWidget *> layoutsToBeBroken() const;

    QUndoGroup *m_undoGroup;
};

}  // namespace qdesigner_internal

QT_END_NAMESPACE

#endif // FORMWINDOWMANAGER_H
