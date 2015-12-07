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

#include "widgetbox_dnditem.h"
#include "ui4_p.h"
#include <spacer_widget_p.h>
#include <qdesigner_formbuilder_p.h>
#include <qtresourcemodel_p.h>
#include <formscriptrunner_p.h>
#include <QtDesigner/QDesignerFormEditorInterface>

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {
/*******************************************************************************
** WidgetBoxResource
*/

class WidgetBoxResource : public QDesignerFormBuilder
{
public:
    WidgetBoxResource(QDesignerFormEditorInterface *core);

    virtual QWidget *createWidget(DomWidget *ui_widget, QWidget *parentWidget)
    { return QDesignerFormBuilder::createWidget(ui_widget, parentWidget); }

    QWidget *createWidgetWithResources(const DomUI *dom_ui, DomWidget *dom_widget, DomResources *dom_resources, QWidget *result);

protected:

    virtual QWidget *create(DomWidget *ui_widget, QWidget *parents);
    virtual QWidget *createWidget(const QString &widgetName, QWidget *parentWidget, const QString &name);
};

WidgetBoxResource::WidgetBoxResource(QDesignerFormEditorInterface *core) :
    QDesignerFormBuilder(core, DisableScripts)
{
}


QWidget *WidgetBoxResource::createWidget(const QString &widgetName, QWidget *parentWidget, const QString &name)
{
    if (widgetName == QLatin1String("Spacer")) {
        Spacer *spacer = new Spacer(parentWidget);
        spacer->setObjectName(name);
        return spacer;
    }

    return QDesignerFormBuilder::createWidget(widgetName, parentWidget, name);
}

QWidget *WidgetBoxResource::create(DomWidget *ui_widget, QWidget *parent)
{
    QWidget *result = QDesignerFormBuilder::create(ui_widget, parent);
    result->setFocusPolicy(Qt::NoFocus);
    result->setObjectName(ui_widget->attributeName());

    return result;
}

QWidget *WidgetBoxResource::createWidgetWithResources(const DomUI *dom_ui, DomWidget *dom_widget, DomResources *dom_resources, QWidget *result)
{
    initialize(dom_ui);
    QtResourceSet *resourceSet = core()->resourceModel()->currentResourceSet();
    createResources(dom_resources);
    core()->resourceModel()->setCurrentResourceSet(internalResourceSet());

    QWidget *widget = createWidget(dom_widget, result);
    core()->resourceModel()->setCurrentResourceSet(resourceSet);
    core()->resourceModel()->removeResourceSet(internalResourceSet());
    return widget;
}

/*******************************************************************************
** WidgetBoxResource
*/

static QSize geometryProp(const DomWidget *dw)
{
    const QList<DomProperty*> prop_list = dw->elementProperty();
    const QString geometry = QLatin1String("geometry");
    foreach (DomProperty *prop, prop_list) {
        if (prop->attributeName() !=  geometry)
            continue;
        DomRect *dr = prop->elementRect();
        if (dr == 0)
            continue;
        return QSize(dr->elementWidth(), dr->elementHeight());
    }
    return QSize();
}

static QSize domWidgetSize(DomWidget *dw)
{
    QSize size = geometryProp(dw);
    if (size.isValid())
        return size;

    foreach (const DomWidget *child, dw->elementWidget()) {
        size = geometryProp(child);
        if (size.isValid())
            return size;
    }

    foreach (const DomLayout *dl, dw->elementLayout()) {
        foreach (DomLayoutItem *item, dl->elementItem()) {
            const DomWidget *child = item->elementWidget();
            if (child == 0)
                continue;
            size = geometryProp(child);
            if (size.isValid())
                return size;
        }
    }

    return QSize();
}

static QWidget *decorationFromDomWidget(const DomUI *dom_ui, DomWidget *dom_widget, DomResources *dom_resources, QDesignerFormEditorInterface *core)
{
    QWidget *result = new QWidget(0, Qt::ToolTip);

    WidgetBoxResource builder(core);
    QWidget *w = builder.createWidgetWithResources(dom_ui, dom_widget, dom_resources, result);
    QSize size = domWidgetSize(dom_widget);
    const QSize minimumSize = w->minimumSizeHint();
    if (!size.isValid())
        size = w->sizeHint();
    if (size.width() < minimumSize.width())
        size.setWidth(minimumSize.width());
    if (size.height() < minimumSize.height())
        size.setHeight(minimumSize.height());
    // A QWidget might have size -1,-1 if no geometry property is specified in the widget box.
    if (size.isEmpty())
        size = size.expandedTo(QSize(16, 16));
    w->setGeometry(QRect(QPoint(0, 0), size));
    result->resize(size);
    return result;
}

WidgetBoxDnDItem::WidgetBoxDnDItem(QDesignerFormEditorInterface *core,
                                   DomUI *dom_ui,
                                   const QPoint &global_mouse_pos) :
    QDesignerDnDItem(CopyDrop)
{
    DomWidget *child = dom_ui->elementWidget()->elementWidget().front();
    QWidget *decoration = decorationFromDomWidget(dom_ui, child, dom_ui->elementResources(), core);
    decoration->move(global_mouse_pos - QPoint(5, 5));

    init(dom_ui, 0, decoration, global_mouse_pos);
}
}

QT_END_NAMESPACE
