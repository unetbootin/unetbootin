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
  pagegenerator.cpp
*/

#include <qfile.h>
#include <qfileinfo.h>

#include "pagegenerator.h"
#include "tree.h"

QT_BEGIN_NAMESPACE

PageGenerator::PageGenerator()
{
}

PageGenerator::~PageGenerator()
{
    while ( !outStreamStack.isEmpty() )
	endSubPage();
}

void PageGenerator::generateTree( const Tree *tree, CodeMarker *marker )
{
    generateInnerNode( tree->root(), marker );
}

QString PageGenerator::fileBase(const Node *node)
{
    if (node->relates())
	node = node->relates();
    else if (!node->isInnerNode())
	node = node->parent();

    QString base = node->doc().baseName();
    if (!base.isEmpty())
        return base;

	const Node *p = node;

        forever {
            base.prepend(p->name());
        const Node *pp = p->parent();
        if (!pp || pp->name().isEmpty() || pp->type() == Node::Fake)
                break;
        base.prepend(QLatin1Char('-'));
        p = pp;
        }

        if (node->type() == Node::Fake) {
#ifdef QDOC2_COMPAT
            if (base.endsWith(".html"))
	        base.truncate(base.length() - 5);
#endif
        }

    // the code below is effectively equivalent to:
    //   base.replace(QRegExp("[^A-Za-z0-9]+"), " ");
    //   base = base.trimmed();
    //   base.replace(" ", "-");
    //   base = base.toLower();
    // as this function accounted for ~8% of total running time
    // we optimize a bit...

    QString res;
    // +5 prevents realloc in fileName() below
    res.reserve(base.size() + 5);
    bool begun = false;
    for (int i = 0; i != base.size(); ++i) {
        QChar c = base.at(i);
        uint u = c.unicode();
        if (u >= 'A' && u <= 'Z')
            u -= 'A' - 'a';
        if ((u >= 'a' &&  u <= 'z') || (u >= '0' && u <= '9')) {
            res += QLatin1Char(u);
            begun = true;
        } else if (begun) {
            res += QLatin1Char('-');
        }
    }
    while (res.endsWith(QLatin1Char('-')))
        res.chop(1);
    return res;
}

QString PageGenerator::fileName( const Node *node )
{
    if (!node->url().isEmpty())
        return node->url();

    QString name = fileBase(node);
    name += QLatin1Char('.');
    name += fileExtension(node);
    return name;
}

QString PageGenerator::outFileName()
{
    return QFileInfo(static_cast<QFile *>(out().device())->fileName()).fileName();
}

void PageGenerator::beginSubPage( const Location& location,
				  const QString& fileName )
{
    QFile *outFile = new QFile( outputDir() + "/" + fileName );
    if ( !outFile->open(QFile::WriteOnly) )
	location.fatal( tr("Cannot open output file '%1'")
			.arg(outFile->fileName()) );
    QTextStream *out = new QTextStream(outFile);
    out->setCodec("ISO-8859-1");
    outStreamStack.push(out);
}

void PageGenerator::endSubPage()
{
    outStreamStack.top()->flush();
    delete outStreamStack.top()->device();
    delete outStreamStack.pop();
}

QTextStream &PageGenerator::out()
{
    return *outStreamStack.top();
}

void PageGenerator::generateInnerNode( const InnerNode *node,
				       CodeMarker *marker )
{
    if (!node->url().isNull())
        return;

    if (node->type() == Node::Fake) {
        const FakeNode *fakeNode = static_cast<const FakeNode *>(node);
        if (fakeNode->subType() == FakeNode::ExternalPage)
            return;
    }

    if ( node->parent() != 0 ) {
	beginSubPage( node->location(), fileName(node) );
	if ( node->type() == Node::Namespace || node->type() == Node::Class) {
	    generateClassLikeNode(node, marker);
	} else if ( node->type() == Node::Fake ) {
	    generateFakeNode(static_cast<const FakeNode *>(node), marker);
	}
	endSubPage();
    }

    NodeList::ConstIterator c = node->childNodes().begin();
    while ( c != node->childNodes().end() ) {
	if ((*c)->isInnerNode() && (*c)->access() != Node::Private)
	    generateInnerNode( (const InnerNode *) *c, marker );
	++c;
    }
}

QT_END_NAMESPACE
