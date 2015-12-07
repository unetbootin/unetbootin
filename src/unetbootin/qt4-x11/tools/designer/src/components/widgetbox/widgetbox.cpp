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

#include "widgetbox.h"
#include "widgetboxtreewidget.h"
#include "widgetbox_dnditem.h"

#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QDesignerFormWindowManagerInterface>

#include <iconloader_p.h>
#include <qdesigner_utils_p.h>
#include <filterwidget_p.h>

#include <QtGui/QDropEvent>
#include <QtGui/QVBoxLayout>
#include <QtGui/QApplication>
#include <QtGui/QToolBar>
#include <QtGui/QIcon>

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {

WidgetBox::WidgetBox(QDesignerFormEditorInterface *core, QWidget *parent, Qt::WindowFlags flags)
    : QDesignerWidgetBox(parent, flags),
      m_core(core),
      m_view(new WidgetBoxTreeWidget(m_core))
{

    QVBoxLayout *l = new QVBoxLayout(this);
    l->setMargin(0);
    l->setSpacing(0);

    // Prevent the filter from grabbing focus since Our view has Qt::NoFocus
    FilterWidget *filterWidget = new FilterWidget(0, FilterWidget::LayoutAlignNone);
    filterWidget->setRefuseFocus(true);
    connect(filterWidget, SIGNAL(filterChanged(QString)), m_view, SLOT(filter(QString)));
    l->addWidget(filterWidget);

    // View
    connect(m_view, SIGNAL(pressed(QString,QString,QPoint)),
            this, SLOT(handleMousePress(QString,QString,QPoint)));
    l->addWidget(m_view);

    setAcceptDrops (true);
}

WidgetBox::~WidgetBox()
{
}

QDesignerFormEditorInterface *WidgetBox::core() const
{
    return m_core;
}

void WidgetBox::handleMousePress(const QString &name, const QString &xml, const QPoint &global_mouse_pos)
{
    if (QApplication::mouseButtons() != Qt::LeftButton)
        return;

    DomUI *ui = xmlToUi(name, xml, true);
    if (ui == 0)
        return;
    QList<QDesignerDnDItemInterface*> item_list;
    item_list.append(new WidgetBoxDnDItem(core(), ui, global_mouse_pos));
    m_core->formWindowManager()->dragItems(item_list);
}

int WidgetBox::categoryCount() const
{
    return m_view->categoryCount();
}

QDesignerWidgetBoxInterface::Category WidgetBox::category(int cat_idx) const
{
    return m_view->category(cat_idx);
}

void WidgetBox::addCategory(const Category &cat)
{
    m_view->addCategory(cat);
}

void WidgetBox::removeCategory(int cat_idx)
{
    m_view->removeCategory(cat_idx);
}

int WidgetBox::widgetCount(int cat_idx) const
{
    return m_view->widgetCount(cat_idx);
}

QDesignerWidgetBoxInterface::Widget WidgetBox::widget(int cat_idx, int wgt_idx) const
{
    return m_view->widget(cat_idx, wgt_idx);
}

void WidgetBox::addWidget(int cat_idx, const Widget &wgt)
{
    m_view->addWidget(cat_idx, wgt);
}

void WidgetBox::removeWidget(int cat_idx, int wgt_idx)
{
    m_view->removeWidget(cat_idx, wgt_idx);
}

void WidgetBox::dropWidgets(const QList<QDesignerDnDItemInterface*> &item_list, const QPoint&)
{
    m_view->dropWidgets(item_list);
}

void WidgetBox::setFileName(const QString &file_name)
{
    m_view->setFileName(file_name);
}

QString WidgetBox::fileName() const
{
    return m_view->fileName();
}

bool WidgetBox::load()
{
    return m_view->load(loadMode());
}

bool WidgetBox::loadContents(const QString &contents)
{
    return m_view->loadContents(contents);
}

bool WidgetBox::save()
{
    return m_view->save();
}

static const QDesignerMimeData *checkDragEvent(QDropEvent * event,
                                               bool acceptEventsFromWidgetBox)
{
    const QDesignerMimeData *mimeData = qobject_cast<const QDesignerMimeData *>(event->mimeData());
    if (!mimeData) {
        event->ignore();
        return 0;
    }
    // If desired, ignore a widget box drag and drop, where widget==0.
    if (!acceptEventsFromWidgetBox) {
        const bool fromWidgetBox = !mimeData->items().first()->widget();
        if (fromWidgetBox) {
            event->ignore();
            return 0;
        }
    }

    mimeData->acceptEvent(event);
    return mimeData;
}

void WidgetBox::dragEnterEvent (QDragEnterEvent * event)
{
    // We accept event originating from the widget box also here,
    // because otherwise Windows will not show the DnD pixmap.
    checkDragEvent(event, true);
}

void WidgetBox::dragMoveEvent(QDragMoveEvent * event)
{
    checkDragEvent(event, true);
}

void WidgetBox::dropEvent(QDropEvent * event)
{
    const QDesignerMimeData *mimeData = checkDragEvent(event, false);
    if (!mimeData)
        return;

    dropWidgets(mimeData->items(), event->pos());
    QDesignerMimeData::removeMovedWidgetsFromSourceForm(mimeData->items());
}

QIcon WidgetBox::iconForWidget(const QString &className, const QString &category) const
{
    Widget widgetData;
    if (!findWidget(this, className, category, &widgetData))
        return QIcon();
    return m_view->iconForWidget(widgetData.iconName());
}

}  // namespace qdesigner_internal

QT_END_NAMESPACE
