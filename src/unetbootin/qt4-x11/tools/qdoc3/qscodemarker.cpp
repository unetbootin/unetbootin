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
  qscodemarker.cpp
*/

#include "node.h"
#include "qscodemarker.h"

QT_BEGIN_NAMESPACE

QsCodeMarker::QsCodeMarker()
{
}

QsCodeMarker::~QsCodeMarker()
{
}

bool QsCodeMarker::recognizeCode( const QString& /* code */ )
{
    return true;
}

bool QsCodeMarker::recognizeExtension( const QString& ext )
{
    return ext == "js" || ext == "qs";
}

bool QsCodeMarker::recognizeLanguage( const QString& lang )
{
    return lang == "JavaScript" || lang == "Qt Script";
}

QString QsCodeMarker::plainName( const Node *node )
{
    QString name = node->name();
    if ( node->type() == Node::Function )
	name += "()";
    return name;
}

QString QsCodeMarker::plainFullName( const Node *node, const Node * /* relative */ )
{
    QString fullName;
    for ( ;; ) {
	fullName.prepend( plainName(node) );
	if ( node->parent()->name().isEmpty() )
	    break;
	node = node->parent();
	fullName.prepend(".");
    }
    return fullName;
}

QString QsCodeMarker::markedUpCode( const QString& code,
				    const Node * /* relative */,
				    const QString& /* dirPath */ )
{
    return protect( code );
}

QString QsCodeMarker::markedUpSynopsis( const Node *node,
					const Node * /* relative */,
					SynopsisStyle style )
{
    QString synopsis;
    QStringList extras;
    QString name;

    name = taggedNode( node );
    if ( style != Detailed )
        name = linkTag( node, name );
    name = "<@name>" + name + "</@name>";

    if ( style == Detailed && !node->parent()->name().isEmpty() &&
	 node->type() != Node::Enum )
	name.prepend( taggedNode(node->parent()) + "." );

    switch ( node->type() ) {
    case Node::Class:
        synopsis = "class " + name;
        break;
    case Node::Function:
	{
            const FunctionNode *func = (const FunctionNode *) node;

	    synopsis = name;

	    if ( style == SeparateList ) {
		synopsis += "()";
	    } else {
		synopsis += " (";
        	if ( !func->parameters().isEmpty() ) {
        	    synopsis += " ";
		    int numOptional = 0;
        	    QList<Parameter>::ConstIterator p = func->parameters().begin();
        	    while ( p != func->parameters().end() ) {
			if ( !(*p).defaultValue().isEmpty() ) {
			    if ( p == func->parameters().begin() ) {
				synopsis += "[ ";
			    } else {
				synopsis += " [ , ";
			    }
			    numOptional++;
			} else {
                	    if ( p != func->parameters().begin() )
                		synopsis += ", ";
			}
			if ( !(*p).name().isEmpty() )
			    synopsis += "<@param>" + protect( (*p).name() ) +
					"</@param> : ";
			synopsis += protect( (*p).leftType() );
                	++p;
        	    }
		    for ( int i = 0; i < numOptional; i++ )
			synopsis += " ]";
        	    synopsis += " ";
        	}
		synopsis += ")";
	    }

	    if ( style != SeparateList && !func->returnType().isEmpty() )
		synopsis += " : " + protect( func->returnType() );

            if ( style == Detailed && func->metaness() == FunctionNode::Signal )
		extras << "[signal]";
	}
        break;
    case Node::Property:
	{
            const PropertyNode *property = (const PropertyNode *) node;

	    synopsis = name;
	    if ( style != SeparateList )
		synopsis += " : " + property->dataType();
	    if ( style == Detailed && property->setters().isEmpty() )
		extras << "[read only]";
	}
        break;
    case Node::Enum:
	{
	    /*
	      The letters A to F and X (upper- and lower-case) can
	      appear in a hexadecimal constant (e.g. 0x3F).
	    */
	    QRegExp letterRegExp( "[G-WYZg-wyz_]" );
	    const EnumNode *enume = (const EnumNode *) node;

	    synopsis = name;
	    if ( style == Summary && !enume->items().isEmpty() ) {
		synopsis += " : ";
		QString comma;
		QList<EnumItem>::ConstIterator it = enume->items().begin();
		while ( it != enume->items().end() ) {
		    if ( enume->itemAccess((*it).name()) == Node::Public ) {
			synopsis += comma;
			synopsis += (*it).name();
			if ( (*it).value().indexOf(letterRegExp) != -1 )
			    synopsis += " = " + (*it).value();
			comma = ", ";
		    }
		    ++it;
		}
	    }
	}
	break;
    case Node::Namespace:
    case Node::Typedef:
    default:
        synopsis = name;
    }

    if ( style == Summary ) {
        if ( node->status() == Node::Preliminary ) {
            extras << "(preliminary)";
        } else if ( node->status() == Node::Deprecated ) {
            extras << "(deprecated)";
        } else if ( node->status() == Node::Obsolete ) {
            extras << "(obsolete)";
        }
    }

    QString extra;
    if ( !extras.isEmpty() )
        extra = "<@extra>" + extras.join(" ") + "</@extra>";
    return synopsis + extra;
}

QString QsCodeMarker::markedUpName( const Node *node )
{
    QString name = linkTag( node, taggedNode(node) );
    if ( node->type() == Node::Function )
	name += "()";
    return name;
}

QString QsCodeMarker::markedUpFullName( const Node *node,
					const Node * /* relative */ )
{
    QString fullName;
    for ( ;; ) {
	fullName.prepend( markedUpName(node) );
	if ( node->parent()->name().isEmpty() )
	    break;
	node = node->parent();
	fullName.prepend( "<@op>.</@op>" );
    }
    return fullName;
}

QString QsCodeMarker::markedUpEnumValue(const QString & /* enumValue */,
                                        const Node * /* relative */)
{
    return QString();
}

QString QsCodeMarker::markedUpIncludes( const QStringList& /* includes */ )
{
    return QString();
}

QString QsCodeMarker::functionBeginRegExp( const QString& funcName )
{
    return "^function[ \t].*\\b" + QRegExp::escape( funcName );
}

QString QsCodeMarker::functionEndRegExp( const QString& /* funcName */ )
{
    return "^}";
}

QList<Section> QsCodeMarker::sections( const InnerNode *inner, SynopsisStyle style, Status status )
{
    QList<Section> sections;

    if (inner->type() != Node::Class)
        return sections;

    const ClassNode *classe = static_cast<const ClassNode *>(inner);

    if ( style == Summary ) {
	FastSection enums(classe, "Enums", "enum", "enums");
	FastSection functions(classe, "Functions", "function", "functions");
	FastSection readOnlyProperties(classe, "Read-Only Properties", "property", "properties");
	FastSection signalz(classe, "Signals", "signal", "signals");
	FastSection writableProperties(classe, "Writable Properties", "property", "properties");

	QStack<const ClassNode *> stack;
	stack.push( classe );

	while ( !stack.isEmpty() ) {
	    const ClassNode *ancestorClass = stack.pop();

	    NodeList::ConstIterator c = ancestorClass->childNodes().begin();
	    while ( c != ancestorClass->childNodes().end() ) {
		if ( (*c)->access() == Node::Public ) {
		    if ( (*c)->type() == Node::Enum ) {
			insert( enums, *c, style, status );
		    } else if ( (*c)->type() == Node::Function ) {
			const FunctionNode *func = (const FunctionNode *) *c;
			if ( func->metaness() == FunctionNode::Signal ) {
			    insert( signalz, *c, style, status );
			} else {
			    insert( functions, *c, style, status );
			}
		    } else if ( (*c)->type() == Node::Property ) {
			const PropertyNode *property =
				(const PropertyNode *) *c;
			if ( property->setters().isEmpty() ) {
			    insert( readOnlyProperties, *c, style, status );
			} else {
			    insert( writableProperties, *c, style, status );
			}
		    }
		}
		++c;
	    }

	    QList<RelatedClass>::ConstIterator r = ancestorClass->baseClasses().begin();
	    while ( r != ancestorClass->baseClasses().end() ) {
		stack.prepend( (*r).node );
		++r;
	    }
	}
	append( sections, enums );
	append( sections, writableProperties );
	append( sections, readOnlyProperties );
	append( sections, functions );
	append( sections, signalz );
    } else if ( style == Detailed ) {
	FastSection enums( classe, "Enum Documentation" );
	FastSection functionsAndSignals( classe, "Function and Signal Documentation" );
	FastSection properties( classe, "Property Documentation" );

	NodeList::ConstIterator c = classe->childNodes().begin();
	while ( c != classe->childNodes().end() ) {
	    if ( (*c)->access() == Node::Public ) {
		if ( (*c)->type() == Node::Enum ) {
		    insert( enums, *c, style, status );
		} else if ( (*c)->type() == Node::Function ) {
		    insert( functionsAndSignals, *c, style, status );
		} else if ( (*c)->type() == Node::Property ) {
		    insert( properties, *c, style, status );
		}
	    }
	    ++c;
	}
	append( sections, enums );
	append( sections, properties );
	append( sections, functionsAndSignals );
    } else { // ( style == SeparateList )
	FastSection all( classe );

	QStack<const ClassNode *> stack;
	stack.push( classe );

	while ( !stack.isEmpty() ) {
	    const ClassNode *ancestorClass = stack.pop();

	    NodeList::ConstIterator c = ancestorClass->childNodes().begin();
	    while ( c != ancestorClass->childNodes().end() ) {
		if ( (*c)->access() == Node::Public )
		    insert( all, *c, style, status );
		++c;
	    }

	    QList<RelatedClass>::ConstIterator r = ancestorClass->baseClasses().begin();
	    while ( r != ancestorClass->baseClasses().end() ) {
		stack.prepend( (*r).node );
		++r;
	    }
	}
	append( sections, all );
    }
    return sections;
}

const Node *QsCodeMarker::resolveTarget( const QString& /* target */,
                                         const Tree * /* tree */,
					 const Node * /* relative */ )
{
    return 0;
}

QT_END_NAMESPACE
