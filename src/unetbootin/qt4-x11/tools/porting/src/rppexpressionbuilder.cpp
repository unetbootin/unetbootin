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

#include "rppexpressionbuilder.h"
#include "tokens.h"
#include "rpp.h"

QT_BEGIN_NAMESPACE

using namespace TokenEngine;
namespace Rpp {

ExpressionBuilder::ExpressionBuilder(const TokenList &tokenList, const QVector<Type> &typeList, TypedPool<Item> *memoryPool)
:i(0)
,m_tokenList(tokenList)
,m_typeList(typeList)
,m_memoryPool(memoryPool)
{}

Rpp::Expression *ExpressionBuilder::parse()
{
    if(unary_expression_lookup())
        return conditional_expression();
    else
        return createIntLiteral(0);
}
Type ExpressionBuilder::next()
{
    if(!hasNext())
        return Token_eof;
    return typeAt(i++);
}

inline bool ExpressionBuilder::test(int token)
{
    if (i < m_tokenList.count() && typeAt(i) == token) {
        ++i;
        return true;
    }
    return false;
}

inline bool ExpressionBuilder::moreTokens(int delta)
{
    return (i + delta < m_tokenList.count());
}

inline Type ExpressionBuilder::lookup(int k)
{
    const int l = i - 1 + k;
    return l < m_tokenList.count() ? typeAt(l) : Token_eof;
}

Expression *ExpressionBuilder::conditional_expression()
{
    Expression *value = logical_OR_expression();
    if (test('?')) {
        Expression *leftExpression = conditional_expression();
        Expression *rightExpression;
        if(test(':'))
            rightExpression = conditional_expression();
        else
            rightExpression = createIntLiteral(0);
        return createConditionalExpression(value, leftExpression, rightExpression);
    }
    return value;
}

Expression *ExpressionBuilder::logical_OR_expression()
{
    Expression *value = logical_AND_expression();
    if (test(Token_or))
        return createBinaryExpression(Expression::OrOp, value, logical_OR_expression());
    return value;
}

Expression *ExpressionBuilder::logical_AND_expression()
{
    Expression *value = inclusive_OR_expression();
    if (test(Token_and))
        return createBinaryExpression(Expression::AndOp, value, logical_AND_expression());
    return value;
}

Expression *ExpressionBuilder::inclusive_OR_expression()
{
    Expression *value = exclusive_OR_expression();
    if (test('|'))
        return createBinaryExpression('|', value, inclusive_OR_expression());
    return value;
}

Expression *ExpressionBuilder::exclusive_OR_expression()
{
    Expression *value = AND_expression();
    if (test('^'))
        return createBinaryExpression('^', value, exclusive_OR_expression());
    return value;
}

Expression *ExpressionBuilder::AND_expression()
{
    Expression *value = equality_expression();
    if (test('&'))
        return createBinaryExpression('&', value, AND_expression());
    return value;
}

Expression *ExpressionBuilder::equality_expression()
{
    Expression *value = relational_expression();
    switch (next()) {
    case Token_eq:
        return createBinaryExpression(Expression::EqOp, value, equality_expression());
    case Token_not_eq:
        return createBinaryExpression(Expression::NotEqOp, value, equality_expression());
    default:
        prev();
        return value;
    }
}

Expression *ExpressionBuilder::relational_expression()
{
    Expression *value = shift_expression();
    switch (next()) {
    case '<':
        return createBinaryExpression('<', value, relational_expression());
    case '>':
        return createBinaryExpression('>', value, relational_expression());
    case Token_leq:
        return createBinaryExpression(Expression::LtEqOp, value, relational_expression());
    case Token_geq:
        return createBinaryExpression(Expression::GtEqOp, value, relational_expression());
    default:
        prev();
        return value;
    }
}

Expression *ExpressionBuilder::shift_expression()
{
    Expression *value = additive_expression();
    switch (next()) {
    case Token_left_shift:
        return createBinaryExpression(Expression::LShiftOp, value, shift_expression());
    case Token_right_shift:
        return createBinaryExpression(Expression::RShiftOp, value, shift_expression());
    default:
        prev();
        return value;
    }
}

Expression *ExpressionBuilder::additive_expression()
{
    Expression *value = multiplicative_expression();
    switch (next()) {
    case '+':
        return createBinaryExpression('+', value, additive_expression());
    case '-':
        return createBinaryExpression('-', value, additive_expression());
    default:
        prev();
        return value;
    }
}

Expression *ExpressionBuilder::multiplicative_expression()
{
    Expression *value = unary_expression();
    switch (next()) {
    case '*':
        return createBinaryExpression('*', value, multiplicative_expression());
    case '%':
        return createBinaryExpression('%', value, multiplicative_expression());
    case '/':
        return createBinaryExpression('/', value, multiplicative_expression());
    default:
        prev();
        return value;
    };
}

Expression *ExpressionBuilder::unary_expression()
{
    switch (next()) {
    case '+':
        return createUnaryExpression('+', unary_expression());
    case '-':
        return createUnaryExpression('-',  unary_expression());
    case '!':
        return createUnaryExpression('!',  unary_expression());
    case '~':
        return createUnaryExpression('~', unary_expression());
    case Token_defined:
    {
        int identifierIndex  = 0;
        if (test(Token_identifier)) {
            identifierIndex = i - 1;
        } else if (test('(')) {
            if (test(Token_identifier))
                identifierIndex = i -1;
            test(')');
        }
        return createMacroReference(MacroReference::DefinedRef, createTokenList(identifierIndex));
    }
    default:
        prev();
        return primary_expression();
    }
}

bool ExpressionBuilder::unary_expression_lookup()
{
    Type t = lookup();
    return (primary_expression_lookup()
            || t == '+'
            || t == '-'
            || t == '!'
            || t == '~'
            || t == Token_defined);
}

Expression *ExpressionBuilder::primary_expression()
{
    Expression *value;
    if (test('(')) {
        if (moreTokens(1))
            value = conditional_expression();
        else
            value = createIntLiteral(0); // Syntax error.
        test(')');
    } else {
        next();
        bool ok;
        int val  = QString::fromLatin1(lexem()).toInt(&ok, 0);
        if(ok)
            value = createIntLiteral(val);
        else
            value = createMacroReference(MacroReference::ValueRef, createTokenList(i -1));
    }
    return value;
}

bool ExpressionBuilder::primary_expression_lookup()
{
    Type t = lookup();
    return (t == Token_identifier
            || t == Token_number_literal
/*            || t == PP_FLOATING_LITERAL*/
            || t == '(');
}
/*
    Creates a tokenList containing one token
*/
TokenList ExpressionBuilder::createTokenList(int tokenIndex) const
{
    return TokenList(m_tokenList.tokenContainer(tokenIndex),
        QVector<int>() << m_tokenList.containerIndex(tokenIndex));
}
/*
    Node cration helper functions
*/
UnaryExpression *ExpressionBuilder::createUnaryExpression(int op, Expression *expression)
{
    return new (m_memoryPool->allocate(sizeof(UnaryExpression))) UnaryExpression(op, expression);
}

BinaryExpression *ExpressionBuilder::createBinaryExpression(int op, Expression *leftExpresson, Expression *rightExpression)
{
    return new (m_memoryPool->allocate(sizeof(BinaryExpression))) BinaryExpression(op, leftExpresson, rightExpression);
}

ConditionalExpression *ExpressionBuilder::createConditionalExpression(Expression *condition, Expression *leftExpression, Expression *rightExpression)
{
    return new (m_memoryPool->allocate(sizeof(ConditionalExpression))) ConditionalExpression(condition, leftExpression, rightExpression);
}

MacroReference *ExpressionBuilder::createMacroReference(MacroReference::Type type, TokenEngine::TokenList token)
{
    return new (m_memoryPool->allocate(sizeof(MacroReference))) MacroReference(token, type);
}

IntLiteral *ExpressionBuilder::createIntLiteral(const int arg)
{
    return new (m_memoryPool->allocate(sizeof(IntLiteral))) IntLiteral(arg);
}

}

QT_END_NAMESPACE
