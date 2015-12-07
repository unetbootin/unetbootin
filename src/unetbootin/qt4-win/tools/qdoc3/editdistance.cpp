/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the tools applications of the Qt Toolkit.
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
  editdistance.cpp
*/

#include "editdistance.h"

QT_BEGIN_NAMESPACE

int editDistance( const QString& s, const QString& t )
{
#define D( i, j ) d[(i) * n + (j)]
    int i;
    int j;
    int m = s.length() + 1;
    int n = t.length() + 1;
    int *d = new int[m * n];
    int result;

    for ( i = 0; i < m; i++ )
	D( i, 0 ) = i;
    for ( j = 0; j < n; j++ )
	D( 0, j ) = j;
    for ( i = 1; i < m; i++ ) {
	for ( j = 1; j < n; j++ ) {
	    if ( s[i - 1] == t[j - 1] ) {
		D( i, j ) = D( i - 1, j - 1 );
	    } else {
		int x = D( i - 1, j );
		int y = D( i - 1, j - 1 );
		int z = D( i, j - 1 );
		D( i, j ) = 1 + qMin( qMin(x, y), z );
	    }
	}
    }
    result = D( m - 1, n - 1 );
    delete[] d;
    return result;
#undef D
}

QString nearestName( const QString& actual, const QSet<QString>& candidates )
{
    int deltaBest = 10000;
    int numBest = 0;
    QString best;

    QSet<QString>::ConstIterator c = candidates.begin();
    while ( c != candidates.end() ) {
	if ( (*c)[0] == actual[0] ) {
	    int delta = editDistance( actual, *c );
	    if ( delta < deltaBest ) {
		deltaBest = delta;
		numBest = 1;
		best = *c;
	    } else if ( delta == deltaBest ) {
		numBest++;
	    }
	}
	++c;
    }

    if ( numBest == 1 && deltaBest <= 2 &&
	 actual.length() + best.length() >= 5 ) {
	return best;
    } else {
	return "";
    }
}

QT_END_NAMESPACE
