/****************************************************************************
**
** Copyright (C) 2004-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
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

#include <QtGui>

#include "pixeldelegate.h"

//! [0]
PixelDelegate::PixelDelegate(QObject *parent)
    : QAbstractItemDelegate(parent)
{
    pixelSize = 12;
}
//! [0]

//! [1]
void PixelDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const
{
//! [2]
    if (option.state & QStyle::State_Selected)
        painter->fillRect(option.rect, option.palette.highlight());
//! [1]

//! [3]
    int size = qMin(option.rect.width(), option.rect.height());
//! [3] //! [4]
    int brightness = index.model()->data(index, Qt::DisplayRole).toInt();
    double radius = (size/2.0) - (brightness/255.0 * size/2.0);
    if (radius == 0.0)
        return;
//! [4]

//! [5]
    painter->save();
//! [5] //! [6]
    painter->setRenderHint(QPainter::Antialiasing, true);
//! [6] //! [7]
    painter->setPen(Qt::NoPen);
//! [7] //! [8]
    if (option.state & QStyle::State_Selected)
//! [8] //! [9]
        painter->setBrush(option.palette.highlightedText());
    else
//! [2]
        painter->setBrush(QBrush(Qt::black));
//! [9]

//! [10]
    painter->drawEllipse(QRectF(option.rect.x() + option.rect.width()/2 - radius,
                                option.rect.y() + option.rect.height()/2 - radius,
                                2*radius, 2*radius));
    painter->restore();
}
//! [10]

//! [11]
QSize PixelDelegate::sizeHint(const QStyleOptionViewItem & /* option */,
                              const QModelIndex & /* index */) const
{
    return QSize(pixelSize, pixelSize);
}
//! [11]

//! [12]
void PixelDelegate::setPixelSize(int size)
{
    pixelSize = size;
}
//! [12]
