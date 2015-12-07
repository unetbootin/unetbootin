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
#include "doc.h"
#include "codemarker.h"
#include "editdistance.h"
#include "openedlist.h"
#include "quoter.h"
#include "text.h"
#include "tokenizer.h"

#include <qdatetime.h>
#include <qdebug.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qhash.h>
#include <qtextstream.h>
#include <qregexp.h>

#include <ctype.h>
#include <limits.h>

QT_BEGIN_NAMESPACE

typedef QMap<QString, QString> QMap_QString_QString;

Q_GLOBAL_STATIC(QSet<QString>, null_Set_QString)
Q_GLOBAL_STATIC(QStringList, null_QStringList)
Q_GLOBAL_STATIC(QList<Text>, null_QList_Text)
Q_GLOBAL_STATIC(QMap_QString_QString, null_QMap_QString_QString)

struct Macro
{
    QString defaultDef;
    Location defaultDefLocation;
    QMap<QString, QString> otherDefs;
    int numParams;
};

enum {
    CMD_A, CMD_ABSTRACT, CMD_BADCODE, CMD_BASENAME, CMD_BOLD, CMD_BRIEF, CMD_C, CMD_CAPTION,
    CMD_CHAPTER, CMD_CODE, CMD_CODELINE, CMD_DOTS, CMD_ELSE, CMD_ENDABSTRACT, CMD_ENDCHAPTER, CMD_ENDCODE,
    CMD_ENDFOOTNOTE, CMD_ENDIF, CMD_ENDLEGALESE, CMD_ENDLINK, CMD_ENDLIST, CMD_ENDOMIT,
    CMD_ENDPART, CMD_ENDQUOTATION, CMD_ENDRAW, CMD_ENDSECTION1, CMD_ENDSECTION2, CMD_ENDSECTION3,
    CMD_ENDSECTION4, CMD_ENDSIDEBAR, CMD_ENDTABLE, CMD_EXPIRE, CMD_FOOTNOTE, CMD_GENERATELIST,
    CMD_GRANULARITY, CMD_HEADER, CMD_I, CMD_IF, CMD_IMAGE, CMD_INCLUDE, CMD_INLINEIMAGE, CMD_INDEX,
    CMD_KEYWORD, CMD_L, CMD_LEGALESE, CMD_LINK, CMD_LIST, CMD_META, CMD_NEWCODE, CMD_O,
    CMD_OLDCODE, CMD_OMIT, CMD_OMITVALUE, CMD_PART, CMD_PRINTLINE, CMD_PRINTTO, CMD_PRINTUNTIL,
    CMD_QUOTATION, CMD_QUOTEFILE, CMD_QUOTEFROMFILE, CMD_QUOTEFUNCTION, CMD_RAW, CMD_ROW, CMD_SA,
    CMD_SECTION1, CMD_SECTION2, CMD_SECTION3, CMD_SECTION4, CMD_SIDEBAR, CMD_SKIPLINE, CMD_SKIPTO,
    CMD_SKIPUNTIL, CMD_SNIPPET, CMD_SUB, CMD_SUP, CMD_TABLE, CMD_TABLEOFCONTENTS, CMD_TARGET, CMD_TT,
    CMD_UNDERLINE, CMD_UNICODE, CMD_VALUE, CMD_WARNING, UNKNOWN_COMMAND
};

static struct {
    const char *english;
    int no;
    QString *alias;
} cmds[] = {
    { "a", CMD_A, 0 },
    { "abstract", CMD_ABSTRACT, 0 },
    { "badcode", CMD_BADCODE, 0 },
    { "basename", CMD_BASENAME, 0 }, // ### don't document for now
    { "bold", CMD_BOLD, 0 },
    { "brief", CMD_BRIEF, 0 },
    { "c", CMD_C, 0 },
    { "caption", CMD_CAPTION, 0 },
    { "chapter", CMD_CHAPTER, 0 },
    { "code", CMD_CODE, 0 },
    { "codeline", CMD_CODELINE, 0},
    { "dots", CMD_DOTS, 0 },
    { "else", CMD_ELSE, 0 },
    { "endabstract", CMD_ENDABSTRACT, 0 },
    { "endchapter", CMD_ENDCHAPTER, 0 },
    { "endcode", CMD_ENDCODE, 0 },
    { "endfootnote", CMD_ENDFOOTNOTE, 0 },
    { "endif", CMD_ENDIF, 0 },
    { "endlegalese", CMD_ENDLEGALESE, 0 },
    { "endlink", CMD_ENDLINK, 0 },
    { "endlist", CMD_ENDLIST, 0 },
    { "endomit", CMD_ENDOMIT, 0 },
    { "endpart", CMD_ENDPART, 0 },
    { "endquotation", CMD_ENDQUOTATION, 0 },
    { "endraw", CMD_ENDRAW, 0 },
    { "endsection1", CMD_ENDSECTION1, 0 },  // ### don't document for now
    { "endsection2", CMD_ENDSECTION2, 0 },  // ### don't document for now
    { "endsection3", CMD_ENDSECTION3, 0 },  // ### don't document for now
    { "endsection4", CMD_ENDSECTION4, 0 },  // ### don't document for now
    { "endsidebar", CMD_ENDSIDEBAR, 0 },
    { "endtable", CMD_ENDTABLE, 0 },
    { "expire", CMD_EXPIRE, 0 },
    { "footnote", CMD_FOOTNOTE, 0 },
    { "generatelist", CMD_GENERATELIST, 0 },
    { "granularity", CMD_GRANULARITY, 0 }, // ### don't document for now
    { "header", CMD_HEADER, 0 },
    { "i", CMD_I, 0 },
    { "if", CMD_IF, 0 },
    { "image", CMD_IMAGE, 0 },
    { "include", CMD_INCLUDE, 0 },
    { "inlineimage", CMD_INLINEIMAGE, 0 },
    { "index", CMD_INDEX, 0 }, // ### don't document for now
    { "keyword", CMD_KEYWORD, 0 },
    { "l", CMD_L, 0 },
    { "legalese", CMD_LEGALESE, 0 },
    { "link", CMD_LINK, 0 },
    { "list", CMD_LIST, 0 },
    { "meta", CMD_META, 0 },
    { "newcode", CMD_NEWCODE, 0 },
    { "o", CMD_O, 0 },
    { "oldcode", CMD_OLDCODE, 0 },
    { "omit", CMD_OMIT, 0 },
    { "omitvalue", CMD_OMITVALUE, 0 },
    { "part", CMD_PART, 0 },
    { "printline", CMD_PRINTLINE, 0 },
    { "printto", CMD_PRINTTO, 0 },
    { "printuntil", CMD_PRINTUNTIL, 0 },
    { "quotation", CMD_QUOTATION, 0 },
    { "quotefile", CMD_QUOTEFILE, 0 },
    { "quotefromfile", CMD_QUOTEFROMFILE, 0 },
    { "quotefunction", CMD_QUOTEFUNCTION, 0 }, // ### don't document for now
    { "raw", CMD_RAW, 0 },
    { "row", CMD_ROW, 0 },
    { "sa", CMD_SA, 0 },
    { "section1", CMD_SECTION1, 0 },
    { "section2", CMD_SECTION2, 0 },
    { "section3", CMD_SECTION3, 0 },
    { "section4", CMD_SECTION4, 0 },
    { "sidebar", CMD_SIDEBAR, 0 }, // ### don't document for now
    { "skipline", CMD_SKIPLINE, 0 },
    { "skipto", CMD_SKIPTO, 0 },
    { "skipuntil", CMD_SKIPUNTIL, 0 },
    { "snippet", CMD_SNIPPET, 0 },
    { "sub", CMD_SUB, 0 },
    { "sup", CMD_SUP, 0 },
    { "table", CMD_TABLE, 0 },
    { "tableofcontents", CMD_TABLEOFCONTENTS, 0 },
    { "target", CMD_TARGET, 0 },
    { "tt", CMD_TT, 0 },
    { "underline", CMD_UNDERLINE, 0 },
    { "unicode", CMD_UNICODE, 0 },
    { "value", CMD_VALUE, 0 },
    { "warning", CMD_WARNING, 0 },
    { 0, 0, 0 }
};

typedef QMap<QString, QString> QMap_QString_QString;
typedef QHash<QString, int> QHash_QString_int;
typedef QHash<QString, Macro> QHash_QString_Macro;

Q_GLOBAL_STATIC(QMap_QString_QString, aliasMap)
Q_GLOBAL_STATIC(QHash_QString_int, commandHash)
Q_GLOBAL_STATIC(QHash_QString_Macro, macroHash)

class DocPrivateExtra
{
public:
    QString baseName;
    Doc::SectioningUnit granularity;
    Doc::SectioningUnit sectioningUnit; // ###
    QList<Atom *> tableOfContents;
    QList<int>    tableOfContentsLevels;
    QList<Atom *> keywords;
    QList<Atom *> targets;
    QMap<QString, QString> metaMap;

    DocPrivateExtra()
	: granularity( Doc::Part ) { }
};

struct Shared // ### get rid of
{
    Shared()
	: count(1) { }
    void ref() { ++count; }
    bool deref() { return (--count == 0); }

    int count;
};

static QString cleanLink(const QString &link)
{
    int colonPos = link.indexOf(':');
    if (colonPos == -1 || (!link.startsWith("file:") && !link.startsWith("mailto:")))
        return link;
    return link.mid(colonPos + 1).simplified();
}

class DocPrivate : public Shared
{
public:
    DocPrivate(const Location &location = Location::null, const QString &source = "");
    ~DocPrivate();

    void addAlso( const Text& also );
    void constructExtra();
    bool isEnumDocSimplifiable() const;

    // ### move some of this in DocPrivateExtra
    Location loc;
    QString src;
    Text text;
    QSet<QString> params;
    QList<Text> alsoList;
    QStringList enumItemList;
    QStringList omitEnumItemList;
    QSet<QString> metaCommandSet;
    QMap<QString, QStringList> metaCommandMap;
    bool hasLegalese : 1;
    bool hasSectioningUnits : 1;
    DocPrivateExtra *extra;
};

DocPrivate::DocPrivate(const Location &location, const QString &source)
    : loc(location), src(source), hasLegalese(false), hasSectioningUnits(false), extra(0)
{
}

DocPrivate::~DocPrivate()
{
    delete extra;
}

void DocPrivate::addAlso( const Text& also )
{
    alsoList.append( also );
}

void DocPrivate::constructExtra()
{
    if ( extra == 0 )
	extra = new DocPrivateExtra;
}

bool DocPrivate::isEnumDocSimplifiable() const
{
    bool justMetColon = false;
    int numValueTables = 0;

    const Atom *atom = text.firstAtom();
    while (atom) {
        if (atom->type() == Atom::AutoLink || atom->type() == Atom::String) {
            justMetColon = atom->string().endsWith(":");
        } else if (atom->type() == Atom::ListLeft && atom->string() == ATOM_LIST_VALUE) {
            if (justMetColon || numValueTables > 0)
                return false;
            ++numValueTables;
        }
        atom = atom->next();
    }
    return true;
}

class DocParser
{
public:
    void parse(const QString &source, DocPrivate *docPrivate, const QSet<QString> &metaCommandSet);

    static int endCommandFor( int command );
    static QString commandName( int command );
    static QString endCommandName( int command );
    static QString untabifyEtc( const QString& str );
    static int indentLevel( const QString& str );
    static QString unindent( int level, const QString& str );
    static QString slashed( const QString& str );

    static int tabSize;
    static QStringList exampleFiles;
    static QStringList exampleDirs;
    static QStringList sourceFiles;
    static QStringList sourceDirs;
    static bool quoting;

private:
    Location& location();
    QString detailsUnknownCommand( const QSet<QString>& metaCommandSet,
				   const QString& str );
    void checkExpiry( const QString& date );
    void insertBaseName(const QString &baseName);
    void insertTarget( const QString& target, bool keyword);
    void include( const QString& fileName );
    void startFormat( const QString& format, int command );
    bool openCommand( int command );
    bool closeCommand( int endCommand );
    void startSection( Doc::SectioningUnit unit, int command );
    void endSection( int unit, int endCommand );
    void parseAlso();
    void append( Atom::Type type, const QString& string = "" );
    void appendChar(QChar ch);
    void appendWord(const QString &word);
    void appendToCode(const QString &code);
    void startNewPara();
    void enterPara( Atom::Type leftType = Atom::ParaLeft,
		    Atom::Type rightType = Atom::ParaRight,
		    const QString& string = "" );
    void leavePara();
    void leaveValue();
    void leaveValueList();
    void leaveTableRow();
    CodeMarker *quoteFromFile();
    void expandMacro( const QString& name, const QString& def, int numParams );
    Doc::SectioningUnit getSectioningUnit();
    QString getArgument( bool verbatim = false );
    QString getOptionalArgument();
    QString getRestOfLine();
    QString getMetaCommandArgument(const QString &commandStr);
    QString getUntilEnd(int command);
    QString getCode(int command, CodeMarker *marker);

    bool isBlankLine();
    bool isLeftBraceAhead();
    void skipSpacesOnLine();
    void skipSpacesOrOneEndl();
    void skipAllSpaces();
    void skipToNextPreprocessorCommand();

    QStack<int> openedInputs;

    QString in;
    int pos;
    int len;
    Location cachedLoc;
    int cachedPos;

    DocPrivate *priv;
    enum ParaState { OutsidePara, InsideSingleLinePara, InsideMultiLinePara };
    ParaState paraState;
    bool inTableHeader;
    bool inTableRow;
    bool inTableItem;
    bool indexStartedPara; // ### rename
    Atom::Type pendingParaLeftType;
    Atom::Type pendingParaRightType;
    QString pendingParaString;

    int braceDepth;
    int minIndent;
    Doc::SectioningUnit currentSectioningUnit;
    QMap<QString, Location> targetMap;
    QMap<int, QString> pendingFormats;
    QStack<int> openedCommands;
    QStack<OpenedList> openedLists;
    Quoter quoter;
};

int DocParser::tabSize;
QStringList DocParser::exampleFiles;
QStringList DocParser::exampleDirs;
QStringList DocParser::sourceFiles;
QStringList DocParser::sourceDirs;
bool DocParser::quoting;

void DocParser::parse( const QString& source, DocPrivate *docPrivate,
		       const QSet<QString>& metaCommandSet )
{
    in = source;
    pos = 0;
    len = in.length();
    cachedLoc = docPrivate->loc;
    cachedPos = 0;
    priv = docPrivate;
    priv->text << Atom::Nop;

    paraState = OutsidePara;
    inTableHeader = false;
    inTableRow = false;
    inTableItem = false;
    indexStartedPara = false;
    pendingParaLeftType = Atom::Nop;
    pendingParaRightType = Atom::Nop;

    braceDepth = 0;
    minIndent = INT_MAX;
    currentSectioningUnit = Doc::Book;
    openedCommands.push( CMD_OMIT );
    quoter.reset();

    CodeMarker *marker = 0;
    Atom *currentLinkAtom = 0;
    QString x;
    QStack<bool> preprocessorSkipping;
    int numPreprocessorSkipping = 0;

    while ( pos < len ) {
        QChar ch = in.at(pos);

        switch (ch.unicode()) {
        case '\\':
            {
                QString commandStr;
                pos++;
                while ( pos < len ) {
                    ch = in.at(pos);
                    if ( ch.isLetterOrNumber() ) {
                        commandStr += ch;
                        pos++;
                    } else {
                        break;
                    }
                }
                if ( commandStr.isEmpty() ) {
                    if ( pos < len ) {
                        enterPara();
                        if ( in.at(pos).isSpace() ) {
                            skipAllSpaces();
                            appendChar( QLatin1Char(' ') );
                        } else {
                            appendChar( in.at(pos++) );
                        }
                    }
                } else {
                    int command = commandHash()->value(commandStr, UNKNOWN_COMMAND);

                    switch ( command ) {
                    case CMD_A:
                        enterPara();
                        x = getArgument();
                        append( Atom::FormattingLeft, ATOM_FORMATTING_PARAMETER );
                        append( Atom::String, x );
                        append( Atom::FormattingRight, ATOM_FORMATTING_PARAMETER );
                        priv->params.insert( x );
                        break;
                    case CMD_ABSTRACT:
                        if ( openCommand(command) ) {
                            leavePara();
                            append( Atom::AbstractLeft );
                        }
                        break;
                    case CMD_BADCODE:
                        leavePara();
                        append(Atom::CodeBad, getCode(CMD_BADCODE, marker));
                        break;
                    case CMD_BASENAME:
                        leavePara();
                        insertBaseName(getArgument());
                        break;
                    case CMD_BOLD:
                        startFormat( ATOM_FORMATTING_BOLD, command );
                        break;
                    case CMD_BRIEF:
                        leavePara();
                        enterPara( Atom::BriefLeft, Atom::BriefRight );
                        break;
                    case CMD_C:
                        enterPara();
                        x = untabifyEtc( getArgument(true) );
                        marker = CodeMarker::markerForCode( x );
                        append( Atom::C, marker->markedUpCode(x, 0, "") );
                        break;
                    case CMD_CAPTION:
                        leavePara();
                        /* ... */
                        break;
                    case CMD_CHAPTER:
                        startSection( Doc::Chapter, command );
                        break;
                    case CMD_CODE:
                        leavePara();
                        append(Atom::Code, getCode(CMD_CODE, marker));
                        break;
                    case CMD_CODELINE:
                        {
                            if (!quoting) {
                                if (priv->text.lastAtom()->type() == Atom::Code
                                        && priv->text.lastAtom()->string().endsWith("\n\n"))
                                    priv->text.lastAtom()->chopString();
                                appendToCode("\n");
                            } else {
                                append(Atom::CodeQuoteCommand, commandStr);
                                append(Atom::CodeQuoteArgument, "\n");
                            }
                        }
                        break;
                    case CMD_DOTS:
                        {
                            if (!quoting) {
                                if (priv->text.lastAtom()->type() == Atom::Code
                                        && priv->text.lastAtom()->string().endsWith("\n\n"))
                                    priv->text.lastAtom()->chopString();

                                QString arg = getOptionalArgument();
                                int indent = 4;
                                if (!arg.isEmpty())
                                    indent = arg.toInt();
                                for (int i = 0; i < indent; ++i)
                                    appendToCode(" ");
                                appendToCode("...\n");
                            } else {
                                append(Atom::CodeQuoteCommand, commandStr);
                                append(Atom::CodeQuoteArgument, "...\n");
                            }
                        }
                        break;
                    case CMD_ELSE:
                        if (preprocessorSkipping.size() > 0) {
                            if (preprocessorSkipping.top()) {
                                --numPreprocessorSkipping;
                            } else {
                                ++numPreprocessorSkipping;
                            }
                            preprocessorSkipping.top() = !preprocessorSkipping.top();
                            (void)getRestOfLine(); // ### should ensure that it's empty
                            if (numPreprocessorSkipping)
                                skipToNextPreprocessorCommand();
                        } else {
                            location().warning(tr("Unexpected '\\%1'").arg(commandName(CMD_ELSE)));
                        }
                        break;
                    case CMD_ENDABSTRACT:
                        if ( closeCommand(command) ) {
                            leavePara();
                            append( Atom::AbstractRight );
                        }
                        break;
                    case CMD_ENDCHAPTER:
                        endSection( 0, command );
                        break;
                    case CMD_ENDCODE:
                        closeCommand( command );
                        break;
                    case CMD_ENDFOOTNOTE:
                        if ( closeCommand(command) ) {
                            leavePara();
                            append( Atom::FootnoteRight );
                            paraState = InsideMultiLinePara; // ###
                        }
                        break;
                    case CMD_ENDIF:
                        if (preprocessorSkipping.count() > 0) {
                            if (preprocessorSkipping.pop())
                                --numPreprocessorSkipping;
                            (void)getRestOfLine(); // ### should ensure that it's empty
                            if (numPreprocessorSkipping)
                                skipToNextPreprocessorCommand();
                        } else {
                            location().warning(tr("Unexpected '\\%1'").arg(commandName(CMD_ENDIF)));
                        }
                        break;
                    case CMD_ENDLEGALESE:
                        if ( closeCommand(command) ) {
                            leavePara();
                            append( Atom::LegaleseRight );
                        }
                        break;
                    case CMD_ENDLINK:
                        if (closeCommand(command)) {
                            if (priv->text.lastAtom()->type() == Atom::String
                                    && priv->text.lastAtom()->string().endsWith(" "))
                                priv->text.lastAtom()->chopString();
                            append(Atom::FormattingRight, ATOM_FORMATTING_LINK);
                        }
                        break;
                    case CMD_ENDLIST:
                        if ( closeCommand(command) ) {
                            leavePara();
                            if ( openedLists.top().isStarted() ) {
                                append( Atom::ListItemRight,
                                        openedLists.top().styleString() );
                                append( Atom::ListRight,
                                        openedLists.top().styleString() );
                            }
                            openedLists.pop();
                        }
                        break;
                    case CMD_ENDOMIT:
                        closeCommand( command );
                        break;
                    case CMD_ENDPART:
                        endSection( -1, command );
                        break;
                    case CMD_ENDQUOTATION:
                        if ( closeCommand(command) ) {
                            leavePara();
                            append( Atom::QuotationRight );
                        }
                        break;
                    case CMD_ENDRAW:
                        location().warning(tr("Unexpected '\\%1'").arg(commandName(CMD_ENDRAW)));
                        break;
                    case CMD_ENDSECTION1:
                        endSection( 1, command );
                        break;
                    case CMD_ENDSECTION2:
                        endSection( 2, command );
                        break;
                    case CMD_ENDSECTION3:
                        endSection( 3, command );
                        break;
                    case CMD_ENDSECTION4:
                        endSection( 4, command );
                        break;
                    case CMD_ENDSIDEBAR:
                        if ( closeCommand(command) ) {
                            leavePara();
                            append( Atom::SidebarRight );
                        }
                        break;
                    case CMD_ENDTABLE:
                        if (closeCommand(command)) {
                            leaveTableRow();
                            append(Atom::TableRight);
                        }
                        break;
                    case CMD_EXPIRE:
                        checkExpiry( getArgument() );
                        break;
                    case CMD_FOOTNOTE:
                        if ( openCommand(command) ) {
                            enterPara();
                            append( Atom::FootnoteLeft );
                            paraState = OutsidePara; // ###
                        }
                        break;
                    case CMD_GENERATELIST:
                        append(Atom::GeneratedList, getArgument());
                        break;
                    case CMD_GRANULARITY:
                        priv->constructExtra();
                        priv->extra->granularity = getSectioningUnit();
                        break;
                    case CMD_HEADER:
                        if (openedCommands.top() == CMD_TABLE) {
                            leaveTableRow();
                            append(Atom::TableHeaderLeft);
                            inTableHeader = true;
                        } else {
                            if (openedCommands.contains(CMD_TABLE)) {
                                location().warning(tr("Cannot use '\\%1' within '\\%2'")
                                                   .arg(commandName(CMD_HEADER))
                                                   .arg(commandName(openedCommands.top())));
                            } else {
                                location().warning(tr("Cannot use '\\%1' outside of '\\%2'")
                                                   .arg(commandName(CMD_HEADER))
                                                   .arg(commandName(CMD_TABLE)));
                            }
                        }
                        break;
                    case CMD_I:
                        startFormat( ATOM_FORMATTING_ITALIC, command );
                        break;
                    case CMD_IF:
                        preprocessorSkipping.push(!Tokenizer::isTrue(getRestOfLine()));
                        if (preprocessorSkipping.top())
                            ++numPreprocessorSkipping;
                        if (numPreprocessorSkipping)
                            skipToNextPreprocessorCommand();
                        break;
                    case CMD_IMAGE:
                        leaveValueList();
                        append( Atom::Image, getArgument() );
                        append( Atom::ImageText, getRestOfLine() );
                        break;
                    case CMD_INCLUDE:
                        include( getArgument() );
                        break;
                    case CMD_INLINEIMAGE:
                        enterPara();
                        append(Atom::InlineImage, getArgument());
                        append(Atom::ImageText, getRestOfLine());
                        append(Atom::String, " ");
                        break;
                    case CMD_INDEX:
                        if ( paraState == OutsidePara ) {
                            enterPara();
                            indexStartedPara = true;
                        } else {
                            const Atom *last = priv->text.lastAtom();
                            if ( indexStartedPara &&
                                 (last->type() != Atom::FormattingRight ||
                                  last->string() != ATOM_FORMATTING_INDEX) )
                                indexStartedPara = false;
                        }
                        startFormat( ATOM_FORMATTING_INDEX, command );
                        break;
                    case CMD_KEYWORD:
                        insertTarget(getRestOfLine(), true);
                        break;
                    case CMD_L:
                        enterPara();
                        if ( isLeftBraceAhead() ) {
                            x = getArgument();
                            append( Atom::Link, x );
                            if ( isLeftBraceAhead() ) {
                                currentLinkAtom = priv->text.lastAtom();
                                startFormat( ATOM_FORMATTING_LINK, command );
                            } else {
                                append(Atom::FormattingLeft, ATOM_FORMATTING_LINK);
                                append(Atom::String, cleanLink(x));
                                append(Atom::FormattingRight, ATOM_FORMATTING_LINK);
                            }
                        } else {
                            x = getArgument();
                            append(Atom::Link, x);
                            append(Atom::FormattingLeft, ATOM_FORMATTING_LINK);
                            append(Atom::String, cleanLink(x));
                            append(Atom::FormattingRight, ATOM_FORMATTING_LINK);
                        }
                        break;
                    case CMD_LEGALESE:
                        leavePara();
                        if (openCommand(command))
                            append(Atom::LegaleseLeft);
                        docPrivate->hasLegalese = true;
                        break;
                    case CMD_LINK:
                        if (openCommand(command)) {
                            enterPara();
                            x = getArgument();
                            append(Atom::Link, x);
                            append(Atom::FormattingLeft, ATOM_FORMATTING_LINK);
                            skipSpacesOrOneEndl();
                        }
                        break;
                    case CMD_LIST:
                        if ( openCommand(command) ) {
                            leavePara();
                            openedLists.push( OpenedList(location(),
                                                         getOptionalArgument()) );
                        }
                        break;
                    case CMD_META:
                        priv->constructExtra();
                        x = getArgument();
                        priv->extra->metaMap.insert(x, getRestOfLine());
                        break;
                    case CMD_NEWCODE:
                        location().warning(tr("Unexpected '\\%1'").arg(commandName(CMD_NEWCODE)));
                        break;
                    case CMD_O:
                        leavePara();
                        if (openedCommands.top() == CMD_LIST) {
                            if ( openedLists.top().isStarted() ) {
                                append( Atom::ListItemRight,
                                        openedLists.top().styleString() );
                            } else {
                                append( Atom::ListLeft,
                                        openedLists.top().styleString() );
                            }
                            openedLists.top().next();
                            append( Atom::ListItemNumber,
                                    openedLists.top().numberString() );
                            append( Atom::ListItemLeft,
                                    openedLists.top().styleString() );
                            enterPara();
                        } else if (openedCommands.top() == CMD_TABLE) {
                            x = "1,1";
                            if (isLeftBraceAhead())
                                x = getArgument();

                            if (!inTableHeader && !inTableRow) {
                                location().warning(tr("Missing '\\%1' or '\\%1' before '\\%3'")
                                                   .arg(commandName(CMD_HEADER))
                                                   .arg(commandName(CMD_ROW))
                                                   .arg(commandName(CMD_O)));
                                append(Atom::TableRowLeft);
                                inTableRow = true;
                            } else if (inTableItem) {
                                append(Atom::TableItemRight);
                                inTableItem = false;
                            }

                            append(Atom::TableItemLeft, x);
                            inTableItem = true;
                        } else {
                            location().warning(tr("Command '\\%1' outside of '\\%2' and '\\%3'")
                                               .arg(commandName(command))
                                               .arg(commandName(CMD_LIST))
                                               .arg(commandName(CMD_TABLE)));
                        }
                        break;
                    case CMD_OLDCODE:
                        leavePara();
                        append(Atom::CodeOld, getCode(CMD_OLDCODE, marker));
                        append(Atom::CodeNew, getCode(CMD_NEWCODE, marker));
                        break;
                    case CMD_OMIT:
                        getUntilEnd( command );
                        break;
                    case CMD_OMITVALUE:
                        x = getArgument();
                        if (!priv->enumItemList.contains(x))
                            priv->enumItemList.append(x);
                        if (!priv->omitEnumItemList.contains(x))
                            priv->omitEnumItemList.append(x);
                        break;
                    case CMD_PART:
                        startSection( Doc::Part, command );
                        break;
                    case CMD_PRINTLINE:
                        leavePara();
                        if (!quoting)
                            appendToCode( quoter.quoteLine(location(), commandStr,
                                                           getRestOfLine()) );
                        else {
                            append(Atom::CodeQuoteCommand, commandStr);
                            append(Atom::CodeQuoteArgument, getRestOfLine());
                        }
                        break;
                    case CMD_PRINTTO:
                        leavePara();
                        if (!quoting)
                            appendToCode( quoter.quoteTo(location(), commandStr,
                                          getRestOfLine()) );
                        else {
                            append(Atom::CodeQuoteCommand, commandStr);
                            append(Atom::CodeQuoteArgument, getRestOfLine());
                        }
                        break;
                    case CMD_PRINTUNTIL:
                        leavePara();
                        if (!quoting)
                            appendToCode( quoter.quoteUntil(location(), commandStr,
                                          getRestOfLine()) );
                        else {
                            append(Atom::CodeQuoteCommand, commandStr);
                            append(Atom::CodeQuoteArgument, getRestOfLine());
                        }
                        break;
                    case CMD_QUOTATION:
                        if ( openCommand(command) ) {
                            leavePara();
                            append( Atom::QuotationLeft );
                        }
                        break;
                    case CMD_QUOTEFILE:
                        leavePara();
                        quoteFromFile();
                        if (!quoting) {
                            append( Atom::Code,
                                    quoter.quoteTo(location(), commandStr, "") );
                            quoter.reset();
                        } else {
                            append(Atom::CodeQuoteCommand, commandStr);
                            append(Atom::CodeQuoteArgument, getArgument());
                        }
                        break;
                    case CMD_QUOTEFROMFILE:
                        leavePara();
                        if (!quoting)
                            quoteFromFile();
                        else {
                            append(Atom::CodeQuoteCommand, commandStr);
                            append(Atom::CodeQuoteArgument, getArgument());
                        }
                        break;
                    case CMD_QUOTEFUNCTION:
                        leavePara();
                        marker = quoteFromFile();
                        x = getRestOfLine();
                        if (!quoting) {
                            quoter.quoteTo( location(), commandStr,
                                            slashed(marker->functionBeginRegExp(x)) );
                            append( Atom::Code,
                                    quoter.quoteUntil(location(), commandStr,
                                            slashed(marker->functionEndRegExp(x))) );
                            quoter.reset();
                        } else {
                            append(Atom::CodeQuoteCommand, commandStr);
                            append(Atom::CodeQuoteArgument, slashed(marker->functionEndRegExp(x)));
                        }
                        break;
                    case CMD_RAW:
                        leavePara();
                        x = getRestOfLine();
                        if (x.isEmpty())
                            location().warning(tr("Missing format name after '\\%1")
                                               .arg(commandName(CMD_RAW)));
                        append(Atom::FormatIf, x);
                        append(Atom::RawString, untabifyEtc(getUntilEnd(command)));
                        append(Atom::FormatElse);
                        append(Atom::FormatEndif);
                        break;
                    case CMD_ROW:
                        if (openedCommands.top() == CMD_TABLE) {
                            leaveTableRow();
                            append(Atom::TableRowLeft);
                            inTableRow = true;
                        } else {
                            if (openedCommands.contains(CMD_TABLE)) {
                                location().warning(tr("Cannot use '\\%1' within '\\%2'")
                                                   .arg(commandName(CMD_ROW))
                                                   .arg(commandName(openedCommands.top())));
                            } else {
                                location().warning(tr("Cannot use '\\%1' outside of '\\%2'")
                                                   .arg(commandName(CMD_ROW))
                                                   .arg(commandName(CMD_TABLE)));
                            }
                        }
                        break;
                    case CMD_SA:
                        parseAlso();
                        break;
                    case CMD_SECTION1:
                        startSection( Doc::Section1, command );
                        break;
                    case CMD_SECTION2:
                        startSection( Doc::Section2, command );
                        break;
                    case CMD_SECTION3:
                        startSection( Doc::Section3, command );
                        break;
                    case CMD_SECTION4:
                        startSection( Doc::Section4, command );
                        break;
                    case CMD_SIDEBAR:
                        if ( openCommand(command) ) {
                            leavePara();
                            append( Atom::SidebarLeft );
                        }
                        break;
                    case CMD_SKIPLINE:
                        leavePara();
                        if (!quoting)
                            quoter.quoteLine( location(), commandStr, getRestOfLine() );
                        else {
                            append(Atom::CodeQuoteCommand, commandStr);
                            append(Atom::CodeQuoteArgument, getRestOfLine());
                        }
                        break;
                    case CMD_SKIPTO:
                        leavePara();
                        if (!quoting)
                            quoter.quoteTo( location(), commandStr, getRestOfLine() );
                        else {
                            append(Atom::CodeQuoteCommand, commandStr);
                            append(Atom::CodeQuoteArgument, getRestOfLine());
                        }
                        break;
                    case CMD_SKIPUNTIL:
                        leavePara();
                        if (!quoting)
                            quoter.quoteUntil( location(), commandStr, getRestOfLine() );
                        else {
                            append(Atom::CodeQuoteCommand, commandStr);
                            append(Atom::CodeQuoteArgument, getRestOfLine());
                        }
                        break;
                    case CMD_SNIPPET:
                        leavePara();
                        {
                            QString snippet = getArgument();
                            QString identifier = getRestOfLine();
                            if (quoting) {
                                append(Atom::SnippetCommand, commandStr);
                                append(Atom::SnippetLocation, snippet);
                                append(Atom::SnippetIdentifier, identifier);
                            } else {
                                Doc::quoteFromFile(location(), quoter, snippet);
                                appendToCode(quoter.quoteSnippet(location(), identifier));
                            }
                        }
                        break;
                    case CMD_SUB:
                        startFormat( ATOM_FORMATTING_SUBSCRIPT, command );
                        break;
                    case CMD_SUP:
                        startFormat( ATOM_FORMATTING_SUPERSCRIPT, command );
                        break;
                    case CMD_TABLE:
                        x = getRestOfLine();
                        if ( openCommand(command) ) {
                            leavePara();
                            append(Atom::TableLeft, x);
                            inTableHeader = false;
                            inTableRow = false;
                            inTableItem = false;
                        }
                        break;
                    case CMD_TABLEOFCONTENTS:
                        x = "1";
                        if (isLeftBraceAhead())
                            x = getArgument();
                        x += ",";
                        x += QString::number((int)getSectioningUnit());
                        append(Atom::TableOfContents, x);
                        break;
                    case CMD_TARGET:
                        insertTarget(getRestOfLine(), false);
                        break;
                    case CMD_TT:
                        startFormat( ATOM_FORMATTING_TELETYPE, command );
                        break;
                    case CMD_UNDERLINE:
                        startFormat( ATOM_FORMATTING_UNDERLINE, command );
                        break;
                    case CMD_UNICODE:
                        enterPara();
                        x = getArgument();
                        {
                            bool ok;
                            uint unicodeChar = x.toUInt(&ok, 0);
                            if (!ok || unicodeChar == 0x0000 || unicodeChar > 0xFFFE) {
                                location().warning(tr("Invalid Unicode character '%1' specified "
                                                      "with '%2'")
                                                   .arg(x, commandName(CMD_UNICODE)));
                            } else {
                                append(Atom::String, QChar(unicodeChar));
                            }
                        }
                        break;
                    case CMD_VALUE:
                        leaveValue();
                        if ( openedLists.top().style() == OpenedList::Value ) {
                            x = getArgument();
                            if (!priv->enumItemList.contains(x))
                                priv->enumItemList.append(x);

                            openedLists.top().next();
                            append( Atom::ListTagLeft, ATOM_LIST_VALUE );
                            append( Atom::String, x );
                            append( Atom::ListTagRight, ATOM_LIST_VALUE );
                            append( Atom::ListItemLeft, ATOM_LIST_VALUE );

                            skipSpacesOrOneEndl();
                            if (isBlankLine())
                                append(Atom::Nop);
                        } else {
                            // ### problems
                        }
                        break;
                    case CMD_WARNING:
                        startNewPara();
                        append( Atom::FormattingLeft, ATOM_FORMATTING_BOLD );
                        append( Atom::String, "Warning:" );
                        append( Atom::FormattingRight, ATOM_FORMATTING_BOLD );
                        append( Atom::String, " " );
                        break;
                    case UNKNOWN_COMMAND:
                        if ( metaCommandSet.contains(commandStr) ) {
                            priv->metaCommandSet.insert( commandStr );
                            priv->metaCommandMap[commandStr].append(
                                    getMetaCommandArgument(commandStr));
                        } else if (macroHash()->contains(commandStr)) {
                            const Macro &macro = macroHash()->value(commandStr);
                            int numPendingFi = 0;
                            QMap<QString, QString>::ConstIterator d = macro.otherDefs.begin();
                            while ( d != macro.otherDefs.end() ) {
                                append( Atom::FormatIf, d.key() );
                                expandMacro( commandStr, *d, macro.numParams );
                                ++d;

                                if (d == macro.otherDefs.end()) {
                                    append( Atom::FormatEndif );
                                } else {
                                    append( Atom::FormatElse );
                                    numPendingFi++;
                                }
                            }
                            while ( numPendingFi-- > 0 )
                                append( Atom::FormatEndif );

                            if (!macro.defaultDef.isEmpty()) {
                                if (!macro.otherDefs.isEmpty()) {
                                    macro.defaultDefLocation.warning(tr("Macro cannot have both"
                                                                        " format-specific and qdoc-"
                                                                        " syntax definitions"));
                                } else {
                                    location().push(macro.defaultDefLocation.filePath()); // ###
                                    in.insert(pos, macro.defaultDef);
                                    len = in.length();
                                    openedInputs.push(pos + macro.defaultDef.length());
                                }
                            }
                        } else {
                            location().warning(
                                    tr("Unknown command '\\%1'").arg(commandStr),
                                    detailsUnknownCommand(metaCommandSet, commandStr));
                            enterPara();
                            append(Atom::UnknownCommand, commandStr);
                        }
                    }
                }
            }
            break;
        case '{':
            enterPara();
            appendChar( '{' );
            braceDepth++;
            pos++;
            break;
        case '}':
            {
                braceDepth--;
                pos++;

                QMap<int, QString>::Iterator f = pendingFormats.find( braceDepth );
                if ( f == pendingFormats.end() ) {
                    enterPara();
                    appendChar( '}' );
                } else {
                    append( Atom::FormattingRight, *f );
                    if (*f == ATOM_FORMATTING_INDEX) {
                        if (indexStartedPara)
                            skipAllSpaces();
                    } else if (*f == ATOM_FORMATTING_LINK) {
                        // hack for C++ to support links like \l{QString::}{count()}
                        if (currentLinkAtom && currentLinkAtom->string().endsWith("::")) {
                            QString suffix = Text::subText(currentLinkAtom,
                                                           priv->text.lastAtom()).toString();
                            currentLinkAtom->appendString(suffix);
                        }
                        currentLinkAtom = 0;
                    }
                    pendingFormats.erase( f );
                }
            }
            break;
        default:
            {
                bool newWord;

                switch (priv->text.lastAtom()->type()) {
                case Atom::ParaLeft:
                    newWord = true;
                    break;
                default:
                    newWord = false;
                }

                if ( paraState == OutsidePara ) {
                    if (ch.isSpace()) {
                        ++pos;
                        newWord = false;
                    } else {
                        enterPara();
                        newWord = true;
                    }
                } else {
                    if (ch.isSpace()) {
                        ++pos;
                        if ((ch == '\n') && (paraState == InsideSingleLinePara || isBlankLine())) {
                            leavePara();
                            newWord = false;
                        } else {
                            appendChar(' ');
                            newWord = true;
                        }
                    } else {
                        newWord = true;
                    }
                }

                if (newWord) {
                    int startPos = pos;
                    int numInternalUppercase = 0;
                    int numLowercase = 0;
                    int numStrangeSymbols = 0;

                    while (pos < len) {
                        unsigned char latin1Ch = in.at(pos).toLatin1();

                        if (islower(latin1Ch)) {
                            ++numLowercase;
                            ++pos;
                        } else if (isupper(latin1Ch)) {
                            if (pos > startPos)
                                ++numInternalUppercase;
                            ++pos;
                        } else if (isdigit(latin1Ch)) {
                            if (pos > startPos) {
                                ++pos;
                            } else {
                                break;
                            }
                        } else if (latin1Ch == '_' || latin1Ch == '@') {
                            ++numStrangeSymbols;
                            ++pos;
                        } else if (latin1Ch == ':' && pos < len - 1
                                   && in.at(pos + 1) == QLatin1Char(':')) {
                            ++numStrangeSymbols;
                            pos += 2;
                        } else if (latin1Ch == '(') {
                            if (pos > startPos) {
                                if (pos < len - 1 && in.at(pos + 1) == QLatin1Char(')')) {
                                    ++numStrangeSymbols;
                                    pos += 2;
                                    break;
                                } else {
                                    // ### handle functions with signatures and function calls
                                    break;
                                }
                            } else {
                                break;
                            }
                        } else {
                            break;
                        }
                    }

                    if (pos == startPos) {
                        if (!ch.isSpace()) {
                            appendChar(ch);
                            ++pos;
                        }
                    } else {
                        QString word = in.mid(startPos, pos - startPos);

                        // is word a C++ symbol or an English word?
                        if ((numInternalUppercase >= 1 && numLowercase >= 2)
                                || numStrangeSymbols >= 1) {
                            append(Atom::AutoLink, word);
                        } else {
                            appendWord(word);
                        }
                    }
                }
            }
        }
    }
    leaveValueList();

    // for compatibility
    if (openedCommands.top() == CMD_LEGALESE) {
        append(Atom::LegaleseRight);
        openedCommands.pop();
    }

    if (openedCommands.top() != CMD_OMIT) {
        location().warning(tr("Missing '\\%1'").arg(endCommandName(openedCommands.top())));
    } else if (preprocessorSkipping.count() > 0) {
        location().warning(tr("Missing '\\%1'").arg(commandName(CMD_ENDIF)));
    }

    while (currentSectioningUnit > Doc::Chapter) {
        int delta = currentSectioningUnit - priv->extra->sectioningUnit;
        append( Atom::SectionRight, QString::number(delta) );
        currentSectioningUnit = Doc::SectioningUnit(int(currentSectioningUnit) - 1);
    }

    if (priv->extra && priv->extra->granularity < priv->extra->sectioningUnit)
        priv->extra->granularity = priv->extra->sectioningUnit;
    priv->text.stripFirstAtom();
}

Location &DocParser::location()
{
    while (!openedInputs.isEmpty() && openedInputs.top() <= pos) {
        cachedLoc.pop();
        cachedPos = openedInputs.pop();
    }
    while (cachedPos < pos)
        cachedLoc.advance(in.at(cachedPos++));
    return cachedLoc;
}

QString DocParser::detailsUnknownCommand(const QSet<QString> &metaCommandSet, const QString &str)
{
    QSet<QString> commandSet = metaCommandSet;
    int i = 0;
    while ( cmds[i].english != 0 ) {
        commandSet.insert(*cmds[i].alias);
        i++;
    }

    if (aliasMap()->contains(str))
        return tr( "The command '\\%1' was renamed '\\%2' by the configuration"
                   " file. Use the new name." )
               .arg(str).arg((*aliasMap())[str]);

    QString best = nearestName( str, commandSet );
    if ( best.isEmpty() )
        return QString();
    return tr( "Maybe you meant '\\%1'?" ).arg( best );
}

void DocParser::checkExpiry( const QString& date )
{
    QRegExp ymd( "(\\d{4})(?:-(\\d{2})(?:-(\\d{2})))" );

    if ( ymd.exactMatch(date) ) {
	int y = ymd.cap( 1 ).toInt();
	int m = ymd.cap( 2 ).toInt();
	int d = ymd.cap( 3 ).toInt();

	if ( m == 0 )
	    m = 1;
	if ( d == 0 )
	    d = 1;
	QDate expiryDate( y, m, d );
	if ( expiryDate.isValid() ) {
	    int days = expiryDate.daysTo( QDate::currentDate() );
	    if ( days == 0 ) {
		location().warning( tr("Documentation expires today") );
	    } else if ( days == 1 ) {
		location().warning( tr("Documentation expired yesterday") );
	    } else if ( days >= 2 ) {
		location().warning( tr("Documentation expired %1 days ago")
				    .arg(days) );
	    }
	} else {
	    location().warning( tr("Date '%1' invalid").arg(date) );
	}
    } else {
	location().warning( tr("Date '%1' not in YYYY-MM-DD format")
			    .arg(date) );
    }
}

void DocParser::insertBaseName(const QString &baseName)
{
    priv->constructExtra();
    if ( currentSectioningUnit == priv->extra->sectioningUnit ) {
	priv->extra->baseName = baseName;
    } else {
	Atom *atom = priv->text.firstAtom();
	Atom *sectionLeft = 0;

	int delta = currentSectioningUnit - priv->extra->sectioningUnit;

	while ( atom != 0 ) {
	    if ( atom->type() == Atom::SectionLeft &&
		 atom->string().toInt() == delta )
		sectionLeft = atom;
	    atom = atom->next();
	}
	if ( sectionLeft != 0 )
	    (void) new Atom( sectionLeft, Atom::BaseName, baseName );
    }
}

void DocParser::insertTarget(const QString &target, bool keyword)
{
    if (targetMap.contains(target)) {
	location().warning( tr("Duplicate target name '%1'").arg(target) );
	targetMap[target].warning( tr("(The previous occurrence is here)") );
    } else {
	targetMap.insert(target, location());
	append(Atom::Target, target);
        priv->constructExtra();
        if (keyword)
            priv->extra->keywords.append(priv->text.lastAtom());
        else
            priv->extra->targets.append(priv->text.lastAtom());
    }
}

void DocParser::include( const QString& fileName )
{
    if ( location().depth() > 16 )
	location().fatal( tr("Too many nested '\\%1's")
			  .arg(commandName(CMD_INCLUDE)) );

    QString userFriendlyFilePath;
    // ### use current directory?
    QString filePath = Config::findFile( location(), sourceFiles, sourceDirs,
					 fileName, userFriendlyFilePath );
    if ( filePath.isEmpty() ) {
	location().warning( tr("Cannot find leaf file '%1'").arg(fileName) );
    } else {
	QFile inFile( filePath );
	if ( !inFile.open(QFile::ReadOnly) ) {
	    location().warning( tr("Cannot open leaf file '%1'")
				.arg(userFriendlyFilePath) );
	} else {
	    location().push( userFriendlyFilePath );

	    QTextStream inStream( &inFile );
	    QString includedStuff = inStream.readAll();
	    inFile.close();

	    in.insert( pos, includedStuff );
	    len = in.length();
	    openedInputs.push( pos + includedStuff.length() );
	}
    }
}

void DocParser::startFormat( const QString& format, int command )
{
    enterPara();

    QMap<int, QString>::ConstIterator f = pendingFormats.begin();
    while ( f != pendingFormats.end() ) {
	if ( *f == format ) {
	    location().warning( tr("Cannot nest '\\%1' commands")
				.arg(commandName(command)) );
	    return;
	}
	++f;
    }

    append( Atom::FormattingLeft, format );

    if ( isLeftBraceAhead() ) {
	skipSpacesOrOneEndl();
	pendingFormats.insert( braceDepth, format );
	++braceDepth;
	++pos;
    } else {
	append( Atom::String, getArgument() );
	append( Atom::FormattingRight, format );
	if ( format == ATOM_FORMATTING_INDEX && indexStartedPara ) {
	    skipAllSpaces();
	    indexStartedPara = false;
	}
    }
}

bool DocParser::openCommand( int command )
{
    int outer = openedCommands.top();
    bool ok = true;

    if (command != CMD_LINK) {
        if (outer == CMD_LIST) {
	    ok = (command == CMD_FOOTNOTE || command == CMD_LIST);
        } else if (outer == CMD_ABSTRACT) {
	    ok = (command == CMD_LIST || command == CMD_QUOTATION || command == CMD_TABLE);
        } else if (outer == CMD_SIDEBAR) {
	    ok = (command == CMD_LIST || command == CMD_QUOTATION || command == CMD_SIDEBAR);
        } else if (outer == CMD_QUOTATION) {
	    ok = (command == CMD_LIST);
        } else if (outer == CMD_TABLE) {
	    ok = (command == CMD_LIST || command == CMD_FOOTNOTE || command == CMD_QUOTATION);
        } else if (outer == CMD_FOOTNOTE || outer == CMD_LINK) {
	    ok = false;
        }
    }

    if (ok) {
	openedCommands.push(command);
    } else {
	location().warning(tr("Cannot use '\\%1' within '\\%2'")
			   .arg(commandName(command)).arg(commandName(outer)));
    }
    return ok;
}

bool DocParser::closeCommand( int endCommand )
{
    if ( endCommandFor(openedCommands.top()) == endCommand && openedCommands.size() > 1 ) {
	openedCommands.pop();
	return true;
    } else {
	bool contains = false;
	QStack<int> opened2 = openedCommands;
	while ( opened2.size() > 1 ) {
	    if ( endCommandFor(opened2.top()) == endCommand ) {
		contains = true;
		break;
	    }
	    opened2.pop();
	}

	if ( contains ) {
	    while (endCommandFor(openedCommands.top()) != endCommand && openedCommands.size() > 1) {
		location().warning( tr("Missing '\\%1' before '\\%2'")
				    .arg(endCommandName(openedCommands.top()))
				    .arg(commandName(endCommand)) );
		openedCommands.pop();
	    }
	} else {
	    location().warning( tr("Unexpected '\\%1'")
				.arg(commandName(endCommand)) );
	}
	return false;
    }
}

void DocParser::startSection( Doc::SectioningUnit unit, int command )
{
    leavePara();

    if ( currentSectioningUnit == Doc::Book ) {
	if ( unit > Doc::Section1 )
	    location().warning( tr("Unexpected '\\%1' without '\\%2'")
				.arg(commandName(command))
				.arg(commandName(CMD_SECTION1)) );
	currentSectioningUnit = (Doc::SectioningUnit) ( unit - 1 );
	priv->constructExtra();
	priv->extra->sectioningUnit = currentSectioningUnit;
    }

    if ( unit <= priv->extra->sectioningUnit ) {
	location().warning( tr("Unexpected '\\%1' in this documentation")
			    .arg(commandName(command)) );
    } else if ( unit - currentSectioningUnit > 1 ) {
	location().warning( tr("Unexpected '\\%1' at this point")
			    .arg(commandName(command)) );
    } else {
	if ( currentSectioningUnit >= unit )
	    endSection( unit, command );

	int delta = unit - priv->extra->sectioningUnit;
	append( Atom::SectionLeft, QString::number(delta) );
        priv->constructExtra();
        priv->extra->tableOfContents.append(priv->text.lastAtom());
        priv->extra->tableOfContentsLevels.append(unit);
	enterPara( Atom::SectionHeadingLeft, Atom::SectionHeadingRight,
		   QString::number(delta) );
	currentSectioningUnit = unit;
    }
}

void DocParser::endSection( int unit, int endCommand )
{
    leavePara();

    if ( unit < priv->extra->sectioningUnit ) {
	location().warning( tr("Unexpected '\\%1' in this documentation")
			    .arg(commandName(endCommand)) );
    } else if ( unit > currentSectioningUnit ) {
	location().warning( tr("Unexpected '\\%1' at this point")
			    .arg(commandName(endCommand)) );
    } else {
	while ( currentSectioningUnit >= unit ) {
	    int delta = currentSectioningUnit - priv->extra->sectioningUnit;
	    append( Atom::SectionRight, QString::number(delta) );
	    currentSectioningUnit =
		    (Doc::SectioningUnit) ( currentSectioningUnit - 1 );
	}
    }
}

void DocParser::parseAlso()
{
    leavePara();
    skipSpacesOnLine();
    while ( pos < len && in[pos] != '\n' ) {
        QString target;
        QString str;

        if ( in[pos] == '{' ) {
            target = getArgument();
            skipSpacesOnLine();
            if ( in[pos] == '{' ) {
                str = getArgument();

                // hack for C++ to support links like \l{QString::}{count()}
                if (target.endsWith("::"))
                    target += str;
            } else {
                str = target;
            }
#ifdef QDOC2_COMPAT
        } else if (in[pos] == '\\' && in.mid(pos, 5) == "\\link") {
            pos += 6;
            target = getArgument();
            int endPos = in.indexOf("\\endlink", pos);
            if (endPos != -1) {
                str = in.mid(pos, endPos - pos).trimmed();
                pos = endPos + 8;
            }
#endif
        } else {
            target = getArgument();
            str = cleanLink(target);
        }

        Text also;
        also << Atom( Atom::Link, target )
             << Atom( Atom::FormattingLeft, ATOM_FORMATTING_LINK ) << str
             << Atom( Atom::FormattingRight, ATOM_FORMATTING_LINK );
        priv->addAlso( also );

        skipSpacesOnLine();
        if ( pos < len && in[pos] == ',' ) {
            pos++;
            skipSpacesOrOneEndl();
        } else if ( in[pos] != '\n' ) {
            location().warning(tr("Missing comma in '\\%1'").arg(commandName(CMD_SA)) );
        }
    }
}

void DocParser::append(Atom::Type type, const QString &string)
{
    if (priv->text.lastAtom()->type() == Atom::Code
        && priv->text.lastAtom()->string().endsWith(QLatin1String("\n\n")))
        priv->text.lastAtom()->chopString();
    priv->text << Atom(type, string);
}

void DocParser::appendChar(QChar ch)
{
    if (priv->text.lastAtom()->type() != Atom::String)
        append(Atom::String);
    Atom *atom = priv->text.lastAtom();
    if (ch == QLatin1Char(' ')) {
        if (!atom->string().endsWith(QLatin1Char(' ')))
            atom->appendChar(QLatin1Char(' '));
    } else {
        atom->appendChar(ch);
    }
}

void DocParser::appendWord(const QString &word)
{
    if (priv->text.lastAtom()->type() != Atom::String) {
	append(Atom::String, word);
    } else {
	priv->text.lastAtom()->appendString(word);
    }
}

void DocParser::appendToCode( const QString& markedCode )
{
    if ( priv->text.lastAtom()->type() != Atom::Code )
	append( Atom::Code );
    priv->text.lastAtom()->appendString( markedCode );
}

void DocParser::startNewPara()
{
    leavePara();
    enterPara();
}

void DocParser::enterPara( Atom::Type leftType, Atom::Type rightType,
			   const QString& string )
{
    if ( paraState == OutsidePara ) {
        if (priv->text.lastAtom()->type() != Atom::ListItemLeft)
            leaveValueList();
	append( leftType, string );
	indexStartedPara = false;
	pendingParaLeftType = leftType;
	pendingParaRightType = rightType;
	pendingParaString = string;
	if (
#if 0
	     leftType == Atom::BriefLeft ||
#endif
	     leftType == Atom::SectionHeadingLeft ) {
	    paraState = InsideSingleLinePara;
	} else {
	    paraState = InsideMultiLinePara;
	}
	skipSpacesOrOneEndl();
    }
}

void DocParser::leavePara()
{
    if ( paraState != OutsidePara ) {
	if ( !pendingFormats.isEmpty() ) {
	    location().warning( tr("Missing '}'") );
	    pendingFormats.clear();
	}

	if ( priv->text.lastAtom()->type() == pendingParaLeftType ) {
	    priv->text.stripLastAtom();
	} else {
	    if ( priv->text.lastAtom()->type() == Atom::String &&
		 priv->text.lastAtom()->string().endsWith(" ") ) {
		priv->text.lastAtom()->chopString();
	    }
	    append( pendingParaRightType, pendingParaString );
	}
	paraState = OutsidePara;
	indexStartedPara = false;
	pendingParaRightType = Atom::Nop;
	pendingParaString = "";
    }
}

void DocParser::leaveValue()
{
    leavePara();
    if ( openedLists.isEmpty() ) {
	openedLists.push( OpenedList(OpenedList::Value) );
	append( Atom::ListLeft, ATOM_LIST_VALUE );
    } else {
        if (priv->text.lastAtom()->type() == Atom::Nop)
            priv->text.stripLastAtom();
	append( Atom::ListItemRight, ATOM_LIST_VALUE );
    }
}

void DocParser::leaveValueList()
{
    leavePara();
    if ( !openedLists.isEmpty() && openedLists.top().style() == OpenedList::Value) {
        if (priv->text.lastAtom()->type() == Atom::Nop)
            priv->text.stripLastAtom();
	append( Atom::ListItemRight, ATOM_LIST_VALUE );
	append( Atom::ListRight, ATOM_LIST_VALUE );
	openedLists.pop();
    }
}

void DocParser::leaveTableRow()
{
    if (inTableItem) {
        leavePara();
        append(Atom::TableItemRight);
        inTableItem = false;
    }
    if (inTableHeader) {
        append(Atom::TableHeaderRight);
        inTableHeader = false;
    }
    if (inTableRow) {
        append(Atom::TableRowRight);
        inTableRow = false;
    }
}

CodeMarker *DocParser::quoteFromFile()
{
    return Doc::quoteFromFile(location(), quoter, getArgument());
}

void DocParser::expandMacro(const QString &name, const QString &def, int numParams)
{
    if (numParams == 0) {
	append( Atom::RawString, def );
    } else {
	QStringList args;
	QString rawString;

	for ( int i = 0; i < numParams; i++ ) {
	    if ( numParams == 1 || isLeftBraceAhead() ) {
		args << getArgument( true );
	    } else {
		location().warning( tr("Macro '\\%1' invoked with too few"
				       " arguments (expected %2, got %3)")
				    .arg(name).arg(numParams).arg(i) );
		break;
	    }
	}

	int j = 0;
	while (j < def.size() ) {
	    int paramNo;
	    if (def[j] == '\\' && j < def.size() - 1
                    && (paramNo = def[j + 1].digitValue()) >= 1 && paramNo <= numParams) {
		if ( !rawString.isEmpty() ) {
		    append( Atom::RawString, rawString );
		    rawString = "";
		}
		append( Atom::String, args[paramNo - 1] );
		j += 2;
	    } else {
		rawString += def[j++];
	    }
	}
	if ( !rawString.isEmpty() )
	    append( Atom::RawString, rawString );
    }
}

Doc::SectioningUnit DocParser::getSectioningUnit()
{
    QString name = getOptionalArgument();

    if ( name == "part" ) {
	return Doc::Part;
    } else if ( name == "chapter" ) {
	return Doc::Chapter;
    } else if ( name == "section1" ) {
	return Doc::Section1;
    } else if ( name == "section2" ) {
	return Doc::Section2;
    } else if ( name == "section3" ) {
	return Doc::Section3;
    } else if ( name == "section4" ) {
	return Doc::Section4;
    } else if (name.isEmpty()) {
	return Doc::Section4;
    } else {
	location().warning( tr("Invalid sectioning unit '%1'").arg(name) );
	return Doc::Book;
    }
}

QString DocParser::getArgument( bool verbatim )
{
    QString arg;
    int delimDepth = 0;

    skipSpacesOrOneEndl();

    int startPos = pos;

    /*
      Typically, an argument ends at the next white-space. However,
      braces can be used to group words:

	  {a few words}

      Also, opening and closing parentheses have to match. Thus,

	  printf("%d\n", x)

      is an argument too, although it contains spaces. Finally,
      trailing punctuation is not included in an argument, nor is 's.
    */
    if ( pos < (int) in.length() && in[pos] == '{' ) {
	pos++;
	while ( pos < (int) in.length() && delimDepth >= 0 ) {
	    switch ( in[pos].unicode() ) {
	    case '{':
		delimDepth++;
		arg += "{";
		pos++;
		break;
	    case '}':
		delimDepth--;
		if ( delimDepth >= 0 )
		    arg += "}";
		pos++;
		break;
	    case '\\':
		if ( verbatim ) {
		    arg += in[pos];
		    pos++;
		} else {
		    pos++;
		    if ( pos < (int) in.length() ) {
			if ( in[pos].isLetterOrNumber() )
			    break;
			arg += in[pos];
			if ( in[pos].isSpace() ) {
			    skipAllSpaces();
			} else {
			    pos++;
			}
		    }
		}
		break;
	    default:
		arg += in[pos];
		pos++;
	    }
	}
	if ( delimDepth > 0 )
	    location().warning( tr("Missing '}'") );
    } else {
	while ( pos < in.length() &&
		(delimDepth > 0 || (delimDepth == 0 && !in[pos].isSpace())) ) {
	    switch ( in[pos].unicode() ) {
	    case '(':
	    case '[':
	    case '{':
		delimDepth++;
		arg += in[pos];
		pos++;
		break;
	    case ')':
	    case ']':
	    case '}':
		delimDepth--;
		if ( pos == startPos || delimDepth >= 0 ) {
		    arg += in[pos];
		    pos++;
		}
		break;
	    case '\\':
		if ( verbatim ) {
		    arg += in[pos];
		    pos++;
		} else {
		    pos++;
		    if ( pos < (int) in.length() ) {
			if ( in[pos].isLetterOrNumber() )
			    break;
			arg += in[pos];
			if ( in[pos].isSpace() ) {
			    skipAllSpaces();
			} else {
			    pos++;
			}
		    }
		}
		break;
	    default:
		arg += in[pos];
		pos++;
	    }
	}
	if ( arg.length() > 1 && QString(".,:;!?").indexOf(in[pos - 1]) != -1 &&
	     !arg.endsWith("...") ) {
	    arg.truncate( arg.length() - 1 );
	    pos--;
	}
	if ( arg.length() > 2 && in.mid(pos - 2, 2) == "'s" ) {
	    arg.truncate( arg.length() - 2 );
	    pos -= 2;
	}
    }
    return arg.simplified();
}

QString DocParser::getOptionalArgument()
{
    skipSpacesOrOneEndl();
    if ( pos + 1 < (int) in.length() && in[pos] == '\\' &&
	 in[pos + 1].isLetterOrNumber() ) {
	return "";
    } else {
	return getArgument();
    }
}

QString DocParser::getRestOfLine()
{
    QString t;

    skipSpacesOnLine();

    bool trailingSlash = false;

    do {
        int begin = pos;

        while (pos < in.size() && in[pos] != '\n') {
            if (in[pos] == '\\' && !trailingSlash) {
                trailingSlash = true;
                ++pos;
                while (pos < in.size() && in[pos].isSpace() && in[pos] != '\n')
                    ++pos;
            } else {
                trailingSlash = false;
                ++pos;
            }
        }

        if (!t.isEmpty())
            t += " ";
        t += in.mid( begin, pos - begin ).simplified();

        if (trailingSlash) {
            t.chop(1);
            t = t.simplified();
        }
        if (pos < in.size())
            ++pos;
    } while (pos < in.size() && trailingSlash);

    return t;
}

QString DocParser::getMetaCommandArgument(const QString &commandStr)
{
    skipSpacesOnLine();

    int begin = pos;
    int parenDepth = 0;

    while ( pos < in.size() && (in[pos] != '\n' || parenDepth > 0)) {
        if (in.at(pos) == '(')
            ++parenDepth;
        else if (in.at(pos) == ')')
            --parenDepth;

	++pos;
    }
    if (pos == in.size() && parenDepth > 0) {
        pos = begin;
        location().warning(tr("Unbalanced parentheses in '%1'").arg(commandStr));
    }

    QString t = in.mid( begin, pos - begin ).simplified();
    skipSpacesOnLine();
    return t;
}

QString DocParser::getUntilEnd(int command)
{
    int endCommand = endCommandFor( command );
    QRegExp rx( "\\\\" + commandName(endCommand) + "\\b" );
    QString t;
    int end = rx.indexIn( in, pos );

    if ( end == -1 ) {
	location().warning(tr("Missing '\\%1'").arg(commandName(endCommand)));
	pos = in.length();
    } else {
	t = in.mid( pos, end - pos );
	pos = end + rx.matchedLength();
    }
    return t;
}

QString DocParser::getCode(int command, CodeMarker *marker)
{
    QString code = untabifyEtc(getUntilEnd(command));
    int indent = indentLevel(code);
    if (indent < minIndent)
        minIndent = indent;
    code = unindent(minIndent, code);
    marker = CodeMarker::markerForCode(code);
    return marker->markedUpCode(code, 0, "");
}

bool DocParser::isBlankLine()
{
    int i = pos;

    while ( i < len && in[i].isSpace() ) {
	if ( in[i] == '\n' )
	    return true;
	i++;
    }
    return false;
}

bool DocParser::isLeftBraceAhead()
{
    int numEndl = 0;
    int i = pos;

    while ( i < len && in[i].isSpace() && numEndl < 2 ) {
	// ### bug with '\\'
	if ( in[i] == '\n' )
	    numEndl++;
    	i++;
    }
    return numEndl < 2 && i < len && in[i] == '{';
}

void DocParser::skipSpacesOnLine()
{
    while (pos < in.length() && in[pos].isSpace() && in[pos].unicode() != '\n')
	++pos;
}

void DocParser::skipSpacesOrOneEndl()
{
    int firstEndl = -1;
    while ( pos < (int) in.length() && in[pos].isSpace() ) {
	QChar ch = in[pos];
	if ( ch == '\n' ) {
	    if ( firstEndl == -1 ) {
		firstEndl = pos;
	    } else {
		pos = firstEndl;
		break;
	    }
	}
	pos++;
    }
}

void DocParser::skipAllSpaces()
{
    while ( pos < len && in[pos].isSpace() )
	pos++;
}

void DocParser::skipToNextPreprocessorCommand()
{
    QRegExp rx("\\\\(?:" + commandName(CMD_IF) + "|" + commandName(CMD_ELSE) + "|"
	       + commandName(CMD_ENDIF) + ")\\b");
    int end = rx.indexIn(in, pos + 1); // ### + 1 necessary?

    if (end == -1)
	pos = in.length();
    else
	pos = end;
}

int DocParser::endCommandFor( int command )
{
    switch ( command ) {
    case CMD_ABSTRACT:
	return CMD_ENDABSTRACT;
    case CMD_BADCODE:
	return CMD_ENDCODE;
    case CMD_CHAPTER:
	return CMD_ENDCHAPTER;
    case CMD_CODE:
	return CMD_ENDCODE;
    case CMD_FOOTNOTE:
	return CMD_ENDFOOTNOTE;
    case CMD_LEGALESE:
        return CMD_ENDLEGALESE;
    case CMD_LINK:
        return CMD_ENDLINK;
    case CMD_LIST:
	return CMD_ENDLIST;
    case CMD_NEWCODE:
        return CMD_ENDCODE;
    case CMD_OLDCODE:
        return CMD_NEWCODE;
    case CMD_OMIT:
	return CMD_ENDOMIT;
    case CMD_PART:
	return CMD_ENDPART;
    case CMD_QUOTATION:
	return CMD_ENDQUOTATION;
    case CMD_RAW:
        return CMD_ENDRAW;
    case CMD_SECTION1:
	return CMD_ENDSECTION1;
    case CMD_SECTION2:
	return CMD_ENDSECTION2;
    case CMD_SECTION3:
	return CMD_ENDSECTION3;
    case CMD_SECTION4:
	return CMD_ENDSECTION4;
    case CMD_SIDEBAR:
	return CMD_ENDSIDEBAR;
    case CMD_TABLE:
	return CMD_ENDTABLE;
    default:
	return command;
    }
}

QString DocParser::commandName( int command )
{
    return *cmds[command].alias;
}

QString DocParser::endCommandName( int command )
{
    return commandName( endCommandFor(command) );
}

QString DocParser::untabifyEtc( const QString& str )
{
    QString result;
    result.reserve(str.length());
    int column = 0;

    for ( int i = 0; i < str.length(); i++ ) {
        const QChar c = str.at(i);
        if ( c == QLatin1Char('\r') )
            continue;
        if ( c == QLatin1Char('\t') ) {
            result += "        " + ( column % tabSize );
            column = ( (column / tabSize) + 1 ) * tabSize;
            continue;
        }
        if ( c == QLatin1Char('\n') ) {
            while ( result.endsWith(QLatin1Char(' ')) )
                result.chop(1);
            result += c;
            column = 0;
            continue;
        }
        result += c;
        column++;
    }

    while ( result.endsWith("\n\n") )
        result.truncate( result.length() - 1 );
    while ( result.startsWith("\n") )
        result = result.mid( 1 );
   
    return result;
}

int DocParser::indentLevel( const QString& str )
{
    int minIndent = INT_MAX;
    int column = 0;

    for ( int i = 0; i < (int) str.length(); i++ ) {
	if ( str[i] == '\n' ) {
	    column = 0;
	} else {
	    if ( str[i] != ' ' && column < minIndent )
		minIndent = column;
	    column++;
	}
    }
    return minIndent;
}

QString DocParser::unindent( int level, const QString& str )
{
    if ( level == 0 )
	return str;

    QString t;
    int column = 0;

    for ( int i = 0; i < (int) str.length(); i++ ) {
        if ( str[i] == QLatin1Char('\n') ) {
	    t += '\n';
	    column = 0;
	} else {
	    if ( column >= level )
		t += str[i];
	    column++;
	}
    }
    return t;
}

QString DocParser::slashed( const QString& str )
{
    QString result = str;
    result.replace( "/", "\\/" );
    return "/" + result + "/";
}

#define COMMAND_BRIEF                   Doc::alias("brief")

Doc::Doc( const Location& location, const QString& source,
	  const QSet<QString>& metaCommandSet )
{
    priv = new DocPrivate( location, source );
    DocParser parser;
    parser.parse( source, priv, metaCommandSet );
}

Doc::Doc( const Doc& doc )
    : priv( 0 )
{
    operator=( doc );
}

Doc::~Doc()
{
    if (priv && priv->deref())
	delete priv;
}

Doc &Doc::operator=( const Doc& doc )
{
    if (doc.priv)
	doc.priv->ref();
    if (priv && priv->deref())
	delete priv;
    priv = doc.priv;
    return *this;
}

void Doc::renameParameters(const QStringList &oldNames, const QStringList &newNames)
{
    if (priv && oldNames != newNames) {
        detach();

        priv->params = newNames.toSet();

        Atom *atom = priv->text.firstAtom();
        while (atom) {
            if (atom->type() == Atom::FormattingLeft
                    && atom->string() == ATOM_FORMATTING_PARAMETER) {
                atom = atom->next();
                if (!atom)
                    return;
                int index = oldNames.indexOf(atom->string());
                if (index != -1 && index < newNames.count())
                    atom->setString(newNames.at(index));
            }
            atom = atom->next();
        }
    }
}

void Doc::simplifyEnumDoc()
{
    if (priv) {
        if (priv->isEnumDocSimplifiable()) {
            detach();

            Text newText;

            Atom *atom = priv->text.firstAtom();
            while (atom) {
                if (atom->type() == Atom::ListLeft && atom->string() == ATOM_LIST_VALUE) {
                    while (atom && (atom->type() != Atom::ListRight
                                    || atom->string() != ATOM_LIST_VALUE))
                        atom = atom->next();
                    if (atom)
                        atom = atom->next();
                } else {
                    newText << *atom;
                    atom = atom->next();
                }
            }
            priv->text = newText;
        }
    }
}

void Doc::setBody(const Text &text)
{
    detach();
    priv->text = text;
}

const Location &Doc::location() const
{
    static const Location dummy;
    return priv == 0 ? dummy : priv->loc;
}

const QString &Doc::source() const
{
    static QString null;
    return priv == 0 ? null : priv->src;
}

bool Doc::isEmpty() const
{
    return priv == 0 || priv->src.isEmpty();
}

const Text& Doc::body() const
{
    static const Text dummy;
    return priv == 0 ? dummy : priv->text;
}

Text Doc::briefText() const
{
    return body().subText( Atom::BriefLeft, Atom::BriefRight );
}

Text Doc::trimmedBriefText(const QString &className) const
{
    QString classNameOnly = className;
    if (className.contains("::"))
        classNameOnly = className.split("::").last();

    Text originalText = briefText();
    Text resultText;
    const Atom *atom = originalText.firstAtom();
    if (atom) {
        QString briefStr;
        QString whats;
        bool standardWording = true;

        /*
            This code is really ugly. The entire \brief business
            should be rethought.
        */
        while (atom && (atom->type() == Atom::AutoLink || atom->type() == Atom::String)) {
            briefStr += atom->string();
            atom = atom->next();
        }

        QStringList w = briefStr.split(" ");
        if (!w.isEmpty() && w.first() == "The")
	    w.removeFirst();
        else {
	    location().warning(
                tr("Nonstandard wording in '\\%1' text for '%2' (expected 'The')")
                .arg(COMMAND_BRIEF).arg(className));
	    standardWording = false;
        }

        if (!w.isEmpty() && (w.first() == className || w.first() == classNameOnly))
	    w.removeFirst();
        else {
	    location().warning(
                tr("Nonstandard wording in '\\%1' text for '%2' (expected '%3')")
                .arg(COMMAND_BRIEF).arg(className).arg(className));
	    standardWording = false;
        }

        if (!w.isEmpty() && (w.first() == "class" || w.first() == "widget"
                             || w.first() == "namespace" || w.first() == "header"))
	    w.removeFirst();
        else {
	    location().warning(
                tr("Nonstandard wording in '\\%1' text for '%2' ("
                   "expected 'class', 'widget', 'namespace' or 'header')")
                .arg(COMMAND_BRIEF).arg(className).arg(className));
	    standardWording = false;
        }

        if (!w.isEmpty() && (w.first() == "is" || w.first() == "provides"))
	    w.removeFirst();

        if (!w.isEmpty() && (w.first() == "a" || w.first() == "an"))
	    w.removeFirst();

        whats = w.join(" ");
        if (whats.endsWith("."))
	    whats.truncate(whats.length() - 1);

        if (whats.isEmpty()) {
	    location().warning(
                tr("Nonstandard wording in '\\%1' text for '%2' (expected more text)")
                .arg(COMMAND_BRIEF).arg(className).arg(className));
	    standardWording = false;
        } else
	    whats[0] = whats[0].toUpper();

	// ### move this once \brief is abolished for properties
        if (standardWording)
            resultText << whats;
    }
    return resultText;
}

Text Doc::legaleseText() const
{
    if (priv == 0 || !priv->hasLegalese)
	return Text();
    else
	return body().subText(Atom::LegaleseLeft, Atom::LegaleseRight);
}

const QString& Doc::baseName() const
{
    static QString null;
    if ( priv == 0 || priv->extra == 0 ) {
	return null;
    } else {
	return priv->extra->baseName;
    }
}

Doc::SectioningUnit Doc::granularity() const
{
    if ( priv == 0 || priv->extra == 0 ) {
	return DocPrivateExtra().granularity;
    } else {
	return priv->extra->granularity;
    }
}

#if notyet // ###
Doc::SectioningUnit Doc::sectioningUnit() const
{
    if ( priv == 0 || priv->extra == 0 ) {
	return DocPrivateExtra().sectioningUnit;
    } else {
	return priv->extra->sectioningUnit;
    }
}
#endif

const QSet<QString> &Doc::parameterNames() const
{
    return priv == 0 ? *null_Set_QString() : priv->params;
}

const QStringList &Doc::enumItemNames() const
{
    return priv == 0 ? *null_QStringList() : priv->enumItemList;
}

const QStringList &Doc::omitEnumItemNames() const
{
    return priv == 0 ? *null_QStringList() : priv->omitEnumItemList;
}

const QSet<QString> &Doc::metaCommandsUsed() const
{
    return priv == 0 ? *null_Set_QString() : priv->metaCommandSet;
}

QStringList Doc::metaCommandArgs( const QString& metaCommand ) const
{
    if ( priv == 0 ) {
	return QStringList();
    } else {
	return priv->metaCommandMap.value(metaCommand);
    }
}

const QList<Text> &Doc::alsoList() const
{
    return priv == 0 ? *null_QList_Text() : priv->alsoList;
}

bool Doc::hasTableOfContents() const
{
    return priv && priv->extra && !priv->extra->tableOfContents.isEmpty();
}

bool Doc::hasKeywords() const
{
    return priv && priv->extra && !priv->extra->keywords.isEmpty();
}

bool Doc::hasTargets() const
{
    return priv && priv->extra && !priv->extra->targets.isEmpty();
}

const QList<Atom *> &Doc::tableOfContents() const
{
    priv->constructExtra();
    return priv->extra->tableOfContents;
}

const QList<int> &Doc::tableOfContentsLevels() const
{
    priv->constructExtra();
    return priv->extra->tableOfContentsLevels;
}

const QList<Atom *> &Doc::keywords() const
{
    priv->constructExtra();
    return priv->extra->keywords;
}

const QList<Atom *> &Doc::targets() const
{
    priv->constructExtra();
    return priv->extra->targets;
}

const QMap<QString, QString> &Doc::metaTagMap() const
{
    return priv && priv->extra ? priv->extra->metaMap : *null_QMap_QString_QString();
}

void Doc::initialize( const Config& config )
{
    DocParser::tabSize = config.getInt( CONFIG_TABSIZE );
    DocParser::exampleFiles = config.getStringList( CONFIG_EXAMPLES );
    DocParser::exampleDirs = config.getStringList( CONFIG_EXAMPLEDIRS );
    DocParser::sourceFiles = config.getStringList( CONFIG_SOURCES );
    DocParser::sourceDirs = config.getStringList( CONFIG_SOURCEDIRS );
    DocParser::quoting = config.getBool(CONFIG_QUOTINGINFORMATION);

    QMap<QString, QString> reverseAliasMap;

    QSet<QString> commands = config.subVars( CONFIG_ALIAS );
    QSet<QString>::ConstIterator c = commands.begin();
    while ( c != commands.end() ) {
	QString alias = config.getString( CONFIG_ALIAS + Config::dot + *c );
	if ( reverseAliasMap.contains(alias) ) {
	    config.lastLocation().warning( tr("Command name '\\%1' cannot stand"
					      " for both '\\%2' and '\\%3'")
					   .arg(alias)
					   .arg(reverseAliasMap[alias])
					   .arg(*c) );
	} else {
	    reverseAliasMap.insert( alias, *c );
	}
	aliasMap()->insert(*c, alias);
	++c;
    }

    int i = 0;
    while ( cmds[i].english ) {
	cmds[i].alias = new QString(alias(cmds[i].english));
	commandHash()->insert(*cmds[i].alias, cmds[i].no);

	if ( cmds[i].no != i )
	    Location::internalError( tr("command %1 missing").arg(i) );
	i++;
    }

    QSet<QString> macroNames = config.subVars( CONFIG_MACRO );
    QSet<QString>::ConstIterator n = macroNames.begin();
    while ( n != macroNames.end() ) {
	QString macroDotName = CONFIG_MACRO + Config::dot + *n;
	Macro macro;
	macro.numParams = -1;
	macro.defaultDef = config.getString( macroDotName );
	if ( !macro.defaultDef.isEmpty() ) {
	    macro.defaultDefLocation = config.lastLocation();
	    macro.numParams = Config::numParams( macro.defaultDef );
	}
	bool silent = false;

	QSet<QString> formats = config.subVars( macroDotName );
	QSet<QString>::ConstIterator f = formats.begin();
	while ( f != formats.end() ) {
	    QString def = config.getString( macroDotName + Config::dot + *f );
	    if ( !def.isEmpty() ) {
		macro.otherDefs.insert( *f, def );
		int m = Config::numParams( macro.defaultDef );
		if ( macro.numParams == -1 ) {
		    macro.numParams = m;
		} else if ( macro.numParams != m ) {
		    if ( !silent ) {
			QString other = tr( "default" );
			if ( macro.defaultDef.isEmpty() )
			    other = macro.otherDefs.begin().key();
			config.lastLocation().warning( tr("Macro '\\%1' takes"
							  " inconsistent number"
							  " of arguments (%2"
							  " %3, %4 %5)")
						       .arg(*n)
						       .arg(*f)
						       .arg(m)
						       .arg(other)
						       .arg(macro.numParams) );
			silent = true;
		    }
		    if ( macro.numParams < m )
			macro.numParams = m;
		}
	    }
	    ++f;
	}

	if (macro.numParams != -1)
	    macroHash()->insert(*n, macro);
	++n;
    }
}

void Doc::terminate()
{
    DocParser::exampleFiles.clear();
    DocParser::exampleDirs.clear();
    DocParser::sourceFiles.clear();
    DocParser::sourceDirs.clear();
    aliasMap()->clear();
    commandHash()->clear();
    macroHash()->clear();

    int i = 0;
    while ( cmds[i].english ) {
        delete cmds[i].alias;
        cmds[i].alias = 0;
        ++i;
    }
}

QString Doc::alias(const QString &english)
{
    return aliasMap()->value(english, english);
}

void Doc::trimCStyleComment( Location& location, QString& str )
{
    QString cleaned;
    Location m = location;
    bool metAsterColumn = true;
    int asterColumn = location.columnNo() + 1;
    int i;

    for ( i = 0; i < (int) str.length(); i++ ) {
	if ( m.columnNo() == asterColumn ) {
	    if ( str[i] != '*' )
		break;
	    cleaned += ' ';
	    metAsterColumn = true;
	} else {
	    if ( str[i] == '\n' ) {
		if ( !metAsterColumn )
		    break;
		metAsterColumn = false;
	    }
	    cleaned += str[i];
	}
	m.advance( str[i] );
    }
    if ( cleaned.length() == str.length() )
	str = cleaned;

    for ( int i = 0; i < 3; i++ )
	location.advance( str[i] );
    str = str.mid( 3, str.length() - 5 );
}

CodeMarker *Doc::quoteFromFile(const Location &location, Quoter &quoter, const QString &fileName)
{
    quoter.reset();

    QString code;

    QString userFriendlyFilePath;
    QString filePath = Config::findFile(location, DocParser::exampleFiles, DocParser::exampleDirs,
					fileName, userFriendlyFilePath);
    if ( filePath.isEmpty() ) {
	location.warning(tr("Cannot find example file '%1'").arg(fileName));
    } else {
	QFile inFile(filePath);
	if (!inFile.open(QFile::ReadOnly)) {
	    location.warning(tr("Cannot open example file '%1'").arg(userFriendlyFilePath));
	} else {
	    QTextStream inStream(&inFile);
	    code = DocParser::untabifyEtc(inStream.readAll());
	}
    }

    QString dirPath = QFileInfo(filePath).path();
    CodeMarker *marker = CodeMarker::markerForFileName(fileName);
    quoter.quoteFromFile(userFriendlyFilePath, code, marker->markedUpCode(code, 0, dirPath));
    return marker;
}

QString Doc::canonicalTitle(const QString &title)
{
    // The code below is equivalent to the following chunk, but _much_
    // faster (accounts for ~10% of total running time)
    //
    //  QRegExp attributeExpr("[^A-Za-z0-9]+");
    //  QString result = title.toLower();
    //  result.replace(attributeExpr, " ");
    //  result = result.simplified();
    //  result.replace(QLatin1Char(' '), QLatin1Char('-'));

    QString result;
    result.reserve(title.size());

    bool slurping = false;
    bool begun = false;
    int lastAlnum = 0;
    for (int i = 0; i != title.size(); ++i) {
        uint c = title.at(i).unicode();
        if (c >= 'A' && c <= 'Z')
            c -= 'A' - 'a';
        bool alnum = (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9');
        if (alnum) {
            result += QLatin1Char(c);
            begun = true;
            slurping = false;
            lastAlnum = result.size();
        } else if (!slurping) {
            if (begun)
                result += QLatin1Char('-');
            slurping = true;
        } else {
            // !alnum && slurping -> nothin
        }
    }
    result.truncate(lastAlnum);
    return result;
}

void Doc::detach()
{
    if (!priv) {
        priv = new DocPrivate;
        return;
    }
    if (priv->count == 1)
        return;

    --priv->count;

    DocPrivate *newPriv = new DocPrivate(*priv);
    newPriv->count = 1;
    if (priv->extra)
        newPriv->extra = new DocPrivateExtra(*priv->extra);

    priv = newPriv;
}

QT_END_NAMESPACE
