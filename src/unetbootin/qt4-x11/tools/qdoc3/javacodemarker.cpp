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
