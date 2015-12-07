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

#ifndef Patternist_ProjectedExpression_H
#define Patternist_ProjectedExpression_H

#include "qitem_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    class ProjectedExpression
    {
    public:
        typedef ProjectedExpression * Ptr;
        typedef QVector<ProjectedExpression::Ptr> Vector;
        virtual ~ProjectedExpression()
        {
        }

        enum Action
        {
            Move = 0,
            Skip = 1,
            Keep = 2,
            KeepSubtree = 4 | Keep
        };

        virtual Action actionForElement(const QXmlName name,
                                        ProjectedExpression::Ptr &next) const
        {
            Q_UNUSED(name);
            Q_UNUSED(next);
            return Skip;
        }

    };

    class ProjectedNodeTest
    {
    public:
        typedef ProjectedNodeTest * Ptr;
        virtual ~ProjectedNodeTest()
        {
        }

        virtual bool isMatch(const QXmlNodeModelIndex::NodeKind kind) const
        {
            Q_UNUSED(kind);
            return false;
        }
    };

    class ProjectedStep : public ProjectedExpression
    {
    public:
        ProjectedStep(const ProjectedNodeTest::Ptr test,
                      const QXmlNodeModelIndex::Axis axis) : m_test(test),
                                               m_axis(axis)
        {
            Q_ASSERT(m_test);
        }

        virtual Action actionForElement(const QXmlName name,
                                        ProjectedExpression::Ptr &next) const
        {
            Q_UNUSED(name);
            Q_UNUSED(next);
            // TODO
            return Skip;
        }

    private:
        const ProjectedNodeTest::Ptr    m_test;
        const QXmlNodeModelIndex::Axis                m_axis;
    };

    class ProjectedPath : public ProjectedExpression
    {
    public:
        ProjectedPath(const ProjectedExpression::Ptr left,
                      const ProjectedExpression::Ptr right) : m_left(left),
                                                              m_right(right)
        {
            Q_ASSERT(m_left);
            Q_ASSERT(m_right);
        }

        virtual Action actionForElement(const QXmlName name,
                                        ProjectedExpression::Ptr &next) const
        {
            ProjectedExpression::Ptr &candidateNext = next;
            const Action a = m_left->actionForElement(name, candidateNext);

            if(a != Skip)
            {
                /* The test accepted it, so let's replace us with the new step. */
                next = candidateNext;
            }

            return a;
        }

    private:
        const ProjectedExpression::Ptr  m_left;
        const ProjectedExpression::Ptr  m_right;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
