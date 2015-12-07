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

Q_GLOBAL_STATIC(QSet<QString>, null_Set_QString)
Q_GLOBAL_STATIC(QStringList, null_QStringList)
Q_GLOBAL_STATIC(QList<Text>, null_QList_Text)
Q_GLOBAL_STATIC(QStringMap, null_QStringMap)

struct Macro
{
    QString defaultDef;
    Location defaultDefLocation;
    QStringMap otherDefs;
    int numParams;
};

enum {
    CMD_A, CMD_ABSTRACT, CMD_BADCODE, CMD_BASENAME, CMD_BOLD,
    CMD_BRIEF, CMD_C, CMD_CAPTION, CMD_CHAPTER, CMD_CODE,
    CMD_CODELINE, CMD_DOTS, CMD_ELSE, CMD_ENDABSTRACT,
    CMD_ENDCHAPTER, CMD_ENDCODE, CMD_ENDFOOTNOTE, CMD_ENDIF,
    CMD_ENDLEGALESE, CMD_ENDLINK, CMD_ENDLIST, CMD_ENDOMIT,
    CMD_ENDPART, CMD_ENDQUOTATION, CMD_ENDRAW, CMD_ENDSECTION1,
    CMD_ENDSECTION2, CMD_ENDSECTION3, CMD_ENDSECTION4,
    CMD_ENDSIDEBAR, CMD_ENDTABLE, CMD_EXPIRE, CMD_FOOTNOTE,
    CMD_GENERATELIST, CMD_GRANULARITY, CMD_HEADER, CMD_I,
    CMD_IF, CMD_IMAGE, CMD_INCLUDE, CMD_INLINEIMAGE, CMD_INDEX,
    CMD_KEYWORD, CMD_L, CMD_LEGALESE, CMD_LINK, CMD_LIST,
    CMD_META, CMD_NEWCODE, CMD_O, CMD_OLDCODE, CMD_OMIT,
    CMD_OMITVALUE, CMD_OVERLOAD,
    CMD_PART, CMD_PRINTLINE, CMD_PRINTTO,
    CMD_PRINTUNTIL, CMD_QUOTATION, CMD_QUOTEFILE,
    CMD_QUOTEFROMFILE, CMD_QUOTEFUNCTION, CMD_RAW, CMD_ROW,
    CMD_SA, CMD_SECTION1, CMD_SECTION2, CMD_SECTION3,
    CMD_SECTION4, CMD_SIDEBAR, CMD_SKIPLINE, CMD_SKIPTO,
    CMD_SKIPUNTIL, CMD_SNIPPET, CMD_SUB, CMD_SUP, CMD_TABLE,
    CMD_TABLEOFCONTENTS, CMD_TARGET, CMD_TT, CMD_UNDERLINE,
    CMD_UNICODE, CMD_VALUE, CMD_WARNING, UNKNOWN_COMMAND
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
    { "overload", CMD_OVERLOAD, 0 },
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

typedef QHash<QString, int> QHash_QString_int;
typedef QHash<QString, Macro> QHash_QString_Macro;

Q_GLOBAL_STATIC(QStringMap, aliasMap)
Q_GLOBAL_STATIC(QHash_QString_int, commandHash)
Q_GLOBAL_STATIC(QHash_QString_Macro, macroHash)

class DocPrivateExtra
{
  public:
    QString             baseName;
    Doc::SectioningUnit granularity;
    Doc::SectioningUnit sectioningUnit; // ###
    QList<Atom*>        tableOfContents;
    QList<int>          tableOfContentsLevels;
    QList<Atom*>        keywords;
    QList<Atom*>        targets;
    QStringMap          metaMap;

    DocPrivateExtra()
	: granularity(Doc::Part) { }
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
    if ((colonPos == -1) ||
        (!link.startsWith("file:") && !link.startsWith("mailto:")))
        return link;
    return link.mid(colonPos + 1).simplified();
}

class DocPrivate : public Shared
{
  public:
    DocPrivate(const Location& start = Location::null,
               const Location& end = Location::null,
               const QString& source = "");
    ~DocPrivate();

    void addAlso(const Text& also);
    void constructExtra();
    bool isEnumDocSimplifiable() const;

    // ### move some of this in DocPrivateExtra
    Location start_loc;
    Location end_loc;
    QString src;
    Text text;
    QSet<QString> params;
    QList<Text> alsoList;
    QStringList enumItemList;
    QStringList omitEnumItemList;
    QSet<QString> metaCommandSet;
    QCommandMap metaCommandMap;
    bool hasLegalese : 1;
    bool hasSectioningUnits : 1;
    DocPrivateExtra *extra;
};

DocPrivate::DocPrivate(const Location& start,
                       const Location& end,
                       const QString& source)
    : start_loc(start),
      end_loc(end),
      src(source),
      hasLegalese(false),
      hasSectioningUnits(false),
      extra(0)
{
    // nothing.
}

DocPrivate::~DocPrivate()
{
    delete extra;
}

void DocPrivate::addAlso(const Text& also)
{
    alsoList.append(also);
}

void DocPrivate::constructExtra()
{
    if (extra == 0)
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
        } else if ((atom->type() == Atom::ListLeft) &&
                   (atom->string() == ATOM_LIST_VALUE)) {
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
    void parse(const QString &source,
               DocPrivate *docPrivate,
               const QSet<QString> &metaCommandSet);

    static int endCommandFor(int command);
    static QString commandName(int command);
    static QString endCommandName(int command);
    static QString untabifyEtc(const QString& str);
    static int indentLevel(const QString& str);
    static QString unindent(int level, const QString& str);
    static QString slashed(const QString& str);

    static int tabSize;
    static QStringList exampleFiles;
    static QStringList exampleDirs;
    static QStringList sourceFiles;
    static QStringList sourceDirs;
    static bool quoting;

  private:
    Location& location();
    QString detailsUnknownCommand(const QSet<QString>& metaCommandSet,
				   const QString& str);
    void checkExpiry(const QString& date);
    void insertBaseName(const QString &baseName);
    void insertTarget(const QString& target, bool keyword);
    void include(const QString& fileName);
    void startFormat(const QString& format, int command);
    bool openCommand(int command);
    bool closeCommand(int endCommand);
    void startSection(Doc::SectioningUnit unit, int command);
    void endSection(int unit, int endCommand);
    void parseAlso();
    void append(Atom::Type type, const QString& string = "");
    void appendChar(QChar ch);
    void appendWord(const QString &word);
    void appendToCode(const QString &code);
    void startNewPara();
    void enterPara(Atom::Type leftType = Atom::ParaLeft,
		    Atom::Type rightType = Atom::ParaRight,
		    const QString& string = "");
    void leavePara();
    void leaveValue();
    void leaveValueList();
    void leaveTableRow();
    CodeMarker *quoteFromFile();
    void expandMacro(const QString& name, const QString& def, int numParams);
    Doc::SectioningUnit getSectioningUnit();
    QString getArgument(bool verbatim = false);
    QString getOptionalArgument();
    QString getRestOfLine();
    QString getMetaCommandArgument(const QString &commandStr);
    QString getUntilEnd(int command);
    QString getCode(int command, CodeMarker *marker);
    QString getUnmarkedCode(int command);

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

void DocParser::parse(const QString& source,
                      DocPrivate *docPrivate,
                      const QSet<QString>& metaCommandSet)
{
    in = source;
    pos = 0;
    len = in.length();
    cachedLoc = docPrivate->start_loc;
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
    openedCommands.push(CMD_OMIT);
    quoter.reset();

    CodeMarker *marker = 0;
    Atom *currentLinkAtom = 0;
    QString x;
    QStack<bool> preprocessorSkipping;
    int numPreprocessorSkipping = 0;

    while (pos < len) {
        QChar ch = in.at(pos);

        switch (ch.unicode()) {
        case '\\':
            {
                QString commandStr;
                pos++;
                while (pos < len) {
                    ch = in.at(pos);
                    if (ch.isLetterOrNumber()) {
                        commandStr += ch;
                        pos++;
                    } else {
                        break;
                    }
                }
                if (commandStr.isEmpty()) {
                    if (pos < len) {
                        enterPara();
                        if (in.at(pos).isSpace()) {
                            skipAllSpaces();
                            appendChar(QLatin1Char(' '));
                        } else {
                            appendChar(in.at(pos++));
                        }
                    }
                } else {
                    int command = commandHash()->value(commandStr,
                                                       UNKNOWN_COMMAND);
                    switch (command) {
                    case CMD_A:
                        enterPara();
                        x = getArgument();
                        append(Atom::FormattingLeft,ATOM_FORMATTING_PARAMETER);
                        append(Atom::String, x);
                        append(Atom::FormattingRight,ATOM_FORMATTING_PARAMETER);
                        priv->params.insert(x);
                        break;
                    case CMD_ABSTRACT:
                        if (openCommand(command)) {
                            leavePara();
                            append(Atom::AbstractLeft);
                        }
                        break;
                    case CMD_BADCODE:
                        leavePara();
                        if (DoxWriter::isDoxPass())
                            append(Atom::CodeBad,getUnmarkedCode(CMD_BADCODE));
                        else
                            append(Atom::CodeBad,getCode(CMD_BADCODE, marker));
                        break;
                    case CMD_BASENAME:
                        leavePara();
                        insertBaseName(getArgument());
                        break;
                    case CMD_BOLD:
                        startFormat(ATOM_FORMATTING_BOLD, command);
                        break;
                    case CMD_BRIEF:
                        leavePara();
                        enterPara(Atom::BriefLeft, Atom::BriefRight);
                        break;
                    case CMD_C:
                        enterPara();
                        x = untabifyEtc(getArgument(true));
                        if (DoxWriter::isDoxPass())
                            append(Atom::C, x);
                        else {
                            marker = CodeMarker::markerForCode(x);
                            append(Atom::C, marker->markedUpCode(x, 0, ""));
                        }
                        break;
                    case CMD_CAPTION:
                        leavePara();
                        /* ... */
                        break;
                    case CMD_CHAPTER:
                        startSection(Doc::Chapter, command);
                        break;
                    case CMD_CODE:
                        leavePara();
                        if (DoxWriter::isDoxPass())
                            append(Atom::Code, getUnmarkedCode(CMD_CODE));
                        else
                            append(Atom::Code, getCode(CMD_CODE, marker));
                        break;
                    case CMD_CODELINE:
                        {
                            if (!quoting && !DoxWriter::isDoxPass()) {
                                if (priv->text.lastAtom()->type() == Atom::Code
                                        && priv->text.lastAtom()->string().endsWith("\n\n"))
                                    priv->text.lastAtom()->chopString();
                                appendToCode("\n");
                            } else {
                                append(Atom::CodeQuoteCommand, commandStr);
                                append(Atom::CodeQuoteArgument, " ");
                            }
                        }
                        break;
                    case CMD_DOTS:
                        {
                            if (DoxWriter::isDoxPass()) {
                                append(Atom::CodeQuoteCommand, commandStr);
                                append(Atom::CodeQuoteArgument, "    ...");
                            }
                            else if (!quoting) {
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
                                QString arg = getOptionalArgument();
                                if (arg.isEmpty())
                                    arg = "4";
                                append(Atom::CodeQuoteArgument, arg);
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
                        if (closeCommand(command)) {
                            leavePara();
                            append(Atom::AbstractRight);
                        }
                        break;
                    case CMD_ENDCHAPTER:
                        endSection(0, command);
                        break;
                    case CMD_ENDCODE:
                        closeCommand(command);
                        break;
                    case CMD_ENDFOOTNOTE:
                        if (closeCommand(command)) {
                            leavePara();
                            append(Atom::FootnoteRight);
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
                        if (closeCommand(command)) {
                            leavePara();
                            append(Atom::LegaleseRight);
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
                        if (closeCommand(command)) {
                            leavePara();
                            if (openedLists.top().isStarted()) {
                                append(Atom::ListItemRight,
                                        openedLists.top().styleString());
                                append(Atom::ListRight,
                                        openedLists.top().styleString());
                            }
                            openedLists.pop();
                        }
                        break;
                    case CMD_ENDOMIT:
                        closeCommand(command);
                        break;
                    case CMD_ENDPART:
                        endSection(-1, command);
                        break;
                    case CMD_ENDQUOTATION:
                        if (closeCommand(command)) {
                            leavePara();
                            append(Atom::QuotationRight);
                        }
                        break;
                    case CMD_ENDRAW:
                        location().warning(tr("Unexpected '\\%1'").arg(commandName(CMD_ENDRAW)));
                        break;
                    case CMD_ENDSECTION1:
                        endSection(1, command);
                        break;
                    case CMD_ENDSECTION2:
                        endSection(2, command);
                        break;
                    case CMD_ENDSECTION3:
                        endSection(3, command);
                        break;
                    case CMD_ENDSECTION4:
                        endSection(4, command);
                        break;
                    case CMD_ENDSIDEBAR:
                        if (closeCommand(command)) {
                            leavePara();
                            append(Atom::SidebarRight);
                        }
                        break;
                    case CMD_ENDTABLE:
                        if (closeCommand(command)) {
                            leaveTableRow();
                            append(Atom::TableRight);
                        }
                        break;
                    case CMD_EXPIRE:
                        checkExpiry(getArgument());
                        break;
                    case CMD_FOOTNOTE:
                        if (openCommand(command)) {
                            enterPara();
                            append(Atom::FootnoteLeft);
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
                        startFormat(ATOM_FORMATTING_ITALIC, command);
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
                        append(Atom::Image, getArgument());
                        append(Atom::ImageText, getRestOfLine());
                        break;
                    case CMD_INCLUDE:
                        include(getArgument());
                        break;
                    case CMD_INLINEIMAGE:
                        enterPara();
                        append(Atom::InlineImage, getArgument());
                        append(Atom::ImageText, getRestOfLine());
                        append(Atom::String, " ");
                        break;
                    case CMD_INDEX:
                        if (paraState == OutsidePara) {
                            enterPara();
                            indexStartedPara = true;
                        } else {
                            const Atom *last = priv->text.lastAtom();
                            if (indexStartedPara &&
                                 (last->type() != Atom::FormattingRight ||
                                  last->string() != ATOM_FORMATTING_INDEX))
                                indexStartedPara = false;
                        }
                        startFormat(ATOM_FORMATTING_INDEX, command);
                        break;
                    case CMD_KEYWORD:
                        insertTarget(getRestOfLine(),true);
                        break;
                    case CMD_L:
                        enterPara();
                        if (isLeftBraceAhead()) {
                            x = getArgument();
                            append(Atom::Link, x);
                            if (isLeftBraceAhead()) {
                                currentLinkAtom = priv->text.lastAtom();
                                startFormat(ATOM_FORMATTING_LINK, command);
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
                        if (openCommand(command)) {
                            leavePara();
                            openedLists.push(OpenedList(location(),
                                                         getOptionalArgument()));
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
                            if (openedLists.top().isStarted()) {
                                append(Atom::ListItemRight,
                                        openedLists.top().styleString());
                            } else {
                                append(Atom::ListLeft,
                                        openedLists.top().styleString());
                            }
                            openedLists.top().next();
                            append(Atom::ListItemNumber,
                                    openedLists.top().numberString());
                            append(Atom::ListItemLeft,
                                    openedLists.top().styleString());
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
                        if (DoxWriter::isDoxPass()) {
                            append(Atom::CodeOld, getUnmarkedCode(CMD_OLDCODE));
                            append(Atom::CodeNew, getUnmarkedCode(CMD_NEWCODE));
                        }
                        else {
                            append(Atom::CodeOld, getCode(CMD_OLDCODE, marker));
                            append(Atom::CodeNew, getCode(CMD_NEWCODE, marker));
                        }
                        break;
                    case CMD_OMIT:
                        getUntilEnd(command);
                        break;
                    case CMD_OMITVALUE:
                        x = getArgument();
                        if (!priv->enumItemList.contains(x))
                            priv->enumItemList.append(x);
                        if (!priv->omitEnumItemList.contains(x))
                            priv->omitEnumItemList.append(x);
                        break;
                    case CMD_PART:
                        startSection(Doc::Part, command);
                        break;
                    case CMD_PRINTLINE:
                        leavePara();
                        if (!quoting)
                            appendToCode(quoter.quoteLine(location(), commandStr,
                                                           getRestOfLine()));
                        else {
                            append(Atom::CodeQuoteCommand, commandStr);
                            append(Atom::CodeQuoteArgument, getRestOfLine());
                        }
                        break;
                    case CMD_PRINTTO:
                        leavePara();
                        if (!quoting)
                            appendToCode(quoter.quoteTo(location(), commandStr,
                                          getRestOfLine()));
                        else {
                            append(Atom::CodeQuoteCommand, commandStr);
                            append(Atom::CodeQuoteArgument, getRestOfLine());
                        }
                        break;
                    case CMD_PRINTUNTIL:
                        leavePara();
                        if (!quoting)
                            appendToCode(quoter.quoteUntil(location(), commandStr,
                                          getRestOfLine()));
                        else {
                            append(Atom::CodeQuoteCommand, commandStr);
                            append(Atom::CodeQuoteArgument, getRestOfLine());
                        }
                        break;
                    case CMD_QUOTATION:
                        if (openCommand(command)) {
                            leavePara();
                            append(Atom::QuotationLeft);
                        }
                        break;
                    case CMD_QUOTEFILE:
                    {
                        leavePara();
                        QString fileName = getArgument();
                        Doc::quoteFromFile(location(), quoter, fileName);
                        if (!quoting) {
                            append(Atom::Code,
                                    quoter.quoteTo(location(), commandStr, ""));
                            quoter.reset();
                        } else {
                            append(Atom::CodeQuoteCommand, commandStr);
                            append(Atom::CodeQuoteArgument, fileName);
                        }
                        break;
                    }
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
                            quoter.quoteTo(location(), commandStr,
                                            slashed(marker->functionBeginRegExp(x)));
                            append(Atom::Code,
                                    quoter.quoteUntil(location(), commandStr,
                                            slashed(marker->functionEndRegExp(x))));
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
                        startSection(Doc::Section1, command);
                        break;
                    case CMD_SECTION2:
                        startSection(Doc::Section2, command);
                        break;
                    case CMD_SECTION3:
                        startSection(Doc::Section3, command);
                        break;
                    case CMD_SECTION4:
                        startSection(Doc::Section4, command);
                        break;
                    case CMD_SIDEBAR:
                        if (openCommand(command)) {
                            leavePara();
                            append(Atom::SidebarLeft);
                        }
                        break;
                    case CMD_SKIPLINE:
                        leavePara();
                        if (!quoting)
                            quoter.quoteLine(location(),
                                             commandStr,
                                             getRestOfLine());
                        else {
                            append(Atom::CodeQuoteCommand, commandStr);
                            append(Atom::CodeQuoteArgument, getRestOfLine());
                        }
                        break;
                    case CMD_SKIPTO:
                        leavePara();
                        if (!quoting)
                            quoter.quoteTo(location(),
                                           commandStr,
                                           getRestOfLine());
                        else {
                            append(Atom::CodeQuoteCommand, commandStr);
                            append(Atom::CodeQuoteArgument, getRestOfLine());
                        }
                        break;
                    case CMD_SKIPUNTIL:
                        leavePara();
                        if (!quoting)
                            quoter.quoteUntil(location(),
                                              commandStr,
                                              getRestOfLine());
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
                            if (quoting || DoxWriter::isDoxPass()) {
                                append(Atom::SnippetCommand, commandStr);
                                append(Atom::SnippetLocation, snippet);
                                append(Atom::SnippetIdentifier, identifier);
                            } else {
                                Doc::quoteFromFile(location(),quoter,snippet);
                                appendToCode(quoter.quoteSnippet(location(),
                                                                 identifier));
                            }
                        }
                        break;
                    case CMD_SUB:
                        startFormat(ATOM_FORMATTING_SUBSCRIPT, command);
                        break;
                    case CMD_SUP:
                        startFormat(ATOM_FORMATTING_SUPERSCRIPT, command);
                        break;
                    case CMD_TABLE:
                        x = getRestOfLine();
                        if (openCommand(command)) {
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
                        insertTarget(getRestOfLine(),false);
                        break;
                    case CMD_TT:
                        startFormat(ATOM_FORMATTING_TELETYPE, command);
                        break;
                    case CMD_UNDERLINE:
                        startFormat(ATOM_FORMATTING_UNDERLINE, command);
                        break;
                    case CMD_UNICODE:
                        enterPara();
                        x = getArgument();
                        {
                            bool ok;
                            uint unicodeChar = x.toUInt(&ok, 0);
                            if (!ok ||
                                (unicodeChar == 0x0000) ||
                                (unicodeChar > 0xFFFE)) {
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
                        if (openedLists.top().style() == OpenedList::Value) {
                            x = getArgument();
                            if (!priv->enumItemList.contains(x))
                                priv->enumItemList.append(x);

                            openedLists.top().next();
                            append(Atom::ListTagLeft, ATOM_LIST_VALUE);
                            append(Atom::String, x);
                            append(Atom::ListTagRight, ATOM_LIST_VALUE);
                            append(Atom::ListItemLeft, ATOM_LIST_VALUE);

                            skipSpacesOrOneEndl();
                            if (isBlankLine())
                                append(Atom::Nop);
                        } else {
                            // ### problems
                        }
                        break;
                    case CMD_WARNING:
                        startNewPara();
                        append(Atom::FormattingLeft, ATOM_FORMATTING_BOLD);
                        append(Atom::String, "Warning:");
                        append(Atom::FormattingRight, ATOM_FORMATTING_BOLD);
                        append(Atom::String, " ");
                        break;
                    case CMD_OVERLOAD: // qdoc --> doxygen
                        priv->metaCommandSet.insert(commandStr);
                        x.clear();
                        if (!isBlankLine())
                            x = getRestOfLine();
                        if (!x.isEmpty()) {
                            append(Atom::ParaLeft);
                            append(Atom::String, "This function overloads ");
                            append(Atom::AutoLink,x);
                            append(Atom::String, ".");
                            append(Atom::ParaRight);
                        }
                        else {
                            append(Atom::ParaLeft);
                            append(Atom::String, "This is an overloaded member function, provided for convenience.");
                            append(Atom::ParaRight);
                            x = getMetaCommandArgument(commandStr);
                        }
                        priv->metaCommandMap[commandStr].append(x);
                        break;
                    case UNKNOWN_COMMAND:
                        if (metaCommandSet.contains(commandStr)) {
                            priv->metaCommandSet.insert(commandStr);
                            QString xxx = getMetaCommandArgument(commandStr);
                            priv->metaCommandMap[commandStr].append(xxx);
                        } else if (macroHash()->contains(commandStr)) {
                            const Macro &macro = macroHash()->value(commandStr);
                            int numPendingFi = 0;
                            QStringMap::ConstIterator d;
                            d = macro.otherDefs.begin();
                            while (d != macro.otherDefs.end()) {
                                append(Atom::FormatIf, d.key());
                                expandMacro(commandStr, *d, macro.numParams);
                                ++d;

                                if (d == macro.otherDefs.end()) {
                                    append(Atom::FormatEndif);
                                } else {
                                    append(Atom::FormatElse);
                                    numPendingFi++;
                                }
                            }
                            while (numPendingFi-- > 0)
                                append(Atom::FormatEndif);

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
            appendChar('{');
            braceDepth++;
            pos++;
            break;
        case '}':
            {
                braceDepth--;
                pos++;

                QMap<int, QString>::Iterator f =
                    pendingFormats.find(braceDepth);
                if (f == pendingFormats.end()) {
                    enterPara();
                    appendChar('}');
                } else {
                    append(Atom::FormattingRight, *f);
                    if (*f == ATOM_FORMATTING_INDEX) {
                        if (indexStartedPara)
                            skipAllSpaces();
                    } else if (*f == ATOM_FORMATTING_LINK) {
                        // hack for C++ to support links like \l{QString::}{count()}
                        if (currentLinkAtom &&
                            currentLinkAtom->string().endsWith("::")) {
                            QString suffix = Text::subText(currentLinkAtom,
                                          priv->text.lastAtom()).toString();
                            currentLinkAtom->appendString(suffix);
                        }
                        currentLinkAtom = 0;
                    }
                    pendingFormats.erase(f);
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

                if (paraState == OutsidePara) {
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
                        if ((ch == '\n') &&
                            (paraState == InsideSingleLinePara ||
                             isBlankLine())) {
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
        append(Atom::SectionRight, QString::number(delta));
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

QString DocParser::detailsUnknownCommand(const QSet<QString> &metaCommandSet,
                                         const QString &str)
{
    QSet<QString> commandSet = metaCommandSet;
    int i = 0;
    while (cmds[i].english != 0) {
        commandSet.insert(*cmds[i].alias);
        i++;
    }

    if (aliasMap()->contains(str))
        return tr("The command '\\%1' was renamed '\\%2' by the configuration"
                   " file. Use the new name.")
               .arg(str).arg((*aliasMap())[str]);

    QString best = nearestName(str, commandSet);
    if (best.isEmpty())
        return QString();
    return tr("Maybe you meant '\\%1'?").arg(best);
}

void DocParser::checkExpiry(const QString& date)
{
    QRegExp ymd("(\\d{4})(?:-(\\d{2})(?:-(\\d{2})))");

    if (ymd.exactMatch(date)) {
	int y = ymd.cap(1).toInt();
	int m = ymd.cap(2).toInt();
	int d = ymd.cap(3).toInt();

	if (m == 0)
	    m = 1;
	if (d == 0)
	    d = 1;
	QDate expiryDate(y, m, d);
	if (expiryDate.isValid()) {
	    int days = expiryDate.daysTo(QDate::currentDate());
	    if (days == 0) {
		location().warning(tr("Documentation expires today"));
	    } else if (days == 1) {
		location().warning(tr("Documentation expired yesterday"));
	    } else if (days >= 2) {
		location().warning(tr("Documentation expired %1 days ago")
				    .arg(days));
	    }
	} else {
	    location().warning(tr("Date '%1' invalid").arg(date));
	}
    } else {
	location().warning(tr("Date '%1' not in YYYY-MM-DD format")
			    .arg(date));
    }
}

void DocParser::insertBaseName(const QString &baseName)
{
    priv->constructExtra();
    if (currentSectioningUnit == priv->extra->sectioningUnit) {
	priv->extra->baseName = baseName;
    } else {
	Atom *atom = priv->text.firstAtom();
	Atom *sectionLeft = 0;

	int delta = currentSectioningUnit - priv->extra->sectioningUnit;

	while (atom != 0) {
	    if (atom->type() == Atom::SectionLeft &&
		 atom->string().toInt() == delta)
		sectionLeft = atom;
	    atom = atom->next();
	}
	if (sectionLeft != 0)
	    (void) new Atom(sectionLeft, Atom::BaseName, baseName);
    }
}

void DocParser::insertTarget(const QString &target, bool keyword)
{
    if (targetMap.contains(target)) {
	location().warning(tr("Duplicate target name '%1'").arg(target));
	targetMap[target].warning(tr("(The previous occurrence is here)"));
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

void DocParser::include(const QString& fileName)
{
    if (location().depth() > 16)
	location().fatal(tr("Too many nested '\\%1's")
			  .arg(commandName(CMD_INCLUDE)));

    QString userFriendlyFilePath;
    // ### use current directory?
    QString filePath = Config::findFile(location(),
                                        sourceFiles,
                                        sourceDirs,
                                        fileName,
                                        userFriendlyFilePath);
    if (filePath.isEmpty()) {
	location().warning(tr("Cannot find leaf file '%1'").arg(fileName));
    } else {
	QFile inFile(filePath);
	if (!inFile.open(QFile::ReadOnly)) {
	    location().warning(tr("Cannot open leaf file '%1'")
				.arg(userFriendlyFilePath));
	} else {
	    location().push(userFriendlyFilePath);

	    QTextStream inStream(&inFile);
	    QString includedStuff = inStream.readAll();
	    inFile.close();

	    in.insert(pos, includedStuff);
	    len = in.length();
	    openedInputs.push(pos + includedStuff.length());
	}
    }
}

void DocParser::startFormat(const QString& format, int command)
{
    enterPara();

    QMap<int, QString>::ConstIterator f = pendingFormats.begin();
    while (f != pendingFormats.end()) {
	if (*f == format) {
	    location().warning(tr("Cannot nest '\\%1' commands")
				.arg(commandName(command)));
	    return;
	}
	++f;
    }

    append(Atom::FormattingLeft, format);

    if (isLeftBraceAhead()) {
	skipSpacesOrOneEndl();
	pendingFormats.insert(braceDepth, format);
	++braceDepth;
	++pos;
    } else {
	append(Atom::String, getArgument());
	append(Atom::FormattingRight, format);
	if (format == ATOM_FORMATTING_INDEX && indexStartedPara) {
	    skipAllSpaces();
	    indexStartedPara = false;
	}
    }
}

bool DocParser::openCommand(int command)
{
    int outer = openedCommands.top();
    bool ok = true;

    if (command != CMD_LINK) {
        if (outer == CMD_LIST) {
	    ok = (command == CMD_FOOTNOTE || command == CMD_LIST);
        } else if (outer == CMD_ABSTRACT) {
	    ok = (command == CMD_LIST ||
                  command == CMD_QUOTATION ||
                  command == CMD_TABLE);
        } else if (outer == CMD_SIDEBAR) {
	    ok = (command == CMD_LIST ||
                  command == CMD_QUOTATION ||
                  command == CMD_SIDEBAR);
        } else if (outer == CMD_QUOTATION) {
	    ok = (command == CMD_LIST);
        } else if (outer == CMD_TABLE) {
	    ok = (command == CMD_LIST ||
                  command == CMD_FOOTNOTE ||
                  command == CMD_QUOTATION);
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

bool DocParser::closeCommand(int endCommand)
{
    if (endCommandFor(openedCommands.top()) == endCommand && openedCommands.size() > 1) {
	openedCommands.pop();
	return true;
    } else {
	bool contains = false;
	QStack<int> opened2 = openedCommands;
	while (opened2.size() > 1) {
	    if (endCommandFor(opened2.top()) == endCommand) {
		contains = true;
		break;
	    }
	    opened2.pop();
	}

	if (contains) {
	    while (endCommandFor(openedCommands.top()) != endCommand && openedCommands.size() > 1) {
		location().warning(tr("Missing '\\%1' before '\\%2'")
				    .arg(endCommandName(openedCommands.top()))
				    .arg(commandName(endCommand)));
		openedCommands.pop();
	    }
	} else {
	    location().warning(tr("Unexpected '\\%1'")
				.arg(commandName(endCommand)));
	}
	return false;
    }
}

void DocParser::startSection(Doc::SectioningUnit unit, int command)
{
    leavePara();

    if (currentSectioningUnit == Doc::Book) {
	if (unit > Doc::Section1)
	    location().warning(tr("Unexpected '\\%1' without '\\%2'")
				.arg(commandName(command))
				.arg(commandName(CMD_SECTION1)));
	currentSectioningUnit = (Doc::SectioningUnit) (unit - 1);
	priv->constructExtra();
	priv->extra->sectioningUnit = currentSectioningUnit;
    }

    if (unit <= priv->extra->sectioningUnit) {
	location().warning(tr("Unexpected '\\%1' in this documentation")
			    .arg(commandName(command)));
    } else if (unit - currentSectioningUnit > 1) {
	location().warning(tr("Unexpected '\\%1' at this point")
			    .arg(commandName(command)));
    } else {
	if (currentSectioningUnit >= unit)
	    endSection(unit, command);

	int delta = unit - priv->extra->sectioningUnit;
	append(Atom::SectionLeft, QString::number(delta));
        priv->constructExtra();
        priv->extra->tableOfContents.append(priv->text.lastAtom());
        priv->extra->tableOfContentsLevels.append(unit);
	enterPara(Atom::SectionHeadingLeft,
                  Atom::SectionHeadingRight,
                  QString::number(delta));
	currentSectioningUnit = unit;
    }
}

void DocParser::endSection(int unit, int endCommand)
{
    leavePara();

    if (unit < priv->extra->sectioningUnit) {
	location().warning(tr("Unexpected '\\%1' in this documentation")
			    .arg(commandName(endCommand)));
    } else if (unit > currentSectioningUnit) {
	location().warning(tr("Unexpected '\\%1' at this point")
			    .arg(commandName(endCommand)));
    } else {
	while (currentSectioningUnit >= unit) {
	    int delta = currentSectioningUnit - priv->extra->sectioningUnit;
	    append(Atom::SectionRight, QString::number(delta));
	    currentSectioningUnit =
		    (Doc::SectioningUnit) (currentSectioningUnit - 1);
	}
    }
}

void DocParser::parseAlso()
{
    leavePara();
    skipSpacesOnLine();
    while (pos < len && in[pos] != '\n') {
        QString target;
        QString str;

        if (in[pos] == '{') {
            target = getArgument();
            skipSpacesOnLine();
            if (in[pos] == '{') {
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
        also << Atom(Atom::Link, target)
             << Atom(Atom::FormattingLeft, ATOM_FORMATTING_LINK)
             << str
             << Atom(Atom::FormattingRight, ATOM_FORMATTING_LINK);
        priv->addAlso(also);

        skipSpacesOnLine();
        if (pos < len && in[pos] == ',') {
            pos++;
            skipSpacesOrOneEndl();
        } else if (in[pos] != '\n') {
            location().warning(tr("Missing comma in '\\%1'").arg(commandName(CMD_SA)));
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

void DocParser::appendToCode(const QString& markedCode)
{
    if (priv->text.lastAtom()->type() != Atom::Code)
	append(Atom::Code);
    priv->text.lastAtom()->appendString(markedCode);
}

void DocParser::startNewPara()
{
    leavePara();
    enterPara();
}

void DocParser::enterPara(Atom::Type leftType,
                          Atom::Type rightType,
                          const QString& string)
{
    if (paraState == OutsidePara) {
        if (priv->text.lastAtom()->type() != Atom::ListItemLeft)
            leaveValueList();
	append(leftType, string);
	indexStartedPara = false;
	pendingParaLeftType = leftType;
	pendingParaRightType = rightType;
	pendingParaString = string;
	if (
#if 0
	     leftType == Atom::BriefLeft ||
#endif
	     leftType == Atom::SectionHeadingLeft) {
	    paraState = InsideSingleLinePara;
	} else {
	    paraState = InsideMultiLinePara;
	}
	skipSpacesOrOneEndl();
    }
}

void DocParser::leavePara()
{
    if (paraState != OutsidePara) {
	if (!pendingFormats.isEmpty()) {
	    location().warning(tr("Missing '}'"));
	    pendingFormats.clear();
	}

	if (priv->text.lastAtom()->type() == pendingParaLeftType) {
	    priv->text.stripLastAtom();
	} else {
	    if (priv->text.lastAtom()->type() == Atom::String &&
		 priv->text.lastAtom()->string().endsWith(" ")) {
		priv->text.lastAtom()->chopString();
	    }
	    append(pendingParaRightType, pendingParaString);
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
    if (openedLists.isEmpty()) {
	openedLists.push(OpenedList(OpenedList::Value));
	append(Atom::ListLeft, ATOM_LIST_VALUE);
    } else {
        if (priv->text.lastAtom()->type() == Atom::Nop)
            priv->text.stripLastAtom();
	append(Atom::ListItemRight, ATOM_LIST_VALUE);
    }
}

void DocParser::leaveValueList()
{
    leavePara();
    if (!openedLists.isEmpty() &&
        (openedLists.top().style() == OpenedList::Value)) {
        if (priv->text.lastAtom()->type() == Atom::Nop)
            priv->text.stripLastAtom();
	append(Atom::ListItemRight, ATOM_LIST_VALUE);
	append(Atom::ListRight, ATOM_LIST_VALUE);
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

void DocParser::expandMacro(const QString &name,
                            const QString &def,
                            int numParams)
{
    if (numParams == 0) {
	append(Atom::RawString, def);
    } else {
	QStringList args;
	QString rawString;

	for (int i = 0; i < numParams; i++) {
	    if (numParams == 1 || isLeftBraceAhead()) {
		args << getArgument(true);
	    } else {
		location().warning(tr("Macro '\\%1' invoked with too few"
				       " arguments (expected %2, got %3)")
				    .arg(name).arg(numParams).arg(i));
		break;
	    }
	}

	int j = 0;
	while (j < def.size()) {
	    int paramNo;
	    if ((def[j] == '\\') && (j < def.size() - 1) &&
                ((paramNo = def[j + 1].digitValue()) >= 1) &&
                (paramNo <= numParams)) {
		if (!rawString.isEmpty()) {
		    append(Atom::RawString, rawString);
		    rawString = "";
		}
		append(Atom::String, args[paramNo - 1]);
		j += 2;
	    } else {
		rawString += def[j++];
	    }
	}
	if (!rawString.isEmpty())
	    append(Atom::RawString, rawString);
    }
}

Doc::SectioningUnit DocParser::getSectioningUnit()
{
    QString name = getOptionalArgument();

    if (name == "part") {
	return Doc::Part;
    } else if (name == "chapter") {
	return Doc::Chapter;
    } else if (name == "section1") {
	return Doc::Section1;
    } else if (name == "section2") {
	return Doc::Section2;
    } else if (name == "section3") {
	return Doc::Section3;
    } else if (name == "section4") {
	return Doc::Section4;
    } else if (name.isEmpty()) {
	return Doc::Section4;
    } else {
	location().warning(tr("Invalid sectioning unit '%1'").arg(name));
	return Doc::Book;
    }
}

QString DocParser::getArgument(bool verbatim)
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
    if (pos < (int) in.length() && in[pos] == '{') {
	pos++;
	while (pos < (int) in.length() && delimDepth >= 0) {
	    switch (in[pos].unicode()) {
	    case '{':
		delimDepth++;
		arg += "{";
		pos++;
		break;
	    case '}':
		delimDepth--;
		if (delimDepth >= 0)
		    arg += "}";
		pos++;
		break;
	    case '\\':
		if (verbatim) {
		    arg += in[pos];
		    pos++;
		} else {
		    pos++;
		    if (pos < (int) in.length()) {
			if (in[pos].isLetterOrNumber())
			    break;
			arg += in[pos];
			if (in[pos].isSpace()) {
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
	if (delimDepth > 0)
	    location().warning(tr("Missing '}'"));
    } else {
	while (pos < in.length() &&
               ((delimDepth > 0) ||
                ((delimDepth == 0) &&
                 !in[pos].isSpace()))) {
	    switch (in[pos].unicode()) {
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
		if (pos == startPos || delimDepth >= 0) {
		    arg += in[pos];
		    pos++;
		}
		break;
	    case '\\':
		if (verbatim) {
		    arg += in[pos];
		    pos++;
		} else {
		    pos++;
		    if (pos < (int) in.length()) {
			if (in[pos].isLetterOrNumber())
			    break;
			arg += in[pos];
			if (in[pos].isSpace()) {
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
	if ((arg.length() > 1) &&
            (QString(".,:;!?").indexOf(in[pos - 1]) != -1) &&
            !arg.endsWith("...")) {
	    arg.truncate(arg.length() - 1);
	    pos--;
	}
	if (arg.length() > 2 && in.mid(pos - 2, 2) == "'s") {
	    arg.truncate(arg.length() - 2);
	    pos -= 2;
	}
    }
    return arg.simplified();
}

QString DocParser::getOptionalArgument()
{
    skipSpacesOrOneEndl();
    if (pos + 1 < (int) in.length() && in[pos] == '\\' &&
	 in[pos + 1].isLetterOrNumber()) {
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
                while ((pos < in.size()) &&
                       in[pos].isSpace() &&
                       (in[pos] != '\n'))
                    ++pos;
            } else {
                trailingSlash = false;
                ++pos;
            }
        }

        if (!t.isEmpty())
            t += " ";
        t += in.mid(begin, pos - begin).simplified();

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

    while (pos < in.size() && (in[pos] != '\n' || parenDepth > 0)) {
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

    QString t = in.mid(begin, pos - begin).simplified();
    skipSpacesOnLine();
    return t;
}

QString DocParser::getUntilEnd(int command)
{
    int endCommand = endCommandFor(command);
    QRegExp rx("\\\\" + commandName(endCommand) + "\\b");
    QString t;
    int end = rx.indexIn(in, pos);

    if (end == -1) {
	location().warning(tr("Missing '\\%1'").arg(commandName(endCommand)));
	pos = in.length();
    } else {
	t = in.mid(pos, end - pos);
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

/*!
  Used only for generating doxygen output.
 */
QString DocParser::getUnmarkedCode(int command)
{
    QString code = getUntilEnd(command);
#if 0    
    int indent = indentLevel(code);
    if (indent < minIndent)
        minIndent = indent;
    code = unindent(minIndent, code);
#endif    
    return code;
}

bool DocParser::isBlankLine()
{
    int i = pos;

    while (i < len && in[i].isSpace()) {
	if (in[i] == '\n')
	    return true;
	i++;
    }
    return false;
}

bool DocParser::isLeftBraceAhead()
{
    int numEndl = 0;
    int i = pos;

    while (i < len && in[i].isSpace() && numEndl < 2) {
	// ### bug with '\\'
	if (in[i] == '\n')
	    numEndl++;
    	i++;
    }
    return numEndl < 2 && i < len && in[i] == '{';
}

void DocParser::skipSpacesOnLine()
{
    while ((pos < in.length()) &&
           in[pos].isSpace() &&
           (in[pos].unicode() != '\n'))
	++pos;
}

void DocParser::skipSpacesOrOneEndl()
{
    int firstEndl = -1;
    while (pos < (int) in.length() && in[pos].isSpace()) {
	QChar ch = in[pos];
	if (ch == '\n') {
	    if (firstEndl == -1) {
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
    while (pos < len && in[pos].isSpace())
	pos++;
}

void DocParser::skipToNextPreprocessorCommand()
{
    QRegExp rx("\\\\(?:" + commandName(CMD_IF) + "|" +
                           commandName(CMD_ELSE) + "|" +
                           commandName(CMD_ENDIF) + ")\\b");
    int end = rx.indexIn(in, pos + 1); // ### + 1 necessary?

    if (end == -1)
	pos = in.length();
    else
	pos = end;
}

int DocParser::endCommandFor(int command)
{
    switch (command) {
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

QString DocParser::commandName(int command)
{
    return *cmds[command].alias;
}

QString DocParser::endCommandName(int command)
{
    return commandName(endCommandFor(command));
}

QString DocParser::untabifyEtc(const QString& str)
{
    QString result;
    result.reserve(str.length());
    int column = 0;

    for (int i = 0; i < str.length(); i++) {
        const QChar c = str.at(i);
        if (c == QLatin1Char('\r'))
            continue;
        if (c == QLatin1Char('\t')) {
            result += "        " + (column % tabSize);
            column = ((column / tabSize) + 1) * tabSize;
            continue;
        }
        if (c == QLatin1Char('\n')) {
            while (result.endsWith(QLatin1Char(' ')))
                result.chop(1);
            result += c;
            column = 0;
            continue;
        }
        result += c;
        column++;
    }

    while (result.endsWith("\n\n"))
        result.truncate(result.length() - 1);
    while (result.startsWith("\n"))
        result = result.mid(1);
   
    return result;
}

int DocParser::indentLevel(const QString& str)
{
    int minIndent = INT_MAX;
    int column = 0;

    for (int i = 0; i < (int) str.length(); i++) {
	if (str[i] == '\n') {
	    column = 0;
	} else {
	    if (str[i] != ' ' && column < minIndent)
		minIndent = column;
	    column++;
	}
    }
    return minIndent;
}

QString DocParser::unindent(int level, const QString& str)
{
    if (level == 0)
	return str;

    QString t;
    int column = 0;

    for (int i = 0; i < (int) str.length(); i++) {
        if (str[i] == QLatin1Char('\n')) {
	    t += '\n';
	    column = 0;
	} else {
	    if (column >= level)
		t += str[i];
	    column++;
	}
    }
    return t;
}

QString DocParser::slashed(const QString& str)
{
    QString result = str;
    result.replace("/", "\\/");
    return "/" + result + "/";
}

#define COMMAND_BRIEF                   Doc::alias("brief")

#define DOXYGEN_INDENT 2
#define DOXYGEN_TAB_SIZE 4
#define DOXYGEN_INDENT_STRING "  "
#define DOXYGEN_TAB_STRING "    "

static QRegExp ws_rx("\\s");
static QRegExp not_ws_rx("\\S");

int DoxWriter::doxPass = 0;
QString DoxWriter::currentClass;
QSet<QString> DoxWriter::anchors;
QStringMap DoxWriter::exampleTitles;
QStringMap DoxWriter::headerFileTitles;
QStringMap DoxWriter::fileTitles;
QStringMap DoxWriter::groupTitles;
QStringMap DoxWriter::moduleTitles;
QStringMap DoxWriter::pageTitles;
QStringMap DoxWriter::externalPageTitles;
QStringMap DoxWriter::exampleTitlesInverse;
QStringMap DoxWriter::headerFileTitlesInverse;
QStringMap DoxWriter::fileTitlesInverse;
QStringMap DoxWriter::groupTitlesInverse;
QStringMap DoxWriter::moduleTitlesInverse;
QStringMap DoxWriter::pageTitlesInverse;
QStringMap DoxWriter::externalPageTitlesInverse;
QStringMultiMap DoxWriter::variables;
QStringMultiMap DoxWriter::properties;
QStringMultiMap DoxWriter::enums;

Doc::Doc(const Location& start_loc,
         const Location& end_loc,
         const QString& source,
         const QSet<QString>& metaCommandSet)
{
    priv = new DocPrivate(start_loc,end_loc,source);
    DocParser parser;
    parser.parse(source,priv,metaCommandSet);
    if (DoxWriter::isDoxPass()) {
        DoxWriter doxWriter(source,priv);
        if (DoxWriter::isDoxPass(1))
            doxWriter.pass1();
        else
            doxWriter.pass2();
    }
}

Doc::Doc(const Doc& doc)
    : priv(0)
{
    operator=(doc);
}

Doc::~Doc()
{
    if (priv && priv->deref())
	delete priv;
}

Doc &Doc::operator=(const Doc& doc)
{
    if (doc.priv)
	doc.priv->ref();
    if (priv && priv->deref())
	delete priv;
    priv = doc.priv;
    return *this;
}

void Doc::renameParameters(const QStringList &oldNames,
                           const QStringList &newNames)
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
                if ((atom->type() == Atom::ListLeft) &&
                    (atom->string() == ATOM_LIST_VALUE)) {
                    while (atom && ((atom->type() != Atom::ListRight) ||
                                    (atom->string() != ATOM_LIST_VALUE)))
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

/*!
  Returns the starting location of a qdoc comment.
 */
const Location &Doc::location() const
{
    static const Location dummy;
    return priv == 0 ? dummy : priv->start_loc;
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
    return body().subText(Atom::BriefLeft, Atom::BriefRight);
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
                .arg(COMMAND_BRIEF).arg(className));
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
                .arg(COMMAND_BRIEF).arg(className));
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
    if (priv == 0 || priv->extra == 0) {
	return null;
    } else {
	return priv->extra->baseName;
    }
}

Doc::SectioningUnit Doc::granularity() const
{
    if (priv == 0 || priv->extra == 0) {
	return DocPrivateExtra().granularity;
    } else {
	return priv->extra->granularity;
    }
}

#if notyet // ###
Doc::SectioningUnit Doc::sectioningUnit() const
{
    if (priv == 0 || priv->extra == 0) {
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

QStringList Doc::metaCommandArgs(const QString& metaCommand) const
{
    if (priv == 0) {
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

const QStringMap &Doc::metaTagMap() const
{
    return priv && priv->extra ? priv->extra->metaMap : *null_QStringMap();
}

void Doc::initialize(const Config& config)
{
    DocParser::tabSize = config.getInt(CONFIG_TABSIZE);
    DocParser::exampleFiles = config.getStringList(CONFIG_EXAMPLES);
    DocParser::exampleDirs = config.getStringList(CONFIG_EXAMPLEDIRS);
    DocParser::sourceFiles = config.getStringList(CONFIG_SOURCES);
    DocParser::sourceDirs = config.getStringList(CONFIG_SOURCEDIRS);
    DocParser::quoting = config.getBool(CONFIG_QUOTINGINFORMATION);

    QStringMap reverseAliasMap;

    QSet<QString> commands = config.subVars(CONFIG_ALIAS);
    QSet<QString>::ConstIterator c = commands.begin();
    while (c != commands.end()) {
	QString alias = config.getString(CONFIG_ALIAS + Config::dot + *c);
	if (reverseAliasMap.contains(alias)) {
	    config.lastLocation().warning(tr("Command name '\\%1' cannot stand"
					      " for both '\\%2' and '\\%3'")
					   .arg(alias)
					   .arg(reverseAliasMap[alias])
					   .arg(*c));
	} else {
	    reverseAliasMap.insert(alias, *c);
	}
	aliasMap()->insert(*c, alias);
	++c;
    }

    int i = 0;
    while (cmds[i].english) {
	cmds[i].alias = new QString(alias(cmds[i].english));
	commandHash()->insert(*cmds[i].alias, cmds[i].no);

	if (cmds[i].no != i)
	    Location::internalError(tr("command %1 missing").arg(i));
	i++;
    }

    QSet<QString> macroNames = config.subVars(CONFIG_MACRO);
    QSet<QString>::ConstIterator n = macroNames.begin();
    while (n != macroNames.end()) {
	QString macroDotName = CONFIG_MACRO + Config::dot + *n;
	Macro macro;
	macro.numParams = -1;
	macro.defaultDef = config.getString(macroDotName);
	if (!macro.defaultDef.isEmpty()) {
	    macro.defaultDefLocation = config.lastLocation();
	    macro.numParams = Config::numParams(macro.defaultDef);
	}
	bool silent = false;

	QSet<QString> formats = config.subVars(macroDotName);
	QSet<QString>::ConstIterator f = formats.begin();
	while (f != formats.end()) {
	    QString def = config.getString(macroDotName + Config::dot + *f);
	    if (!def.isEmpty()) {
		macro.otherDefs.insert(*f, def);
		int m = Config::numParams(macro.defaultDef);
		if (macro.numParams == -1) {
		    macro.numParams = m;
		} else if (macro.numParams != m) {
		    if (!silent) {
			QString other = tr("default");
			if (macro.defaultDef.isEmpty())
			    other = macro.otherDefs.begin().key();
			config.lastLocation().warning(tr("Macro '\\%1' takes"
							  " inconsistent number"
							  " of arguments (%2"
							  " %3, %4 %5)")
						       .arg(*n)
						       .arg(*f)
						       .arg(m)
						       .arg(other)
						       .arg(macro.numParams));
			silent = true;
		    }
		    if (macro.numParams < m)
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
    while (cmds[i].english) {
        delete cmds[i].alias;
        cmds[i].alias = 0;
        ++i;
    }
}

QString Doc::alias(const QString &english)
{
    return aliasMap()->value(english, english);
}

void Doc::trimCStyleComment(Location& location, QString& str)
{
    QString cleaned;
    Location m = location;
    bool metAsterColumn = true;
    int asterColumn = location.columnNo() + 1;
    int i;

    for (i = 0; i < (int) str.length(); i++) {
	if (m.columnNo() == asterColumn) {
	    if (str[i] != '*')
		break;
	    cleaned += ' ';
	    metAsterColumn = true;
	} else {
	    if (str[i] == '\n') {
		if (!metAsterColumn)
		    break;
		metAsterColumn = false;
	    }
	    cleaned += str[i];
	}
	m.advance(str[i]);
    }
    if (cleaned.length() == str.length())
	str = cleaned;

    for (int i = 0; i < 3; i++)
	location.advance(str[i]);
    str = str.mid(3, str.length() - 5);
}

CodeMarker *Doc::quoteFromFile(const Location &location,
                               Quoter &quoter,
                               const QString &fileName)
{
    quoter.reset();

    QString code;

    QString userFriendlyFilePath;
    QString filePath = Config::findFile(location,
                                        DocParser::exampleFiles,
                                        DocParser::exampleDirs,
					fileName, userFriendlyFilePath);
    if (filePath.isEmpty()) {
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
    quoter.quoteFromFile(userFriendlyFilePath,
                         code,
                         marker->markedUpCode(code, 0, dirPath));
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

/*!
  Sets the doxygen writer pass to \a pass. You can use
  isDoxPass(), with or without a parameter, to test if
  you are in a doxygen writer run or in a specific pass
  of a doxygen writer run.

  This function is only called from main() if either the
  \e doxygen1 or \e doxygen2 flag is passed to qdoc3 on
  the command line.
 */
void DoxWriter::setDoxPass(int pass)
{
    qDebug() << "SETTING doxygen pass to " << pass
             << " in DoxWriter::setDoxPass()";
    doxPass = pass;
}

/*!
  Returns true if the doxygen pass is set to \a pass, 
  which means we are in the specified \a pass of a doxygen
  writer run of qdoc3.
 */
bool DoxWriter::isDoxPass(int pass) { return (doxPass == pass); }

/*!
  Returns true if the doxygen pass is 1 or 2, which 
  means this is a doxygen writer run to transform qdoc
  comments into doxygen comments.
 */
bool DoxWriter::isDoxPass() { return (doxPass > 0); }

bool DoxWriter::conversionRequired() const
{
    /*
      Loop through all the topic commands searching for
      one that must be transformed to doxygen format. If
      one is found, return true.
     */
    QCommandMap::const_iterator i;
    i = priv->metaCommandMap.constBegin();
    while (i != priv->metaCommandMap.constEnd()) {
        QString s = i.key();
        if (s == "enum")
            return true;
        else if (s == "example")
            return true;
        else if (s == "externalpage")
            return true;
        else if (s == "group")
            return true;
        else if (s == "headerfile")
            return true;
        else if (s == "module")
            return true;
        else if (s == "page")
            return true;
        else if (s == "property")
            return true;
        else if (s == "typedef")
            return true;
        else if (s == "variable")
            return true;
        else if (s == "overload")
            return true;
        else if (s == "reimp")
            return true;
        else if (s == "relates")
            return true;
        else if (s == "macro")
            return true;
        else {
#if 0            
            if (s == "class") 
            else if (s == "namespace")
            else if (s == "service")
            else if (s == "inheaderfile") 
            else if (s == "file") 
            else if (s == "fn") 
            else if (s == "contentspage") 
            else if (s == "nextpage") 
            else if (s == "previous") 
            else if (s == "indexpage") 
            else if (s == "startpage") 
#endif            
        }
        ++i;
    }
    
    /*
      Loop through all the qdoc atoms searching for one
      that must be transformed to doxygen format. If one
      is found, return true.
     */
    const Atom* next = priv->text.firstAtom();
    while (next != 0) {
        Atom::Type atomType = next->type();
        switch (atomType) {
            case Atom::C:
            case Atom::CaptionLeft: 
            case Atom::Code: 
            case Atom::CodeBad: 
            case Atom::CodeNew: 
            case Atom::CodeOld: 
            case Atom::CodeQuoteArgument:
            case Atom::CodeQuoteCommand:
            case Atom::FootnoteLeft:
            case Atom::FormatElse: 
            case Atom::FormatEndif:
            case Atom::FormatIf:
            case Atom::GeneratedList:
            case Atom::Image: 
            case Atom::ImageText:
            case Atom::InlineImage:
            case Atom::LegaleseLeft:
            case Atom::LineBreak: 
            case Atom::Link: 
            case Atom::LinkNode:
            case Atom::ListLeft: 
            case Atom::ListItemNumber:
            case Atom::ListTagLeft:
            case Atom::ListItemLeft:
            case Atom::QuotationLeft: 
            case Atom::RawString:
            case Atom::SectionLeft:
            case Atom::SectionHeadingLeft:
            case Atom::SidebarLeft:
            case Atom::SnippetCommand:
            case Atom::SnippetIdentifier:
            case Atom::SnippetLocation:
            case Atom::TableLeft:
            case Atom::TableHeaderLeft:
            case Atom::TableRowLeft:
            case Atom::TableItemLeft: 
            case Atom::TableOfContents:
            case Atom::Target:
                return true;
            case Atom::AbstractLeft: 
            case Atom::AbstractRight: 
            case Atom::AutoLink:
            case Atom::BaseName: 
            case Atom::BriefLeft:
            case Atom::BriefRight: 
            case Atom::CaptionRight:
            case Atom::FormattingLeft:
            case Atom::FormattingRight:
            case Atom::Nop: 
            case Atom::ParaLeft:
            case Atom::ParaRight: 
            case Atom::FootnoteRight:
            case Atom::LegaleseRight:
            case Atom::ListTagRight:
            case Atom::ListItemRight: 
            case Atom::ListRight: 
            case Atom::QuotationRight:
            case Atom::SectionRight:
            case Atom::SectionHeadingRight:
            case Atom::SidebarRight:
            case Atom::String:
            case Atom::TableRight: 
            case Atom::TableHeaderRight:
            case Atom::TableRowRight: 
            case Atom::TableItemRight:
            default:
                break;
        }
        next = next->next();
    }
    return false;
}

/*!
  A convenience function to write a qdoc metacommand as a
  doxygen command, without conversion. i.e., some of the
  qdoc metacommands don't require conversion for doxygen.
 */
void DoxWriter::writeCommand(QCommandMap::const_iterator cmd)
{
    concatenate("\\" + cmd.key() + " " + cmd.value()[0]);
    newLine();
}

/*!
  Convert the qdoc commands in the metacommand map to
  doxygen format. This function is called only in pass2().
  The metacommand map contains all the metacommands that
  were found in the qdoc comment that is being converted.
  The metacommands are the ones that begin with the '\'.
  These are not considered part of the text of the comment.
  The text is converted by convertText().
 */
void DoxWriter::convertMetaCommands()
{
    QCommandMap& metaCmdMap = priv->metaCommandMap;
    QCommandMap::iterator cmd;
    int c;

    currentPage.clear();
    currentFn.clear();
    currentTitle.clear();
    currentEnum.clear();
    currentProperty.clear();
    currentVariable.clear();
    currentClass.clear();
    currentExample.clear();
    currentGroup.clear();
    currentModule.clear();
    currentMacro.clear();
    currentService.clear();
    currentTypedef.clear();
    currentHeaderFile.clear();
    commentType = OtherComment;

    if ((cmd = metaCmdMap.find("class")) != metaCmdMap.end()) {
        currentClass = cmd.value()[0];
        if ((c = currentClass.indexOf(' ')) > 0)
            currentClass = currentClass.left(c);
        writeCommand(cmd);
        metaCmdMap.erase(cmd);
        commentType = ClassComment;
    }
    else if ((cmd = metaCmdMap.find("fn")) != metaCmdMap.end()) {
        currentFn = cmd.value()[0];
        writeCommand(cmd);
        metaCmdMap.erase(cmd);
        commentType = FnComment;
    }
    else if ((cmd = metaCmdMap.find("enum")) != metaCmdMap.end()) {
        currentEnum = cmd.value()[0];
        if ((c = currentEnum.lastIndexOf("::")) > 0) {
            currentClass = currentEnum.left(c);
            currentEnum = currentEnum.right(currentEnum.size()-c-2);
            qDebug() << "currentEnum =" << currentEnum;
            qDebug() << "currentClass =" << currentClass;
        }
        writeCommand(cmd);
        metaCmdMap.erase(cmd);
        commentType = EnumComment;
    }
    else if ((cmd = metaCmdMap.find("property")) != metaCmdMap.end()) {
        currentClass = cmd.value()[0];
        if ((c = currentClass.lastIndexOf("::")) > 0) {
            currentProperty = currentClass.right(currentClass.size()-c-2);
            currentClass = currentClass.left(c);
            qDebug() << "currentProperty =" << currentProperty;
            qDebug() << "currentClass =" << currentClass;
        }
        writeCommand(cmd);
        metaCmdMap.erase(cmd);
        commentType = PropertyComment;
    }
    else if ((cmd = metaCmdMap.find("variable")) != metaCmdMap.end()) {
        currentClass = cmd.value()[0];
        if ((c = currentClass.lastIndexOf("::")) > 0) {
            currentVariable = currentClass.right(currentClass.size()-c-2);
            currentClass = currentClass.left(c);
            qDebug() << "currentVariable =" << currentVariable;
            qDebug() << "currentClass =" << currentClass;
        }
        concatenate("\\var " + cmd.value()[0]);
        newLine();
        metaCmdMap.erase(cmd);
        commentType = VariableComment;
    }

    if ((cmd = metaCmdMap.find("page")) != metaCmdMap.end()) {
        currentPage = cmd.value()[0];
        QString htmlFile = currentPage;
        const QString* title = getPageTitle(htmlFile);
        QStringList parts = htmlFile.split('.');
        metaCmdMap.erase(cmd);
        if (title) {
            concatenate("\\page " + parts[0] + " " + *title);
            newLine();
        }
        commentType = PageComment;
        qDebug() << "currentPage =" << currentPage;
    }

    if ((cmd = metaCmdMap.find("example")) != metaCmdMap.end()) {
        currentExample = cmd.value()[0];
        metaCmdMap.erase(cmd);
        commentType = ExampleComment;
        qDebug() << "currentExample =" << currentExample;
    }
    
    if ((cmd = metaCmdMap.find("macro")) != metaCmdMap.end()) {
        currentMacro = cmd.value()[0];
        metaCmdMap.erase(cmd);
        commentType = MacroComment;
        qDebug() << "currentMacro =" << currentMacro;
    }
    
    if ((cmd = metaCmdMap.find("group")) != metaCmdMap.end()) {
        currentGroup = cmd.value()[0];
        metaCmdMap.erase(cmd);
        commentType = GroupComment;
        qDebug() << "currentGroup =" << currentGroup;
    }
    
    if ((cmd = metaCmdMap.find("module")) != metaCmdMap.end()) {
        currentModule = cmd.value()[0];
        metaCmdMap.erase(cmd);
        commentType = ModuleComment;
        qDebug() << "currentModule =" << currentModule;
    }
    
    if ((cmd = metaCmdMap.find("headerfile")) != metaCmdMap.end()) {
        currentHeaderFile = cmd.value()[0];
        metaCmdMap.erase(cmd);
        commentType = HeaderFileComment;
        qDebug() << "currentHeaderFile =" << currentHeaderFile;
    }

    if ((cmd = metaCmdMap.find("typedef")) != metaCmdMap.end()) {
        currentClass = cmd.value()[0];
        if ((c = currentClass.lastIndexOf("::")) > 0) {
            currentTypedef = currentClass.right(currentClass.size()-c-2);
            currentClass = currentClass.left(c);
        }
        metaCmdMap.erase(cmd);
        commentType = TypedefComment;
        qDebug() << "currentTypedef =" << currentTypedef;
        qDebug() << "currentClass =" << currentClass;
    }
    
    cmd = priv->metaCommandMap.begin();
    while (cmd != priv->metaCommandMap.end()) {
        for (int i=0; i<cmd.value().size(); i++) {
            concatenate("\\" + cmd.key() + " " + cmd.value()[i]);
            newLine();
        }
        //qDebug() << "   " << cmd.key() << ": " << cmd.value();
        ++cmd;
    }
}

/*!
  Convert the qdoc text to doxygen format. The metacommands
  are converted by convertMetaCommands(). This function is
  called in pass2().
 */
void DoxWriter::convertText()
{
    const Atom* prev = 0;
    const Atom* next = priv->text.firstAtom();
    while (next != 0) {
        next->dump();
        Atom::Type atomType = next->type();
        switch (atomType) {
            case Atom::AbstractLeft:
                break;
            case Atom::AbstractRight:
                break;
            case Atom::AutoLink:
                concatenate(next->string());
                break;
            case Atom::BaseName:
                break;
            case Atom::BriefLeft:
                concatenate("\\brief ");
                break;
            case Atom::BriefRight:
                newLine();
                break;
            case Atom::C:
                tt(next);
                break;
            case Atom::CaptionLeft:
                unhandled(next);
                break;
            case Atom::CaptionRight:
                unhandled(next);
                break;
            case Atom::Code:
                code(next);
                break;
            case Atom::CodeBad:
                code(next);
                break;
            case Atom::CodeNew:
                newLine();
                concatenate("you can rewrite it as");
                code(next);
                break;
            case Atom::CodeOld:
                newLine();
                concatenate("For example, if you have code like");
                code(next);
                break;
            case Atom::CodeQuoteArgument:
                unhandled(next);
                break;
            case Atom::CodeQuoteCommand:
                next = codeQuoteCommand(next);
                break;
            case Atom::FootnoteLeft:
                break;
            case Atom::FootnoteRight:
                break;
            case Atom::FormatElse:
                formatElse();
                break;
            case Atom::FormatEndif:
                formatEndif();
                break;
            case Atom::FormatIf:
                formatIf(next);
                break;
            case Atom::FormattingLeft:
                formattingLeft(next,next->next());
                break;
            case Atom::FormattingRight:
                formattingRight(next,prev);
                break;
            case Atom::GeneratedList:
                break;
            case Atom::Image:
                break;
            case Atom::ImageText:
                break;
            case Atom::InlineImage:
                break;
            case Atom::LegaleseLeft:
                break;
            case Atom::LegaleseRight:
                break;
            case Atom::LineBreak:
                break;
            case Atom::Link:
                next = link(next);
                break;
            case Atom::LinkNode:
                break;
            case Atom::ListLeft:
                {
                    bool nested = false;
                    if (structs.isEmpty()) {
                        const Atom* i = next->next();
                        while (i->type() != Atom::ListRight) {
                            if ((i->type() == Atom::ListLeft) ||
                                (i->type() == Atom::TableLeft)) {
                                nested = true;
                                break;
                            }
                            i = i->next();
                        }
                    }
                    else
                        nested = true;
                    StructDesc d(BulletList,nested);
                    if (next->string() == "numeric")
                        d.structType = NumericList;
                    else if (next->string() == "value") {
                        d.structType = ValueList;
                    }
                    else if (next->string() != "bullet")
                        qDebug() << "UNKNOWN LIST TYPE" << next->string();
                    structs.push(d);
                    if (nested || (d.structType != BulletList)) {
                        if (d.structType == BulletList)
                            concatenate("<ul>");
                        else if (d.structType == NumericList)
                            concatenate("<ol>");
                        else if (d.structType == ValueList)
                            concatenate("<dl>");
                        newLine();
                    }
                }
                break;
            case Atom::ListItemNumber:
                structs.top().count = next->string().toInt();
                break;
            case Atom::ListTagLeft:
                {
                    structs.top().count++;
                    concatenate("<dt>");
                    const Atom* n = next->next();
                    if (n->type() == Atom::String) {
                        qDebug() << "ENUM VALUE" << n->string();
                    }
                    else 
                        qDebug() << "NOT EN ENUM";
                }
                break;
            case Atom::ListTagRight:
                concatenate("</dt>");
                break;
            case Atom::ListItemLeft:
                {
                    newLine();
                    const StructDesc& d = structs.top();
                    if (d.structType == BulletList) {
                        if (!d.nested) 
                            concatenate("\\arg ");
                        else
                            concatenate("<li>");
                    }
                    else if (d.structType == NumericList)
                        concatenate("<li>");
                    else if (d.structType == ValueList)
                        concatenate("<dd>");
                }
                break;
            case Atom::ListItemRight:
                {
                    const StructDesc& d = structs.top();
                    if (d.structType == BulletList) {
                        if (d.nested) {
                            concatenate("</li>");
                            newLine();
                        }
                    }
                    else if (d.structType == NumericList) {
                        concatenate("</li>");
                        newLine();
                    }
                    else if (d.structType == ValueList) {
                        concatenate("</dd>");
                        newLine();
                    }
                }
                break;
            case Atom::ListRight:
                {
                    if (!structs.isEmpty()) {
                        const StructDesc& d = structs.top();
                        if (d.nested || (d.structType != BulletList)) {
                            if (d.structType == BulletList)
                                concatenate("</ul>");
                            else if (d.structType == NumericList)
                                concatenate("</ol>");
                            else if (d.structType == ValueList)
                                concatenate("</dl>");
                            newLine();
                        }
                        structs.pop();
                    }
                }
                break;
            case Atom::Nop:
                // nothing.
                break;
            case Atom::ParaLeft:
                if (structs.isEmpty())
                    newLine();
                break;
            case Atom::ParaRight:
                {
                    if (structs.isEmpty())
                        newLine();
                    else {
                        const StructDesc& d = structs.top();
                        if (d.nested || (d.structType != BulletList)) {
                            Atom::Type t = next->next()->type(); 
                            if ((t != Atom::ListItemRight) &&
                                (t != Atom::TableItemRight))
                                newLine();
                        }
                        else
                            newLine();
                    }
                }
                break;
            case Atom::QuotationLeft:
                break;
            case Atom::QuotationRight:
                break;
            case Atom::RawString:
                concatenate(next->string());
                break;
            case Atom::SectionLeft:
                // nothing.
                break;
            case Atom::SectionRight:
                // nothing.
                break;
            case Atom::SectionHeadingLeft:
                next = sectionHeading(next);
                break;
            case Atom::SectionHeadingRight:
                newLine();
                break;
            case Atom::SidebarLeft:
                break;
            case Atom::SidebarRight:
                break;
            case Atom::SnippetCommand:
                newLine();
                concatenate("\\snippet ");
                break;
            case Atom::SnippetIdentifier:
                newText += next->string();
                lineLength += next->string().size();
                newLine();
                break;
            case Atom::SnippetLocation:
                newText += next->string() + " ";
                lineLength += next->string().size() + 1;
                break;
            case Atom::String:
                wrap(next->string());
                break;
            case Atom::TableLeft:
                {
                    bool nested = false;
                    if (structs.isEmpty()) {
                        const Atom* i = next->next();
                        while (i->type() != Atom::TableRight) {
                            if ((i->type() == Atom::ListLeft) ||
                                (i->type() == Atom::TableLeft)) {
                                nested = true;
                                break;
                            }
                            i = i->next();
                        }
                    }
                    else
                        nested = true;
                    StructDesc d(Table,nested);
                    structs.push(d);
                    if (next->string().isEmpty())
                        concatenate("<table>");
                    else {
                        QString attrs = "width=\"" + next->string() + "\"";
                        attrs += " align=\"center\""; 
                        concatenate("<table " + attrs + ">");
                    }
                    newLine();
                }
                break;
            case Atom::TableRight:
                concatenate("</table>");
                if (!structs.isEmpty())
                    structs.pop();
                newLine();
                break;
            case Atom::TableHeaderLeft:
                concatenate("<tr>");
                if (!structs.isEmpty())
                    structs.top().inTableHeader = true;
                newLine();
                break;
            case Atom::TableHeaderRight:
                concatenate("</tr>");
                if (!structs.isEmpty())
                    structs.top().inTableHeader = false;
                newLine();
                break;
            case Atom::TableRowLeft:
                if (!structs.isEmpty()) {
                    structs.top().inTableRow = true;
                    concatenate("<tr valign=\"top\" class=\"");
                    if (structs.top().odd)
                        concatenate("odd\">");
                    else
                        concatenate("even\">");
                    structs.top().odd = !structs.top().odd;
                }
                newLine();
                break;
            case Atom::TableRowRight:
                concatenate("</tr>");
                if (!structs.isEmpty())
                    structs.top().inTableRow = false;
                newLine();
                break;
            case Atom::TableItemLeft:
                if (!structs.isEmpty()) {
                    structs.top().inTableItem = true;
                    concatenate("<td>");
                    if (structs.top().inTableHeader)
                        concatenate("<b> ");
                }
                break;
            case Atom::TableItemRight:
                if (!structs.isEmpty()) {
                    structs.top().inTableItem = false;
                    if (structs.top().inTableHeader)
                        concatenate(" </b>");
                    concatenate("</td>");
                }
                newLine();
                break;
            case Atom::TableOfContents:
                break;
            case Atom::Target:
                {
                    QString text = next->string();
                    text.remove(ws_rx);
                    newLine();
                    concatenate("\\anchor ");
                    newText += text;
                    lineLength += text.size();
                    newLine();
                }
                break;
            case Atom::UnhandledFormat:
                unhandled(next);
                break;
            case Atom::UnknownCommand:
                unhandled(next);
                break;
            default:
                //next->dump();
                break;
        }
        prev = next;
        next = next->next();
    }
}

/*!
  
  Pass one looks for topic commands and target and section
  commands, and maybe other stuff. These are serialized to
  text files, which are read back in by pass2().
 */
void DoxWriter::pass1()
{
    QCommandMap& metaCmdMap = priv->metaCommandMap;
    if (!metaCmdMap.isEmpty()) {
        int c;
        QCommandMap::iterator cmd;
        if ((cmd = metaCmdMap.find("enum")) != metaCmdMap.end()) {
            commentType = EnumComment;
            currentEnum = cmd.value()[0];
            if ((c = currentEnum.lastIndexOf("::")) > 0) {
                currentClass = currentEnum.left(c);
                currentEnum = currentEnum.right(currentEnum.size()-c-2);
                qDebug() << "currentEnum =" << currentEnum;
                qDebug() << "currentClass =" << currentClass;
                if (enums.contains(currentEnum,currentClass)) {
                    qWarning() << "DoxWriter::pass1():"
                               << "Duplicate enum:" 
                               << currentClass << currentEnum;
                }
                else 
                    enums.insert(currentEnum,currentClass);
            }
        }
        else if ((cmd = metaCmdMap.find("property")) != metaCmdMap.end()) {
            commentType = PropertyComment;
            currentClass = cmd.value()[0];
            if ((c = currentClass.lastIndexOf("::")) > 0) {
                currentProperty = currentClass.right(currentClass.size()-c-2);
                currentClass = currentClass.left(c);
                qDebug() << "currentProperty =" << currentProperty;
                qDebug() << "currentClass =" << currentClass;
                if (properties.contains(currentProperty,currentClass)) {
                    qWarning() << "DoxWriter::pass1():"
                               << "Duplicate property:" 
                               << currentClass << currentProperty;
                }
                else 
                    properties.insert(currentProperty,currentClass);
            }
        }
        else if ((cmd = metaCmdMap.find("variable")) != metaCmdMap.end()) {
            commentType = VariableComment;
            currentClass = cmd.value()[0];
            if ((c = currentClass.lastIndexOf("::")) > 0) {
                currentVariable = currentClass.right(currentClass.size()-c-2);
                currentClass = currentClass.left(c);
                qDebug() << "currentVariable =" << currentVariable;
                qDebug() << "currentClass =" << currentClass;
                if (variables.contains(currentVariable,currentClass)) {
                    qWarning() << "DoxWriter::pass1():"
                               << "Duplicate variable:" 
                               << currentClass << currentVariable;
                }
                else 
                    variables.insert(currentVariable,currentClass);
            }
        }
    }

    /*
     */
    const Atom* next = priv->text.firstAtom();
    while (next != 0) {
        switch (next->type()) {
            case Atom::SectionHeadingLeft:
                {
                    QString text;
                    next = next->next();
                    while (next) {
                        if (next->type() == Atom::SectionHeadingRight)
                            break;
                        else
                            text += next->string();
                        next = next->next();
                    }
                    //text.remove(ws_rx);
                    insertAnchor(text);
                }
                break;
            case Atom::Target:
                {
                    QString text = next->string();
                    //text.remove(ws_rx);
                    insertAnchor(text);
                }
            default:
                break;
        }
        next = next->next();
    }
}

/*!
  Output a parsed, tokenized qdoc comment as a doxygen
  comment in diff format for input to the patch command.
 */
void DoxWriter::pass2()
{
    if (!conversionRequired()) {
        qDebug() << "NO CONVERSION - FILE:" << priv->start_loc.fileName()
                 << "START:" << priv->start_loc.lineNo()
                 << "END:" << priv->end_loc.lineNo() - 1;
        return;
    }

    /*
      Transformation to doxygen required...
     */
    newText = "\n/*! \n";
    convertMetaCommands();
    convertText();
    if (newText[newText.size()-1] == ' ')
        newText.remove(newText.size()-1,1);
    newText += " */\n";
    qDebug() << "CONVERTED COMMENT - FILE:" << priv->start_loc.fileName()
             << "START:" << priv->start_loc.lineNo()
             << "END:" << priv->end_loc.lineNo() - 1;
    qDebug() << newText; 
}

/*!
  Unparse the second parameter of a "\l" command.
 */
const Atom* DoxWriter::link(const Atom* atom)
{
    QString first_text = atom->string();
    QString second_text;
    const QString* value = 0;

    const Atom* next = atom->next(Atom::FormattingLeft,Atom::LINK_);
    if (next) {
        next->dump();
        while (1) {
            next = next->next();
            next->dump();
            if (next->type() == Atom::FormattingRight) {
                if (next->string() == Atom::LINK_)
                    break;
                else {
                    // ignore it.
                }
            }
            else
                second_text += next->string();
        }
        int i = first_text.indexOf('#');
        if (i >= 0) 
            first_text = first_text.right(first_text.size() - i - 1);
        //newLine();
        if ((value = getExternalPage(first_text))) {
            //qDebug() << "USED AN EXTERNAL PAGE TITLE" << first_text;
            QString href = "<a href=\""+*value+"\">"+first_text+"</a>";
            concatenate(href);
        }
        else if (first_text.startsWith("http:",Qt::CaseInsensitive)) {
            if (first_text == second_text) {
                concatenate(first_text);
            }
            else {
                QString href = "<a href=\""+first_text+"\">"+second_text+"</a>";
                concatenate(href);
            }
        }
        else if ((value = getPageFile(first_text))) {
            //qDebug() << "USED A PAGE TITLE" << first_text;
            QStringList parts = (*value).split('.');
            QString ref = "\\ref " + parts[0] + " \"" + second_text + "\"";
            concatenate(ref);
        }
        else if ((value = getGroup(first_text))) {
            //qDebug() << "USED A GROUP TITLE" << first_text;
            concatenate("\\ref " + *value + " \"" + second_text + "\"");
        }
        else if ((value = getModule(first_text))) {
            //qDebug() << "USED A MODULE TITLE" << first_text;
            concatenate("\\ref " + *value + " \"" + second_text + "\"");
        }
        else if ((value = getExamplePath(first_text))) {
            //qDebug() << "USED AN EXAMPLE TITLE" << first_text;
            first_text.remove(ws_rx);
            QString ref = "\\ref " + first_text + " \"" + second_text + "\"";
            concatenate(ref);
        }
        else if ((value = getFile(first_text))) {
            //qDebug() << "USED A FILE TITLE" << first_text;
            // I think this command is no longer available.
            first_text.remove(ws_rx);
            QString ref = "\\ref " + first_text + " \"" + second_text + "\"";
            concatenate(ref);
        }
        else if ((value = getHeaderFile(first_text))) {
            //qDebug() << "USED A HEADER FILE TITLE" << first_text;
            first_text.remove(ws_rx);
            QString ref = "\\ref " + first_text + " \"" + second_text + "\"";
            concatenate(ref);
        }
        else if (isAnchor(first_text)) {
            //qDebug() << "USED AN ANCHOR" << first_text;
            first_text.remove(ws_rx);
            QString ref = "\\ref " + first_text + " \"" + second_text + "\"";
            concatenate(ref);
        }
        else if ((value = getPageTitle(first_text))) {
            //qDebug() << "USED AN INVERSE PAGE TITLE" << first_text;
            QStringList parts = first_text.split('.');
            QString ref = "\\ref " + parts[0] + " \"" + second_text + "\"";
            concatenate(ref);
        }
        else if ((value = getExampleTitle(first_text))) {
            //qDebug() << "USED AN INVERSE EXAMPLE TITLE" << first_text;
            QString title = *value;
            title.remove(ws_rx);
            QString ref = "\\ref " + title  + " \"" + second_text + "\"";
            concatenate(ref);
        }
        else if ((value = getGroupTitle(first_text))) {
            //qDebug() << "USED AN INVERSE GROUP TITLE" << first_text;
            concatenate("\\ref " + first_text + " \"" + second_text + "\"");
        }
        else if ((value = getModuleTitle(first_text))) {
            //qDebug() << "USED AN INVERSE MODULE TITLE" << first_text;
            concatenate("\\ref " + first_text + " \"" + second_text + "\"");
        }
        else if ((value = getFileTitle(first_text))) {
            qDebug() << "USED AN INVERSE FILE TITLE" << first_text;
        }
        else if ((value = getHeaderFileTitle(first_text))) {
            qDebug() << "USED AN INVERSE HEADER FILE TITLE" << first_text;
        }
        else if ((first_text.indexOf("::") >= 0) ||
                 (first_text.indexOf("()") >= 0) ||
                 (first_text[0] == 'Q')) {
            //qDebug() << "AUTO-LINKABLE" << first_text;
            if (first_text == second_text)
                concatenate(first_text);
            else {
                QString link = first_text + " " + second_text;
                concatenate("\\link " + link + "\\endlink");
            }
        }
        else {
            QString link;
            QStringList propertyClasses;
            QStringList variableClasses;
            QStringList enumClasses;
            bool p = isProperty(first_text,propertyClasses);
            bool v = isVariable(first_text,variableClasses);
            bool e = isEnum(first_text,enumClasses);
            if (e) {
                if (enumClasses.size() == 1)
                    link = enumClasses[0];
                else if (enumClasses.contains(currentClass))
                    link = currentClass;
                else {
                    QString msg = "Unqualified enum name: " + first_text; 
                    QString details = "Classes: " + enumClasses.join(", ");
                    priv->start_loc.error(msg,details);
                }
                if (!link.isEmpty())
                    qDebug() << "FOUND ENUM" << link << first_text;
            }
            else if (p && v) {
                if (propertyClasses.size() == 1) {
                    if (variableClasses.size() == 1) {
                        if (propertyClasses[0] == variableClasses[0])
                            link = propertyClasses[0];
                    }
                }
                if (link.isEmpty()) {
                    if (propertyClasses.contains(currentClass) ||
                        variableClasses.contains(currentClass))
                        link = currentClass;
                    else {
                        propertyClasses += variableClasses;
                        QString msg = "Unqualified property or variable name: "
                            + first_text; 
                        QString details = "Classes: " +
                            propertyClasses.join(", ");
                        priv->start_loc.error(msg,details);
                    }
                }
            }
            else if (p) {
                if (propertyClasses.size() == 1)
                    link = propertyClasses[0];
                else if (propertyClasses.contains(currentClass))
                    link = currentClass;
                else {
                    QString msg = "Unqualified property name: " + first_text; 
                    QString details = "Classes: " + propertyClasses.join(", ");
                    priv->start_loc.error(msg,details);
                }
            }
            else if (v) {
                if (variableClasses.size() == 1)
                    link = variableClasses[0];
                else if (variableClasses.contains(currentClass))
                    link = currentClass;
                else {
                    QString msg = "Unqualified variable name: " + first_text; 
                    QString details = "Classes: " + variableClasses.join(", ");
                    priv->start_loc.error(msg,details);
                }
            }
            else {
                qDebug() << "NOT AUTO-LINKABLE" << first_text;
                QString s = first_text + " " + second_text;
                concatenate("\\link " + s + "\\endlink");
            }
            if (!link.isEmpty()) {
                link += "::" + first_text + " " + second_text;
                concatenate("\\link " + link + "\\endlink");
            }
        }
    }
    else 
        qDebug() << "LINK with no second parameter!!!!";
    return next? next : atom;
}

/*!
  If the current line length is 0, the current line is
  indented according to the context.
 */
void DoxWriter::indentLine()
{
    if (lineLength == 0) {
        newText += DOXYGEN_INDENT_STRING;
        lineLength = DOXYGEN_INDENT;
        if (!structs.isEmpty()) {
            for (int i=1; i<structs.size(); ++i) {
                newText += DOXYGEN_TAB_STRING;
                lineLength += DOXYGEN_TAB_SIZE;
            }
        }
    }
}

/*!
  Concatenates a newline to the doxygen text, increments the
  line count, and resets the line length to 0.
 */
void DoxWriter::newLine()
{
    newText += "\n";
    ++lineCount;
    lineLength = 0;
}

static const int maxLineLength = 70;

/*!
  Concatenate the \a text to the doxygen comment currently
  under construction and increment the current line length
  by the size of the \a text.
  
  If incrementing the current line length by the \a text size
  would make the current line length longer than the maximum
  line length, then call newLine() and indentLine() \e before
  concatenating the \a text.
 */
void DoxWriter::concatenate(QString text)
{
    if ((lineLength + text.size()) > maxLineLength)
        newLine();
    indentLine(); 
    newText += text;
    lineLength += text.size();
}

static bool punctuation(QChar c)
{
    switch (c.toAscii()) {
    case '.':
    case ',':
    case ':':
    case ';':
    case '/':
    case '+':
    case '-':
    case '?':
    case '!':
    case '\"':
        return true;
    default:
        break;
    }
    return false;
}

/*!
  Concatenate the \a text string to the doxygen text, doing
  line wrapping where necessary. 
 */
void DoxWriter::wrap(QString text)
{
    int from = 0;
    int to = -1;

    if ((lineLength == 0) || (lineLength >= maxLineLength)) {
        if (!text.isEmpty() && (text[0] == ' '))
            text = text.right(text.size() - 1);
    }

    indentLine();
    while (text.size()) {
        int avail = maxLineLength - lineLength;
        from = text.indexOf(' ',from);
        if (from >= 0) {
            if (from < avail)
                to = from++;
            else if (from == 1 && punctuation(text[0]))
                to = from++;
            else {
                if (to >= 0) {
                    newText += text.left(to+1);
                    lineLength += to + 1;
                    text = text.right(text.size() - to - 1);
                }
                else {
                    newLine();
                    indentLine();
                    newText += text.left(from+1);
                    lineLength += from + 1;
                    text = text.right(text.size() - from - 1);
                }
                from = 0;
                to = -1;
                if (text.size() && (lineLength > maxLineLength)) {
                    newLine();
                    indentLine();
                }
            }
        }
        else
            break;
    }
    if (text.size()) {
        if (lineLength >= maxLineLength) {
            newLine();
            indentLine();
        }
        newText += text;
        lineLength += text.size();
    }
}

/*!
  This will output something, but it depends on what the
  \a atom string and the \a next atom string are.
 */
void DoxWriter::formattingLeft(const Atom* atom, const Atom* next)
{
    if (atom->string() == "parameter") {
        concatenate("\\a ");
        return;
    }
    else if (atom->string() == "underline") {
        concatenate("<u>");
        return;
    }
    else if (atom->string() == "superscript") {
        concatenate("<sup>");
        return;
    }
    else if (atom->string() == "subscript") {
        concatenate("<sub>");
        return;
    }
    int ws = -1;
    if (next)
        ws = next->string().indexOf(ws_rx);
    if (atom->string() == "bold") {
        if (ws < 0)
            concatenate("\\b ");
        else
            concatenate("<b>");
    }
    else if (atom->string() == "italic") {
        if (ws < 0)
            concatenate("\\e ");
        else
            concatenate("<i>");
    }
    else if (atom->string() == "teletype") {
        if (ws < 0)
            concatenate("\\c ");
        else
            concatenate("<tt>");
    }
    else
        qDebug() << "UNHANDLED FormattingLeft: " << atom->string();
}

/*!
  This will output something, but it depends on what the
  \a atom string and the \a prev atom string are.
 */
void DoxWriter::formattingRight(const Atom* atom, const Atom* prev)
{
    if (atom->string() == "parameter")
        return;
    else if (atom->string() == "underline") {
        concatenate("</u>");
        return;
    }
    else if (atom->string() == "superscript") {
        concatenate("</sup>");
        return;
    }
    else if (atom->string() == "subscript") {
        concatenate("</sub>");
        return;
    }
    int ws = -1;
    if (prev)
        ws = prev->string().indexOf(ws_rx);
    if (ws < 0)
        return;
    if (atom->string() == "bold")
        concatenate("</b>");
    else if (atom->string() == "italic")
        concatenate("</i>");
    else if (atom->string() == "teletype")
        concatenate("</tt>");
    else
        qDebug() << "UNHANDLED FormattingRight: " << atom->string();
}

/*!
  Output a \c or a <tt>...</tt>.
 */
void DoxWriter::tt(const Atom* atom)
{
    if (atom->string().indexOf(ws_rx) < 0) {
        concatenate("\\c ");
        concatenate(atom->string());
    }
    else {
        concatenate("<tt>");
        concatenate(atom->string());
        concatenate("</tt>");
    }
}

/*!
 */
void DoxWriter::formatIf(const Atom* atom)
{
    if (atom->string() == "HTML") {
        newLine();
        concatenate("\\htmlonly");
        newLine();
    }
}

/*!
 */
void DoxWriter::formatEndif()
{
    newLine();
    concatenate("\\endhtmlonly");
    newLine();
}

/*!
 */
void DoxWriter::formatElse()
{
    // nothing.
}

/*!
  Pass 1: Construct a section identifier and insert it into
  the anchor set.
  
  Pass 2: Convert section1, section2, and section3 commands
  to section, subsection, and subsubsection respectively.
  Warn if a section command higher than 3 is seen.
 */
const Atom* DoxWriter::sectionHeading(const Atom* atom)
{
    QString heading_level = atom->string();
    QString heading_text;
    const Atom* next = atom->next();
    while (next) {
        next->dump();
        if (next->type() == Atom::SectionHeadingRight) {
            if (next->string() == heading_level)
                break;
            else {
                qDebug() << "WRONG SectionHeading number!!!!";
            }
        }
        else
            heading_text += next->string();
        next = next->next();
    }

    QString heading_identifier = heading_text;
    heading_identifier.remove(ws_rx);
    
    newLine();
    if (heading_level == "1")
        heading_level = "\\section ";
    else if (heading_level == "2")
        heading_level = "\\subsection ";
    else if (heading_level == "3")
        heading_level = "\\subsubsection ";
    else if (heading_level == "4") {
        heading_level = "\\subsubsection ";
        qDebug() << "WARNING section4 converted to \\subsubsection";
    }
    else {
        heading_level = "\\subsubsection ";
        qDebug() << "WARNING section5 converted to \\subsubsection";
    }
    concatenate(heading_level);
    newText += heading_identifier + " ";
    lineLength += heading_identifier.size() + 1;
    newText += heading_text;
    lineLength += heading_text.size();
    newLine();
    return next? next : atom;
}

/*!
  Report an unhandled atom.
 */
void DoxWriter::unhandled(const Atom* atom)
{
    qDebug() << "UNHANDLED ATOM";
    atom->dump();
}

/*!
  Output a code/endcode block.
 */
void DoxWriter::code(const Atom* atom)
{
    newLine();
    concatenate("\\code");
    writeCode(atom->string());
    concatenate("\\endcode");
    newLine();
}

/*!
  Output a code/endcode block depending on the
  CodeQuote Command and CodeQuoteArgument parameters.
 */
const Atom* DoxWriter::codeQuoteCommand(const Atom* atom)
{
    QString command = atom->string();
    atom = atom->next();
    concatenate("\\code");
    if (command == "codeline") {
        newLine();
        concatenate(atom->string());
        newLine();
    }
    else if (command == "dots") {
        newLine();
        concatenate(atom->string());
        newLine();
    }
    else {
        writeCode(atom->string());
    }
    concatenate("\\endcode");
    return atom;
}

/*!
  Appends a block of code to the comment.
 */
void DoxWriter::writeCode(QString text)
{
    int cr_count = text.count('\n') - 1;
    if (cr_count >= 0) {
        int last_cr = text.lastIndexOf('\n');
        newText += text.left(last_cr);
        lineCount += cr_count;
    }
    else 
        newText += text;
    newLine();
}

/*!
  Inserts \a text into the anchor set. This function is called
  during doxygen pass 1.
 */
void DoxWriter::insertAnchor(const QString& text)
{
    anchors.insert(text);
}

/*!
  Returns true if \a text identifies an anchor, section,
  subsection, subsubsection, or page.
 */
bool DoxWriter::isAnchor(const QString& text)
{
    return anchors.contains(text);
}

/*!
  Write the set of anchors to a file, one per line.
 */
void DoxWriter::writeAnchors()
{
    QFile file("anchors.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning("Unable to open anchors.txt for writing.");
        return;
    }
    
    QTextStream out(&file);
    QSet<QString>::const_iterator i = anchors.constBegin();
    while (i != anchors.constEnd()) {
        out << *i << "\n";
        ++i;
    }
    file.close();
}

/*!
  Read the set of anchors from the anchors file, one per line,
  and insert each one into the anchor set.
 */
void DoxWriter::readAnchors()
{
    QFile file("anchors.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning("Unable to open anchors.txt for reading.");
        return;
    }
    
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        anchors.insert(line);
    }
    file.close();
#if 0    
    QSet<QString>::const_iterator i = anchors.constBegin();
    while (i != anchors.constEnd()) {
        qDebug() << *i;
        ++i;
    }
#endif    
}

/*!
  Inserts \a title into one of the title maps. \a title is
  mapped to the \a node name. This function is called during
  doxygen pass 1.
 */
void DoxWriter::insertTitle(FakeNode* node, const QString& title)
{
    switch (node->subType()) {
    case FakeNode::Example:
        if (exampleTitles.contains(title)) {
            qWarning() << "DoxWriter::insertTitle():"
                       << "Duplicate example title:"
                       << title;
        }
        else {
            exampleTitles[title] = node->name();
            exampleTitlesInverse[node->name()] = title;
        }
        break;
    case FakeNode::HeaderFile: 
        if (headerFileTitles.contains(title)) {
            qWarning() << "DoxWriter::insertTitle():"
                       << "Duplicate header file title:"
                       << title;
        }
        else {
            headerFileTitles[title] = node->name();
            headerFileTitlesInverse[node->name()] = title;
        }
        break;
    case FakeNode::File:
        if (fileTitles.contains(title)) {
            qWarning() << "DoxWriter::insertTitle():"
                       << "Duplicate file title:"
                       << title;
        }
        else {
            fileTitles[title] = node->name();
            fileTitlesInverse[node->name()] = title;
        }
        break;
    case FakeNode::Group:
        if (groupTitles.contains(title)) {
            qWarning() << "DoxWriter::insertTitle():"
                       << "Duplicate group title:"
                       << title;
        }
        else {
            groupTitles[title] = node->name();
            groupTitlesInverse[node->name()] = title;
        }
        break;
    case FakeNode::Module:
        if (moduleTitles.contains(title)) {
            qWarning() << "DoxWriter::insertTitle():"
                       << "Duplicate module title:"
                       << title;
        }
        else {
            moduleTitles[title] = node->name();
            moduleTitlesInverse[node->name()] = title;
        }
        break;
    case FakeNode::Page:
        if (pageTitles.contains(title)) {
            qWarning() << "DoxWriter::insertTitle():"
                       << "Duplicate page title:"
                       << title;
        }
        else {
            pageTitles[title] = node->name();
            pageTitlesInverse[node->name()] = title;
        }
        break;
    case FakeNode::ExternalPage:
        if (externalPageTitles.contains(title)) {
            qWarning() << "DoxWriter::insertTitle():"
                       << "Duplicate external page title:"
                       << title;
        }
        else {
            externalPageTitles[title] = node->name();
            externalPageTitlesInverse[node->name()] = title;
        }
        break;
    default:
        break;
    }
}

/*!
 */
const QString* DoxWriter::getPageFile(const QString& title)
{
    QStringMapEntry entry = pageTitles.find(title);
    return (entry == pageTitles.end()) ? 0 : &entry.value();
}

/*!
 */
const QString* DoxWriter::getExamplePath(const QString& title)
{
    QStringMapEntry entry = exampleTitles.find(title);
    return (entry == exampleTitles.end()) ? 0 : &entry.value();
}

/*!
 */
const QString* DoxWriter::getFile(const QString& title)
{
    QStringMapEntry entry = fileTitles.find(title);
    return (entry == fileTitles.end()) ? 0 : &entry.value();
}

/*!
 */
const QString* DoxWriter::getHeaderFile(const QString& title)
{
    QStringMapEntry entry = headerFileTitles.find(title);
    return (entry == headerFileTitles.end()) ? 0 : &entry.value();
}

/*!
 */
const QString* DoxWriter::getGroup(const QString& title)
{
    QStringMapEntry entry = groupTitles.find(title);
    return (entry == groupTitles.end()) ? 0 : &entry.value();
}

/*!
 */
const QString* DoxWriter::getModule(const QString& title)
{
    QStringMapEntry entry = moduleTitles.find(title);
    return (entry == moduleTitles.end()) ? 0 : &entry.value();
}

/*!
 */
const QString* DoxWriter::getExternalPage(const QString& title)
{
    QStringMapEntry entry = externalPageTitles.find(title);
    return (entry == externalPageTitles.end()) ? 0 : &entry.value();
}

/*!
 */
const QString* DoxWriter::getPageTitle(const QString& text)
{
    QStringMapEntry entry = pageTitlesInverse.find(text);
    return (entry == pageTitlesInverse.end()) ? 0 : &entry.value();
}

/*!
 */
const QString* DoxWriter::getExampleTitle(const QString& text)
{
    QStringMapEntry entry = exampleTitlesInverse.find(text);
    return (entry == exampleTitlesInverse.end()) ? 0 : &entry.value();
}

/*!
 */
const QString* DoxWriter::getFileTitle(const QString& text)
{
    QStringMapEntry entry = fileTitlesInverse.find(text);
    return (entry == fileTitlesInverse.end()) ? 0 : &entry.value();
}

/*!
 */
const QString* DoxWriter::getHeaderFileTitle(const QString& text)
{
    QStringMapEntry entry = headerFileTitlesInverse.find(text);
    return (entry == headerFileTitlesInverse.end()) ? 0 : &entry.value();
}

/*!
 */
const QString* DoxWriter::getGroupTitle(const QString& text)
{
    QStringMapEntry entry = groupTitlesInverse.find(text);
    return (entry == groupTitlesInverse.end()) ? 0 : &entry.value();
}

/*!
 */
const QString* DoxWriter::getModuleTitle(const QString& text)
{
    QStringMapEntry entry = moduleTitlesInverse.find(text);
    return (entry == moduleTitlesInverse.end()) ? 0 : &entry.value();
}

/*!
 */
const QString* DoxWriter::getExternalPageTitle(const QString& text)
{
    QStringMapEntry entry = externalPageTitlesInverse.find(text);
    return (entry == externalPageTitlesInverse.end()) ? 0 : &entry.value();
}

/*!
  Serialize \a map to file \a name.
 */
void DoxWriter::writeMap(const QStringMap& map, const QString& name)
{

    QFile file(name);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Unable to open" << name << "for writing.";
        return;
    }
    
    QTextStream out(&file);
    QStringMap::const_iterator i = map.constBegin();
    while (i != map.constEnd()) {
        out << i.key() << "\n";
        out << i.value() << "\n";
        ++i;
    }
    file.close();
}

/*!
  Read file \a name into the \a map.
 */
void DoxWriter::readMap(QStringMap& map, QStringMap& inverseMap, const QString& name)
{
    QFile file(name);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Unable to open" << name << "for reading.";
        return;
    }
    
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString title = in.readLine();
        QString value = in.readLine();
        map[title] = value;
        inverseMap[value] = title;
    }
    file.close();
}

/*!
  Write the sets of titles to text files, one per line.
 */
void DoxWriter::writeTitles()
{
    if (!pageTitles.isEmpty())
        writeMap(pageTitles,"pagetitles.txt");
    if (!fileTitles.isEmpty())
        writeMap(fileTitles,"filetitles.txt");
    if (!headerFileTitles.isEmpty())
        writeMap(headerFileTitles,"headerfiletitles.txt");
    if (!exampleTitles.isEmpty())
        writeMap(exampleTitles,"exampletitles.txt");
    if (!moduleTitles.isEmpty())
        writeMap(moduleTitles,"moduletitles.txt");
    if (!groupTitles.isEmpty())
        writeMap(groupTitles,"grouptitles.txt");
    if (!externalPageTitles.isEmpty())
        writeMap(externalPageTitles,"externalpagetitles.txt");
}

/*!
  Read the sets of titles from the titles files, one per line,
  and insert each one into the appropriate title set.
 */
void DoxWriter::readTitles()
{
    readMap(pageTitles,pageTitlesInverse,"pagetitles.txt");
    readMap(fileTitles,fileTitlesInverse,"filetitles.txt");
    readMap(headerFileTitles,headerFileTitlesInverse,"headerfiletitles.txt");
    readMap(exampleTitles,exampleTitlesInverse,"exampletitles.txt");
    readMap(moduleTitles,moduleTitlesInverse,"moduletitles.txt");
    readMap(groupTitles,groupTitlesInverse,"grouptitles.txt");
    readMap(externalPageTitles,
            externalPageTitlesInverse,
            "externalpagetitles.txt");
}

/*!
  Serialize \a map to file \a name.
 */
void DoxWriter::writeMultiMap(const QStringMultiMap& map, const QString& name)
{

    QFile file(name);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Unable to open" << name << "for writing.";
        return;
    }
    
    QTextStream out(&file);
    QStringMultiMap::const_iterator i = map.constBegin();
    while (i != map.constEnd()) {
        out << i.key() << "\n";
        out << i.value() << "\n";
        ++i;
    }
    file.close();
}

/*!
  Write the4 property names and variable names to text files.
 */
void DoxWriter::writeMembers()
{
    if (!variables.isEmpty())
        writeMultiMap(variables,"variables.txt");
    if (!properties.isEmpty())
        writeMultiMap(properties,"properties.txt");
    if (!enums.isEmpty())
        writeMultiMap(enums,"enums.txt");
}

/*!
  Read file \a name into the \a map.
 */
void DoxWriter::readMultiMap(QStringMultiMap& map, const QString& name)
{
    QFile file(name);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Unable to open" << name << "for reading.";
        return;
    }
    
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString member = in.readLine();
        QString className = in.readLine();
        map.insert(member,className);
    }
    file.close();
}

/*!
  Read the property names and variable names from the test files.
 */
void DoxWriter::readMembers()
{
    readMultiMap(variables,"variables.txt");
    readMultiMap(properties,"properties.txt");
    readMultiMap(enums,"enums.txt");
}

/*!
  Return true if \a name is a property. Loads \a classes with
  the names of all the classes in which \a name is a property.
 */
bool DoxWriter::isProperty(const QString& name, QStringList& classes)
{
    classes = properties.values(name);
    return !classes.isEmpty();
}

/*!
  Return true if \a name is a variable. Loads \a classes with
  the names of all the classes in which \a name is a variable.
 */
bool DoxWriter::isVariable(const QString& name, QStringList& classes)
{
    classes = variables.values(name);
    return !classes.isEmpty();
}

/*!
  Return true if \a name is an enum type. Loads \a classes with
  the names of all the classes in which \a name is an enum type.
 */
bool DoxWriter::isEnum(const QString& name, QStringList& classes)
{
    classes = enums.values(name);
    return !classes.isEmpty();
}

QT_END_NAMESPACE
