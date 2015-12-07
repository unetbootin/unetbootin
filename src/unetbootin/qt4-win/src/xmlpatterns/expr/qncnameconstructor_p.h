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
         * Expands @p lexicalTarget, which is a lexical representation of a QName such as "x:body", into
         * a SharedQName using @p nsResolver to supply the namespace bindings.
         *
         * If @p lexicalTarget is lexically invalid @p InvalidQName is raised via @p context, or if
         * no namespace binding does not exists for a prefix(if any) in @p lexicalTarget, @p NoBinding
         * is raised via @p context.
         *
         * @p nsResolver is paramterized meaning the function can be instantiated with either
         * DynamicContext or StaticContext.
         */
        template<typename TReportContext,
                 const ReportContext::ErrorCode NameIsXML,
                 const ReportContext::ErrorCode LexicallyInvalid>
        static inline
        QString validateTargetName(const QString &lexicalQName,
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
    QString NCNameConstructor::validateTargetName(const QString &lexicalTarget,
                                                  const TReportContext &context,
                                                  const SourceLocationReflection *const r)
    {
        Q_ASSERT(context);

        if(QXmlUtils::isNCName(lexicalTarget))
        {
            if(QString::compare(QLatin1String("xml"), lexicalTarget, Qt::CaseInsensitive) == 0)
            {
                context->error(nameIsXML(lexicalTarget), NameIsXML, r);
                return QString();
            }
            else
                return lexicalTarget;
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
            return QString();
        }
    }
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
