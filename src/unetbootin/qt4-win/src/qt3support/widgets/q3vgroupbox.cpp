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

#include "q3vgroupbox.h"

QT_BEGIN_NAMESPACE

/*!
    \class Q3VGroupBox

    \brief The Q3VGroupBox widget organizes widgets in a group with one
    vertical column.

    \compat

    Q3VGroupBox is a convenience class that offers a thin layer on top
    of Q3GroupBox. Think of it as a Q3VBox that offers a frame with a
    title.

    \sa Q3HGroupBox
*/

/*!
    Constructs a horizontal group box with no title.

    The \a parent and \a name arguments are passed to the QWidget
    constructor.
*/
Q3VGroupBox::Q3VGroupBox( QWidget *parent, const char *name )
    : Q3GroupBox( 1, Qt::Horizontal /* sic! */, parent, name )
{
}

/*!
    Constructs a horizontal group box with the title \a title.

    The \a parent and \a name arguments are passed to the QWidget
    constructor.
*/

Q3VGroupBox::Q3VGroupBox( const QString &title, QWidget *parent,
			    const char *name )
    : Q3GroupBox( 1, Qt::Horizontal /* sic! */, title, parent, name )
{
}

/*!
    Destroys the horizontal group box, deleting its child widgets.
*/
Q3VGroupBox::~Q3VGroupBox()
{
}

QT_END_NAMESPACE
