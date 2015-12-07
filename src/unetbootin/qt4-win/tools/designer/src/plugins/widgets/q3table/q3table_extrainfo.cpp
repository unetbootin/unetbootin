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

#include "q3table_extrainfo.h"

#include <QtDesigner/QDesignerIconCacheInterface>
#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/private/ui4_p.h>

#include <Qt3Support/Q3Table>

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

Q3TableExtraInfo::Q3TableExtraInfo(Q3Table *widget, QDesignerFormEditorInterface *core, QObject *parent)
    : QObject(parent), m_widget(widget), m_core(core)
{}

QWidget *Q3TableExtraInfo::widget() const
{ return m_widget; }

QDesignerFormEditorInterface *Q3TableExtraInfo::core() const
{ return m_core; }

bool Q3TableExtraInfo::saveUiExtraInfo(DomUI *ui)
{ Q_UNUSED(ui); return false; }

bool Q3TableExtraInfo::loadUiExtraInfo(DomUI *ui)
{ Q_UNUSED(ui); return false; }


bool Q3TableExtraInfo::saveWidgetExtraInfo(DomWidget *ui_widget)
{
    Q_UNUSED(ui_widget);

    Q3Table *table = qobject_cast<Q3Table*>(widget());
    Q_ASSERT(table != 0);

    Q3Header *hHeader = table->horizontalHeader();

    QList<DomColumn*> columns;
    for (int i=0; i<hHeader->count(); ++i) {
        DomColumn *column = new DomColumn();
        QList<DomProperty *> properties;

        DomProperty *property = new DomProperty();
        DomString *string = new DomString();
        string->setText(hHeader->label(i));
        property->setElementString(string);
        property->setAttributeName("text");
        properties.append(property);

        column->setElementProperty(properties);
        columns.append(column);
    }
    ui_widget->setElementColumn(columns);

    Q3Header *vHeader = table->verticalHeader();

    QList<DomRow*> rows;
    for (int i=0; i<vHeader->count(); ++i) {
        DomRow *row = new DomRow();
        QList<DomProperty *> properties;

        DomProperty *property = new DomProperty();
        DomString *string = new DomString();
        string->setText(vHeader->label(i));
        property->setElementString(string);
        property->setAttributeName("text");
        properties.append(property);

        row->setElementProperty(properties);
        rows.append(row);
    }
    ui_widget->setElementRow(rows);

    return true;
}

bool Q3TableExtraInfo::loadWidgetExtraInfo(DomWidget *ui_widget)
{
    Q_UNUSED(ui_widget);

    Q3Table *table = qobject_cast<Q3Table*>(widget());
    Q_ASSERT(table != 0);

    Q3Header *hHeader = table->horizontalHeader();

    QList<DomColumn*> columns = ui_widget->elementColumn();
    for (int i=0; i<columns.size(); ++i) {
        DomColumn *column = columns.at(i);

        QHash<QString, DomProperty*> properties = propertyMap(column->elementProperty());
        DomProperty *text = properties.value(QLatin1String("text"));
        DomProperty *pixmap = properties.value(QLatin1String("pixmap"));

        QString txt = text->elementString()->text();

        if (pixmap != 0) {
            DomResourcePixmap *pix = pixmap->elementPixmap();
            QIcon icon(core()->iconCache()->resolveQrcPath(pix->text(), pix->attributeResource(), workingDirectory()));
            hHeader->setLabel(i, icon, txt);
        } else {
            hHeader->setLabel(i, txt);
        }
    }

    Q3Header *vHeader = table->verticalHeader();

    QList<DomRow*> rows = ui_widget->elementRow();
    for (int i=0; i<rows.size(); ++i) {
        DomRow *row = rows.at(i);

        QHash<QString, DomProperty*> properties = propertyMap(row->elementProperty());
        DomProperty *text = properties.value(QLatin1String("text"));
        DomProperty *pixmap = properties.value(QLatin1String("pixmap"));

        QString txt = text->elementString()->text();

        if (pixmap != 0) {
            DomResourcePixmap *pix = pixmap->elementPixmap();
            QIcon icon(core()->iconCache()->resolveQrcPath(pix->text(), pix->attributeResource(), workingDirectory()));
            vHeader->setLabel(i, icon, txt);
        } else {
            vHeader->setLabel(i, txt);
        }
    }

    return true;
}

Q3TableExtraInfoFactory::Q3TableExtraInfoFactory(QDesignerFormEditorInterface *core, QExtensionManager *parent)
    : QExtensionFactory(parent), m_core(core)
{}

QObject *Q3TableExtraInfoFactory::createExtension(QObject *object, const QString &iid, QObject *parent) const
{
    if (iid != Q_TYPEID(QDesignerExtraInfoExtension))
        return 0;

    if (Q3Table *w = qobject_cast<Q3Table*>(object))
        return new Q3TableExtraInfo(w, m_core, parent);

    return 0;
}

QT_END_NAMESPACE
