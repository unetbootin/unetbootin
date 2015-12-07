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

#ifndef Patternist_TemplateInvoker_H
#define Patternist_TemplateInvoker_H

#include "qcallsite_p.h"
#include "qwithparam_p.h"

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short Base class for classes that invokes @em templates, such as
     * CallTemplate and ApplyTemplate.
     *
     * TemplateInvoker has the member m_withParams, which is the @c
     * xsl:with-param instructions of the caller. The definite source for the
     * expressions is m_withParams, not Expression::operands(). However, the
     * order of operands() is defined, while m_withParams is not since it's a
     * hash. Therefore operands() is definite on order.
     *
     * TemplateInvoker is intended to be sub-classed.
     *
     * @author Frans Englich <fenglich@trolltech.com>
     * @ingroup Patternist_expressions
     * @since 4.5
     */
    class TemplateInvoker : public CallSite
    {
    public:
        virtual Expression::Ptr compress(const StaticContext::Ptr &context);

        inline const WithParam::Hash &withParams() const;
        WithParam::Hash m_withParams;

        /**
         * This is a bit complicated by that we have two required types, one
         * specified by @c xsl:param in the template declaration, and one on @c
         * xsl:with-param.
         *
         * @see UserFunctionCallsite::expectedOperandTypes()
         * @see <a href="http://www.w3.org/TR/xslt20/#with-param">XSL
         * Transformations (XSLT) Version 2.0, 10.1.1 Passing Parameters to Templates</a>
         */
        virtual SequenceType::List expectedOperandTypes() const;
    protected:
        /**
         * @p withParams may be empty.
         */
        TemplateInvoker(const WithParam::Hash &withParams,
                        const QXmlName &name = QXmlName());

    private:
        Q_DISABLE_COPY(TemplateInvoker)
    };

    const WithParam::Hash &TemplateInvoker::withParams() const
    {
        return m_withParams;
    }

}

QT_END_NAMESPACE
QT_END_HEADER

#endif

