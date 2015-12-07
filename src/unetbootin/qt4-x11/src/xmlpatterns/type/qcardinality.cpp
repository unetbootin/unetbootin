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

#include "qpatternistlocale_p.h"

#include "qcardinality_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

QString Cardinality::displayName(const CustomizeDisplayName explain) const
{
    if(explain == IncludeExplanation)
    {
        if(isEmpty())
            return QString(QtXmlPatterns::tr("empty") + QLatin1String("(\"empty-sequence()\")"));
        else if(isZeroOrOne())
            return QString(QtXmlPatterns::tr("zero or one") + QLatin1String("(\"?\")"));
        else if(isExactlyOne())
            return QString(QtXmlPatterns::tr("exactly one"));
        else if(isOneOrMore())
            return QString(QtXmlPatterns::tr("one or more") + QLatin1String("(\"+\")"));
        else
            return QString(QtXmlPatterns::tr("zero or more") + QLatin1String("(\"*\")"));
    }
    else
    {
        Q_ASSERT(explain == ExcludeExplanation);

        if(isEmpty() || isZeroOrOne())
            return QLatin1String("?");
        else if(isExactlyOne())
            return QString();
        else if(isExact())
        {
            return QString(QLatin1Char('{'))    +
                   QString::number(maximum())   +
                   QLatin1Char('}');
        }
        else
        {
            if(m_max == -1)
            {
                if(isOneOrMore())
                    return QChar::fromLatin1('+');
                else
                    return QChar::fromLatin1('*');
            }
            else
            {
                /* We have a range. We use a RegExp-like syntax. */
                return QString(QLatin1Char('{'))    +
                       QString::number(minimum())   +
                       QLatin1String(", ")          +
                       QString::number(maximum())   +
                       QLatin1Char('}');

            }
        }
    }
}

QT_END_NAMESPACE
