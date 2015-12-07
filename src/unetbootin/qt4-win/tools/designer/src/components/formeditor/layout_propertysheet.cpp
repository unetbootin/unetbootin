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

#include "layout_propertysheet.h"

// sdk
#include <QtDesigner/QExtensionManager>

// shared
#include <qlayout_widget_p.h>

#include <QtGui/QFormLayout>

#include <QtCore/QHash>
#include <QtCore/QDebug>

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
        LayoutPropertySizeConstraint
    };
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
}

QT_END_NAMESPACE
