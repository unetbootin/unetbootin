/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#ifndef QCOMMONSTYLE_P_H
#define QCOMMONSTYLE_P_H

#include "qcommonstyle.h"
#include "qstyle_p.h"

#include "qstyleoption.h"

QT_BEGIN_NAMESPACE

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of qapplication_*.cpp, qwidget*.cpp and qfiledialog.cpp.  This header
// file may change from version to version without notice, or even be removed.
//
// We mean it.
//


// Private class
class QCommonStylePrivate : public QStylePrivate
{
    Q_DECLARE_PUBLIC(QCommonStyle)
public:
    inline QCommonStylePrivate()
    { }

#ifndef QT_NO_ITEMVIEWS
    void viewItemDrawText(QPainter *p, const QStyleOptionViewItemV4 *option, const QRect &rect) const;
    void viewItemLayout(const QStyleOptionViewItemV4 *opt,  QRect *checkRect,
                        QRect *pixmapRect, QRect *textRect, bool sizehint) const;
    QSize viewItemSize(const QStyleOptionViewItemV4 *option, int role) const;

    mutable QRect decorationRect, displayRect, checkRect;
    mutable QStyleOptionViewItemV4 cachedOption;
    bool isViewItemCached(const QStyleOptionViewItemV4 &option) const {
        return option.rect == cachedOption.rect
               && option.direction == cachedOption.direction
               && option.state == cachedOption.state
               && option.displayAlignment == cachedOption.displayAlignment
               && option.decorationAlignment == cachedOption.decorationAlignment
               && option.decorationPosition == cachedOption.decorationPosition
               && option.decorationSize == cachedOption.decorationSize
               && option.font == cachedOption.font
               && option.features == cachedOption.features
               && option.widget == cachedOption.widget
               && option.index == cachedOption.index
               && option.icon.isNull() == cachedOption.icon.isNull()
               && option.text == cachedOption.text
               && option.viewItemPosition == cachedOption.viewItemPosition;
    }
#endif

};

QT_END_NAMESPACE

#endif //QCOMMONSTYLE_P_H
