/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
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

#include "qdesigner_widgetitem_p.h"
#include "qdesigner_widget_p.h"

#include <QtGui/QFrame>
#include <QtGui/QBoxLayout>
#include <QtCore/QDebug>
#include <QtCore/QMetaObject>
#include <private/qlayout_p.h>

QT_BEGIN_NAMESPACE

enum { DebugWidgetItem = 0 };
enum { MinimumLength = 10 };

// Widget item creation function to be registered as factory method with QLayoutPrivate
static QWidgetItem *createDesignerWidgetItem(const QLayout *layout, QWidget *widget)
{
    const QMetaObject *mo = widget->metaObject();
    // Only for QWidgets and QFrames within designer
    if (mo == &QWidget::staticMetaObject || mo == &QFrame::staticMetaObject || mo == &QDesignerWidget::staticMetaObject) {
        Qt::Orientations o = Qt::Horizontal|Qt::Vertical;
        // If it is a box, restrict to its orientation
        if (const QBoxLayout *bl = qobject_cast<const QBoxLayout *>(layout)) {
            const QBoxLayout::Direction direction = bl->direction();
            o = direction == QBoxLayout::LeftToRight || direction == QBoxLayout::RightToLeft ? Qt::Horizontal : Qt::Vertical;
        }
        if (DebugWidgetItem)
            qDebug() << "QDesignerWidgetItem: Creating on " << widget;
        return new qdesigner_internal::QDesignerWidgetItem(widget, o);
    }
    return 0;
}

namespace qdesigner_internal {

// ------------------ QDesignerWidgetItem
QDesignerWidgetItem::QDesignerWidgetItem(QWidget *w, Qt::Orientations o) :
    QWidgetItemV2(w),
    m_orientations(o)
{
}

QSize QDesignerWidgetItem::expand(const QSize &s) const
{
    // Expand the size if its too small
    QSize rc = s;
    if (m_orientations & Qt::Horizontal && rc.width() <= 0)
        rc.setWidth(MinimumLength);
    if (m_orientations & Qt::Vertical && rc.height() <= 0)
        rc.setHeight(MinimumLength);
    return rc;
}

void QDesignerWidgetItem::install()
{
    QLayoutPrivate::widgetItemFactoryMethod = createDesignerWidgetItem;
}

void QDesignerWidgetItem::deinstall()
{
    QLayoutPrivate::widgetItemFactoryMethod = 0;
}

// ------------------   QDesignerWidgetItemInstaller

int QDesignerWidgetItemInstaller::m_instanceCount = 0;

QDesignerWidgetItemInstaller::QDesignerWidgetItemInstaller()
{
    if (m_instanceCount++ == 0) {
        if (DebugWidgetItem)
            qDebug() << "QDesignerWidgetItemInstaller: installing";
        QDesignerWidgetItem::install();
    }

}

QDesignerWidgetItemInstaller::~QDesignerWidgetItemInstaller()
{
    if (--m_instanceCount == 0) {
        if (DebugWidgetItem)
            qDebug() << "QDesignerWidgetItemInstaller: deinstalling";
        QDesignerWidgetItem::deinstall();
    }
}

}

QT_END_NAMESPACE
