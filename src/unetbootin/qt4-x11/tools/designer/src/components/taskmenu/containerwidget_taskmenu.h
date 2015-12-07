/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt Designer of the Qt Toolkit.
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

#ifndef CONTAINERWIDGER_TASKMENU_H
#define CONTAINERWIDGER_TASKMENU_H

#include <qdesigner_taskmenu_p.h>
#include <shared_enums_p.h>

#include <extensionfactory_p.h>

#include <QtCore/QPointer>

QT_BEGIN_NAMESPACE

class QDesignerFormWindowInterface;
class QDesignerFormEditorInterface;
class QDesignerContainerExtension;
class QAction;
class QMdiArea;
class QWorkspace;
class QMenu;
class QWizard;

namespace qdesigner_internal {

class PromotionTaskMenu;

// ContainerWidgetTaskMenu: Task menu for containers with extension

class ContainerWidgetTaskMenu: public QDesignerTaskMenu
{
    Q_OBJECT
public:
    explicit ContainerWidgetTaskMenu(QWidget *widget, ContainerType type, QObject *parent = 0);
    virtual ~ContainerWidgetTaskMenu();

    virtual QAction *preferredEditAction() const;
    virtual QList<QAction*> taskActions() const;

private slots:
    void removeCurrentPage();
    void addPage();
    void addPageAfter();

protected:
    QDesignerContainerExtension *containerExtension() const;
    QList<QAction*> &containerActions() { return m_taskActions; }
    int pageCount() const;

private:
    QDesignerFormWindowInterface *formWindow() const;

private:
    static QString pageMenuText(ContainerType ct, int index, int count);
    bool canDeletePage() const;

    const ContainerType m_type;
    QWidget *m_containerWidget;
    QDesignerFormEditorInterface *m_core;
    PromotionTaskMenu *m_pagePromotionTaskMenu;
    QAction *m_pageMenuAction;
    QMenu *m_pageMenu;
    QList<QAction*> m_taskActions;
    QAction *m_actionDeletePage;
};

// WizardContainerWidgetTaskMenu: Provide next/back since QWizard
// has modes in which the "Back" button is not visible.

class WizardContainerWidgetTaskMenu : public ContainerWidgetTaskMenu {
    Q_OBJECT
public:
    explicit WizardContainerWidgetTaskMenu(QWizard *w, QObject *parent = 0);

    virtual QList<QAction*> taskActions() const;

private:
    QAction *m_nextAction;
    QAction *m_previousAction;
};


// MdiContainerWidgetTaskMenu: Provide tile/cascade for MDI containers in addition

class MdiContainerWidgetTaskMenu : public ContainerWidgetTaskMenu {
    Q_OBJECT
public:
    explicit MdiContainerWidgetTaskMenu(QMdiArea *m, QObject *parent = 0);
    explicit MdiContainerWidgetTaskMenu(QWorkspace *m, QObject *parent = 0);

    virtual QList<QAction*> taskActions() const;
private:
    void initializeActions();

    QAction *m_nextAction;
    QAction *m_previousAction;
    QAction *m_tileAction;
    QAction *m_cascadeAction;
};

class ContainerWidgetTaskMenuFactory: public QExtensionFactory
{
    Q_OBJECT
public:
    explicit ContainerWidgetTaskMenuFactory(QDesignerFormEditorInterface *core, QExtensionManager *extensionManager = 0);

protected:
    virtual QObject *createExtension(QObject *object, const QString &iid, QObject *parent) const;

private:
    QDesignerFormEditorInterface *m_core;
};

}  // namespace qdesigner_internal

QT_END_NAMESPACE

#endif // CONTAINERWIDGER_TASKMENU_H
