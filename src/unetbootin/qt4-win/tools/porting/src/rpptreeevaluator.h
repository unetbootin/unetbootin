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

#ifndef RPPTREEEVALUATOR_H
#define RPPTREEEVALUATOR_H

#include "tokenengine.h"
#include "rpp.h"
#include "rpptreewalker.h"
#include <QObject>
#include <QList>
#include <QHash>
#include <QSet>

QT_BEGIN_NAMESPACE

namespace Rpp {

class DefineMap : public QHash<QByteArray, const DefineDirective *>
{

};

class RppTreeEvaluator: public QObject, public RppTreeWalker
{
Q_OBJECT
public:
    RppTreeEvaluator();
    ~RppTreeEvaluator();
    TokenEngine::TokenSectionSequence evaluate(const Source *source,
                                              DefineMap *activedefinitions);
    enum IncludeType {QuoteInclude, AngleBracketInclude};
signals:
    void includeCallback(Rpp::Source *&includee,
                         const Rpp::Source *includer,
                         const QString &filename,
                         Rpp::RppTreeEvaluator::IncludeType includeType);
protected:
    void evaluateIncludeDirective(const IncludeDirective *directive);
    void evaluateDefineDirective(const DefineDirective *directive);
    void evaluateUndefDirective(const UndefDirective *directive);
    void evaluateIfSection(const IfSection *ifSection);
    void evaluateText(const Text *text);
    bool evaluateCondition(const ConditionalDirective *conditionalDirective);
    int evaluateExpression(Expression *expression);

    TokenEngine::TokenContainer evaluateMacro(TokenEngine::TokenContainer tokenContainer, int &identiferTokenIndex);
    TokenEngine::TokenContainer evaluateMacroInternal(QSet<QByteArray> skip, TokenEngine::TokenContainer tokenContainer);
    TokenEngine::TokenContainer cloneTokenList(const TokenEngine::TokenList &list);
    Source *getParentSource(const Item *item) const;
    IncludeType includeTypeFromDirective(
                    const IncludeDirective *includeDirective) const;
private:
    QVector<TokenEngine::TokenSection> m_tokenSections;
    DefineMap *m_activeDefinitions;
    TokenEngine::TokenSection *newlineSection;
};

class MacroFunctionParser
{
public:
    MacroFunctionParser(const TokenEngine::TokenContainer &tokenContainer, int startToken);
    bool isValid();
    int tokenCount();
    int argumentCount();
    TokenEngine::TokenSection argument(int argumentIndex);
private:
    const TokenEngine::TokenContainer &m_tokenContainer;
    const int m_startToken;
    int m_numTokens;
    bool m_valid;
    QVector<TokenEngine::TokenSection> m_arguments;
};

}//namespace Rpp

QT_END_NAMESPACE

#endif
