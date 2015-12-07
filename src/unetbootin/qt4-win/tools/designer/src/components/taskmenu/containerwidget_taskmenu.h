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
