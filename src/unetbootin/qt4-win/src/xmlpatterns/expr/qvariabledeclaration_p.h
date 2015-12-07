/****************************************************************************
**
** Copyright (C) 2006-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtXMLPatterns module of the Qt Toolkit.
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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef Patternist_VariableDeclaration_H
#define Patternist_VariableDeclaration_H

#include <QSharedData>

#include "qexpression_p.h"
#include "qpatternistlocale_p.h"
#include "qvariablereference_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

template<typename T> class QStack;

namespace QPatternist
{
    /**
     * @short Represents a declared variable. Only used at
     * the compilation stage.
     *
     * @author Frans Englich <fenglich@trolltech.com>
     * @ingroup Patternist_expressions
     */
    class VariableDeclaration : public QSharedData
    {
    public:
        typedef QExplicitlySharedDataPointer<VariableDeclaration> Ptr;
        typedef QStack<VariableDeclaration::Ptr> Stack;
        typedef QList<VariableDeclaration::Ptr> List;

        enum Type
        {
            RangeVariable,
            ExpressionVariable,
            FunctionArgument,
            PositionalVariable,

            /**
             * A global variable is always an external variable, but it is
             * cached differently.
             *
             * @see DynamicContext::globalItemCacheCell()
             */
            GlobalVariable,

            /**
             * External variables doesn't use slots, that's a big difference
             * compared to the other types.
             */
            ExternalVariable
        };

        /**
         * Creates a VariableDeclaration.
         *
         * @p sourceExpr and @p seqType may be @c null.
         */
        VariableDeclaration(const QXmlName n,
                            const VariableSlotID varSlot,
                            const Type t,
                            const SequenceType::Ptr &seqType) : name(n)
                                                              , slot(varSlot)
                                                              , type(t)
                                                              , sequenceType(seqType)
                                                              , canSourceRewrite(true)
        {
            Q_ASSERT(!name.isNull());
            Q_ASSERT(t == ExternalVariable || varSlot > -1);
        }

        inline bool isUsed() const
        {
            return !references.isEmpty();
        }

        inline const Expression::Ptr &expression() const
        {
            return m_expression;
        }

        inline void setExpression(const Expression::Ptr &expr)
        {
            m_expression = expr;
        }

        /**
         * @short Returns how many times this variable is used.
         */
        inline bool usedByMany() const
        {
            return references.count() > 1;
        }

        const QXmlName                  name;
        const VariableSlotID            slot;
        const Type                      type;
        const SequenceType::Ptr         sequenceType;
        VariableReference::List         references;

        /**
         * @short Whether a reference can rewrite itself to expression().
         *
         * The default value is @c true.
         */
        bool canSourceRewrite;

    private:
        Expression::Ptr                 m_expression;
        Q_DISABLE_COPY(VariableDeclaration)
    };

    /**
     * @short Formats @p var appropriately for display.
     *
     * @relates VariableDeclaration
     */
    static inline QString formatKeyword(const VariableDeclaration::Ptr &var,
                                        const NamePool::Ptr &np)
    {
        Q_ASSERT(var);
        Q_ASSERT(np);
        return formatKeyword(np->displayName(var->name));
    }

}

QT_END_NAMESPACE

QT_END_HEADER

#endif
