/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtXMLPatterns module of the Qt Toolkit.
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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef Patternist_TokenSource_H
#define Patternist_TokenSource_H

#include "qatomiccomparator_p.h"
#include "qatomicmathematician_p.h"
#include "qcombinenodes_p.h"
#include "qfunctionargument_p.h"
#include "qitem_p.h"
#include "qitemtype_p.h"
#include "qorderby_p.h"
#include "qpath_p.h"
#include "qquerytransformparser_p.h"
#include "qtokensource_p.h"
#include "qvalidate_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

template<typename T> class QQueue;

namespace QPatternist
{
    /**
     * @short A union of all the enums the parser uses.
     */
    union EnumUnion
    {
        AtomicComparator::Operator              valueOperator;
        AtomicMathematician::Operator           mathOperator;
        CombineNodes::Operator                  combinedNodeOp;
        QXmlNodeModelIndex::Axis                axis;
        QXmlNodeModelIndex::DocumentOrder       nodeOperator;
        StaticContext::BoundarySpacePolicy      boundarySpacePolicy;
        StaticContext::ConstructionMode         constructionMode;
        StaticContext::OrderingEmptySequence    orderingEmptySequence;
        StaticContext::OrderingMode             orderingMode;
        OrderBy::OrderSpec::Direction           sortDirection;
        Validate::Mode                          validationMode;
        VariableSlotID                          slot;
        int                                     tokenizerPosition;
        qint16                                  zeroer;
        bool                                    Bool;
        xsDouble                                Double;
        Path::Kind                              pathKind;
    };

    /**
     * @short Base class for components that needs to return tokens.
     *
     * TokenSource represents a stream of Token instances. The end
     * is reached when readNext() returns a Token constructed with
     * END_OF_FILE.
     *
     * @see <a href="http://www.w3.org/TR/xquery-xpath-parsing/">Building a
     * Tokenizer for XPath or XQuery</a>
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class TokenSource : public QSharedData
    {
    public:
        /**
         * typedef for the enum Bison generates that contains
         * the token symbols.
         */
        typedef yytokentype TokenType;

        /**
         * Represents a token by carrying its name and value.
         */
        class Token
        {
        public:
            /**
             * Constructs an invalid Token. This default constructor
             * is need in Qt's container classes.
             */
            inline Token() {}
            inline Token(const TokenType t) : type(t) {}
            inline Token(const TokenType t, const QString &val) : type(t), value(val) {}

            bool hasError() const
            {
                return type == ERROR;
            }

            TokenType type;
            QString value;
        };

        typedef QExplicitlySharedDataPointer<TokenSource> Ptr;
        typedef QQueue<Ptr> Queue;

        /**
         * The C++ compiler cannot synthesize it when we use the
         * Q_DISABLE_COPY() macro.
         */
        inline TokenSource()
        {
        }

        virtual ~TokenSource();

        /**
         * @returns the next token.
         */

            virtual Token nextToken(YYLTYPE *const sourceLocator) = 0;

    private:
        Q_DISABLE_COPY(TokenSource)
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
