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

#include "layout_propertysheet.h"

// sdk
#include <QtDesigner/QExtensionManager>
#include <QtDesigner/QDesignerFormEditorInterface>

// shared
#include <ui4_p.h>
#include <qlayout_widget_p.h>
#include <formbuilderextra_p.h>

#include <QtGui/QFormLayout>

#include <QtCore/QHash>
#include <QtCore/QDebug>
#include <QtCore/QTextStream>
#include <QtCore/QByteArray>
#include <QtCore/QRegExp> // Remove once there is an editor for lists

QT_BEGIN_NAMESPACE

#define USE_LAYOUT_SIZE_CONSTRAINT

static const char *leftMargin = "leftMargin";
static const char *topMargin = "topMargin";
static const char *rightMargin = "rightMargin";
static const char *bottomMargin = "bottomMargin";
static const char *horizontalSpacing = "horizontalSpacing";
static const char *verticalSpacing = "verticalSpacing";
static const char *spacing = "spacing";
static const char *margin = "margin";
static const char *sizeConstraint = "sizeConstraint";
static const char *boxStretchPropertyC = "stretch";
static const char *gridRowStretchPropertyC = "rowStretch";
static const char *gridColumnStretchPropertyC = "columnStretch";
static const char *gridRowMinimumHeightPropertyC = "rowMinimumHeight";
static const char *gridColumnMinimumWidthPropertyC = "columnMinimumWidth";

namespace {
    enum LayoutPropertyType {
        LayoutPropertyNone,
        LayoutPropertyMargin, // Deprecated
        LayoutPropertyLeftMargin,
        LayoutPropertyTopMargin,
        LayoutPropertyRightMargin,
        LayoutPropertyBottomMargin,
        LayoutPropertySpacing,
        LayoutPropertyHorizontalSpacing,
        LayoutPropertyVerticalSpacing,
        LayoutPropertySizeConstraint,
        LayoutPropertyBoxStretch,
        LayoutPropertyGridRowStretch,
        LayoutPropertyGridColumnStretch,
        LayoutPropertyGridRowMinimumHeight,
        LayoutPropertyGridColumnMinimumWidth
    };
}

// Check for a  comma-separated list of integers. Used for
// per-cell stretch properties and grid per row/column properties.
// As it works now, they are passed as QByteArray strings. The
// property sheet refuses all invalid values. This could be
// replaced by lists once the property editor can handle them.

static bool isIntegerList(const QString &s)
{
    // Check for empty string or comma-separated list of integers
    static const QRegExp re(QLatin1String("[0-9]+(,[0-9]+)+"));
    Q_ASSERT(re.isValid());
    return s.isEmpty() || re.exactMatch(s);
}

// Quick lookup by name
static LayoutPropertyType  layoutPropertyType(const QString &name)
{
    static QHash<QString, LayoutPropertyType> namePropertyMap;
    if (namePropertyMap.empty()) {
        namePropertyMap.insert(QLatin1String(leftMargin), LayoutPropertyLeftMargin);
        namePropertyMap.insert(QLatin1String(topMargin), LayoutPropertyTopMargin);
        namePropertyMap.insert(QLatin1String(rightMargin), LayoutPropertyRightMargin);
        namePropertyMap.insert(QLatin1String(bottomMargin), LayoutPropertyBottomMargin);
        namePropertyMap.insert(QLatin1String(horizontalSpacing), LayoutPropertyHorizontalSpacing);
        namePropertyMap.insert(QLatin1String(verticalSpacing), LayoutPropertyVerticalSpacing);
        namePropertyMap.insert(QLatin1String(spacing), LayoutPropertySpacing);
        namePropertyMap.insert(QLatin1String(margin), LayoutPropertyMargin);
        namePropertyMap.insert(QLatin1String(sizeConstraint), LayoutPropertySizeConstraint);
        namePropertyMap.insert(QLatin1String(boxStretchPropertyC ), LayoutPropertyBoxStretch);
        namePropertyMap.insert(QLatin1String(gridRowStretchPropertyC), LayoutPropertyGridRowStretch);
        namePropertyMap.insert(QLatin1String(gridColumnStretchPropertyC), LayoutPropertyGridColumnStretch);
        namePropertyMap.insert(QLatin1String(gridRowMinimumHeightPropertyC), LayoutPropertyGridRowMinimumHeight);
        namePropertyMap.insert(QLatin1String(gridColumnMinimumWidthPropertyC), LayoutPropertyGridColumnMinimumWidth);
    }
    return namePropertyMap.value(name, LayoutPropertyNone);
}

// return the layout margin if it is  margin
static int getLayoutMargin(const QLayout *l, LayoutPropertyType type)
{
    int left, top, right, bottom;
    l->getContentsMargins(&left, &top, &right, &bottom);
    switch (type) {
    case LayoutPropertyLeftMargin:
        return left;
    case LayoutPropertyTopMargin:
        return top;
    case LayoutPropertyRightMargin:
        return right;
    case LayoutPropertyBottomMargin:
        return bottom;
    default:
        Q_ASSERT(0);
        break;
    }
    return 0;
}

// return the layout margin if it is  margin
static void setLayoutMargin(QLayout *l, LayoutPropertyType type, int margin)
{
    int left, top, right, bottom;
    l->getContentsMargins(&left, &top, &right, &bottom);
    switch (type) {
    case LayoutPropertyLeftMargin:
        left = margin;
        break;
    case LayoutPropertyTopMargin:
        top = margin;
        break;
    case LayoutPropertyRightMargin:
        right = margin;
        break;
    case LayoutPropertyBottomMargin:
        bottom = margin;
        break;
    default:
        Q_ASSERT(0);
        break;
    }
    l->setContentsMargins(left, top, right, bottom);
}

namespace qdesigner_internal {

// ---------- LayoutPropertySheet: This sheet is never visible in
// the property editor. Rather, the sheet pulled for QLayoutWidget
// forwards all properties to it. Some properties (grid spacings) must be handled
// manually, as they are QDOC_PROPERTY only and not visible to introspection. Ditto
// for the 4 margins.

LayoutPropertySheet::LayoutPropertySheet(QLayout *l, QObject *parent)
    : QDesignerPropertySheet(l, parent), m_layout(l)
{
    const QString layoutGroup = QLatin1String("Layout");
    int pindex = createFakeProperty(QLatin1String(leftMargin), 0);
    setPropertyGroup(pindex, layoutGroup);

    pindex = createFakeProperty(QLatin1String(topMargin), 0);
    setPropertyGroup(pindex, layoutGroup);

    pindex = createFakeProperty(QLatin1String(rightMargin), 0);
    setPropertyGroup(pindex, layoutGroup);

    pindex = createFakeProperty(QLatin1String(bottomMargin), 0);
    setPropertyGroup(pindex, layoutGroup);

    const int visibleMask = LayoutProperties::visibleProperties(m_layout);
    if (visibleMask & LayoutProperties::HorizSpacingProperty) {
        pindex = createFakeProperty(QLatin1String(horizontalSpacing), 0);
        setPropertyGroup(pindex, layoutGroup);

        pindex = createFakeProperty(QLatin1String(verticalSpacing), 0);
        setPropertyGroup(pindex, layoutGroup);

        setAttribute(indexOf(QLatin1String(spacing)), true);
    }

    setAttribute(indexOf(QLatin1String(margin)), true);
    // Stretch
    if (visibleMask & LayoutProperties::BoxStretchProperty) {
        pindex = createFakeProperty(QLatin1String(boxStretchPropertyC), QByteArray());
        setPropertyGroup(pindex, layoutGroup);
        setAttribute(pindex, true);
    } else {
        // Add the grid per-row/column stretch and size limits
        if (visibleMask & LayoutProperties::GridColumnStretchProperty) {
            const QByteArray empty;
            pindex = createFakeProperty(QLatin1String(gridRowStretchPropertyC), empty);
            setPropertyGroup(pindex, layoutGroup);
            setAttribute(pindex, true);
            pindex = createFakeProperty(QLatin1String(gridColumnStretchPropertyC), empty);
            setPropertyGroup(pindex, layoutGroup);
            setAttribute(pindex, true);
            pindex = createFakeProperty(QLatin1String(gridRowMinimumHeightPropertyC), empty);
            setPropertyGroup(pindex, layoutGroup);
            setAttribute(pindex, true);
            pindex = createFakeProperty(QLatin1String(gridColumnMinimumWidthPropertyC), empty);
            setPropertyGroup(pindex, layoutGroup);
            setAttribute(pindex, true);
        }
    }
#ifdef USE_LAYOUT_SIZE_CONSTRAINT
    // SizeConstraint cannot possibly be handled as a real property
    // as it affects the layout parent widget and thus
    // conflicts with Designer's special layout widget.
    // It will take effect on the preview only.
    pindex = createFakeProperty(QLatin1String(sizeConstraint));
    setPropertyGroup(pindex, layoutGroup);
#endif
}

LayoutPropertySheet::~LayoutPropertySheet()
{
}

void LayoutPropertySheet::setProperty(int index, const QVariant &value)
{
    const LayoutPropertyType type = layoutPropertyType(propertyName(index));
    if (QLayoutWidget *lw = qobject_cast<QLayoutWidget *>(m_layout->parent())) {
        switch (type) {
        case LayoutPropertyLeftMargin:
            lw->setLayoutLeftMargin(value.toInt());
            return;
        case LayoutPropertyTopMargin:
            lw->setLayoutTopMargin(value.toInt());
            return;
        case LayoutPropertyRightMargin:
            lw->setLayoutRightMargin(value.toInt());
            return;
        case LayoutPropertyBottomMargin:
            lw->setLayoutBottomMargin(value.toInt());
            return;
        case LayoutPropertyMargin: {
            const int v = value.toInt();
            lw->setLayoutLeftMargin(v);
            lw->setLayoutTopMargin(v);
            lw->setLayoutRightMargin(v);
            lw->setLayoutBottomMargin(v);
        }
            return;
        default:
            break;
        }
    }
    switch (type) {
    case LayoutPropertyLeftMargin:
    case LayoutPropertyTopMargin:
    case LayoutPropertyRightMargin:
    case LayoutPropertyBottomMargin:
        setLayoutMargin(m_layout, type, value.toInt());
        return;
    case LayoutPropertyHorizontalSpacing:
        if (QGridLayout *grid = qobject_cast<QGridLayout *>(m_layout)) {
            grid->setHorizontalSpacing(value.toInt());
            return;
        }
        if (QFormLayout *form = qobject_cast<QFormLayout *>(m_layout)) {
            form->setHorizontalSpacing(value.toInt());
            return;
        }
        break;
    case LayoutPropertyVerticalSpacing:
        if (QGridLayout *grid = qobject_cast<QGridLayout *>(m_layout)) {
            grid->setVerticalSpacing(value.toInt());
            return;
        }
        if (QFormLayout *form = qobject_cast<QFormLayout *>(m_layout)) {
            form->setVerticalSpacing(value.toInt());
            return;
        }
        break;
    case LayoutPropertyBoxStretch:
        // TODO: Remove the regexp check once a proper editor for integer
        // lists is in place?
        if (QBoxLayout *box = qobject_cast<QBoxLayout *>(m_layout)) {
            const QString stretch = value.toString();
            if (isIntegerList(stretch))
                QFormBuilderExtra::setBoxLayoutStretch(value.toString(), box);
        }
        break;
    case LayoutPropertyGridRowStretch:
        if (QGridLayout *grid = qobject_cast<QGridLayout *>(m_layout)) {
            const QString stretch = value.toString();
            if (isIntegerList(stretch))
                QFormBuilderExtra::setGridLayoutRowStretch(stretch, grid);
        }
        break;
    case LayoutPropertyGridColumnStretch:
        if (QGridLayout *grid = qobject_cast<QGridLayout *>(m_layout)) {
            const QString stretch = value.toString();
            if (isIntegerList(stretch))
                QFormBuilderExtra::setGridLayoutColumnStretch(value.toString(), grid);
        }
        break;
    case LayoutPropertyGridRowMinimumHeight:
        if (QGridLayout *grid = qobject_cast<QGridLayout *>(m_layout)) {
            const QString minSize = value.toString();
            if (isIntegerList(minSize))
                QFormBuilderExtra::setGridLayoutRowMinimumHeight(minSize, grid);
        }
        break;
    case LayoutPropertyGridColumnMinimumWidth:
        if (QGridLayout *grid = qobject_cast<QGridLayout *>(m_layout)) {
            const QString minSize = value.toString();
            if (isIntegerList(minSize))
                QFormBuilderExtra::setGridLayoutColumnMinimumWidth(minSize, grid);
        }
        break;
    default:
        break;
    }
    QDesignerPropertySheet::setProperty(index, value);
}

QVariant LayoutPropertySheet::property(int index) const
{
    const LayoutPropertyType type = layoutPropertyType(propertyName(index));
    if (const QLayoutWidget *lw = qobject_cast<QLayoutWidget *>(m_layout->parent())) {
        switch (type) {
        case LayoutPropertyLeftMargin:
            return lw->layoutLeftMargin();
        case LayoutPropertyTopMargin:
            return lw->layoutTopMargin();
        case LayoutPropertyRightMargin:
            return lw->layoutRightMargin();
        case LayoutPropertyBottomMargin:
             return lw->layoutBottomMargin();
        default:
            break;
        }
    }
    switch (type) {
    case LayoutPropertyLeftMargin:
    case LayoutPropertyTopMargin:
    case LayoutPropertyRightMargin:
    case LayoutPropertyBottomMargin:
        return getLayoutMargin(m_layout, type);
    case LayoutPropertyHorizontalSpacing:
        if (const QGridLayout *grid = qobject_cast<QGridLayout *>(m_layout))
            return grid->horizontalSpacing();
        if (const QFormLayout *form = qobject_cast<QFormLayout *>(m_layout))
            return form->horizontalSpacing();
        break;
    case LayoutPropertyVerticalSpacing:
        if (const QGridLayout *grid = qobject_cast<QGridLayout *>(m_layout))
            return grid->verticalSpacing();
        if (const QFormLayout *form = qobject_cast<QFormLayout *>(m_layout))
            return form->verticalSpacing();
    case LayoutPropertyBoxStretch:
        if (const QBoxLayout *box = qobject_cast<QBoxLayout *>(m_layout))
            return QVariant(QByteArray(QFormBuilderExtra::boxLayoutStretch(box).toUtf8()));
        break;
    case LayoutPropertyGridRowStretch:
        if (const QGridLayout *grid = qobject_cast<QGridLayout *>(m_layout))
            return QVariant(QByteArray(QFormBuilderExtra::gridLayoutRowStretch(grid).toUtf8()));
        break;
    case LayoutPropertyGridColumnStretch:
        if (const QGridLayout *grid = qobject_cast<QGridLayout *>(m_layout))
            return QVariant(QByteArray(QFormBuilderExtra::gridLayoutColumnStretch(grid).toUtf8()));
        break;
    case LayoutPropertyGridRowMinimumHeight:
        if (const QGridLayout *grid = qobject_cast<QGridLayout *>(m_layout))
            return QVariant(QByteArray(QFormBuilderExtra::gridLayoutRowMinimumHeight(grid).toUtf8()));
        break;
    case LayoutPropertyGridColumnMinimumWidth:
        if (const QGridLayout *grid = qobject_cast<QGridLayout *>(m_layout))
            return QVariant(QByteArray(QFormBuilderExtra::gridLayoutColumnMinimumWidth(grid).toUtf8()));
        break;
    default:
        break;
    }
    return QDesignerPropertySheet::property(index);
}

bool LayoutPropertySheet::reset(int index)
{
    int left, top, right, bottom;
    m_layout->getContentsMargins(&left, &top, &right, &bottom);
    const LayoutPropertyType type = layoutPropertyType(propertyName(index));
    bool rc = true;
    switch (type) {
    case LayoutPropertyLeftMargin:
        m_layout->setContentsMargins(-1, top, right, bottom);
        break;
    case LayoutPropertyTopMargin:
        m_layout->setContentsMargins(left, -1, right, bottom);
        break;
    case LayoutPropertyRightMargin:
        m_layout->setContentsMargins(left, top, -1, bottom);
        break;
    case LayoutPropertyBottomMargin:
        m_layout->setContentsMargins(left, top, right, -1);
        break;
    case LayoutPropertyBoxStretch:
        if (QBoxLayout *box = qobject_cast<QBoxLayout *>(m_layout))
            QFormBuilderExtra::clearBoxLayoutStretch(box);
        break;
    case LayoutPropertyGridRowStretch:
        if (QGridLayout *grid = qobject_cast<QGridLayout *>(m_layout))
            QFormBuilderExtra::clearGridLayoutRowStretch(grid);
        break;
    case LayoutPropertyGridColumnStretch:
        if (QGridLayout *grid = qobject_cast<QGridLayout *>(m_layout))
            QFormBuilderExtra::clearGridLayoutColumnStretch(grid);
        break;
    case LayoutPropertyGridRowMinimumHeight:
        if (QGridLayout *grid = qobject_cast<QGridLayout *>(m_layout))
            QFormBuilderExtra::clearGridLayoutRowMinimumHeight(grid);
        break;
    case LayoutPropertyGridColumnMinimumWidth:
        if (QGridLayout *grid = qobject_cast<QGridLayout *>(m_layout))
            QFormBuilderExtra::clearGridLayoutColumnMinimumWidth(grid);
        break;
    default:
        rc = QDesignerPropertySheet::reset(index);
        break;
    }
    return rc;
}

void LayoutPropertySheet::setChanged(int index, bool changed)
{
    const LayoutPropertyType type = layoutPropertyType(propertyName(index));
    switch (type) {
    case LayoutPropertySpacing:
        if (LayoutProperties::visibleProperties(m_layout) & LayoutProperties::HorizSpacingProperty) {
            setChanged(indexOf(QLatin1String(horizontalSpacing)), changed);
            setChanged(indexOf(QLatin1String(verticalSpacing)), changed);
        }
        break;
    case LayoutPropertyMargin:
        setChanged(indexOf(QLatin1String(leftMargin)), changed);
        setChanged(indexOf(QLatin1String(topMargin)), changed);
        setChanged(indexOf(QLatin1String(rightMargin)), changed);
        setChanged(indexOf(QLatin1String(bottomMargin)), changed);
        break;
    default:
        break;
    }
    QDesignerPropertySheet::setChanged(index, changed);
}

void LayoutPropertySheet::stretchAttributesToDom(QDesignerFormEditorInterface *core, QLayout *lt, DomLayout *domLayout)
{
    // Check if the respective stretch properties of the layout are changed.
    // If so, set them to the DOM
    const int visibleMask = LayoutProperties::visibleProperties(lt);
    if (!(visibleMask & (LayoutProperties::BoxStretchProperty|LayoutProperties::GridColumnStretchProperty|LayoutProperties::GridRowStretchProperty)))
        return;
    const QDesignerPropertySheetExtension *sheet = qt_extension<QDesignerPropertySheetExtension*>(core->extensionManager(), lt);
    Q_ASSERT(sheet);

    // Stretch
    if (visibleMask & LayoutProperties::BoxStretchProperty) {
        const int index = sheet->indexOf(QLatin1String(boxStretchPropertyC));
        Q_ASSERT(index != -1);
        if (sheet->isChanged(index))
            domLayout->setAttributeStretch(sheet->property(index).toString());
    }
    if (visibleMask & LayoutProperties::GridColumnStretchProperty) {
        const int index = sheet->indexOf(QLatin1String(gridColumnStretchPropertyC));
        Q_ASSERT(index != -1);
        if (sheet->isChanged(index))
            domLayout->setAttributeColumnStretch(sheet->property(index).toString());
    }
    if (visibleMask & LayoutProperties::GridRowStretchProperty) {
        const int index = sheet->indexOf(QLatin1String(gridRowStretchPropertyC));
        Q_ASSERT(index != -1);
        if (sheet->isChanged(index))
            domLayout->setAttributeRowStretch(sheet->property(index).toString());
    }
    if (visibleMask & LayoutProperties::GridRowMinimumHeightProperty) {
        const int index = sheet->indexOf(QLatin1String(gridRowMinimumHeightPropertyC));
        Q_ASSERT(index != -1);
        if (sheet->isChanged(index))
            domLayout->setAttributeRowMinimumHeight(sheet->property(index).toString());
    }
    if (visibleMask & LayoutProperties::GridColumnMinimumWidthProperty) {
        const int index = sheet->indexOf(QLatin1String(gridColumnMinimumWidthPropertyC));
        Q_ASSERT(index != -1);
        if (sheet->isChanged(index))
            domLayout->setAttributeColumnMinimumWidth(sheet->property(index).toString());
    }
}

void LayoutPropertySheet::markChangedStretchProperties(QDesignerFormEditorInterface *core, QLayout *lt, const DomLayout *domLayout)
{
    // While the actual values are applied by the form builder, we still need
    // to mark them as 'changed'.
    QDesignerPropertySheetExtension *sheet = qt_extension<QDesignerPropertySheetExtension*>(core->extensionManager(), lt);
    Q_ASSERT(sheet);
    if (!domLayout->attributeStretch().isEmpty())
        sheet->setChanged(sheet->indexOf(QLatin1String(boxStretchPropertyC)), true);
    if (!domLayout->attributeRowStretch().isEmpty())
        sheet->setChanged(sheet->indexOf(QLatin1String(gridRowStretchPropertyC)), true);
    if (!domLayout->attributeColumnStretch().isEmpty())
        sheet->setChanged(sheet->indexOf(QLatin1String(gridColumnStretchPropertyC)), true);
   if (!domLayout->attributeColumnMinimumWidth().isEmpty())
        sheet->setChanged(sheet->indexOf(QLatin1String(gridColumnMinimumWidthPropertyC)), true);
   if (!domLayout->attributeRowMinimumHeight().isEmpty())
        sheet->setChanged(sheet->indexOf(QLatin1String(gridRowMinimumHeightPropertyC)), true);
}

}

QT_END_NAMESPACE
