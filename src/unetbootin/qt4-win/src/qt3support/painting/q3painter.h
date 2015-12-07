/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt3Support module of the Qt Toolkit.
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

#ifndef Q3PAINTER_H
#define Q3PAINTER_H

#include <QtGui/qpainter.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3SupportLight)

class Q_COMPAT_EXPORT Q3Painter : public QPainter
{
public:
    Q3Painter() : QPainter() { }
    Q3Painter(QPaintDevice *pdev) : QPainter(pdev) { }

    inline void drawRect(const QRect &rect);
    inline void drawRect(int x1, int y1, int w, int h)
    { drawRect(QRect(x1, y1, w, h)); }

    inline void drawRoundRect(const QRect &r, int xround = 25, int yround = 25);
    inline void drawRoundRect(int x, int y, int w, int h, int xround = 25, int yround = 25)
    { drawRoundRect(QRect(x, y, w, h), xround, yround); }

    inline void drawEllipse(const QRect &r);
    inline void drawEllipse(int x, int y, int w, int h)
    { drawEllipse(QRect(x, y, w, h)); }

    inline void drawArc(const QRect &r, int a, int alen);
    inline void drawArc(int x, int y, int w, int h, int a, int alen)
    { drawArc(QRect(x, y, w, h), a, alen); }

    inline void drawPie(const QRect &r, int a, int alen);
    inline void drawPie(int x, int y, int w, int h, int a, int alen)
    { drawPie(QRect(x, y, w, h), a, alen); }

    inline void drawChord(const QRect &r, int a, int alen);
    inline void drawChord(int x, int y, int w, int h, int a, int alen)
    { drawChord(QRect(x, y, w, h), a, alen); }

private:
    QRect adjustedRectangle(const QRect &r);
};

void Q_COMPAT_EXPORT Q3Painter::drawRect(const QRect &r)
{
    QPainter::drawRect(adjustedRectangle(r));
}

void Q_COMPAT_EXPORT Q3Painter::drawEllipse(const QRect &r)
{
    QPainter::drawEllipse(adjustedRectangle(r));
}

void Q_COMPAT_EXPORT Q3Painter::drawRoundRect(const QRect &r, int xrnd, int yrnd)
{
    QPainter::drawRoundRect(adjustedRectangle(r), xrnd, yrnd);
}

void Q_COMPAT_EXPORT Q3Painter::drawArc(const QRect &r, int angle, int arcLength)
{
    QPainter::drawArc(adjustedRectangle(r), angle, arcLength);
}

void Q_COMPAT_EXPORT Q3Painter::drawPie(const QRect &r, int angle, int arcLength)
{
    QPainter::drawPie(adjustedRectangle(r), angle, arcLength);
}

void Q_COMPAT_EXPORT Q3Painter::drawChord(const QRect &r, int angle, int arcLength)
{
    QPainter::drawChord(adjustedRectangle(r), angle, arcLength);
}

QT_END_NAMESPACE

QT_END_HEADER

#endif // Q3PAINTER_H
