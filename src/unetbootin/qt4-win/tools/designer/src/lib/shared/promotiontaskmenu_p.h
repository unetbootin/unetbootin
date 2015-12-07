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

#ifndef PROMOTIONTASKMENU_H
#define PROMOTIONTASKMENU_H

#include "shared_global_p.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QList>

QT_BEGIN_NAMESPACE

class QDesignerFormWindowInterface;
class QDesignerFormEditorInterface;

class QAction;
class QMenu;
class QWidget;
class QSignalMapper;

namespace qdesigner_internal {

// A helper class for creating promotion context menus and handling promotion actions.

class QDESIGNER_SHARED_EXPORT PromotionTaskMenu: public QObject
{
    Q_OBJECT
public:
    enum Mode { ModeSingleWidget, ModeMultiSelection };

    explicit PromotionTaskMenu(QWidget *widget,Mode mode = ModeMultiSelection, QObject *parent = 0);

    void setWidget(QWidget *widget);

    // Set menu labels
    void setPromoteLabel(const QString &promoteLabel);
    void setEditPromoteToLabel(const QString &promoteEditLabel);
    // Defaults to "Demote to %1".arg(class).
    void setDemoteLabel(const QString &demoteLabel);

    typedef QList<QAction*> ActionList;

    enum AddFlags { LeadingSeparator = 1, TrailingSeparator = 2, SuppressGlobalEdit = 4};

    // Adds a list of promotion actions according to the current promotion state of the widget.
    void addActions(QDesignerFormWindowInterface *fw, unsigned flags, ActionList &actionList);
    // Convenience that finds the form window.
    void addActions(unsigned flags, ActionList &actionList);

    void addActions(QDesignerFormWindowInterface *fw, unsigned flags, QMenu *menu);
    void addActions(unsigned flags, QMenu *menu);

    // Pop up the editor in a global context.
    static void editPromotedWidgets(QDesignerFormEditorInterface *core, QWidget* parent);

private slots:
    void slotPromoteToCustomWidget(const QString &customClassName);
    void slotDemoteFromCustomWidget();
    void slotEditPromotedWidgets();
    void slotEditPromoteTo();
    void slotEditSignalsSlots();

private:
    void promoteTo(QDesignerFormWindowInterface *fw, const QString &customClassName);

    enum PromotionState { NotApplicable, NoHomogenousSelection, CanPromote, CanDemote };
    PromotionState createPromotionActions(QDesignerFormWindowInterface *formWindow);
    QDesignerFormWindowInterface *formWindow() const;

    typedef QList<QPointer<QWidget> > PromotionSelectionList;
    PromotionSelectionList promotionSelectionList(QDesignerFormWindowInterface *formWindow) const;

    const Mode m_mode;

    QPointer<QWidget> m_widget;

    QSignalMapper *m_promotionMapper;
    // Per-Widget actions
    QList<QAction *> m_promotionActions;

    QAction *m_globalEditAction;
    QAction *m_EditPromoteToAction;
    QAction *m_EditSignalsSlotsAction;

    QString m_promoteLabel;
    QString m_demoteLabel;
};

} // namespace qdesigner_internal

QT_END_NAMESPACE

#endif // PROMOTIONTASKMENU_H
