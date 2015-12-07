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

#ifndef Patternist_TemplatePattern_H
#define Patternist_TemplatePattern_H

#include "qtemplate_p.h"

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short Houses the data necessary for a template pattern.
     *
     * A template pattern is the match pattern, but have had each operand to @c
     * | separated out into a separate TemplatePattern. For instance, the
     * pattern <tt>a | b | c</tt>, becomes three separate TemplatePattern
     * instances.
     *
     * @see TemplateMode
     * @see Template
     * @author Frans Englich <fenglich@trolltech.com>
     * @ingroup Patternist_expressions
     * @since 4.5
     */
    class TemplatePattern : public QSharedData
    {
    public:
        typedef QExplicitlySharedDataPointer<TemplatePattern> Ptr;
        typedef QVector<Ptr> Vector;
        typedef int ID;

        inline TemplatePattern(const Expression::Ptr &matchPattern,
                               const PatternPriority pri,
                               const ID id,
                               const Template::Ptr templ);

        inline PatternPriority priority() const;
        inline const Expression::Ptr &matchPattern() const;
        inline void setMatchPattern(const Expression::Ptr &pattern);
        inline const Template::Ptr &templateTarget() const;
        inline ID id() const;

        /**
         * This ID is used to ensure that, as 6.4 Conflict Resolution for
         * Template Rules reads:
         *
         * "If the pattern contains multiple alternatives separated by |, then
         * the template rule is treated equivalently to a set of template
         * rules, one for each alternative. However, it is not an error if a
         * node matches more than one of the alternatives."
         *
         * For patterns separated by @c |, we have one Template instance for
         * each alternative, but they all have the same ID, hence if several
         * alternatives match, we don't flag it as an error if they have the
         * same ID.
         */
    private:
        Expression::Ptr m_matchPattern;
        PatternPriority m_priority;
        ID              m_id;
        Template::Ptr   m_templateTarget;
        Q_DISABLE_COPY(TemplatePattern)
    };

    TemplatePattern::TemplatePattern(const Expression::Ptr &matchPattern,
                                     const PatternPriority pri,
                                     const ID id,
                                     const Template::Ptr templ) : m_matchPattern(matchPattern)
                                                                , m_priority(pri)
                                                                , m_id(id)
                                                                , m_templateTarget(templ)

    {
        Q_ASSERT(m_matchPattern);
        Q_ASSERT(m_templateTarget);
    }

    const Expression::Ptr &TemplatePattern::matchPattern() const
    {
        return m_matchPattern;
    }

    void TemplatePattern::setMatchPattern(const Expression::Ptr &pattern)
    {
        m_matchPattern = pattern;
    }

    PatternPriority TemplatePattern::priority() const
    {
        return m_priority;
    }

    TemplatePattern::ID TemplatePattern::id() const
    {
        return m_id;
    }

    const Template::Ptr &TemplatePattern::templateTarget() const
    {
        return m_templateTarget;
    }

}

QT_END_NAMESPACE

QT_END_HEADER

#endif

