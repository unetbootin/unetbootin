/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt Linguist of the Qt Toolkit.
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

#include "lupdate.h"
#include <metatranslator.h>

#include <QFile>
#include <QRegExp>
#include <QString>
#include <QStack>
#include <QTextCodec>
#include <QStack>
#include <QDebug>

#include <ctype.h>

QT_BEGIN_NAMESPACE

enum { Tok_Eof, Tok_class, Tok_return, Tok_tr,
       Tok_translate, Tok_Ident, Tok_Package,
       Tok_Comment, Tok_String, Tok_Colon, Tok_Dot,
       Tok_LeftBrace, Tok_RightBrace, Tok_LeftParen,
       Tok_RightParen, Tok_Comma, Tok_Semicolon,
       Tok_Integer, Tok_Plus, Tok_PlusPlus, Tok_PlusEq };

class Scope
{
    public:
        QString name;
        enum Type {Clazz, Function, Other} type;
        int line;

        Scope(const QString & name, Type type, int line) :
                name(name),
                type(type),
                line(line)
        {}

        ~Scope()
        {}
};

/*
  The tokenizer maintains the following global variables. The names
  should be self-explanatory.
*/

static QString yyFileName;
static QChar yyCh;
static QString yyIdent;
static QString yyComment;
static QString yyString;


static qlonglong yyInteger;
static int yyParenDepth;
static int yyLineNo;
static int yyCurLineNo;
static int yyParenLineNo;
static int yyTok;

// the file to read from (if reading from a file)
static QTextStream *yyInTextStream;

// the string to read from and current position in the string (otherwise)
static QString yyInStr;
static int yyInPos;

// The parser maintains the following global variables.
static QString yyPackage;
static QStack<Scope*> yyScope;
static QString yyDefaultContext;

static QChar getChar()
{
    ++yyInPos;
    if (yyInPos == yyInStr.length() || yyInStr.isEmpty()) {
        if( yyInTextStream->atEnd() )
            return 0;

        yyInStr = yyInTextStream->readLine();

        yyCurLineNo ++;
        yyInPos = -1;
        return QLatin1Char('\n');
    }
    return yyInStr.at( yyInPos );
}

static int getToken()
{
    const char tab[] = "bfnrt\"\'\\";
    const char backTab[] = "\b\f\n\r\t\"\'\\";

    yyIdent.clear();
    yyComment.clear();
    yyString.clear();

    while ( yyCh != 0 ) {
        yyLineNo = yyCurLineNo;

        if ( yyCh.isLetter() || yyCh.toLatin1() == '_' ) {
            do {
                yyIdent.append(yyCh);
                yyCh = getChar();
            } while ( yyCh.isLetterOrNumber() || yyCh.toLatin1() == '_' );

            if (yyTok != Tok_Dot) {
                switch ( yyIdent.at(0).toLatin1() ) {
                    case 'r':
                        if ( yyIdent == QLatin1String("return") )
                            return Tok_return;
                        break;
                     case 'c':
                        if ( yyIdent == QLatin1String("class") )
                            return Tok_class;
                    break;
                }
            }
            switch ( yyIdent.at(0).toLatin1() ) {
            case 'T':
                // TR() for when all else fails
                if ( yyIdent == QLatin1String("TR") )
                    return Tok_tr;
                break;
            case 'p':
                if( yyIdent == QLatin1String("package") )
                    return Tok_Package;
                break;
            case 't':
                if ( yyIdent == QLatin1String("tr") )
                    return Tok_tr;
                if ( yyIdent == QLatin1String("translate") )
                    return Tok_translate;
                }
            return Tok_Ident;
        } else {
            switch ( yyCh.toLatin1() ) {

            case '/':
                yyCh = getChar();
                if ( yyCh == QLatin1Char('/') ) {
                    do {
                        yyCh = getChar();
                        yyComment.append(yyCh);
                    } while ( yyCh != 0 && yyCh != QLatin1Char('\n') );
                    return Tok_Comment;

                } else if ( yyCh == QLatin1Char('*') ) {
                    bool metAster = false;
                    bool metAsterSlash = false;

                    while ( !metAsterSlash ) {
                        yyCh = getChar();
                        if ( yyCh == EOF ) {
                            qFatal( "%s: Unterminated Java comment starting at"
                                    " line %d\n",
                                    qPrintable(yyFileName), yyLineNo );

                            return Tok_Comment;
                        }

                        yyComment.append( yyCh );

                        if ( yyCh == QLatin1Char('*') )
                            metAster = true;
                        else if ( metAster && yyCh == QLatin1Char('/') )
                            metAsterSlash = true;
                        else
                            metAster = false;
                    }
                    yyCh = getChar();

                    return Tok_Comment;
                }
                break;
            case '"':
                yyCh = getChar();

                while ( yyCh != 0 && yyCh != QLatin1Char('\n') && yyCh != QLatin1Char('"') ) {
                    if ( yyCh == QLatin1Char('\\') ) {
                        yyCh = getChar();
                        if ( yyCh == QLatin1Char('u') ) {
                            yyCh = getChar();
                            uint unicode(0);
                            for (int i = 4; i > 0; --i) {
                                unicode = unicode << 4;
                                if( yyCh.isDigit() ) {
                                    unicode += yyCh.digitValue();
                                }
                                else {
                                    int sub(yyCh.toLower().toAscii() - 87);
                                    if( sub > 15 || sub < 10) {
                                        qFatal( "%s:%d: Invalid Unicode",
                                            qPrintable(yyFileName), yyLineNo );
                                    }
                                    unicode += sub;
                                }
                                yyCh = getChar();
                            }
                            yyString.append(QChar(unicode));
                        }
                        else if ( yyCh == QLatin1Char('\n') ) {
                            yyCh = getChar();
                        }
                        else {
                            yyString.append( QLatin1Char(backTab[strchr( tab, yyCh.toAscii() ) - tab]) );
                            yyCh = getChar();
                        }
                    } else {
                        yyString.append(yyCh);
                        yyCh = getChar();
                    }
                }

                if ( yyCh != QLatin1Char('"') )
                    qFatal( "%s:%d: Unterminated string",
                        qPrintable(yyFileName), yyLineNo );

                yyCh = getChar();

                return Tok_String;

            case ':':
                yyCh = getChar();
                return Tok_Colon;
            case '\'':
                yyCh = getChar();

                if ( yyCh == QLatin1Char('\\') )
                    yyCh = getChar();
                do {
                    yyCh = getChar();
                } while ( yyCh != 0 && yyCh != QLatin1Char('\'') );
                yyCh = getChar();
                break;
            case '{':
                yyCh = getChar();
                return Tok_LeftBrace;
            case '}':
                yyCh = getChar();
                return Tok_RightBrace;
            case '(':
                if (yyParenDepth == 0)
                    yyParenLineNo = yyCurLineNo;
                yyParenDepth++;
                yyCh = getChar();
                return Tok_LeftParen;
            case ')':
                if (yyParenDepth == 0)
                    yyParenLineNo = yyCurLineNo;
                yyParenDepth--;
                yyCh = getChar();
                return Tok_RightParen;
            case ',':
                yyCh = getChar();
                return Tok_Comma;
            case '.':
                yyCh = getChar();
                return Tok_Dot;
            case ';':
                yyCh = getChar();
                return Tok_Semicolon;
            case '+':
                yyCh = getChar();
                if (yyCh == QLatin1Char('+')) {
                    yyCh = getChar();
                    return Tok_PlusPlus;
		}
                if( yyCh == QLatin1Char('=') ){
                    yyCh = getChar();
                    return Tok_PlusEq;
		}
                return Tok_Plus;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                {
                    QByteArray ba;
                    ba += yyCh.toLatin1();
                    yyCh = getChar();
                    bool hex = yyCh == QLatin1Char('x');
                    if ( hex ) {
                        ba += yyCh.toLatin1();
                        yyCh = getChar();
                    }
                    while ( hex ? isxdigit(yyCh.toLatin1()) : yyCh.isDigit() ) {
                        ba += yyCh.toLatin1();
                        yyCh = getChar();
                    }
                    bool ok;
                    yyInteger = ba.toLongLong(&ok);
                    if (ok) return Tok_Integer;
                    break;
                }
            default:
                yyCh = getChar();
            }
        }
    }
    return Tok_Eof;
}

static bool match( int t )
{
    bool matches = ( yyTok == t );
    if ( matches )
        yyTok = getToken();
    return matches;
}

static bool matchString( QString &s )
{
    if ( yyTok != Tok_String )
        return false;

    s = yyString;
    yyTok = getToken();
    while ( yyTok == Tok_Plus ) {
        yyTok = getToken();
        if (yyTok == Tok_String)
            s += yyString;
        else {
            qWarning( "%s:%d: String used in translation can only contain strings"
                " concatenated with other strings, not expressions or numbers.",
                qPrintable(yyFileName), yyLineNo );
            return false;
        }
        yyTok = getToken();
    }
    return true;
}

static bool matchInteger( qlonglong *number)
{
    bool matches = (yyTok == Tok_Integer);
    if (matches) {
        yyTok = getToken();
        *number = yyInteger;
    }
    return matches;
}

static bool matchStringOrNull(QString &s)
{
    bool matches = matchString(s);
    qlonglong num = 0;
    if (!matches) matches = matchInteger(&num);
    return matches && num == 0;
}

/*
 * match any expression that can return a number, which can be
 * 1. Literal number (e.g. '11')
 * 2. simple identifier (e.g. 'm_count')
 * 3. simple function call (e.g. 'size()' )
 * 4. function call on an object (e.g. 'list.size()')
 * 5. function call on an object (e.g. 'list->size()')
 *
 * Other cases:
 * size(2,4)
 * list().size()
 * list(a,b).size(2,4)
 * etc...
 */
static bool matchExpression()
{
    if (match(Tok_Integer)) {
        return true;
    }

    int parenlevel = 0;
    while (match(Tok_Ident) || parenlevel > 0) {
        if (yyTok == Tok_RightParen) {
            if (parenlevel == 0) break;
            --parenlevel;
            yyTok = getToken();
        } else if (yyTok == Tok_LeftParen) {
            yyTok = getToken();
            if (yyTok == Tok_RightParen) {
                yyTok = getToken();
            } else {
                ++parenlevel;
            }
        } else if (yyTok == Tok_Ident) {
            continue;
        } else if (parenlevel == 0) {
            return false;
        }
    }
    return true;
}

static const QString context()
{
      QString context(yyPackage);
      bool innerClass = false;
      for (int i = 0; i < yyScope.size(); ++i) {
         if (yyScope.at(i)->type == Scope::Clazz) {
             if (innerClass)
                 context.append(QLatin1String("$"));
             else
                 context.append(QLatin1String("."));

             context.append(yyScope.at(i)->name);
             innerClass = true;
         }
     }
     return context.isEmpty() ? yyDefaultContext : context;
}

static void parse( MetaTranslator *tor )
{
    QString text;
    QString com;

    yyCh = getChar();

    yyTok = getToken();
    while ( yyTok != Tok_Eof ) {
        switch ( yyTok ) {
        case Tok_class:
            yyTok = getToken();
            if(yyTok == Tok_Ident) {
                yyScope.push(new Scope(yyIdent, Scope::Clazz, yyLineNo));
            }
            else {
                qFatal( "%s:%d: Class must be followed by a classname",
                                          qPrintable(yyFileName), yyLineNo );
            }
            while (!match(Tok_LeftBrace)) {
                yyTok = getToken();
            }
            break;

        case Tok_tr:
            yyTok = getToken();
            if ( match(Tok_LeftParen) && matchString(text) ) {
                com.clear();
                bool plural = false;

                if ( match(Tok_RightParen) ) {
                    // no comment
                } else if (match(Tok_Comma) && matchStringOrNull(com)) {   //comment
                    if ( match(Tok_RightParen)) {
                        // ok,
                    } else if (match(Tok_Comma)) {
                        plural = true;
                    }
                }
                tor->insert( TranslatorMessage(context().toUtf8(),
                    text.toUtf8(), com.toUtf8(), "", yyFileName, yyLineNo,
                    QStringList(), true, TranslatorMessage::Unfinished, plural) );
            }
            break;
        case Tok_translate:
            {
                QString contextOverride;
                yyTok = getToken();
                if ( match(Tok_LeftParen) &&
                     matchString(contextOverride) &&
                     match(Tok_Comma) &&
                     matchString(text) ) {

                    com.clear();
                    bool plural = false;
                    if (!match(Tok_RightParen)) {
                        // look for comment
                        if ( match(Tok_Comma) && matchStringOrNull(com)) {
                            if (!match(Tok_RightParen)) {
                                if (match(Tok_Comma) && matchExpression() && match(Tok_RightParen)) {
                                    plural = true;
                                } else {
                                    break;
                                }
                            }
                        } else {
                            break;
                        }
                    }
                    tor->insert( TranslatorMessage(contextOverride.toUtf8(),
                        text.toUtf8(), com.toUtf8(), "", yyFileName, yyLineNo,
                        QStringList(), true, TranslatorMessage::Unfinished, plural) );
                }
            }
            break;

        case Tok_Ident:
            yyTok = getToken();
            break;

        case Tok_RightBrace:
            if ( yyScope.isEmpty() ) {
                qFatal( "%s:%d: Unbalanced right brace in Java code\n",
                        qPrintable(yyFileName), yyLineNo );
            }
            else
                delete (yyScope.pop());
            yyTok = getToken();
            break;

         case Tok_LeftBrace:
            yyScope.push(new Scope(QString(), Scope::Other, yyLineNo));
            yyTok = getToken();
            break;

        case Tok_Semicolon:
            yyTok = getToken();
            break;

        case Tok_Package:
            yyTok = getToken();
            while(!match(Tok_Semicolon)) {
                switch(yyTok) {
                    case Tok_Ident:
                        yyPackage.append(yyIdent);
                        break;
                    case Tok_Dot:
                        yyPackage.append(QLatin1String("."));
                        break;
                    default:
                         qFatal( "%s:%d: Package keyword should be followed by com.package.name;",
                                          qPrintable(yyFileName), yyLineNo );
                         break;
                }
                yyTok = getToken();
            }
            break;

        default:
            yyTok = getToken();
        }
    }

    if ( !yyScope.isEmpty() )
        qFatal( "%s:%d: Unbalanced braces in Java code\n",
                 qPrintable(yyFileName), yyScope.top()->line );
    else if ( yyParenDepth != 0 )
        qFatal( "%s:%d: Unbalanced parentheses in Java code\n",
                 qPrintable(yyFileName), yyParenLineNo );
}

void LupdateApplication::fetchtr_java( const QString &fileName, MetaTranslator *tor,
    const QString &defaultContext, bool mustExist, const QByteArray &codecForSource )
{
    yyDefaultContext = defaultContext;
    yyInPos = -1;
    yyFileName = fileName;
    yyPackage.clear();
    yyInStr.clear();
    yyScope.clear();
    yyTok = -1;
    yyParenDepth = 0;
    yyCurLineNo = 0;
    yyParenLineNo = 1;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if( mustExist )
            qFatal( "lupdate error: Cannot open java source file '%s'\n",
                qPrintable(fileName));
    }

    yyInTextStream = new QTextStream( &file );

    if (!codecForSource.isEmpty()) {
        yyInTextStream->setCodec( QTextCodec::codecForName(codecForSource) );
    }

    parse( tor );

    delete( yyInTextStream );
}

QT_END_NAMESPACE
