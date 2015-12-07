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

#ifndef Patternist_Template_H
#define Patternist_Template_H

#include <QSharedData>
#include <QVector>

#include "qdynamiccontext_p.h"
#include "qexpression_p.h"
#include "qsourcelocationreflection_p.h"
#include "qtemplateinvoker_p.h"
#include "qvariabledeclaration_p.h"

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short Contains data related to a template.
     *
     * A Template is associated with a mode, by being housed
     * inside a TemplateMode instance.
     *
     * Template has role very similar to UserFunction.
     *
     * @see TemplateMode
     * @see TemplatePattern
     * @see UserFunction
     * @author Frans Englich <fenglich@trolltech.com>
     * @ingroup Patternist_expressions
     * @since 4.5
     */
    class Template : public QSharedData
                   , public SourceLocationReflection

    {
    public:
        typedef QExplicitlySharedDataPointer<Template> Ptr;
        typedef QVector<Template::Ptr> Vector;

        inline Template(const ImportPrecedence ip,
                        const SequenceType::Ptr &reqType) : importPrecedence(ip)
                                                          , m_reqType(reqType)
        {
        }

        Expression::Ptr body;

        /**
         * Returns @c this.
         */
        virtual const SourceLocationReflection* actualReflection() const;

        const ImportPrecedence importPrecedence;

        VariableDeclaration::List templateParameters;

        /**
         * If @p isCallTemplate, the caller is @c xsl:call-template, as opposed
         * to for instance @c xsl:apply-templates. This affects error
         * reporting.
         */
        DynamicContext::Ptr createContext(const TemplateInvoker *const invoker,
                                         const DynamicContext::Ptr &context,
                                         const bool isCallTemplate) const;

        /**
         * Since we have our template parameters in templateParameters, we need
         * this separate step to do the regular phases:
         * Expression::typeCheck(), and Expression::compress().
         */
        void compileParameters(const StaticContext::Ptr &context);

        /**
         * A value which takes into account the body and its template
         * parameters.
         */
        Expression::Properties properties() const;

        Expression::Properties dependencies() const;

        static void raiseXTSE0680(const ReportContext::Ptr &context,
                                  const QXmlName &name,
                                  const SourceLocationReflection *const reflection);

    private:
        DynamicContext::TemplateParameterHash parametersAsHash() const;
        const SequenceType::Ptr m_reqType;
    };
}

QT_END_NAMESPACE
QT_END_HEADER

#endif
