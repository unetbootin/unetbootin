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
#include "metatranslator.h"

#include <QDebug>
#include <QFile>
#include <QRegExp>
#include <QString>
#include <QStack>
#include <QTextCodec>
#include <QXmlAttributes>
#include <QXmlDefaultHandler>
#include <QXmlLocator>
#include <QXmlParseException>

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

QT_BEGIN_NAMESPACE

/* qmake ignore Q_OBJECT */

static const char MagicComment[] = "TRANSLATOR ";

static QMap<QByteArray, int> needs_Q_OBJECT;
static QMap<QByteArray, int> lacks_Q_OBJECT;

//#define DIAGNOSE_RETRANSLATABILITY
/*
  The first part of this source file is the C++ tokenizer.  We skip
  most of C++; the only tokens that interest us are defined here.
  Thus, the code fragment

      int main()
      {
          printf( "Hello, world!\n" );
          return 0;
      }

  is broken down into the following tokens (Tok_ omitted):

      Ident Ident LeftParen RightParen
      LeftBrace
          Ident LeftParen String RightParen Semicolon
          return Semicolon
      RightBrace.

  The 0 doesn't produce any token.
*/

enum { Tok_Eof, Tok_class, Tok_namespace, Tok_return, Tok_tr,
       Tok_trUtf8, Tok_translate, Tok_Q_OBJECT, Tok_Ident,
       Tok_Comment, Tok_String, Tok_Arrow, Tok_Colon,
       Tok_ColonColon, Tok_LeftBrace, Tok_RightBrace, Tok_LeftParen,
       Tok_RightParen, Tok_Comma, Tok_Semicolon, Tok_Integer,
       Tok_Other };

/*
  The tokenizer maintains the following global variables. The names
  should be self-explanatory.
*/
static QString yyFileName;
static int yyCh;
static char yyIdent[128];
static size_t yyIdentLen;
static char yyComment[65536];
static size_t yyCommentLen;
static char yyString[65536];
static size_t yyStringLen;
static qlonglong yyInteger;
static QStack<int> yySavedBraceDepth;
static QStack<int> yySavedParenDepth;
static int yyBraceDepth;
static int yyParenDepth;
static int yyLineNo;
static int yyCurLineNo;
static int yyBraceLineNo;
static int yyParenLineNo;
static bool yyTokColonSeen = false;
static QTextCodec *yyCodecForTr = 0;
static QTextCodec *yyCodecForSource = 0;

// the file to read from (if reading from a file)
static FILE *yyInFile;

// the string to read from and current position in the string (otherwise)
static QString yyInStr;
static int yyInPos;

static int (*getChar)();

static bool yyParsingUtf8;

static int getCharFromFile()
{
    int c = getc( yyInFile );
    if ( c == '\n' )
        yyCurLineNo++;
    return c;
}

static int getCharFromString()
{
    if ( yyInPos == (int) yyInStr.length() ) {
        return EOF;
    } else {
        return yyInStr[yyInPos++].toLatin1();
    }
}

static void startTokenizer(const QString &fileName, int (*getCharFunc)(),
    QTextCodec *codecForTr, QTextCodec *codecForSource )
{
    yyInPos = 0;
    getChar = getCharFunc;

    yyFileName = fileName;
    yySavedBraceDepth.clear();
    yySavedParenDepth.clear();
    yyBraceDepth = 0;
    yyParenDepth = 0;
    yyCurLineNo = 1;
    yyBraceLineNo = 1;
    yyParenLineNo = 1;
    yyCh = getChar();
	yyCodecForTr = codecForTr;
	if (!yyCodecForTr)
		yyCodecForTr = QTextCodec::codecForName("ISO-8859-1");
	Q_ASSERT(yyCodecForTr);
	yyCodecForSource = codecForSource;

	yyParsingUtf8 = false;
}

static int getToken()
{
    const char tab[] = "abfnrtv";
    const char backTab[] = "\a\b\f\n\r\t\v";
    uint n;
	bool quiet;

    yyIdentLen = 0;
    yyCommentLen = 0;
    yyStringLen = 0;

    while ( yyCh != EOF ) {
        yyLineNo = yyCurLineNo;

        if ( isalpha(yyCh) || yyCh == '_' ) {
            do {
                if ( yyIdentLen < sizeof(yyIdent) - 1 )
                    yyIdent[yyIdentLen++] = (char) yyCh;
                yyCh = getChar();
            } while ( isalnum(yyCh) || yyCh == '_' );
            yyIdent[yyIdentLen] = '\0';

            switch ( yyIdent[0] ) {
            case 'Q':
                if ( strcmp(yyIdent + 1, "_OBJECT") == 0 )
                    return Tok_Q_OBJECT;
                if ( strcmp(yyIdent + 1, "T_TR_NOOP") == 0 ) {
					yyParsingUtf8 = false;
                    return Tok_tr;
                }
                if ( strcmp(yyIdent + 1, "T_TRANSLATE_NOOP") == 0 ) {
					yyParsingUtf8 = false;
                    return Tok_translate;
                }
                if ( strcmp(yyIdent + 1, "T_TRANSLATE_NOOP3") == 0 ) {
					yyParsingUtf8 = false;
                    return Tok_translate;
                }
                break;
            case 'T':
                // TR() for when all else fails
				if ( qstricmp(yyIdent + 1, "R") == 0 ) {
					yyParsingUtf8 = false;
                    return Tok_tr;
				}
                break;
            case 'c':
                if ( strcmp(yyIdent + 1, "lass") == 0 )
                    return Tok_class;
                break;
            case 'f':
                /*
                  QTranslator::findMessage() has the same parameters as
                  QApplication::translate().
                */
                if ( strcmp(yyIdent + 1, "indMessage") == 0 )
                    return Tok_translate;
                break;
            case 'n':
                if ( strcmp(yyIdent + 1, "amespace") == 0 )
                    return Tok_namespace;
                break;
            case 'r':
                if ( strcmp(yyIdent + 1, "eturn") == 0 )
                    return Tok_return;
                break;
            case 's':
                if ( strcmp(yyIdent + 1, "truct") == 0 )
                    return Tok_class;
                break;
            case 't':
                if ( strcmp(yyIdent + 1, "r") == 0 ) {
					yyParsingUtf8 = false;
                    return Tok_tr;
                }
                if ( qstrcmp(yyIdent + 1, "rUtf8") == 0 ) {
					yyParsingUtf8 = true;
                    return Tok_trUtf8;
                }
                if ( qstrcmp(yyIdent + 1, "ranslate") == 0 ) {
					yyParsingUtf8 = false;
                    return Tok_translate;
                }
            }
            return Tok_Ident;
        } else {
            switch ( yyCh ) {
            case '#':
                /*
                  Early versions of lupdate complained about
                  unbalanced braces in the following code:

                      #ifdef ALPHA
                          while ( beta ) {
                      #else
                          while ( gamma ) {
                      #endif
                              delta;
                          }

                  The code contains, indeed, two opening braces for
                  one closing brace; yet there's no reason to panic.

                  The solution is to remember yyBraceDepth as it was
                  when #if, #ifdef or #ifndef was met, and to set
                  yyBraceDepth to that value when meeting #elif or
                  #else.
                */
                do {
                    yyCh = getChar();
                } while ( isspace(yyCh) && yyCh != '\n' );

                switch ( yyCh ) {
                case 'i':
                    yyCh = getChar();
                    if ( yyCh == 'f' ) {
                        // if, ifdef, ifndef
                        yySavedBraceDepth.push( yyBraceDepth );
                        yySavedParenDepth.push( yyParenDepth );
                    }
                    break;
                case 'e':
                    yyCh = getChar();
                    if ( yyCh == 'l' ) {
                        // elif, else
                        if ( !yySavedBraceDepth.isEmpty() ) {
                            yyBraceDepth = yySavedBraceDepth.top();
                            yyParenDepth = yySavedParenDepth.top();
                        }
                    } else if ( yyCh == 'n' ) {
                        // endif
                        if ( !yySavedBraceDepth.isEmpty() ) {
                            yySavedBraceDepth.pop();
                            yySavedParenDepth.pop();
                        }
                    }
                }
                while ( isalnum(yyCh) || yyCh == '_' )
                    yyCh = getChar();
                break;
            case '/':
                yyCh = getChar();
                if ( yyCh == '/' ) {
                    do {
                        yyCh = getChar();
                    } while ( yyCh != EOF && yyCh != '\n' );
                } else if ( yyCh == '*' ) {
                    bool metAster = false;
                    bool metAsterSlash = false;

                    while ( !metAsterSlash ) {
                        yyCh = getChar();
                        if ( yyCh == EOF ) {
                            qWarning( "%s: Unterminated C++ comment starting at"
                                     " line %d\n",
                                     qPrintable(yyFileName), yyLineNo );
                            yyComment[yyCommentLen] = '\0';
                            return Tok_Comment;
                        }
                        if ( yyCommentLen < sizeof(yyComment) - 1 )
                            yyComment[yyCommentLen++] = (char) yyCh;

                        if ( yyCh == '*' )
                            metAster = true;
                        else if ( metAster && yyCh == '/' )
                            metAsterSlash = true;
                        else
                            metAster = false;
                    }
                    yyCh = getChar();
                    yyCommentLen -= 2;
                    yyComment[yyCommentLen] = '\0';
                    return Tok_Comment;
                }
                break;
            case '"':
                yyCh = getChar();
				quiet = false;

                while ( yyCh != EOF && yyCh != '\n' && yyCh != '"' ) {
                    if ( yyCh == '\\' ) {
                        yyCh = getChar();

                        if ( yyCh == '\n' ) {
                            yyCh = getChar();
                        } else if ( yyCh == 'x' ) {
                            QByteArray hex = "0";

                            yyCh = getChar();
                            while ( isxdigit(yyCh) ) {
                                hex += (char) yyCh;
                                yyCh = getChar();
                            }
#if defined(_MSC_VER) && _MSC_VER >= 1400
							sscanf_s( hex, "%x", &n );
#else
                            sscanf( hex, "%x", &n );
#endif
                            if ( yyStringLen < sizeof(yyString) - 1 )
                                yyString[yyStringLen++] = (char) n;
                        } else if ( yyCh >= '0' && yyCh < '8' ) {
                            QByteArray oct = "";
                            int n = 0;

                            do {
                                oct += (char) yyCh;
                                ++n;
                                yyCh = getChar();
                            } while ( yyCh >= '0' && yyCh < '8' && n < 3 );
#if defined(_MSC_VER) && _MSC_VER >= 1400
							sscanf_s( oct, "%o", &n );
#else
                            sscanf( oct, "%o", &n );
#endif
                            if ( yyStringLen < sizeof(yyString) - 1 )
                                yyString[yyStringLen++] = (char) n;
                        } else {
                            const char *p = strchr( tab, yyCh );
                            if ( yyStringLen < sizeof(yyString) - 1 )
                                yyString[yyStringLen++] = ( p == 0 ) ?
                                        (char) yyCh : backTab[p - tab];
                            yyCh = getChar();
                        }
                    } else {
						if (!yyCodecForSource) {
							if ( yyParsingUtf8 && yyCh >= 0x80 && !quiet) {
								qWarning( "%s:%d: Non-ASCII character detected in trUtf8 string",
										  qPrintable(yyFileName), yyLineNo );
								quiet = true;
							}
							// common case: optimized
							if ( yyStringLen < sizeof(yyString) - 1 )
								yyString[yyStringLen++] = (char) yyCh;
							yyCh = getChar();
						} else {
							QByteArray originalBytes;
							while ( yyCh != EOF && yyCh != '\n' && yyCh != '"' && yyCh != '\\' ) {
								if ( yyParsingUtf8 && yyCh >= 0x80 && !quiet) {
									qWarning( "%s:%d: Non-ASCII character detected in trUtf8 string",
											qPrintable(yyFileName), yyLineNo );
									quiet = true;
								}
								originalBytes += (char)yyCh;
								yyCh = getChar();
							}

							QString unicodeStr = yyCodecForSource->toUnicode(originalBytes);
							QByteArray convertedBytes;

							if (!yyCodecForTr->canEncode(unicodeStr) && !quiet) {
								qWarning( "%s:%d: Cannot convert C++ string from %s to %s",
										  qPrintable(yyFileName), yyLineNo,
                                          yyCodecForSource->name().constData(),
										  yyCodecForTr->name().constData() );
								quiet = true;
							}
							convertedBytes = yyCodecForTr->fromUnicode(unicodeStr);

							size_t len = qMin((size_t)convertedBytes.size(), sizeof(yyString) - yyStringLen - 1);
							memcpy(yyString + yyStringLen, convertedBytes.constData(), len);
							yyStringLen += len;
						}
                    }
                }
                yyString[yyStringLen] = '\0';

                if ( yyCh != '"' )
                    qWarning( "%s:%d: Unterminated C++ string",
                              qPrintable(yyFileName), yyLineNo );

                if ( yyCh == EOF ) {
                    return Tok_Eof;
                } else {
                    yyCh = getChar();
                    return Tok_String;
                }
                break;
            case '-':
                yyCh = getChar();
                if ( yyCh == '>' ) {
                    yyCh = getChar();
                    return Tok_Arrow;
                }
                break;
            case ':':
                yyCh = getChar();
                if ( yyCh == ':' ) {
                    yyCh = getChar();
                    return Tok_ColonColon;
                }
                return Tok_Colon;
            // Incomplete: '<' might be part of '<=' or of template syntax.
            // The main intent of not completely ignoring it is to break
            // parsing of things like   std::cout << QObject::tr()  as
            // context std::cout::QObject (see Task 161106)
            case '=':
            case '>':
            case '<':
                yyCh = getChar();
                return Tok_Other;
            case '\'':
                yyCh = getChar();
                if ( yyCh == '\\' )
                    yyCh = getChar();

                do {
                    yyCh = getChar();
                } while ( yyCh != EOF && yyCh != '\'' );
                yyCh = getChar();
                break;
            case '{':
                if (yyBraceDepth == 0)
                    yyBraceLineNo = yyCurLineNo;
                yyBraceDepth++;
                yyCh = getChar();
                return Tok_LeftBrace;
            case '}':
                if (yyBraceDepth == 0)
                    yyBraceLineNo = yyCurLineNo;
                yyBraceDepth--;
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
            case ';':
                yyCh = getChar();
                return Tok_Semicolon;
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
                    ba+=yyCh;
                    yyCh = getChar();
                    bool hex = yyCh == 'x';
                    if ( hex ) {
                        ba+=yyCh;
                        yyCh = getChar();
                    }
                    while ( (hex ? isxdigit(yyCh) : isdigit(yyCh)) ) {
                        ba+=yyCh;
                        yyCh = getChar();
                    }
                    bool ok;
                    yyInteger = ba.toLongLong(&ok);
                    if (ok) return Tok_Integer;
                    break;
                }
            default:
                yyCh = getChar();
                break;
            }
        }
    }
    return Tok_Eof;
}

/*
  The second part of this source file is the parser. It accomplishes
  a very easy task: It finds all strings inside a tr() or translate()
  call, and possibly finds out the context of the call. It supports
  three cases: (1) the context is specified, as in
  FunnyDialog::tr("Hello") or translate("FunnyDialog", "Hello");
  (2) the call appears within an inlined function; (3) the call
  appears within a function defined outside the class definition.
*/

static int yyTok;

static bool match( int t )
{
    bool matches = ( yyTok == t );
    if ( matches )
        yyTok = getToken();
    return matches;
}

static bool matchString( QByteArray *s )
{
    bool matches = ( yyTok == Tok_String );
    *s = "";
    while ( yyTok == Tok_String ) {
        *s += yyString;
        yyTok = getToken();
    }
    return matches;
}

static bool matchEncoding( bool *utf8 )
{
    if ( yyTok == Tok_Ident ) {
        if ( strcmp(yyIdent, "QApplication") == 0 || strcmp(yyIdent, "QCoreApplication") == 0) {
            yyTok = getToken();
            if ( yyTok == Tok_ColonColon )
                yyTok = getToken();
        }
        if (strcmp(yyIdent, "UnicodeUTF8") == 0) {
            *utf8 = true;
            yyTok = getToken();
        } else if ((strcmp(yyIdent, "DefaultCodec") == 0) || (strcmp(yyIdent, "CodecForTr") == 0) ) {
            *utf8 = false;
            yyTok = getToken();
        } else {
            return false;
        }
        return true;
    } else {
        return false;
    }
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

static bool matchStringOrNull(QByteArray *s)
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
        } else if (yyTok == Tok_Arrow) {
            yyTok = getToken();
        } else if (parenlevel == 0) {
            return false;
        }
    }
    return true;
}

static QByteArray getFullyQualifiedClassName(
    const QList<QByteArray> &classes, const QStringList &namespaces,
    const QByteArray &ident, bool hasPrefix = false)
{
    QByteArray context = ident;
    if (context.endsWith("::"))
        context.chop(2);
    int n = namespaces.count() - 1;
    if (!context.startsWith("::")) {
        for ( ; n >= 0; --n) {
            QByteArray ns;
            for (int i = 0; i <= n; ++i) {
                ns+=namespaces[i].toAscii() + "::";
            }
            if (classes.indexOf(ns + context) != -1) {
                context = ns + context;
                break;
            }
        }
        if (!hasPrefix && n == -1 && namespaces.count()) {
            context = namespaces.join(QLatin1String("::")).toLatin1() + "::" + context;
        }
    } else {
        context.remove(0, 2);
    }
    return context;
}


static void parse( MetaTranslator *tor, const char *initialContext, const char *defaultContext )
{
    QMap<QByteArray, QByteArray> qualifiedContexts;
    QStringList namespaces;
    QList<QByteArray> classes;
    QByteArray context;
    QByteArray text;
    QByteArray comment;
    QByteArray functionContext = initialContext;
    QByteArray prefix;
#ifdef DIAGNOSE_RETRANSLATABILITY
    QByteArray functionName;
#endif
    bool utf8 = false;
    bool missing_Q_OBJECT = false;

    yyTok = getToken();
    while ( yyTok != Tok_Eof ) {
        switch ( yyTok ) {
        case Tok_class:
            yyTokColonSeen = false;
            /*
              Partial support for inlined functions.
            */
            yyTok = getToken();
            if ( yyBraceDepth == namespaces.count() &&
                 yyParenDepth == 0 ) {
                do {
                    /*
                      This code should execute only once, but we play
                      safe with impure definitions such as
                      'class Q_EXPORT QMessageBox', in which case
                      'QMessageBox' is the class name, not 'Q_EXPORT'.
                    */
                    functionContext = yyIdent;
                    yyTok = getToken();
                } while ( yyTok == Tok_Ident );

                while ( yyTok == Tok_ColonColon ) {
                    yyTok = getToken();
                    functionContext += "::";
                    functionContext += yyIdent;
                    yyTok = getToken();
                }
                if (namespaces.count() > 0) {
                    functionContext.prepend( namespaces.join(QLatin1String("::")).toAscii().append("::") );
                }
                classes.append( functionContext );

                if ( yyTok == Tok_Colon ) {
                    missing_Q_OBJECT = true;
                    // Skip any token until '{' since lupdate might do things wrong if it finds
                    // a '::' token here.
                    do {
                        yyTok = getToken();
                    } while (yyTok != Tok_LeftBrace && yyTok != Tok_Eof);
                } else {
                    functionContext = defaultContext;
                }
            }
            break;
        case Tok_namespace:
            yyTokColonSeen = false;
            yyTok = getToken();
            if ( yyTok == Tok_Ident ) {
                QByteArray ns = yyIdent;
                yyTok = getToken();
                if ( yyTok == Tok_LeftBrace &&
                     yyBraceDepth == namespaces.count() + 1 )
                    namespaces.append( QString::fromLatin1(ns.constData()) );
            }
            break;
        case Tok_tr:
        case Tok_trUtf8:
            utf8 = ( yyTok == Tok_trUtf8 );
            yyTok = getToken();
            if ( match(Tok_LeftParen) && matchString(&text) ) {
                comment = "";
                bool plural = false;

                if ( match(Tok_RightParen) ) {
                    // no comment
                } else if (match(Tok_Comma) && matchStringOrNull(&comment)) {   //comment
                    if ( match(Tok_RightParen)) {
                        // ok,
                    } else if (match(Tok_Comma)) {
                        plural = true;
                    }
                }
                if ( prefix.isNull() ) {
                    context = functionContext;
                } else {
#ifdef DIAGNOSE_RETRANSLATABILITY
                    int last = prefix.lastIndexOf("::");
                    QByteArray className = prefix.mid(last == -1 ? 0 : last + 2);
                    if (!className.isEmpty() && className == functionName) {
                        qWarning( "%s::%d: It is not recommended to call tr() from within a constructor '%s::%s' ",
                                  qPrintable(yyFileName), yyLineNo,
                                  className.constData(), functionName.constData() );
                    }
#endif
                    context = getFullyQualifiedClassName(classes, namespaces, prefix, true);
                }
                prefix = (const char *) 0;
                if ( qualifiedContexts.contains(context) )
                    context = qualifiedContexts[context];
                tor->insert( TranslatorMessage(context, text, comment, "",
                    yyFileName, yyLineNo, QStringList(), utf8,
                    TranslatorMessage::Unfinished, plural) );

                if ( lacks_Q_OBJECT.contains(context) ) {
                    qWarning( "%s:%d: Class '%s' lacks Q_OBJECT macro",
                              qPrintable(yyFileName), yyLineNo,
                              (const char *) context );
                    lacks_Q_OBJECT.remove( context );
                } else {
                    needs_Q_OBJECT.insert( context, 0 );
                }
            }
            break;
        case Tok_translate:
            utf8 = false;
            yyTok = getToken();
            if ( match(Tok_LeftParen) &&
                 matchString(&context) &&
                 match(Tok_Comma) &&
                 matchString(&text) ) {
                 comment = "";
                 bool plural = false;
                 if (!match(Tok_RightParen)) {
                    // look for comment
                    if ( match(Tok_Comma) && matchStringOrNull(&comment)) {
                        if (!match(Tok_RightParen)) {
                            // look for encoding
                            if (match(Tok_Comma)) {
                                if (matchEncoding(&utf8)) {
                                    if (!match(Tok_RightParen)) {
                                        // look for the plural quantifier,
                                        // this can be a number, an identifier or
                                        // a function call,
                                        // so for simplicity we mark it as plural if
                                        // we know we have a comma instead of an
                                        // right parentheses.
                                        plural = match(Tok_Comma);
                                    }
                                } else {
                                    // This can be a QTranslator::translate("context",
                                    // "source", "comment", n) plural translation
                                    if (matchExpression() && match(Tok_RightParen)) {
                                        plural = true;
                                    } else {
                                        break;
                                    }
                                }
                            } else {
                                break;
                            }
                        }
                    } else {
                        break;
                    }
                 }
                tor->insert( TranslatorMessage(context, text, comment, "",
                    yyFileName, yyLineNo, QStringList(), utf8,
                    TranslatorMessage::Unfinished, plural) );
            }
            break;
        case Tok_Q_OBJECT:
            missing_Q_OBJECT = false;
            yyTok = getToken();
            break;
        case Tok_Ident:
            prefix += yyIdent;
            yyTok = getToken();
            if ( yyTok != Tok_ColonColon )
                prefix = (const char *) 0;
            break;
        case Tok_Comment:
            comment = yyComment;
            comment = comment.simplified();
            if ( comment.left(sizeof(MagicComment) - 1) == MagicComment ) {
                comment.remove( 0, sizeof(MagicComment) - 1 );
                int k = comment.indexOf( ' ' );
                if ( k == -1 ) {
                    context = comment;
                } else {
                    context = comment.left( k );
                    comment.remove( 0, k + 1 );
                    tor->insert( TranslatorMessage(context, "", comment, "",
                       yyFileName, yyLineNo, QStringList(), false) );
                }

                /*
                  Provide a backdoor for people using "using
                  namespace". See the manual for details.
                */
                k = 0;
                while ( (k = context.indexOf("::", k)) != -1 ) {
                    qualifiedContexts.insert( context.mid(k + 2), context );
                    k++;
                }
            }
            yyTok = getToken();
            break;
        case Tok_Arrow:
            yyTok = getToken();
            if ( yyTok == Tok_tr || yyTok == Tok_trUtf8 )
                qWarning( "%s:%d: Cannot invoke tr() like this",
                          qPrintable(yyFileName), yyLineNo );
            break;
        case Tok_ColonColon:
            // at top level?
            prefix += "::";
            if ( yyBraceDepth == (int) namespaces.count() && yyParenDepth == 0
               && !yyTokColonSeen)
                functionContext = getFullyQualifiedClassName(classes, namespaces, prefix);
            yyTok = getToken();
#ifdef DIAGNOSE_RETRANSLATABILITY
            if ( yyTok == Tok_Ident && yyBraceDepth == (int) namespaces.count() && yyParenDepth == 0 ) {
                functionName = yyIdent;
            }
#endif
            break;
        case Tok_RightBrace:
        case Tok_Semicolon:
            yyTokColonSeen = false;
            if ( yyBraceDepth >= 0 &&
                 yyBraceDepth + 1 == namespaces.count() )
                namespaces.removeLast();
            if ( yyBraceDepth == namespaces.count() ) {
                if ( missing_Q_OBJECT ) {
                    if ( needs_Q_OBJECT.contains(functionContext) ) {
                        qWarning( "%s:%d: Class '%s' lacks Q_OBJECT macro",
                                  qPrintable(yyFileName), yyLineNo,
                                  (const char *) functionContext );
                    } else {
                        lacks_Q_OBJECT.insert( functionContext, 0 );
                    }
                }
                functionContext = defaultContext;
                missing_Q_OBJECT = false;
            }
            yyTok = getToken();
            break;
        case Tok_Colon:
            yyTokColonSeen = true;
            yyTok = getToken();
            break;
        case Tok_LeftParen:
        case Tok_RightParen:
        case Tok_LeftBrace:
            yyTokColonSeen = false;
            yyTok = getToken();
            break;
        default:
            yyTok = getToken();
            break;
        }
    }

    if ( yyBraceDepth != 0 )
        qWarning( "%s:%d: Unbalanced braces in C++ code (or abuse of the C++"
                  " preprocessor)\n",
                  qPrintable(yyFileName), yyBraceLineNo );
    else if ( yyParenDepth != 0 )
        qWarning( "%s:%d: Unbalanced parentheses in C++ code (or abuse of the C++"
                 " preprocessor)\n",
                 qPrintable(yyFileName), yyParenLineNo );
}

void LupdateApplication::fetchtr_cpp( const QString &fileName, MetaTranslator *tor,
    const QString &defaultContext, bool mustExist, const QByteArray &codecForSource )
{
#if defined(_MSC_VER) && _MSC_VER >= 1400
	if (fopen_s(&yyInFile, qPrintable(fileName), "r")) {
		if ( mustExist ) {
			char buf[100];
			strerror_s(buf, sizeof(buf), errno);
			qWarning( "lupdate error: Cannot open C++ source file '%s': %s\n",
                     qPrintable(fileName), buf );
		}
#else
    yyInFile = fopen( qPrintable(fileName), "r" );
    if ( yyInFile == 0 ) {
        if ( mustExist )
            qWarning( "lupdate error: Cannot open C++ source file '%s': %s\n",
                     qPrintable(fileName), strerror(errno) );
#endif
        return;
    }

    startTokenizer( fileName, getCharFromFile, tor->codecForTr(),
        QTextCodec::codecForName(codecForSource) );
    parse( tor, defaultContext.toAscii(), defaultContext.toAscii() );
    fclose( yyInFile );
}

/*
  In addition to C++, we support Qt Designer UI files.
*/

/*
  Fetches tr() calls in C++ code in UI files (inside "<function>"
  tag). This mechanism is obsolete.
*/
void fetchtr_inlined_cpp(const QString &fileName, const QString &in,
    MetaTranslator *tor, const char *context )
{
    yyInStr = in;
    startTokenizer( fileName, getCharFromString, 0, 0 );
    parse( tor, context, 0 );
    yyInStr.clear();
}

class UiHandler : public QXmlDefaultHandler
{
public:
    UiHandler( MetaTranslator *translator, const QString &fileName )
        : tor( translator ), fname( fileName ), comment( QLatin1String("") ), m_lineNumber(-1)  { }

    virtual bool startElement( const QString& namespaceURI,
                               const QString& localName, const QString& qName,
                               const QXmlAttributes& atts );
    virtual bool endElement( const QString& namespaceURI,
                             const QString& localName, const QString& qName );
    virtual bool characters( const QString& ch );
    virtual bool fatalError( const QXmlParseException& exception );

    virtual void setDocumentLocator(QXmlLocator *locator)
    {
        m_locator = locator;
    }
    QXmlLocator *m_locator;
private:
    void flush();

    MetaTranslator *tor;
    QString fname;
    QString context;
    QString source;
    QString comment;

    QString accum;
    int m_lineNumber;
    bool trString;
};

bool UiHandler::startElement( const QString& /* namespaceURI */,
                              const QString& /* localName */,
                              const QString& qName,
                              const QXmlAttributes& atts )
{
    if ( qName == QLatin1String("item") ) {
        flush();
        if ( !atts.value(QLatin1String("text")).isEmpty() )
            source = atts.value(QLatin1String("text"));
    } else if ( qName == QLatin1String("string") ) {
        flush();
        if (atts.value(QLatin1String("notr")).isEmpty() ||
            atts.value(QLatin1String("notr")) != QLatin1String("true")) {
            trString = true;
            comment = atts.value(QLatin1String("comment"));
        } else {
            trString = false;
        }
    }
    if (trString)
        m_lineNumber = m_locator->lineNumber();
    accum.truncate( 0 );
    return true;
}

bool UiHandler::endElement( const QString& /* namespaceURI */,
                            const QString& /* localName */,
                            const QString& qName )
{
    accum.replace( QRegExp(QLatin1String("\r\n")), QLatin1String("\n") );

    if ( qName == QLatin1String("class") ) {
        if ( context.isEmpty() )
            context = accum;
    } else if ( qName == QLatin1String("string") && trString ) {
        source = accum;
    } else if ( qName == QLatin1String("comment") ) {
        comment = accum;
        flush();
    } else if ( qName == QLatin1String("function") ) {
        fetchtr_inlined_cpp( fname, accum, tor, context.toLatin1() );
    } else {
        flush();
    }
    return true;
}

bool UiHandler::characters( const QString& ch )
{
    accum += ch;
    return true;
}

bool UiHandler::fatalError( const QXmlParseException& exception )
{
    QString msg;
    msg.sprintf( "Parse error at line %d, column %d (%s).",
                 exception.lineNumber(), exception.columnNumber(),
                 exception.message().toLatin1().data() );
    qWarning( "XML error: %s\n", msg.toLatin1().data() );
    return false;
}

void UiHandler::flush()
{
    if ( !context.isEmpty() && !source.isEmpty() )
        tor->insert( TranslatorMessage(context.toUtf8(), source.toUtf8(),
                                 comment.toUtf8(), "", fname, m_lineNumber,
                                           QStringList(), true) );
    source.truncate( 0 );
    comment.truncate( 0 );
}

void LupdateApplication::fetchtr_ui( const QString &fileName, MetaTranslator *tor,
                 const QString &defaultContext, bool mustExist )
{
    Q_UNUSED(defaultContext)

    QFile f( fileName );
    if ( !f.open(QIODevice::ReadOnly) ) {
		if ( mustExist ) {
#if defined(_MSC_VER) && _MSC_VER >= 1400
			char buf[100];
			strerror_s(buf, sizeof(buf), errno);
			qWarning( "lupdate error: cannot open UI file '%s': %s\n",
                     qPrintable(fileName), buf );
#else
            qWarning( "lupdate error: cannot open UI file '%s': %s\n",
                     qPrintable(fileName), strerror(errno) );
#endif
		}
        return;
    }

    QXmlInputSource in( &f );
    QXmlSimpleReader reader;
    reader.setFeature( QLatin1String("http://xml.org/sax/features/namespaces"), false );
    reader.setFeature( QLatin1String("http://xml.org/sax/features/namespace-prefixes"), true );
    reader.setFeature( QLatin1String("http://trolltech.com/xml/features/report-whitespace"
                                     "-only-CharData"), false );
    QXmlDefaultHandler *hand = new UiHandler( tor, fileName );
    reader.setContentHandler( hand );
    reader.setErrorHandler( hand );

    if ( !reader.parse(in) )
        qWarning( "%s: Parse error in UI file\n", qPrintable(fileName) );
    reader.setContentHandler( 0 );
    reader.setErrorHandler( 0 );
    delete hand;
    f.close();
}

QT_END_NAMESPACE
