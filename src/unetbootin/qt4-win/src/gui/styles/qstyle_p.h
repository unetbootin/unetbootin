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

#ifndef QSTYLE_P_H
#define QSTYLE_P_H

#include "private/qobject_p.h"

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

class QStylePrivate: public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QStyle)
public:
    inline QStylePrivate()
        : layoutSpacingIndex(-1)
    { }
    mutable int layoutSpacingIndex;
};


#define BEGIN_STYLE_PIXMAPCACHE(a) \
    QRect rect = option->rect; \
    QPixmap internalPixmapCache; \
    QImage imageCache; \
    QPainter *p = painter; \
    QString unique = uniqueName((a), option, option->rect.size()); \
    int txType = painter->deviceTransform().type() | painter->worldTransform().type(); \
    bool doPixmapCache = UsePixmapCache && txType <= QTransform::TxTranslate; \
    if (doPixmapCache && QPixmapCache::find(unique, internalPixmapCache)) { \
        painter->drawPixmap(option->rect.topLeft(), internalPixmapCache); \
    } else { \
        if (doPixmapCache) { \
            rect.setRect(0, 0, option->rect.width(), option->rect.height()); \
            imageCache = QImage(option->rect.size(), QImage::Format_ARGB32_Premultiplied); \
            imageCache.fill(0); \
            p = new QPainter(&imageCache); \
        }


#define END_STYLE_PIXMAPCACHE \
        if (doPixmapCache) { \
            p->end(); \
            delete p; \
            internalPixmapCache = QPixmap::fromImage(imageCache); \
            painter->drawPixmap(option->rect.topLeft(), internalPixmapCache); \
            QPixmapCache::insert(unique, internalPixmapCache); \
        } \
    }

QT_END_NAMESPACE

#endif //QSTYLE_P_H
