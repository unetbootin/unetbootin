/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
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

/*
  uncompressor.cpp
*/

#include "uncompressor.h"

QT_BEGIN_NAMESPACE

QList<Uncompressor *> Uncompressor::uncompressors;


/*!
  \class Uncompressor
  
  \brief The Uncompressor class is a base class for classes that
  know how to uncompress a certain kind of compressed file.

  The uncompressor contains a list of the filename extensions
  of the file types that the uncompressor knows how to uncompress.

  It maintains a static list of all the instances of Uncompressor
  that have been created. It also has a static function for searching
  that list to find the uncompressor to use for uncompressing a file
  with a certain extension.
 */

/*!
  The constructor takes a list of filename extensions, which it
  copies and saves internally. This uncompressor is prepended
  to the stack list.
 */
Uncompressor::Uncompressor( const QStringList& extensions )
    : fileExts( extensions )
{
    uncompressors.prepend( this );
}

/*!
  The destructor deletes all the filename extensions.
 */
Uncompressor::~Uncompressor()
{
    uncompressors.removeAll( this );
}

/*!
  This function searches the static list of uncompressors to find the
  first one that can handle \a fileName. If it finds an acceptable
  uncompressor, it returns a pointer to it. Otherwise it returns null.
*/
Uncompressor*
Uncompressor::uncompressorForFileName( const QString& fileName )
{
    int dot = -1;
    while ( (dot = fileName.indexOf(".", dot + 1)) != -1 ) {
        QString ext = fileName.mid( dot + 1 );
        QList<Uncompressor *>::ConstIterator u = uncompressors.begin();
        while ( u != uncompressors.end() ) {
            if ( (*u)->fileExtensions().contains(ext) )
                return *u;
            ++u;
        }
    }
    return 0;
}

QT_END_NAMESPACE
