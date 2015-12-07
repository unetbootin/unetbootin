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

#include "qdynamiccontext_p.h"
#include "qvalidationerror_p.h"
#include "qitem_p.h"

#include "qanyuri_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

AnyURI::AnyURI(const QString &s) : AtomicString(s)
{
}

AnyURI::Ptr AnyURI::fromValue(const QString &value)
{
    return AnyURI::Ptr(new AnyURI(value));
}

AnyURI::Ptr AnyURI::fromValue(const QUrl &uri)
{
    return AnyURI::Ptr(new AnyURI(uri.toString()));
}

AnyURI::Ptr AnyURI::resolveURI(const QString &relative,
                               const QString &base)
{
    const QUrl urlBase(base);
    return AnyURI::fromValue(urlBase.resolved(relative).toString());
}

ItemType::Ptr AnyURI::type() const
{
    return BuiltinTypes::xsAnyURI;
}

AnyURI::Ptr AnyURI::fromLexical(const QString &value)
{
    bool isValid;
    /* The error code doesn't matter, because we never raise error. */
    const QUrl retval(toQUrl<ReportContext::FORG0001>(value,
                                                      DynamicContext::Ptr(),
                                                      0,
                                                      &isValid,
                                                      false));
    if(isValid)
        return fromValue(retval);
    else
        return ValidationError::createError();
}

bool AnyURI::isValid(const QString &candidate)
{
    bool isOk = false;
    /* The error code doesn't matter, because we never raise error. */
    toQUrl<ReportContext::FORG0001>(candidate,
                                    ReportContext::Ptr(),
                                    0,
                                    &isOk,
                                    false);
    return isOk;
}
QT_END_NAMESPACE
