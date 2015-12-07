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

#include <qimage.h>
#include <qcolor.h>

static inline int blendComponent( int v, int av, int s, int as )
{
    return as*s + av*v -(av*as*s)/255;  
}
    
static inline QRgb blendShade( QRgb v, QRgb s )
{
    //shadow image is already reduced and blurred
    int as = qAlpha(s); 
    int av = qAlpha(v);
    if ( as == 0 || av == 255 )
	return v;

    int a = as + av -(as*av)/255;
    
    int r = blendComponent( qRed(v),av, qRed(s), as)/a;
    int g = blendComponent( qGreen(v),av, qGreen(s), as)/a;
    int b = blendComponent( qBlue(v),av, qBlue(s), as)/a;

    return qRgba(r,g,b,a);
}

 

int main( int*, char**)
{
    QImage image( "out.png" );
    image.convertDepth( 32 );
    QImage shade( "outshade.png" );
    shade.convertDepth( 32 );
    int dx = 10;
    int dy = 5;
	
    int w = image.width();
    int h = image.height();
	
    QImage img( w+dx, h+dy, 32 );
    img.setAlphaBuffer( TRUE );

    for ( int y = 0; y < h+dy; y++ ) {
	for ( int x = 0; x < w+dx; x++ ) {
	    QRgb sh =  (x<dx||y<dy) ? 0 : shade.pixel( x-dx, y-dy );
	    QRgb pixel = (x<w&y<h) ? image.pixel( x, y ) : 0;
	    img.setPixel( x, y, blendShade( pixel, sh ) ); 
	}
    }
    img.save("blend.png", "PNG" );
}


