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

#ifndef Patternist_CopyOf_H
#define Patternist_CopyOf_H

#include "qsinglecontainer_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short Does node copying in a parameterized way, in order to deal with
     * namespace preservation/inheritance.
     *
     * If someone tries to call evaluateEBV(), evaluateSingleton() or
     * evaluateSequence() on us, we will infinitely loop. But apparently
     * that's not possible because we're always a child of ElementConstructor,
     * currently.
     *
     * @author Frans Englich <fenglich@trolltech.com>
     * @ingroup Patternist_expressions
     */
    class CopyOf : public SingleContainer
    {
    public:
        /**
         * Creats a CopyOf where it is checked that the expression @p operand conforms
         * to the type @p reqType.
         */
        CopyOf(const Expression::Ptr &operand,
               const bool inheritNSS,
               const bool preserveNSS);

        virtual void evaluateToSequenceReceiver(const DynamicContext::Ptr &context) const;

        /**
         * @returns always the SequenceType passed in the constructor to this class. That is, the
         * SequenceType that the operand must conform to.
         */
        virtual SequenceType::Ptr staticType() const;

        /**
         * @returns a list containing one CommonSequenceTypes::ZeroOrMoreItems
         */
        virtual SequenceType::List expectedOperandTypes() const;

        virtual ExpressionVisitorResult::Ptr accept(const ExpressionVisitor::Ptr &visitor) const;

        inline Item mapToItem(const Item &source,
                              const DynamicContext::Ptr &context) const;

        virtual Expression::Ptr compress(const StaticContext::Ptr &context);

        virtual Properties properties() const;
        virtual ItemType::Ptr expectedContextItemType() const;

    private:
        typedef QExplicitlySharedDataPointer<const CopyOf> ConstPtr;
        const bool                                      m_inheritNamespaces;
        const bool                                      m_preserveNamespaces;
        const QAbstractXmlNodeModel::NodeCopySettings   m_settings;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
