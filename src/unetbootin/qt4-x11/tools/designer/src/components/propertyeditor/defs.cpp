/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt Designer of the Qt Toolkit.
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
