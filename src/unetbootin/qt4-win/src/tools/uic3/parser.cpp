/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the tools applications of the Qt Toolkit.
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
