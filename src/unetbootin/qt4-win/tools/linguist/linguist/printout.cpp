/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt Linguist of the Qt Toolkit.
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

#include "printout.h"

#include <QPrinter>
#include <QFontMetrics>

QT_BEGIN_NAMESPACE

PrintOut::PrintOut( QPrinter *printer )
    : pr( printer ), nextRule( NoRule ), page( 0 )
{
    p.begin( pr );
    QFont f( QLatin1String("Arial") );
    f8 = f;
    f8.setPointSize( 8 );
    f10 = f;
    f10.setPointSize( 10 );
    p.setFont( f10 );
    fmetrics = new QFontMetrics( p.fontMetrics() );
    hmargin = 5 * printer->width() / printer->widthMM(); // 5 mm
    vmargin = 5 * printer->height() / printer->heightMM(); // 5 mm
    hsize = printer->width() - 2 * hmargin;
    vsize = printer->height() - vmargin;
    dateTime = QDateTime::currentDateTime();
    breakPage(true); // init vsize and draw first header
    cp = Paragraph( QPoint(hmargin, voffset) );
}

PrintOut::~PrintOut()
{
    flushLine();
    delete fmetrics;
    p.end();
}

void PrintOut::setRule( Rule rule )
{
    if ( (int) nextRule < (int) rule )
        nextRule = rule;
}

void PrintOut::setGuide( const QString& guide )
{
    g = guide;
}

void PrintOut::vskip()
{
    if ( !firstParagraph )
        voffset += 14;
}

void PrintOut::flushLine( bool /* mayBreak */ )
{
    if ( voffset + cp.rect.height() > vsize )
        breakPage();
    else if ( !firstParagraph )
        drawRule( nextRule );

    for ( int i = 0; i < (int) cp.boxes.count(); i++ ) {
        Box b = cp.boxes[i];
        b.rect.translate( 0, voffset );
        QRect r = b.rect;
        p.setFont( b.font );
        p.drawText( r, b.align, b.text );
    }
    voffset += cp.rect.height();

    nextRule = NoRule;
    cp = Paragraph( QPoint(hmargin, voffset) );
    firstParagraph = false;
}

void PrintOut::addBox( int percent, const QString& text, Style style,
                       int halign )
{
    int align = halign | Qt::AlignTop;
    QFont f = f10;
    if ( style == Strong )
        f.setBold( true );
    else if ( style == Emphasis )
        f.setItalic( true );
    int wd = hsize * percent / 100;
    QRect r( cp.rect.x() + cp.rect.width(), 0, wd, vsize );
    int ht = p.boundingRect( r, align, text ).height();

    Box b( r, text, f, align );
    cp.boxes.append( b );
    cp.rect.setSize( QSize(cp.rect.width() + wd, qMax(cp.rect.height(), ht)) );
}

// use init if inital vsize should be calculated (first breakPage call)
void PrintOut::breakPage(bool init)
{
    static const int LeftAlign = Qt::AlignLeft | Qt::AlignTop;
    static const int RightAlign = Qt::AlignRight | Qt::AlignTop;
    QRect r1, r2;
    int h1 = 0;
    int h2 = 0;

    if (page > 0)
        pr->newPage();

    if (!init)
        page++;

    voffset = 0;

    p.setFont( f10 );
    r1 = QRect( hmargin, voffset, 3 * hsize / 4, vsize );
    r2 = QRect( r1.x() + r1.width(), voffset, hsize - r1.width(), vsize );
    h1 = p.boundingRect( r1, LeftAlign, pr->docName() ).height();
    if (!init)
        p.drawText( r1, LeftAlign, pr->docName() );
    h2 = p.boundingRect( r2, RightAlign, QString::number(page) ).height();
    if (!init)
        p.drawText( r2, RightAlign, QString::number(page) );
    voffset += qMax( h1, h2 );

    r1 = QRect( hmargin, voffset, hsize / 2, LeftAlign );
    p.setFont( f8 );
    h1 = p.boundingRect( r1, LeftAlign, dateTime.toString() ).height();
    if (!init)
        p.drawText ( r1, LeftAlign, dateTime.toString() );
    p.setFont( f10 );
    voffset += qMax( h1, h2 );

    voffset += 4;
    if (!init)
        p.drawLine( QPoint(hmargin, voffset), QPoint(hmargin + hsize, voffset) );
    voffset += 14;

    firstParagraph = true;

    if (init)
    {
        vsize -= voffset;
        breakPage(); // now draw it when the vsize is ok
    }

}

void PrintOut::drawRule( Rule rule )
{
    QPen pen;

    switch ( rule ) {
    case NoRule:
        voffset += 5;
        break;
    case ThinRule:
        pen.setColor( QColor(192, 192, 192) );
        pen.setStyle( Qt::DotLine );
        pen.setWidth( 0 );
        p.setPen( pen );
        voffset += 5;
        p.drawLine( QPoint(hmargin, voffset),
                    QPoint(hmargin + hsize, voffset) );
        p.setPen( QPen() );
        voffset += 2;
        break;
    case ThickRule:
        voffset += 7;
        p.drawLine( QPoint(hmargin, voffset),
                    QPoint(hmargin + hsize, voffset) );
        voffset += 4;
    }
}

QT_END_NAMESPACE
