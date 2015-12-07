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

#include "qdesigner_toolbox_p.h"
#include "qdesigner_command_p.h"
#include "orderdialog_p.h"
#include "promotiontaskmenu_p.h"
#include "formwindowbase_p.h"

#include <QtDesigner/QDesignerFormWindowInterface>

#include <QtGui/QAction>
#include <QtGui/QToolBox>
#include <QtGui/QMenu>
#include <QtGui/QLayout>
#include <QtCore/QHash>

QT_BEGIN_NAMESPACE

QToolBoxHelper::QToolBoxHelper(QToolBox *toolbox) :
    QObject(toolbox),
    m_toolbox(toolbox),
    m_actionDeletePage(new QAction(tr("Delete Page"), this)),
    m_actionInsertPage(new QAction(tr("Before Current Page"), this)),
    m_actionInsertPageAfter(new QAction(tr("After Current Page"), this)),
    m_actionChangePageOrder(new QAction(tr("Change Page Order..."), this)),
    m_pagePromotionTaskMenu(new qdesigner_internal::PromotionTaskMenu(0, qdesigner_internal::PromotionTaskMenu::ModeSingleWidget, this))
{
    connect(m_actionDeletePage, SIGNAL(triggered()), this, SLOT(removeCurrentPage()));
    connect(m_actionInsertPage, SIGNAL(triggered()), this, SLOT(addPage()));
    connect(m_actionInsertPageAfter, SIGNAL(triggered()), this, SLOT(addPageAfter()));
    connect(m_actionChangePageOrder, SIGNAL(triggered()), this, SLOT(changeOrder()));
    connect(m_toolbox, SIGNAL(currentChanged(int)), this, SLOT(slotCurrentChanged(int)));
}

void QToolBoxHelper::install(QToolBox *toolbox)
{
    new QToolBoxHelper(toolbox);
}

QToolBoxHelper *QToolBoxHelper::helperOf(const QToolBox *toolbox)
{
    QList<QToolBoxHelper*> helpers = qFindChildren<QToolBoxHelper*>(toolbox);
    if (helpers.empty())
        return 0;
    return helpers.front();
}

QMenu *QToolBoxHelper::addToolBoxContextMenuActions(const QToolBox *toolbox, QMenu *popup)
{
    QToolBoxHelper *helper = helperOf(toolbox);
    if (!helper)
        return 0;
    return helper->addContextMenuActions(popup);
}

void QToolBoxHelper::removeCurrentPage()
{
    if (m_toolbox->currentIndex() == -1 || !m_toolbox->widget(m_toolbox->currentIndex()))
        return;

    if (QDesignerFormWindowInterface *fw = QDesignerFormWindowInterface::findFormWindow(m_toolbox)) {
        qdesigner_internal::DeleteToolBoxPageCommand *cmd = new qdesigner_internal::DeleteToolBoxPageCommand(fw);
        cmd->init(m_toolbox);
        fw->commandHistory()->push(cmd);
    }
}

void QToolBoxHelper::addPage()
{
    if (QDesignerFormWindowInterface *fw = QDesignerFormWindowInterface::findFormWindow(m_toolbox)) {
        qdesigner_internal::AddToolBoxPageCommand *cmd = new qdesigner_internal::AddToolBoxPageCommand(fw);
        cmd->init(m_toolbox, qdesigner_internal::AddToolBoxPageCommand::InsertBefore);
        fw->commandHistory()->push(cmd);
    }
}

void QToolBoxHelper::changeOrder()
{
    QDesignerFormWindowInterface *fw = QDesignerFormWindowInterface::findFormWindow(m_toolbox);

    if (!fw)
        return;

    const QWidgetList oldPages = qdesigner_internal::OrderDialog::pagesOfContainer(fw->core(), m_toolbox);
    const int pageCount = oldPages.size();
    if (pageCount < 2)
        return;

    qdesigner_internal::OrderDialog dlg(fw);
    dlg.setPageList(oldPages);
    if (dlg.exec() == QDialog::Rejected)
        return;

    const QWidgetList newPages = dlg.pageList();
    if (newPages == oldPages)
        return;

    fw->beginCommand(tr("Change Page Order"));
    for(int i=0; i < pageCount; ++i) {
        if (newPages.at(i) == m_toolbox->widget(i))
            continue;
        qdesigner_internal::MoveToolBoxPageCommand *cmd = new qdesigner_internal::MoveToolBoxPageCommand(fw);
        cmd->init(m_toolbox, newPages.at(i), i);
        fw->commandHistory()->push(cmd);
    }
    fw->endCommand();
}

void QToolBoxHelper::addPageAfter()
{
    if (QDesignerFormWindowInterface *fw = QDesignerFormWindowInterface::findFormWindow(m_toolbox)) {
        qdesigner_internal::AddToolBoxPageCommand *cmd = new qdesigner_internal::AddToolBoxPageCommand(fw);
        cmd->init(m_toolbox, qdesigner_internal::AddToolBoxPageCommand::InsertAfter);
        fw->commandHistory()->push(cmd);
    }
}

QPalette::ColorRole QToolBoxHelper::currentItemBackgroundRole() const
{
    const QWidget *w = m_toolbox->widget(0);
    if (!w)
        return  QPalette::Window;
    return w->backgroundRole();
}

void QToolBoxHelper::setCurrentItemBackgroundRole(QPalette::ColorRole role)
{
    const int count = m_toolbox->count();
    for (int i = 0; i < count; ++i) {
        QWidget *w = m_toolbox->widget(i);
        w->setBackgroundRole(role);
        w->update();
    }
}

void QToolBoxHelper::slotCurrentChanged(int index)
{
    if (m_toolbox->widget(index)) {
        if (QDesignerFormWindowInterface *fw = QDesignerFormWindowInterface::findFormWindow(m_toolbox)) {
            fw->clearSelection();
            fw->selectWidget(m_toolbox, true);
        }
    }
}

QMenu *QToolBoxHelper::addContextMenuActions(QMenu *popup) const
{
    QMenu *pageMenu = 0;
    const int count = m_toolbox->count();
    m_actionDeletePage->setEnabled(count > 1);
    if (count) {
        const QString pageSubMenuLabel = tr("Page %1 of %2").arg(m_toolbox->currentIndex() + 1).arg(count);
        pageMenu = popup->addMenu(pageSubMenuLabel);

        pageMenu->addAction(m_actionDeletePage);
        // Set up promotion menu for current widget.
        if (QWidget *page =  m_toolbox->currentWidget ()) {
            m_pagePromotionTaskMenu->setWidget(page);
            m_pagePromotionTaskMenu->addActions(QDesignerFormWindowInterface::findFormWindow(m_toolbox),
                                                qdesigner_internal::PromotionTaskMenu::SuppressGlobalEdit,
                                                pageMenu);
        }
    }
    QMenu *insertPageMenu = popup->addMenu(tr("Insert Page"));
    insertPageMenu->addAction(m_actionInsertPageAfter);
    insertPageMenu->addAction(m_actionInsertPage);
    if (count > 1) {
        popup->addAction(m_actionChangePageOrder);
    }
    popup->addSeparator();
    return pageMenu;
}

// -------- QToolBoxWidgetPropertySheet

static const char *currentItemTextKey = "currentItemText";
static const char *currentItemNameKey = "currentItemName";
static const char *currentItemIconKey = "currentItemIcon";
static const char *currentItemToolTipKey = "currentItemToolTip";
static const char *tabSpacingKey = "tabSpacing";

enum { tabSpacingDefault = -1 };

QToolBoxWidgetPropertySheet::QToolBoxWidgetPropertySheet(QToolBox *object, QObject *parent) :
    QDesignerPropertySheet(object, parent),
    m_toolBox(object)
{
    createFakeProperty(QLatin1String(currentItemTextKey), QString());
    createFakeProperty(QLatin1String(currentItemNameKey), QString());
    createFakeProperty(QLatin1String(currentItemIconKey), qVariantFromValue(qdesigner_internal::PropertySheetIconValue()));
    if (formWindowBase())
        formWindowBase()->addReloadableProperty(this, indexOf(QLatin1String(currentItemIconKey)));
    createFakeProperty(QLatin1String(currentItemToolTipKey), QString());
    createFakeProperty(QLatin1String(tabSpacingKey), QVariant(tabSpacingDefault));
}

QToolBoxWidgetPropertySheet::ToolBoxProperty QToolBoxWidgetPropertySheet::toolBoxPropertyFromName(const QString &name)
{
    typedef QHash<QString, ToolBoxProperty> ToolBoxPropertyHash;
    static ToolBoxPropertyHash toolBoxPropertyHash;
    if (toolBoxPropertyHash.empty()) {
        toolBoxPropertyHash.insert(QLatin1String(currentItemTextKey),    PropertyCurrentItemText);
        toolBoxPropertyHash.insert(QLatin1String(currentItemNameKey),    PropertyCurrentItemName);
        toolBoxPropertyHash.insert(QLatin1String(currentItemIconKey),    PropertyCurrentItemIcon);
        toolBoxPropertyHash.insert(QLatin1String(currentItemToolTipKey), PropertyCurrentItemToolTip);
        toolBoxPropertyHash.insert(QLatin1String(tabSpacingKey),         PropertyTabSpacing);
    }
    return toolBoxPropertyHash.value(name, PropertyToolBoxNone);
}

void QToolBoxWidgetPropertySheet::setProperty(int index, const QVariant &value)
{
    const ToolBoxProperty toolBoxProperty = toolBoxPropertyFromName(propertyName(index));
    // independent of index
    switch (toolBoxProperty) {
    case PropertyTabSpacing:
        m_toolBox->layout()->setSpacing(value.toInt());
        return;
    case PropertyToolBoxNone:
        QDesignerPropertySheet::setProperty(index, value);
        return;
    default:
        break;
    }
    // index-dependent
    const int currentIndex = m_toolBox->currentIndex();
    if (currentIndex == -1)
        return;

    switch (toolBoxProperty) {
    case PropertyCurrentItemText:
        m_toolBox->setItemText(currentIndex, value.toString());
        break;
    case PropertyCurrentItemName:
        m_toolBox->widget(currentIndex)->setObjectName(value.toString());
        break;
    case PropertyCurrentItemIcon:
        m_toolBox->setItemIcon(currentIndex, qvariant_cast<QIcon>(resolvePropertyValue(index, value)));
        m_pageToIcon[currentIndex] = qVariantValue<qdesigner_internal::PropertySheetIconValue>(value);
        break;
    case PropertyCurrentItemToolTip:
        m_toolBox->setItemToolTip(currentIndex, value.toString());
        break;
    case PropertyTabSpacing:
    case PropertyToolBoxNone:
        break;
    }
}

bool QToolBoxWidgetPropertySheet::isEnabled(int index) const
{
    switch (toolBoxPropertyFromName(propertyName(index))) {
    case PropertyToolBoxNone:  // independent of index
    case PropertyTabSpacing:
        return QDesignerPropertySheet::isEnabled(index);
    default:
        break;
    }
    return m_toolBox->currentIndex() != -1;
}

QVariant QToolBoxWidgetPropertySheet::property(int index) const
{
    const ToolBoxProperty toolBoxProperty = toolBoxPropertyFromName(propertyName(index));
    // independent of index
    switch (toolBoxProperty) {
    case PropertyTabSpacing:
        return m_toolBox->layout()->spacing();
    case PropertyToolBoxNone:
        return QDesignerPropertySheet::property(index);
    default:
        break;
    }
    // index-dependent
    const int currentIndex = m_toolBox->currentIndex();
    if (currentIndex == -1) {
        if (toolBoxProperty == PropertyCurrentItemIcon)
            return  qVariantFromValue(qdesigner_internal::PropertySheetIconValue());
        return QVariant(QString());
    }

    // index-dependent
    switch (toolBoxProperty) {
    case PropertyCurrentItemText:
        return m_toolBox->itemText(currentIndex);
    case PropertyCurrentItemName:
        return m_toolBox->widget(currentIndex)->objectName();
    case PropertyCurrentItemIcon:
        return qVariantFromValue(m_pageToIcon.value(currentIndex));
    case PropertyCurrentItemToolTip:
        return m_toolBox->itemToolTip(currentIndex);
    case PropertyTabSpacing:
    case PropertyToolBoxNone:
        break;
    }
    return QVariant();
}

bool QToolBoxWidgetPropertySheet::reset(int index)
{
    const ToolBoxProperty toolBoxProperty = toolBoxPropertyFromName(propertyName(index));
    // independent of index
    switch (toolBoxProperty) {
    case PropertyTabSpacing:
        setProperty(index, QVariant(tabSpacingDefault));
        return true;
    case PropertyToolBoxNone:
        return QDesignerPropertySheet::reset(index);
    default:
        break;
    }
    // index-dependent
    const int currentIndex = m_toolBox->currentIndex();
    if (currentIndex == -1)
        return false;

    // index-dependent
    switch (toolBoxProperty) {
    case PropertyCurrentItemText:
    case PropertyCurrentItemName:
    case PropertyCurrentItemToolTip:
        setProperty(index, QString());
        break;
    case PropertyCurrentItemIcon:
        m_pageToIcon.remove(index);
        setProperty(index, QIcon());
        break;
    case PropertyTabSpacing:
    case PropertyToolBoxNone:
        break;
    }
    return true;
}

bool QToolBoxWidgetPropertySheet::checkProperty(const QString &propertyName)
{
    switch (toolBoxPropertyFromName(propertyName)) {
    case PropertyCurrentItemText:
    case PropertyCurrentItemName:
    case PropertyCurrentItemToolTip:
    case PropertyCurrentItemIcon:
        return false;
    default:
        break;
    }
    return true;
}

QT_END_NAMESPACE
