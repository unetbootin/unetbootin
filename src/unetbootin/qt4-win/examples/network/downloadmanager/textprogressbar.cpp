/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
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

#include "textprogressbar.h"
#include <QByteArray>
#include <stdio.h>

TextProgressBar::TextProgressBar()
    : value(0), maximum(-1), iteration(0)
{
}

void TextProgressBar::clear()
{
    printf("\n");
    fflush(stdout);

    iteration = 0;
    value = 0;
    maximum = -1;
}

void TextProgressBar::update()
{
    ++iteration;

    if (maximum > 0) {
        // we know the maximum
        // draw a progress bar
        int percent = value * 100 / maximum;
        int hashes = percent / 2;

        QByteArray progressbar(hashes, '#');
        if (percent % 2)
            progressbar += '>';

        printf("\r[%-50s] %3d%% %s     ",
               progressbar.constData(),
               percent,
               qPrintable(message));
    } else {
        // we don't know the maximum, so we can't draw a progress bar
        int center = (iteration % 48) + 1; // 50 spaces, minus 2
        QByteArray before(qMax(center - 2, 0), ' ');
        QByteArray after(qMin(center + 2, 50), ' ');

        printf("\r[%s###%s]      %s      ",
               before.constData(), after.constData(), qPrintable(message));
    }
}

void TextProgressBar::setMessage(const QString &m)
{
    message = m;
}

void TextProgressBar::setStatus(qint64 val, qint64 max)
{
    value = val;
    maximum = max;
}
