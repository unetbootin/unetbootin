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

#include "qboolean_p.h"
#include "qcommonvalues_p.h"
#include "qdynamiccontext_p.h"

#include "qexternalvariableloader_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

ExternalVariableLoader::~ExternalVariableLoader()
{
}


SequenceType::Ptr ExternalVariableLoader::announceExternalVariable(const QXmlName name,
                                                                   const SequenceType::Ptr &declaredType)
{
    Q_ASSERT(!name.isNull());
    Q_ASSERT(declaredType);
    Q_UNUSED(name); /* Needed when compiling in release mode. */
    Q_UNUSED(declaredType); /* Needed when compiling in release mode. */

    return SequenceType::Ptr();
}

Item::Iterator::Ptr ExternalVariableLoader::evaluateSequence(const QXmlName name,
                                                             const DynamicContext::Ptr &context)
{
    Q_ASSERT(!name.isNull());
    const Item item(evaluateSingleton(name, context));

    if(item)
        return makeSingletonIterator(item);
    else
        return CommonValues::emptyIterator;
}

Item ExternalVariableLoader::evaluateSingleton(const QXmlName name,
                                                    const DynamicContext::Ptr &context)
{
    Q_ASSERT(!name.isNull());
    return Boolean::fromValue(evaluateEBV(name, context));
}

bool ExternalVariableLoader::evaluateEBV(const QXmlName name,
                                         const DynamicContext::Ptr &context)
{
    Q_ASSERT(!name.isNull());
    return Boolean::evaluateEBV(evaluateSequence(name, context), context);
}

QT_END_NAMESPACE
