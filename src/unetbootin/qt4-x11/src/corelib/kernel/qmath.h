/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#ifndef QMATH_H
#define QMATH_H

#include <math.h>

#include <QtCore/qglobal.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Core)

inline int qCeil(qreal v)
{
#ifdef QT_USE_MATH_H_FLOATS
    if (sizeof(qreal) == sizeof(float))
        return int(ceilf(v));
    else
#endif
        return int(ceil(v));
}

inline int qFloor(qreal v)
{
#ifdef QT_USE_MATH_H_FLOATS
    if (sizeof(qreal) == sizeof(float))
        return int(floorf(v));
    else
#endif
        return int(floor(v));
}

inline qreal qSin(qreal v)
{
#ifdef QT_USE_MATH_H_FLOATS
    if (sizeof(qreal) == sizeof(float))
        return sinf(v);
    else
#endif
        return sin(v);
}

inline qreal qCos(qreal v)
{
#ifdef QT_USE_MATH_H_FLOATS
    if (sizeof(qreal) == sizeof(float))
        return cosf(v);
    else
#endif
        return cos(v);
}

inline qreal qAcos(qreal v)
{
#ifdef QT_USE_MATH_H_FLOATS
    if (sizeof(qreal) == sizeof(float))
        return acosf(v);
    else
#endif
        return acos(v);
}

inline qreal qSqrt(qreal v)
{
#ifdef QT_USE_MATH_H_FLOATS
    if (sizeof(qreal) == sizeof(float))
        return sqrtf(v);
    else
#endif
        return sqrt(v);
}

inline qreal qLn(qreal v)
{
#ifdef QT_USE_MATH_H_FLOATS
    if (sizeof(qreal) == sizeof(float))
        return logf(v);
    else
#endif
        return log(v);
}

inline qreal qPow(qreal x, qreal y)
{
#ifdef QT_USE_MATH_H_FLOATS
    if (sizeof(qreal) == sizeof(float))
        return powf(x, y);
    else
#endif
        return pow(x, y);
}

QT_END_NAMESPACE

QT_END_HEADER

#endif // QMATH_H
