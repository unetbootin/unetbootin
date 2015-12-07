/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
** Copyright (C) 2001-2004 Roberto Raggi
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

#include "rpptreeevaluator.h"
#include <QChar>
#include <QtDebug>

QT_BEGIN_NAMESPACE

using namespace TokenEngine;
namespace Rpp {

RppTreeEvaluator::RppTreeEvaluator()
{
    QByteArray text(" ");
    TokenEngine::Token token;
    token.start = 0;
    token.length = 1;
    QVector<TokenEngine::Token> tokenList;
    tokenList.append(token);
    TokenContainer newLineContainer(text, tokenList, new TokenEngine::GeneratedInfo());
    newlineSection= new TokenSection(newLineContainer, 0, 1);
}

RppTreeEvaluator::~RppTreeEvaluator()
{
    delete newlineSection;
}

TokenSectionSequence RppTreeEvaluator::evaluate(const Source *source,
                                                DefineMap *activeDefinitions)
{
    m_tokenSections.clear();
    m_activeDefinitions = activeDefinitions;
    evaluateSource(source);
    return TokenSectionSequence(m_tokenSections);
}

void RppTreeEvaluator::evaluateText(const Text *textLine)
{
    const int numTokens = textLine->count();
    const TokenContainer tokenContainer = textLine->text().tokenContainer(0);

    int t = 0;
    int startTokenRun = 0;
    while(t < numTokens) {
        const Token *currentToken = textLine->token(t);
        int currentContainerIndex = currentToken->index();
        //handle macro replacements
        if(currentToken->toIdToken()) {
            const int tokenIndex = currentToken->index();
            const QByteArray tokenText = tokenContainer.tempText(tokenIndex);
            if(m_activeDefinitions->contains(tokenText)) {
                //crate section
                TokenSection section(tokenContainer, textLine->token(startTokenRun)->index(), t - startTokenRun);
                m_tokenSections.append(section);
                //evaluate macro
                const int oldContainerIndex = currentContainerIndex;
                TokenContainer evaluatedText = evaluateMacro(tokenContainer, currentContainerIndex);
                TokenSection evalSection(evaluatedText, 0, evaluatedText.count());
                m_tokenSections.append(evalSection);
                t += currentContainerIndex - oldContainerIndex;
                startTokenRun = t;
            }
            ++t;
            continue;
        }

        //handle comments
        if(currentToken->toLineComment() || currentToken->toMultiLineComment()) {
            //create section
            TokenSection section(tokenContainer, textLine->token(startTokenRun)->index(), t - startTokenRun );
            m_tokenSections.append(section);
            t++; //skip comment
            startTokenRun = t;
            t++;
            continue;
        }

        // handle escaped newlines
		if (currentContainerIndex + 1 < numTokens) {
            const TokenTempRef tokenRef1 = tokenContainer.tokenTempRef(currentContainerIndex);
            const TokenTempRef tokenRef2 = tokenContainer.tokenTempRef(currentContainerIndex + 1);
			// This is i slight hack. We want to check if the next token is a newline token,
			// but since we don't have any lexical info at this point we just check if it starts
			// with \r or \n
			if (tokenRef1.at(0) == '\\' && (tokenRef2.at(0) == '\n' || tokenRef2.at(0) == '\r')) {
				//create section
                TokenSection section(tokenContainer, textLine->token(startTokenRun)->index(), t - startTokenRun );
                m_tokenSections.append(section);
                t += 2;
                startTokenRun = t;
                t++;
                continue;
            }
        }

        t++;
    }
    //round up any tokens at the end and put them in a section
    if(t - startTokenRun > 1) {
        TokenSection section(tokenContainer, textLine->token(startTokenRun)->index(), t - startTokenRun );
        m_tokenSections.append(section);
    }

    m_tokenSections.append(*newlineSection);
}

/*
    Evaluates and ifsection by selecting which one of the if-elif-else
    groups and then evaling that.
*/
void RppTreeEvaluator::evaluateIfSection(const IfSection *ifSection)
{
    ConditionalDirective *ifGroup = ifSection->ifGroup();
    if(evaluateCondition(ifGroup)) {
        evaluateConditionalDirective(ifGroup);
        return;
    }

    QVector<ConditionalDirective *> elifGroups = ifSection->elifGroups();
    foreach(ConditionalDirective *elifGroup, elifGroups) {
        if(evaluateCondition(elifGroup)) {
            evaluateConditionalDirective(elifGroup);
            return;
        }
    }

    ConditionalDirective *elseGroup = ifSection->elseGroup();
    if(elseGroup)
        evaluateConditionalDirective(elseGroup);
}

/*
    Evaluate an IncludeDirective by evaluating the Source for the included
    file. The source is found by emitting the includeCallback signal, which
    must be handled outside RppTreeEvaluator.
*/
void RppTreeEvaluator::evaluateIncludeDirective(const IncludeDirective *directive)
{
    Source *currentSource = getParentSource(directive);
    IncludeType includeType = includeTypeFromDirective(directive);
    Source *newSource = 0;
    emit includeCallback(newSource, currentSource, QString::fromLatin1(directive->filename().constData()), includeType);
    Q_ASSERT(newSource);    // If you get an assert here you probably
                            // forgot to connect to the includeCallback signal
    evaluateSource(newSource);
}

void RppTreeEvaluator::evaluateDefineDirective(const DefineDirective *directive)
{
    m_tokenSections.append(*newlineSection);
    m_activeDefinitions->insert(directive->identifier().fullText(), directive);
}

void RppTreeEvaluator::evaluateUndefDirective(const UndefDirective *directive)
{
    m_tokenSections.append(*newlineSection);
    const QByteArray text = directive->identifier().fullText();
    m_activeDefinitions->remove(text);
}

/*
    Evaluate the truth-value of an conditionalDirective
*/
bool RppTreeEvaluator::evaluateCondition(const ConditionalDirective *conditionalDirective)
{
    if (IfDirective *ifDirective = conditionalDirective->toIfDirective())
        return (evaluateExpression(ifDirective->expression()) != 0);
    if (ElifDirective *elifDirective = conditionalDirective->toElifDirective())
        return (evaluateExpression(elifDirective->expression()) != 0);
    if (IfdefDirective *ifdefDirective = conditionalDirective->toIfdefDirective())
        return m_activeDefinitions->contains(ifdefDirective->identifier().fullText());
    if (IfndefDirective *ifndefDirective = conditionalDirective->toIfndefDirective())
        return !m_activeDefinitions->contains(ifndefDirective->identifier().fullText());
    else
        return false; //error!
}

/*
    Recursively evaluates an Expression
*/
int RppTreeEvaluator::evaluateExpression(Expression *expression)
{
    if (IntLiteral *e = expression->toIntLiteral()) {
        return e->value();
    } else if (StringLiteral *e = expression->toStringLiteral()) {
        return e->value().size();
    } else if (MacroReference *e = expression->toMacroReference()) {
       switch(e->type()) {
           case MacroReference::DefinedRef: {
               return m_activeDefinitions->contains(e->name().fullText()) ? 1 : 0;
           } case MacroReference::ValueRef: {
               const QByteArray identifier = e->name().fullText();
               if (m_activeDefinitions->contains(identifier)) {
                   int token = e->name().containerIndex(0);
                   TokenContainer value = evaluateMacro(e->name().tokenContainer(token), token);
                   return QString(QLatin1String(value.fullText())).toInt(0, 0);
               } else {
                   return 0; // error
               }
           }
           default: Q_ASSERT(0);
        }
    } else if (MacroFunctionReference *e = expression->toMacroFunctionReference()) {
        Q_UNUSED(e);
        //TODO handle MacroFunctionReference
//        DefineDirective *def = e->findDefinition(e->name());
//        Q_ASSERT(def->toMacroFunctionDefinition());
//        qWarning("not implemented yet");
        return 0;
    } else if (UnaryExpression *e = expression->toUnaryExpression()) {
        int result = evaluateExpression(e->expression());
        switch (e->op()) {
            case '+': return + result;
            case '-': return - result;
            case '!': return ! result;
            case '~': return ~ result;
            default:  Q_ASSERT(0);
        }
    } else if (BinaryExpression *e = expression->toBinaryExpression()) {
        int v1 = evaluateExpression(e->leftExpression());
        int v2 = evaluateExpression(e->rightExpression());

        switch (e->op()) {
            case '/': { return v2 ? v1 / v2 : 0; } //avoid division by zero
            case '*':                  return v1 * v2;
            case '%': { return v2 ? v1 % v2 : 0; } //avoid modulus by zero
            case '+':                  return v1 + v2;
            case '-':                  return v1 - v2;
            case '<':                  return v1 < v2;
            case '>':                  return v1 > v2;
            case '&':                  return v1 & v2;
            case '^':                  return v1 ^ v2;
            case '|':                  return v1 | v2;
            case Expression::LtEqOp:   return v1 <= v2;
            case Expression::GtEqOp:   return v1 >= v2;
            case Expression::EqOp:     return v1 == v2;
            case Expression::NotEqOp:  return v1 != v2;
            case Expression::AndOp:    return v1 && v2;
            case Expression::OrOp:     return v1 || v2;
            case Expression::LShiftOp: return v1 << v2;
            case Expression::RShiftOp: return v1 >> v2;
            default:    Q_ASSERT(0);
        }

    } else if ( ConditionalExpression *e = expression->toConditionalExpression()){
        return e->condition() ? evaluateExpression(e->leftExpression()) : evaluateExpression(e->rightExpression());
    }
    return 0;
}
/*
    Expands a macro at index identiferTokenIndex in tokenContainer. Returns
    the expanded macro text, and updates identiferTokenIndex to point after
    the last token consumed.

    Given the construct 'FN(a)', the '(a)' part will be consumed if FN is
    defined to be a macro function, but not if it is an ordenary macro.
*/
TokenContainer RppTreeEvaluator::evaluateMacro(TokenContainer tokenContainer, int &identiferTokenIndex)
{
    QByteArray identifierText = tokenContainer.text(identiferTokenIndex);
    if(!m_activeDefinitions->contains(identifierText))
        return TokenContainer();

    const Rpp::DefineDirective *directive = m_activeDefinitions->value(identifierText);
    Q_ASSERT(directive);

    // To prevent infinite recursive macro expansions, the skip set contains
    // a set of identifers already seen.
    QSet<QByteArray> skip;

    if(directive->toMacroDefinition()) {
        ++identiferTokenIndex;
        QVector<TokenEngine::Token> tokenList;
        tokenList.append(TokenEngine::Token(0, identifierText.count()));
        return evaluateMacroInternal(skip, TokenContainer(identifierText, tokenList));
    } else if (Rpp::MacroFunctionDefinition *macro = directive->toMacroFunctionDefinition()) {
        MacroFunctionParser macroFunctionParser(tokenContainer, identiferTokenIndex);
        if (macroFunctionParser.isValid() && macro->parameters().count() ==  macroFunctionParser.argumentCount()) {
            TokenContainer macroFunctionContainer =
                TokenEngine::copy(tokenContainer, identiferTokenIndex, macroFunctionParser.tokenCount());
            identiferTokenIndex += macroFunctionParser.tokenCount();
            return evaluateMacroInternal(skip, macroFunctionContainer);
        } else {
            // Error case, such as calling a macro function with the wrong number of parameters,
            // or calling a macro function witout a parameter list.
            return TokenEngine::copy(tokenContainer, identiferTokenIndex++, 1);
        }
    }
    return TokenContainer();
}

/*
    Recursively expands all macroes in macroInvokeTokens, returns a
    TokenContainer with the new tokens.
*/
TokenEngine::TokenContainer RppTreeEvaluator::evaluateMacroInternal(QSet<QByteArray> skip, TokenEngine::TokenContainer macroInvokeTokens)
{
    bool changed = false;
    QByteArray tokenText;
    QVector<TokenEngine::Token> tokenList;
    const int numTokens = macroInvokeTokens.count();

    for (int t = 0; t < numTokens; ++t) {
        const QByteArray identifierText = macroInvokeTokens.text(t);

        // if the current token text is not a part of a macro definition we just copy it.
        if (!m_activeDefinitions->contains(identifierText)) {
            tokenList.append(TokenEngine::Token(tokenText.count(), identifierText.count()));
            tokenText.append(identifierText);
            continue;
        }

        // If the token text is in the skip list we copy it.
         if (skip.contains(identifierText)) {
            tokenList.append(TokenEngine::Token(tokenText.count(), identifierText.count()));
            tokenText.append(identifierText);
            continue;
        }

        skip.insert(identifierText);
        changed = true;
        const Rpp::DefineDirective *directive = m_activeDefinitions->value(identifierText);
        Q_ASSERT(directive);
        // if it is a macro, we copy in the replacement list.
        if (Rpp::MacroDefinition *macro = directive->toMacroDefinition()) {
            TokenList replacementList = macro->replacementList();
            TokenEngine::copy(tokenText, tokenList, replacementList, 0, replacementList.count());

            // To avoid infinite loops, set changed to false if the replacement
            // text is identical to the identifier text.
            if (replacementList.fullText().simplified() == identifierText.simplified())
                changed = false;
        } else if (Rpp::MacroFunctionDefinition *macro = directive->toMacroFunctionDefinition()) {
            TokenList replacementList = macro->replacementList();
            TokenList paramenterList =  macro->parameters();

            MacroFunctionParser macroFunctionParser(macroInvokeTokens, t);
            if (macroFunctionParser.isValid() && macro->parameters().count() == macroFunctionParser.argumentCount()) {
                t += macroFunctionParser.tokenCount();
                // For each token in the replacement list: If the token matches a
                // token in the parameter list, replace it with the
                // corresponding argument tokens from the argument list.
                for (int replacementToken = 0; replacementToken < replacementList.count(); ++replacementToken) {
                    const QByteArray replacementTokenText = replacementList.text(replacementToken);
                    bool replaced = false;
                    for (int parameterToken = 0; parameterToken < paramenterList.count(); ++parameterToken) {
                        const QByteArray parameterTokenText = paramenterList.text(parameterToken);
                        if (parameterTokenText == replacementTokenText) {
                            TokenSection argumentTokenSection = macroFunctionParser.argument(parameterToken);
                            TokenEngine::copy(tokenText, tokenList, argumentTokenSection, 0, argumentTokenSection.count());
                            replaced = true;
                            break;
                        }
                    }
                    if (! replaced) {
                        TokenEngine::copy(tokenText, tokenList, replacementList, replacementToken, 1);
                    }
                }
            }
        }
    }
    if (!changed)
        return macroInvokeTokens;
    return evaluateMacroInternal(skip, TokenContainer(tokenText, tokenList));
}

TokenContainer RppTreeEvaluator::cloneTokenList(const TokenList &list)
{
    QByteArray text;
    QVector<TokenEngine::Token> tokens;
    int index = 0;
    for (int t = 0; t<list.count(); ++t) {
        const QByteArray tokenText = list.text(t);
        const int textLength = tokenText.count();
        text += tokenText;
        TokenEngine::Token token;
        token.start = index;
        token.length = textLength;
        tokens.append(token);
        index += textLength;
    }
    TokenContainer container(text, tokens, new GeneratedInfo());
    return container;
}

/*
    Returns the parent Source for a given item.
*/
Source *RppTreeEvaluator::getParentSource(const Item *item) const
{
    Q_ASSERT(item);
    while(item->toSource() == 0) {
        item = item->parent();
        Q_ASSERT(item);
    }

    return item->toSource();
}
/*
    We have two IncludeType enums, one in IncludeDirective and one in
    RppTreeEvaluator. This function translates between them.
*/
RppTreeEvaluator::IncludeType RppTreeEvaluator::includeTypeFromDirective(
                    const IncludeDirective *includeDirective) const
{
    if(includeDirective->includeType() == IncludeDirective::QuoteInclude)
        return QuoteInclude;
    else
        return AngleBracketInclude;
}

/*
    The MacrofunctionParser class is used to parse a macro function call (not
    a macro function definition.)

    startToken should give the token index for the identifier token for the macro function.
*/
MacroFunctionParser::MacroFunctionParser(const TokenEngine::TokenContainer &tokenContainer, int startToken)
:m_tokenContainer(tokenContainer)
,m_startToken(startToken)
,m_numTokens(0)
,m_valid(false)
{
    int tokenIndex = startToken;
    ++tokenIndex; //skip identifier token
    int parenthesisCount = 0;
    int currentArgumentStartToken = tokenIndex;

    // Parse argument tokens, add arguments to the m_arguments list.
    // Arguments may consist of multiple tokens. Parenthesis in arguments
    // are allowed, as long as they match. Inside a pair of argument
    // parenthesis, ',' no longer signals a new argument. For example,
    // FN((a,b)) is legal and contains one argument.
    while(tokenIndex < tokenContainer.count()) {
        QByteArray currentText = tokenContainer.text(tokenIndex);
        ++tokenIndex;
        if (currentText == "(") {
            ++parenthesisCount;
            if (parenthesisCount == 1) {
                // first parenthesis
                currentArgumentStartToken = tokenIndex;
                continue;
            }
        }
        if (currentText == ")") {
            --parenthesisCount;
            if (parenthesisCount == 0) {
                //end of argument
                m_arguments.append(TokenSection(tokenContainer, currentArgumentStartToken, tokenIndex - currentArgumentStartToken - 1));
                currentArgumentStartToken = tokenIndex;
                //end of argument list
                break;
            }
        }
        if (currentText == "," && parenthesisCount == 1) {
            //end of argument
            m_arguments.append(TokenSection(tokenContainer, currentArgumentStartToken, tokenIndex - currentArgumentStartToken - 1));
            currentArgumentStartToken = tokenIndex;
            continue;
        }

        if (QChar::fromLatin1(currentText.at(0)).isSpace()) {
            continue;
        }

        // If we get here without having seen a paranthesis we have a syntax
        // error in the macro function call.
        if (parenthesisCount == 0) {
            parenthesisCount = -1;
            break;
        }
    }
    m_numTokens = tokenIndex - startToken;
    m_valid = (parenthesisCount == 0);
}

/*
    Returns true if the MacroFunctionParser contains a valid macro function
*/
bool MacroFunctionParser::isValid()
{
    return m_valid;
}

/*
    Returns the number of tokens in the tokenContainer that is covered by
    the macro function.
*/
int MacroFunctionParser::tokenCount()
{
    return m_numTokens;
}

/*
    Returns the number of arguments for the macro function.
*/
int MacroFunctionParser::argumentCount()
{
    return m_arguments.count();
}

/*
    Returns the tokens for the argument given by argumentIndex.
*/
TokenSection MacroFunctionParser::argument(int argumentIndex)
{
    Q_ASSERT(argumentIndex < m_arguments.count());
    return m_arguments.at(argumentIndex);
}

} //namespace Rpp

QT_END_NAMESPACE
