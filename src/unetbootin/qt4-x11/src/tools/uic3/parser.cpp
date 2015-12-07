/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
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

#include "parser.h"
#include <QObject>
#include <QStringList>

QT_BEGIN_NAMESPACE

QString Parser::cleanArgs(const QString &func)
{
    QString slot(func);
    int begin = slot.indexOf(QLatin1String("(")) + 1;
    QString args = slot.mid(begin);
    args = args.left(args.indexOf(QLatin1String(")")));
    QStringList lst = args.split(QLatin1Char(','));
    QString res = slot.left(begin);
    for (QStringList::Iterator it = lst.begin(); it != lst.end(); ++it) {
        if (it != lst.begin())
            res += QLatin1String(",");
        QString arg = *it;
        int pos = 0;
        if ((pos = arg.indexOf(QLatin1Char('&'))) != -1) {
            arg = arg.left(pos + 1);
        } else if ((pos = arg.indexOf(QLatin1Char('*'))) != -1) {
            arg = arg.left(pos + 1);
        } else {
            arg = arg.simplified();
            if ((pos = arg.indexOf(QLatin1Char(':'))) != -1)
                arg = arg.left(pos).simplified() + QLatin1String(":") + arg.mid(pos + 1).simplified();
            QStringList l = arg.split(QLatin1Char(' '));
            if (l.count() == 2) {
                if (l[0] != QLatin1String("const")
                        && l[0] != QLatin1String("unsigned")
                        && l[0] != QLatin1String("var"))
                    arg = l[0];
            } else if (l.count() == 3) {
                arg = l[0] + QLatin1String(" ") + l[1];
            }
        }
        res += arg;
    }
    res += QLatin1String(")");
    return res;
}

QT_END_NAMESPACE
