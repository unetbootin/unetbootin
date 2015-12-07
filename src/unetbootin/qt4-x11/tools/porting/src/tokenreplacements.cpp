/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the qt3to4 porting application of the Qt Toolkit.
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

#include "tokenreplacements.h"
#include "logger.h"
#include "portingrules.h"

QT_BEGIN_NAMESPACE
using namespace TokenEngine;

void addLogSourceEntry(const QString &text, const TokenContainer &tokenContainer, const int index)
{
    Logger *logger = Logger::instance();
    int line = tokenContainer.line(index);
    int col = tokenContainer.column(index);
    SourcePointLogEntry *logEntry =
                new SourcePointLogEntry(QLatin1String("Info"), QLatin1String("Porting"),
                                        logger->globalState.value(QLatin1String("currentFileName")),
                                        line, col, text);
    logger->addEntry(logEntry);
}

void addLogWarning(const QString &text)
{
     Logger::instance()->addEntry(new PlainLogEntry(QLatin1String("Warning"), QLatin1String("Porting"), text));
}

QualifiedNameParser::QualifiedNameParser(const TokenContainer &tokenContainer, const int tokenIndex)
:tokenContainer(tokenContainer)
,currentIndex(tokenIndex)
{
    Q_ASSERT(isValidIndex(currentIndex));
}

bool QualifiedNameParser::isPartOfQualifiedName()
{
    return ((nextScopeToken(Left) != -1) || (nextScopeToken(Right) != -1));
}


bool QualifiedNameParser::isValidIndex(int index)
{
    return (index < tokenContainer.count() && index >= 0);
}

/*
    A qualifier is a the leftmost or middle part of a qualified name
*/
bool QualifiedNameParser::isQualifier()
{
    return (nextScopeToken(Right) != -1);
}

/*
    A name is a the rightmost part of a qualified name.
*/
bool QualifiedNameParser::isName()
{
    return (nextScopeToken(Left) != -1);
}

/*
    Peek for a qualifier or name in the given direction
*/
int QualifiedNameParser::peek(Direction direction)
{
    return nextScopeToken(direction);
}

/*
    Look for a qualifier or name in the given direction,update
    current position if found.
*/
int QualifiedNameParser::move(Direction direction)
{
    int tokenIndex = nextScopeToken(direction);
    if(tokenIndex != -1)
        currentIndex = tokenIndex;
    return tokenIndex;
}

/*
    Looks for "::" starting at currentIndex, returns the token index
    for it if found. If the first non-whitespace token found is something else,
    -1 is returned.
*/
int QualifiedNameParser::findScopeOperator(Direction direction)
{
    int tokenIndex = currentIndex;
    QByteArray tokenText;
    //loop until we get a token containg text or we pass the beginning/end of the source
    tokenIndex += direction;
    while(tokenText.isEmpty() && isValidIndex(tokenIndex)) {
        tokenText = tokenContainer.text(tokenIndex).trimmed();
        if(tokenText==QByteArray("::"))
           return tokenIndex;
        tokenIndex += direction;
    }
    return -1;
}
/*
    Walks a qualified name. Returns the token index
    for the next identifer in the qualified name, or -1 if its not found.
*/
int QualifiedNameParser::nextScopeToken(Direction direction)
{
    int tokenIndex  = findScopeOperator(direction);
    if (tokenIndex == -1)
        return -1;
    QByteArray tokenText;
   //loop until we get a token containg text or we pass the start of the source
    tokenIndex += direction;
    while(tokenText.isEmpty() && isValidIndex(tokenIndex)) {
       tokenText = tokenContainer.text(tokenIndex).trimmed();
       tokenIndex += direction;
    }
    return tokenIndex - direction;
}

/////////////////////
GenericTokenReplacement::GenericTokenReplacement(QByteArray oldToken, QByteArray newToken)
:oldToken(oldToken)
,newToken(newToken)
{}

QByteArray GenericTokenReplacement::getReplaceKey()
{
    return QByteArray(oldToken);
}

bool GenericTokenReplacement::doReplace(const TokenContainer &tokenContainer,
                            int index, TextReplacements &textReplacements)
{
    QByteArray tokenText = tokenContainer.text(index);
    if(tokenText == oldToken){
        addLogSourceEntry(QString::fromLatin1(tokenText + QByteArray(" -> ") + newToken), tokenContainer, index);
        TokenEngine::Token token = tokenContainer.token(index);
        textReplacements.insert(newToken, token.start, token.length);
        return true;
    }
    return false;

}

///////////////////
ClassNameReplacement::ClassNameReplacement(QByteArray oldToken, QByteArray newToken)
:oldToken(oldToken)
,newToken(newToken)
{}

QByteArray ClassNameReplacement::getReplaceKey()
{
    return QByteArray(oldToken);
}

/*
    Replace a class name token. If the class name is a scope specifier (a "qualifier")
    in a qualified name, we check if qualified name will be replaced by a porting rule.
    If so, we don't do the class name replacement.
*/
bool ClassNameReplacement::doReplace(const TokenContainer &tokenContainer, int index, TextReplacements &textReplacements)
{
    QByteArray tokenText = tokenContainer.text(index);
    if(tokenText != oldToken)
        return false;

    QualifiedNameParser nameParser(tokenContainer, index);
    if(nameParser.isPartOfQualifiedName() &&
       nameParser.peek(QualifiedNameParser::Right) != -1) {
        int nameTokenIndex = nameParser.peek(QualifiedNameParser::Right);
        QByteArray name = tokenContainer.text(nameTokenIndex); 
        TextReplacements textReplacements;
        QList<TokenReplacement*> tokenReplacements
            = PortingRules::instance()->getTokenReplacementRules();
        bool changed = false;
        foreach(TokenReplacement *tokenReplacement, tokenReplacements) {
            changed = tokenReplacement->doReplace(tokenContainer, nameTokenIndex, textReplacements);
            if(changed)
                break;
        }
        if(changed)
            return false;
    }
    addLogSourceEntry(QString::fromLatin1(tokenText + QByteArray(" -> ") + newToken), tokenContainer, index);
    TokenEngine::Token token = tokenContainer.token(index);
    textReplacements.insert(newToken, token.start, token.length);
    return true;
}

///////////////////

ScopedTokenReplacement::ScopedTokenReplacement(const QByteArray &oldToken,
                                               const QByteArray &newToken)
:newScopedName(newToken)
{
    Q_ASSERT(oldToken.contains(QByteArray("::")));

    // Split oldToken into scope and name parts.
    oldName = oldToken.mid(oldToken.lastIndexOf(':')+1);
    oldScope = oldToken.mid(0, oldToken.indexOf(':'));

    // Split newToken into scope and name parts, execept if we have a spcial
    // case like Qt::WType_Modal -> (Qt::WType_Dialog | Qt::WShowModal)
    if (newToken.count(QByteArray("::")) != 1 || newToken.contains(QByteArray("("))) {
        newName = newToken;
    } else {
        newName = newToken.mid(newToken.lastIndexOf(':')+1);
        newScope = newToken.mid(0, newToken.indexOf(':'));
    }

    strictMode = Logger::instance()->globalState.contains(QString::fromLatin1("strictMode"));
}

bool ScopedTokenReplacement::doReplace(const TokenContainer &tokenContainer, int sourceIndex, TextReplacements &textReplacements)
{
    const QByteArray sourceName = tokenContainer.text(sourceIndex);

    // Check if the token texts matches.
    if (sourceName != oldName)
        return false;

    // Get token attributes. The attributes are created by the the C++ parser/analyzer.
    const TokenAttributes *attributes = tokenContainer.tokenAttributes();
    // If the declaration attribute is set we don't replace.
    if (!attributes->attribute(sourceIndex, "declaration").isEmpty())
        return false;
    // If the unknown (undeclared) attribute is set we don't replace.
    if (!attributes->attribute(sourceIndex, "unknown").isEmpty())
        return false;
    // If nameUse is set we test if the nameUse refers to the correct declaration.
    // This is done by checking the parentScope attriute, wich returns the scope
    // for the declaration associated with this name use.
    const bool haveNameUseInfo = !attributes->attribute(sourceIndex, "nameUse").isEmpty();
    if (haveNameUseInfo) {
        if (attributes->attribute(sourceIndex, "parentScope") != oldScope)
            return false;
    // If the user has specified -strict, we don't replace tokens when we don't have name use info.
    } else if (strictMode) {
        return false;
    }

    // The token might have a qualifier, and in that case we need to check if
    // we should replace the qualifier as well.
    QualifiedNameParser nameParser(tokenContainer, sourceIndex);

    // This is a pretty special case, it means that in a qualified
    // name like aaa::bbb the replacement rule has been triggered for
    // the aaa part. Since this is not what we'd normally use a
    // ScopedReplacement for, we just return here.
    if (nameParser.isQualifier())
        return false;

    // If the token is unqualified, just replace it.
    if (!nameParser.isPartOfQualifiedName()) {
        // If we have no name use info we try to avoid replacements of
        // e.g. Vertical with QSizePolicy::Vertically. Unqualified tokens
        // can't happen for classes one does not usually inherit from, so
        // we only let them pass for stuff that people usually inherited from.
        if (!haveNameUseInfo && newScope != "Qt" && newScope != "QFrame" && newScope != "QValidator")
            return false;

        const Token sourceToken = tokenContainer.token(sourceIndex);
        addLogSourceEntry(QString::fromLatin1(sourceName + QByteArray(" -> ") + newScopedName), tokenContainer, sourceIndex);
        textReplacements.insert(newScopedName, sourceToken.start, sourceName.size());
        return true;
    }

    // Peek left for the qualifer token.
    const int sourceScopeIndex = nameParser.peek(QualifiedNameParser::Left);
    if (sourceScopeIndex == -1) {
        return false;
    }

    const Token sourceNameToken = tokenContainer.token(sourceIndex);
    const Token sourceScopeToken = tokenContainer.token(sourceScopeIndex);
    const QByteArray sourceScope = tokenContainer.text(sourceScopeIndex);

    // If we have no name use info and the source and old scopes don't match,
    // we generally dont't do a replace, unless the old scope is Qt and
    // the source scope inherits Qt. For example, QWidget::ButtonState should
    // be renamed to Qt::ButtonState.
    if (!haveNameUseInfo && sourceScope != oldScope) {
        if (oldScope != "Qt")
            return false;
        // Check if sourceScope inherits the Qt class.
        if (!PortingRules::instance()->getInheritsQt().contains(QString::fromLatin1(sourceScope.constData()))) //TODO optimize: linear search
            return false;
    }

    // Spcecial cases, such as QIODevice::Offset -> Q_LONGLONG
    // or Qt::WType_Modal -> (Qt::WType_Dialog | Qt::WShowModal).
    if (newScope.isEmpty()) {
        addLogSourceEntry(QString::fromLatin1((sourceScope + QByteArray("::") + sourceName +
                          QByteArray(" -> ") + newScopedName).constData()), tokenContainer, sourceIndex);
        const int qualiferLength = sourceNameToken.start - sourceScopeToken.start;
        const int length = qualiferLength + sourceNameToken.length;
        textReplacements.insert(newName, sourceScopeToken.start, length);
        return true;
    }

    // If the old and new scopes are equal, we replace the name part only.
    if (newScope == sourceScope) {
        // If the names are equal, there is no need to do anything.
        if (newName == sourceName)
            return true;
        addLogSourceEntry(QString::fromLatin1((sourceName + QByteArray(" -> ") + newName).constData()), tokenContainer, sourceIndex);
        textReplacements.insert(newName, sourceNameToken.start, sourceNameToken.length);
        return true;
    }

    // If the names are equal, replace scope only.
    if (newName == sourceName) {
        addLogSourceEntry(QString::fromLatin1((sourceScope + QByteArray(" -> ") + newScope).constData()), tokenContainer, sourceScopeIndex);
        textReplacements.insert(newScope, sourceScopeToken.start, sourceScopeToken.length);
        return true;
    }

    // Replace scope and name.
    addLogSourceEntry(QString::fromLatin1((sourceScope + QByteArray("::") + sourceName +
                      QByteArray(" -> ") + newScopedName).constData()),
                      tokenContainer, sourceScopeIndex);
    textReplacements.insert(newScope, sourceScopeToken.start, sourceScopeToken.length);
    textReplacements.insert(newName, sourceNameToken.start, sourceNameToken.length);
    return true;
}

QByteArray ScopedTokenReplacement::getReplaceKey()
{
   return oldName;
}


QT_END_NAMESPACE
