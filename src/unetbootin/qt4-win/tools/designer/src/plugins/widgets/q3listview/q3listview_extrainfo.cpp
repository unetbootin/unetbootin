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

#include "q3listview_extrainfo.h"

#include <QtDesigner/QDesignerIconCacheInterface>
#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/private/ui4_p.h>

#include <Qt3Support/Q3ListView>
#include <Qt3Support/Q3Header>

QT_BEGIN_NAMESPACE

inline QHash<QString, DomProperty *> propertyMap(const QList<DomProperty *> &properties) // ### remove me
{
    QHash<QString, DomProperty *> map;

    for (int i=0; i<properties.size(); ++i) {
        DomProperty *p = properties.at(i);
        map.insert(p->attributeName(), p);
    }

    return map;
}

Q3ListViewExtraInfo::Q3ListViewExtraInfo(Q3ListView *widget, QDesignerFormEditorInterface *core, QObject *parent)
    : QObject(parent), m_widget(widget), m_core(core)
{}

QWidget *Q3ListViewExtraInfo::widget() const
{ return m_widget; }

QDesignerFormEditorInterface *Q3ListViewExtraInfo::core() const
{ return m_core; }

bool Q3ListViewExtraInfo::saveUiExtraInfo(DomUI *ui)
{ Q_UNUSED(ui); return false; }

bool Q3ListViewExtraInfo::loadUiExtraInfo(DomUI *ui)
{ Q_UNUSED(ui); return false; }


bool Q3ListViewExtraInfo::saveWidgetExtraInfo(DomWidget *ui_widget)
{
    // ### finish me
    Q3ListView *listView = qobject_cast<Q3ListView*>(widget());
    Q_ASSERT(listView != 0);

    QList<DomColumn*> columns;
    Q3Header *header = listView->header();
    for (int i=0; i<header->count(); ++i) {
        DomColumn *c = new DomColumn();

        QList<DomProperty*> properties;

        DomString *str = new DomString();
        str->setText(header->label(i));

        DomProperty *ptext = new DomProperty();
        ptext->setAttributeName(QLatin1String("text"));
        ptext->setElementString(str);

        DomProperty *pclickable = new DomProperty();
        pclickable->setAttributeName(QLatin1String("clickable"));
        pclickable->setElementBool(header->isClickEnabled(i) ? QLatin1String("true") : QLatin1String("false"));

        DomProperty *presizable = new DomProperty();
        presizable->setAttributeName(QLatin1String("resizable"));
        presizable->setElementBool(header->isResizeEnabled(i) ? QLatin1String("true") : QLatin1String("false"));

        properties.append(ptext);
        properties.append(pclickable);
        properties.append(presizable);

        c->setElementProperty(properties);
        columns.append(c);
    }

    ui_widget->setElementColumn(columns);

    QList<DomItem *> items;
    Q3ListViewItem *child = listView->firstChild();
    while (child) {
        items.append(saveQ3ListViewItem(child));
        child = child->nextSibling();
    }
    ui_widget->setElementItem(items);


    return true;
}

bool Q3ListViewExtraInfo::loadWidgetExtraInfo(DomWidget *ui_widget)
{
    Q3ListView *listView = qobject_cast<Q3ListView*>(widget());
    Q_ASSERT(listView != 0);

    Q3Header *header = listView->header();

    QList<DomColumn*> columns = ui_widget->elementColumn();
    for (int i=0; i<columns.size(); ++i) {
        DomColumn *column = columns.at(i);

        QHash<QString, DomProperty*> properties = propertyMap(column->elementProperty());
        DomProperty *text = properties.value(QLatin1String("text"));
        DomProperty *pixmap = properties.value(QLatin1String("pixmap"));
        DomProperty *clickable = properties.value(QLatin1String("clickable"));
        DomProperty *resizable = properties.value(QLatin1String("resizable"));

        QString txt = text->elementString()->text();

        if (pixmap != 0) {
            DomResourcePixmap *pix = pixmap->elementPixmap();
            QIcon icon(core()->iconCache()->resolveQrcPath(pix->text(), pix->attributeResource(), workingDirectory()));
            listView->addColumn(icon, txt);
        } else {
            listView->addColumn(txt);
        }

        if (clickable != 0) {
            header->setClickEnabled(clickable->elementBool() == QLatin1String("true"), header->count() - 1);
        }

        if (resizable != 0) {
            header->setResizeEnabled(resizable->elementBool() == QLatin1String("true"), header->count() - 1);
        }
    }

    if (ui_widget->elementItem().size()) {
        initializeQ3ListViewItems(ui_widget->elementItem());
    }

    return true;
}

DomItem *Q3ListViewExtraInfo::saveQ3ListViewItem(Q3ListViewItem *item) const
{
    DomItem *pitem = new DomItem();
    QList<DomProperty *> properties;
    const int columnCount = static_cast<Q3ListView*>(widget())->columns();
    for (int i = 0; i < columnCount; ++i) {
        DomString *str = new DomString();
        str->setText(item->text(i));

        DomProperty *ptext = new DomProperty();
        ptext->setAttributeName(QLatin1String("text"));
        ptext->setElementString(str);

        properties.append(ptext);
    }
    pitem->setElementProperty(properties);
    QList<DomItem *> items;
    Q3ListViewItem *child = item->firstChild();
    while (child) {
        items.append(saveQ3ListViewItem(child));
        child = child->nextSibling();
    }
    pitem->setElementItem(items);
    return pitem;
}

void Q3ListViewExtraInfo::initializeQ3ListViewItems(const QList<DomItem *> &items, Q3ListViewItem *parentItem)
{
    for (int i=0; i<items.size(); ++i) {
        DomItem *item = items.at(i);

        Q3ListViewItem *__item = 0;
        if (parentItem != 0)
            __item = new Q3ListViewItem(parentItem);
        else
            __item = new Q3ListViewItem(static_cast<Q3ListView*>(widget()));

        int textCount = 0, pixCount = 0;
        QList<DomProperty*> properties = item->elementProperty();
        for (int i=0; i<properties.size(); ++i) {
            DomProperty *p = properties.at(i);
            if (p->attributeName() == QLatin1String("text"))
                __item->setText(textCount++, p->elementString()->text());

            if (p->attributeName() == QLatin1String("pixmap")) {
                DomResourcePixmap *pix = p->elementPixmap();
                QPixmap pixmap(core()->iconCache()->resolveQrcPath(pix->text(), pix->attributeResource(), workingDirectory()));
                __item->setPixmap(pixCount++, pixmap);
            }
        }

        if (item->elementItem().size()) {
            __item->setOpen(true);
            initializeQ3ListViewItems(item->elementItem(), __item);
        }
    }
}


Q3ListViewExtraInfoFactory::Q3ListViewExtraInfoFactory(QDesignerFormEditorInterface *core, QExtensionManager *parent)
    : QExtensionFactory(parent), m_core(core)
{}

QObject *Q3ListViewExtraInfoFactory::createExtension(QObject *object, const QString &iid, QObject *parent) const
{
    if (iid != Q_TYPEID(QDesignerExtraInfoExtension))
        return 0;

    if (Q3ListView *w = qobject_cast<Q3ListView*>(object))
        return new Q3ListViewExtraInfo(w, m_core, parent);

    return 0;
}

QT_END_NAMESPACE
