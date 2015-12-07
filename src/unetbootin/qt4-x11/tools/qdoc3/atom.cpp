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

#include <qregexp.h>
#include "atom.h"
#include "location.h"
#include <stdio.h>

QT_BEGIN_NAMESPACE

QString Atom::BOLD_          ("bold");
QString Atom::INDEX_         ("index");
QString Atom::ITALIC_        ("italic");
QString Atom::LINK_          ("link");
QString Atom::PARAMETER_     ("parameter");
QString Atom::SUBSCRIPT_     ("subscript");
QString Atom::SUPERSCRIPT_   ("superscript");
QString Atom::TELETYPE_      ("teletype");
QString Atom::UNDERLINE_     ("underline");

QString Atom::BULLET_        ("bullet");
QString Atom::TAG_           ("tag");
QString Atom::VALUE_         ("value");
QString Atom::LOWERALPHA_    ("loweralpha");
QString Atom::LOWERROMAN_    ("lowerroman");
QString Atom::NUMERIC_       ("numeric");
QString Atom::UPPERALPHA_    ("upperalpha");
QString Atom::UPPERROMAN_    ("upperroman");

/*! \class Atom
    \brief The Atom class is the fundamental unit for representing
    documents internally.

  Atoms have a \i type and are completed by a \i string whose
  meaning depends on the \i type. For example, the string
  \quotation
      \i italic text looks nicer than \bold bold text
  \endquotation
  is represented by the following atoms:
  \quotation
      (FormattingLeft, ATOM_FORMATTING_ITALIC)
      (String, "italic")
      (FormattingRight, ATOM_FORMATTING_ITALIC)
      (String, " text is more attractive than ")
      (FormattingLeft, ATOM_FORMATTING_BOLD)
      (String, "bold")
      (FormattingRight, ATOM_FORMATTING_BOLD)
      (String, " text")
  \endquotation

  \also Text
*/

/*! \enum Atom::Type

  \value AbstractLeft
  \value AbstractRight
  \value AutoLink
  \value BaseName
  \value BriefLeft
  \value BriefRight
  \value C
  \value CaptionLeft
  \value CaptionRight
  \value Code
  \value CodeBad
  \value CodeNew
  \value CodeOld
  \value CodeQuoteArgument
  \value CodeQuoteCommand
  \value FormatElse
  \value FormatEndif
  \value FormatIf
  \value FootnoteLeft
  \value FootnoteRight
  \value FormattingLeft
  \value FormattingRight
  \value GeneratedList
  \value Image
  \value ImageText
  \value InlineImage
  \value LineBreak
  \value Link
  \value LinkNode
  \value ListLeft
  \value ListItemNumber
  \value ListTagLeft
  \value ListTagRight
  \value ListItemLeft
  \value ListItemRight
  \value ListRight
  \value Nop
  \value ParaLeft
  \value ParaRight
  \value QuotationLeft
  \value QuotationRight
  \value RawString
  \value SectionLeft
  \value SectionRight
  \value SectionHeadingLeft
  \value SectionHeadingRight
  \value SidebarLeft
  \value SidebarRight
  \value String
  \value TableLeft
  \value TableRight
  \value TableHeaderLeft
  \value TableHeaderRight
  \value TableRowLeft
  \value TableRowRight
  \value TableItemLeft
  \value TableItemRight
  \value TableOfContents
  \value Target
  \value UnhandledFormat
  \value UnknownCommand
*/

static const struct {
    const char *english;
    int no;
} atms[] = {
    { "AbstractLeft", Atom::AbstractLeft },
    { "AbstractRight", Atom::AbstractRight },
    { "AutoLink", Atom::AutoLink },
    { "BaseName", Atom::BaseName },
    { "BriefLeft", Atom::BriefLeft },
    { "BriefRight", Atom::BriefRight },
    { "C", Atom::C },
    { "CaptionLeft", Atom::CaptionLeft },
    { "CaptionRight", Atom::CaptionRight },
    { "Code", Atom::Code },
    { "CodeBad", Atom::CodeBad },
    { "CodeNew", Atom::CodeNew },
    { "CodeOld", Atom::CodeOld },
    { "CodeQuoteArgument", Atom::CodeQuoteArgument },
    { "CodeQuoteCommand", Atom::CodeQuoteCommand },
    { "FootnoteLeft", Atom::FootnoteLeft },
    { "FootnoteRight", Atom::FootnoteRight },
    { "FormatElse", Atom::FormatElse },
    { "FormatEndif", Atom::FormatEndif },
    { "FormatIf", Atom::FormatIf },
    { "FormattingLeft", Atom::FormattingLeft },
    { "FormattingRight", Atom::FormattingRight },
    { "GeneratedList", Atom::GeneratedList },
    { "Image", Atom::Image },
    { "ImageText", Atom::ImageText },
    { "InlineImage", Atom::InlineImage },
    { "LegaleseLeft", Atom::LegaleseLeft },
    { "LegaleseRight", Atom::LegaleseRight },
    { "LineBreak", Atom::LineBreak },
    { "Link", Atom::Link },
    { "LinkNode", Atom::LinkNode },
    { "ListLeft", Atom::ListLeft },
    { "ListItemNumber", Atom::ListItemNumber },
    { "ListTagLeft", Atom::ListTagLeft },
    { "ListTagRight", Atom::ListTagRight },
    { "ListItemLeft", Atom::ListItemLeft },
    { "ListItemRight", Atom::ListItemRight },
    { "ListRight", Atom::ListRight },
    { "Nop", Atom::Nop },
    { "ParaLeft", Atom::ParaLeft },
    { "ParaRight", Atom::ParaRight },
    { "QuotationLeft", Atom::QuotationLeft },
    { "QuotationRight", Atom::QuotationRight },
    { "RawString", Atom::RawString },
    { "SectionLeft", Atom::SectionLeft },
    { "SectionRight", Atom::SectionRight },
    { "SectionHeadingLeft", Atom::SectionHeadingLeft },
    { "SectionHeadingRight", Atom::SectionHeadingRight },
    { "SidebarLeft", Atom::SidebarLeft },
    { "SidebarRight", Atom::SidebarRight },
    { "SnippetCommand", Atom::SnippetCommand },
    { "SnippetIdentifier", Atom::SnippetIdentifier },
    { "SnippetLocation", Atom::SnippetLocation },
    { "String", Atom::String },
    { "TableLeft", Atom::TableLeft },
    { "TableRight", Atom::TableRight },
    { "TableHeaderLeft", Atom::TableHeaderLeft },
    { "TableHeaderRight", Atom::TableHeaderRight },
    { "TableRowLeft", Atom::TableRowLeft },
    { "TableRowRight", Atom::TableRowRight },
    { "TableItemLeft", Atom::TableItemLeft },
    { "TableItemRight", Atom::TableItemRight },
    { "TableOfContents", Atom::TableOfContents },
    { "Target", Atom::Target },
    { "UnhandledFormat", Atom::UnhandledFormat },
    { "UnknownCommand", Atom::UnknownCommand },
    { 0, 0 }
};

/*! \fn Atom::Atom( Type type, const QString& string )

  Constructs an atom (\a type, \a string) outside of any atom list.
*/

/*! \fn Atom( Atom *prev, Type type, const QString& string )

  Constructs an atom (\a type, \a string) that follows \a prev in \a
  prev's atom list.
*/

/*! \fn void Atom::appendChar( QChar ch )

  Appends \a ch to the string parameter of this atom.

  \also string()
*/

/*! \fn void Atom::appendString( const QString& string )

  Appends \a string to the string parameter of this atom.

  \also string()
*/

/*! \fn void Atom::chopString()

  \also string()
*/

/*! \fn Atom *Atom::next()
  Return the next atom in the atom list.
  \also type(), string()
*/

/*!
  Return the next Atom in the list if it is of Type \a t.
  Otherwise return 0.
 */
const Atom* Atom::next(Type t) const
{
    return (nxt && (nxt->type() == t)) ? nxt : 0;
}

/*!
  Return the next Atom in the list if it is of Type \a t
  and its string part is \a s. Otherwise return 0.
 */
const Atom* Atom::next(Type t, const QString& s) const
{
    return (nxt && (nxt->type() == t) && (nxt->string() == s)) ? nxt : 0;
}

/*! \fn const Atom *Atom::next() const
  Return the next atom in the atom list.
  \also type(), string()
*/

/*! \fn Type Atom::type() const
  Return the type of this atom.
  \also string(), next()
*/

/*!
  Return the type of this atom as a string. Return "Invalid" if
  type() returns an impossible value.

  This is only useful for debugging.

  \also type()
*/
QString Atom::typeString() const
{
    static bool deja = false;

    if ( !deja ) {
	int i = 0;
	while ( atms[i].english != 0 ) {
	    if ( atms[i].no != i )
		Location::internalError( tr("atom %1 missing").arg(i) );
	    i++;
	}
	deja = true;
    }

    int i = (int) type();
    if ( i < 0 || i > (int) Last )
        return QLatin1String("Invalid");
    return QLatin1String(atms[i].english);
}

/*! \fn const QString& Atom::string() const

  Returns the string parameter that together with the type
  characterizes this atom.

  \also type(), next()
*/

/*!
  Dumps this Atom to stderr in printer friendly form.
 */
void Atom::dump() const
{
    QString str = string();
    str.replace( "\\", "\\\\" );
    str.replace( "\"", "\\\"" );
    str.replace( "\n", "\\n" );
    str.replace( QRegExp("[^\x20-\x7e]"), "?" );
    if (!str.isEmpty())
        str = " \"" + str + "\"";
    fprintf(stderr,
            "    %-15s%s\n",
            typeString().toLatin1().data(),
            str.toLatin1().data());
}

QT_END_NAMESPACE
