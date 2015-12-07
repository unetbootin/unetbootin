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

#include "qcommonvalues_p.h"
#include "qcommonsequencetypes_p.h"
#include "qemptysequencetype_p.h"

#include "qemptysequence_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

Expression::Ptr EmptySequence::create(const Expression *const replacementFor,
                                      const StaticContext::Ptr &context)
{
    Q_ASSERT(replacementFor);
    Q_ASSERT(context);

    EmptySequence *const retval = new EmptySequence();
    context->addLocation(retval, context->locationFor(replacementFor));
    return Expression::Ptr(retval);
}

Item::Iterator::Ptr EmptySequence::evaluateSequence(const DynamicContext::Ptr &) const
{
    return CommonValues::emptyIterator;
}

Item EmptySequence::evaluateSingleton(const DynamicContext::Ptr &) const
{
    return Item();
}

void EmptySequence::evaluateToSequenceReceiver(const DynamicContext::Ptr &) const
{
}

ItemType::Ptr EmptySequence::type() const
{
    return CommonSequenceTypes::Empty;
}

SequenceType::Ptr EmptySequence::staticType() const
{
    return CommonSequenceTypes::Empty;
}

bool EmptySequence::evaluateEBV(const DynamicContext::Ptr &) const
{
    return false;
}

QString EmptySequence::stringValue() const
{
    return QString();
}

ExpressionVisitorResult::Ptr EmptySequence::accept(const ExpressionVisitor::Ptr &visitor) const
{
    return visitor->visit(this);
}

Expression::ID EmptySequence::id() const
{
    return IDEmptySequence;
}

Expression::Properties EmptySequence::properties() const
{
    return IsEvaluated;
}

QT_END_NAMESPACE
