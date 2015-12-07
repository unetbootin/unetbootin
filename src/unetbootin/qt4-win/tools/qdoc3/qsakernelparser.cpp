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
