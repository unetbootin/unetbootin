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

#include "config.h"
#include "location.h"

#include <qregexp.h>
#include <qtranslator.h>
#include <stdlib.h>
#include <limits.h>

#include <stdio.h>

QT_BEGIN_NAMESPACE

QT_STATIC_CONST_IMPL Location Location::null;

int Location::tabSize;
QString Location::programName;
QRegExp *Location::spuriousRegExp = 0;


/*!
  Constructs an empty location.
*/
Location::Location()
    : stk( 0 ), stkTop( &stkBottom ), stkDepth( 0 ), etcetera( false )
{
}

/*!

*/
Location::Location( const QString& fileName )
    : stk( 0 ), stkTop( &stkBottom ), stkDepth( 0 ), etcetera( false )
{
    push( fileName );
}

Location::Location( const Location& other )
    : stk( 0 ), stkTop( &stkBottom ), stkDepth( 0 ), etcetera( false )
{
    *this = other;
}

Location& Location::operator=( const Location& other )
{
    QStack<StackEntry> *oldStk = stk;

    stkBottom = other.stkBottom;
    if ( other.stk == 0 ) {
	stk = 0;
	stkTop = &stkBottom;
    } else {
	stk = new QStack<StackEntry>( *other.stk );
	stkTop = &stk->top();
    }
    stkDepth = other.stkDepth;
    etcetera = other.etcetera;
    delete oldStk;
    return *this;
}

void Location::start()
{
    if ( stkTop->lineNo < 1 ) {
	stkTop->lineNo = 1;
	stkTop->columnNo = 1;
    }
}

/*!
  Advances the current location with character \a ch. If \a ch is
  '\\n' or '\\t', the line and column numbers are updated correctly.
*/
void Location::advance( QChar ch )
{
    if ( ch == QLatin1Char('\n') ) {
        stkTop->lineNo++;
        stkTop->columnNo = 1;
    } else if ( ch == QLatin1Char('\t') ) {
        stkTop->columnNo = 1 + tabSize * ( stkTop->columnNo + tabSize - 1 )
                       / tabSize;
    } else {
        stkTop->columnNo++;
    }
}

/*!
  Pushes \a filePath onto the file position stack. The current
  location becomes (\a filePath, 1, 1).

  \sa pop()
*/
void Location::push( const QString& filePath )
{
    if ( stkDepth++ >= 1 ) {
	if ( stk == 0 )
	    stk = new QStack<StackEntry>;
	stk->push( StackEntry() );
	stkTop = &stk->top();
    }

    stkTop->filePath = filePath;
    stkTop->lineNo = INT_MIN;
    stkTop->columnNo = 1;
}

/*!
  Pops the top of the internal stack. The current location becomes
  what it was just before the corresponding push().

  \sa push()
*/
void Location::pop()
{
    if ( --stkDepth == 0 ) {
	stkBottom = StackEntry();
    } else {
	stk->pop();
	if ( stk->isEmpty() ) {
	    delete stk;
	    stk = 0;
	    stkTop = &stkBottom;
	} else {
	    stkTop = &stk->top();
	}
    }
}

/*! \fn bool Location::isEmpty() const

  Returns true if there is no file name set yet; returns false
  otherwise. The functions filePath(), lineNo() and columnNo()
  may only be called on non-empty objects.
*/

/*! \fn const QString& Location::filePath() const

  Returns the current path and file name.

  \sa lineNo(), columnNo()
*/

/*!
  ###
*/
QString Location::fileName() const
{
    QString fp = filePath();
    return fp.mid( fp.lastIndexOf('/') + 1 );
}

/*! \fn int Location::lineNo() const

  Returns the current line number.

  \sa filePath(), columnNo()
*/

/*! \fn int Location::columnNo() const

  Returns the current column number.

  \sa filePath(), lineNo()
*/

void Location::warning( const QString& message, const QString& details ) const
{
    emitMessage( Warning, message, details );
}

void Location::error( const QString& message, const QString& details ) const
{
    emitMessage( Error, message, details );
}

void Location::fatal( const QString& message, const QString& details ) const
{
    emitMessage( Error, message, details );
    information( "Aborting" );
    exit( EXIT_FAILURE );
}

/*!

*/
void Location::initialize( const Config& config )
{
    tabSize = config.getInt( CONFIG_TABSIZE );
    programName = config.programName();

    QRegExp regExp = config.getRegExp( CONFIG_SPURIOUS );
    if ( regExp.isValid() ) {
	spuriousRegExp = new QRegExp( regExp );
    } else {
	config.lastLocation().warning( tr("Invalid regular expression '%1'")
				       .arg(regExp.pattern()) );
    }
}

void Location::terminate()
{
    delete spuriousRegExp;
    spuriousRegExp = 0;
}

void Location::information( const QString& message )
{
    printf( "%s\n", message.toLatin1().data() );
    fflush( stdout );
}

void Location::internalError( const QString& hint )
{
    Location::null.fatal( tr("Internal error (%1)").arg(hint),
			  tr("There is a bug in %1. Seek advice from your local"
			     " %2 guru.")
			  .arg(programName).arg(programName) );
}

void Location::emitMessage( MessageType type, const QString& message,
			    const QString& details ) const
{
    if ( type == Warning && spuriousRegExp != 0 && spuriousRegExp->exactMatch(message) )
	return;

    QString result = message;
    if ( !details.isEmpty() )
	result += "\n[" + details + "]";
    result.replace( "\n", "\n    " );
    if ( type == Error )
	result.prepend( tr("error: ") );
    result.prepend( toString() );
    fprintf( stderr, "%s\n", result.toLatin1().data() );
    fflush( stderr );
}

QString Location::toString() const
{
    QString str;

    if ( isEmpty() ) {
        str = programName;
    } else {
        Location loc2 = *this;
        loc2.setEtc( false );
        loc2.pop();
        if ( !loc2.isEmpty() ) {
            QString blah = tr( "In file included from " );
            for ( ;; ) {
                str += blah;
                str += loc2.top();
                loc2.pop();
                if ( loc2.isEmpty() )
                    break;
                str += tr( "," );
                str += QLatin1Char('\n');
                blah.fill( ' ' );
            }
            str += tr( ":" );
            str += QLatin1Char('\n');
        }
        str += top();
    }
    str += QLatin1String(": ");
    return str;
}

QString Location::top() const
{
    QString str = filePath();
    if ( lineNo() >= 1 ) {
        str += QLatin1Char(':'); 
        str += QString::number( lineNo() );
#if 0
        if ( columnNo() >= 1 )
            str += ":" + QString::number( columnNo() );
#endif
    }
    if ( etc() )
        str += QLatin1String(" (etc.)");
    return str;
}

QT_END_NAMESPACE
