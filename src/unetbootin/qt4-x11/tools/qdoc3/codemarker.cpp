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

#include <QMetaObject>
#include <QDebug>
#include "codemarker.h"
#include "config.h"
#include "node.h"

#include <stdio.h>

QT_BEGIN_NAMESPACE

QString CodeMarker::defaultLang;
QList<CodeMarker *> CodeMarker::markers;

CodeMarker::CodeMarker()
    : slow(false)
{
    markers.prepend( this );
}

CodeMarker::~CodeMarker()
{
    markers.removeAll( this );
}

void CodeMarker::initializeMarker( const Config &config)
{
    slow = config.getBool(QLatin1String(CONFIG_SLOW));
}

void CodeMarker::terminateMarker()
{
}

void CodeMarker::initialize( const Config& config )
{
    defaultLang = config.getString( QLatin1String(CONFIG_LANGUAGE) );

    QList<CodeMarker *>::ConstIterator m = markers.begin();
    while ( m != markers.end() ) {
	(*m)->initializeMarker( config );
	++m;
    }
}

void CodeMarker::terminate()
{
    QList<CodeMarker *>::ConstIterator m = markers.begin();
    while ( m != markers.end() ) {
	(*m)->terminateMarker();
	++m;
    }
}

CodeMarker *CodeMarker::markerForCode( const QString& code )
{
    CodeMarker *defaultMarker = markerForLanguage( defaultLang );
    if ( defaultMarker != 0 && defaultMarker->recognizeCode(code) )
	return defaultMarker;

    QList<CodeMarker *>::ConstIterator m = markers.begin();
    while ( m != markers.end() ) {
	if ( (*m)->recognizeCode(code) )
	    return *m;
	++m;
    }
    return defaultMarker;
}

CodeMarker *CodeMarker::markerForFileName( const QString& fileName )
{
    CodeMarker *defaultMarker = markerForLanguage( defaultLang );
    int dot = -1;
    while ((dot = fileName.lastIndexOf(QLatin1Char('.'), dot)) != -1) {
	QString ext = fileName.mid( dot + 1 );
	if ( defaultMarker != 0 && defaultMarker->recognizeExtension(ext) )
	    return defaultMarker;
	QList<CodeMarker *>::ConstIterator m = markers.begin();
	while ( m != markers.end() ) {
	    if ( (*m)->recognizeExtension(ext) )
		return *m;
	    ++m;
	}
        --dot;
    }
    return defaultMarker;
}

CodeMarker *CodeMarker::markerForLanguage( const QString& lang )
{
    QList<CodeMarker *>::ConstIterator m = markers.begin();
    while ( m != markers.end() ) {
	if ( (*m)->recognizeLanguage(lang) )
	    return *m;
	++m;
    }
    return 0;
}

const Node *CodeMarker::nodeForString( const QString& string )
{
    if (sizeof(const Node *) == sizeof(uint)) {
        return reinterpret_cast<const Node *>(string.toUInt());
    } else {
        return reinterpret_cast<const Node *>(string.toULongLong());
    }
}

QString CodeMarker::stringForNode( const Node *node )
{
    if (sizeof(const Node *) == sizeof(ulong)) {
        return QString::number(reinterpret_cast<ulong>(node));
    } else {
        return QString::number(reinterpret_cast<qulonglong>(node));
    }
}

static const QString samp  = QLatin1String("&amp;");
static const QString slt   = QLatin1String("&lt;");
static const QString sgt   = QLatin1String("&gt;");
static const QString squot = QLatin1String("&quot;");

QString CodeMarker::protect( const QString& str )
{
    int n = str.length();
    QString marked;
    marked.reserve(n * 2 + 30);
    const QChar *data = str.constData();
    for (int i = 0; i != n; ++i) {
        switch (data[i].unicode()) {
            case '&': marked += samp;  break;
            case '<': marked += slt;   break;
            case '>': marked += sgt;   break;
            case '"': marked += squot; break;
            default : marked += data[i];
        }
    }
    return marked;
}

QString CodeMarker::typified(const QString &string)
{
    QString result;
    QString pendingWord;

    for (int i = 0; i <= string.size(); ++i) {
        QChar ch;
        if (i != string.size())
            ch = string.at(i);

        QChar lower = ch.toLower();
        if ((lower >= QLatin1Char('a') && lower <= QLatin1Char('z'))
            || ch.digitValue() >= 0 || ch == QLatin1Char('_')
                || ch == QLatin1Char(':')) {
            pendingWord += ch;
        } else {
            if (!pendingWord.isEmpty()) {
                bool isProbablyType = (pendingWord != QLatin1String("const"));
                if (isProbablyType)
                    result += QLatin1String("<@type>");
                result += pendingWord;
                if (isProbablyType)
                    result += QLatin1String("</@type>");
            }
            pendingWord.clear();

            switch (ch.unicode()) {
            case '\0':
                break;
            case '&':
                result += QLatin1String("&amp;");
                break;
            case '<':
                result += QLatin1String("&lt;");
                break;
            case '>':
                result += QLatin1String("&gt;");
                break;
            default:
                result += ch;
            }
        }
    }
    return result;
}

QString CodeMarker::taggedNode( const Node *node )
{
    QString tag;

    switch ( node->type() ) {
    case Node::Namespace:
        tag = QLatin1String("@namespace");
        break;
    case Node::Class:
        tag = QLatin1String("@class");
        break;
    case Node::Enum:
        tag = QLatin1String("@enum");
        break;
    case Node::Typedef:
        tag = QLatin1String("@typedef");
        break;
    case Node::Function:
        tag = QLatin1String("@function");
        break;
    case Node::Property:
        tag = QLatin1String("@property");
        break;
    default:
        tag = QLatin1String("@unknown");
    }
    return QLatin1Char('<') + tag + QLatin1Char('>') + protect( node->name() )
        + QLatin1String("</") + tag + QLatin1Char('>');
}

QString CodeMarker::linkTag( const Node *node, const QString& body )
{
    return QLatin1String("<@link node=\"") + stringForNode( node )
        + QLatin1String("\">") + body + QLatin1String("</@link>");
}

QString CodeMarker::sortName( const Node *node )
{
    QString nodeName = node->name();
    int numDigits = 0;
    for (int i = nodeName.size() - 1; i > 0; --i) {
        if (nodeName.at(i).digitValue() == -1)
            break;
        ++numDigits;
    }

    // we want 'qint8' to appear before 'qint16'
    if (numDigits > 0) {
        for (int i = 0; i < 4 - numDigits; ++i)
            nodeName.insert(nodeName.size() - numDigits - 1, QLatin1String("0"));
    }

    if ( node->type() == Node::Function ) {
        const FunctionNode *func = static_cast<const FunctionNode *>(node);
        QString sortNo;
        if ( func->metaness() == FunctionNode::Ctor ) {
            sortNo = QLatin1String("C");
        } else if ( func->metaness() == FunctionNode::Dtor ) {
            sortNo = QLatin1String("D");
        } else {
            if (nodeName.startsWith(QLatin1String("operator"))
                    && nodeName.length() > 8
                    && !nodeName[8].isLetterOrNumber())
                sortNo = QLatin1String("F");
            else
                sortNo = QLatin1String("E");
        }
        return sortNo + nodeName + QLatin1Char(' ')
             + QString::number(func->overloadNumber(), 36);
    }

    if (node->type() == Node::Class)
        return QLatin1Char('A') + nodeName;

    if (node->type() == Node::Property || node->type() == Node::Variable)
        return QLatin1Char('E') + nodeName;

    return QLatin1Char('B') + nodeName;
}

void CodeMarker::insert(FastSection &fastSection, Node *node, SynopsisStyle style, Status status)
{
    bool inheritedMember = (!node->relates() &&
			    (node->parent() != (const InnerNode *)fastSection.innerNode));
    bool irrelevant = false;

    if (node->access() == Node::Private) {
	irrelevant = true;
    } else if ( node->type() == Node::Function ) {
	FunctionNode *func = (FunctionNode *) node;
	irrelevant = (inheritedMember
		      && (func->metaness() == FunctionNode::Ctor ||
			  func->metaness() == FunctionNode::Dtor));
    } else if ( node->type() == Node::Class || node->type() == Node::Enum
		    || node->type() == Node::Typedef ) {
	irrelevant = ( inheritedMember && style != SeparateList );
        if (!irrelevant && style == Detailed && node->type() == Node::Typedef) {
            const TypedefNode *typedeffe = static_cast<const TypedefNode *>(node);
            if (typedeffe->associatedEnum())
                irrelevant = true;
        }
    }

    if (!irrelevant) {
        if (status == Compat) {
            irrelevant = (node->status() != Node::Compat);
	} else if (status == Obsolete) {
            irrelevant = (node->status() != Node::Obsolete);
	} else {
            irrelevant = (node->status() == Node::Compat || node->status() == Node::Obsolete);
	}
    }

    if ( !irrelevant ) {
	if ( !inheritedMember || style == SeparateList ) {
	    QString key = sortName(node);
            if (!fastSection.memberMap.contains(key))
		fastSection.memberMap.insert(key, node);
	} else {
	    if ( node->parent()->type() == Node::Class ) {
		if (fastSection.inherited.isEmpty()
			|| fastSection.inherited.last().first != node->parent()) {
		    QPair<ClassNode *, int> p((ClassNode *)node->parent(), 0);
		    fastSection.inherited.append(p);
		}
		fastSection.inherited.last().second++;
	    }
	}
    }
}

void CodeMarker::append(QList<Section>& sectionList, const FastSection& fastSection)
{
    if ( !fastSection.memberMap.isEmpty() ||
	 !fastSection.inherited.isEmpty() ) {
	Section section(fastSection.name, fastSection.singularMember, fastSection.pluralMember);
	section.members = fastSection.memberMap.values();
	section.inherited = fastSection.inherited;
	sectionList.append( section );
    }
}

static QString encode(const QString &string)
{
#if 0
    QString result = string;

    for (int i = string.size() - 1; i >= 0; --i) {
        uint ch = string.at(i).unicode();
        if (ch > 0xFF)
            ch = '?';
        if ((ch - '0') >= 10 && (ch - 'a') >= 26 && (ch - 'A') >= 26
                && ch != '/' && ch != '(' && ch != ')' && ch != ',' && ch != '*'
                && ch != '&' && ch != '_' && ch != '<' && ch != '>' && ch != ':' && ch != '~')
            result.replace(i, 1, QString("%") + QString("%1").arg(ch, 2, 16));
    }
    return result;
#else
    return string;
#endif
}

QStringList CodeMarker::macRefsForNode(const Node *node)
{
    QString result = QLatin1String("cpp/");
    switch (node->type()) {
    case Node::Class:
        {
            const ClassNode *classe = static_cast<const ClassNode *>(node);
#if 0
            if (!classe->templateStuff().isEmpty()) {
                 result += QLatin1String("tmplt/");
            } else
#endif
            {
                 result += QLatin1String("cl/");
            }
            result += macName(classe); // ### Maybe plainName?
        }
        break;
    case Node::Enum:
        {
            QStringList stringList;
            stringList << encode(result + QLatin1String("tag/") + macName(node));
            foreach (const QString &enumName, node->doc().enumItemNames()) {
                // ### Write a plainEnumValue() and use it here
                stringList << encode(result + QLatin1String("econst/") + macName(node->parent(), enumName));
            }
            return stringList;
        }
    case Node::Typedef:
        result += QLatin1String("tdef/") + macName(node);
        break;
    case Node::Function:
        {
            bool isMacro = false;
            const FunctionNode *func = static_cast<const FunctionNode *>(node);

            if (func->isOverload()) // overloads are too clever for the Xcode documentation browser
                return QStringList();

            if (func->metaness() == FunctionNode::MacroWithParams
                || func->metaness() == FunctionNode::MacroWithoutParams) {
                result += QLatin1String("macro/");
                isMacro = true;
#if 0
            } else if (!func->templateStuff().isEmpty()) {
                result += QLatin1String("ftmplt/");
#endif
            } else if (func->isStatic()) {
                result += QLatin1String("clm/");
            } else if (!func->parent()->name().isEmpty()) {
                result += QLatin1String("instm/");
            } else {
                result += QLatin1String("func/");
            }

            result += macName(func);
            if (result.endsWith(QLatin1String("()")))
                result.chop(2);
#if 0
            // this code is too clever for the Xcode documentation browser and/or pbhelpindexer
            if (!isMacro) {
                result += "/" + QLatin1String(QMetaObject::normalizedSignature(func->returnType().toLatin1().constData())) + "/(";
                const QList<Parameter> &params = func->parameters();
                for (int i = 0; i < params.count(); ++i) {
                    QString type = params.at(i).leftType() + params.at(i).rightType();
                    type = QLatin1String(QMetaObject::normalizedSignature(type.toLatin1().constData()));
                    if (i != 0)
                        result += ",";
                    result += type;
                }
                result += ")";
            }
#endif
        }
        break;
    case Node::Variable:
         result += QLatin1String("data/") + macName(node);
         break;
    case Node::Property:
         {
             NodeList list = static_cast<const PropertyNode *>(node)->functions();
             QStringList stringList;
             foreach (const Node *node, list) {
                stringList += macRefsForNode(node);
             }
             return stringList;
         }
    case Node::Namespace:
    case Node::Fake:
    case Node::Target:
    default:
        return QStringList();
    }

    return QStringList(encode(result));
}

QString CodeMarker::macName(const Node *node, const QString &name)
{
    QString myName = name;
    if (myName.isEmpty()) {
        myName = node->name();
        node = node->parent();
    }

    if (node->name().isEmpty()) {
        return QLatin1Char('/') + myName;
    } else {
        return plainFullName(node) + QLatin1Char('/') + myName;
    }
}


QT_END_NAMESPACE
