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

#include "qpatternistlocale_p.h"

#include "qdocumentcontentvalidator_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

DocumentContentValidator::
DocumentContentValidator(QAbstractXmlReceiver *const receiver,
                         const DynamicContext::Ptr &context,
                         const Expression::ConstPtr &expr) : m_receiver(receiver)
                                                           , m_context(context)
                                                           , m_expr(expr)
                                                           , m_elementDepth(0)
{
    Q_ASSERT(receiver);
    Q_ASSERT(m_expr);
    Q_ASSERT(context);
}

void DocumentContentValidator::namespaceBinding(const QXmlName &nb)
{
    m_receiver->namespaceBinding(nb);
}

void DocumentContentValidator::startElement(const QXmlName &name)
{
    ++m_elementDepth;
    m_receiver->startElement(name);
}

void DocumentContentValidator::endElement()
{
    Q_ASSERT(m_elementDepth > 0);
    --m_elementDepth;
    m_receiver->endElement();
}

void DocumentContentValidator::attribute(const QXmlName &name,
                                         const QStringRef &value)
{
    if(m_elementDepth == 0)
    {
        m_context->error(QtXmlPatterns::tr("An attribute node cannot be a "
                                           "child of a document node. "
                                           "Therefore, the attribute %1 "
                                           "is out of place.")
                         .arg(formatKeyword(m_context->namePool(), name)),
                         ReportContext::XPTY0004, m_expr.data());
    }
    else
        m_receiver->attribute(name, value);
}

void DocumentContentValidator::comment(const QString &value)
{
    m_receiver->comment(value);
}

void DocumentContentValidator::characters(const QStringRef &value)
{
    m_receiver->characters(value);
}

void DocumentContentValidator::processingInstruction(const QXmlName &name,
                                                     const QString &value)
{
    m_receiver->processingInstruction(name, value);
}

void DocumentContentValidator::item(const Item &outputItem)
{
    /* We can't send outputItem directly to m_receiver since its item() function
     * won't dispatch to this DocumentContentValidator, but to itself. We're not sub-classing here,
     * we're delegating. */

    if(outputItem.isNode())
        sendAsNode(outputItem);
    else
        m_receiver->item(outputItem);
}

void DocumentContentValidator::startDocument()
{
    m_receiver->startDocument();
}

void DocumentContentValidator::endDocument()
{
    m_receiver->endDocument();
}

void DocumentContentValidator::atomicValue(const QVariant &value)
{
    Q_UNUSED(value);
}

void DocumentContentValidator::startOfSequence()
{
}

void DocumentContentValidator::endOfSequence()
{
}

QT_END_NAMESPACE
