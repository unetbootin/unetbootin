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

#include "layoutinfo_p.h"

#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QDesignerContainerExtension>
#include <QtDesigner/QDesignerMetaDataBaseInterface>
#include <QtDesigner/QExtensionManager>

#include <QtGui/QHBoxLayout>
#include <QtGui/QFormLayout>
#include <QtGui/QSplitter>
#include <QtCore/QDebug>
#include <QtCore/QHash>

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {
/*!
  \overload
*/
LayoutInfo::Type LayoutInfo::layoutType(const QDesignerFormEditorInterface *core, const QLayout *layout)
{
    Q_UNUSED(core)
    if (qobject_cast<const QHBoxLayout*>(layout))
        return HBox;
    else if (qobject_cast<const QVBoxLayout*>(layout))
        return VBox;
    else if (qobject_cast<const QGridLayout*>(layout))
        return Grid;
    else if (qobject_cast<const QFormLayout*>(layout))
       return Form;
    return NoLayout;
}

LayoutInfo::Type LayoutInfo::layoutType(const QString &typeName)
{
    static QHash<QString, Type> nameTypeMap;
    if (nameTypeMap.empty()) {
        nameTypeMap.insert(QLatin1String("QVBoxLayout"), VBox);
        nameTypeMap.insert(QLatin1String("QHBoxLayout"), HBox);
        nameTypeMap.insert(QLatin1String("QGridLayout"), Grid);
        nameTypeMap.insert(QLatin1String("QFormLayout"), Form);
    }
    return nameTypeMap.value(typeName, NoLayout);
}
/*!
  \overload
*/
LayoutInfo::Type LayoutInfo::layoutType(const QDesignerFormEditorInterface *core, const QWidget *w)
{
    if (const QSplitter *splitter = qobject_cast<const QSplitter *>(w))
        return  splitter->orientation() == Qt::Horizontal ? HSplitter : VSplitter;
    return layoutType(core, w->layout());
}

QWidget *LayoutInfo::layoutParent(const QDesignerFormEditorInterface *core, QLayout *layout)
{
    Q_UNUSED(core)

    QObject *o = layout;
    while (o) {
        if (QWidget *widget = qobject_cast<QWidget*>(o))
            return widget;

        o = o->parent();
    }
    return 0;
}

void LayoutInfo::deleteLayout(const QDesignerFormEditorInterface *core, QWidget *widget)
{
    if (QDesignerContainerExtension *container = qt_extension<QDesignerContainerExtension*>(core->extensionManager(), widget))
        widget = container->widget(container->currentIndex());

    Q_ASSERT(widget != 0);

    QLayout *layout = managedLayout(core, widget);

    if (layout == 0 || core->metaDataBase()->item(layout) != 0) {
        delete layout;
        widget->updateGeometry();
        return;
    }

    qDebug() << "trying to delete an unmanaged layout:" << "widget:" << widget << "layout:" << layout;
}

LayoutInfo::Type LayoutInfo::laidoutWidgetType(const QDesignerFormEditorInterface *core, QWidget *widget, bool *isManaged)
{
    if (isManaged)
        *isManaged = false;

    QWidget *parent = widget->parentWidget();
    if (!parent)
        return NoLayout;

    // 1) Splitter
    if (QSplitter *splitter  = qobject_cast<QSplitter*>(parent)) {
        if (isManaged)
            *isManaged = core->metaDataBase()->item(splitter);
        return  splitter->orientation() == Qt::Horizontal ? HSplitter : VSplitter;
    }

    // 2) Layout of parent
    QLayout *parentLayout = parent->layout();
    if (!parentLayout)
        return NoLayout;

    if (parentLayout->indexOf(widget) != -1) {
        if (isManaged)
            *isManaged = core->metaDataBase()->item(parentLayout);
        return layoutType(core, parentLayout);
    }

    // 3) Some child layout
    const QList<QLayout*> childLayouts = qFindChildren<QLayout*>(parentLayout);
    if (childLayouts.empty())
        return NoLayout;
    const QList<QLayout*>::const_iterator lcend = childLayouts.constEnd();
    for (QList<QLayout*>::const_iterator it = childLayouts.constBegin(); it != lcend; ++it)
        if ((*it)->indexOf(widget) != -1) {
            if (isManaged)
                *isManaged = core->metaDataBase()->item(*it);
            return layoutType(core, parentLayout);
        }

    return NoLayout;
}

QLayout *LayoutInfo::internalLayout(const QWidget *widget)
{
    QLayout *widgetLayout = widget->layout();
    if (widgetLayout && widget->inherits("Q3GroupBox")) {
        if (widgetLayout->count()) {
            widgetLayout = widgetLayout->itemAt(0)->layout();
        } else {
            widgetLayout = 0;
        }
    }
    return widgetLayout;
}


QLayout *LayoutInfo::managedLayout(const QDesignerFormEditorInterface *core, const QWidget *widget)
{
    if (widget == 0)
        return 0;

    QLayout *layout = widget->layout();
    if (!layout)
        return 0;

    return managedLayout(core, layout);
}

QLayout *LayoutInfo::managedLayout(const QDesignerFormEditorInterface *core, QLayout *layout)
{
    QDesignerMetaDataBaseInterface *metaDataBase = core->metaDataBase();

    if (!metaDataBase)
        return layout;

    const QDesignerMetaDataBaseItemInterface *item = metaDataBase->item(layout);
    if (item == 0) {
        layout = qFindChild<QLayout*>(layout);
        item = metaDataBase->item(layout);
    }
    if (!item)
        return 0;
    return layout;
}

    static inline void getFormItemPosition(const QFormLayout *formLayout, int index, int *row, int *column)
{
    QFormLayout::ItemRole role;
    formLayout->getItemPosition(index, row, &role);
    *column = role == QFormLayout::LabelRole ? 0 : 1;
}

QDESIGNER_SHARED_EXPORT void getFormLayoutItemPosition(const QFormLayout *formLayout, int index, int *rowPtr, int *columnPtr, int *rowspanPtr, int *colspanPtr)
{
    int row;
    QFormLayout::ItemRole role;
    formLayout->getItemPosition(index, &row, &role);
    if (rowPtr)
        *rowPtr = row;
    if (columnPtr)
        *columnPtr = role == QFormLayout::LabelRole ? 0 : 1;
    if (rowspanPtr)
        *rowspanPtr = 1;
    if (colspanPtr)
        *colspanPtr = 1;
}
} // namespace qdesigner_internal

QT_END_NAMESPACE
