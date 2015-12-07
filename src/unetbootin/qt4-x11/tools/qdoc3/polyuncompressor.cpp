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

#include "command.h"
#include "polyuncompressor.h"

QT_BEGIN_NAMESPACE

/*!
  \class PolyUncompressor
  
  \brief The PolyUncompressor class is a class for uncompressing
  compressed files.

  This subclass of Uncompressor contains a parameterized
  command for doing the uncompression

  It has an uncompressFile() function you call to do the
  actual uncompression.
 */

/*!
  The constructor takes the list of filename \a extensions,
  which it passes to the base class, and the \a commandFormat,
  which it stores locally. The \a commandFormat is a command
  template string.
 */
PolyUncompressor::PolyUncompressor( const QStringList& extensions,
				    const QString& commandFormat )
    : Uncompressor( extensions ), cmd( commandFormat )
{
}

/*!
  The destructor doesn't have to do anything.
 */
PolyUncompressor::~PolyUncompressor()
{
}

/*!
  From \a filePath, derive a file path for the uncompressed
  file and return it. If it can't figure out what the file
  path should be, it just concatenates ".out" to the
  \a filePath and returns that.
 */
QString PolyUncompressor::uncompressedFilePath( const QString& filePath )
{
    QStringList::ConstIterator e = fileExtensions().begin();
    while ( e != fileExtensions().end() ) {
	QString dotExt = "." + *e;
	if ( filePath.endsWith(dotExt) )
	    return filePath.left( filePath.length() - dotExt.length() );
	++e;
    }
    return filePath + ".out"; // doesn't really matter
}

/*!
  Call this function to do the actual uncompressing. It calls
  the executeCommand() function to do the work. That's all it does.
 */
void PolyUncompressor::uncompressFile( const Location& location,
				       const QString& filePath,
				       const QString& outputFilePath )
{
    executeCommand( location, cmd,
		    QStringList() << filePath << outputFilePath );
}

QT_END_NAMESPACE
