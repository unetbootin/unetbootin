/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt Designer of the Qt Toolkit.
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

#include "defs.h"

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {

int size_type_to_int( QSizePolicy::Policy t )
{
    if ( t == QSizePolicy::Fixed )
	return 0;
    if ( t == QSizePolicy::Minimum )
	return 1;
    if ( t == QSizePolicy::Maximum )
	return 2;
    if ( t == QSizePolicy::Preferred )
	return 3;
    if ( t == QSizePolicy::MinimumExpanding )
	return 4;
    if ( t == QSizePolicy::Expanding )
	return 5;
    if ( t == QSizePolicy::Ignored )
	return 6;
    return 0;
}

QString size_type_to_string( QSizePolicy::Policy t )
{
    if ( t == QSizePolicy::Fixed )
	return QString::fromUtf8("Fixed");
    if ( t == QSizePolicy::Minimum )
	return QString::fromUtf8("Minimum");
    if ( t == QSizePolicy::Maximum )
	return QString::fromUtf8("Maximum");
    if ( t == QSizePolicy::Preferred )
	return QString::fromUtf8("Preferred");
    if ( t == QSizePolicy::MinimumExpanding )
	return QString::fromUtf8("MinimumExpanding");
    if ( t == QSizePolicy::Expanding )
	return QString::fromUtf8("Expanding");
    if ( t == QSizePolicy::Ignored )
	return QString::fromUtf8("Ignored");
    return QString();
}

QSizePolicy::Policy int_to_size_type( int i )
{
    if ( i == 0 )
	return QSizePolicy::Fixed;
    if ( i == 1 )
	return QSizePolicy::Minimum;
    if ( i == 2 )
	return QSizePolicy::Maximum;
    if ( i == 3 )
	return QSizePolicy::Preferred;
    if ( i == 4 )
	return QSizePolicy::MinimumExpanding;
    if ( i == 5 )
	return QSizePolicy::Expanding;
    if ( i == 6 )
	return QSizePolicy::Ignored;
    return QSizePolicy::Preferred;
}

}  // namespace qdesigner_internal

QT_END_NAMESPACE
