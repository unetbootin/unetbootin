/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt Linguist of the Qt Toolkit.
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

#include "printout.h"

#include <QPrinter>
#include <QFontMetrics>

QT_BEGIN_NAMESPACE

PrintOut::PrintOut(QPrinter *printer)
    : pr(printer), nextRule(NoRule), page(0)
{
    p.begin(pr);
    QFont f(QLatin1String("Arial"));
    f8 = f;
    f8.setPointSize(8);
    f10 = f;
    f10.setPointSize(10);
    p.setFont(f10);
    fmetrics = new QFontMetrics(p.fontMetrics());
    hmargin = 5 * printer->width() / printer->widthMM(); // 5 mm
    vmargin = 5 * printer->height() / printer->heightMM(); // 5 mm
    hsize = printer->width() - 2 * hmargin;
    vsize = printer->height() - vmargin;
    dateTime = QDateTime::currentDateTime();
    breakPage(true); // init vsize and draw first header
    cp = Paragraph(QPoint(hmargin, voffset));
}

PrintOut::~PrintOut()
{
    flushLine();
    delete fmetrics;
    p.end();
}

void PrintOut::setRule(Rule rule)
{
    if (nextRule < rule)
        nextRule = rule;
}

void PrintOut::setGuide(const QString &guide)
{
    g = guide;
}

void PrintOut::vskip()
{
    if (!firstParagraph)
        voffset += 14;
}

void PrintOut::flushLine(bool /* mayBreak */)
{
    if (voffset + cp.rect.height() > vsize)
        breakPage();
    else if (!firstParagraph)
        drawRule(nextRule);

    for (int i = 0; i < cp.boxes.count(); ++i) {
        Box b = cp.boxes[i];
        b.rect.translate(0, voffset);
        QRect r = b.rect;
        p.setFont(b.font);
        p.drawText(r, b.text, b.options);
    }
    voffset += cp.rect.height();

    nextRule = NoRule;
    cp = Paragraph(QPoint(hmargin, voffset));
    firstParagraph = false;
}

void PrintOut::addBox(int percent, const QString &text, Style style, Qt::Alignment halign)
{
    QTextOption options;
    options.setAlignment(halign | Qt::AlignTop);
    options.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    QFont f = f10;
    if (style == Strong)
        f.setBold(true);
    else if (style == Emphasis)
        f.setItalic(true);
    int wd = hsize * percent / 100;
    QRect r(cp.rect.x() + cp.rect.width(), 0, wd, vsize);
    const int ht = static_cast<int>(p.boundingRect(r, text, options).height());

    Box b(r, text, f, options);
    cp.boxes.append(b);
    cp.rect.setSize(QSize(cp.rect.width() + wd, qMax(cp.rect.height(), ht)));
}

// use init if inital vsize should be calculated (first breakPage call)
void PrintOut::breakPage(bool init)
{
    static const int LeftAlign = Qt::AlignLeft | Qt::AlignTop;
    static const int RightAlign = Qt::AlignRight | Qt::AlignTop;
    QRect r1, r2;
    int h1 = 0;
    int h2 = 0;

    if (page > 0)
        pr->newPage();

    if (!init)
        page++;

    voffset = 0;

    p.setFont(f10);
    r1 = QRect(hmargin, voffset, 3 * hsize / 4, vsize);
    r2 = QRect(r1.x() + r1.width(), voffset, hsize - r1.width(), vsize);
    h1 = p.boundingRect(r1, LeftAlign, pr->docName()).height();
    if (!init)
        p.drawText(r1, LeftAlign, pr->docName());
    h2 = p.boundingRect(r2, RightAlign, QString::number(page)).height();
    if (!init)
        p.drawText(r2, RightAlign, QString::number(page));
    voffset += qMax(h1, h2 );

    r1 = QRect(hmargin, voffset, hsize / 2, LeftAlign);
    p.setFont(f8);
    h1 = p.boundingRect(r1, LeftAlign, dateTime.toString()).height();
    if (!init)
        p.drawText(r1, LeftAlign, dateTime.toString());
    p.setFont(f10);
    voffset += qMax(h1, h2);

    voffset += 4;
    if (!init)
        p.drawLine(QPoint(hmargin, voffset), QPoint(hmargin + hsize, voffset));
    voffset += 14;

    firstParagraph = true;

    if (init) {
        vsize -= voffset;
        breakPage(); // now draw it when the vsize is ok
    }

}

void PrintOut::drawRule(Rule rule)
{
    QPen pen;

    switch (rule) {
    case NoRule:
        voffset += 5;
        break;
    case ThinRule:
        pen.setColor(QColor(192, 192, 192));
        pen.setStyle(Qt::DotLine);
        pen.setWidth(0);
        p.setPen(pen);
        voffset += 5;
        p.drawLine(QPoint(hmargin, voffset),
                   QPoint(hmargin + hsize, voffset));
        p.setPen(QPen());
        voffset += 2;
        break;
    case ThickRule:
        voffset += 7;
        p.drawLine(QPoint(hmargin, voffset),
                   QPoint(hmargin + hsize, voffset));
        voffset += 4;
    }
}

QT_END_NAMESPACE
