/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the qt3to4 porting application of the Qt Toolkit.
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

#ifndef RPPEXPRESSIONBUILDER_H
#define RPPEXPRESSIONBUILDER_H

#include "tokens.h"
#include "tokenengine.h"
#include "smallobject.h"
#include "rpp.h"
#include <QByteArray>

QT_BEGIN_NAMESPACE

namespace Rpp {

class ExpressionBuilder
{
public:
    ExpressionBuilder(const TokenEngine::TokenList &tokenList, const QVector<Type> &typeList, TypedPool<Item> *memoryPool);
    Rpp::Expression *parse();
private:

    inline bool hasNext() const { return (i < m_tokenList.count()); }
    Type next();
    bool test(int);
    bool moreTokens(int delta);
    inline void prev() {--i;}
    Type lookup(int k = 1);
    inline Type token() { return typeAt(i-1);}
    inline QByteArray lexem() { return m_tokenList.text(i-1);}
    inline Type typeAt(int t) { return m_typeList.at(m_tokenList.containerIndex(t));}

    Expression *conditional_expression();
    Expression *logical_OR_expression();
    Expression *logical_AND_expression();
    Expression *inclusive_OR_expression();
    Expression *exclusive_OR_expression();
    Expression *AND_expression();
    Expression *equality_expression();
    Expression *relational_expression();
    Expression *shift_expression();
    Expression *additive_expression();
    Expression *multiplicative_expression();
    Expression *unary_expression();
    Expression *primary_expression();

    bool unary_expression_lookup();
    bool primary_expression_lookup();

    UnaryExpression *createUnaryExpression(int op, Expression *expression);
    BinaryExpression *createBinaryExpression(int op, Expression *leftExpresson, Expression *rightExpression);
    ConditionalExpression *createConditionalExpression(Expression *condition, Expression *leftExpression, Expression *rightExpression);
    MacroReference *createMacroReference(MacroReference::Type type, TokenEngine::TokenList token);
    IntLiteral *createIntLiteral(const int arg);

    TokenEngine::TokenList createTokenList(int tokenIndex) const;

    int i;
    TokenEngine::TokenList m_tokenList;
    QVector<Type> m_typeList;
    TypedPool<Item> *m_memoryPool;
};

}

QT_END_NAMESPACE

#endif
