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

#include <qfile.h>

#include "qsakernelparser.h"
#include "tokenizer.h"
#include "tree.h"

QT_BEGIN_NAMESPACE

QsaKernelParser::QsaKernelParser( Tree *cppTree )
    : cppTre( cppTree )
{
}

QsaKernelParser::~QsaKernelParser()
{
}

QString QsaKernelParser::language()
{
    return "QSA Kernel C++";
}

QString QsaKernelParser::sourceFileNameFilter()
{
    return "*.cpp";
}

void QsaKernelParser::parseSourceFile( const Location& location,
				       const QString& filePath,
				       Tree * /* tree */ )
{
    FILE *in = fopen( QFile::encodeName(filePath), "r" );
    if ( in == 0 ) {
        location.error( tr("Cannot open QSA kernel file '%1'").arg(filePath) );
        return;
    }

    Location fileLocation( filePath );
    Tokenizer fileTokenizer( fileLocation, in );
    tokenizer = &fileTokenizer;
    readToken();

    QString ident;
    QString className;
    int delimDepth = 0;

    while ( tok != Tok_Eoi ) {
	if ( tok == Tok_Ident ) {
	    ident = tokenizer->lexeme();
	    readToken();
	    if ( tok == Tok_Gulbrandsen && tokenizer->braceDepth() == 0 &&
		 tokenizer->parenDepth() == 0 ) {
		className = ident;
	    } else if ( ident.startsWith("add") && ident.endsWith("Member") &&
			tok == Tok_LeftParen ) {
		bool isProperty = ident.endsWith( "VariableMember" );
		bool isStatic = ident.startsWith( "addStatic" );
		bool isWritable = !isStatic;

		readToken();
		if ( tok == Tok_String ) {
		    QString member = tokenizer->lexeme();
		    member = member.mid( 1, member.length() - 2 );

		    readToken();
		    if ( tok == Tok_Comma )
			readToken();
		    if ( tok == Tok_Ident && tokenizer->lexeme() == "QSMember" )
			readToken();
		    if ( tok == Tok_LeftParen ) {
			delimDepth++;
			readToken();
		    }

		    while ( tok != Tok_Eoi && tok != Tok_RightParen &&
			    tok != Tok_Semicolon ) {
			if ( tok == Tok_Ident ) {
			    ident = tokenizer->lexeme();
			    if ( ident == "Custom" ) {
				isProperty = true;
			    } else if ( ident == "AttributeNonWritable" ) {
				isWritable = false;
			    } else if ( ident == "AttributeStatic" ) {
				isStatic = true;
			    }
			}
			readToken();
		    }

		    ClassNode *classe =
			    (ClassNode *) cppTre->findNode( QStringList(className),
							    Node::Class );
		    if ( classe == 0 ) {
			classe = new ClassNode( cppTre->root(), className );
			classe->setLocation( tokenizer->location() );
		    }

		    if ( isProperty ) {
			PropertyNode *property = new PropertyNode(classe, member);
			property->setLocation( tokenizer->location() );
			property->setDataType( "Object" );
#if 0
			property->setGetter( member );
			if ( isWritable ) {
			    QString setter = member;
			    setter[0] = setter[0].toUpper();
			    setter.prepend( "set" );
			    property->setSetter( setter );
			}
#endif
		    } else {
			FunctionNode *func = new FunctionNode( classe, member );
			func->setLocation( tokenizer->location() );
			func->setAccess( FunctionNode::Public );
			func->setMetaness( FunctionNode::Slot );
			if ( member == "toLocaleString" ||
			     member == "toString" ) {
			    func->setReturnType( "QString" );
			} else if ( member == "valueOf" ) {
			    func->setReturnType( "Object" );
			} else {
			    func->setReturnType( "Object" );
			    func->addParameter( Parameter("...") );
			}
			func->setStatic( false ); // ###
		    }
		}
	    }
	} else {
	    readToken();
	}
    }
    fclose( in );
}

void QsaKernelParser::doneParsingSourceFiles( Tree * /* tree */ )
{
}

void QsaKernelParser::readToken()
{
    tok = tokenizer->getToken();
}

QT_END_NAMESPACE
