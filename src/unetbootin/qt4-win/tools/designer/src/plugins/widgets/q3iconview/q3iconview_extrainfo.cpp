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

#include "q3iconview_extrainfo.h"

#include <QtDesigner/QDesignerIconCacheInterface>
#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/private/ui4_p.h>

#include <Qt3Support/Q3IconView>

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

Q3IconViewExtraInfo::Q3IconViewExtraInfo(Q3IconView *widget, QDesignerFormEditorInterface *core, QObject *parent)
    : QObject(parent), m_widget(widget), m_core(core)
{}

QWidget *Q3IconViewExtraInfo::widget() const
{ return m_widget; }

QDesignerFormEditorInterface *Q3IconViewExtraInfo::core() const
{ return m_core; }

bool Q3IconViewExtraInfo::saveUiExtraInfo(DomUI *ui)
{ Q_UNUSED(ui); return false; }

bool Q3IconViewExtraInfo::loadUiExtraInfo(DomUI *ui)
{ Q_UNUSED(ui); return false; }


bool Q3IconViewExtraInfo::saveWidgetExtraInfo(DomWidget *ui_widget)
{
    // ### finish me
    Q3IconView *iconView = qobject_cast<Q3IconView*>(widget());
    Q_ASSERT(iconView != 0);

    QList<DomItem*> ui_items;

    Q3IconViewItem *__item = iconView->firstItem();
    while (__item != 0) {
        DomItem *ui_item = new DomItem();

        QList<DomProperty*> properties;

        // text property
        DomProperty *ptext = new DomProperty();
        DomString *str = new DomString();
        str->setText(__item->text());
        ptext->setAttributeName(QLatin1String("text"));
        ptext->setElementString(str);
        properties.append(ptext);

        ui_item->setElementProperty(properties);
        ui_items.append(ui_item);

        if (__item->pixmap() != 0 && core()->iconCache()) {
            QPixmap pix = *__item->pixmap();
            QString filePath = core()->iconCache()->pixmapToFilePath(pix);
            QString qrcPath = core()->iconCache()->pixmapToQrcPath(pix);
            DomResourcePixmap *ui_pix = new DomResourcePixmap();
            if (!qrcPath.isEmpty())
                ui_pix->setAttributeResource(qrcPath);
            ui_pix->setText(filePath);

            DomProperty *ppix = new DomProperty();
            ppix->setAttributeName(QLatin1String("pixmap"));
            ppix->setElementPixmap(ui_pix);
            properties.append(ppix);
        }

        __item = __item->nextItem();
    }

    ui_widget->setElementItem(ui_items);

    return true;
}

bool Q3IconViewExtraInfo::loadWidgetExtraInfo(DomWidget *ui_widget)
{
    Q3IconView *iconView = qobject_cast<Q3IconView*>(widget());
    Q_ASSERT(iconView != 0);
    Q_UNUSED(iconView);

    if (ui_widget->elementItem().size()) {
        initializeQ3IconViewItems(ui_widget->elementItem());
    }

    return true;
}

void Q3IconViewExtraInfo::initializeQ3IconViewItems(const QList<DomItem *> &items)
{
    Q3IconView *iconView = qobject_cast<Q3IconView*>(widget());
    Q_ASSERT(iconView != 0);

    for (int i=0; i<items.size(); ++i) {
        DomItem *item = items.at(i);

        Q3IconViewItem *__item = new Q3IconViewItem(iconView);

        QList<DomProperty*> properties = item->elementProperty();
        for (int i=0; i<properties.size(); ++i) {
            DomProperty *p = properties.at(i);
            if (p->attributeName() == QLatin1String("text"))
                __item->setText(p->elementString()->text());

            if (p->attributeName() == QLatin1String("pixmap")) {
                DomResourcePixmap *pix = p->elementPixmap();
                QPixmap pixmap(core()->iconCache()->resolveQrcPath(pix->text(), pix->attributeResource(), workingDirectory()));
                __item->setPixmap(pixmap);
            }
        }
    }
}


Q3IconViewExtraInfoFactory::Q3IconViewExtraInfoFactory(QDesignerFormEditorInterface *core, QExtensionManager *parent)
    : QExtensionFactory(parent), m_core(core)
{}

QObject *Q3IconViewExtraInfoFactory::createExtension(QObject *object, const QString &iid, QObject *parent) const
{
    if (iid != Q_TYPEID(QDesignerExtraInfoExtension))
        return 0;

    if (Q3IconView *w = qobject_cast<Q3IconView*>(object))
        return new Q3IconViewExtraInfo(w, m_core, parent);

    return 0;
}

QT_END_NAMESPACE
