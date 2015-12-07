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

#ifndef Patternist_AccelIterators_H
#define Patternist_AccelIterators_H

#include "qacceltree_p.h"
#include "qitem_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short Abstract base class for Iterators for the AccelTree, that
     * contains common functions and members.
     */
    class AccelIterator : public QXmlNodeModelIndex::Iterator
    {
    public:
        virtual xsInteger position() const;
        virtual QXmlNodeModelIndex current() const;

    protected:
        inline AccelIterator(const AccelTree *const doc,
                             const AccelTree::PreNumber pre,
                             const AccelTree::PreNumber currentPre) : m_document(doc)
                                                                    , m_preNumber(pre)
                                                                    , m_currentPre(currentPre)
                                                                    , m_position(0)

        {
            Q_ASSERT(m_document);
            Q_ASSERT(m_preNumber >= 0);
        }

        inline QXmlNodeModelIndex closedExit()
        {
            m_position = -1;
            m_current.reset();
            return QXmlNodeModelIndex();
        }

        /**
         * We do not own it.
         */
        const AccelTree *const      m_document;

        /**
         * The pre number of the node that should be navigated from.
         */
        const AccelTree::PreNumber  m_preNumber;
        AccelTree::PreNumber        m_currentPre;
        xsInteger                   m_position;
        QXmlNodeModelIndex          m_current;
    };

    template<const bool IncludeSelf>
    class AncestorIterator : public AccelIterator
    {
    public:
        /**
         * @p pre is the node from which iteration starts
         * from. In the @c ancestor axis it is excluded,
         * while in @c ancestor-or-self it is included. @p pre
         * must have at least one ancestor.
         */
        inline AncestorIterator(const AccelTree *const doc,
                                const AccelTree::PreNumber pre) : AccelIterator(doc, pre, IncludeSelf ? pre : doc->basicData.at(pre).parent())
        {
            Q_ASSERT(IncludeSelf || m_document->hasParent(pre));
        }

        virtual QXmlNodeModelIndex next()
        {
            if(m_currentPre == -1)
                return closedExit();
            else
            {
                ++m_position;
                m_current = m_document->createIndex(m_currentPre);
                m_currentPre = m_document->basicData.at(m_currentPre).parent();

                return m_current;
            }
        }

        virtual QXmlNodeModelIndex::Iterator::Ptr copy() const
        {
            return QXmlNodeModelIndex::Iterator::Ptr(new AncestorIterator<IncludeSelf>(m_document, m_preNumber));
        }
    };

    class ChildIterator : public AccelIterator
    {
    public:
        /**
         * @p pre must have at least one child.
         */
        inline ChildIterator(const AccelTree *const doc,
                             const AccelTree::PreNumber pre) : AccelIterator(doc, pre, pre + 1),
                                                               m_depth(m_document->depth(m_currentPre))
        {
            Q_ASSERT(m_document->hasChildren(pre));

            /* Skip the attributes, that are children in the pre/post plane, of
             * the node we're applying the child axis to. */
            while(m_document->kind(m_currentPre) == QXmlNodeModelIndex::Attribute)
            {
                ++m_currentPre;
                /* We check the depth here because we would otherwise include
                 * following siblings. */
                if(m_currentPre > m_document->maximumPreNumber() || m_document->depth(m_currentPre) != m_depth)
                {
                    m_currentPre = -1;
                    break;
                }
            }
        }

        virtual QXmlNodeModelIndex next();
        virtual QXmlNodeModelIndex::Iterator::Ptr copy() const;

    private:
        const AccelTree::Depth m_depth;
    };

    template<const bool IsFollowing>
    class SiblingIterator : public AccelIterator
    {
    public:
        inline SiblingIterator(const AccelTree *const doc,
                               const AccelTree::PreNumber pre) : AccelIterator(doc, pre, pre + (IsFollowing ? 0 : -1)),
                                                                 m_depth(doc->depth(pre))
        {
            Q_ASSERT_X(IsFollowing || pre != 0, "",
                       "When being preceding-sibling, the context node cannot be the first node in the document.");
            Q_ASSERT_X(!IsFollowing || pre != m_document->maximumPreNumber(), "",
                       "When being following-sibling, the context node cannot be the last node in the document.");
        }

        virtual QXmlNodeModelIndex next()
        {
            if(m_currentPre == -1)
                return QXmlNodeModelIndex();

            if(IsFollowing)
            {
                /* Skip the descendants, and jump to the next node. */
                m_currentPre += m_document->size(m_currentPre) + 1;

                if(m_currentPre > m_document->maximumPreNumber() || m_document->depth(m_currentPre) != m_depth)
                    return closedExit();
                else
                {
                    ++m_position;
                    m_current = m_document->createIndex(m_currentPre);
                    return m_current;
                }
            }
            else
            {
                while(m_document->depth(m_currentPre) > m_depth)
                    --m_currentPre;

                while(m_document->kind(m_currentPre) == QXmlNodeModelIndex::Attribute)
                    --m_currentPre;

                if(m_document->depth(m_currentPre) == m_depth &&
                   m_document->kind(m_currentPre) != QXmlNodeModelIndex::Attribute)
                {
                    m_current = m_document->createIndex(m_currentPre);
                    ++m_position;
                    --m_currentPre;
                    return m_current;
                }
                else
                {
                    m_currentPre = -1;
                    return closedExit();
                }
            }
        }

        virtual QXmlNodeModelIndex::Iterator::Ptr copy() const
        {
            return QXmlNodeModelIndex::Iterator::Ptr(new SiblingIterator<IsFollowing>(m_document, m_preNumber));
        }

    private:
        const AccelTree::Depth m_depth;
    };

    /**
     * @short Implements axis @c descendant and @c descendant-or-self for the
     * AccelTree.
     *
     * @author Frans Englich <fenglich@trolltech.com>
     */
    template<const bool IncludeSelf>
    class DescendantIterator : public AccelIterator
    {
    public:
        /**
         * @p pre must have at least one child.
         */
        inline DescendantIterator(const AccelTree *const doc,
                                  const AccelTree::PreNumber pre) : AccelIterator(doc, pre, pre + (IncludeSelf ? 0 : 1)),
                                                                    m_postNumber(doc->postNumber(pre))
        {
            Q_ASSERT(IncludeSelf || m_document->hasChildren(pre));

            /* Make sure that m_currentPre is the first node part of this axis.
             * Since we're not including ourself, advance to the node after our
             * attributes, if any. */
            if(!IncludeSelf)
            {
                while(m_document->kind(m_currentPre) == QXmlNodeModelIndex::Attribute)
                {
                    ++m_currentPre;
                    /* We check the depth here because we would otherwise include
                     * following siblings. */
                    if(m_currentPre > m_document->maximumPreNumber() || m_document->postNumber(m_currentPre) > m_postNumber)
                    {
                        m_currentPre = -1;
                        break;
                    }
                }
            }
        }

        virtual QXmlNodeModelIndex next()
        {
            if(m_currentPre == -1)
                return closedExit();

            ++m_position;
            m_current = m_document->createIndex(m_currentPre);

            ++m_currentPre;

            if(m_currentPre > m_document->maximumPreNumber())
            {
                m_currentPre = -1;
                return m_current;
            }

            if(m_document->postNumber(m_currentPre) < m_postNumber)
            {
                while(m_document->kind(m_currentPre) == QXmlNodeModelIndex::Attribute)
                {
                    ++m_currentPre;
                    if(m_currentPre > m_document->maximumPreNumber())
                    {
                        m_currentPre = -1;
                        break;
                    }
                }
            }
            else
                m_currentPre = -1;

            return m_current;
        }

        virtual QXmlNodeModelIndex::Iterator::Ptr copy() const
        {
            return QXmlNodeModelIndex::Iterator::Ptr(new DescendantIterator<IncludeSelf>(m_document, m_preNumber));
        }

    private:
        const AccelTree::PreNumber m_postNumber;
    };

    /**
     * @short Implements axis @c following for the AccelTree.
     *
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class FollowingIterator : public AccelIterator
    {
    public:
        /**
         * @ pre must have at least one child.
         */
        inline FollowingIterator(const AccelTree *const doc,
                                 const AccelTree::PreNumber pre) : AccelIterator(doc, pre, pre)
        {
        }

        virtual QXmlNodeModelIndex next();
        virtual QXmlNodeModelIndex::Iterator::Ptr copy() const;
    };

    /**
     * @short Implements axis @c preceding for the AccelTree.
     *
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class PrecedingIterator : public AccelIterator
    {
    public:
        /**
         * @ pre must have at least one child.
         */
        inline PrecedingIterator(const AccelTree *const doc,
                                 const AccelTree::PreNumber pre) : AccelIterator(doc, pre,
                                                                                 pre - 1 /* currentPre */)
                                                                 , m_postNumber(m_document->postNumber(m_preNumber))
        {
        }

        virtual QXmlNodeModelIndex next();
        virtual QXmlNodeModelIndex::Iterator::Ptr copy() const;

    private:
        const AccelTree::PreNumber  m_postNumber;
    };

    class AttributeIterator : public AccelIterator
    {
    public:
        /**
         * @p pre must have at least one child.
         */
        inline AttributeIterator(const AccelTree *const doc, const AccelTree::PreNumber pre) : AccelIterator(doc, pre, pre + 1)
        {
            Q_ASSERT(m_document->hasChildren(pre));
            Q_ASSERT(m_document->kind(m_currentPre) == QXmlNodeModelIndex::Attribute);
        }

        virtual QXmlNodeModelIndex next();
        virtual QXmlNodeModelIndex::Iterator::Ptr copy() const;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
