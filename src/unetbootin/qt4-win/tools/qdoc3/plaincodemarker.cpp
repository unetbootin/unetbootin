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

#include "plaincodemarker.h"

QT_BEGIN_NAMESPACE

PlainCodeMarker::PlainCodeMarker()
{
}

PlainCodeMarker::~PlainCodeMarker()
{
}

bool PlainCodeMarker::recognizeCode( const QString& /* code */ )
{
    return true;
}

bool PlainCodeMarker::recognizeExtension( const QString& /* ext */ )
{
    return true;
}

bool PlainCodeMarker::recognizeLanguage( const QString& /* lang */ )
{
    return false;
}

QString PlainCodeMarker::plainName( const Node * /* node */ )
{
    return "";
}

QString PlainCodeMarker::plainFullName(const Node * /* node */, const Node * /* relative */)
{
    return "";
}

QString PlainCodeMarker::markedUpCode( const QString& code,
				       const Node * /* relative */,
				       const QString& /* dirPath */ )
{
    return protect( code );
}

QString PlainCodeMarker::markedUpSynopsis( const Node * /* node */,
					   const Node * /* relative */,
					   SynopsisStyle /* style */ )
{
    return "foo";
}

QString PlainCodeMarker::markedUpName( const Node * /* node */ )
{
    return "";
}

QString PlainCodeMarker::markedUpFullName( const Node * /* node */,
					   const Node * /* relative */ )
{
    return "";
}

QString PlainCodeMarker::markedUpEnumValue(const QString & /* enumValue */,
                                           const Node * /* relative */)
{
    return "";
}

QString PlainCodeMarker::markedUpIncludes( const QStringList& /* includes */ )
{
    return "";
}

QString PlainCodeMarker::functionBeginRegExp( const QString& /* funcName */ )
{
    return "";
}

QString PlainCodeMarker::functionEndRegExp( const QString& /* funcName */ )
{
    return "";
}

QList<Section> PlainCodeMarker::sections(const InnerNode * /* innerNode */,
                                         SynopsisStyle /* style */,
					 Status /* status */)
{
     return QList<Section>();
}

const Node *PlainCodeMarker::resolveTarget( const QString& /* target */,
                                            const Tree * /* tree */,
					    const Node * /* relative */ )
{
    return 0;
}

QT_END_NAMESPACE
