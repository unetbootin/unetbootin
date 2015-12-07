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

#include "itemview_propertysheet.h"

#include <QtDesigner/QDesignerFormEditorInterface>

#include <QtGui/QAbstractItemView>
#include <QtGui/QHeaderView>

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {

struct Property {
    Property() : m_sheet(0) {}
    Property(QDesignerPropertySheetExtension *sheet, int id)
        : m_sheet(sheet), m_id(id) {}
    bool operator==(const Property &p) { return m_sheet == p.m_sheet && m_id == p.m_id; }
    uint qHash() {
        return ((int)(m_sheet-(QDesignerPropertySheetExtension*)(0))) & m_id;
    }

    QDesignerPropertySheetExtension *m_sheet;
    int m_id;
};

class ItemViewPropertySheetPrivate {

public:
    ItemViewPropertySheetPrivate(QHeaderView *horizontalHeader,
                                 QHeaderView *verticalHeader,
                                 QObject *parent);

    inline void createMapping(int fakeId, QHeaderView *header, const QString &headerName);
    inline QStringList realPropertyNames();
    inline QString fakePropertyName(const QString &prefix, const QString &realName);

    QDesignerFormEditorInterface *m_core;

    // Maps index of fake property
    // to index of real property in respective sheet
    QHash<int, Property> m_propertyIdMap;

    // Maps name of fake property
    // to name of real property
    QHash<QString, QString> m_propertyNameMap;

private:
    static QDesignerFormEditorInterface *formEditorForObject(QObject *o);

    QHeaderView *m_hHeader;
    QHeaderView *m_vHeader;
    QHash<QHeaderView *, QDesignerPropertySheetExtension *> m_propertySheet;
    QStringList m_realPropertyNames;
};

// Name of the fake group
static const char *headerGroup = "Header";

// Name of the real properties
static const char *visibleProperty = "visible";
static const char *cascadingSectionResizesProperty = "cascadingSectionResizes";
static const char *defaultSectionSizeProperty = "defaultSectionSize";
static const char *highlightSectionsProperty = "highlightSections";
static const char *minimumSectionSizeProperty = "minimumSectionSize";
static const char *showSortIndicatorProperty = "showSortIndicator";
static const char *stretchLastSectionProperty = "stretchLastSection";
} // namespace qdesigner_internal

using namespace qdesigner_internal;


/***************** ItemViewPropertySheetPrivate *********************/

ItemViewPropertySheetPrivate::ItemViewPropertySheetPrivate(QHeaderView *horizontalHeader,
                                                           QHeaderView *verticalHeader,
                                                           QObject *parent)
        : m_core(formEditorForObject(parent)),
          m_hHeader(horizontalHeader),
          m_vHeader(verticalHeader)
{
    if (horizontalHeader)
        m_propertySheet.insert(horizontalHeader,
                               qt_extension<QDesignerPropertySheetExtension*>
                               (m_core->extensionManager(), horizontalHeader));
    if (verticalHeader)
        m_propertySheet.insert(verticalHeader,
                               qt_extension<QDesignerPropertySheetExtension*>
                               (m_core->extensionManager(), verticalHeader));
}

// Find the form editor in the hierarchy.
// We know that the parent of the sheet is the extension manager
// whose parent is the core.
QDesignerFormEditorInterface *ItemViewPropertySheetPrivate::formEditorForObject(QObject *o)
{
    do {
        if (QDesignerFormEditorInterface* core = qobject_cast<QDesignerFormEditorInterface*>(o))
            return core;
        o = o->parent();
    } while(o);
    Q_ASSERT(o);
    return 0;
}

void ItemViewPropertySheetPrivate::createMapping(int fakeId, QHeaderView *header,
                                                 const QString &headerName)
{
    const int realPropertyId = m_propertySheet.value(header)->indexOf(headerName);
    QDesignerPropertySheetExtension *propertySheet = m_propertySheet.value(header);
    m_propertyIdMap.insert(fakeId, Property(propertySheet, realPropertyId));
}

QStringList ItemViewPropertySheetPrivate::realPropertyNames()
{
    if (m_realPropertyNames.isEmpty())
        m_realPropertyNames
            << QLatin1String(visibleProperty)
            << QLatin1String(cascadingSectionResizesProperty)
            << QLatin1String(defaultSectionSizeProperty)
            << QLatin1String(highlightSectionsProperty)
            << QLatin1String(minimumSectionSizeProperty)
            << QLatin1String(showSortIndicatorProperty)
            << QLatin1String(stretchLastSectionProperty);
    return m_realPropertyNames;
}

QString ItemViewPropertySheetPrivate::fakePropertyName(const QString &prefix,
                                                       const QString &realName)
{
    // prefix = "header", realPropertyName = "isVisible" returns "headerIsVisible"
    QString fakeName = prefix + realName.at(0).toUpper() + realName.mid(1);
    m_propertyNameMap.insert(fakeName, realName);
    return fakeName;
}

/***************** ItemViewPropertySheet *********************/

/*!
  \class qdesigner_internal::ItemViewPropertySheet

  \brief
    Adds header fake properties to QTreeView and QTableView objects

    QHeaderView objects are currently not shown in the object inspector.
    This class adds some fake properties to the property sheet
    of QTreeView and QTableView objects that nevertheless allow the manipulation
    of the headers attached to the item view object.

    Currently the defaultAlignment property is not shown because the property sheet
    would only show integers, instead of the Qt::Alignment enumeration.

    The fake properties here need special handling in QDesignerResource, uiloader and uic.
  */

ItemViewPropertySheet::ItemViewPropertySheet(QTreeView *treeViewObject, QObject *parent)
        : QDesignerPropertySheet(treeViewObject, parent),
        d(new ItemViewPropertySheetPrivate(treeViewObject->header(), 0, parent))
{
    QHeaderView *hHeader = treeViewObject->header();

    foreach (const QString &realPropertyName, d->realPropertyNames()) {
        const QString fakePropertyName
                = d->fakePropertyName(QLatin1String("header"), realPropertyName);
        d->createMapping(createFakeProperty(fakePropertyName, 0), hHeader, realPropertyName);
    }

    foreach (int id, d->m_propertyIdMap.keys()) {
        setAttribute(id, true);
        setPropertyGroup(id, QLatin1String(headerGroup));
    }
}


ItemViewPropertySheet::ItemViewPropertySheet(QTableView *tableViewObject, QObject *parent)
        : QDesignerPropertySheet(tableViewObject, parent),
        d(new ItemViewPropertySheetPrivate(tableViewObject->horizontalHeader(),
                                           tableViewObject->verticalHeader(), parent))
{
    QHeaderView *hHeader = tableViewObject->horizontalHeader();
    QHeaderView *vHeader = tableViewObject->verticalHeader();

    foreach (const QString &realPropertyName, d->realPropertyNames()) {
        const QString fakePropertyName
                = d->fakePropertyName(QLatin1String("horizontalHeader"), realPropertyName);
        d->createMapping(createFakeProperty(fakePropertyName, 0), hHeader, realPropertyName);
    }
    foreach (const QString &realPropertyName, d->realPropertyNames()) {
        const QString fakePropertyName
                = d->fakePropertyName(QLatin1String("verticalHeader"), realPropertyName);
        d->createMapping(createFakeProperty(fakePropertyName, 0), vHeader, realPropertyName);
    }

    foreach (int id, d->m_propertyIdMap.keys()) {
        setAttribute(id, true);
        setPropertyGroup(id, QLatin1String(headerGroup));
    }
}

ItemViewPropertySheet::~ItemViewPropertySheet()
{
    delete d;
}

/*!
  Returns the mapping of fake property names to real property names
  */
QHash<QString,QString> ItemViewPropertySheet::propertyNameMap() const
{
    return d->m_propertyNameMap;
}

QVariant ItemViewPropertySheet::property(int index) const
{
    if (d->m_propertyIdMap.contains(index)) {
        Property realProperty = d->m_propertyIdMap.value(index);
        return realProperty.m_sheet->property(realProperty.m_id);
    } else {
        return QDesignerPropertySheet::property(index);
    }
}

void ItemViewPropertySheet::setProperty(int index, const QVariant &value)
{
    if (d->m_propertyIdMap.contains(index)) {
        Property realProperty = d->m_propertyIdMap.value(index);
        realProperty.m_sheet->setProperty(realProperty.m_id, value);
    } else {
        QDesignerPropertySheet::setProperty(index, value);
    }
}

void ItemViewPropertySheet::setChanged(int index, bool changed)
{
    if (d->m_propertyIdMap.contains(index)) {
        Property realProperty = d->m_propertyIdMap.value(index);
        realProperty.m_sheet->setChanged(realProperty.m_id, changed);
    }
    QDesignerPropertySheet::setChanged(index, changed);
}

bool ItemViewPropertySheet::reset(int index)
{
    if (d->m_propertyIdMap.contains(index)) {
        Property realProperty = d->m_propertyIdMap.value(index);
        return realProperty.m_sheet->reset(realProperty.m_id);
    } else {
        return QDesignerPropertySheet::reset(index);
    }
}

QT_END_NAMESPACE
