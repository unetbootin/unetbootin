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

#include <QUrl>

#include "qanyuri_p.h"
#include "qliteral_p.h"
#include "qpatternistlocale_p.h"
#include "qatomicstring_p.h"

#include "qresolveurifn_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

Item ResolveURIFN::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    const Item relItem(m_operands.first()->evaluateSingleton(context));

    if(relItem)
    {
        const QString base(m_operands.last()->evaluateSingleton(context).stringValue());
        const QString relative(relItem.stringValue());

        const QUrl baseURI(AnyURI::toQUrl<ReportContext::FORG0002, DynamicContext::Ptr>(base, context, this));
        const QUrl relativeURI(AnyURI::toQUrl<ReportContext::FORG0002, DynamicContext::Ptr>(relative, context, this));

        return toItem(AnyURI::fromValue(baseURI.resolved(relativeURI)));
    }
    else
        return Item();
}

Expression::Ptr ResolveURIFN::typeCheck(const StaticContext::Ptr &context,
                                        const SequenceType::Ptr &reqType)
{
    Q_ASSERT(m_operands.count() == 1 || m_operands.count() == 2);

    if(m_operands.count() == 1)
    {
        /* Our base URI is always well-defined. */
        m_operands.append(wrapLiteral(toItem(AnyURI::fromValue(context->baseURI())), context, this));
    }

    return FunctionCall::typeCheck(context, reqType);
}

QT_END_NAMESPACE
