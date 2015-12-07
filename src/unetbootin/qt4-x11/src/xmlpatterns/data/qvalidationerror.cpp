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


#include "qvalidationerror_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

ValidationError::ValidationError(const QString &msg,
                                 const ReportContext::ErrorCode code) : m_message(msg),
                                                                        m_code(code)
{
}

AtomicValue::Ptr ValidationError::createError(const QString &description,
                                              const ReportContext::ErrorCode code)
{
    return ValidationError::Ptr(new ValidationError(description, code));
}

bool ValidationError::hasError() const
{
    return true;
}

QString ValidationError::stringValue() const
{
    Q_ASSERT_X(false, Q_FUNC_INFO, "stringValue() asked for ValidationError -- it makes no sense.");
    return QString();
}

QString ValidationError::message() const
{
    return m_message;
}

ItemType::Ptr ValidationError::type() const
{
    Q_ASSERT_X(false, Q_FUNC_INFO,
               "This function should never be called, the caller "
               "didn't check whether the AtomicValue was an ValidationError.");
    return ItemType::Ptr();
}

ReportContext::ErrorCode ValidationError::errorCode() const
{
    return m_code;
}

QT_END_NAMESPACE
