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

#ifndef QLAYOUTENGINE_P_H
#define QLAYOUTENGINE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "QtGui/qlayoutitem.h"
#include "QtGui/qstyle.h"

QT_BEGIN_NAMESPACE

template <typename T> class QVector;

struct QLayoutStruct
{
    inline void init(int stretchFactor = 0, int minSize = 0) {
        stretch = stretchFactor;
        minimumSize = sizeHint = minSize;
        maximumSize = QLAYOUTSIZE_MAX;
        expansive = false;
        empty = true;
        spacing = 0;
    }

    int smartSizeHint() {
        return (stretch > 0) ? minimumSize : sizeHint;
    }
    int effectiveSpacer(int uniformSpacer) const {
        Q_ASSERT(uniformSpacer >= 0 || spacing >= 0);
        return (uniformSpacer >= 0) ? uniformSpacer : spacing;
    }

    // parameters
    int stretch;
    int sizeHint;
    int maximumSize;
    int minimumSize;
    bool expansive;
    bool empty;
    int spacing;

    // temporary storage
    bool done;

    // result
    int pos;
    int size;
};


Q_GUI_EXPORT void qGeomCalc(QVector<QLayoutStruct> &chain, int start, int count,
                            int pos, int space, int spacer = -1);
Q_GUI_EXPORT QSize qSmartMinSize(const QSize &sizeHint, const QSize &minSizeHint,
                                 const QSize &minSize, const QSize &maxSize,
                                 const QSizePolicy &sizePolicy);
Q_GUI_EXPORT QSize qSmartMinSize(const QWidgetItem *i);
Q_GUI_EXPORT QSize qSmartMinSize(const QWidget *w);
Q_GUI_EXPORT QSize qSmartMaxSize(const QSize &sizeHint,
                                 const QSize &minSize, const QSize &maxSize,
                                 const QSizePolicy &sizePolicy, Qt::Alignment align = 0);
Q_GUI_EXPORT QSize qSmartMaxSize(const QWidgetItem *i, Qt::Alignment align = 0);
Q_GUI_EXPORT QSize qSmartMaxSize(const QWidget *w, Qt::Alignment align = 0);

Q_GUI_EXPORT int qSmartSpacing(const QLayout *layout, QStyle::PixelMetric pm);

/*
  Modify total maximum (max), total expansion (exp), and total empty
  when adding boxmax/boxexp.

  Expansive boxes win over non-expansive boxes.
  Non-empty boxes win over empty boxes.
*/
static inline void qMaxExpCalc(int & max, bool &exp, bool &empty,
                               int boxmax, bool boxexp, bool boxempty)
{
    if (exp) {
        if (boxexp)
            max = qMax(max, boxmax);
    } else {
        if (boxexp || (empty && (!boxempty || max == 0)))
            max = boxmax;
        else if (empty == boxempty)
            max = qMin(max, boxmax);
    }
    exp = exp || boxexp;
    empty = empty && boxempty;
}

QT_END_NAMESPACE

#endif // QLAYOUTENGINE_P_H
