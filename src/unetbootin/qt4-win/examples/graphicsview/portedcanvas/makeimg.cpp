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
    //shadow gets a color inversely proportional to the
    //alpha value
    s = (s*(255-as))/255;
    //then do standard blending
    return as*s + av*v -(av*as*s)/255;  
}
    
static inline QRgb blendShade( QRgb v, QRgb s )
{
    //pick a number: shadow is 1/3 of object
    int as = qAlpha(s)/3; 
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
    QImage *img;

    img = new QImage( "in.png" );
    int w,h;
    int y;
    img->setAlphaBuffer( TRUE );
    *img = img->convertDepth( 32 );
    w = img->width();
    h = img->height();
#if 0
    for ( y = 0; y < h; y ++ ) {
	uint *line = (uint*)img->scanLine( y );
	for ( int x = 0; x < w; x++ ) {
	    uint pixel = line[x];
	    int r = qRed(pixel);
	    int g = qGreen(pixel);
	    int b = qBlue(pixel);
	    int min = QMIN( r, QMIN( g, b ) );
	    int max = QMAX( r, QMAX( g, b ) );
	    r -= min;
	    g -= min;
	    b -= min;
	    if ( max !=min ) {
		r = (r*255)/(max-min);
		g = (g*255)/(max-min);
		b = (b*255)/(max-min);
	    }
	    int a = 255-min;
	    a -=  (max-min)/3; //hack more transparency for colors.
	    line[x] = qRgba( r, g, b, a );
	}
    }
#endif    
    *img = img->smoothScale( w/2, h/2 );

    qDebug( "saving out.png");
    img->save( "out.png", "PNG" );
    
    w = img->width();
    h = img->height();
    
    QImage *img2 = new QImage( w, h, 32 );
    img2->setAlphaBuffer( TRUE );
    for ( y = 0; y < h; y++ ) {
	for ( int x = 0; x < w; x++ ) {
	    QRgb shader = img->pixel( x, y );

	    int as = qAlpha(shader)/3;

	    int r = (qRed(shader)*(255-as))/255;
	    int g = (qGreen(shader)*(255-as))/255;
	    int b = (qBlue(shader)*(255-as))/255;

	    img2->setPixel( x, y, qRgba(r,g,b,as) ); 
	}
    }

    img2->save( "outshade.png", "PNG" );

}
