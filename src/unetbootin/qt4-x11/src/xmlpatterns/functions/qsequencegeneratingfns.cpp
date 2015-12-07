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

#include <QStack>
#include <QStringList>

#include "qanyuri_p.h"
#include "qboolean_p.h"
#include "qcommonsequencetypes_p.h"
#include "qcommonvalues_p.h"
#include "qemptysequence_p.h"
#include "qitemmappingiterator_p.h"
#include "qnodesort_p.h"
#include "qpatternistlocale_p.h"
#include "private/qxmlutils_p.h"

#include "qsequencegeneratingfns_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

IdFN::IdFN() : m_hasCreatedSorter(false)
{
}

Item IdFN::mapToItem(const QString &id,
                     const IDContext &context) const
{
    return context.second->elementById(context.first->namePool()->allocateQName(QString(), id));
}

/**
 * @short Helper class for StringSplitter
 *
 * Needed by the QAbstractXmlForwardIterator sub-class.
 *
 * @relates StringSplitter
 */
static bool qIsForwardIteratorEnd(const QString &unit)
{
    return unit.isNull();
}

/**
 * @short Helper class for IdFN.
 *
 * StringSplitter takes an Iterator which delivers strings of this kind:
 *
 * "a", "b c", "%invalidNCName", "  ", "d"
 *
 * and we deliver instead:
 *
 * "a", "b", "c", "d"
 *
 * That is, we:
 * - Remove invalid @c NCName
 * - Split IDREFs into individual NCNames
 *
 * @author Frans Englich
 */
class StringSplitter : public QAbstractXmlForwardIterator<QString>
{
public:
    StringSplitter(const Item::Iterator::Ptr &source);
    virtual QString next();
    virtual QString current() const;
    virtual qint64 position() const;
private:
    QString loadNext();
    const Item::Iterator::Ptr   m_source;
    QStack<QString>             m_buffer;
    QString                     m_current;
    qint64                      m_position;
    bool                        m_sourceAtEnd;
};

StringSplitter::StringSplitter(const Item::Iterator::Ptr &source) : m_source(source)
                                                                  , m_position(0)
                                                                  , m_sourceAtEnd(false)
{
    Q_ASSERT(m_source);
    m_buffer.push(loadNext());
}

QString StringSplitter::next()
{
    /* We also check m_position, we want to load on our first run. */
    if(!m_buffer.isEmpty())
    {
        ++m_position;
        m_current = m_buffer.pop();
        return m_current;
    }
    else if(m_sourceAtEnd)
    {
        m_current.clear();
        m_position = -1;
        return QString();
    }

    return loadNext();
}

QString StringSplitter::loadNext()
{
    const Item sourceNext(m_source->next());

    if(sourceNext.isNull())
    {
        m_sourceAtEnd = true;
        /* We might have strings in m_buffer, let's empty it. */
        return next();
    }

    const QStringList candidates(sourceNext.stringValue().simplified().split(QLatin1Char(' ')));
    const int count = candidates.length();

    for(int i = 0; i < count; ++i)
    {
        const QString &at = candidates.at(i);

        if(QXmlUtils::isNCName(at))
            m_buffer.push(at);
    }

    /* So, now we have populated m_buffer, let's start from the beginning. */
    return next();
}

QString StringSplitter::current() const
{
    return m_current;
}

qint64 StringSplitter::position() const
{
    return m_position;
}

Item::Iterator::Ptr IdFN::evaluateSequence(const DynamicContext::Ptr &context) const
{
    const Item::Iterator::Ptr idrefs(m_operands.first()->evaluateSequence(context));
    const Item node(m_operands.last()->evaluateSingleton(context));

    checkTargetNode(node.asNode(), context, ReportContext::FODC0001);

    return makeItemMappingIterator<Item,
                                   QString,
                                   IdFN::ConstPtr,
                                   IDContext>(ConstPtr(this),
                                              StringSplitter::Ptr(new StringSplitter(idrefs)),
                                              qMakePair(context, node.asNode().model()));
}

Expression::Ptr IdFN::typeCheck(const StaticContext::Ptr &context,
                                const SequenceType::Ptr &reqType)
{
    if(m_hasCreatedSorter)
        return FunctionCall::typeCheck(context, reqType);
    else
    {
        const Expression::Ptr newMe(new NodeSortExpression(Expression::Ptr(this)));
        // TODO
        m_hasCreatedSorter = true;
        return newMe->typeCheck(context, reqType);
    }
}

Item::Iterator::Ptr IdrefFN::evaluateSequence(const DynamicContext::Ptr &context) const
{
    const Item::Iterator::Ptr ids(m_operands.first()->evaluateSequence(context));

    Item mId(ids->next());
    if(!mId)
        return CommonValues::emptyIterator;

    const Item node(m_operands.last()->evaluateSingleton(context));
    checkTargetNode(node.asNode(), context, ReportContext::FODC0001);

    return CommonValues::emptyIterator; /* TODO Haven't implemented further. */
}

Item DocFN::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    const Item itemURI(m_operands.first()->evaluateSingleton(context));

    if(!itemURI)
        return Item();

    /* These two lines were previously in a separate function but are now duplicated
     * in DocAvailableFN::evaluateEBV() and DocFN::typeCheck(),
     * as part of a workaround for solaris-cc-64. DocFN::typeCheck() is in qsequencefns.cpp
     * as part of that workaround. */
    const QUrl mayRela(AnyURI::toQUrl<ReportContext::FODC0005>(itemURI.stringValue(), context, this));
    const QUrl uri(context->resolveURI(mayRela, staticBaseURI()));

    Q_ASSERT(uri.isValid());
    Q_ASSERT(!uri.isRelative());

    const Item doc(context->resourceLoader()->openDocument(uri, context));

    return doc;
}

SequenceType::Ptr DocFN::staticType() const
{
    if(m_type)
        return m_type;
    else
        return CommonSequenceTypes::ZeroOrOneDocumentNode;
}

bool DocAvailableFN::evaluateEBV(const DynamicContext::Ptr &context) const
{
    const Item itemURI(m_operands.first()->evaluateSingleton(context));

    /* 15.5.4 fn:doc reads: "If $uri is the empty sequence, the result is an empty sequence."
     * Hence, we return false for the empty sequence, because this doesn't hold true:
     * "If this function returns true, then calling fn:doc($uri) within
     * the same execution scope must return a document node."(15.5.5 fn:doc-available) */
    if(!itemURI)
        return false;

    /* These two lines are duplicated in DocFN::evaluateSingleton(), as part
     * of a workaround for solaris-cc-64. */
    const QUrl mayRela(AnyURI::toQUrl<ReportContext::FODC0005>(itemURI.stringValue(), context, this));
    const QUrl uri(context->resolveURI(mayRela, staticBaseURI()));

    Q_ASSERT(!uri.isRelative());
    return context->resourceLoader()->isDocumentAvailable(uri);
}

Item::Iterator::Ptr CollectionFN::evaluateSequence(const DynamicContext::Ptr &context) const
{
    // TODO resolve with URI resolve
    if(m_operands.isEmpty())
    {
        // TODO check default collection
        context->error(QtXmlPatterns::tr("The default collection is undefined"),
                       ReportContext::FODC0002, this);
        return CommonValues::emptyIterator;
    }
    else
    {
        const Item itemURI(m_operands.first()->evaluateSingleton(context));

        if(itemURI)
        {
            const QUrl uri(AnyURI::toQUrl<ReportContext::FODC0004>(itemURI.stringValue(), context, this));

            // TODO 2. Resolve against static context base URI(store base URI at compile time)
            context->error(QtXmlPatterns::tr("%1 cannot be retrieved").arg(formatResourcePath(uri)),
                           ReportContext::FODC0004, this);
            return CommonValues::emptyIterator;
        }
        else
        {
            /* This is out default collection currently, */
            return CommonValues::emptyIterator;
        }
    }
}

QT_END_NAMESPACE
