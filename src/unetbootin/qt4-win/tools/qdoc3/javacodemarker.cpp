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
    javacodemarker.cpp
*/

#include "javacodemarker.h"
#include "node.h"
#include "text.h"
#include "tree.h"

QT_BEGIN_NAMESPACE

JavaCodeMarker::JavaCodeMarker()
{
}

JavaCodeMarker::~JavaCodeMarker()
{
}

bool JavaCodeMarker::recognizeCode( const QString& /* code */ )
{
    return true;
}

bool JavaCodeMarker::recognizeExtension( const QString& ext )
{
    return ext == "java";
}

bool JavaCodeMarker::recognizeLanguage( const QString& lang )
{
    return lang == "Java";
}

QString JavaCodeMarker::plainName( const Node *node )
{
    return node->name();
}

QString JavaCodeMarker::plainFullName( const Node *node, const Node * /* relative */ )
{
    if (!node)
        return QString();

    QString fullName;
    for ( ;; ) {
	fullName.prepend( plainName(node) );
	if ( node->parent() && node->parent()->name().isEmpty() )
	    break;
	node = node->parent();
    if (!node)
        break;
	fullName.prepend(".");
    }
    return fullName;
}

QString JavaCodeMarker::markedUpCode( const QString& code,
				      const Node * /* relative */,
				      const QString& /* dirPath */ )
{
    return protect( code );
}

QString JavaCodeMarker::markedUpSynopsis(const Node * /* node */,
					 const Node * /* relative */,
					 SynopsisStyle /* style */)
{
    return QString();
}

QString JavaCodeMarker::markedUpName( const Node *node )
{
    return linkTag(node, taggedNode(node));
}

QString JavaCodeMarker::markedUpFullName(const Node *node, const Node * /* relative */ )
{
    QString fullName;
    for ( ;; ) {
	fullName.prepend( markedUpName(node) );
	if ( node->parent()->name().isEmpty() )
	    break;
	node = node->parent();
	fullName.prepend( "." );
    }
    return fullName;
}

QString JavaCodeMarker::markedUpEnumValue(const QString &enumValue,
                                          const Node * /* relative */)
{
    return protect(enumValue);
}

QString JavaCodeMarker::markedUpIncludes( const QStringList& /* includes */ )
{
    return QString();
}

QString JavaCodeMarker::functionBeginRegExp( const QString& /* funcName */)
{
    return "^x$"; // ### invalid regexp
}

QString JavaCodeMarker::functionEndRegExp( const QString& /* funcName */ )
{
    return "^}";
}

QList<Section> JavaCodeMarker::sections(const InnerNode * /* inner */, SynopsisStyle /* style */,
                                        Status /* status */)
{
    return QList<Section>();
}

const Node *JavaCodeMarker::resolveTarget(const QString &target, const Tree *tree,
					  const Node *relative)
{
    if (target.endsWith("()")) {
        const FunctionNode *func;
        QString funcName = target;
        funcName.chop(2);

        QStringList path = funcName.split('.');
        if ((func = tree->findFunctionNode(path, relative, Tree::SearchBaseClasses)))
            return func;
    } else if (target.contains("#")) {
        int hashAt = target.indexOf("#");
        QString link = target.left(hashAt);
        QString ref = target.mid(hashAt + 1);
        const Node *node;
        if (link.isEmpty()) {
            node = relative;
        } else {
            QStringList path(link);
            node = tree->findNode(path, tree->root(), Tree::SearchBaseClasses);
        }
        if (node && node->isInnerNode()) {
            const Atom *atom = node->doc().body().firstAtom();
            while (atom) {
                if (atom->type() == Atom::Target && atom->string() == ref) {
                    Node *parentNode = const_cast<Node *>(node);
                    return new TargetNode(static_cast<InnerNode*>(parentNode),
                                          ref);
                }
                atom = atom->next();
            }
        }
    } else {
        QStringList path = target.split('.');
        const Node *node;
        if ((node = tree->findNode(path, relative,
                                   Tree::SearchBaseClasses | Tree::SearchEnumValues)))
            return node;
    }
    return 0;
}

QT_END_NAMESPACE
