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

#include "qcommonsequencetypes_p.h"
#include "qcommonvalues_p.h"
#include "qitemmappingiterator_p.h"
#include "qpatternistlocale_p.h"

#include "qtracefn_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

namespace QPatternist
{
    /**
     * @short TraceCallback is a MappingCallback and takes care of
     * the tracing of each individual item.
     *
     * Because Patternist must be thread safe, TraceFN creates a TraceCallback
     * each time the function is evaluated. In other words, TraceFN, which is
     * an Expression sub class, can't modify its members, but MappingCallback
     * does not have this limitation since it's created on a per evaluation basis.
     *
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class TraceCallback : public QSharedData
    {
    public:
        typedef QExplicitlySharedDataPointer<TraceCallback> Ptr;

        inline TraceCallback(const QString &msg) : m_position(0),
                                                   m_msg(msg)
        {
        }

        /**
         * Performs the actual tracing.
         */
        Item mapToItem(const Item &item,
                            const DynamicContext::Ptr &context)
        {
            QTextStream out(stderr);
            ++m_position;
            if(m_position == 1)
            {
                if(item)
                {
                    out << qPrintable(m_msg)
                        << " : "
                        << qPrintable(item.stringValue());
                }
                else
                {
                    out << qPrintable(m_msg)
                        << " : ("
                        << qPrintable(formatType(context->namePool(), CommonSequenceTypes::Empty))
                        << ")\n";
                    return Item();
                }
            }
            else
            {
                out << qPrintable(item.stringValue())
                    << '['
                    << m_position
                    << "]\n";
            }

            return item;
        }

    private:
        xsInteger m_position;
        const QString m_msg;
    };
}

Item::Iterator::Ptr TraceFN::evaluateSequence(const DynamicContext::Ptr &context) const
{
    const QString msg(m_operands.last()->evaluateSingleton(context).stringValue());

    return makeItemMappingIterator<Item>(TraceCallback::Ptr(new TraceCallback(msg)),
                                              m_operands.first()->evaluateSequence(context),
                                              context);
}

Item TraceFN::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    const QString msg(m_operands.last()->evaluateSingleton(context).stringValue());
    const Item item(m_operands.first()->evaluateSingleton(context));

    return TraceCallback::Ptr(new TraceCallback(msg))->mapToItem(item, context);
}

SequenceType::Ptr TraceFN::staticType() const
{
    return m_operands.first()->staticType();
}

QT_END_NAMESPACE
