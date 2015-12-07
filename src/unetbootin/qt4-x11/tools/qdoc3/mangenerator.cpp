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
	  << " " << protectArg( "Nokia Corporation and/or its subsidiary(-ies)" )
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
