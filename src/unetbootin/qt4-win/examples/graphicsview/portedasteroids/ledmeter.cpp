/****************************************************************************
**
** Copyright (C) 2006-2008 Trolltech ASA. All rights reserved.
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

/*
 * KAsteroids - Copyright (c) Martin R. Jones 1997
 *
 * Part of the KDE project
 */

#include <qpainter.h>
//Added by qt3to4:
#include <QResizeEvent>
#include <Q3Frame>
#include "ledmeter.h"

KALedMeter::KALedMeter( QWidget *parent ) : Q3Frame( parent )
{
    mCRanges.setAutoDelete( TRUE );
    mRange = 100;
    mCount = 20;
    mCurrentCount = 0;
    mValue = 0;
    setMinimumWidth( mCount * 2 + frameWidth() );
}

void KALedMeter::setRange( int r )
{
    mRange = r;
    if ( mRange < 1 )
        mRange = 1;
    setValue( mValue );
    update();
}

void KALedMeter::setCount( int c )
{
    mCount = c;
    if ( mCount < 1 )
        mCount = 1;
    setMinimumWidth( mCount * 2 + frameWidth() );
    calcColorRanges();
    setValue( mValue );
    update();
}

void KALedMeter::setValue( int v )
{
    mValue = v;
    if ( mValue > mRange )
        mValue = mRange;
    else if ( mValue < 0 )
        mValue = 0;
    int c = ( mValue + mRange / mCount - 1 ) * mCount / mRange;
    if ( c != mCurrentCount )
    {
        mCurrentCount = c;
        update();
    }
}

void KALedMeter::addColorRange( int pc, const QColor &c )
{
    ColorRange *cr = new ColorRange;
    cr->mPc = pc;
    cr->mColor = c;
    mCRanges.append( cr );
    calcColorRanges();
}

void KALedMeter::resizeEvent( QResizeEvent *e )
{
    Q3Frame::resizeEvent( e );
    int w = ( width() - frameWidth() - 2 ) / mCount * mCount;
    w += frameWidth() + 2;
    setFrameRect( QRect( 0, 0, w, height() ) );
}

void KALedMeter::drawContents( QPainter *p )
{
    QRect b = contentsRect();

    unsigned cidx = 0;
    int ncol = mCount;
    QColor col = colorGroup().foreground();
   
    if ( !mCRanges.isEmpty() )
    {
        col = mCRanges.at( cidx )->mColor;
        ncol = mCRanges.at( cidx )->mValue;
    }
    p->setBrush( col );
    p->setPen( col );

    int lw = b.width() / mCount;
    int lx = b.left() + 1;
    for ( int i = 0; i < mCurrentCount; i++, lx += lw )
    {
        if ( i > ncol )
        {
            if ( ++cidx < mCRanges.count() )
            {
                col = mCRanges.at( cidx )->mColor;
                ncol = mCRanges.at( cidx )->mValue;
                p->setBrush( col );
                p->setPen( col );
            }
        }

        p->drawRect( lx, b.top() + 1, lw - 1, b.height() - 2 );
    }
}

void KALedMeter::calcColorRanges()
{
    int prev = 0;
    ColorRange *cr;
    for ( cr = mCRanges.first(); cr; cr = mCRanges.next() )
    {
        cr->mValue = prev + cr->mPc * mCount / 100;
        prev = cr->mValue;
    }
}

