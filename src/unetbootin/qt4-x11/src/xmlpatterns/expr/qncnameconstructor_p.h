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

#ifndef Patternist_NCNameConstructor_H
#define Patternist_NCNameConstructor_H

#include "qsinglecontainer_p.h"
#include "qpatternistlocale_p.h"
#include "private/qxmlutils_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short Ensures the lexical space of the string value of the Item returned
     * from its child Expression is an NCName.
     *
     * @note It doesn't actually construct an @c xs:NCName. It only ensures the lexical
     * space is an @c NCName. The atomic value can be of any string type, such as @c xs:untypedAtomic
     * of @c xs:string.
     *
     * @author Frans Englich <fenglich@trolltech.com>
     * @ingroup Patternist_expressions
     */
    class NCNameConstructor : public SingleContainer
    {
    public:

        NCNameConstructor(const Expression::Ptr &source);

        virtual Item evaluateSingleton(const DynamicContext::Ptr &) const;

        virtual SequenceType::List expectedOperandTypes() const;

        virtual Expression::Ptr typeCheck(const StaticContext::Ptr &context,
                                          const SequenceType::Ptr &reqType);

        virtual SequenceType::Ptr staticType() const;

        virtual ExpressionVisitorResult::Ptr accept(const ExpressionVisitor::Ptr &visitor) const;

        /**
         *  Validates @p lexicalNCName as a processing instruction's target
         *  name, and raise an error if it's not an @c  NCName.
         */
        template<typename TReportContext,
                 const ReportContext::ErrorCode NameIsXML,
                 const ReportContext::ErrorCode LexicallyInvalid>
        static inline
        void validateTargetName(const QString &lexicalNCName,
                                const TReportContext &context,
                                const SourceLocationReflection *const r);
    private:

        /**
         * This translation string is put here in order to avoid duplicate messages and
         * hence reduce work for translators and increase consistency.
         */
        static
        const QString nameIsXML(const QString &lexTarget)
        {
            return QtXmlPatterns::tr("The target name in a processing instruction "
                                     "cannot be %1 in any combination of upper "
                                     "and lower case. Therefore, is %2 invalid.")
                .arg(formatKeyword("xml"), formatKeyword(lexTarget));
        }
    };

    template<typename TReportContext,
             const ReportContext::ErrorCode NameIsXML,
             const ReportContext::ErrorCode LexicallyInvalid>
    inline
    void NCNameConstructor::validateTargetName(const QString &lexicalTarget,
                                               const TReportContext &context,
                                               const SourceLocationReflection *const r)
    {
        Q_ASSERT(context);

        if(QXmlUtils::isNCName(lexicalTarget))
        {
            if(QString::compare(QLatin1String("xml"), lexicalTarget, Qt::CaseInsensitive) == 0)
                context->error(nameIsXML(lexicalTarget), NameIsXML, r);
        }
        else
        {
            context->error(QtXmlPatterns::tr("%1 is not a valid target name in "
                                             "a processing instruction. It "
                                             "must be a %2 value, e.g. %3.")
                           .arg(formatKeyword(lexicalTarget))
                           .arg(formatType(context->namePool(),
                                           BuiltinTypes::xsNCName))
                           .arg(formatKeyword("my-name.123")),
                           LexicallyInvalid,
                           r);
        }
    }
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
