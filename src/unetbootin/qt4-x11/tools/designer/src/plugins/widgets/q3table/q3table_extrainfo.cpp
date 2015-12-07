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
