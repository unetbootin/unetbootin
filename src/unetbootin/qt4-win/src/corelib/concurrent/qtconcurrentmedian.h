/****************************************************************************
**
** Copyright (C) 2005-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#ifndef QTCONCURRENT_MEDIAN_H
#define QTCONCURRENT_MEDIAN_H

#include <QtCore/qglobal.h>

#ifndef QT_NO_CONCURRENT

#include <QtCore/qvector.h>
#include <QtCore/qalgorithms.h>

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

QT_MODULE(Core)

#ifndef qdoc

namespace QtConcurrent {

template <typename T>
class Median
{
public:
    Median(int _bufferSize)
        : currentMedian(), bufferSize(_bufferSize), currentIndex(0), valid(false), dirty(true)
    {
        values.resize(bufferSize);
    }

    void reset()
    {
        values.fill(0);
        currentIndex = 0;
        valid = false;
        dirty = true;
    }

    void addValue(T value)
    {
        currentIndex = ((currentIndex + 1) % bufferSize);
        if (valid == false && currentIndex % bufferSize == 0)
            valid = true;

        // Only update the cached median value when we have to, that
        // is when the new value is on then other side of the median
        // compared to the current value at the index.
        const T currentIndexValue = values[currentIndex];
        if (currentIndexValue > currentMedian && currentMedian > value ||
            currentMedian > currentIndexValue && value > currentMedian) {
            dirty = true;
        }

        values[currentIndex] = value;
    }

    bool isMedianValid() const
    {
        return valid;
    }

    T median()
    {
        if (dirty) {
            dirty = false;
            QVector<T> sorted = values;
            qSort(sorted);
            currentMedian = sorted.at(bufferSize / 2 + 1);
        }
        return currentMedian;
    }
private:
    QVector<T> values;
    T currentMedian;
    int bufferSize;
    int currentIndex;
    bool valid;
    bool dirty;
};

} // namespace QtConcurrent

#endif //qdoc

QT_END_NAMESPACE
QT_END_HEADER

#endif // QT_NO_CONCURRENT

#endif
