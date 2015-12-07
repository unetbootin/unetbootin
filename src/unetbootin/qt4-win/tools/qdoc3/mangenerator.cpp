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
  mangenerator.cpp
*/

#include <qdatetime.h>
#include <qregexp.h>

#include "mangenerator.h"
#include "node.h"
#include "tree.h"

QT_BEGIN_NAMESPACE

ManGenerator::ManGenerator()
{
    date = QDate::currentDate().toString( "d MMMM yyyy" );
}

ManGenerator::~ManGenerator()
{
}

QString ManGenerator::format()
{
    return "man";
}

int ManGenerator::generateAtom( const Atom *atom, const Node * /* relative */,
				CodeMarker * /* marker */ )
{
#if 0
    switch ( atom->type() ) {
    case Atom::AbstractBegin:
	break;
    case Atom::AbstractEnd:
	break;
    case Atom::Alias:
	break;
    case Atom::AliasArg:
	break;
    case Atom::BaseName:
	break;
    case Atom::BriefBegin:
	break;
    case Atom::BriefEnd:
	break;
    case Atom::C:
	break;
    case Atom::CaptionBegin:
	break;
    case Atom::CaptionEnd:
	break;
    case Atom::CitationBegin:
	break;
    case Atom::CitationEnd:
	break;
    case Atom::Code:
	break;
    case Atom::FootnoteBegin:
	break;
    case Atom::FootnoteEnd:
	break;
    case Atom::FormatBegin:
	break;
    case Atom::FormatEnd:
	break;
    case Atom::GeneratedList:
	break;
    case Atom::Image:
	break;
    case Atom::ImageText:
	break;
    case Atom::Link:
	break;
    case Atom::LinkNode:
	break;
    case Atom::ListBegin:
	break;
    case Atom::ListItemNumber:
	break;
    case Atom::ListItemBegin:
	out() << ".IP " << atom->string() << ".\n";
	break;
    case Atom::ListItemEnd:
	break;
    case Atom::ListEnd:
	break;
    case Atom::Nop:
	break;
    case Atom::ParaBegin:
	out() << ".PP\n";
	break;
    case Atom::ParaEnd:
	out() << "\n";
	break;
    case Atom::RawFormat:
	break;
    case Atom::RawString:
	break;
    case Atom::SectionBegin:
	break;
    case Atom::SectionEnd:
	break;
    case Atom::SectionHeadingBegin:
	break;
    case Atom::SectionHeadingEnd:
	break;
    case Atom::SidebarBegin:
	break;
    case Atom::SidebarEnd:
	break;
    case Atom::String:
	out() << protectTextLine( atom->string() );
	break;
    case Atom::TableBegin:
	break;
    case Atom::TableEnd:
	break;
    case Atom::TableOfContents:
	break;
    case Atom::Target:
	break;
    case Atom::UnknownCommand:
	;
    }
#endif
    unknownAtom( atom );
    return 0;
}

void ManGenerator::generateClassLikeNode( const InnerNode *classe,
				      CodeMarker *marker )
{
    generateHeader( classe->name() );
    out() << ".SH NAME\n"
	  << classe->name() << "\n"
          << ".SH SYNOPSYS\n";
    generateBody( classe, marker );
    generateFooter();
}

void ManGenerator::generateFakeNode( const FakeNode *fake, CodeMarker *marker )
{
    generateHeader( "foo" );
    generateBody( fake, marker );
    generateFooter();
}

QString ManGenerator::fileExtension(const Node * /* node */)
{
    return "3qt";
}

void ManGenerator::generateHeader( const QString& name )
{
    out() << ".TH " << protectArg( name )
	  << " " << protectArg( "3qt" )
	  << " " << protectArg( date )
	  << " " << protectArg( "Trolltech ASA" )
	  << " " << protectArg( "Qt Toolkit" ) << "\n";
}

void ManGenerator::generateFooter()
{
}

QString ManGenerator::protectArg( const QString& str )
{
    for ( int i = 0; i < (int) str.length(); i++ ) {
	if ( str[i] == ' ' || str[i].isSpace() ) {
	    QString quoted = str;
	    quoted.replace( "\"", "\"\"" );
	    return "\"" + quoted + "\"";
	}
    }
    return str;
}

QString ManGenerator::protectTextLine( const QString& str )
{
    QString t = str;
    if ( t.startsWith(".") || t.startsWith("'") )
	t.prepend( "\\&" );
    return t;
}

QT_END_NAMESPACE
