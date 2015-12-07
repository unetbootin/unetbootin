/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt3Support module of the Qt Toolkit.
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

#include "q3richtext_p.h"

#ifndef QT_NO_RICHTEXT

#include "qbitmap.h"
#include "qapplication.h"
#include "q3cleanuphandler.h"
#include "qcursor.h"
#include "qdatastream.h"
#include "q3dragobject.h"
#include "qdrawutil.h"
#include "qfile.h"
#include "qfileinfo.h"
#include "qfont.h"
#include "qimage.h"
#include "qmap.h"
#include "qmime.h"
#include "q3paintdevicemetrics.h"
#include "qpainter.h"
#include "qstringlist.h"
#include "qstyle.h"
#include "qstyleoption.h"
#include "q3stylesheet.h"
#include "qtextstream.h"
#include <private/qtextengine_p.h>
#include <private/qunicodetables_p.h>

#include <stdlib.h>

#if defined(Q_WS_X11)
#include "qx11info_x11.h"
#endif

QT_BEGIN_NAMESPACE

static Q3TextCursor* richTextExportStart = 0;
static Q3TextCursor* richTextExportEnd = 0;

class Q3TextFormatCollection;

const int border_tolerance = 2;

#ifdef Q_WS_WIN
QT_BEGIN_INCLUDE_NAMESPACE
#include "qt_windows.h"
QT_END_INCLUDE_NAMESPACE
#endif

static inline bool is_printer(QPainter *p)
{
    if (!p || !p->device())
        return false;
    return p->device()->devType() == QInternal::Printer;
}

static inline int scale(int value, QPainter *painter)
{
    if (is_printer(painter)) {
        Q3PaintDeviceMetrics metrics(painter->device());
#if defined(Q_WS_X11)
        value = value * metrics.logicalDpiY() /
                QX11Info::appDpiY(painter->device()->x11Screen());
#elif defined (Q_WS_WIN)
        HDC hdc = GetDC(0);
        int gdc = GetDeviceCaps(hdc, LOGPIXELSY);
        if (gdc)
            value = value * metrics.logicalDpiY() / gdc;
        ReleaseDC(0, hdc);
#elif defined (Q_WS_MAC)
        value = value * metrics.logicalDpiY() / 75; // ##### FIXME
#elif defined (Q_WS_QWS)
        value = value * metrics.logicalDpiY() / 75;
#endif
    }
    return value;
}


static inline bool isBreakable(Q3TextString *string, int pos)
{
    if (string->at(pos).nobreak)
        return false;
    return (pos < string->length()-1 && string->at(pos+1).softBreak);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void Q3TextCommandHistory::addCommand(Q3TextCommand *cmd)
{
    if (current < history.count() - 1) {
        QList<Q3TextCommand *> commands;

        for (int i = 0; i <= current; ++i)
            commands.insert(i, history.takeFirst());

        commands.append(cmd);
        while (!history.isEmpty())
            delete history.takeFirst();
        history = commands;
    } else {
        history.append(cmd);
    }

    if (history.count() > steps)
        delete history.takeFirst();
    else
        ++current;
}

Q3TextCursor *Q3TextCommandHistory::undo(Q3TextCursor *c)
{
    if (current > -1) {
        Q3TextCursor *c2 = history.at(current)->unexecute(c);
        --current;
        return c2;
    }
    return 0;
}

Q3TextCursor *Q3TextCommandHistory::redo(Q3TextCursor *c)
{
    if (current > -1) {
        if (current < history.count() - 1) {
            ++current;
            return history.at(current)->execute(c);
        }
    } else {
        if (history.count() > 0) {
            ++current;
            return history.at(current)->execute(c);
        }
    }
    return 0;
}

bool Q3TextCommandHistory::isUndoAvailable()
{
    return current > -1;
}

bool Q3TextCommandHistory::isRedoAvailable()
{
   return (current > -1 && current < history.count() - 1) || (current == -1 && history.count() > 0);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Q3TextDeleteCommand::Q3TextDeleteCommand(Q3TextDocument *dc, int i, int idx, const QVector<Q3TextStringChar> &str,
                                        const QByteArray& oldStyleInfo)
    : Q3TextCommand(dc), id(i), index(idx), parag(0), text(str), styleInformation(oldStyleInfo)
{
    for (int j = 0; j < (int)text.size(); ++j) {
        if (text[j].format())
            text[j].format()->addRef();
    }
}

Q3TextDeleteCommand::Q3TextDeleteCommand(Q3TextParagraph *p, int idx, const QVector<Q3TextStringChar> &str)
    : Q3TextCommand(0), id(-1), index(idx), parag(p), text(str)
{
    for (int i = 0; i < (int)text.size(); ++i) {
        if (text[i].format())
            text[i].format()->addRef();
    }
}

Q3TextDeleteCommand::~Q3TextDeleteCommand()
{
    for (int i = 0; i < (int)text.size(); ++i) {
        if (text[i].format())
            text[i].format()->removeRef();
    }
    text.resize(0);
}

Q3TextCursor *Q3TextDeleteCommand::execute(Q3TextCursor *c)
{
    Q3TextParagraph *s = doc ? doc->paragAt(id) : parag;
    if (!s) {
        qWarning("can't locate parag at %d, last parag: %d", id, doc->lastParagraph()->paragId());
        return 0;
    }

    cursor.setParagraph(s);
    cursor.setIndex(index);
    int len = text.size();
    if (c)
        *c = cursor;
    if (doc) {
        doc->setSelectionStart(Q3TextDocument::Temp, cursor);
        for (int i = 0; i < len; ++i)
            cursor.gotoNextLetter();
        doc->setSelectionEnd(Q3TextDocument::Temp, cursor);
        doc->removeSelectedText(Q3TextDocument::Temp, &cursor);
        if (c)
            *c = cursor;
    } else {
        s->remove(index, len);
    }

    return c;
}

Q3TextCursor *Q3TextDeleteCommand::unexecute(Q3TextCursor *c)
{
    Q3TextParagraph *s = doc ? doc->paragAt(id) : parag;
    if (!s) {
        qWarning("can't locate parag at %d, last parag: %d", id, doc->lastParagraph()->paragId());
        return 0;
    }

    cursor.setParagraph(s);
    cursor.setIndex(index);
    QString str = Q3TextString::toString(text);
    cursor.insert(str, true, &text);
    if (c)
        *c = cursor;
    cursor.setParagraph(s);
    cursor.setIndex(index);

#ifndef QT_NO_DATASTREAM
    if (!styleInformation.isEmpty()) {
        QDataStream styleStream(&styleInformation, IO_ReadOnly);
        int num;
        styleStream >> num;
        Q3TextParagraph *p = s;
        while (num-- && p) {
            p->readStyleInformation(styleStream);
            p = p->next();
        }
    }
#endif
    s = cursor.paragraph();
    while (s) {
        s->format();
        s->setChanged(true);
        if (s == c->paragraph())
            break;
        s = s->next();
    }

    return &cursor;
}

Q3TextFormatCommand::Q3TextFormatCommand(Q3TextDocument *dc, int sid, int sidx, int eid, int eidx,
                                        const QVector<Q3TextStringChar> &old, Q3TextFormat *f, int fl)
    : Q3TextCommand(dc), startId(sid), startIndex(sidx), endId(eid), endIndex(eidx), format(f), oldFormats(old), flags(fl)
{
    format = dc->formatCollection()->format(f);
    for (int j = 0; j < (int)oldFormats.size(); ++j) {
        if (oldFormats[j].format())
            oldFormats[j].format()->addRef();
    }
}

Q3TextFormatCommand::~Q3TextFormatCommand()
{
    format->removeRef();
    for (int j = 0; j < (int)oldFormats.size(); ++j) {
        if (oldFormats[j].format())
            oldFormats[j].format()->removeRef();
    }
}

Q3TextCursor *Q3TextFormatCommand::execute(Q3TextCursor *c)
{
    Q3TextParagraph *sp = doc->paragAt(startId);
    Q3TextParagraph *ep = doc->paragAt(endId);
    if (!sp || !ep)
        return c;

    Q3TextCursor start(doc);
    start.setParagraph(sp);
    start.setIndex(startIndex);
    Q3TextCursor end(doc);
    end.setParagraph(ep);
    end.setIndex(endIndex);

    doc->setSelectionStart(Q3TextDocument::Temp, start);
    doc->setSelectionEnd(Q3TextDocument::Temp, end);
    doc->setFormat(Q3TextDocument::Temp, format, flags);
    doc->removeSelection(Q3TextDocument::Temp);
    if (endIndex == ep->length())
        end.gotoLeft();
    *c = end;
    return c;
}

Q3TextCursor *Q3TextFormatCommand::unexecute(Q3TextCursor *c)
{
    Q3TextParagraph *sp = doc->paragAt(startId);
    Q3TextParagraph *ep = doc->paragAt(endId);
    if (!sp || !ep)
        return 0;

    int idx = startIndex;
    int fIndex = 0;
    while ( fIndex < int(oldFormats.size()) ) {
        if (oldFormats.at(fIndex).c == QLatin1Char('\n')) {
            if (idx > 0) {
                if (idx < sp->length() && fIndex > 0)
                    sp->setFormat(idx, 1, oldFormats.at(fIndex - 1).format());
                if (sp == ep)
                    break;
                sp = sp->next();
                idx = 0;
            }
            fIndex++;
        }
        if (oldFormats.at(fIndex).format())
            sp->setFormat(idx, 1, oldFormats.at(fIndex).format());
        idx++;
        fIndex++;
        if (fIndex >= (int)oldFormats.size())
            break;
        if (idx >= sp->length()) {
            if (sp == ep)
                break;
            sp = sp->next();
            idx = 0;
        }
    }

    Q3TextCursor end(doc);
    end.setParagraph(ep);
    end.setIndex(endIndex);
    if (endIndex == ep->length())
        end.gotoLeft();
    *c = end;
    return c;
}

Q3TextStyleCommand::Q3TextStyleCommand(Q3TextDocument *dc, int fParag, int lParag, const QByteArray& beforeChange)
    : Q3TextCommand(dc), firstParag(fParag), lastParag(lParag), before(beforeChange)
{
    after = readStyleInformation( dc, fParag, lParag);
}


QByteArray Q3TextStyleCommand::readStyleInformation( Q3TextDocument* doc, int fParag, int lParag)
{
    QByteArray style;
#ifndef QT_NO_DATASTREAM
    Q3TextParagraph *p = doc->paragAt(fParag);
    if (!p)
        return style;
    QDataStream styleStream(&style, IO_WriteOnly);
    int num = lParag - fParag + 1;
    styleStream << num;
    while (num -- && p) {
        p->writeStyleInformation(styleStream);
        p = p->next();
    }
#endif
    return style;
}

void Q3TextStyleCommand::writeStyleInformation( Q3TextDocument* doc, int fParag, const QByteArray& style)
{
#ifndef QT_NO_DATASTREAM
    Q3TextParagraph *p = doc->paragAt(fParag);
    if (!p)
        return;
    QByteArray copy = style;
    QDataStream styleStream(&copy, IO_ReadOnly);
    int num;
    styleStream >> num;
    while (num-- && p) {
        p->readStyleInformation(styleStream);
        p = p->next();
    }
#endif
}

Q3TextCursor *Q3TextStyleCommand::execute(Q3TextCursor *c)
{
    writeStyleInformation(doc, firstParag, after);
    return c;
}

Q3TextCursor *Q3TextStyleCommand::unexecute(Q3TextCursor *c)
{
    writeStyleInformation(doc, firstParag, before);
    return c;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Q3TextCursor::Q3TextCursor(Q3TextDocument *dc)
    : idx(0), tmpX(-1), ox(0), oy(0),
      valid(true)
{
    para = dc ? dc->firstParagraph() : 0;
}

Q3TextCursor::Q3TextCursor(const Q3TextCursor &c)
{
    ox = c.ox;
    oy = c.oy;
    idx = c.idx;
    para = c.para;
    tmpX = c.tmpX;
    indices = c.indices;
    paras = c.paras;
    xOffsets = c.xOffsets;
    yOffsets = c.yOffsets;
    valid = c.valid;
}

Q3TextCursor::~Q3TextCursor()
{
}

Q3TextCursor &Q3TextCursor::operator=(const Q3TextCursor &c)
{
    ox = c.ox;
    oy = c.oy;
    idx = c.idx;
    para = c.para;
    tmpX = c.tmpX;
    indices = c.indices;
    paras = c.paras;
    xOffsets = c.xOffsets;
    yOffsets = c.yOffsets;
    valid = c.valid;

    return *this;
}

bool Q3TextCursor::operator==(const Q3TextCursor &c) const
{
    return para == c.para && idx == c.idx;
}

int Q3TextCursor::totalOffsetX() const
{
    int xoff = ox;
    for (QStack<int>::ConstIterator xit = xOffsets.begin(); xit != xOffsets.end(); ++xit)
        xoff += *xit;
    return xoff;
}

int Q3TextCursor::totalOffsetY() const
{
    int yoff = oy;
    for (QStack<int>::ConstIterator yit = yOffsets.begin(); yit != yOffsets.end(); ++yit)
        yoff += *yit;
    return yoff;
}

#ifndef QT_NO_TEXTCUSTOMITEM
void Q3TextCursor::gotoIntoNested(const QPoint &globalPos)
{
    if (!para)
        return;
    Q_ASSERT(para->at(idx)->isCustom());
    push();
    ox = 0;
    int bl, y;
    para->lineHeightOfChar(idx, &bl, &y);
    oy = y + para->rect().y();
    ox = para->at(idx)->x;
    Q3TextDocument* doc = document();
    para->at(idx)->customItem()->enterAt(this, doc, para, idx, ox, oy, globalPos-QPoint(ox,oy));
}
#endif

void Q3TextCursor::invalidateNested()
{
    if (nestedDepth()) {
        QStack<Q3TextParagraph*>::Iterator it = paras.begin();
        QStack<int>::Iterator it2 = indices.begin();
        for (; it != paras.end(); ++it, ++it2) {
            if (*it == para)
                continue;
            (*it)->invalidate(0);
#ifndef QT_NO_TEXTCUSTOMITEM
            if ((*it)->at(*it2)->isCustom())
                (*it)->at(*it2)->customItem()->invalidate();
#endif
        }
    }
}

void Q3TextCursor::insert(const QString &str, bool checkNewLine, QVector<Q3TextStringChar> *formatting)
{
    tmpX = -1;
    bool justInsert = true;
    QString s(str);
#if defined(Q_WS_WIN)
    if (checkNewLine) {
        int i = 0;
        while ((i = s.indexOf(QLatin1Char('\r'), i)) != -1)
            s.remove(i ,1);
    }
#endif
    if (checkNewLine)
        justInsert = s.indexOf(QLatin1Char('\n')) == -1;
    if (justInsert) { // we ignore new lines and insert all in the current para at the current index
        para->insert(idx, s.unicode(), s.length());
        if (formatting) {
            for (int i = 0; i < (int)s.length(); ++i) {
                if (formatting->at(i).format()) {
                    formatting->at(i).format()->addRef();
                    para->string()->setFormat(idx + i, formatting->at(i).format(), true);
                }
            }
        }
        idx += s.length();
    } else { // we split at new lines
        int start = -1;
        int end;
        int y = para->rect().y() + para->rect().height();
        int lastIndex = 0;
        do {
            end = s.indexOf(QLatin1Char('\n'), start + 1); // find line break
            if (end == -1) // didn't find one, so end of line is end of string
                end = s.length();
            int len = (start == -1 ? end : end - start - 1);
            if (len > 0) // insert the line
                para->insert(idx, s.unicode() + start + 1, len);
            else
                para->invalidate(0);
            if (formatting) { // set formats to the chars of the line
                for (int i = 0; i < len; ++i) {
                    if (formatting->at(i + lastIndex).format()) {
                        formatting->at(i + lastIndex).format()->addRef();
                        para->string()->setFormat(i + idx, formatting->at(i + lastIndex).format(), true);
                    }
                }
                lastIndex += len;
            }
            start = end; // next start is at the end of this line
            idx += len; // increase the index of the cursor to the end of the inserted text
            if (s[end] == QLatin1Char('\n')) { // if at the end was a line break, break the line
                splitAndInsertEmptyParagraph(false, true);
                para->setEndState(-1);
                para->prev()->format(-1, false);
                lastIndex++;
            }

        } while (end < (int)s.length());

        para->format(-1, false);
        int dy = para->rect().y() + para->rect().height() - y;
        Q3TextParagraph *p = para;
        p->setParagId(p->prev() ? p->prev()->paragId() + 1 : 0);
        p = p->next();
        while (p) {
            p->setParagId(p->prev()->paragId() + 1);
            p->move(dy);
            p->invalidate(0);
            p->setEndState(-1);
            p = p->next();
        }
    }

    int h = para->rect().height();
    para->format(-1, true);
    if (h != para->rect().height())
        invalidateNested();
    else if (para->document() && para->document()->parent())
        para->document()->nextDoubleBuffered = true;

    fixCursorPosition();
}

void Q3TextCursor::gotoLeft()
{
    if (para->string()->isRightToLeft())
        gotoNextLetter();
    else
        gotoPreviousLetter();
}

void Q3TextCursor::gotoPreviousLetter()
{
    tmpX = -1;

    if (idx > 0) {
        idx = para->string()->previousCursorPosition(idx);
#ifndef QT_NO_TEXTCUSTOMITEM
        const Q3TextStringChar *tsc = para->at(idx);
        if (tsc && tsc->isCustom() && tsc->customItem()->isNested())
            processNesting(EnterEnd);
#endif
    } else if (para->prev()) {
        para = para->prev();
        while (!para->isVisible() && para->prev())
            para = para->prev();
        idx = para->length() - 1;
    } else if (nestedDepth()) {
        pop();
        processNesting(Prev);
        if (idx == -1) {
            pop();
            if (idx > 0) {
                idx = para->string()->previousCursorPosition(idx);
#ifndef QT_NO_TEXTCUSTOMITEM
                const Q3TextStringChar *tsc = para->at(idx);
                if (tsc && tsc->isCustom() && tsc->customItem()->isNested())
                    processNesting(EnterEnd);
#endif
            } else if (para->prev()) {
                para = para->prev();
                idx = para->length() - 1;
            }
        }
    }
}

void Q3TextCursor::push()
{
    indices.push(idx);
    paras.push(para);
    xOffsets.push(ox);
    yOffsets.push(oy);
}

void Q3TextCursor::pop()
{
    if (indices.isEmpty())
        return;
    idx = indices.pop();
    para = paras.pop();
    ox = xOffsets.pop();
    oy = yOffsets.pop();
}

void Q3TextCursor::restoreState()
{
    while (!indices.isEmpty())
        pop();
}

bool Q3TextCursor::place(const QPoint &p, Q3TextParagraph *s, bool link)
{
    QPoint pos(p);
    QRect r;
    Q3TextParagraph *str = s;
    if (pos.y() < s->rect().y()) {
        pos.setY(s->rect().y());
#ifdef Q_WS_MAC
        pos.setX(s->rect().x());
#endif
    }
    while (s) {
        r = s->rect();
        r.setWidth(document() ? document()->width() : QWIDGETSIZE_MAX);
        if (s->isVisible())
            str = s;
        if (pos.y() >= r.y() && pos.y() <= r.y() + r.height())
            break;
        if (!s->next()) {
#ifdef Q_WS_MAC
            pos.setX(s->rect().x() + s->rect().width());
#endif
            break;
        }
        s = s->next();
    }

    if (!s || !str)
        return false;

    s = str;

    setParagraph(s);
    int y = s->rect().y();
    int lines = s->lines();
    Q3TextStringChar *chr = 0;
    int index = 0;
    int i = 0;
    int cy = 0;
    int ch = 0;
    for (; i < lines; ++i) {
        chr = s->lineStartOfLine(i, &index);
        cy = s->lineY(i);
        ch = s->lineHeight(i);
        if (!chr)
            return false;
        if (pos.y() <= y + cy + ch)
            break;
    }
    int nextLine;
    if (i < lines - 1)
        s->lineStartOfLine(i+1, &nextLine);
    else
        nextLine = s->length();
    i = index;
    int x = s->rect().x();
    if (pos.x() < x)
        pos.setX(x + 1);
    int cw;
    int curpos = s->length()-1;
    int dist = 10000000;
    bool inCustom = false;
    while (i < nextLine) {
        chr = s->at(i);
        int cpos = x + chr->x;
        cw = s->string()->width(i);
#ifndef QT_NO_TEXTCUSTOMITEM
        if (chr->isCustom() && chr->customItem()->isNested()) {
            if (pos.x() >= cpos && pos.x() <= cpos + cw &&
                 pos.y() >= y + cy && pos.y() <= y + cy + chr->height()) {
                inCustom = true;
                curpos = i;
                break;
            }
        } else
#endif
        {
            if(chr->rightToLeft)
                cpos += cw;
            int diff = cpos - pos.x();
            bool dm = diff < 0 ? !chr->rightToLeft : chr->rightToLeft;
            if ((QABS(diff) < dist || (dist == diff && dm == true)) && para->string()->validCursorPosition(i)) {
                dist = QABS(diff);
                if (!link || pos.x() >= x + chr->x)
                    curpos = i;
            }
        }
        i++;
    }
    setIndex(curpos);

#ifndef QT_NO_TEXTCUSTOMITEM
    if (inCustom && para->document() && para->at(curpos)->isCustom() && para->at(curpos)->customItem()->isNested()) {
        Q3TextDocument *oldDoc = para->document();
        gotoIntoNested(pos);
        if (oldDoc == para->document())
            return true;
        QPoint p(pos.x() - offsetX(), pos.y() - offsetY());
        if (!place(p, document()->firstParagraph(), link))
            pop();
    }
#endif
    return true;
}

bool Q3TextCursor::processNesting(Operation op)
{
    if (!para->document())
        return false;
    Q3TextDocument* doc = para->document();
    push();
    ox = para->at(idx)->x;
    int bl, y;
    para->lineHeightOfChar(idx, &bl, &y);
    oy = y + para->rect().y();
    bool ok = false;

#ifndef QT_NO_TEXTCUSTOMITEM
    switch (op) {
    case EnterBegin:
        ok = para->at(idx)->customItem()->enter(this, doc, para, idx, ox, oy);
        break;
    case EnterEnd:
        ok = para->at(idx)->customItem()->enter(this, doc, para, idx, ox, oy, true);
        break;
    case Next:
        ok = para->at(idx)->customItem()->next(this, doc, para, idx, ox, oy);
        break;
    case Prev:
        ok = para->at(idx)->customItem()->prev(this, doc, para, idx, ox, oy);
        break;
    case Down:
        ok = para->at(idx)->customItem()->down(this, doc, para, idx, ox, oy);
        break;
    case Up:
        ok = para->at(idx)->customItem()->up(this, doc, para, idx, ox, oy);
        break;
    }
    if (!ok)
#endif
        pop();
    return ok;
}

void Q3TextCursor::gotoRight()
{
    if (para->string()->isRightToLeft())
        gotoPreviousLetter();
    else
        gotoNextLetter();
}

void Q3TextCursor::gotoNextLetter()
{
   tmpX = -1;

#ifndef QT_NO_TEXTCUSTOMITEM
    const Q3TextStringChar *tsc = para->at(idx);
    if (tsc && tsc->isCustom() && tsc->customItem()->isNested()) {
        if (processNesting(EnterBegin))
            return;
    }
#endif

    if (idx < para->length() - 1) {
        idx = para->string()->nextCursorPosition(idx);
    } else if (para->next()) {
        para = para->next();
        while (!para->isVisible() && para->next())
            para = para->next();
        idx = 0;
    } else if (nestedDepth()) {
        pop();
        processNesting(Next);
        if (idx == -1) {
            pop();
            if (idx < para->length() - 1) {
                idx = para->string()->nextCursorPosition(idx);
            } else if (para->next()) {
                para = para->next();
                idx = 0;
            }
        }
    }
}

void Q3TextCursor::gotoUp()
{
    int indexOfLineStart;
    int line;
    Q3TextStringChar *c = para->lineStartOfChar(idx, &indexOfLineStart, &line);
    if (!c)
        return;

    if (tmpX < 0)
        tmpX = x();

    if (indexOfLineStart == 0) {
        if (!para->prev()) {
            if (!nestedDepth())
                return;
            pop();
            processNesting(Up);
            if (idx == -1) {
                pop();
                if (!para->prev())
                    return;
                idx = tmpX = 0;
            } else {
                tmpX = -1;
                return;
            }
        }
        Q3TextParagraph *p = para->prev();
        while (p && !p->isVisible())
            p = p->prev();
        if (p)
            para = p;
        int lastLine = para->lines() - 1;
        if (!para->lineStartOfLine(lastLine, &indexOfLineStart))
            return;
        idx = indexOfLineStart;
        while (idx < para->length()-1 && para->at(idx)->x < tmpX)
            ++idx;
        if (idx > indexOfLineStart &&
            para->at(idx)->x - tmpX > tmpX - para->at(idx-1)->x)
            --idx;
    } else {
        --line;
        int oldIndexOfLineStart = indexOfLineStart;
        if (!para->lineStartOfLine(line, &indexOfLineStart))
            return;
        idx = indexOfLineStart;
        while (idx < oldIndexOfLineStart-1 && para->at(idx)->x < tmpX)
            ++idx;
        if (idx > indexOfLineStart &&
            para->at(idx)->x - tmpX > tmpX - para->at(idx-1)->x)
            --idx;
    }
    fixCursorPosition();
}

void Q3TextCursor::gotoDown()
{
    int indexOfLineStart;
    int line;
    Q3TextStringChar *c = para->lineStartOfChar(idx, &indexOfLineStart, &line);
    if (!c)
        return;

    if (tmpX < 0)
        tmpX = x();

    if (line == para->lines() - 1) {
        if (!para->next()) {
            if (!nestedDepth())
                return;
            pop();
            processNesting(Down);
            if (idx == -1) {
                pop();
                if (!para->next())
                    return;
                idx = tmpX = 0;
            } else {
                tmpX = -1;
                return;
            }
        }
        Q3TextParagraph *s = para->next();
        while (s && !s->isVisible())
            s = s->next();
        if (s)
            para = s;
        if (!para->lineStartOfLine(0, &indexOfLineStart))
            return;
        int end;
        if (para->lines() == 1)
            end = para->length();
        else
            para->lineStartOfLine(1, &end);

        idx = indexOfLineStart;
        while (idx < end-1 && para->at(idx)->x < tmpX)
            ++idx;
        if (idx > indexOfLineStart &&
            para->at(idx)->x - tmpX > tmpX - para->at(idx-1)->x)
            --idx;
    } else {
        ++line;
        int end;
        if (line == para->lines() - 1)
            end = para->length();
        else
            para->lineStartOfLine(line + 1, &end);
        if (!para->lineStartOfLine(line, &indexOfLineStart))
            return;
        idx = indexOfLineStart;
        while (idx < end-1 && para->at(idx)->x < tmpX)
            ++idx;
        if (idx > indexOfLineStart &&
            para->at(idx)->x - tmpX > tmpX - para->at(idx-1)->x)
            --idx;
    }
    fixCursorPosition();
}

void Q3TextCursor::gotoLineEnd()
{
    tmpX = -1;
    int indexOfLineStart;
    int line;
    Q3TextStringChar *c = para->lineStartOfChar(idx, &indexOfLineStart, &line);
    if (!c)
        return;

    if (line == para->lines() - 1) {
        idx = para->length() - 1;
    } else {
        c = para->lineStartOfLine(++line, &indexOfLineStart);
        indexOfLineStart--;
        idx = indexOfLineStart;
    }
}

void Q3TextCursor::gotoLineStart()
{
    tmpX = -1;
    int indexOfLineStart;
    int line;
    Q3TextStringChar *c = para->lineStartOfChar(idx, &indexOfLineStart, &line);
    if (!c)
        return;

    idx = indexOfLineStart;
}

void Q3TextCursor::gotoHome()
{
    if (topParagraph()->document())
        gotoPosition(topParagraph()->document()->firstParagraph());
    else
        gotoLineStart();
}

void Q3TextCursor::gotoEnd()
{
    if (topParagraph()->document() && topParagraph()->document()->lastParagraph()->isValid())
        gotoPosition(topParagraph()->document()->lastParagraph(),
                      topParagraph()->document()->lastParagraph()->length() - 1);
    else
        gotoLineEnd();
}

void Q3TextCursor::gotoPageUp(int visibleHeight)
{
    int targetY  = globalY() - visibleHeight;
    Q3TextParagraph* old; int index;
    do {
        old = para; index = idx;
        gotoUp();
    } while ((old != para || index != idx)  && globalY() > targetY);
}

void Q3TextCursor::gotoPageDown(int visibleHeight)
{
    int targetY  = globalY() + visibleHeight;
    Q3TextParagraph* old; int index;
    do {
        old = para; index = idx;
        gotoDown();
    } while ((old != para || index != idx) && globalY() < targetY);
}

void Q3TextCursor::gotoWordRight()
{
    if (para->string()->isRightToLeft())
        gotoPreviousWord();
    else
        gotoNextWord();
}

void Q3TextCursor::gotoWordLeft()
{
    if (para->string()->isRightToLeft())
        gotoNextWord();
    else
        gotoPreviousWord();
}

static bool is_seperator(const QChar &c, bool onlySpace)
{
    if (onlySpace)
        return c.isSpace();
    return c.isSpace() ||
           c == QLatin1Char('\t') ||
           c == QLatin1Char('.') ||
           c == QLatin1Char(',') ||
           c == QLatin1Char(':') ||
           c == QLatin1Char(';') ||
           c == QLatin1Char('-') ||
           c == QLatin1Char('<') ||
           c == QLatin1Char('>') ||
           c == QLatin1Char('[') ||
           c == QLatin1Char(']') ||
           c == QLatin1Char('(') ||
           c == QLatin1Char(')') ||
           c == QLatin1Char('{') ||
           c == QLatin1Char('}');
}

void Q3TextCursor::gotoPreviousWord(bool onlySpace)
{
    gotoPreviousLetter();
    tmpX = -1;
    Q3TextString *s = para->string();
    bool allowSame = false;
    if (idx == ((int)s->length()-1))
        return;
    for (int i = idx; i >= 0; --i) {
        if (is_seperator(s->at(i).c, onlySpace)) {
            if (!allowSame)
                continue;
            idx = i + 1;
            return;
        }
        if (!allowSame && !is_seperator(s->at(i).c, onlySpace))
            allowSame = true;
    }
    idx = 0;
}

void Q3TextCursor::gotoNextWord(bool onlySpace)
{
    tmpX = -1;
    Q3TextString *s = para->string();
    bool allowSame = false;
    for (int i = idx; i < (int)s->length(); ++i) {
        if (!is_seperator(s->at(i).c, onlySpace)) {
            if (!allowSame)
                continue;
            idx = i;
            return;
        }
        if (!allowSame && is_seperator(s->at(i).c, onlySpace))
            allowSame = true;

    }

    if (idx < ((int)s->length()-1)) {
        gotoLineEnd();
    } else if (para->next()) {
        Q3TextParagraph *p = para->next();
        while (p  && !p->isVisible())
            p = p->next();
        if (s) {
            para = p;
            idx = 0;
        }
    } else {
        gotoLineEnd();
    }
}

bool Q3TextCursor::atParagStart()
{
    return idx == 0;
}

bool Q3TextCursor::atParagEnd()
{
    return idx == para->length() - 1;
}

void Q3TextCursor::splitAndInsertEmptyParagraph(bool ind, bool updateIds)
{
    if (!para->document())
        return;
    tmpX = -1;
    Q3TextFormat *f = 0;
    if (para->document()->useFormatCollection()) {
        f = para->at(idx)->format();
        if (idx == para->length() - 1 && idx > 0)
            f = para->at(idx - 1)->format();
        if (f->isMisspelled()) {
            f->removeRef();
            f = para->document()->formatCollection()->format(f->font(), f->color());
        }
    }

    if (atParagEnd()) {
        Q3TextParagraph *n = para->next();
        Q3TextParagraph *s = para->document()->createParagraph(para->document(), para, n, updateIds);
        if (f)
            s->setFormat(0, 1, f, true);
        s->copyParagData(para);
        if (ind) {
            int oi, ni;
            s->indent(&oi, &ni);
            para = s;
            idx = ni;
        } else {
            para = s;
            idx = 0;
        }
    } else if (atParagStart()) {
        Q3TextParagraph *p = para->prev();
        Q3TextParagraph *s = para->document()->createParagraph(para->document(), p, para, updateIds);
        if (f)
            s->setFormat(0, 1, f, true);
        s->copyParagData(para);
        if (ind) {
            s->indent();
            s->format();
            indent();
            para->format();
        }
    } else {
        QString str = para->string()->toString().mid(idx, 0xFFFFFF);
        Q3TextParagraph *n = para->next();
        Q3TextParagraph *s = para->document()->createParagraph(para->document(), para, n, updateIds);
        s->copyParagData(para);
        s->remove(0, 1);
        s->append(str, true);
        for (int i = 0; i < str.length(); ++i) {
            Q3TextStringChar* tsc = para->at(idx + i);
            s->setFormat(i, 1, tsc->format(), true);
#ifndef QT_NO_TEXTCUSTOMITEM
            if (tsc->isCustom()) {
                Q3TextCustomItem * item = tsc->customItem();
                s->at(i)->setCustomItem(item);
                tsc->loseCustomItem();
            }
#endif
            if (tsc->isAnchor())
                s->at(i)->setAnchor(tsc->anchorName(),
                                       tsc->anchorHref());
        }
        para->truncate(idx);
        if (ind) {
            int oi, ni;
            s->indent(&oi, &ni);
            para = s;
            idx = ni;
        } else {
            para = s;
            idx = 0;
        }
    }

    invalidateNested();
}

bool Q3TextCursor::remove()
{
    tmpX = -1;
    if (!atParagEnd()) {
        int next = para->string()->nextCursorPosition(idx);
        para->remove(idx, next-idx);
        int h = para->rect().height();
        para->format(-1, true);
        if (h != para->rect().height())
            invalidateNested();
        else if (para->document() && para->document()->parent())
            para->document()->nextDoubleBuffered = true;
        return false;
    } else if (para->next()) {
        para->join(para->next());
        invalidateNested();
        return true;
    }
    return false;
}

/* needed to implement backspace the correct way */
bool Q3TextCursor::removePreviousChar()
{
    tmpX = -1;
    if (!atParagStart()) {
        para->remove(idx-1, 1);
        int h = para->rect().height();
        idx--;
        // shouldn't be needed, just to make sure.
        fixCursorPosition();
        para->format(-1, true);
        if (h != para->rect().height())
            invalidateNested();
        else if (para->document() && para->document()->parent())
            para->document()->nextDoubleBuffered = true;
        return false;
    } else if (para->prev()) {
        para = para->prev();
        para->join(para->next());
        invalidateNested();
        return true;
    }
    return false;
}

void Q3TextCursor::indent()
{
    int oi = 0, ni = 0;
    para->indent(&oi, &ni);
    if (oi == ni)
        return;

    if (idx >= oi)
        idx += ni - oi;
    else
        idx = ni;
}

void Q3TextCursor::fixCursorPosition()
{
    // searches for the closest valid cursor position
    if (para->string()->validCursorPosition(idx))
        return;

    int lineIdx;
    Q3TextStringChar *start = para->lineStartOfChar(idx, &lineIdx, 0);
    int x = para->string()->at(idx).x;
    int diff = QABS(start->x - x);
    int best = lineIdx;

    Q3TextStringChar *c = start;
    ++c;

    Q3TextStringChar *end = &para->string()->at(para->length()-1);
    while (c <= end && !c->lineStart) {
        int xp = c->x;
        if (c->rightToLeft)
            xp += para->string()->width(lineIdx + (c-start));
        int ndiff = QABS(xp - x);
        if (ndiff < diff && para->string()->validCursorPosition(lineIdx + (c-start))) {
            diff = ndiff;
            best = lineIdx + (c-start);
        }
        ++c;
    }
    idx = best;
}


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Q3TextDocument::Q3TextDocument(Q3TextDocument *p)
    : par(p), parentPar(0)
#ifndef QT_NO_TEXTCUSTOMITEM
    , tc(0)
#endif
    , tArray(0), tStopWidth(0)
{
    fCollection = par ? par->fCollection : new Q3TextFormatCollection;
    init();
}

void Q3TextDocument::init()
{
    oTextValid = true;
    mightHaveCustomItems = false;
    if (par)
        par->insertChild(this);
    pProcessor = 0;
    useFC = true;
    pFormatter = 0;
    indenter = 0;
    fParag = 0;
    txtFormat = Qt::AutoText;
    preferRichText = false;
    pages = false;
    focusIndicator.parag = 0;
    minw = 0;
    wused = 0;
    minwParag = curParag = 0;
    align = Qt::AlignAuto;
    nSelections = 1;

    setStyleSheet(Q3StyleSheet::defaultSheet());
#ifndef QT_NO_MIME
    factory_ = Q3MimeSourceFactory::defaultFactory();
#endif
    contxt.clear();

    underlLinks = par ? par->underlLinks : true;
    backBrush = 0;
    buf_pixmap = 0;
    nextDoubleBuffered = false;

    if (par)
        withoutDoubleBuffer = par->withoutDoubleBuffer;
    else
        withoutDoubleBuffer = false;

    lParag = fParag = createParagraph(this, 0, 0);

    cx = 0;
    cy = 2;
    if (par)
        cx = cy = 0;
    cw = 600;
    vw = 0;
    flow_ = new Q3TextFlow;
    flow_->setWidth(cw);

    leftmargin = rightmargin = 4;
    scaleFontsFactor = 1;

    commandHistory = new Q3TextCommandHistory(100);
    tStopWidth = formatCollection()->defaultFormat()->width(QLatin1Char('x')) * 8;
}

Q3TextDocument::~Q3TextDocument()
{
    delete commandHistory;
    if (par)
        par->removeChild(this);
    clear();
    delete flow_;
    if (!par) {
        delete pFormatter;
        delete fCollection;
    }
    delete pProcessor;
    delete buf_pixmap;
    delete indenter;
    delete backBrush;
    delete [] tArray;
}

void Q3TextDocument::clear(bool createEmptyParag)
{
    while (fParag) {
        Q3TextParagraph *p = fParag->next();
        delete fParag;
        fParag = p;
    }
    if (flow_)
        flow_->clear();
    fParag = lParag = 0;
    if (createEmptyParag)
        fParag = lParag = createParagraph(this);
    selections.clear();
    oText.clear();
    oTextValid = false;
}

int Q3TextDocument::widthUsed() const
{
    return wused + 2*border_tolerance;
}

int Q3TextDocument::height() const
{
    int h = 0;
    if (lParag)
        h = lParag->rect().top() + lParag->rect().height() + 1;
    int fh = flow_->boundingRect().bottom();
    return qMax(h, fh);
}



Q3TextParagraph *Q3TextDocument::createParagraph(Q3TextDocument *dc, Q3TextParagraph *pr, Q3TextParagraph *nx, bool updateIds)
{
    return new Q3TextParagraph(dc, pr, nx, updateIds);
}

bool Q3TextDocument::setMinimumWidth(int needed, int used, Q3TextParagraph *p)
{
    if (needed == -1) {
        minw = 0;
        wused = 0;
        p = 0;
    }
    if (p == minwParag) {
        if (minw > needed) {
            Q3TextParagraph *tp = fParag;
            while (tp) {
                if (tp != p && tp->minwidth > needed) {
                    needed = tp->minwidth;
                    minwParag = tp;
                }
                tp = tp->n;
            }
        }
        minw = needed;
        emit minimumWidthChanged(minw);
    } else if (needed > minw) {
        minw = needed;
        minwParag = p;
        emit minimumWidthChanged(minw);
    }
    wused = qMax(wused, used);
    wused = qMax(wused, minw);
    cw = qMax(minw, cw);
    return true;
}

void Q3TextDocument::setPlainText(const QString &text)
{
    preferRichText = false;
    clear();
    oTextValid = true;
    oText = text;

    int lastNl = 0;
    int nl = text.indexOf(QLatin1Char('\n'));
    if (nl == -1) {
        lParag = createParagraph(this, lParag, 0);
        if (!fParag)
            fParag = lParag;
        QString s = text;
        if (!s.isEmpty()) {
            if (s[(int)s.length() - 1] == QLatin1Char('\r'))
                s.remove(s.length() - 1, 1);
            lParag->append(s);
        }
    } else {
        for (;;) {
            lParag = createParagraph(this, lParag, 0);
            if (!fParag)
                fParag = lParag;
            int l = nl - lastNl;
            if (l > 0) {
                if (text.unicode()[nl-1] == QLatin1Char('\r'))
                    l--;
                QString cs = QString::fromRawData(text.unicode()+lastNl, l);
                lParag->append(cs);
            }
            if (nl == (int)text.length())
                break;
            lastNl = nl + 1;
            nl = text.indexOf(QLatin1Char('\n'), nl + 1);
            if (nl == -1)
                nl = text.length();
        }
    }
    if (!lParag)
        lParag = fParag = createParagraph(this, 0, 0);
}

struct Q3TextDocumentTag {
    Q3TextDocumentTag(){}
    Q3TextDocumentTag(const QString&n, const Q3StyleSheetItem* s, const Q3TextFormat& f)
        :name(n),style(s), format(f), alignment(Qt::AlignAuto), direction(QChar::DirON),liststyle(Q3StyleSheetItem::ListDisc) {
            wsm = Q3StyleSheetItem::WhiteSpaceNormal;
    }
    QString name;
    const Q3StyleSheetItem* style;
    QString anchorHref;
    Q3StyleSheetItem::WhiteSpaceMode wsm;
    Q3TextFormat format;
    signed int alignment : 16;
    signed int direction : 5;
    Q3StyleSheetItem::ListStyle liststyle;

    Q3TextDocumentTag( const Q3TextDocumentTag& t) {
        name = t.name;
        style = t.style;
        anchorHref = t.anchorHref;
        wsm = t.wsm;
        format = t.format;
        alignment = t.alignment;
        direction = t.direction;
        liststyle = t.liststyle;
    }
    Q3TextDocumentTag& operator=(const Q3TextDocumentTag& t) {
        name = t.name;
        style = t.style;
        anchorHref = t.anchorHref;
        wsm = t.wsm;
        format = t.format;
        alignment = t.alignment;
        direction = t.direction;
        liststyle = t.liststyle;
        return *this;
    }

    Q_DUMMY_COMPARISON_OPERATOR(Q3TextDocumentTag)
};


#define NEWPAR \
    do{ \
        if (!hasNewPar) { \
            if (!textEditMode && curpar && curpar->length()>1 \
                 && curpar->at(curpar->length()-2)->c == QChar::LineSeparator) \
                curpar->remove(curpar->length()-2, 1); \
            curpar = createParagraph(this, curpar, curpar->next()); \
            styles.append(vec); \
            vec = 0; \
        } \
        hasNewPar = true; \
        curpar->rtext = true;  \
        curpar->align = curtag.alignment; \
        curpar->lstyle = curtag.liststyle; \
        curpar->litem = (curtag.style->displayMode() == Q3StyleSheetItem::DisplayListItem); \
        curpar->str->setDirection((QChar::Direction)curtag.direction); \
        space = true; \
        tabExpansionColumn = 0; \
        delete vec; \
        vec = new QVector<Q3StyleSheetItem *>(); \
        for (QStack<Q3TextDocumentTag>::Iterator it = tags.begin(); it != tags.end(); ++it) \
            vec->append(const_cast<Q3StyleSheetItem *>((*it).style)); \
        vec->append(const_cast<Q3StyleSheetItem *>(curtag.style)); \
    } while(false);


void Q3TextDocument::setRichText(const QString &text, const QString &context, const Q3TextFormat *initialFormat)
{
    preferRichText = true;
    if (!context.isEmpty())
        setContext(context);
    clear();
    fParag = lParag = createParagraph(this);
    oTextValid = true;
    oText = text;
    setRichTextInternal(text, 0, initialFormat);
    fParag->rtext = true;
}

void Q3TextDocument::setRichTextInternal(const QString &text, Q3TextCursor* cursor, const Q3TextFormat *initialFormat)
{
    Q3TextParagraph* curpar = lParag;
    int pos = 0;
    QStack<Q3TextDocumentTag> tags;
    if (!initialFormat)
        initialFormat = formatCollection()->defaultFormat();
    Q3TextDocumentTag initag(QLatin1String(""), sheet_->item(QLatin1String("")), *initialFormat);
    if (bodyText.isValid())
        initag.format.setColor(bodyText);
    Q3TextDocumentTag curtag = initag;
    bool space = true;
    bool canMergeLi = false;

    bool textEditMode = false;
    int tabExpansionColumn = 0;

    const QChar* doc = text.unicode();
    int length = text.length();
    bool hasNewPar = curpar->length() <= 1;
    QString anchorName;

    // style sheet handling for margin and line spacing calculation below
    Q3TextParagraph* stylesPar = curpar;
    QVector<Q3StyleSheetItem *>* vec = 0;
    QList< QVector<Q3StyleSheetItem *> *> styles;

    if (cursor) {
        cursor->splitAndInsertEmptyParagraph();
        Q3TextCursor tmp = *cursor;
        tmp.gotoPreviousLetter();
        stylesPar = curpar = tmp.paragraph();
        hasNewPar = true;
        textEditMode = true;
    } else {
        NEWPAR;
    }

    // set rtext spacing to false for the initial paragraph.
    curpar->rtext = false;

    QString wellKnownTags = QLatin1String("br hr wsp table qt body meta title");

    while (pos < length) {
        if (hasPrefix(doc, length, pos, QLatin1Char('<'))){
            if (!hasPrefix(doc, length, pos+1, QLatin1Char('/'))) {
                // open tag
                QMap<QString, QString> attr;
                QMap<QString, QString>::Iterator it, end = attr.end();
                bool emptyTag = false;
                QString tagname = parseOpenTag(doc, length, pos, attr, emptyTag);
                if (tagname.isEmpty())
                    continue; // nothing we could do with this, probably parse error

                const Q3StyleSheetItem* nstyle = sheet_->item(tagname);

                if (nstyle) {
                    // we might have to close some 'forgotten' tags
                    while (!nstyle->allowedInContext(curtag.style)) {
                        QString msg;
                        msg.sprintf("QText Warning: Document not valid ('%s' not allowed in '%s' #%d)",
                                     tagname.ascii(), curtag.style->name().ascii(), pos);
                        sheet_->error(msg);
                        if (tags.isEmpty())
                            break;
                        curtag = tags.pop();
                    }

                    /* special handling for p and li for HTML
                       compatibility. We do not want to embed blocks in
                       p, and we do not want new blocks inside non-empty
                       lis. Plus we want to merge empty lis sometimes. */
                    if(nstyle->displayMode() == Q3StyleSheetItem::DisplayListItem) {
                        canMergeLi = true;
                    } else if (nstyle->displayMode() == Q3StyleSheetItem::DisplayBlock) {
                        while (curtag.style->name() == QLatin1String("p")) {
                            if (tags.isEmpty())
                                break;
                            curtag = tags.pop();
                        }

                        if (curtag.style->displayMode() == Q3StyleSheetItem::DisplayListItem) {
                            // we are in a li and a new block comes along
                            if (nstyle->name() == QString(QLatin1String("ul")) || nstyle->name() == QLatin1String("ol"))
                                hasNewPar = false; // we want an empty li (like most browsers)
                            if (!hasNewPar) {
                                /* do not add new blocks inside
                                   non-empty lis */
                                while (curtag.style->displayMode() == Q3StyleSheetItem::DisplayListItem) {
                                    if (tags.isEmpty())
                                        break;
                                    curtag = tags.pop();
                                }
                            } else if (canMergeLi) {
                                /* we have an empty li and a block
                                   comes along, merge them */
                                nstyle = curtag.style;
                            }
                            canMergeLi = false;
                        }
                    }
                }

#ifndef QT_NO_TEXTCUSTOMITEM
                Q3TextCustomItem* custom =  0;
#else
                bool custom = false;
#endif

                // some well-known tags, some have a nstyle, some not
                if (wellKnownTags.contains(tagname)) {
                    if (tagname == QLatin1String("br")) {
                        emptyTag = space = true;
                        int index = qMax(curpar->length(),1) - 1;
                        Q3TextFormat format = curtag.format.makeTextFormat(nstyle, attr, scaleFontsFactor);
                        curpar->append(QString(QChar(QChar::LineSeparator)));
                        curpar->setFormat(index, 1, &format);
                        hasNewPar = false;
                    }  else if (tagname == QLatin1String("hr")) {
                        emptyTag = space = true;
#ifndef QT_NO_TEXTCUSTOMITEM
                        custom = tag(sheet_, tagname, attr, contxt, *factory_ , emptyTag, this);
#endif
                    } else if (tagname == QLatin1String("table")) {
                        emptyTag = space = true;
#ifndef QT_NO_TEXTCUSTOMITEM
                        Q3TextFormat format = curtag.format.makeTextFormat( nstyle, attr, scaleFontsFactor);
                        curpar->setAlignment(curtag.alignment);
                        custom = parseTable(attr, format, doc, length, pos, curpar);
#endif
                    } else if (tagname == QLatin1String("qt") || tagname == QLatin1String("body")) {
                        it = attr.find(QLatin1String("bgcolor"));
                        if (it != end) {
                            QBrush *b = new QBrush(QColor(*it));
                            setPaper(b);
                        }
                        it = attr.find(QLatin1String("background"));
                        if (it != end) {
#ifndef QT_NO_MIME
                            QImage img;
                            QString bg = *it;
                            const QMimeSource* m = factory_->data(bg, contxt);
                            if (!m) {
                                qCritical("QRichText: no mimesource for %s",
                                          QFile::encodeName(bg).data());
                            } else {
                                if (!Q3ImageDrag::decode(m, img)) {
                                    qCritical("Q3TextImage: cannot decode %s",
                                              QFile::encodeName(bg).data());
                                }
                            }
                            if (!img.isNull()) {
                                QBrush *b = new QBrush(QColor(), QPixmap(img));
                                setPaper(b);
                            }
#endif
                        }
                        it = attr.find(QLatin1String("text"));
                        if (it != end) {
                            QColor c(*it);
                            initag.format.setColor(c);
                            curtag.format.setColor(c);
                            bodyText = c;
                        }
                        it = attr.find(QLatin1String("link"));
                        if (it != end)
                            linkColor = QColor(*it);
                        it = attr.find(QLatin1String("title"));
                        if (it != end)
                            attribs.insert(QLatin1String("title"), *it);

                        if (textEditMode) {
                            it = attr.find(QLatin1String("style"));
                            if (it != end) {
                                QString a = *it;
                                int count = a.count(QLatin1Char(';')) + 1;
                                for (int s = 0; s < count; s++) {
                                    QString style = a.section(QLatin1Char(';'), s, s);
                                    if (style.startsWith(QLatin1String("font-size:")) && style.endsWith(QLatin1String("pt"))) {
                                        scaleFontsFactor = double(formatCollection()->defaultFormat()->fn.pointSize()) /
                                                           style.mid(10, style.length() - 12).toInt();
                                    }
                                }
                            }
                            nstyle = 0; // ignore body in textEditMode
                        }
                        // end qt- and body-tag handling
                    } else if (tagname == QLatin1String("meta")) {
                        if (attr[QLatin1String("name")] == QLatin1String("qrichtext") && attr[QLatin1String("content")] == QLatin1String("1"))
                            textEditMode = true;
                    } else if (tagname == QLatin1String("title")) {
                        QString title;
                        while (pos < length) {
                            if (hasPrefix(doc, length, pos, QLatin1Char('<')) && hasPrefix(doc, length, pos+1, QLatin1Char('/')) &&
                                 parseCloseTag(doc, length, pos) == QLatin1String("title"))
                                break;
                            title += doc[pos];
                            ++pos;
                        }
                        attribs.insert(QLatin1String("title"), title);
                    }
                } // end of well-known tag handling

#ifndef QT_NO_TEXTCUSTOMITEM
                if (!custom) // try generic custom item
                    custom = tag(sheet_, tagname, attr, contxt, *factory_ , emptyTag, this);
#endif
                if (!nstyle && !custom) // we have no clue what this tag could be, ignore it
                    continue;

                if (custom) {
#ifndef QT_NO_TEXTCUSTOMITEM
                    int index = qMax(curpar->length(),1) - 1;
                    Q3TextFormat format = curtag.format.makeTextFormat(nstyle, attr, scaleFontsFactor);
                    curpar->append(QString(QLatin1Char('*')));
                    Q3TextFormat* f = formatCollection()->format(&format);
                    curpar->setFormat(index, 1, f);
                    curpar->at(index)->setCustomItem(custom);
                    if (!curtag.anchorHref.isEmpty())
                        curpar->at(index)->setAnchor(QString(), curtag.anchorHref);
                    if (!anchorName.isEmpty() ) {
                        curpar->at(index)->setAnchor(anchorName, curpar->at(index)->anchorHref());
                        anchorName.clear();
                    }
                    registerCustomItem(custom, curpar);
                    hasNewPar = false;
#endif
                } else if (!emptyTag) {
                    /* if we do nesting, push curtag on the stack,
                       otherwise reinint curag. */
                    if (curtag.style->name() != tagname || nstyle->selfNesting()) {
                        tags.push(curtag);
                    } else {
                        if (!tags.isEmpty())
                            curtag = tags.top();
                        else
                            curtag = initag;
                    }

                    curtag.name = tagname;
                    curtag.style = nstyle;
                    curtag.name = tagname;
                    curtag.style = nstyle;
                    if (nstyle->whiteSpaceMode()  != Q3StyleSheetItem::WhiteSpaceModeUndefined)
                        curtag.wsm = nstyle->whiteSpaceMode();

                    /* netscape compatibility: eat a newline and only a newline if a pre block starts */
                    if (curtag.wsm == Q3StyleSheetItem::WhiteSpacePre &&
                         nstyle->displayMode() == Q3StyleSheetItem::DisplayBlock)
                        eat(doc, length, pos, QLatin1Char('\n'));

                    /* ignore whitespace for inline elements if there
                       was already one*/
                    if (!textEditMode &&
                         (curtag.wsm == Q3StyleSheetItem::WhiteSpaceNormal
                          || curtag.wsm == Q3StyleSheetItem::WhiteSpaceNoWrap)
                         && (space || nstyle->displayMode() != Q3StyleSheetItem::DisplayInline))
                        eatSpace(doc, length, pos);

                    curtag.format = curtag.format.makeTextFormat(nstyle, attr, scaleFontsFactor);
                    if (nstyle->isAnchor()) {
                        if (!anchorName.isEmpty())
                            anchorName += QLatin1String("#") + attr[QLatin1String("name")];
                        else
                            anchorName = attr[QLatin1String("name")];
                        curtag.anchorHref = attr[QLatin1String("href")];
                    }

                    if (nstyle->alignment() != Q3StyleSheetItem::Undefined)
                        curtag.alignment = nstyle->alignment();

                    if (nstyle->listStyle() != Q3StyleSheetItem::ListStyleUndefined)
                        curtag.liststyle = nstyle->listStyle();

                    if (nstyle->displayMode() == Q3StyleSheetItem::DisplayBlock
                         || nstyle->displayMode() == Q3StyleSheetItem::DisplayListItem) {

                        if (nstyle->name() == QLatin1String("ol") ||
                            nstyle->name() == QLatin1String("ul") ||
                            nstyle->name() == QLatin1String("li")) {
                            QString type = attr[QLatin1String("type")];
                            if (!type.isEmpty()) {
                                if (type == QLatin1String("1")) {
                                    curtag.liststyle = Q3StyleSheetItem::ListDecimal;
                                } else if (type == QLatin1String("a")) {
                                    curtag.liststyle = Q3StyleSheetItem::ListLowerAlpha;
                                } else if (type == QLatin1String("A")) {
                                    curtag.liststyle = Q3StyleSheetItem::ListUpperAlpha;
                                } else {
                                    type = type.toLower();
                                    if (type == QLatin1String("square"))
                                        curtag.liststyle = Q3StyleSheetItem::ListSquare;
                                    else if (type == QLatin1String("disc"))
                                        curtag.liststyle = Q3StyleSheetItem::ListDisc;
                                    else if (type == QLatin1String("circle"))
                                        curtag.liststyle = Q3StyleSheetItem::ListCircle;
                                }
                            }
                        }


                        /* Internally we treat ordered and bullet
                          lists the same for margin calculations. In
                          order to have fast pointer compares in the
                          xMargin() functions we restrict ourselves to
                          <ol>. Once we calculate the margins in the
                          parser rathern than later, the unelegance of
                          this approach goes awy
                         */
                        if (nstyle->name() == QLatin1String("ul"))
                            curtag.style = sheet_->item(QLatin1String("ol"));

                        it = attr.find(QLatin1String("align"));
                        if (it != end) {
                            QString align = (*it).toLower();
                            if (align == QLatin1String("center"))
                                curtag.alignment = Qt::AlignCenter;
                            else if (align == QLatin1String("right"))
                                curtag.alignment = Qt::AlignRight;
                            else if (align == QLatin1String("justify"))
                                curtag.alignment = Qt::AlignJustify;
                        }
                        it = attr.find(QLatin1String("dir"));
                        if (it != end) {
                            QString dir = (*it).toLower();
                            if (dir == QLatin1String("rtl"))
                                curtag.direction = QChar::DirR;
                            else if (dir == QLatin1String("ltr"))
                                curtag.direction = QChar::DirL;
                        }

                        NEWPAR;

                        if (curtag.style && curtag.style->displayMode() == Q3StyleSheetItem::DisplayListItem) {
                            it = attr.find(QLatin1String("value"));
                            if (it != end)
                                curpar->setListValue((*it).toInt());
                        }

                        it = attr.find(QLatin1String("style"));
                        if (it != end) {
                            QString a = *it;
                            bool ok = true;
                            int count = a.count(QLatin1Char(';'))+1;
                            for (int s = 0; ok && s < count; s++) {
                                QString style = a.section(QLatin1Char(';'), s, s);
                                if (style.startsWith(QLatin1String("margin-top:")) && style.endsWith(QLatin1String("px")))
                                    curpar->utm = 1+style.mid(11, style.length() - 13).toInt(&ok);
                                else if (style.startsWith(QLatin1String("margin-bottom:")) && style.endsWith(QLatin1String("px")))
                                    curpar->ubm = 1+style.mid(14, style.length() - 16).toInt(&ok);
                                else if (style.startsWith(QLatin1String("margin-left:")) && style.endsWith(QLatin1String("px")))
                                    curpar->ulm = 1+style.mid(12, style.length() - 14).toInt(&ok);
                                else if (style.startsWith(QLatin1String("margin-right:")) && style.endsWith(QLatin1String("px")))
                                    curpar->urm = 1+style.mid(13, style.length() - 15).toInt(&ok);
                                else if (style.startsWith(QLatin1String("text-indent:")) && style.endsWith(QLatin1String("px")))
                                    curpar->uflm = 1+style.mid(12, style.length() - 14).toInt(&ok);
                            }
                            if (!ok) // be pressmistic
                                curpar->utm = curpar->ubm = curpar->urm = curpar->ulm = 0;
                        }
                    } else if (nstyle->name() == QLatin1String("html")) {
                        it = attr.find(QLatin1String("dir"));
                        if (it != end) {
                            QString dir = (*it).toLower();
                            if (dir == QLatin1String("rtl"))
                                curtag.direction = QChar::DirR;
                            else if (dir == QLatin1String("ltr"))
                                curtag.direction = QChar::DirL;
                        }
                    }
                }
            } else {
                QString tagname = parseCloseTag(doc, length, pos);
                if (tagname.isEmpty())
                    continue; // nothing we could do with this, probably parse error
                if (!sheet_->item(tagname)) // ignore unknown tags
                    continue;
                if (tagname == QLatin1String("li"))
                    continue;

                // we close a block item. Since the text may continue, we need to have a new paragraph
                bool needNewPar = curtag.style->displayMode() == Q3StyleSheetItem::DisplayBlock
                                 || curtag.style->displayMode() == Q3StyleSheetItem::DisplayListItem;


                // html slopiness: handle unbalanched tag closing
                while (curtag.name != tagname) {
                    QString msg;
                    msg.sprintf("QText Warning: Document not valid ('%s' not closed before '%s' #%d)",
                                 curtag.name.ascii(), tagname.ascii(), pos);
                    sheet_->error(msg);
                    if (tags.isEmpty())
                        break;
                    curtag = tags.pop();
                }


                // close the tag
                if (!tags.isEmpty())
                    curtag = tags.pop();
                else
                    curtag = initag;

                if (needNewPar) {
                    if (textEditMode && (tagname == QLatin1String("p") || tagname == QLatin1String("div"))) // preserve empty paragraphs
                        hasNewPar = false;
                    NEWPAR;
                }
            }
        } else {
            // normal contents
            QString s;
            QChar c;
            while (pos < length && !hasPrefix(doc, length, pos, QLatin1Char('<'))){
                if (textEditMode) {
                    // text edit mode: we handle all white space but ignore newlines
                    c = parseChar(doc, length, pos, Q3StyleSheetItem::WhiteSpacePre);
                    if (c == QChar::LineSeparator)
                        break;
                } else {
                    int l = pos;
                    c = parseChar(doc, length, pos, curtag.wsm);

                    // in white space pre mode: treat any space as non breakable
                    // and expand tabs to eight character wide columns.
                    if (curtag.wsm == Q3StyleSheetItem::WhiteSpacePre) {
                        if  (c == QLatin1Char('\t')) {
                            c = QLatin1Char(' ');
                            while((++tabExpansionColumn)%8)
                                s += c;
                        }
                        if (c == QChar::LineSeparator)
                            tabExpansionColumn = 0;
                        else
                            tabExpansionColumn++;

                    }
                    if (c == QLatin1Char(' ') || c == QChar::LineSeparator) {
                        /* avoid overlong paragraphs by forcing a new
                               paragraph after 4096 characters. This case can
                               occur when loading undiscovered plain text
                               documents in rich text mode. Instead of hanging
                               forever, we do the trick.
                            */
                        if (curtag.wsm == Q3StyleSheetItem::WhiteSpaceNormal && s.length() > 4096) do {
                            if (doc[l] == QLatin1Char('\n')) {
                                hasNewPar = false; // for a new paragraph ...
                                NEWPAR;
                                hasNewPar = false; // ... and make it non-reusable
                                c = QLatin1Char('\n');  // make sure we break below
                                break;
                            }
                        } while (++l < pos);
                    }
                }

                if (c == QLatin1Char('\n'))
                    break;  // break on  newlines, pre delievers a QChar::LineSeparator

                bool c_isSpace = c.isSpace() && c.unicode() != 0x00a0U && !textEditMode;

                if (curtag.wsm == Q3StyleSheetItem::WhiteSpaceNormal && c_isSpace && space)
                    continue;
                if (c == QLatin1Char('\r'))
                    continue;
                space = c_isSpace;
                s += c;
            }
            if (!s.isEmpty() && curtag.style->displayMode() != Q3StyleSheetItem::DisplayNone) {
                hasNewPar = false;
                int index = qMax(curpar->length(),1) - 1;
                curpar->append(s);
                if (curtag.wsm != Q3StyleSheetItem::WhiteSpaceNormal) {
                    Q3TextString *str = curpar->string();
                    for (int i = index; i < index + s.length(); ++i)
                        str->at(i).nobreak = true;
                }

                Q3TextFormat* f = formatCollection()->format(&curtag.format);
                curpar->setFormat(index, s.length(), f, false); // do not use collection because we have done that already
                f->ref += s.length() -1; // that what friends are for...
                if (!curtag.anchorHref.isEmpty()) {
                    for (int i = 0; i < int(s.length()); i++)
                        curpar->at(index + i)->setAnchor(QString(), curtag.anchorHref);
                }
                if (!anchorName.isEmpty() ) {
                    for (int i = 0; i < int(s.length()); i++)
                        curpar->at(index + i)->setAnchor(anchorName, curpar->at(index + i)->anchorHref());
                    anchorName.clear();
                }
            }
        }
    }

    if (hasNewPar && curpar != fParag && !cursor && stylesPar != curpar) {
        // cleanup unused last paragraphs
        curpar = curpar->p;
        delete curpar->n;
    }

    if (!anchorName.isEmpty() ) {
        curpar->at(curpar->length() - 1)->setAnchor(anchorName, curpar->at(curpar->length() - 1)->anchorHref());
        anchorName.clear();
    }

    setRichTextMarginsInternal(styles, stylesPar);

    if (cursor) {
        cursor->gotoPreviousLetter();
        cursor->remove();
    }
    while (!styles.isEmpty())
        delete styles.takeFirst();
    delete vec;
}

void Q3TextDocument::setRichTextMarginsInternal(QList< QVector<Q3StyleSheetItem *> *>& styles, Q3TextParagraph* stylesPar)
{
    // margin and line spacing calculation
    // qDebug("setRichTextMarginsInternal: styles.size() = %d", styles.size());
    QVector<Q3StyleSheetItem *>* prevStyle = 0;
    int stylesIndex = 0;
    QVector<Q3StyleSheetItem *>* curStyle = styles.size() ? styles.first() : 0;
    QVector<Q3StyleSheetItem *>* nextStyle =
        (++stylesIndex) < styles.size() ? styles.at(stylesIndex) : 0;
    while (stylesPar) {
        if (!curStyle) {
            stylesPar = stylesPar->next();
            prevStyle = curStyle;
            curStyle = nextStyle;
            nextStyle = (++stylesIndex) < styles.size() ? styles.at(stylesIndex) : 0;
            continue;
        }

        int i, mar;
        Q3StyleSheetItem* mainStyle = curStyle->size() ? (*curStyle)[curStyle->size()-1] : 0;
        if (mainStyle && mainStyle->displayMode() == Q3StyleSheetItem::DisplayListItem)
            stylesPar->setListItem(true);
        int numLists = 0;
        for (i = 0; i < (int)curStyle->size(); ++i) {
            if ((*curStyle)[i]->displayMode() == Q3StyleSheetItem::DisplayBlock
                 && (*curStyle)[i]->listStyle() != Q3StyleSheetItem::ListStyleUndefined)
                numLists++;
        }
        stylesPar->ldepth = numLists;
        if (stylesPar->next() && nextStyle) {
            // also set the depth of the next paragraph, required for the margin calculation
            numLists = 0;
            for (i = 0; i < (int)nextStyle->size(); ++i) {
                if ((*nextStyle)[i]->displayMode() == Q3StyleSheetItem::DisplayBlock
                     && (*nextStyle)[i]->listStyle() != Q3StyleSheetItem::ListStyleUndefined)
                    numLists++;
            }
            stylesPar->next()->ldepth = numLists;
        }

        // do the top margin
        Q3StyleSheetItem* item = mainStyle;
        int m;
        if (stylesPar->utm > 0) {
            m = stylesPar->utm-1;
            stylesPar->utm = 0;
        } else {
            m = qMax(0, item->margin(Q3StyleSheetItem::MarginTop));
            if (stylesPar->ldepth) {
                if (item->displayMode() == Q3StyleSheetItem::DisplayListItem)
                    m /= stylesPar->ldepth * stylesPar->ldepth;
                else
                    m = 0;
            }
        }
        for (i = (int)curStyle->size() - 2 ; i >= 0; --i) {
            item = (*curStyle)[i];
            if (prevStyle && i < (int) prevStyle->size() &&
                 ( item->displayMode() == Q3StyleSheetItem::DisplayBlock &&
                    (*prevStyle)[i] == item))
                break;
            // emulate CSS2' standard 0 vertical margin for multiple ul or ol tags
            if (item->listStyle() != Q3StyleSheetItem::ListStyleUndefined  &&
                 (( i> 0 && (*curStyle)[i-1] == item) || (*curStyle)[i+1] == item))
                continue;
            mar = qMax(0, item->margin(Q3StyleSheetItem::MarginTop));
            m = qMax(m, mar);
        }
        stylesPar->utm = m - stylesPar->topMargin();

        // do the bottom margin
        item = mainStyle;
        if (stylesPar->ubm > 0) {
            m = stylesPar->ubm-1;
            stylesPar->ubm = 0;
        } else {
            m = qMax(0, item->margin(Q3StyleSheetItem::MarginBottom));
            if (stylesPar->ldepth) {
                if (item->displayMode() == Q3StyleSheetItem::DisplayListItem)
                    m /= stylesPar->ldepth * stylesPar->ldepth;
                else
                    m = 0;
            }
        }
        for (i = (int)curStyle->size() - 2 ; i >= 0; --i) {
            item = (*curStyle)[i];
            if (nextStyle && i < (int) nextStyle->size() &&
                 ( item->displayMode() == Q3StyleSheetItem::DisplayBlock &&
                    (*nextStyle)[i] == item))
                break;
            // emulate CSS2' standard 0 vertical margin for multiple ul or ol tags
            if (item->listStyle() != Q3StyleSheetItem::ListStyleUndefined  &&
                 (( i> 0 && (*curStyle)[i-1] == item) || (*curStyle)[i+1] == item))
                continue;
            mar = qMax(0, item->margin(Q3StyleSheetItem::MarginBottom));
            m = qMax(m, mar);
        }
        stylesPar->ubm = m - stylesPar->bottomMargin();

        // do the left margin, simplyfied
        item = mainStyle;
        if (stylesPar->ulm > 0) {
            m = stylesPar->ulm-1;
            stylesPar->ulm = 0;
        } else {
            m = qMax(0, item->margin(Q3StyleSheetItem::MarginLeft));
        }
        for (i = (int)curStyle->size() - 2 ; i >= 0; --i) {
            item = (*curStyle)[i];
            m += qMax(0, item->margin(Q3StyleSheetItem::MarginLeft));
        }
        stylesPar->ulm = m - stylesPar->leftMargin();

        // do the right margin, simplyfied
        item = mainStyle;
        if (stylesPar->urm > 0) {
            m = stylesPar->urm-1;
            stylesPar->urm = 0;
        } else {
            m = qMax(0, item->margin(Q3StyleSheetItem::MarginRight));
        }
        for (i = (int)curStyle->size() - 2 ; i >= 0; --i) {
            item = (*curStyle)[i];
            m += qMax(0, item->margin(Q3StyleSheetItem::MarginRight));
        }
        stylesPar->urm = m - stylesPar->rightMargin();

        // do the first line margin, which really should be called text-indent
        item = mainStyle;
        if (stylesPar->uflm > 0) {
            m = stylesPar->uflm-1;
            stylesPar->uflm = 0;
        } else {
            m = qMax(0, item->margin(Q3StyleSheetItem::MarginFirstLine));
        }
        for (i = (int)curStyle->size() - 2 ; i >= 0; --i) {
            item = (*curStyle)[i];
            mar = qMax(0, item->margin(Q3StyleSheetItem::MarginFirstLine));
            m = qMax(m, mar);
        }
        stylesPar->uflm =m - stylesPar->firstLineMargin();

        // do the bogus line "spacing", which really is just an extra margin
        item = mainStyle;
        for (i = (int)curStyle->size() - 1 ; i >= 0; --i) {
            item = (*curStyle)[i];
            if (item->lineSpacing() != Q3StyleSheetItem::Undefined) {
                stylesPar->ulinespacing = item->lineSpacing();
                if (formatCollection() &&
                     stylesPar->ulinespacing < formatCollection()->defaultFormat()->height())
                    stylesPar->ulinespacing += formatCollection()->defaultFormat()->height();
                break;
            }
        }

        stylesPar = stylesPar->next();
        prevStyle = curStyle;
        curStyle = nextStyle;
        nextStyle = (++stylesIndex) < styles.size() ? styles.at(stylesIndex) : 0;
    }
}

void Q3TextDocument::setText(const QString &text, const QString &context)
{
    focusIndicator.parag = 0;
    selections.clear();
    if ((txtFormat == Qt::AutoText && Q3StyleSheet::mightBeRichText(text))
        || txtFormat == Qt::RichText)
        setRichText(text, context);
    else
        setPlainText(text);
}

QString Q3TextDocument::plainText() const
{
    QString buffer;
    QString s;
    Q3TextParagraph *p = fParag;
    while (p) {
        if (!p->mightHaveCustomItems) {
            const Q3TextString *ts = p->string(); // workaround VC++ and Borland
            s = ts->toString(); // with false we don't fix spaces (nbsp)
        } else {
            for (int i = 0; i < p->length() - 1; ++i) {
#ifndef QT_NO_TEXTCUSTOMITEM
                if (p->at(i)->isCustom()) {
                    if (p->at(i)->customItem()->isNested()) {
                        s += QLatin1String("\n");
                        Q3TextTable *t = (Q3TextTable*)p->at(i)->customItem();
                        QList<Q3TextTableCell *> cells = t->tableCells();
                        for (int idx = 0; idx < cells.size(); ++idx) {
                            Q3TextTableCell *c = cells.at(idx);
                            s += c->richText()->plainText() + QLatin1String("\n");
                        }
                        s += QLatin1String("\n");
                    }
                } else
#endif
                {
                    s += p->at(i)->c;
                }
            }
        }
        s.remove(s.length() - 1, 1);
        if (p->next())
            s += QLatin1String("\n");
        buffer += s;
        p = p->next();
    }
    return buffer;
}

static QString align_to_string(int a)
{
    if (a & Qt::AlignRight)
        return QLatin1String(" align=\"right\"");
    if (a & Qt::AlignHCenter)
        return QLatin1String(" align=\"center\"");
    if (a & Qt::AlignJustify)
        return QLatin1String(" align=\"justify\"");
    return QString();
}

static QString direction_to_string(int dir)
{
    if (dir != QChar::DirON)
        return (dir == QChar::DirL? QLatin1String(" dir=\"ltr\"") : QLatin1String(" dir=\"rtl\""));
    return QString();
}

static QString list_value_to_string(int v)
{
    if (v != -1)
        return QLatin1String(" listvalue=\"") + QString::number(v) + QLatin1Char('"');
    return QString();
}

static QString list_style_to_string(int v)
{
    switch(v) {
    case Q3StyleSheetItem::ListDecimal: return QLatin1String("\"1\"");
    case Q3StyleSheetItem::ListLowerAlpha: return QLatin1String("\"a\"");
    case Q3StyleSheetItem::ListUpperAlpha: return QLatin1String("\"A\"");
    case Q3StyleSheetItem::ListDisc: return QLatin1String("\"disc\"");
    case Q3StyleSheetItem::ListSquare: return QLatin1String("\"square\"");
    case Q3StyleSheetItem::ListCircle: return QLatin1String("\"circle\"");
    default:
        return QString();
    }
}

static inline bool list_is_ordered(int v)
{
    return v == Q3StyleSheetItem::ListDecimal ||
           v == Q3StyleSheetItem::ListLowerAlpha ||
           v == Q3StyleSheetItem::ListUpperAlpha;
}


static QString margin_to_string(Q3StyleSheetItem* style, int t, int b, int l, int r, int fl)
{
    QString s;
    if (l > 0)
        s += QString(s.size() ? QLatin1String(";") : QLatin1String("")) + QLatin1String("margin-left:") +
             QString::number(l+qMax(0,style->margin(Q3StyleSheetItem::MarginLeft))) + QLatin1String("px");
    if (r > 0)
        s += QString(s.size() ? QLatin1String(";") : QLatin1String("")) + QLatin1String("margin-right:") +
             QString::number(r+qMax(0,style->margin(Q3StyleSheetItem::MarginRight))) + QLatin1String("px");
    if (t > 0)
        s += QString(s.size() ? QLatin1String(";") : QLatin1String("")) + QLatin1String("margin-top:") +
             QString::number(t+qMax(0,style->margin(Q3StyleSheetItem::MarginTop))) + QLatin1String("px");
    if (b > 0)
        s += QString(s.size() ? QLatin1String(";") : QLatin1String("")) + QLatin1String("margin-bottom:") +
             QString::number(b+qMax(0,style->margin(Q3StyleSheetItem::MarginBottom))) + QLatin1String("px");
    if (fl > 0)
        s += QString(s.size() ? QLatin1String(";") : QLatin1String("")) + QLatin1String("text-indent:") +
             QString::number(fl+qMax(0,style->margin(Q3StyleSheetItem::MarginFirstLine))) + QLatin1String("px");
    if (s.size())
        return QLatin1String(" style=\"") + s + QLatin1String("\"");
    return QString();
}

QString Q3TextDocument::richText() const
{
    QString s = QLatin1String("");
    if (!par) {
        s += QLatin1String("<html><head><meta name=\"qrichtext\" content=\"1\" /></head><body style=\"font-size:");
        s += QString::number(formatCollection()->defaultFormat()->font().pointSize());
        s += QLatin1String("pt;font-family:");
        s += formatCollection()->defaultFormat()->font().family();
        s += QLatin1String("\">");
    }
    Q3TextParagraph* p = fParag;

    Q3StyleSheetItem* item_p = styleSheet()->item(QLatin1String("p"));
    Q3StyleSheetItem* item_div = styleSheet()->item(QLatin1String("div"));
    Q3StyleSheetItem* item_ul = styleSheet()->item(QLatin1String("ul"));
    Q3StyleSheetItem* item_ol = styleSheet()->item(QLatin1String("ol"));
    Q3StyleSheetItem* item_li = styleSheet()->item(QLatin1String("li"));
    if (!item_p || !item_div || !item_ul || !item_ol || !item_li) {
        qWarning("QTextEdit: cannot export HTML due to insufficient stylesheet (lack of p, div, ul, ol, or li)");
        return QString();
    }
    int pastListDepth = 0;
    int listDepth = 0;
#if 0
    int futureListDepth = 0;
#endif
    QVector<int> listStyles(10);

    while (p) {
        listDepth = p->listDepth();
        if (listDepth < pastListDepth)  {
            for (int i = pastListDepth; i > listDepth; i--)
                s += list_is_ordered(listStyles[i]) ? QLatin1String("</ol>") : QLatin1String("</ul>");
            s += QLatin1Char('\n');
        } else if (listDepth > pastListDepth) {
            s += QLatin1Char('\n');
            listStyles.resize(qMax((int)listStyles.size(), listDepth+1));
            QString list_type;
            listStyles[listDepth] = p->listStyle();
            if (!list_is_ordered(p->listStyle()) || item_ol->listStyle() != p->listStyle())
                list_type = QLatin1String(" type=") + list_style_to_string(p->listStyle());
            for (int i = pastListDepth; i < listDepth; i++) {
                s += list_is_ordered(p->listStyle()) ? QLatin1String("<ol") : QLatin1String("<ul");
                s += list_type + QLatin1String(">");
            }
        } else {
            s += QLatin1Char('\n');
        }

        QString ps = p->richText();

#if 0
          // for the bottom margin we need to know whether we are at the end of a list
        futureListDepth = 0;
        if (listDepth > 0 && p->next())
            futureListDepth = p->next()->listDepth();
#endif

        if (richTextExportStart && richTextExportStart->paragraph() ==p &&
             richTextExportStart->index() == 0)
            s += QLatin1String("<!--StartFragment-->");

        if (p->isListItem()) {
            s += QLatin1String("<li");
            if (p->listStyle() != listStyles[listDepth])
                s += QLatin1String(" type=") + list_style_to_string(p->listStyle());
            s += align_to_string(p->alignment());
            s += margin_to_string(item_li, p->utm, p->ubm, p->ulm, p->urm, p->uflm);
            s += list_value_to_string(p->listValue());
            s += direction_to_string(p->direction());
            s += QLatin1String(">");
            s += ps;
            s += QLatin1String("</li>");
        } else if (p->listDepth()) {
            s += QLatin1String("<div");
            s += align_to_string(p->alignment());
            s += margin_to_string(item_div, p->utm, p->ubm, p->ulm, p->urm, p->uflm);
            s += direction_to_string(p->direction());
            s += QLatin1String(">");
            s += ps;
            s += QLatin1String("</div>");
        } else {
            // normal paragraph item
            s += QLatin1String("<p");
            s += align_to_string(p->alignment());
            s += margin_to_string(item_p, p->utm, p->ubm, p->ulm, p->urm, p->uflm);
            s += direction_to_string(p->direction());
            s += QLatin1String(">");
            s += ps;
            s += QLatin1String("</p>");
        }
        pastListDepth = listDepth;
        p = p->next();
    }
    while (listDepth > 0) {
        s += list_is_ordered(listStyles[listDepth]) ? QLatin1String("</ol>") : QLatin1String("</ul>");
        listDepth--;
    }

    if (!par)
        s += QLatin1String("\n</body></html>\n");

    return s;
}

QString Q3TextDocument::text() const
{
    if ((txtFormat == Qt::AutoText && preferRichText) || txtFormat == Qt::RichText)
        return richText();
    return plainText();
}

QString Q3TextDocument::text(int parag) const
{
    Q3TextParagraph *p = paragAt(parag);
    if (!p)
        return QString();

    if ((txtFormat == Qt::AutoText && preferRichText) || txtFormat == Qt::RichText)
        return p->richText();
    else
        return p->string()->toString();
}

void Q3TextDocument::invalidate()
{
    Q3TextParagraph *s = fParag;
    while (s) {
        s->invalidate(0);
        s = s->next();
    }
}

void Q3TextDocument::selectionStart(int id, int &paragId, int &index)
{
    QMap<int, Q3TextDocumentSelection>::Iterator it = selections.find(id);
    if (it == selections.end())
        return;
    Q3TextDocumentSelection &sel = *it;
    paragId = !sel.swapped ? sel.startCursor.paragraph()->paragId() : sel.endCursor.paragraph()->paragId();
    index = !sel.swapped ? sel.startCursor.index() : sel.endCursor.index();
}

Q3TextCursor Q3TextDocument::selectionStartCursor(int id)
{
    QMap<int, Q3TextDocumentSelection>::Iterator it = selections.find(id);
    if (it == selections.end())
        return Q3TextCursor(this);
    Q3TextDocumentSelection &sel = *it;
    if (sel.swapped)
        return sel.endCursor;
    return sel.startCursor;
}

Q3TextCursor Q3TextDocument::selectionEndCursor(int id)
{
    QMap<int, Q3TextDocumentSelection>::Iterator it = selections.find(id);
    if (it == selections.end())
        return Q3TextCursor(this);
    Q3TextDocumentSelection &sel = *it;
    if (!sel.swapped)
        return sel.endCursor;
    return sel.startCursor;
}

void Q3TextDocument::selectionEnd(int id, int &paragId, int &index)
{
    QMap<int, Q3TextDocumentSelection>::Iterator it = selections.find(id);
    if (it == selections.end())
        return;
    Q3TextDocumentSelection &sel = *it;
    paragId = sel.swapped ? sel.startCursor.paragraph()->paragId() : sel.endCursor.paragraph()->paragId();
    index = sel.swapped ? sel.startCursor.index() : sel.endCursor.index();
}

void Q3TextDocument::addSelection(int id)
{
    nSelections = qMax(nSelections, id + 1);
}

static void setSelectionEndHelper(int id, Q3TextDocumentSelection &sel, Q3TextCursor &start, Q3TextCursor &end)
{
    Q3TextCursor c1 = start;
    Q3TextCursor c2 = end;
    if (sel.swapped) {
        c1 = end;
        c2 = start;
    }

    c1.paragraph()->removeSelection(id);
    c2.paragraph()->removeSelection(id);
    if (c1.paragraph() != c2.paragraph()) {
        c1.paragraph()->setSelection(id, c1.index(), c1.paragraph()->length() - 1);
        c2.paragraph()->setSelection(id, 0, c2.index());
    } else {
        c1.paragraph()->setSelection(id, qMin(c1.index(), c2.index()), qMax(c1.index(), c2.index()));
    }

    sel.startCursor = start;
    sel.endCursor = end;
    if (sel.startCursor.paragraph() == sel.endCursor.paragraph())
        sel.swapped = sel.startCursor.index() > sel.endCursor.index();
}

bool Q3TextDocument::setSelectionEnd(int id, const Q3TextCursor &cursor)
{
    QMap<int, Q3TextDocumentSelection>::Iterator it = selections.find(id);
    if (it == selections.end())
        return false;
    Q3TextDocumentSelection &sel = *it;

    Q3TextCursor start = sel.startCursor;
    Q3TextCursor end = cursor;

    if (start == end) {
        removeSelection(id);
        setSelectionStart(id, cursor);
        return true;
    }

    if (sel.endCursor.paragraph() == end.paragraph()) {
        setSelectionEndHelper(id, sel, start, end);
        return true;
    }

    bool inSelection = false;
    Q3TextCursor c(this);
    Q3TextCursor tmp = sel.startCursor;
    if (sel.swapped)
        tmp = sel.endCursor;
    tmp.restoreState();
    Q3TextCursor tmp2 = cursor;
    tmp2.restoreState();
    c.setParagraph(tmp.paragraph()->paragId() < tmp2.paragraph()->paragId() ? tmp.paragraph() : tmp2.paragraph());
    bool hadStart = false;
    bool hadEnd = false;
    bool hadStartParag = false;
    bool hadEndParag = false;
    bool hadOldStart = false;
    bool hadOldEnd = false;
    bool leftSelection = false;
    sel.swapped = false;
    for (;;) {
        if (c == start)
            hadStart = true;
        if (c == end)
            hadEnd = true;
        if (c.paragraph() == start.paragraph())
            hadStartParag = true;
        if (c.paragraph() == end.paragraph())
            hadEndParag = true;
        if (c == sel.startCursor)
            hadOldStart = true;
        if (c == sel.endCursor)
            hadOldEnd = true;

        if (!sel.swapped &&
             ((hadEnd && !hadStart)
              || (hadEnd && hadStart && start.paragraph() == end.paragraph() && start.index() > end.index())))
            sel.swapped = true;

        if ((c == end && hadStartParag) || (c == start && hadEndParag)) {
            Q3TextCursor tmp = c;
            tmp.restoreState();
            if (tmp.paragraph() != c.paragraph()) {
                int sstart = tmp.paragraph()->selectionStart(id);
                tmp.paragraph()->removeSelection(id);
                tmp.paragraph()->setSelection(id, sstart, tmp.index());
            }
        }

        if (inSelection &&
             ((c == end && hadStart) || (c == start && hadEnd)))
             leftSelection = true;
        else if (!leftSelection && !inSelection && (hadStart || hadEnd))
            inSelection = true;

        bool noSelectionAnymore = hadOldStart && hadOldEnd && leftSelection && !inSelection && !c.paragraph()->hasSelection(id) && c.atParagEnd();
        c.paragraph()->removeSelection(id);
        if (inSelection) {
            if (c.paragraph() == start.paragraph() && start.paragraph() == end.paragraph()) {
                c.paragraph()->setSelection(id, qMin(start.index(), end.index()), qMax(start.index(), end.index()));
            } else if (c.paragraph() == start.paragraph() && !hadEndParag) {
                c.paragraph()->setSelection(id, start.index(), c.paragraph()->length() - 1);
            } else if (c.paragraph() == end.paragraph() && !hadStartParag) {
                c.paragraph()->setSelection(id, end.index(), c.paragraph()->length() - 1);
            } else if (c.paragraph() == end.paragraph() && hadEndParag) {
                c.paragraph()->setSelection(id, 0, end.index());
            } else if (c.paragraph() == start.paragraph() && hadStartParag) {
                c.paragraph()->setSelection(id, 0, start.index());
            } else {
                c.paragraph()->setSelection(id, 0, c.paragraph()->length() - 1);
            }
        }

        if (leftSelection)
            inSelection = false;

        if (noSelectionAnymore)
            break;
        // *ugle*hack optimization
        Q3TextParagraph *p = c.paragraph();
        if ( p->mightHaveCustomItems || p == start.paragraph() || p == end.paragraph() || p == lastParagraph()) {
            c.gotoNextLetter();
            if (p == lastParagraph() && c.atParagEnd())
                break;
        } else {
            if (p->document()->parent())
                do {
                    c.gotoNextLetter();
                } while (c.paragraph() == p);
            else
                c.setParagraph(p->next());
        }
    }

    if (!sel.swapped)
        sel.startCursor.paragraph()->setSelection(id, sel.startCursor.index(), sel.startCursor.paragraph()->length() - 1);

    sel.startCursor = start;
    sel.endCursor = end;
    if (sel.startCursor.paragraph() == sel.endCursor.paragraph())
        sel.swapped = sel.startCursor.index() > sel.endCursor.index();

    setSelectionEndHelper(id, sel, start, end);

    return true;
}

void Q3TextDocument::selectAll(int id)
{
    removeSelection(id);

    Q3TextDocumentSelection sel;
    sel.swapped = false;
    Q3TextCursor c(this);

    c.setParagraph(fParag);
    c.setIndex(0);
    sel.startCursor = c;

    c.setParagraph(lParag);
    c.setIndex(lParag->length() - 1);
    sel.endCursor = c;

    selections.insert(id, sel);

    Q3TextParagraph *p = fParag;
    while (p) {
        p->setSelection(id, 0, p->length() - 1);
        p = p->next();
    }

    for (int idx = 0; idx < childList.size(); ++idx) {
        Q3TextDocument *dc = childList.at(idx);
        dc->selectAll(id);
    }
}

bool Q3TextDocument::removeSelection(int id)
{
    if (!selections.contains(id))
        return false;

    Q3TextDocumentSelection &sel = selections[id];

    Q3TextCursor start = sel.swapped ? sel.endCursor : sel.startCursor;
    Q3TextCursor end = sel.swapped ? sel.startCursor : sel.endCursor;
    Q3TextParagraph* p = 0;
    while (start != end) {
        if (p != start.paragraph()) {
            p = start.paragraph();
            p->removeSelection(id);
            //### avoid endless loop by all means necessary, did somebody mention refactoring?
            if (!parent() && p == lParag)
                break;
        }
        start.gotoNextLetter();
    }
    p = start.paragraph();
    p->removeSelection(id);
    selections.remove(id);
    return true;
}

QString Q3TextDocument::selectedText(int id, bool asRichText) const
{
    QMap<int, Q3TextDocumentSelection>::ConstIterator it = selections.find(id);
    if (it == selections.end())
        return QString();

    Q3TextDocumentSelection sel = *it;


    Q3TextCursor c1 = sel.startCursor;
    Q3TextCursor c2 = sel.endCursor;
    if (sel.swapped) {
        c2 = sel.startCursor;
        c1 = sel.endCursor;
    }

    /* 3.0.3 improvement: Make it possible to get a reasonable
       selection inside a table.  This approach is very conservative:
       make sure that both cursors have the same depth level and point
       to paragraphs within the same text document.

       Meaning if you select text in two table cells, you will get the
       entire table. This is still far better than the 3.0.2, where
       you always got the entire table.

       ### Fix this properly when refactoring
     */
    while (c2.nestedDepth() > c1.nestedDepth())
        c2.oneUp();
    while (c1.nestedDepth() > c2.nestedDepth())
        c1.oneUp();
    while (c1.nestedDepth() && c2.nestedDepth() &&
            c1.paragraph()->document() != c2.paragraph()->document()) {
        c1.oneUp();
        c2.oneUp();
    }
    // do not trust sel_swapped with tables. Fix this properly when refactoring as well
    if (c1.paragraph()->paragId() > c2.paragraph()->paragId() ||
         (c1.paragraph() == c2.paragraph() && c1.index() > c2.index())) {
        Q3TextCursor tmp = c1;
        c2 = c1;
        c1 = tmp;
    }

    // end selection 3.0.3 improvement

    if (asRichText && !parent()) {
        richTextExportStart = &c1;
        richTextExportEnd = &c2;

        QString sel = richText();
        int from = sel.indexOf(QLatin1String("<!--StartFragment-->"));
        if (from >= 0) {
            from += 20;
            // find the previous span and move it into the start fragment before we clip it
            QString prevspan;
            int pspan = sel.lastIndexOf(QLatin1String("<span"), from-21);
            if (pspan > sel.lastIndexOf(QLatin1String("</span"), from-21)) {
                int spanend = sel.indexOf(QLatin1Char('>'), pspan);
                prevspan = sel.mid(pspan, spanend - pspan + 1);
            }
            int to = sel.lastIndexOf(QLatin1String("<!--EndFragment-->"));
            if (from <= to)
                sel = QLatin1String("<!--StartFragment-->") + prevspan + sel.mid(from, to - from);
        }
        richTextExportStart = richTextExportEnd = 0;
        return sel;
    }

    QString s;
    if (c1.paragraph() == c2.paragraph()) {
        Q3TextParagraph *p = c1.paragraph();
        int end = c2.index();
        if (p->at(qMax(0, end - 1))->isCustom())
            ++end;
        if (!p->mightHaveCustomItems) {
            s += p->string()->toString().mid(c1.index(), end - c1.index());
        } else {
            for (int i = c1.index(); i < end; ++i) {
#ifndef QT_NO_TEXTCUSTOMITEM
                if (p->at(i)->isCustom()) {
                    if (p->at(i)->customItem()->isNested()) {
                        s += QLatin1String("\n");
                        Q3TextTable *t = (Q3TextTable*)p->at(i)->customItem();
                        QList<Q3TextTableCell *> cells = t->tableCells();
                        for (int idx = 0; idx < cells.size(); ++idx) {
                            Q3TextTableCell *c = cells.at(idx);
                            s += c->richText()->plainText() + QLatin1String("\n");
                        }
                        s += QLatin1String("\n");
                    }
                } else
#endif
                {
                    s += p->at(i)->c;
                }
            }
        }
    } else {
        Q3TextParagraph *p = c1.paragraph();
        int start = c1.index();
        while (p) {
            int end = p == c2.paragraph() ? c2.index() : p->length() - 1;
            if (p == c2.paragraph() && p->at(qMax(0, end - 1))->isCustom())
                ++end;
            if (!p->mightHaveCustomItems) {
                s += p->string()->toString().mid(start, end - start);
                if (p != c2.paragraph())
                    s += QLatin1String("\n");
            } else {
                for (int i = start; i < end; ++i) {
#ifndef QT_NO_TEXTCUSTOMITEM
                    if (p->at(i)->isCustom()) {
                        if (p->at(i)->customItem()->isNested()) {
                            s += QLatin1String(QLatin1String("\n"));
                            Q3TextTable *t = (Q3TextTable*)p->at(i)->customItem();
                            QList<Q3TextTableCell *> cells = t->tableCells();
                            for (int idx = 0; idx < cells.size(); ++idx) {
                                Q3TextTableCell *c = cells.at(idx);
                                s += c->richText()->plainText() + QLatin1String("\n");
                            }
                            s += QLatin1String("\n");
                        }
                    } else
#endif
                    {
                        s += p->at(i)->c;
                    }
                }
            }
            start = 0;
            if (p == c2.paragraph())
                break;
            p = p->next();
        }
    }
    // ### workaround for plain text export until we get proper
    // mime types: turn unicode line seperators into the more
    // widely understood \n. Makes copy and pasting code snipplets
    // from within Assistent possible
    QChar* uc = (QChar*) s.unicode();
    for (int ii = 0; ii < s.length(); ii++) {
        if (uc[(int)ii] == QChar::LineSeparator)
            uc[(int)ii] = QLatin1Char('\n');
        else if ( uc[(int)ii] == QChar::Nbsp )
            uc[(int)ii] = QLatin1Char(' ');
    }
    return s;
}

void Q3TextDocument::setFormat(int id, Q3TextFormat *f, int flags)
{
    QMap<int, Q3TextDocumentSelection>::ConstIterator it = selections.constFind(id);
    if (it == selections.constEnd())
        return;

    Q3TextDocumentSelection sel = *it;

    Q3TextCursor c1 = sel.startCursor;
    Q3TextCursor c2 = sel.endCursor;
    if (sel.swapped) {
        c2 = sel.startCursor;
        c1 = sel.endCursor;
    }

    c2.restoreState();
    c1.restoreState();

    if (c1.paragraph() == c2.paragraph()) {
        c1.paragraph()->setFormat(c1.index(), c2.index() - c1.index(), f, true, flags);
        return;
    }

    c1.paragraph()->setFormat(c1.index(), c1.paragraph()->length() - c1.index(), f, true, flags);
    Q3TextParagraph *p = c1.paragraph()->next();
    while (p && p != c2.paragraph()) {
        p->setFormat(0, p->length(), f, true, flags);
        p = p->next();
    }
    c2.paragraph()->setFormat(0, c2.index(), f, true, flags);
}

void Q3TextDocument::removeSelectedText(int id, Q3TextCursor *cursor)
{
    QMap<int, Q3TextDocumentSelection>::Iterator it = selections.find(id);
    if (it == selections.end())
        return;

    Q3TextDocumentSelection sel = *it;
    Q3TextCursor c1 = sel.startCursor;
    Q3TextCursor c2 = sel.endCursor;
    if (sel.swapped) {
        c2 = sel.startCursor;
        c1 = sel.endCursor;
    }

    // ### no support for editing tables yet
    if (c1.nestedDepth() || c2.nestedDepth())
        return;

    c2.restoreState();
    c1.restoreState();

    *cursor = c1;
    removeSelection(id);

    if (c1.paragraph() == c2.paragraph()) {
        c1.paragraph()->remove(c1.index(), c2.index() - c1.index());
        return;
    }

    if (c1.paragraph() == fParag && c1.index() == 0 &&
         c2.paragraph() == lParag && c2.index() == lParag->length() - 1)
        cursor->setValid(false);

    bool didGoLeft = false;
    if ( c1.index() == 0 && c1.paragraph() != fParag) {
        cursor->gotoPreviousLetter();
        didGoLeft = cursor->isValid();
    }

    c1.paragraph()->remove(c1.index(), c1.paragraph()->length() - 1 - c1.index());
    Q3TextParagraph *p = c1.paragraph()->next();
    int dy = 0;
    Q3TextParagraph *tmp;
    while (p && p != c2.paragraph()) {
        tmp = p->next();
        dy -= p->rect().height();
        delete p;
        p = tmp;
    }
    c2.paragraph()->remove(0, c2.index());
    while (p) {
        p->move(dy);
        p->invalidate(0);
        p->setEndState(-1);
        p = p->next();
    }


    c1.paragraph()->join(c2.paragraph());

    if (didGoLeft)
        cursor->gotoNextLetter();
}

void Q3TextDocument::indentSelection(int id)
{
    QMap<int, Q3TextDocumentSelection>::Iterator it = selections.find(id);
    if (it == selections.end())
        return;

    Q3TextDocumentSelection sel = *it;
    Q3TextParagraph *startParag = sel.startCursor.paragraph();
    Q3TextParagraph *endParag = sel.endCursor.paragraph();
    if (sel.endCursor.paragraph()->paragId() < sel.startCursor.paragraph()->paragId()) {
        endParag = sel.startCursor.paragraph();
        startParag = sel.endCursor.paragraph();
    }

    Q3TextParagraph *p = startParag;
    while (p && p != endParag) {
        p->indent();
        p = p->next();
    }
}

void Q3TextCommandHistory::clear()
{
    while (!history.isEmpty())
        delete history.takeFirst();
    current = -1;
}

void Q3TextDocument::addCommand(Q3TextCommand *cmd)
{
    commandHistory->addCommand(cmd);
}

Q3TextCursor *Q3TextDocument::undo(Q3TextCursor *c)
{
    return commandHistory->undo(c);
}

Q3TextCursor *Q3TextDocument::redo(Q3TextCursor *c)
{
    return commandHistory->redo(c);
}

bool Q3TextDocument::find(Q3TextCursor& cursor, const QString &expr, bool cs, bool wo, bool forward)
{
    Qt::CaseSensitivity caseSensitive = cs ? Qt::CaseSensitive : Qt::CaseInsensitive;
    removeSelection(Standard);
    if (expr.isEmpty())
        return false;
    for (;;) {
	QString s = cursor.paragraph()->string()->toString();
	int start = cursor.index();
	for (;;) {
	    int res = forward
		? s.indexOf(expr, start, caseSensitive)
		: s.lastIndexOf(expr, start, caseSensitive);
	    int end = res + expr.length();
	    if (res == -1 || (!forward && start <= res))
		break;
	    if (!wo || ((res == 0 || !s[res-1].isLetterOrNumber())
		&& (end == (int)s.length() || !s[end].isLetterOrNumber()))) {
		removeSelection(Standard);
		cursor.setIndex(forward ? end : res);
		setSelectionStart(Standard, cursor);
		cursor.setIndex(forward ? res : end);
		setSelectionEnd(Standard, cursor);
		if (!forward)
		    cursor.setIndex(res);
		return true;
	    }
	    start = res + (forward ? 1 : -1);
	}
        if (forward) {
            if (cursor.paragraph() == lastParagraph() && cursor.atParagEnd())
                 break;
            cursor.gotoNextLetter();
        } else {
            if (cursor.paragraph() == firstParagraph() && cursor.atParagStart())
                 break;
            cursor.gotoPreviousLetter();
        }
    }
    return false;
}

void Q3TextDocument::setTextFormat(Qt::TextFormat f)
{
    txtFormat = f;
    if (fParag == lParag && fParag->length() <= 1)
        fParag->rtext = (f == Qt::RichText);
}

Qt::TextFormat Q3TextDocument::textFormat() const
{
    return txtFormat;
}

bool Q3TextDocument::inSelection(int selId, const QPoint &pos) const
{
    QMap<int, Q3TextDocumentSelection>::ConstIterator it = selections.find(selId);
    if (it == selections.end())
        return false;

    Q3TextDocumentSelection sel = *it;
    Q3TextParagraph *startParag = sel.startCursor.paragraph();
    Q3TextParagraph *endParag = sel.endCursor.paragraph();
    if (sel.startCursor.paragraph() == sel.endCursor.paragraph() &&
         sel.startCursor.paragraph()->selectionStart(selId) == sel.endCursor.paragraph()->selectionEnd(selId))
        return false;
    if (sel.endCursor.paragraph()->paragId() < sel.startCursor.paragraph()->paragId()) {
        endParag = sel.startCursor.paragraph();
        startParag = sel.endCursor.paragraph();
    }

    Q3TextParagraph *p = startParag;
    while (p) {
        if (p->rect().contains(pos)) {
            bool inSel = false;
            int selStart = p->selectionStart(selId);
            int selEnd = p->selectionEnd(selId);
            int y = 0;
            int h = 0;
            for (int i = 0; i < p->length(); ++i) {
                if (i == selStart)
                    inSel = true;
                if (i == selEnd)
                    break;
                if (p->at(i)->lineStart) {
                    y = (*p->lineStarts.find(i))->y;
                    h = (*p->lineStarts.find(i))->h;
                }
                if (pos.y() - p->rect().y() >= y && pos.y() - p->rect().y() <= y + h) {
                    if (inSel && pos.x() >= p->at(i)->x &&
                         pos.x() <= p->at(i)->x + p->at(i)->format()->width(p->at(i)->c))
                        return true;
                }
            }
        }
        if (pos.y() < p->rect().y())
            break;
        if (p == endParag)
            break;
        p = p->next();
    }

    return false;
}

void Q3TextDocument::doLayout(QPainter *p, int w)
{
    minw = wused = 0;
    if (!is_printer(p))
        p = 0;
    withoutDoubleBuffer = (p != 0);
    QPainter * oldPainter = Q3TextFormat::painter();
    Q3TextFormat::setPainter(p);
    tStopWidth = formatCollection()->defaultFormat()->width( QLatin1Char('x') ) * 8;
    flow_->setWidth(w);
    cw = w;
    vw = w;
    Q3TextParagraph *parag = fParag;
    while (parag) {
        parag->invalidate(0);
        if (p)
            parag->adjustToPainter(p);
        parag->format();
        parag = parag->next();
    }
    Q3TextFormat::setPainter(oldPainter);
}

QPixmap *Q3TextDocument::bufferPixmap(const QSize &s)
{
    if (!buf_pixmap)
        buf_pixmap = new QPixmap(s.expandedTo(QSize(1,1)));
    else if (buf_pixmap->size() != s)
        buf_pixmap->resize(s.expandedTo(buf_pixmap->size()));
    return buf_pixmap;
}

void Q3TextDocument::draw(QPainter *p, const QRect &rect, const QPalette &pal,
                          const QBrush *paper)
{
    if (!firstParagraph())
        return;

    if (paper) {
        p->setBrushOrigin(-qIntCast(p->translationX()),
                           -qIntCast(p->translationY()));

        p->fillRect(rect, *paper);
    }

    QPainter * oldPainter = Q3TextFormat::painter();
    Q3TextFormat::setPainter(p);

    if (formatCollection()->defaultFormat()->color() != pal.text().color())
        setDefaultFormat(formatCollection()->defaultFormat()->font(), pal.text().color());

    Q3TextParagraph *parag = firstParagraph();
    while (parag) {
        if (!parag->isValid())
            parag->format();
        int y = parag->rect().y();
        QRect pr(parag->rect());
        pr.setX(0);
        pr.setWidth(QWIDGETSIZE_MAX);
        if (!rect.isNull() && !rect.intersects(pr)) {
            parag = parag->next();
            continue;
        }
        p->translate(0, y);
        if (rect.isValid())
            parag->paint(*p, pal, 0, false, rect.x(), rect.y(), rect.width(), rect.height());
        else
            parag->paint(*p, pal, 0, false);
        p->translate(0, -y);
        parag = parag->next();
        if (!flow()->isEmpty())
            flow()->drawFloatingItems(p, rect.x(), rect.y(), rect.width(), rect.height(), pal, false);
    }
    Q3TextFormat::setPainter(oldPainter);
}

void Q3TextDocument::drawParagraph(QPainter *painter, Q3TextParagraph *parag, int cx, int cy,
                                   int cw, int ch,
                                   QPixmap *&/*doubleBuffer*/, const QPalette &pal,
                                   bool drawCursor, Q3TextCursor *cursor, bool resetChanged)
{
    if (resetChanged)
        parag->setChanged(false);
    QRect ir(parag->rect());
#ifndef QT_NO_TEXTCUSTOMITEM
    if (!parag->tableCell())
#endif
        ir.setWidth(width());

    painter->translate(ir.x(), ir.y());

    if (!parag->document()->parent()) {
        const QPoint oldOrigin = painter->brushOrigin();
        painter->setBrushOrigin(-ir.topLeft());
        painter->fillRect(QRect(0, 0, ir.width(), ir.height()), parag->backgroundBrush(pal));
        painter->setBrushOrigin(oldOrigin);
    }

    painter->translate(-(ir.x() - parag->rect().x()),
                       -(ir.y() - parag->rect().y()));
    parag->paint(*painter, pal, drawCursor ? cursor : 0, true, cx, cy, cw, ch);

    painter->translate(-ir.x(), -ir.y());

    parag->document()->nextDoubleBuffered = false;
}

Q3TextParagraph *Q3TextDocument::draw(QPainter *p, int cx, int cy, int cw, int ch,
                                     const QPalette &pal, bool onlyChanged, bool drawCursor,
                                     Q3TextCursor *cursor, bool resetChanged)
{
    if (withoutDoubleBuffer || (par && par->withoutDoubleBuffer)) {
        withoutDoubleBuffer = true;
        QRect r;
        draw(p, r, pal);
        return 0;
    }
    withoutDoubleBuffer = false;

    if (!firstParagraph())
        return 0;

    QPainter * oldPainter = Q3TextFormat::painter();
    Q3TextFormat::setPainter(p);
    if (formatCollection()->defaultFormat()->color() != pal.text().color())
        setDefaultFormat(formatCollection()->defaultFormat()->font(), pal.text().color());

    if (cx < 0 && cy < 0) {
        cx = 0;
        cy = 0;
        cw = width();
        ch = height();
    }

    Q3TextParagraph *lastFormatted = 0;
    Q3TextParagraph *parag = firstParagraph();

    QPixmap *doubleBuffer = 0;

    while (parag) {
        lastFormatted = parag;
        if (!parag->isValid())
            parag->format();

        QRect pr = parag->rect();
        pr.setWidth(parag->document()->width());
        if (pr.y() > cy + ch)
            goto floating;
        QRect clipr(cx, cy, cw, ch);
        if (!pr.intersects(clipr) || (onlyChanged && !parag->hasChanged())) {
            pr.setWidth(parag->document()->width());
            parag = parag->next();
            continue;
        }

        drawParagraph(p, parag, cx, cy, cw, ch, doubleBuffer, pal, drawCursor,
                       cursor, resetChanged);
        parag = parag->next();
    }

    parag = lastParagraph();

 floating:
    if (parag->rect().y() + parag->rect().height() < parag->document()->height()) {
        if (!parag->document()->parent()) {
            QRect fillRect = QRect(0, parag->rect().y() + parag->rect().height(), parag->document()->width(),
                parag->document()->height() - (parag->rect().y() + parag->rect().height()));
            if (QRect(cx, cy, cw, ch).intersects(fillRect))
                p->fillRect(fillRect, pal.brush(QPalette::Base));
        }
        if (!flow()->isEmpty()) {
            QRect cr(cx, cy, cw, ch);
            flow()->drawFloatingItems(p, cr.x(), cr.y(), cr.width(), cr.height(), pal, false);
        }
    }

    if (buf_pixmap && buf_pixmap->height() > 300) {
        delete buf_pixmap;
        buf_pixmap = 0;
    }

    Q3TextFormat::setPainter(oldPainter);
    return lastFormatted;
}

/*
  #### this function only sets the default font size in the format collection
 */
void Q3TextDocument::setDefaultFormat(const QFont &font, const QColor &color)
{
    bool reformat = font != fCollection->defaultFormat()->font();
    for (int idx = 0; idx < childList.size(); ++idx) {
        Q3TextDocument *dc = childList.at(idx);
        dc->setDefaultFormat(font, color);
    }
    fCollection->updateDefaultFormat(font, color, sheet_);

    if (!reformat)
        return;
    tStopWidth = formatCollection()->defaultFormat()->width(QLatin1Char('x')) * 8;

    // invalidate paragraphs and custom items
    Q3TextParagraph *p = fParag;
    while (p) {
        p->invalidate(0);
#ifndef QT_NO_TEXTCUSTOMITEM
        for (int i = 0; i < p->length() - 1; ++i)
            if (p->at(i)->isCustom())
                p->at(i)->customItem()->invalidate();
#endif
        p = p->next();
    }
}


/*!
    \preliminary

    Generates an internal object for the tag called \a name, given the
    attributes \a attr, and using additional information provided by
    the mime source factory \a factory.

    \a context is the optional context of the document, i.e. the path
    to look for relative links. This becomes important if the text
    contains relative references, for example within image tags.
    QSimpleRichText always uses the default mime source factory (see
    \l{Q3MimeSourceFactory::defaultFactory()}) to resolve these
    references. The context will then be used to calculate the
    absolute path. See Q3MimeSourceFactory::makeAbsolute() for details.

    \a emptyTag and \a doc are for internal use only.

    This function should not be used in application code.
*/
#ifndef QT_NO_TEXTCUSTOMITEM
Q3TextCustomItem* Q3TextDocument::tag(Q3StyleSheet *sheet, const QString& name,
                                        const QMap<QString, QString> &attr,
                                        const QString& context,
                                        const Q3MimeSourceFactory& factory,
                                        bool /*emptyTag */, Q3TextDocument *doc)
{
    const Q3StyleSheetItem* style = sheet->item(name);
    // first some known  tags
    if (!style)
        return 0;
    if (style->name() == QLatin1String("img"))
        return new Q3TextImage(doc, attr, context, (Q3MimeSourceFactory&)factory);
    if (style->name() == QLatin1String("hr"))
        return new Q3TextHorizontalLine(doc, attr, context, (Q3MimeSourceFactory&)factory );
   return 0;
}
#endif


#ifndef QT_NO_TEXTCUSTOMITEM
void Q3TextDocument::registerCustomItem(Q3TextCustomItem *i, Q3TextParagraph *p)
{
    if (i && i->placement() != Q3TextCustomItem::PlaceInline) {
        flow_->registerFloatingItem(i);
        p->registerFloatingItem(i);
        i->setParagraph(p);
    }
    p->mightHaveCustomItems = mightHaveCustomItems = true;
}

void Q3TextDocument::unregisterCustomItem(Q3TextCustomItem *i, Q3TextParagraph *p)
{
    p->unregisterFloatingItem(i);
    i->setParagraph(0);
    flow_->unregisterFloatingItem(i);
}
#endif

bool Q3TextDocument::hasFocusParagraph() const
{
    return !!focusIndicator.parag;
}

QString Q3TextDocument::focusHref() const
{
    return focusIndicator.href;
}

QString Q3TextDocument::focusName() const
{
    return focusIndicator.name;
}

bool Q3TextDocument::focusNextPrevChild(bool next)
{
    if (!focusIndicator.parag) {
        if (next) {
            focusIndicator.parag = fParag;
            focusIndicator.start = 0;
            focusIndicator.len = 0;
        } else {
            focusIndicator.parag = lParag;
            focusIndicator.start = lParag->length();
            focusIndicator.len = 0;
        }
    } else {
        focusIndicator.parag->setChanged(true);
    }
    focusIndicator.href.clear();
    focusIndicator.name.clear();

    if (next) {
        Q3TextParagraph *p = focusIndicator.parag;
        int index = focusIndicator.start + focusIndicator.len;
        while (p) {
            for (int i = index; i < p->length(); ++i) {
                if (p->at(i)->isAnchor()) {
                    p->setChanged(true);
                    focusIndicator.parag = p;
                    focusIndicator.start = i;
                    focusIndicator.len = 0;
                    focusIndicator.href = p->at(i)->anchorHref();
                    focusIndicator.name = p->at(i)->anchorName();
                    while (i < p->length()) {
                        if (!p->at(i)->isAnchor())
                            return true;
                        focusIndicator.len++;
                        i++;
                    }
#ifndef QT_NO_TEXTCUSTOMITEM
                } else if (p->at(i)->isCustom()) {
                    if (p->at(i)->customItem()->isNested()) {
                        Q3TextTable *t = (Q3TextTable*)p->at(i)->customItem();
                        QList<Q3TextTableCell *> cells = t->tableCells();
                        // first try to continue
                        int idx;
                        bool resetCells = true;
                        for (idx = 0; idx < cells.size(); ++idx) {
                            Q3TextTableCell *c = cells.at(idx);
                            if (c->richText()->hasFocusParagraph()) {
                                if (c->richText()->focusNextPrevChild(next)) {
                                    p->setChanged(true);
                                    focusIndicator.parag = p;
                                    focusIndicator.start = i;
                                    focusIndicator.len = 0;
                                    focusIndicator.href = c->richText()->focusHref();
                                    focusIndicator.name = c->richText()->focusName();
                                    return true;
                                } else {
                                    resetCells = false;
                                    ++idx;
                                    break;
                                }
                            }
                        }
                        // now really try
                        if (resetCells)
                            idx = 0;
                        for (; idx < cells.size(); ++idx) {
                            Q3TextTableCell *c = cells.at(idx);
                            if (c->richText()->focusNextPrevChild(next)) {
                                p->setChanged(true);
                                focusIndicator.parag = p;
                                focusIndicator.start = i;
                                focusIndicator.len = 0;
                                focusIndicator.href = c->richText()->focusHref();
                                focusIndicator.name = c->richText()->focusName();
                                return true;
                            }
                        }
                    }
#endif
                }
            }
            index = 0;
            p = p->next();
        }
    } else {
        Q3TextParagraph *p = focusIndicator.parag;
        int index = focusIndicator.start - 1;
        if (focusIndicator.len == 0 && index < focusIndicator.parag->length() - 1)
            index++;
        while (p) {
            for (int i = index; i >= 0; --i) {
                if (p->at(i)->isAnchor()) {
                    p->setChanged(true);
                    focusIndicator.parag = p;
                    focusIndicator.start = i;
                    focusIndicator.len = 0;
                    focusIndicator.href = p->at(i)->anchorHref();
                    focusIndicator.name = p->at(i)->anchorName();
                    while (i >= -1) {
                        if (i < 0 || !p->at(i)->isAnchor()) {
                            focusIndicator.start++;
                            return true;
                        }
                        if (i < 0)
                            break;
                        focusIndicator.len++;
                        focusIndicator.start--;
                        i--;
                    }
#ifndef QT_NO_TEXTCUSTOMITEM
                } else if (p->at(i)->isCustom()) {
                    if (p->at(i)->customItem()->isNested()) {
                        Q3TextTable *t = (Q3TextTable*)p->at(i)->customItem();
                        QList<Q3TextTableCell *> cells = t->tableCells();
                        // first try to continue
                        int idx;
                        bool resetCells = true;
                        for (idx = cells.size()-1; idx >= 0; --idx) {
                            Q3TextTableCell *c = cells.at(idx);
                            if (c->richText()->hasFocusParagraph()) {
                                if (c->richText()->focusNextPrevChild(next)) {
                                    p->setChanged(true);
                                    focusIndicator.parag = p;
                                    focusIndicator.start = i;
                                    focusIndicator.len = 0;
                                    focusIndicator.href = c->richText()->focusHref();
                                    focusIndicator.name = c->richText()->focusName();
                                    return true;
                                } else {
                                    resetCells = false;
                                    --idx;
                                    break;
                                }
                            }
                        }
                        // now really try
                        if (resetCells)
                            idx = cells.size()-1;
                        for (; idx >= 0; --idx) {
                            Q3TextTableCell *c = cells.at(idx);
                            if (c->richText()->focusNextPrevChild(next)) {
                                p->setChanged(true);
                                focusIndicator.parag = p;
                                focusIndicator.start = i;
                                focusIndicator.len = 0;
                                focusIndicator.href = c->richText()->focusHref();
                                focusIndicator.name = c->richText()->focusName();
                                return true;
                            }
                        }
                    }
#endif
                }
            }
            p = p->prev();
            if (p)
                index = p->length() - 1;
        }
    }

    focusIndicator.parag = 0;

    return false;
}

int Q3TextDocument::length() const
{
    int l = -1;
    Q3TextParagraph *p = fParag;
    while (p) {
        l += p->length();
        p = p->next();
    }
    return qMax(0,l);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

int Q3TextFormat::width(const QChar &c) const
{
    if (c.unicode() == 0xad) // soft hyphen
        return 0;
    if (!pntr || !pntr->isActive()) {
        if (c == QLatin1Char('\t'))
            return fm.width(QLatin1Char(' '));
        if (ha == AlignNormal) {
            int w;
            if (c.row())
                w = fm.width(c);
            else
                w = widths[c.unicode()];
            if (w == 0 && !c.row()) {
                w = fm.width(c);
                ((Q3TextFormat*)this)->widths[c.unicode()] = w;
            }
            return w;
        } else {
            QFont f(fn);
            if (usePixelSizes)
                f.setPixelSize((f.pixelSize() * 2) / 3);
            else
                f.setPointSize((f.pointSize() * 2) / 3);
            QFontMetrics fm_(f);
            return fm_.width(c);
        }
    }

    QFont f(fn);
    if (ha != AlignNormal) {
        if (usePixelSizes)
            f.setPixelSize((f.pixelSize() * 2) / 3);
        else
            f.setPointSize((f.pointSize() * 2) / 3);
    }
    applyFont(f);

    return pntr_fm->width(c);
}

int Q3TextFormat::width(const QString &str, int pos) const
{
    int w = 0;
    if (str.unicode()[pos].unicode() == 0xad)
        return w;
    if (!pntr || !pntr->isActive()) {
        if (ha == AlignNormal) {
            w = fm.charWidth(str, pos);
        } else {
            QFont f(fn);
            if (usePixelSizes)
                f.setPixelSize((f.pixelSize() * 2) / 3);
            else
                f.setPointSize((f.pointSize() * 2) / 3);
            QFontMetrics fm_(f);
            w = fm_.charWidth(str, pos);
        }
    } else {
        QFont f(fn);
        if (ha != AlignNormal) {
            if (usePixelSizes)
                f.setPixelSize((f.pixelSize() * 2) / 3);
            else
                f.setPointSize((f.pointSize() * 2) / 3);
        }
        applyFont(f);
        w = pntr_fm->charWidth(str, pos);
    }
    return w;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Q3TextString::Q3TextString()
{
    bidiDirty = true;
    bidi = false;
    rightToLeft = false;
    dir = QChar::DirON;
}

Q3TextString::Q3TextString(const Q3TextString &s)
{
    bidiDirty = true;
    bidi = s.bidi;
    rightToLeft = s.rightToLeft;
    dir = s.dir;
    data = s.data;
    data.detach();
    for (int i = 0; i < (int)data.size(); ++i) {
        Q3TextFormat *f = data[i].format();
        if (f)
            f->addRef();
    }
}

void Q3TextString::insert(int index, const QString &s, Q3TextFormat *f)
{
    insert(index, s.unicode(), s.length(), f);
}

void Q3TextString::insert(int index, const QChar *unicode, int len, Q3TextFormat *f)
{
    int os = data.size();
    data.resize(data.size() + len);
    if (index < os) {
        memmove(data.data() + index + len, data.data() + index,
                 sizeof(Q3TextStringChar) * (os - index));
    }
    Q3TextStringChar *ch = data.data() + index;
    for (int i = 0; i < len; ++i) {
        ch->x = 0;
        ch->lineStart = 0;
        ch->nobreak = false;
        ch->type = Q3TextStringChar::Regular;
        ch->p.format = f;
        ch->rightToLeft = 0;
        ch->c = unicode[i];
        ++ch;
    }
    bidiDirty = true;
}

Q3TextString::~Q3TextString()
{
    clear();
}

void Q3TextString::insert(int index, Q3TextStringChar *c, bool doAddRefFormat )
{
    int os = data.size();
    data.resize(data.size() + 1);
    if (index < os) {
        memmove(data.data() + index + 1, data.data() + index,
                 sizeof(Q3TextStringChar) * (os - index));
    }
    Q3TextStringChar &ch = data[(int)index];
    ch.c = c->c;
    ch.x = 0;
    ch.lineStart = 0;
    ch.rightToLeft = 0;
    ch.p.format = 0;
    ch.type = Q3TextStringChar::Regular;
    ch.nobreak = false;
    if (doAddRefFormat && c->format())
        c->format()->addRef();
    ch.setFormat(c->format());
    bidiDirty = true;
}

int Q3TextString::appendParagraphs( Q3TextParagraph *start, Q3TextParagraph *end )
{
    int paragCount = 0;
    int newLength = data.size();
    for (Q3TextParagraph *p = start; p != end; p = p->next()) {
        newLength += p->length();
        ++paragCount;
    }

    const int oldLength = data.size();
    data.resize(newLength);

    Q3TextStringChar *d = &data[oldLength];
    for (Q3TextParagraph *p = start; p != end; p = p->next()) {
        const Q3TextStringChar * const src = p->at(0);
        int i = 0;
        for (; i < p->length() - 1; ++i) {
            d[i].c = src[i].c;
            d[i].x = 0;
            d[i].lineStart = 0;
            d[i].rightToLeft = 0;
            d[i].type = Q3TextStringChar::Regular;
            d[i].nobreak = false;
            d[i].p.format = src[i].format();
            if (d[i].p.format)
                d[i].p.format->addRef();
        }
        d[i].x = 0;
        d[i].lineStart = 0;
        d[i].nobreak = false;
        d[i].type = Q3TextStringChar::Regular;
        d[i].p.format = 0;
        d[i].rightToLeft = 0;
        d[i].c = QLatin1Char('\n');
        d += p->length();
    }

    bidiDirty = true;
    return paragCount;
}

void Q3TextString::truncate(int index)
{
    index = qMax(index, 0);
    index = qMin(index, (int)data.size() - 1);
    if (index < (int)data.size()) {
        for (int i = index + 1; i < (int)data.size(); ++i) {
            Q3TextStringChar &ch = data[i];
#ifndef QT_NO_TEXTCUSTOMITEM
            if (!(ch.type == Q3TextStringChar::Regular)) {
                delete ch.customItem();
                if (ch.p.custom->format)
                    ch.p.custom->format->removeRef();
                delete ch.p.custom;
                ch.p.custom = 0;
            } else
#endif
                if (ch.format()) {
                    ch.format()->removeRef();
                }
        }
    }
    data.resize(index);
    bidiDirty = true;
}

void Q3TextString::remove(int index, int len)
{
    for (int i = index; i < (int)data.size() && i - index < len; ++i) {
        Q3TextStringChar &ch = data[i];
#ifndef QT_NO_TEXTCUSTOMITEM
        if (!(ch.type == Q3TextStringChar::Regular)) {
            delete ch.customItem();
            if (ch.p.custom->format)
                ch.p.custom->format->removeRef();
            delete ch.p.custom;
            ch.p.custom = 0;
        } else
#endif
            if (ch.format()) {
                ch.format()->removeRef();
            }
    }
    memmove(data.data() + index, data.data() + index + len,
             sizeof(Q3TextStringChar) * (data.size() - index - len));
    data.resize(data.size() - len);
    bidiDirty = true;
}

void Q3TextString::clear()
{
    for (int i = 0; i < (int)data.count(); ++i) {
        Q3TextStringChar &ch = data[i];
#ifndef QT_NO_TEXTCUSTOMITEM
        if (!(ch.type == Q3TextStringChar::Regular)) {
            if (ch.customItem() && ch.customItem()->placement() == Q3TextCustomItem::PlaceInline)
                delete ch.customItem();
            if (ch.p.custom->format)
                ch.p.custom->format->removeRef();
            delete ch.p.custom;
            ch.p.custom = 0;
        } else
#endif
            if (ch.format()) {
                ch.format()->removeRef();
            }
    }
    data.resize(0);
    bidiDirty = true;
}

void Q3TextString::setFormat(int index, Q3TextFormat *f, bool useCollection)
{
    Q3TextStringChar &ch = data[index];
    if (useCollection && ch.format())
        ch.format()->removeRef();
    ch.setFormat(f);
}

void Q3TextString::checkBidi() const
{
    // ############ fix BIDI handling
    Q3TextString *that = (Q3TextString *)this;
    that->bidiDirty = false;
    int length = data.size();
    if (!length) {
        that->bidi = rightToLeft;
        that->rightToLeft = (dir == QChar::DirR);
        return;
    }

    if (dir == QChar::DirR) {
        that->rightToLeft = true;
    } else if (dir == QChar::DirL) {
        that->rightToLeft = false;
    } else {
        that->rightToLeft = (QApplication::layoutDirection() == Qt::RightToLeft);
    }

    const Q3TextStringChar *start = data.data();
    const Q3TextStringChar *end = start + length;

    ((Q3TextString *)this)->stringCache = toString(data);

    // determines the properties we need for layouting
    QTextEngine textEngine;
    textEngine.text = toString();
    textEngine.option.setTextDirection(rightToLeft ? Qt::RightToLeft : Qt::LeftToRight);
    textEngine.itemize();
    const HB_CharAttributes *ca = textEngine.attributes() + length-1;
    Q3TextStringChar *ch = (Q3TextStringChar *)end - 1;
    QScriptItem *item = &textEngine.layoutData->items[textEngine.layoutData->items.size()-1];
    unsigned char bidiLevel = item->analysis.bidiLevel;
    that->bidi = (bidiLevel || rightToLeft);
    int pos = length-1;
    while (ch >= start) {
        if (item->position > pos) {
            --item;
            Q_ASSERT(item >= &textEngine.layoutData->items[0]);
            bidiLevel = item->analysis.bidiLevel;
            if (bidiLevel)
                that->bidi = true;
        }
        ch->softBreak = ca->lineBreakType >= HB_Break;
        ch->whiteSpace = ca->whiteSpace;
        ch->charStop = ca->charStop;
        ch->bidiLevel = bidiLevel;
        ch->rightToLeft = (bidiLevel%2);
        --ch;
        --ca;
        --pos;
    }
}

void Q3TextDocument::setStyleSheet(Q3StyleSheet *s)
{
    if (!s)
        return;
    sheet_ = s;
    list_tm = list_bm = par_tm = par_bm = 12;
    list_lm = 40;
    li_tm = li_bm = 0;
    Q3StyleSheetItem* item = s->item(QLatin1String("ol"));
    if (item) {
        list_tm = qMax(0,item->margin(Q3StyleSheetItem::MarginTop));
        list_bm = qMax(0,item->margin(Q3StyleSheetItem::MarginBottom));
        list_lm = qMax(0,item->margin(Q3StyleSheetItem::MarginLeft));
    }
    if ((item = s->item(QLatin1String("li")))) {
        li_tm = qMax(0,item->margin(Q3StyleSheetItem::MarginTop));
        li_bm = qMax(0,item->margin(Q3StyleSheetItem::MarginBottom));
    }
    if ((item = s->item(QLatin1String("p")))) {
        par_tm = qMax(0,item->margin(Q3StyleSheetItem::MarginTop));
        par_bm = qMax(0,item->margin(Q3StyleSheetItem::MarginBottom));
    }
}

void Q3TextDocument::setUnderlineLinks(bool b) {
    underlLinks = b;
    for (int idx = 0; idx < childList.size(); ++idx) {
        Q3TextDocument *dc = childList.at(idx);
        dc->setUnderlineLinks(b);
    }
}

void Q3TextStringChar::setFormat(Q3TextFormat *f)
{
    if (type == Regular) {
        p.format = f;
    } else {
#ifndef QT_NO_TEXTCUSTOMITEM
        if (!p.custom) {
            p.custom = new CustomData;
            p.custom->custom = 0;
        }
        p.custom->format = f;
#endif
    }
}

#ifndef QT_NO_TEXTCUSTOMITEM
void Q3TextStringChar::setCustomItem(Q3TextCustomItem *i)
{
    if (type == Regular) {
        Q3TextFormat *f = format();
        p.custom = new CustomData;
        p.custom->format = f;
    } else {
        delete p.custom->custom;
    }
    p.custom->custom = i;
    type = (type == Anchor ? CustomAnchor : Custom);
}

void Q3TextStringChar::loseCustomItem()
{
    if (type == Custom) {
        Q3TextFormat *f = p.custom->format;
        p.custom->custom = 0;
        delete p.custom;
        type = Regular;
        p.format = f;
    } else if (type == CustomAnchor) {
        p.custom->custom = 0;
        type = Anchor;
    }
}

#endif

QString Q3TextStringChar::anchorName() const
{
    if (type == Regular)
        return QString();
    else
        return p.custom->anchorName;
}

QString Q3TextStringChar::anchorHref() const
{
    if (type == Regular)
        return QString();
    else
        return p.custom->anchorHref;
}

void Q3TextStringChar::setAnchor(const QString& name, const QString& href)
{
    if (type == Regular) {
        Q3TextFormat *f = format();
        p.custom = new CustomData;
#ifndef QT_NO_TEXTCUSTOMITEM
        p.custom->custom = 0;
#endif
        p.custom->format = f;
        type = Anchor;
    } else if (type == Custom) {
        type = CustomAnchor;
    }
    p.custom->anchorName = name;
    p.custom->anchorHref = href;
}


int Q3TextString::width(int idx) const
{
     int w = 0;
     Q3TextStringChar *c = &at(idx);
     if (!c->charStop || c->c.unicode() == 0xad || c->c.unicode() == 0x2028)
         return 0;
#ifndef QT_NO_TEXTCUSTOMITEM
     if(c->isCustom()) {
         if(c->customItem()->placement() == Q3TextCustomItem::PlaceInline)
             w = c->customItem()->width;
     } else
#endif
     {
         int r = c->c.row();
         if(r < 0x06
#ifndef Q_WS_WIN
             // Uniscribe's handling of Asian makes the condition below fail.
             || (r > 0x1f && !(r > 0xd7 && r < 0xe0))
#endif
             ) {
             w = c->format()->width(c->c);
         } else {
             // complex text. We need some hacks to get the right metric here
             w = c->format()->width(toString(), idx);
         }
     }
     return w;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Q3TextParagraph::Q3TextParagraph(Q3TextDocument *dc, Q3TextParagraph *pr, Q3TextParagraph *nx, bool updateIds)
    : p(pr), n(nx), docOrPseudo(dc),
      changed(false), firstFormat(true), firstPProcess(true), needPreProcess(false), fullWidth(true),
      lastInFrame(false), visible(true), breakable(true), movedDown(false),
      mightHaveCustomItems(false), hasdoc(dc != 0), litem(false), rtext(false),
      align(0), lstyle(Q3StyleSheetItem::ListDisc), invalid(0), mSelections(0),
#ifndef QT_NO_TEXTCUSTOMITEM
      mFloatingItems(0),
#endif
      utm(0), ubm(0), ulm(0), urm(0), uflm(0), ulinespacing(0),
      tabStopWidth(0), minwidth(0), tArray(0), eData(0), ldepth(0)
{
    lstyle = Q3StyleSheetItem::ListDisc;
    if (!hasdoc)
        docOrPseudo = new Q3TextParagraphPseudoDocument;
    bgcol = 0;
    list_val = -1;
    paintdevice = 0;
    Q3TextFormat* defFormat = formatCollection()->defaultFormat();
    if (!hasdoc) {
        tabStopWidth = defFormat->width(QLatin1Char('x')) * 8;
        pseudoDocument()->commandHistory = new Q3TextCommandHistory(100);
    }

    if (p)
        p->n = this;
    if (n)
        n->p = this;

    if (!p && hasdoc)
        document()->setFirstParagraph(this);
    if (!n && hasdoc)
        document()->setLastParagraph(this);

    state = -1;

    if (p)
        id = p->id + 1;
    else
        id = 0;
    if (n && updateIds) {
        Q3TextParagraph *s = n;
        while (s) {
            s->id = s->p->id + 1;
            s->invalidateStyleCache();
            s = s->n;
        }
    }

    str = new Q3TextString();
    const QChar ch(QLatin1Char(' '));
    str->insert(0, &ch, 1, formatCollection()->defaultFormat());
}

Q3TextParagraph::~Q3TextParagraph()
{
    delete str;
    if (hasdoc) {
        register Q3TextDocument *doc = document();
        if (this == doc->minwParag) {
            doc->minwParag = 0;
            doc->minw = 0;
        }
        if (this == doc->curParag)
            doc->curParag = 0;
    } else {
        delete pseudoDocument();
    }
    delete [] tArray;
    delete eData;
    QMap<int, QTextLineStart*>::Iterator it = lineStarts.begin();
    for (; it != lineStarts.end(); ++it)
        delete *it;
    if (mSelections)
        delete mSelections;
#ifndef QT_NO_TEXTCUSTOMITEM
    if (mFloatingItems)
        delete mFloatingItems;
#endif
    if (p)
        p->setNext(n);
    if (n)
        n->setPrev(p);
    delete bgcol;
}

void Q3TextParagraph::setNext(Q3TextParagraph *s)
{
    n = s;
    if (!n && hasdoc)
        document()->setLastParagraph(this);
}

void Q3TextParagraph::setPrev(Q3TextParagraph *s)
{
    p = s;
    if (!p && hasdoc)
        document()->setFirstParagraph(this);
}

void Q3TextParagraph::invalidate(int chr)
{
    if (invalid < 0)
        invalid = chr;
    else
        invalid = qMin(invalid, chr);
#ifndef QT_NO_TEXTCUSTOMITEM
    if (mFloatingItems) {
        for (int idx = 0; idx < mFloatingItems->size(); ++idx) {
            Q3TextCustomItem *i = mFloatingItems->at(idx);
            i->ypos = -1;
        }
    }
#endif
    invalidateStyleCache();
}

void Q3TextParagraph::invalidateStyleCache()
{
    if (list_val < 0)
        list_val = -1;
}


void Q3TextParagraph::insert(int index, const QString &s)
{
    insert(index, s.unicode(), s.length());
}

void Q3TextParagraph::insert(int index, const QChar *unicode, int len)
{
    if (hasdoc && !document()->useFormatCollection() && document()->preProcessor())
        str->insert(index, unicode, len,
                     document()->preProcessor()->format(Q3TextPreProcessor::Standard));
    else
        str->insert(index, unicode, len, formatCollection()->defaultFormat());
    invalidate(index);
    needPreProcess = true;
}

void Q3TextParagraph::truncate(int index)
{
    str->truncate(index);
    insert(length(), QLatin1String(" "));
    needPreProcess = true;
}

void Q3TextParagraph::remove(int index, int len)
{
    if (index + len - str->length() > 0)
        return;
#ifndef QT_NO_TEXTCUSTOMITEM
    for (int i = index; i < index + len; ++i) {
        Q3TextStringChar *c = at(i);
        if (hasdoc && c->isCustom()) {
            document()->unregisterCustomItem(c->customItem(), this);
        }
    }
#endif
    str->remove(index, len);
    invalidate(0);
    needPreProcess = true;
}

void Q3TextParagraph::join(Q3TextParagraph *s)
{
    int oh = r.height() + s->r.height();
    n = s->n;
    if (n)
        n->p = this;
    else if (hasdoc)
        document()->setLastParagraph(this);

    int start = str->length();
    if (length() > 0 && at(length() - 1)->c == QLatin1Char(' ')) {
        remove(length() - 1, 1);
        --start;
    }
    append(s->str->toString(), true);

    for (int i = 0; i < s->length(); ++i) {
        if (!hasdoc || document()->useFormatCollection()) {
            s->str->at(i).format()->addRef();
            str->setFormat(i + start, s->str->at(i).format(), true);
        }
#ifndef QT_NO_TEXTCUSTOMITEM
        if (s->str->at(i).isCustom()) {
            Q3TextCustomItem * item = s->str->at(i).customItem();
            str->at(i + start).setCustomItem(item);
            s->str->at(i).loseCustomItem();
            if (hasdoc) {
                document()->unregisterCustomItem(item, s);
                document()->registerCustomItem(item, this);
            }
        }
        if (s->str->at(i).isAnchor()) {
            str->at(i + start).setAnchor(s->str->at(i).anchorName(),
                            s->str->at(i).anchorHref());
        }
#endif
    }

    if (!extraData() && s->extraData()) {
        setExtraData(s->extraData());
        s->setExtraData(0);
    } else if (extraData() && s->extraData()) {
        extraData()->join(s->extraData());
    }
    delete s;
    invalidate(0);
    r.setHeight(oh);
    needPreProcess = true;
    if (n) {
        Q3TextParagraph *s = n;
        s->invalidate(0);
        while (s) {
            s->id = s->p->id + 1;
            s->state = -1;
            s->needPreProcess = true;
            s->changed = true;
            s->invalidateStyleCache();
            s = s->n;
        }
    }
    format();
    state = -1;
}

void Q3TextParagraph::move(int &dy)
{
    if (dy == 0)
        return;
    changed = true;
    r.moveBy(0, dy);
#ifndef QT_NO_TEXTCUSTOMITEM
    if (mFloatingItems) {
        for (int idx = 0; idx < mFloatingItems->size(); ++idx) {
            Q3TextCustomItem *i = mFloatingItems->at(idx);
            i->ypos += dy;
        }
    }
#endif
    if (p)
        p->lastInFrame = true;

    // do page breaks if required
    if (hasdoc && document()->isPageBreakEnabled()) {
        int shift;
        if ((shift = document()->formatter()->formatVertically( document(), this))) {
            if (p)
                p->setChanged(true);
            dy += shift;
        }
    }
}

void Q3TextParagraph::format(int start, bool doMove)
{
    if (!str || str->length() == 0 || !formatter())
        return;

    if (hasdoc &&
         document()->preProcessor() &&
         (needPreProcess || state == -1))
        document()->preProcessor()->process(document(), this, invalid <= 0 ? 0 : invalid);
    needPreProcess = false;

    if (invalid == -1)
        return;

    r.moveTopLeft(QPoint(documentX(), p ? p->r.y() + p->r.height() : documentY()));
    if (p)
        p->lastInFrame = false;

    movedDown = false;
    bool formattedAgain = false;

 formatAgain:

    r.setWidth(documentWidth());
#ifndef QT_NO_TEXTCUSTOMITEM
    if (hasdoc && mFloatingItems) {
        for (int idx = 0; idx < mFloatingItems->size(); ++idx) {
            Q3TextCustomItem *i = mFloatingItems->at(idx);
            i->ypos = r.y();
            if (i->placement() == Q3TextCustomItem::PlaceRight) {
                i->xpos = r.x() + r.width() - i->width;
            }
        }
    }
#endif
    QMap<int, QTextLineStart*> oldLineStarts = lineStarts;
    lineStarts.clear();
    int y = formatter()->format(document(), this, start, oldLineStarts);


    r.setWidth(qMax(r.width(), formatter()->minimumWidth()));


    QMap<int, QTextLineStart*>::Iterator it = oldLineStarts.begin();

    for (; it != oldLineStarts.end(); ++it)
        delete *it;

    if (!hasdoc) { // qt_format_text bounding rect handling
        it = lineStarts.begin();
        int usedw = 0;
        for (; it != lineStarts.end(); ++it)
            usedw = qMax(usedw, (*it)->w);
        if (r.width() <= 0) {
            // if the user specifies an invalid rect, this means that the
            // bounding box should grow to the width that the text actually
            // needs
            r.setWidth(usedw);
        } else {
            r.setWidth(qMin(usedw, r.width()));
        }
    }

    if (y != r.height())
        r.setHeight(y);

    if (!visible) {
        r.setHeight(0);
    } else {
        int minw = minwidth = formatter()->minimumWidth();
        int wused = formatter()->widthUsed();
        wused = qMax(minw, wused);
        if (hasdoc) {
            document()->setMinimumWidth(minw, wused, this);
        }  else {
            pseudoDocument()->minw = qMax(pseudoDocument()->minw, minw);
            pseudoDocument()->wused = qMax(pseudoDocument()->wused, wused);
        }
    }

    // do page breaks if required
    if (hasdoc && document()->isPageBreakEnabled()) {
        int shift = document()->formatter()->formatVertically(document(), this);
        if (shift && !formattedAgain) {
            formattedAgain = true;
            goto formatAgain;
        }
    }

    if (n && doMove && n->invalid == -1 && r.y() + r.height() != n->r.y()) {
        int dy = (r.y() + r.height()) - n->r.y();
        Q3TextParagraph *s = n;
        bool makeInvalid = p && p->lastInFrame;
        while (s && dy) {
            if (!s->isFullWidth())
                makeInvalid = true;
            if (makeInvalid)
                s->invalidate(0);
            s->move(dy);
            if (s->lastInFrame)
                makeInvalid = true;
            s = s->n;
        }
    }

    firstFormat = false;
    changed = true;
    invalid = -1;
    //#####   string()->setTextChanged(false);
}

int Q3TextParagraph::lineHeightOfChar(int i, int *bl, int *y) const
{
    if (!isValid())
        ((Q3TextParagraph*)this)->format();

    QMap<int, QTextLineStart*>::ConstIterator it = lineStarts.end();
    --it;
    for (;;) {
        if (i >= it.key()) {
            if (bl)
                *bl = (*it)->baseLine;
            if (y)
                *y = (*it)->y;
            return (*it)->h;
        }
        if (it == lineStarts.begin())
            break;
        --it;
    }

    qWarning("Q3TextParagraph::lineHeightOfChar: couldn't find lh for %d", i);
    return 15;
}

Q3TextStringChar *Q3TextParagraph::lineStartOfChar(int i, int *index, int *line) const
{
    if (!isValid())
        ((Q3TextParagraph*)this)->format();

    int l = (int)lineStarts.count() - 1;
    QMap<int, QTextLineStart*>::ConstIterator it = lineStarts.end();
    --it;
    for (;;) {
        if (i >= it.key()) {
            if (index)
                *index = it.key();
            if (line)
                *line = l;
            return &str->at(it.key());
        }
        if (it == lineStarts.begin())
            break;
        --it;
        --l;
    }

    qWarning("Q3TextParagraph::lineStartOfChar: couldn't find %d", i);
    return 0;
}

int Q3TextParagraph::lines() const
{
    if (!isValid())
        ((Q3TextParagraph*)this)->format();

    return (int)lineStarts.count();
}

Q3TextStringChar *Q3TextParagraph::lineStartOfLine(int line, int *index) const
{
    if (!isValid())
        ((Q3TextParagraph*)this)->format();

    if (line >= 0 && line < (int)lineStarts.count()) {
        QMap<int, QTextLineStart*>::ConstIterator it = lineStarts.begin();
        while (line-- > 0)
            ++it;
        int i = it.key();
        if (index)
            *index = i;
        return &str->at(i);
    }

    qWarning("Q3TextParagraph::lineStartOfLine: couldn't find %d", line);
    return 0;
}

int Q3TextParagraph::leftGap() const
{
    if (!isValid())
        ((Q3TextParagraph*)this)->format();

    if (str->length() == 0)
        return 0;

    int line = 0;
    int x = str->length() ? str->at(0).x : 0;  /* set x to x of first char */
    if (str->isBidi()) {
        for (int i = 1; i < str->length()-1; ++i)
            x = qMin(x, str->at(i).x);
        return x;
    }

    QMap<int, QTextLineStart*>::ConstIterator it = lineStarts.begin();
    while (line < (int)lineStarts.count()) {
        int i = it.key(); /* char index */
        x = qMin(x, str->at(i).x);
        ++it;
        ++line;
    }
    return x;
}

void Q3TextParagraph::setFormat(int index, int len, Q3TextFormat *f, bool useCollection, int flags)
{
    if (!f)
        return;
    if (index < 0)
        index = 0;
    if (index > str->length() - 1)
        index = str->length() - 1;
    if (index + len >= str->length())
        len = str->length() - index;

    Q3TextFormatCollection *fc = 0;
    if (useCollection)
        fc = formatCollection();
    Q3TextFormat *of;
    for (int i = 0; i < len; ++i) {
        of = str->at(i + index).format();
        if (!changed && (!of || f->key() != of->key()))
            changed = true;
        if (invalid == -1 &&
             (f->font().family() != of->font().family() ||
               f->font().pointSize() != of->font().pointSize() ||
               f->font().weight() != of->font().weight() ||
               f->font().italic() != of->font().italic() ||
               f->vAlign() != of->vAlign())) {
            invalidate(0);
        }
        if (flags == -1 || flags == Q3TextFormat::Format || !fc) {
            if (fc)
                f = fc->format(f);
            str->setFormat(i + index, f, useCollection);
        } else {
            Q3TextFormat *fm = fc->format(of, f, flags);
            str->setFormat(i + index, fm, useCollection);
        }
    }
}

void Q3TextParagraph::indent(int *oldIndent, int *newIndent)
{
    if (!hasdoc || !document()->indent() || isListItem()) {
        if (oldIndent)
            *oldIndent = 0;
        if (newIndent)
            *newIndent = 0;
        if (oldIndent && newIndent)
            *newIndent = *oldIndent;
        return;
    }
    document()->indent()->indent(document(), this, oldIndent, newIndent);
}

void Q3TextParagraph::paint(QPainter &painter, const QPalette &pal, Q3TextCursor *cursor,
                            bool drawSelections, int clipx, int clipy, int clipw, int cliph)
{
    if (!visible)
        return;
    int i, y, h, baseLine, xstart, xend = 0;
    i = y =h = baseLine = 0;
    QRect cursorRect;
    drawSelections &= (mSelections != 0);
    // macintosh full-width selection style
    bool fullWidthStyle = QApplication::style()->styleHint(QStyle::SH_RichText_FullWidthSelection);
    int fullSelectionWidth = 0;
    if (drawSelections && fullWidthStyle)
        fullSelectionWidth = (hasdoc ? document()->width() : r.width());

    QString qstr = str->toString();
    qstr.detach();
    // ### workaround so that \n are not drawn, actually this should
    // be fixed in QFont somewhere (under Windows you get ugly boxes
    // otherwise)
    QChar* uc = (QChar*) qstr.unicode();
    for (int ii = 0; ii < qstr.length(); ii++)
        if (uc[(int)ii]== QLatin1Char(QLatin1Char('\n')) || uc[(int)ii] == QLatin1Char('\t'))
            uc[(int)ii] = 0x20;

    int line = -1;
    int paintStart = 0;
    Q3TextStringChar *chr = 0;
    Q3TextStringChar *nextchr = at(0);
    for (i = 0; i < length(); i++) {
        chr = nextchr;
        if (i < length()-1)
            nextchr = at(i+1);

        // we flush at end of document
        bool flush = (i == length()-1);
        bool ignoreSoftHyphen = false;
        if (!flush) {
            // we flush at end of line
            flush |= nextchr->lineStart;
            // we flush on format changes
            flush |= (nextchr->format() != chr->format());
            // we flush on link changes
            flush |= (nextchr->isLink() != chr->isLink());
            // we flush on start of run
            flush |= (nextchr->bidiLevel != chr->bidiLevel);
            // we flush on bidi changes
            flush |= (nextchr->rightToLeft != chr->rightToLeft);
            // we flush before and after tabs
            flush |= (chr->c == QLatin1Char('\t') || nextchr->c == QLatin1Char('\t'));
            // we flush on soft hyphens
            if (chr->c.unicode() == 0xad) {
                flush = true;
                if (!nextchr->lineStart)
                    ignoreSoftHyphen = true;
            }
            // we flush on custom items
            flush |= chr->isCustom();
            // we flush before custom items
            flush |= nextchr->isCustom();
            // when painting justified, we flush on spaces
            if ((alignment() & Qt::AlignJustify) == Qt::AlignJustify)
                flush |= chr->whiteSpace;
        }

        // init a new line
        if (chr->lineStart) {
            ++line;
            paintStart = i;
            lineInfo(line, y, h, baseLine);
            if (clipy != -1 && cliph != 0 && y + r.y() - h > clipy + cliph) { // outside clip area, leave
                break;
            }

            // if this is the first line and we are a list item, draw the the bullet label
            if (line == 0 && isListItem()) {
                int x = chr->x;
                if (str->isBidi()) {
                    if (str->isRightToLeft()) {
                        x = chr->x + str->width(0);
                        for (int k = 1; k < length(); ++k) {
                            if (str->at(k).lineStart)
                                break;
                            x = qMax(x, str->at(k).x + str->width(k));
                        }
                    } else {
                        x = chr->x;
                        for (int k = 1; k < length(); ++k) {
                            if (str->at(k).lineStart)
                                break;
                            x = qMin(x, str->at(k).x);
                        }
                    }
                }
                drawLabel(&painter, x, y, 0, 0, baseLine, pal);
            }
        }

        // check for cursor mark
        if (cursor && this == cursor->paragraph() && i == cursor->index()) {
            Q3TextStringChar *c = i == 0 ? chr : chr - 1;
            cursorRect.setRect(cursor->x() , y + baseLine - c->format()->ascent(),
                                1, c->format()->height());
        }

        if (flush) {  // something changed, draw what we have so far
            if (chr->rightToLeft) {
                xstart = chr->x;
                xend = at(paintStart)->x + str->width(paintStart);
            } else {
                xstart = at(paintStart)->x;
                xend = chr->x;
                if (i < length() - 1) {
                    if (!str->at(i + 1).lineStart &&
                         str->at(i + 1).rightToLeft == chr->rightToLeft)
                        xend = str->at(i + 1).x;
                    else
                        xend += str->width(i);
                }
            }

            if ((clipx == -1 || clipw <= 0 || (xend >= clipx && xstart <= clipx + clipw)) &&
                 (clipy == -1 || clipy < y+r.y()+h)) {
                if (!chr->isCustom())
                    drawString(painter, qstr, paintStart, i - paintStart + (ignoreSoftHyphen ? 0 : 1), xstart, y,
                                baseLine, xend-xstart, h, drawSelections, fullSelectionWidth,
                                chr, pal, chr->rightToLeft);
#ifndef QT_NO_TEXTCUSTOMITEM
                else if (chr->customItem()->placement() == Q3TextCustomItem::PlaceInline) {
                    bool inSelection = false;
                    if (drawSelections) {
                        QMap<int, Q3TextParagraphSelection>::ConstIterator it = mSelections->constFind(Q3TextDocument::Standard);
                        inSelection = (it != mSelections->constEnd() && (*it).start <= i && (*it).end > i);
                    }
                    chr->customItem()->draw(&painter, chr->x, y,
                                             clipx == -1 ? clipx : (clipx - r.x()),
                                             clipy == -1 ? clipy : (clipy - r.y()),
                                             clipw, cliph, pal, inSelection);
                }
#endif
            }
            paintStart = i+1;
        }

    }

    // time to draw the cursor
    const int cursor_extent = 4;
    if (!cursorRect.isNull() && cursor &&
         ((clipx == -1 || clipw == -1) || (cursorRect.right()+cursor_extent >= clipx && cursorRect.left()-cursor_extent <= clipx + clipw))) {
        painter.fillRect(cursorRect, pal.color(QPalette::Text));
        painter.save();
        if (string()->isBidi()) {
            if (at(cursor->index())->rightToLeft) {
                painter.setPen(Qt::black);
                painter.drawLine(cursorRect.x(), cursorRect.y(), cursorRect.x() - cursor_extent / 2, cursorRect.y() + cursor_extent / 2);
                painter.drawLine(cursorRect.x(), cursorRect.y() + cursor_extent, cursorRect.x() - cursor_extent / 2, cursorRect.y() + cursor_extent / 2);
            } else {
                painter.setPen(Qt::black);
                painter.drawLine(cursorRect.x(), cursorRect.y(), cursorRect.x() + cursor_extent / 2, cursorRect.y() + cursor_extent / 2);
                painter.drawLine(cursorRect.x(), cursorRect.y() + cursor_extent, cursorRect.x() + cursor_extent / 2, cursorRect.y() + cursor_extent / 2);
            }
        }
        painter.restore();
    }
}

//#define BIDI_DEBUG

void Q3TextParagraph::setColorForSelection(QColor &color, QPainter &painter,
                                           const QPalette &pal, int selection)
{
    if (selection < 0)
        return;
    color = (hasdoc && selection != Q3TextDocument::Standard) ?
            document()->selectionColor(selection) :
            pal.color(QPalette::Highlight);
    QColor text = (hasdoc && document()->hasSelectionTextColor(selection)) ? document()->selectionTextColor(selection) : pal.color(QPalette::HighlightedText);
    if (text.isValid())
        painter.setPen(text);
}

void Q3TextParagraph::drawString(QPainter &painter, const QString &str, int start, int len,
                                 int xstart, int y, int baseLine, int w, int h,
                                 bool drawSelections, int fullSelectionWidth,
                                 Q3TextStringChar *formatChar, const QPalette& pal,
                                 bool rightToLeft)
{
    bool plainText = hasdoc ? document()->textFormat() == Qt::PlainText : false;
    Q3TextFormat* format = formatChar->format();

    int textFlags = int(rightToLeft ? Qt::TextForceRightToLeft : Qt::TextForceLeftToRight);

    if (!plainText || (hasdoc && format->color() != document()->formatCollection()->defaultFormat()->color()))
        painter.setPen(QPen(format->color()));
    else
        painter.setPen(pal.text().color());
    painter.setFont(format->font());

    if (hasdoc && formatChar->isAnchor() && !formatChar->anchorHref().isEmpty()) {
        if (format->useLinkColor())
            painter.setPen(document()->linkColor.isValid() ? document()->linkColor :
                           pal.link().color());
        if (document()->underlineLinks()) {
            QFont fn = format->font();
            fn.setUnderline(true);
            painter.setFont(fn);
        }
    }

    int real_length = len;
    if (len && !rightToLeft && start + len == length()) // don't draw the last character (trailing space)
        len--;
    if (len && str.unicode()[start+len-1] == QChar::LineSeparator)
        len--;


    Q3TextFormat::VerticalAlignment vAlign = format->vAlign();
    if (vAlign != Q3TextFormat::AlignNormal) {
        // sub or superscript
        QFont f(painter.font());
        if (format->fontSizesInPixels())
            f.setPixelSize((f.pixelSize() * 2) / 3);
        else
            f.setPointSize((f.pointSize() * 2) / 3);
        painter.setFont(f);
        int h = painter.fontMetrics().height();
        baseLine += (vAlign == Q3TextFormat::AlignSubScript) ? h/6 : -h/2;
    }

    bool allSelected = false;
    if (drawSelections) {
        QMap<int, Q3TextParagraphSelection>::ConstIterator it = mSelections->constFind(Q3TextDocument::Standard);
        allSelected = (it != mSelections->constEnd() && (*it).start <= start && (*it).end >= start+len);
    }
    if (!allSelected)
        painter.drawText(QPointF(xstart, y + baseLine), str.mid(start, len), textFlags, /*justificationPadding*/0);

#ifdef BIDI_DEBUG
    painter.save();
    painter.setPen (Qt::red);
    painter.drawLine(xstart, y, xstart, y + baseLine);
    painter.drawLine(xstart, y + baseLine/2, xstart + 10, y + baseLine/2);
    int w = 0;
    int i = 0;
    while(i < len)
        w += painter.fontMetrics().charWidth(str, start + i++);
    painter.setPen (Qt::blue);
    painter.drawLine(xstart + w - 1, y, xstart + w - 1, y + baseLine);
    painter.drawLine(xstart + w - 1, y + baseLine/2, xstart + w - 1 - 10, y + baseLine/2);
    painter.restore();
#endif

    // check if we are in a selection and draw it
    if (drawSelections) {
        QMap<int, Q3TextParagraphSelection>::ConstIterator it = mSelections->constEnd();
        while (it != mSelections->constBegin()) {
            --it;
            int selStart = (*it).start;
            int selEnd = (*it).end;
            int tmpw = w;

            selStart = qMax(selStart, start);
            int real_selEnd = qMin(selEnd, start+real_length);
            selEnd = qMin(selEnd, start+len);
            bool extendRight = false;
            bool extendLeft = false;
            bool selWrap = (real_selEnd == length()-1 && n && n->hasSelection(it.key()));
            if (selWrap || this->str->at(real_selEnd).lineStart) {
                extendRight = (fullSelectionWidth != 0);
                if (!extendRight && !rightToLeft)
                    tmpw += painter.fontMetrics().width(QLatin1Char(' '));
            }
            if (fullSelectionWidth && (selStart == 0 || this->str->at(selStart).lineStart)) {
                extendLeft = true;
            }
            if (this->str->isRightToLeft() != rightToLeft)
                extendLeft = extendRight = false;

            if (this->str->isRightToLeft()) {
                bool tmp = extendLeft;
                extendLeft = extendRight;
                extendRight = tmp;
            }

            if (selStart < real_selEnd ||
                (selWrap && fullSelectionWidth && extendRight &&
                // don't draw the standard selection on a printer=
                (it.key() != Q3TextDocument::Standard || !is_printer(&painter)))) {
                int selection = it.key();
                QColor color;
                setColorForSelection(color, painter, pal, selection);
                if (selStart != start || selEnd != start + len || selWrap) {
                    // have to clip
                    painter.save();
                    int cs, ce;
                    if (rightToLeft) {
                        cs = (selEnd != start + len) ?
                             this->str->at(this->str->previousCursorPosition(selEnd)).x : xstart;
                        ce = (selStart != start) ?
                             this->str->at(this->str->previousCursorPosition(selStart)).x : xstart+tmpw;
                    } else {
                        cs = (selStart != start) ? this->str->at(selStart).x : xstart;
                        ce = (selEnd != start + len) ? this->str->at(selEnd).x : xstart+tmpw;
                    }
                    QRect r(cs, y, ce-cs, h);
                    if (extendLeft)
                        r.setLeft(0);
                    if (extendRight)
                        r.setRight(fullSelectionWidth);
                    QRegion reg(r);
                    if (painter.hasClipping())
                        reg &= painter.clipRegion();
                    painter.setClipRegion(reg);
                }
                int xleft = xstart;
                if (extendLeft) {
                    tmpw += xstart;
                    xleft = 0;
                }
                if (extendRight)
                    tmpw = fullSelectionWidth - xleft;
                if(color.isValid())
                    painter.fillRect(xleft, y, tmpw, h, color);
                painter.drawText(QPointF(xstart, y + baseLine), str.mid(start, len), textFlags, /*justificationPadding*/0);
                if (selStart != start || selEnd != start + len || selWrap)
                    painter.restore();
            }
        }
    }

    if (format->isMisspelled()) {
        painter.save();
        painter.setPen(QPen(Qt::red, 1, Qt::DotLine));
        painter.drawLine(xstart, y + baseLine + 1, xstart + w, y + baseLine + 1);
        painter.restore();
    }

    if (hasdoc && formatChar->isAnchor() && !formatChar->anchorHref().isEmpty() &&
         document()->focusIndicator.parag == this &&
         ((document()->focusIndicator.start >= start  &&
           document()->focusIndicator.start + document()->focusIndicator.len <= start + len)
          || (document()->focusIndicator.start <= start &&
              document()->focusIndicator.start + document()->focusIndicator.len >= start + len))) {
        QStyleOptionFocusRect opt;
        opt.rect.setRect(xstart, y, w, h);
#ifndef Q_WS_WIN
        opt.state = QStyle::State_None;
#else
        // force drawing a focus rect but only on windows because it's
        // configurable by the user in windows settings (see
        // SH_UnderlineShortcut style hint) and we want to override
        // this settings.
        opt.state = QStyle::State_KeyboardFocusChange;
#endif
        opt.palette = pal;
        QApplication::style()->drawPrimitive(QStyle::PE_FrameFocusRect, &opt, &painter);
    }
}

void Q3TextParagraph::drawLabel(QPainter* p, int x, int y, int w, int h, int base,
                                const QPalette& pal)
{
    QRect r (x, y, w, h);
    Q3StyleSheetItem::ListStyle s = listStyle();

    p->save();
    Q3TextFormat *format = at(0)->format();
    if (format) {
        p->setPen(format->color());
        p->setFont(format->font());
    }
    QFontMetrics fm(p->fontMetrics());
    int size = fm.lineSpacing() / 3;

    bool rtl = str->isRightToLeft();

    switch (s) {
    case Q3StyleSheetItem::ListDecimal:
    case Q3StyleSheetItem::ListLowerAlpha:
    case Q3StyleSheetItem::ListUpperAlpha:
        {
            if (list_val == -1) { // uninitialised list value, calcluate the right one
                int depth = listDepth();
                list_val--;
                // ### evil, square and expensive. This needs to be done when formatting, not when painting
                Q3TextParagraph* s = prev();
                int depth_s;
                while (s && (depth_s = s->listDepth()) >= depth) {
                    if (depth_s == depth && s->isListItem())
                        list_val--;
                    s = s->prev();
                }
            }

            int n = list_val;
            if (n < -1)
                n = -n - 1;
            QString l;
            switch (s) {
            case Q3StyleSheetItem::ListLowerAlpha:
                if (n < 27) {
                    l = QLatin1Char(('a' + (char) (n-1)));
                    break;
                }
            case Q3StyleSheetItem::ListUpperAlpha:
                if (n < 27) {
                    l = QLatin1Char(('A' + (char) (n-1)));
                    break;
                }
                break;
            default:  //Q3StyleSheetItem::ListDecimal:
                l.setNum(n);
                break;
            }
            if (rtl)
                l.prepend(QLatin1String(" ."));
            else
                l += QString::fromLatin1(". ");
            int x = (rtl ? r.left() : r.right() - fm.width(l));
            p->drawText(x, r.top() + base, l);
        }
        break;
    case Q3StyleSheetItem::ListSquare:
        {
            int x = rtl ? r.left() + size : r.right() - size*2;
            QRect er(x, r.top() + fm.height() / 2 - size / 2, size, size);
            p->fillRect(er , pal.brush(QPalette::Text));
        }
        break;
    case Q3StyleSheetItem::ListCircle:
        {
            int x = rtl ? r.left() + size : r.right() - size*2;
            QRect er(x, r.top() + fm.height() / 2 - size / 2, size, size);
            p->drawEllipse(er);
        }
        break;
    case Q3StyleSheetItem::ListDisc:
    default:
        {
            p->setBrush(pal.brush(QPalette::Text));
            int x = rtl ? r.left() + size : r.right() - size*2;
            QRect er(x, r.top() + fm.height() / 2 - size / 2, size, size);
            p->drawEllipse(er);
            p->setBrush(Qt::NoBrush);
        }
        break;
    }

    p->restore();
}

#ifndef QT_NO_DATASTREAM
void Q3TextParagraph::readStyleInformation(QDataStream &stream)
{
    int int_align, int_lstyle;
    uchar uchar_litem, uchar_rtext, uchar_dir;
    stream >> int_align >> int_lstyle >> utm >> ubm >> ulm >> urm >> uflm
           >> ulinespacing >> ldepth >> uchar_litem >> uchar_rtext >> uchar_dir;
    align = int_align; lstyle = (Q3StyleSheetItem::ListStyle) int_lstyle;
    litem = uchar_litem; rtext = uchar_rtext; str->setDirection((QChar::Direction)uchar_dir);
    Q3TextParagraph* s = prev() ? prev() : this;
    while (s) {
        s->invalidate(0);
        s = s->next();
    }
}

void Q3TextParagraph::writeStyleInformation(QDataStream& stream) const
{
    stream << (int) align << (int) lstyle << utm << ubm << ulm << urm << uflm << ulinespacing << ldepth << (uchar)litem << (uchar)rtext << (uchar)str->direction();
}
#endif


void Q3TextParagraph::setListItem(bool li)
{
    if ((bool)litem == li)
        return;
    litem = li;
    changed = true;
    Q3TextParagraph* s = prev() ? prev() : this;
    while (s) {
        s->invalidate(0);
        s = s->next();
    }
}

void Q3TextParagraph::setListDepth(int depth) {
    if (!hasdoc || depth == ldepth)
        return;
    ldepth = depth;
    Q3TextParagraph* s = prev() ? prev() : this;
    while (s) {
        s->invalidate(0);
        s = s->next();
    }
}

int *Q3TextParagraph::tabArray() const
{
    int *ta = tArray;
    if (!ta && hasdoc)
        ta = document()->tabArray();
    return ta;
}

int Q3TextParagraph::nextTab(int, int x)
{
    int *ta = tArray;
    if (hasdoc) {
        if (!ta)
            ta = document()->tabArray();
        tabStopWidth = document()->tabStopWidth();
    }
    if (ta) {
        int i = 0;
        while (ta[i]) {
            if (ta[i] >= x)
                return tArray[i];
            ++i;
        }
        return tArray[0];
    } else {
        int n;
        if (tabStopWidth != 0)
            n = x / tabStopWidth;
        else
            return x;
        return tabStopWidth * (n + 1);
    }
}

void Q3TextParagraph::adjustToPainter(QPainter *p)
{
#ifndef QT_NO_TEXTCUSTOMITEM
    for (int i = 0; i < length(); ++i) {
        if (at(i)->isCustom())
            at(i)->customItem()->adjustToPainter(p);
    }
#endif
}

Q3TextFormatCollection *Q3TextParagraph::formatCollection() const
{
    if (hasdoc)
        return document()->formatCollection();
    Q3TextFormatCollection* fc = &pseudoDocument()->collection;
    if (paintdevice != fc->paintDevice())
        fc->setPaintDevice(paintdevice);
    return fc;
}

QString Q3TextParagraph::richText() const
{
    QString s;
    Q3TextStringChar *formatChar = 0;
    QString spaces;
    bool doStart = richTextExportStart && richTextExportStart->paragraph() == this;
    bool doEnd = richTextExportEnd && richTextExportEnd->paragraph() == this;
    int i;
    QString lastAnchorName;
    for (i = 0; i < length()-1; ++i) {
        if (doStart && i && richTextExportStart->index() == i)
            s += QLatin1String("<!--StartFragment-->");
        if (doEnd && richTextExportEnd->index() == i)
            s += QLatin1String("<!--EndFragment-->");
        Q3TextStringChar *c = &str->at(i);
        if (c->isAnchor() && !c->anchorName().isEmpty() && c->anchorName() != lastAnchorName) {
	    lastAnchorName = c->anchorName();
            if (c->anchorName().contains(QLatin1Char('#'))) {
                QStringList l = c->anchorName().split(QLatin1Char('#'));
                for (QStringList::ConstIterator it = l.constBegin(); it != l.constEnd(); ++it)
                    s += QLatin1String("<a name=\"") + *it + QLatin1String("\"></a>");
            } else {
                s += QLatin1String("<a name=\"") + c->anchorName() + QLatin1String("\"></a>");
            }
        }
        if (!formatChar) {
            s += c->format()->makeFormatChangeTags(formatCollection()->defaultFormat(),
                                                    0, QString(), c->anchorHref());
            formatChar = c;
        } else if ((formatChar->format()->key() != c->format()->key()) ||
                  (c->anchorHref() != formatChar->anchorHref()))  {
            s += c->format()->makeFormatChangeTags(formatCollection()->defaultFormat(),
                                                    formatChar->format() , formatChar->anchorHref(), c->anchorHref());
            formatChar = c;
        }
        if (c->c == QLatin1Char('<'))
            s += QLatin1String("&lt;");
        else if (c->c == QLatin1Char('>'))
            s += QLatin1String("&gt;");
        else if (c->c == QLatin1Char('&'))
            s += QLatin1String("&amp;");
        else if (c->c == QLatin1Char('\"'))
            s += QLatin1String("&quot;");
#ifndef QT_NO_TEXTCUSTOMITEM
        else if (c->isCustom())
            s += c->customItem()->richText();
#endif
        else if (c->c == QLatin1Char('\n') || c->c == QChar::LineSeparator)
            s += QLatin1String("<br />"); // space on purpose for compatibility with Netscape, Lynx & Co.
        else
            s += c->c;
    }
    if (doEnd && richTextExportEnd->index() == i)
        s += QLatin1String("<!--EndFragment-->");
    if (formatChar)
        s += formatChar->format()->makeFormatEndTags(formatCollection()->defaultFormat(), formatChar->anchorHref());
    return s;
}

void Q3TextParagraph::addCommand(Q3TextCommand *cmd)
{
    if (!hasdoc)
        pseudoDocument()->commandHistory->addCommand(cmd);
    else
        document()->commands()->addCommand(cmd);
}

Q3TextCursor *Q3TextParagraph::undo(Q3TextCursor *c)
{
    if (!hasdoc)
        return pseudoDocument()->commandHistory->undo(c);
    return document()->commands()->undo(c);
}

Q3TextCursor *Q3TextParagraph::redo(Q3TextCursor *c)
{
    if (!hasdoc)
        return pseudoDocument()->commandHistory->redo(c);
    return document()->commands()->redo(c);
}

int Q3TextParagraph::topMargin() const
{
    int m = 0;
    if (rtext) {
        m = isListItem() ? (document()->li_tm/qMax(1,listDepth()*listDepth())) :
            (listDepth() ? 0 : document()->par_tm);
        if (listDepth() == 1 &&( !prev() || prev()->listDepth() < listDepth()))
            m = qMax<int>(m, document()->list_tm);
    }
    m += utm;
    return scale(m, Q3TextFormat::painter());
}

int Q3TextParagraph::bottomMargin() const
{
    int m = 0;
    if (rtext) {
        m = isListItem() ? (document()->li_bm/qMax(1,listDepth()*listDepth())) :
            (listDepth() ? 0 : document()->par_bm);
        if (listDepth() == 1 &&( !next() || next()->listDepth() < listDepth()))
            m = qMax<int>(m, document()->list_bm);
    }
    m += ubm;
    return scale(m, Q3TextFormat::painter());
}

int Q3TextParagraph::leftMargin() const
{
    int m = ulm;
    if (listDepth() && !string()->isRightToLeft())
        m += listDepth() * document()->list_lm;
    return scale(m, Q3TextFormat::painter());
}

int Q3TextParagraph::firstLineMargin() const
{
    int m = uflm;
    return scale(m, Q3TextFormat::painter());
}

int Q3TextParagraph::rightMargin() const
{
    int m = urm;
    if (listDepth() && string()->isRightToLeft())
        m += listDepth() * document()->list_lm;
    return scale(m, Q3TextFormat::painter());
}

int Q3TextParagraph::lineSpacing() const
{
    int l = ulinespacing;
    l = scale(l, Q3TextFormat::painter());
    return l;
}

void Q3TextParagraph::copyParagData(Q3TextParagraph *parag)
{
    rtext = parag->rtext;
    lstyle = parag->lstyle;
    ldepth = parag->ldepth;
    litem = parag->litem;
    align = parag->align;
    utm = parag->utm;
    ubm = parag->ubm;
    urm = parag->urm;
    ulm = parag->ulm;
    uflm = parag->uflm;
    ulinespacing = parag->ulinespacing;
    QColor *c = parag->backgroundColor();
    if (c)
        setBackgroundColor(*c);
    str->setDirection(parag->str->direction());
}

void Q3TextParagraph::show()
{
    if (visible || !hasdoc)
        return;
    visible = true;
}

void Q3TextParagraph::hide()
{
    if (!visible || !hasdoc)
        return;
    visible = false;
}

void Q3TextParagraph::setDirection(QChar::Direction dir)
{
    if (str && str->direction() != dir) {
        str->setDirection(dir);
        invalidate(0);
    }
}

QChar::Direction Q3TextParagraph::direction() const
{
    return (str ? str->direction() : QChar::DirON);
}

void Q3TextParagraph::setChanged(bool b, bool recursive)
{
    changed = b;
    if (recursive) {
        if (document() && document()->parentParagraph())
            document()->parentParagraph()->setChanged(b, recursive);
    }
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


Q3TextPreProcessor::Q3TextPreProcessor()
{
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Q3TextFormatter::Q3TextFormatter()
    : thisminw(0), thiswused(0), wrapEnabled(true), wrapColumn(-1), biw(false)
{
}

QTextLineStart *Q3TextFormatter::formatLine(Q3TextParagraph *parag, Q3TextString *string, QTextLineStart *line,
                                                   Q3TextStringChar *startChar, Q3TextStringChar *lastChar, int align, int space)
{
    if (lastChar < startChar)
        return new QTextLineStart;
#ifndef QT_NO_COMPLEXTEXT
    if(string->isBidi())
        return bidiReorderLine(parag, string, line, startChar, lastChar, align, space);
#endif
    int start = (startChar - &string->at(0));
    int last = (lastChar - &string->at(0));

    // ignore white space at the end of the line.
    Q3TextStringChar *ch = lastChar;
    while (ch > startChar && ch->whiteSpace) {
        space += ch->format()->width(QLatin1Char(' '));
        --ch;
    }

    if (space < 0)
        space = 0;

    // do alignment Auto == Left in this case
    if (align & Qt::AlignHCenter || align & Qt::AlignRight) {
        if (align & Qt::AlignHCenter)
            space /= 2;
        for (int j = start; j <= last; ++j)
            string->at(j).x += space;
    } else if (align & Qt::AlignJustify) {
        int numSpaces = 0;
        // End at "last-1", the last space ends up with a width of 0
        for (int j = last-1; j >= start; --j) {
            // Start at last tab, if any.
            Q3TextStringChar &ch = string->at(j);
            if (ch.c == QLatin1Char('\t')) {
                start = j+1;
                break;
            }
            if(ch.whiteSpace)
                numSpaces++;
        }
        int toAdd = 0;
        for (int k = start + 1; k <= last; ++k) {
            Q3TextStringChar &ch = string->at(k);
            if(numSpaces && ch.whiteSpace) {
                int s = space / numSpaces;
                toAdd += s;
                space -= s;
                numSpaces--;
            }
            string->at(k).x += toAdd;
        }
    }

    if (last >= 0 && last < string->length())
        line->w = string->at(last).x + string->width(last);
    else
        line->w = 0;

    return new QTextLineStart;
}

#ifndef QT_NO_COMPLEXTEXT

#ifdef BIDI_DEBUG
QT_BEGIN_INCLUDE_NAMESPACE
#include <iostream>
QT_END_INCLUDE_NAMESPACE
#endif

// collects one line of the paragraph and transforms it to visual order
QTextLineStart *Q3TextFormatter::bidiReorderLine(Q3TextParagraph * /*parag*/, Q3TextString *text, QTextLineStart *line,
                                                        Q3TextStringChar *startChar, Q3TextStringChar *lastChar, int align, int space)
{
    // ignore white space at the end of the line.
    int endSpaces = 0;
    while (lastChar > startChar && lastChar->whiteSpace) {
        space += lastChar->format()->width(QLatin1Char(' '));
        --lastChar;
        ++endSpaces;
    }

    int start = (startChar - &text->at(0));
    int last = (lastChar - &text->at(0));

    int length = lastChar - startChar + 1;


    int x = startChar->x;

    unsigned char _levels[256];
    int _visual[256];

    unsigned char *levels = _levels;
    int *visual = _visual;

    if (length > 255) {
        levels = (unsigned char *)malloc(length*sizeof(unsigned char));
        visual = (int *)malloc(length*sizeof(int));
    }

    //qDebug("bidiReorderLine: length=%d (%d-%d)", length, start, last);

    Q3TextStringChar *ch = startChar;
    unsigned char *l = levels;
    while (ch <= lastChar) {
        //qDebug("  level: %d", ch->bidiLevel);
        *(l++) = (ch++)->bidiLevel;
    }

    QTextEngine::bidiReorder(length, levels, visual);

    // now construct the reordered string out of the runs...

    int numSpaces = 0;
    align = QStyle::visualAlignment(text->isRightToLeft() ? Qt::RightToLeft : Qt::LeftToRight, QFlag(align));

    // This is not really correct, but as we can't make the scroll bar move to the left of the origin,
    // this ensures all text can be scrolled to and read.
    if (space < 0)
        space = 0;

    if (align & Qt::AlignHCenter)
        x += space/2;
    else if (align & Qt::AlignRight)
        x += space;
    else if (align & Qt::AlignJustify) {
        // End at "last-1", the last space ends up with a width of 0
        for (int j = last-1; j >= start; --j) {
            // Start at last tab, if any.
            Q3TextStringChar &ch = text->at(j);
            if (ch.c == QLatin1Char('\t')) {
                start = j+1;
                break;
            }
            if(ch.whiteSpace)
                numSpaces++;
        }
    }

    int toAdd = 0;
    int xorig = x;
    Q3TextStringChar *lc = startChar + visual[0];
    for (int i = 0; i < length; i++) {
        Q3TextStringChar *ch = startChar + visual[i];
        if (numSpaces && ch->whiteSpace) {
            int s = space / numSpaces;
            toAdd += s;
            space -= s;
            numSpaces--;
        }

        if (lc->format() != ch->format() && !ch->c.isSpace()
            && lc->format()->font().italic() && !ch->format()->font().italic()) {
            int rb = lc->format()->fontMetrics().rightBearing(lc->c);
            if (rb < 0)
                x -= rb;
        }

        ch->x = x + toAdd;
        ch->rightToLeft = ch->bidiLevel % 2;
        //qDebug("visual: %d (%p) placed at %d rightToLeft=%d", visual[i], ch, x +toAdd, ch->rightToLeft );
        int ww = 0;
        if (ch->c.unicode() >= 32 || ch->c == QLatin1Char(QLatin1Char('\t')) || ch->c == QLatin1Char('\n') || ch->isCustom()) {
            ww = text->width(start+visual[i]);
        } else {
            ww = ch->format()->width(QLatin1Char(' '));
        }
        x += ww;
        lc = ch;
    }
    x += toAdd;

    while (endSpaces--) {
        ++lastChar;
        int sw = lastChar->format()->width(QLatin1Char(' '));
        if (text->isRightToLeft()) {
            xorig -= sw;
            lastChar->x = xorig;
            ch->rightToLeft = true;
        } else {
            lastChar->x = x;
            x += sw;
            ch->rightToLeft = false;
        }
    }

    line->w = x;

    if (length > 255) {
        free(levels);
        free(visual);
    }

    return new QTextLineStart;
}
#endif


void Q3TextFormatter::insertLineStart(Q3TextParagraph *parag, int index, QTextLineStart *ls)
{
    QMap<int, QTextLineStart*>::Iterator it;
    if ((it = parag->lineStartList().find(index)) == parag->lineStartList().end()) {
        parag->lineStartList().insert(index, ls);
    } else {
        delete *it;
        parag->lineStartList().erase(it);
        parag->lineStartList().insert(index, ls);
    }
}


/* Standard pagebreak algorithm using Q3TextFlow::adjustFlow. Returns
 the shift of the paragraphs bottom line.
 */
int Q3TextFormatter::formatVertically(Q3TextDocument* doc, Q3TextParagraph* parag)
{
    int oldHeight = parag->rect().height();
    QMap<int, QTextLineStart*>& lineStarts = parag->lineStartList();
    QMap<int, QTextLineStart*>::Iterator it = lineStarts.begin();
    int h = parag->prev() ? qMax(parag->prev()->bottomMargin(),parag->topMargin()) / 2: 0;
    for (; it != lineStarts.end() ; ++it ) {
        QTextLineStart * ls = it.value();
        ls->y = h;
        Q3TextStringChar *c = &parag->string()->at(it.key());
#ifndef QT_NO_TEXTCUSTOMITEM
        if (c && c->customItem() && c->customItem()->ownLine()) {
            int h = c->customItem()->height;
            c->customItem()->pageBreak(parag->rect().y() + ls->y + ls->baseLine - h, doc->flow());
            int delta = c->customItem()->height - h;
            ls->h += delta;
            if (delta)
                parag->setMovedDown(true);
        } else
#endif
        {

            int shift = doc->flow()->adjustFlow(parag->rect().y() + ls->y, ls->w, ls->h);
            ls->y += shift;
            if (shift)
                parag->setMovedDown(true);
        }
        h = ls->y + ls->h;
    }
    int m = parag->bottomMargin();
    if (!parag->next())
        m = 0;
    else
        m = qMax(m, parag->next()->topMargin()) / 2;
    h += m;
    parag->setHeight(h);
    return h - oldHeight;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Q3TextFormatterBreakInWords::Q3TextFormatterBreakInWords()
{
}

#define SPACE(s) s

int Q3TextFormatterBreakInWords::format(Q3TextDocument *doc,Q3TextParagraph *parag,
                                        int start, const QMap<int, QTextLineStart*> &)
{
    // make sure bidi information is correct.
    (void)parag->string()->isBidi();

    Q3TextStringChar *c = 0;
    Q3TextStringChar *firstChar = 0;
    int left = doc ? parag->leftMargin() + doc->leftMargin() : 0;
    int x = left + (doc ? parag->firstLineMargin() : 0);
    int dw = parag->documentVisibleWidth() - (doc ? doc->rightMargin() : 0);
    int y = parag->prev() ? qMax(parag->prev()->bottomMargin(),parag->topMargin()) / 2: 0;
    int h = y;
    int len = parag->length();
    if (doc)
        x = doc->flow()->adjustLMargin(y + parag->rect().y(), parag->rect().height(), x, 4);
    int rm = parag->rightMargin();
    int w = dw - (doc ? doc->flow()->adjustRMargin(y + parag->rect().y(), parag->rect().height(), rm, 4) : 0);
    bool fullWidth = true;
    int minw = 0;
    int wused = 0;
    bool wrapEnabled = isWrapEnabled(parag);

    start = 0;    //######### what is the point with start?! (Matthias)
    if (start == 0)
        c = &parag->string()->at(0);

    int i = start;
    QTextLineStart *lineStart = new QTextLineStart(y, y, 0);
    insertLineStart(parag, 0, lineStart);

    QPainter *painter = Q3TextFormat::painter();

    int col = 0;
    int ww = 0;
    QChar lastChr;
    int tabBase = left < x ? left : x;
    for (; i < len; ++i, ++col) {
        if (c)
            lastChr = c->c;
        c = &parag->string()->at(i);
        // ### the lines below should not be needed
        if (painter)
            c->format()->setPainter(painter);
        if (i > 0) {
            c->lineStart = 0;
        } else {
            c->lineStart = 1;
            firstChar = c;
        }
        if (c->c.unicode() >= 32 || c->isCustom()) {
            ww = parag->string()->width(i);
        } else if (c->c == QLatin1Char('\t')) {
            int nx = parag->nextTab(i, x - tabBase) + tabBase;
            if (nx < x)
                ww = w - x;
            else
                ww = nx - x;
        } else {
            ww = c->format()->width(QLatin1Char(' '));
        }

#ifndef QT_NO_TEXTCUSTOMITEM
        if (c->isCustom() && c->customItem()->ownLine()) {
            x = doc ? doc->flow()->adjustLMargin(y + parag->rect().y(), parag->rect().height(), left, 4) : left;
            w = dw - (doc ? doc->flow()->adjustRMargin(y + parag->rect().y(), parag->rect().height(), rm, 4) : 0);
            c->customItem()->resize(w - x);
            w = dw;
            y += h;
            h = c->height();
            lineStart = new QTextLineStart(y, h, h);
            insertLineStart(parag, i, lineStart);
            c->lineStart = 1;
            firstChar = c;
            x = 0xffffff;
            continue;
        }
#endif

        if (wrapEnabled &&
             ((wrapAtColumn() == -1 && x + ww > w) ||
               (wrapAtColumn() != -1 && col >= wrapAtColumn()))) {
            x = doc ? parag->document()->flow()->adjustLMargin(y + parag->rect().y(), parag->rect().height(), left, 4) : left;
            w = dw;
            y += h;
            h = c->height();
            lineStart = formatLine(parag, parag->string(), lineStart, firstChar, c-1);
            lineStart->y = y;
            insertLineStart(parag, i, lineStart);
            lineStart->baseLine = c->ascent();
            lineStart->h = c->height();
            c->lineStart = 1;
            firstChar = c;
            col = 0;
            if (wrapAtColumn() != -1)
                minw = qMax(minw, w);
        } else if (lineStart) {
            lineStart->baseLine = qMax(lineStart->baseLine, c->ascent());
            h = qMax(h, c->height());
            lineStart->h = h;
        }

        c->x = x;
        x += ww;
        wused = qMax(wused, x);
    }

    int m = parag->bottomMargin();
    if (!parag->next())
        m = 0;
    else
        m = qMax(m, parag->next()->topMargin()) / 2;
    parag->setFullWidth(fullWidth);
    y += h + m;
    if (doc)
        minw += doc->rightMargin();
    if (!wrapEnabled)
        minw = qMax(minw, wused);

    thisminw = minw;
    thiswused = wused;
    return y;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Q3TextFormatterBreakWords::Q3TextFormatterBreakWords()
{
}

#define DO_FLOW(lineStart) do{ if (doc && doc->isPageBreakEnabled()) { \
                    int yflow = lineStart->y + parag->rect().y();\
                    int shift = doc->flow()->adjustFlow(yflow, dw, lineStart->h); \
                    lineStart->y += shift;\
                    y += shift;\
                }}while(false)

int Q3TextFormatterBreakWords::format(Q3TextDocument *doc, Q3TextParagraph *parag,
                                      int start, const QMap<int, QTextLineStart*> &)
{
    // make sure bidi information is correct.
    (void)parag->string()->isBidi();

    Q3TextStringChar *c = 0;
    Q3TextStringChar *firstChar = 0;
    Q3TextString *string = parag->string();
    int left = doc ? parag->leftMargin() + doc->leftMargin() : 0;
    int x = left + (doc ? parag->firstLineMargin() : 0);
    int y = parag->prev() ? qMax(parag->prev()->bottomMargin(),parag->topMargin()) / 2: 0;
    int h = y;
    int len = parag->length();
    if (doc)
        x = doc->flow()->adjustLMargin(y + parag->rect().y(), parag->rect().height(), x, 0);
    int dw = parag->documentVisibleWidth() - (doc ? (left != x ? 0 : doc->rightMargin()) : 0);

    int curLeft = x;
    int rm = parag->rightMargin();
    int rdiff = doc ? doc->flow()->adjustRMargin(y + parag->rect().y(), parag->rect().height(), rm, 0) : 0;
    int w = dw - rdiff;
    bool fullWidth = true;
    int marg = left + rdiff;
    int minw = 0;
    int wused = 0;
    int tminw = marg;
    int linespacing = doc ? parag->lineSpacing() : 0;
    bool wrapEnabled = isWrapEnabled(parag);

    start = 0;

    int i = start;
    QTextLineStart *lineStart = new QTextLineStart(y, y, 0);
    insertLineStart(parag, 0, lineStart);
    int lastBreak = -1;
    int tmpBaseLine = 0, tmph = 0;
    bool lastWasNonInlineCustom = false;

    int align = parag->alignment();
    if (align == Qt::AlignAuto && doc && doc->alignment() != Qt::AlignAuto)
        align = doc->alignment();

    align &= Qt::AlignHorizontal_Mask;

    // ### hack. The last char in the paragraph is always invisible,
    // ### and somehow sometimes has a wrong format. It changes
    // ### between // layouting and printing. This corrects some
    // ### layouting errors in BiDi mode due to this.
    if (len > 1) {
        c = &parag->string()->at(len - 1);
        if (!c->isAnchor()) {
            if (c->format())
                c->format()->removeRef();
            c->setFormat(string->at(len - 2).format());
            if (c->format())
                c->format()->addRef();
        }
    }

    c = &parag->string()->at(0);

    QPainter *painter = Q3TextFormat::painter();
    int col = 0;
    int ww = 0;
    QChar lastChr = c->c;
    Q3TextFormat *lastFormat = c->format();
    int tabBase = left < x ? left : x;
    for (; i < len; ++i, ++col) {
        if (i) {
            c = &parag->string()->at(i-1);
            lastChr = c->c;
            lastFormat = c->format();
        }
        bool lastWasOwnLineCustomItem = lastBreak == -2;
        bool hadBreakableChar = lastBreak != -1;
        bool lastWasHardBreak = lastChr == QChar::LineSeparator;

        // ### next line should not be needed
        if (painter)
            c->format()->setPainter(painter);
        c = &string->at(i);

        if (lastFormat != c->format() && !c->c.isSpace()
            && lastFormat->font().italic() && !c->format()->font().italic()) {
            int rb = lastFormat->fontMetrics().rightBearing(lastChr);
            if (rb < 0)
                x -= rb;
        }

        if ((i > 0 && (x > curLeft || ww == 0)) || lastWasNonInlineCustom) {
            c->lineStart = 0;
        } else {
            c->lineStart = 1;
            firstChar = c;
        }

        // ignore non spacing marks for column count.
        if (col != 0 && QChar::category(c->c.unicode()) == QChar::Mark_NonSpacing)
            --col;

#ifndef QT_NO_TEXTCUSTOMITEM
        lastWasNonInlineCustom =  (c->isCustom() && c->customItem()->placement() != Q3TextCustomItem::PlaceInline);
#endif

        if (c->c.unicode() >= 32 || c->isCustom()) {
            ww = string->width(i);
        } else if (c->c == QLatin1Char('\t')) {
            if (align == Qt::AlignRight || align == Qt::AlignCenter) {
                // we can not  (yet) do tabs
                ww = c->format()->width(QLatin1Char(' '));
            } else {
                int tabx = lastWasHardBreak ? (left + (doc ? parag->firstLineMargin() : 0)) : x;
                int nx = parag->nextTab(i, tabx - tabBase) + tabBase;
                if (nx < tabx) // strrrange...
                    ww = 0;
                else
                    ww = nx - tabx;
            }
        } else {
            ww = c->format()->width(QLatin1Char(' '));
        }

#ifndef QT_NO_TEXTCUSTOMITEM
        Q3TextCustomItem* ci = c->customItem();
        if (c->isCustom() && ci->ownLine()) {
            QTextLineStart *lineStart2 = formatLine(parag, string, lineStart, firstChar, c-1, align, SPACE(w - x - ww));
            x = doc ? doc->flow()->adjustLMargin(y + parag->rect().y(), parag->rect().height(), left, 4) : left;
            w = dw - (doc ? doc->flow()->adjustRMargin(y + parag->rect().y(), parag->rect().height(), rm, 4) : 0);
            ci->resize(w - x);
            if (ci->width < w - x) {
                if (align & Qt::AlignHCenter)
                    x = (w - ci->width) / 2;
                else if (align & Qt::AlignRight) {
                    x = w - ci->width;
                }
            }
            c->x = x;
            curLeft = x;
            if (i == 0 || !isBreakable(string, i-1) ||
                 string->at(i - 1).lineStart == 0) {
                y += qMax(h, qMax(tmph, linespacing));
                tmph = c->height();
                h = tmph;
                lineStart = lineStart2;
                lineStart->y = y;
                insertLineStart(parag, i, lineStart);
                c->lineStart = 1;
                firstChar = c;
            } else {
                tmph = c->height();
                h = tmph;
                delete lineStart2;
            }
            lineStart->h = h;
            lineStart->baseLine = h;
            tmpBaseLine = lineStart->baseLine;
            lastBreak = -2;
            x = w;
            minw = qMax(minw, tminw);

            int tw = ci->minimumWidth() + (doc ? doc->leftMargin() : 0);
            if (tw < QWIDGETSIZE_MAX)
                tminw = tw;
            else
                tminw = marg;
            wused = qMax(wused, ci->width);
            continue;
        } else if (c->isCustom() && ci->placement() != Q3TextCustomItem::PlaceInline) {
            int tw = ci->minimumWidth();
            if (tw < QWIDGETSIZE_MAX)
                minw = qMax(minw, tw);
        }
#endif
        // we break if
        // 1. the last character was a hard break (QChar::LineSeparator) or
        // 2. the last character was a own-line custom item (eg. table or ruler) or
        // 3. wrapping was enabled, it was not a space and following
        // condition is true: We either had a breakable character
        // previously or we ar allowed to break in words and - either
        // we break at w pixels and the current char would exceed that
        // or - we break at a column and the current character would
        // exceed that.
        if (lastWasHardBreak || lastWasOwnLineCustomItem ||
             (wrapEnabled &&
               ((!c->c.isSpace() && (hadBreakableChar || allowBreakInWords()) &&
                  ((wrapAtColumn() == -1 && x + ww > w) ||
                    (wrapAtColumn() != -1 && col >= wrapAtColumn()))))
              )
            ) {
            if (wrapAtColumn() != -1)
                minw = qMax(minw, x + ww);
            // if a break was forced (no breakable char, hard break or own line custom item), break immediately....
            if (!hadBreakableChar || lastWasHardBreak || lastWasOwnLineCustomItem) {
                if (lineStart) {
                    lineStart->baseLine = qMax(lineStart->baseLine, tmpBaseLine);
                    h = qMax(h, tmph);
                    lineStart->h = h;
                    DO_FLOW(lineStart);
                }
                lineStart = formatLine(parag, string, lineStart, firstChar, c-1, align, SPACE(w - x));
                x = doc ? doc->flow()->adjustLMargin(y + parag->rect().y(), parag->rect().height(), left, 4) : left;
                w = dw - (doc ? doc->flow()->adjustRMargin(y + parag->rect().y(), parag->rect().height(), rm, 4) : 0);
                if (!doc && c->c == QLatin1Char('\t')) { // qt_format_text tab handling
                    int nx = parag->nextTab(i, x - tabBase) + tabBase;
                    if (nx < x)
                        ww = w - x;
                    else
                        ww = nx - x;
                }
                curLeft = x;
                y += qMax(h, linespacing);
                tmph = c->height();
                h = 0;
                lineStart->y = y;
                insertLineStart(parag, i, lineStart);
                lineStart->baseLine = c->ascent();
                lineStart->h = c->height();
                c->lineStart = 1;
                firstChar = c;
                tmpBaseLine = lineStart->baseLine;
                lastBreak = -1;
                col = 0;
                if (allowBreakInWords() || lastWasHardBreak) {
                    minw = qMax(minw, tminw);
                    tminw = marg + ww;
                }
            } else { // ... otherwise if we had a breakable char, break there
                DO_FLOW(lineStart);
                c->x = x;
                i = lastBreak;
                lineStart = formatLine(parag, string, lineStart, firstChar, parag->at(lastBreak),align, SPACE(w - string->at(i+1).x));
                x = doc ? doc->flow()->adjustLMargin(y + parag->rect().y(), parag->rect().height(), left, 4) : left;
                w = dw - (doc ? doc->flow()->adjustRMargin(y + parag->rect().y(), parag->rect().height(), rm, 4) : 0);
                if (!doc && c->c == QLatin1Char('\t')) { // qt_format_text tab handling
                    int nx = parag->nextTab(i, x - tabBase) + tabBase;
                    if (nx < x)
                        ww = w - x;
                    else
                        ww = nx - x;
                }
                curLeft = x;
                y += qMax(h, linespacing);
                tmph = c->height();
                h = tmph;
                lineStart->y = y;
                insertLineStart(parag, i + 1, lineStart);
                lineStart->baseLine = c->ascent();
                lineStart->h = c->height();
                c->lineStart = 1;
                firstChar = c;
                tmpBaseLine = lineStart->baseLine;
                lastBreak = -1;
                col = 0;
                minw = qMax(minw, tminw);
                tminw = marg;
                continue;
            }
        } else if (lineStart && isBreakable(string, i)) {
            if (len <= 2 || i < len - 1) {
                tmpBaseLine = qMax(tmpBaseLine, c->ascent());
                tmph = qMax(tmph, c->height());
            }
            minw = qMax(minw, tminw);

            tminw = marg + ww;
            lineStart->baseLine = qMax(lineStart->baseLine, tmpBaseLine);
            h = qMax(h, tmph);
            lineStart->h = h;
            if (i < len - 2 || c->c != QLatin1Char(' '))
                lastBreak = i;
        } else {
            tminw += ww;
            int cascent = c->ascent();
            int cheight = c->height();
            int belowBaseLine = qMax(tmph - tmpBaseLine, cheight-cascent);
            tmpBaseLine = qMax(tmpBaseLine, cascent);
            tmph = tmpBaseLine + belowBaseLine;
        }

        c->x = x;
        x += ww;
        wused = qMax(wused, x);
    }

    if (lineStart) {
        lineStart->baseLine = qMax(lineStart->baseLine, tmpBaseLine);
        h = qMax(h, tmph);
        lineStart->h = h;
        // last line in a paragraph is not justified
        if (align & Qt::AlignJustify) {
            align |= Qt::AlignLeft;
            align &= ~(Qt::AlignJustify|Qt::AlignAbsolute);
        }
        DO_FLOW(lineStart);
        lineStart = formatLine(parag, string, lineStart, firstChar, c, align, SPACE(w - x));
        delete lineStart;
    }

    minw = qMax(minw, tminw);
    if (doc)
        minw += doc->rightMargin();

    int m = parag->bottomMargin();
    if (!parag->next())
        m = 0;
    else
        m = qMax(m, parag->next()->topMargin()) / 2;
    parag->setFullWidth(fullWidth);
    y += qMax(h, linespacing) + m;

    wused += rm;
    if (!wrapEnabled || wrapAtColumn() != -1)
        minw = qMax(minw, wused);

    // This is the case where we are breaking wherever we darn well please
    // in cases like that, the minw should not be the length of the entire
    // word, because we necessarily want to show the word on the whole line.
    // example: word wrap in iconview
    if (allowBreakInWords() && minw > wused)
        minw = wused;

    thisminw = minw;
    thiswused = wused;
    return y;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Q3TextIndent::Q3TextIndent()
{
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Q3TextFormatCollection::Q3TextFormatCollection()
    : paintdevice(0)
{
    defFormat = new Q3TextFormat(QApplication::font(),
                                  QApplication::palette().color(QPalette::Active, QPalette::Text));
    lastFormat = cres = 0;
    cflags = -1;
    cachedFormat = 0;
}

Q3TextFormatCollection::~Q3TextFormatCollection()
{
    QHash<QString, Q3TextFormat *>::ConstIterator it = cKey.constBegin();
    while (it != cKey.constEnd()) {
        delete it.value();
        ++it;
    }
    delete defFormat;
}

void Q3TextFormatCollection::setPaintDevice(QPaintDevice *pd)
{
    paintdevice = pd;

#if defined(Q_WS_X11)
    int scr = (paintdevice) ? paintdevice->x11Screen() : QX11Info::appScreen();

    defFormat->fn.x11SetScreen(scr);
    defFormat->update();

    QHash<QString, Q3TextFormat *>::Iterator it = cKey.begin();
    for (; it != cKey.end(); ++it) {
        Q3TextFormat *format = *it;
        format->fn.x11SetScreen(scr);
        format->update();
    }
#endif // Q_WS_X11
}

Q3TextFormat *Q3TextFormatCollection::format(Q3TextFormat *f)
{
    if (f->parent() == this || f == defFormat) {
        lastFormat = f;
        lastFormat->addRef();
        return lastFormat;
    }

    if (f == lastFormat || (lastFormat && f->key() == lastFormat->key())) {
        lastFormat->addRef();
        return lastFormat;
    }

    Q3TextFormat *fm = cKey.value(f->key());
    if (fm) {
        lastFormat = fm;
        lastFormat->addRef();
        return lastFormat;
    }

    if (f->key() == defFormat->key())
        return defFormat;

    lastFormat = createFormat(*f);
    lastFormat->collection = this;
    cKey.insert(lastFormat->key(), lastFormat);
    return lastFormat;
}

Q3TextFormat *Q3TextFormatCollection::format(Q3TextFormat *of, Q3TextFormat *nf, int flags)
{
    if (cres && kof == of->key() && knf == nf->key() && cflags == flags) {
        cres->addRef();
        return cres;
    }

    cres = createFormat(*of);
    kof = of->key();
    knf = nf->key();
    cflags = flags;
    if (flags & Q3TextFormat::Bold)
        cres->fn.setBold(nf->fn.bold());
    if (flags & Q3TextFormat::Italic)
        cres->fn.setItalic(nf->fn.italic());
    if (flags & Q3TextFormat::Underline)
        cres->fn.setUnderline(nf->fn.underline());
    if (flags & Q3TextFormat::StrikeOut)
        cres->fn.setStrikeOut(nf->fn.strikeOut());
    if (flags & Q3TextFormat::Family)
        cres->fn.setFamily(nf->fn.family());
    if (flags & Q3TextFormat::Size) {
        if (of->usePixelSizes)
            cres->fn.setPixelSize(nf->fn.pixelSize());
        else
            cres->fn.setPointSize(nf->fn.pointSize());
    }
    if (flags & Q3TextFormat::Color)
        cres->col = nf->col;
    if (flags & Q3TextFormat::Misspelled)
        cres->missp = nf->missp;
    if (flags & Q3TextFormat::VAlign)
        cres->ha = nf->ha;
    cres->update();

    Q3TextFormat *fm = cKey.value(cres->key());
    if (!fm) {
        cres->collection = this;
        cKey.insert(cres->key(), cres);
    } else {
        delete cres;
        cres = fm;
        cres->addRef();
    }

    return cres;
}

Q3TextFormat *Q3TextFormatCollection::format(const QFont &f, const QColor &c)
{
    if (cachedFormat && cfont == f && ccol == c) {
        cachedFormat->addRef();
        return cachedFormat;
    }

    QString key = Q3TextFormat::getKey(f, c, false, Q3TextFormat::AlignNormal);
    cachedFormat = cKey.value(key);
    cfont = f;
    ccol = c;

    if (cachedFormat) {
        cachedFormat->addRef();
        return cachedFormat;
    }

    if (key == defFormat->key())
        return defFormat;

    cachedFormat = createFormat(f, c);
    cachedFormat->collection = this;
    cKey.insert(cachedFormat->key(), cachedFormat);
    if (cachedFormat->key() != key)
        qWarning("ASSERT: keys for format not identical: '%s '%s'", cachedFormat->key().latin1(), key.latin1());
    return cachedFormat;
}

void Q3TextFormatCollection::remove(Q3TextFormat *f)
{
    if (lastFormat == f)
        lastFormat = 0;
    if (cres == f)
        cres = 0;
    if (cachedFormat == f)
        cachedFormat = 0;
    if (cKey.value(f->key()) == f)
        delete cKey.take(f->key());
}

#define UPDATE(up, lo, rest) \
        if (font.lo##rest() != defFormat->fn.lo##rest() && fm->fn.lo##rest() == defFormat->fn.lo##rest()) \
            fm->fn.set##up##rest(font.lo##rest())

void Q3TextFormatCollection::updateDefaultFormat(const QFont &font, const QColor &color, Q3StyleSheet *sheet)
{
    bool usePixels = font.pointSize() == -1;
    bool changeSize = usePixels ? font.pixelSize() != defFormat->fn.pixelSize() :
        font.pointSize() != defFormat->fn.pointSize();
    int base = usePixels ? font.pixelSize() : font.pointSize();
    QHash<QString, Q3TextFormat *>::Iterator it = cKey.begin();
    for (; it != cKey.end(); ++it) {
        Q3TextFormat *fm = *it;
        UPDATE(F, f, amily);
        UPDATE(W, w, eight);
        UPDATE(B, b, old);
        UPDATE(I, i, talic);
        UPDATE(U, u, nderline);
        if (changeSize) {
            fm->stdSize = base;
            fm->usePixelSizes = usePixels;
            if (usePixels)
                fm->fn.setPixelSize(fm->stdSize);
            else
                fm->fn.setPointSize(fm->stdSize);
            sheet->scaleFont(fm->fn, fm->logicalFontSize);
        }
        if (color.isValid() && color != defFormat->col && fm->col == defFormat->col)
            fm->col = color;
        fm->update();
    }

    defFormat->fn = font;
    defFormat->col = color;
    defFormat->update();
    defFormat->stdSize = base;
    defFormat->usePixelSizes = usePixels;

    updateKeys();
}

// the keys in cKey have changed, rebuild the hashtable
void Q3TextFormatCollection::updateKeys()
{
    if (cKey.isEmpty())
        return;
    Q3TextFormat** formats = new Q3TextFormat *[cKey.count() + 1];
    Q3TextFormat **f = formats;
    for (QHash<QString, Q3TextFormat *>::Iterator it = cKey.begin(); it != cKey.end(); ++it, ++f)
        *f = *it;
    *f = 0;
    cKey.clear();
    for (f = formats; *f; f++)
        cKey.insert((*f)->key(), *f);
    delete [] formats;
}



// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void Q3TextFormat::setBold(bool b)
{
    if (b == fn.bold())
        return;
    fn.setBold(b);
    update();
}

void Q3TextFormat::setMisspelled(bool b)
{
    if (b == (bool)missp)
        return;
    missp = b;
    update();
}

void Q3TextFormat::setVAlign(VerticalAlignment a)
{
    if (a == ha)
        return;
    ha = a;
    update();
}

void Q3TextFormat::setItalic(bool b)
{
    if (b == fn.italic())
        return;
    fn.setItalic(b);
    update();
}

void Q3TextFormat::setUnderline(bool b)
{
    if (b == fn.underline())
        return;
    fn.setUnderline(b);
    update();
}

void Q3TextFormat::setStrikeOut(bool b)
{
    if (b == fn.strikeOut())
        return;
    fn.setStrikeOut(b);
    update();
}

void Q3TextFormat::setFamily(const QString &f)
{
    if (f == fn.family())
        return;
    fn.setFamily(f);
    update();
}

void Q3TextFormat::setPointSize(int s)
{
    if (s == fn.pointSize())
        return;
    fn.setPointSize(s);
    usePixelSizes = false;
    update();
}

void Q3TextFormat::setFont(const QFont &f)
{
    if (f == fn && !k.isEmpty())
        return;
    fn = f;
    update();
}

void Q3TextFormat::setColor(const QColor &c)
{
    if (c == col)
        return;
    col = c;
    update();
}

QString Q3TextFormat::makeFormatChangeTags(Q3TextFormat* defaultFormat, Q3TextFormat *f,
                           const QString& oldAnchorHref, const QString& anchorHref ) const
{
    QString tag;
    if (f)
        tag += f->makeFormatEndTags(defaultFormat, oldAnchorHref);

    if (!anchorHref.isEmpty())
        tag += QLatin1String("<a href=\"") + anchorHref + QLatin1String("\">");

    if (font() != defaultFormat->font()
        || vAlign() != defaultFormat->vAlign()
        || color().rgb() != defaultFormat->color().rgb()) {
        QString s;
        if (font().family() != defaultFormat->font().family())
            s += QString(s.size()?QLatin1String(";"):QLatin1String("")) + QLatin1String("font-family:") + fn.family();
        if (font().italic() && font().italic() != defaultFormat->font().italic())
            s += QString(s.size()?QLatin1String(";"):QLatin1String("")) + QLatin1String("font-style:") + (font().italic() ? QLatin1String("italic") : QLatin1String("normal"));
        if (font().pointSize() != defaultFormat->font().pointSize())
            s += QString(s.size()?QLatin1String(";"):QLatin1String("")) + QLatin1String("font-size:") + QString::number(fn.pointSize()) + QLatin1String("pt");
        if (font().weight() != defaultFormat->font().weight())
            s += QString(s.size()?QLatin1String(";"):QLatin1String("")) + QLatin1String("font-weight:") + QString::number(fn.weight() * 8);
        QString textDecoration;
        bool none = false;
 	if ( font().underline() != defaultFormat->font().underline() ) {
            if (font().underline())
                textDecoration = QLatin1String("underline");
            else
                none = true;
        }
 	if ( font().overline() != defaultFormat->font().overline() ) {
            if (font().overline())
                textDecoration += QLatin1String(" overline");
            else
                none = true;
        }
	if ( font().strikeOut() != defaultFormat->font().strikeOut() ) {
            if (font().strikeOut())
                textDecoration += QLatin1String(" line-through");
            else
                none = true;
        }
        if (none && textDecoration.isEmpty())
            textDecoration = QLatin1String("none");
        if (!textDecoration.isEmpty())
 	    s += QString(s.size()?QLatin1String(";"):QLatin1String("")) + QLatin1String("text-decoration:") + textDecoration;
        if (vAlign() != defaultFormat->vAlign()) {
            s += QString(s.size()?QLatin1String(";"):QLatin1String("")) + QLatin1String("vertical-align:");
            if (vAlign() == Q3TextFormat::AlignSuperScript)
                s += QLatin1String("super");
            else if (vAlign() == Q3TextFormat::AlignSubScript)
                s += QLatin1String("sub");
            else
                s += QLatin1String("normal");
        }
        if (color().rgb() != defaultFormat->color().rgb())
            s += QString(s.size()?QLatin1String(";"):QLatin1String("")) + QLatin1String("color:") + col.name();
        if (!s.isEmpty())
            tag += QLatin1String("<span style=\"") + s + QLatin1String("\">");
    }

    return tag;
}

QString Q3TextFormat::makeFormatEndTags(Q3TextFormat* defaultFormat, const QString& anchorHref) const
{
    QString tag;
    if (font().family() != defaultFormat->font().family()
         || font().pointSize() != defaultFormat->font().pointSize()
         || font().weight() != defaultFormat->font().weight()
         || font().italic() != defaultFormat->font().italic()
         || font().underline() != defaultFormat->font().underline()
         || font().strikeOut() != defaultFormat->font().strikeOut()
         || vAlign() != defaultFormat->vAlign()
         || color().rgb() != defaultFormat->color().rgb())
        tag += QLatin1String("</span>");
    if (!anchorHref.isEmpty())
        tag += QLatin1String("</a>");
    return tag;
}

Q3TextFormat Q3TextFormat::makeTextFormat(const Q3StyleSheetItem *style, const QMap<QString,QString>& attr, double scaleFontsFactor) const
{
    Q3TextFormat format(*this);
    if (!style)
        return format;

    if (!style->isAnchor() && style->color().isValid()) {
        // the style is not an anchor and defines a color.
        // It might be used inside an anchor and it should
        // override the link color.
        format.linkColor = false;
    }
    switch (style->verticalAlignment()) {
    case Q3StyleSheetItem::VAlignBaseline:
        format.setVAlign(Q3TextFormat::AlignNormal);
        break;
    case Q3StyleSheetItem::VAlignSuper:
        format.setVAlign(Q3TextFormat::AlignSuperScript);
        break;
    case Q3StyleSheetItem::VAlignSub:
        format.setVAlign(Q3TextFormat::AlignSubScript);
        break;
    }

    if (style->fontWeight() != Q3StyleSheetItem::Undefined)
        format.fn.setWeight(style->fontWeight());
    if (style->fontSize() != Q3StyleSheetItem::Undefined) {
        format.fn.setPointSize(style->fontSize());
    } else if (style->logicalFontSize() != Q3StyleSheetItem::Undefined) {
        format.logicalFontSize = style->logicalFontSize();
        if (format.usePixelSizes)
            format.fn.setPixelSize(format.stdSize);
        else
            format.fn.setPointSize(format.stdSize);
        style->styleSheet()->scaleFont(format.fn, format.logicalFontSize);
    } else if (style->logicalFontSizeStep()) {
        format.logicalFontSize += style->logicalFontSizeStep();
        if (format.usePixelSizes)
            format.fn.setPixelSize(format.stdSize);
        else
            format.fn.setPointSize(format.stdSize);
        style->styleSheet()->scaleFont(format.fn, format.logicalFontSize);
    }
    if (!style->fontFamily().isEmpty())
        format.fn.setFamily(style->fontFamily());
    if (style->color().isValid())
        format.col = style->color();
    if (style->definesFontItalic())
        format.fn.setItalic(style->fontItalic());
    if (style->definesFontUnderline())
        format.fn.setUnderline(style->fontUnderline());
    if (style->definesFontStrikeOut())
        format.fn.setStrikeOut(style->fontStrikeOut());

    QMap<QString, QString>::ConstIterator it, end = attr.end();

    if (style->name() == QLatin1String("font")) {
        it = attr.find(QLatin1String("color"));
        if (it != end && ! (*it).isEmpty()){
            format.col.setNamedColor(*it);
            format.linkColor = false;
        }
        it = attr.find(QLatin1String("face"));
        if (it != end) {
            QString family = (*it).section(QLatin1Char(','), 0, 0);
            if (family.size())
                format.fn.setFamily(family);
        }
        it = attr.find(QLatin1String("size"));
        if (it != end) {
            QString a = *it;
            int n = a.toInt();
            if (a[0] == QLatin1Char('+') || a[0] == QLatin1Char('-'))
                n += 3;
            format.logicalFontSize = n;
            if (format.usePixelSizes)
                format.fn.setPixelSize(format.stdSize);
            else
                format.fn.setPointSize(format.stdSize);
            style->styleSheet()->scaleFont(format.fn, format.logicalFontSize);
        }
    }

    it = attr.find(QLatin1String("style"));
    if (it != end) {
        QString a = *it;
        int count = a.count(QLatin1Char(';'))+1;
        for (int s = 0; s < count; s++) {
            QString style = a.section(QLatin1Char(';'), s, s);
            if (style.startsWith(QLatin1String("font-size:")) && style.endsWith(QLatin1String("pt"))) {
                format.logicalFontSize = 0;
                int size = int(scaleFontsFactor * style.mid(10, style.length() - 12).toDouble());
                format.setPointSize(size);
            } else if (style.startsWith(QLatin1String("font-style:"))) {
                QString s = style.mid(11).trimmed();
                if (s == QLatin1String("normal"))
                    format.fn.setItalic(false);
                else if (s == QLatin1String("italic") || s == QLatin1String("oblique"))
                    format.fn.setItalic(true);
            } else if (style.startsWith(QLatin1String("font-weight:"))) {
                QString s = style.mid(12);
                bool ok = true;
                int n = s.toInt(&ok);
                if (ok)
                    format.fn.setWeight(n/8);
            } else if (style.startsWith(QLatin1String("font-family:"))) {
                QString family = style.mid(12).section(QLatin1Char(','),0,0);
                family.replace(QLatin1Char('\"'), QLatin1Char(' '));
                family.replace(QLatin1Char('\''), QLatin1Char(' '));
                family = family.trimmed();
                format.fn.setFamily(family);
            } else if (style.startsWith(QLatin1String("text-decoration:"))) {
 		QString s = style.mid( 16 );
 		format.fn.setOverline(s.contains(QLatin1String("overline")));
 		format.fn.setStrikeOut(s.contains(QLatin1String("line-through")));
 		format.fn.setUnderline(s.contains(QLatin1String("underline")));
            } else if (style.startsWith(QLatin1String("vertical-align:"))) {
                QString s = style.mid(15).trimmed();
                if (s == QLatin1String("sub"))
                    format.setVAlign(Q3TextFormat::AlignSubScript);
                else if (s == QLatin1String("super"))
                    format.setVAlign(Q3TextFormat::AlignSuperScript);
                else
                    format.setVAlign(Q3TextFormat::AlignNormal);
            } else if (style.startsWith(QLatin1String("color:"))) {
                format.col.setNamedColor(style.mid(6));
                format.linkColor = false;
            }
        }
    }

    format.update();
    return format;
}

#ifndef QT_NO_TEXTCUSTOMITEM

struct QPixmapInt
{
    QPixmapInt() : ref(0) {}
    QPixmap pm;
    int            ref;
    Q_DUMMY_COMPARISON_OPERATOR(QPixmapInt)
};

static QMap<QString, QPixmapInt> *pixmap_map = 0;

Q3TextImage::Q3TextImage(Q3TextDocument *p, const QMap<QString, QString> &attr, const QString& context,
                        Q3MimeSourceFactory &factory)
    : Q3TextCustomItem(p)
{
    width = height = 0;

    QMap<QString, QString>::ConstIterator it, end = attr.end();
    it = attr.find(QLatin1String("width"));
    if (it != end)
        width = (*it).toInt();
    it = attr.find(QLatin1String("height"));
    if (it != end)
        height = (*it).toInt();

    reg = 0;
    QString imageName = attr[QLatin1String("src")];

    if (imageName.size() == 0)
        imageName = attr[QLatin1String("source")];

    if (!imageName.isEmpty()) {
        imgId = QString(QLatin1String("%1,%2,%3,%4")).arg(imageName).arg(width).arg(height).arg((ulong)&factory);
        if (!pixmap_map)
            pixmap_map = new QMap<QString, QPixmapInt>;
        if (pixmap_map->contains(imgId)) {
            QPixmapInt& pmi = pixmap_map->operator[](imgId);
            pm = pmi.pm;
            pmi.ref++;
            width = pm.width();
            height = pm.height();
        } else {
            QImage img;
            const QMimeSource* m =
                factory.data(imageName, context);
            if (!m) {
                qCritical("Q3TextImage: no mimesource for %s", imageName.latin1());
            }
            else {
                if (!Q3ImageDrag::decode(m, img)) {
                    qCritical("Q3TextImage: cannot decode %s", imageName.latin1());
                }
            }

            if (!img.isNull()) {
                if (width == 0) {
                    width = img.width();
                    if (height != 0) {
                        width = img.width() * height / img.height();
                    }
                }
                if (height == 0) {
                    height = img.height();
                    if (width != img.width()) {
                        height = img.height() * width / img.width();
                    }
                }
                if (img.width() != width || img.height() != height){
#ifndef QT_NO_IMAGE_SMOOTHSCALE
                    img = img.smoothScale(width, height);
#endif
                    width = img.width();
                    height = img.height();
                }
                pm.convertFromImage(img);
            }
            if (!pm.isNull()) {
                QPixmapInt& pmi = pixmap_map->operator[](imgId);
                pmi.pm = pm;
                pmi.ref++;
            }
        }
        if (pm.hasAlphaChannel()) {
            QRegion mask(pm.mask());
            QRegion all(0, 0, pm.width(), pm.height());
            reg = new QRegion(all.subtracted(mask));
        }
    }

    if (pm.isNull() && (width*height)==0)
        width = height = 50;

    place = PlaceInline;
    if (attr[QLatin1String("align")] == QLatin1String("left"))
        place = PlaceLeft;
    else if (attr[QLatin1String("align")] == QLatin1String("right"))
        place = PlaceRight;

    tmpwidth = width;
    tmpheight = height;

    attributes = attr;
}

Q3TextImage::~Q3TextImage()
{
    if (pixmap_map && pixmap_map->contains(imgId)) {
        QPixmapInt& pmi = pixmap_map->operator[](imgId);
        pmi.ref--;
        if (!pmi.ref) {
            pixmap_map->remove(imgId);
            if (pixmap_map->isEmpty()) {
                delete pixmap_map;
                pixmap_map = 0;
            }
        }
    }
    delete reg;
}

QString Q3TextImage::richText() const
{
    QString s;
    s += QLatin1String("<img ");
    QMap<QString, QString>::ConstIterator it = attributes.begin();
    for (; it != attributes.end(); ++it) {
        s += it.key() + QLatin1String("=");
        if ((*it).contains(QLatin1Char(' ')))
            s += QLatin1String("\"") + *it + QLatin1String("\" ");
        else
            s += *it + QLatin1String(" ");
    }
    s += QLatin1String(">");
    return s;
}

void Q3TextImage::adjustToPainter(QPainter* p)
{
    width = scale(tmpwidth, p);
    height = scale(tmpheight, p);
}

#if !defined(Q_WS_X11)
static QPixmap *qrt_selection = 0;
static Q3SingleCleanupHandler<QPixmap> qrt_cleanup_pixmap;
static void qrt_createSelectionPixmap(const QPalette &pal)
{
    qrt_selection = new QPixmap(2, 2);
    qrt_cleanup_pixmap.set(&qrt_selection);
    qrt_selection->fill(Qt::color0);
    QBitmap m(2, 2);
    m.fill(Qt::color1);
    QPainter p(&m);
    p.setPen(Qt::color0);
    for (int j = 0; j < 2; ++j) {
        p.drawPoint(j % 2, j);
    }
    p.end();
    qrt_selection->setMask(m);
    qrt_selection->fill(pal.highlight().color());
}
#endif

void Q3TextImage::draw(QPainter* p, int x, int y, int cx, int cy, int cw, int ch,
                       const QPalette &pal, bool selected)
{
    if (placement() != PlaceInline) {
        x = xpos;
        y = ypos;
    }

    if (pm.isNull()) {
        p->fillRect(x , y, width, height, pal.dark());
        return;
    }

    if (is_printer(p)) {
        p->drawPixmap(QRect(x, y, width, height), pm);
        return;
    }

    if (placement() != PlaceInline && !QRect(xpos, ypos, width, height).intersects(QRect(cx, cy, cw, ch)))
        return;

    if (placement() == PlaceInline)
        p->drawPixmap(x , y, pm);
    else
        p->drawPixmap(cx , cy, pm, cx - x, cy - y, cw, ch);

    if (selected && placement() == PlaceInline && is_printer(p)) {
#if defined(Q_WS_X11)
        p->fillRect(QRect(QPoint(x, y), pm.size()), QBrush(pal.highlight(),
                                                                 Qt::Dense4Pattern));
#else // in WIN32 Qt::Dense4Pattern doesn't work correctly (transparency problem), so work around it
        if (!qrt_selection)
            qrt_createSelectionPixmap(pal);
        p->drawTiledPixmap(x, y, pm.width(), pm.height(), *qrt_selection);
#endif
    }
}

void Q3TextHorizontalLine::adjustToPainter(QPainter* p)
{
    height = scale(tmpheight, p);
}


Q3TextHorizontalLine::Q3TextHorizontalLine(Q3TextDocument *p, const QMap<QString, QString> &attr,
                                          const QString &,
                                          Q3MimeSourceFactory &)
    : Q3TextCustomItem(p)
{
    height = tmpheight = 8;
    QMap<QString, QString>::ConstIterator it, end = attr.end();
    it = attr.find(QLatin1String("color"));
    if (it != end)
        color = QColor(*it);
    shade = attr.find(QLatin1String("noshade")) == end;
}

Q3TextHorizontalLine::~Q3TextHorizontalLine()
{
}

QString Q3TextHorizontalLine::richText() const
{
    return QLatin1String("<hr>");
}

void Q3TextHorizontalLine::draw(QPainter* p, int x, int y, int , int , int , int ,
                                const QPalette& pal, bool selected)
{
    QRect r(x, y, width, height);
    if (is_printer(p) || !shade) {
        QPen oldPen = p->pen();
        if (!color.isValid())
            p->setPen(QPen(pal.text().color(), is_printer(p) ? height/8 : qMax(2, height/4)));
        else
            p->setPen(QPen(color, is_printer(p) ? height/8 : qMax(2, height/4)));
        p->drawLine(r.left()-1, y + height / 2, r.right() + 1, y + height / 2);
        p->setPen(oldPen);
    } else {
        if (selected)
            p->fillRect(r, pal.highlight());
        QPalette pal2(pal);
        if (color.isValid())
            pal2.setColor(pal2.currentColorGroup(), QPalette::Dark, color);
        qDrawShadeLine(p, r.left() - 1, y + height / 2, r.right() + 1, y + height / 2, pal2,
                        true, height / 8);
    }
}
#endif //QT_NO_TEXTCUSTOMITEM

/*****************************************************************/
// Small set of utility functions to make the parser a bit simpler
//

bool Q3TextDocument::hasPrefix(const QChar* doc, int length, int pos, QChar c)
{
    if (pos + 1 > length)
        return false;
    return doc[pos].toLower() == c.toLower();
}

bool Q3TextDocument::hasPrefix(const QChar* doc, int length, int pos, const QString& s)
{
    if (pos + (int) s.length() > length)
        return false;
    for (int i = 0; i < (int)s.length(); i++) {
        if (doc[pos + i].toLower() != s[i].toLower())
            return false;
    }
    return true;
}

#ifndef QT_NO_TEXTCUSTOMITEM
static bool qt_is_cell_in_use(QList<Q3TextTableCell *>& cells, int row, int col)
{
    for (int idx = 0; idx < cells.size(); ++idx) {
        Q3TextTableCell *c = cells.at(idx);
        if (row >= c->row() && row < c->row() + c->rowspan()
             && col >= c->column() && col < c->column() + c->colspan())
            return true;
    }
    return false;
}

Q3TextCustomItem* Q3TextDocument::parseTable(const QMap<QString, QString> &attr, const Q3TextFormat &fmt,
                                            const QChar* doc, int length, int& pos, Q3TextParagraph *curpar)
{

    Q3TextTable* table = new Q3TextTable(this, attr);
    int row = -1;
    int col = -1;

    QString rowbgcolor;
    QString rowalign;
    QString tablebgcolor = attr[QLatin1String("bgcolor")];

    QList<Q3TextTableCell *> multicells;

    QString tagname;
    (void) eatSpace(doc, length, pos);
    while (pos < length) {
        if (hasPrefix(doc, length, pos, QLatin1Char('<'))){
            if (hasPrefix(doc, length, pos+1, QLatin1Char('/'))) {
                tagname = parseCloseTag(doc, length, pos);
                if (tagname == QLatin1String("table")) {
                    return table;
                }
            } else {
                QMap<QString, QString> attr2;
                bool emptyTag = false;
                tagname = parseOpenTag(doc, length, pos, attr2, emptyTag);
                if (tagname == QLatin1String("tr")) {
                    rowbgcolor = attr2[QLatin1String("bgcolor")];
                    rowalign = attr2[QLatin1String("align")];
                    row++;
                    col = -1;
                }
                else if (tagname == QLatin1String("td") || tagname == QLatin1String("th")) {
                    col++;
                    while (qt_is_cell_in_use(multicells, row, col)) {
                        col++;
                    }

                    if (row >= 0 && col >= 0) {
                        const Q3StyleSheetItem* s = sheet_->item(tagname);
                        if (!attr2.contains(QLatin1String("bgcolor"))) {
                            if (!rowbgcolor.isEmpty())
                                attr2[QLatin1String("bgcolor")] = rowbgcolor;
                            else if (!tablebgcolor.isEmpty())
                                attr2[QLatin1String("bgcolor")] = tablebgcolor;
                        }
                        if (!attr2.contains(QLatin1String("align"))) {
                            if (!rowalign.isEmpty())
                                attr2[QLatin1String("align")] = rowalign;
                        }

                        // extract the cell contents
                        int end = pos;
                        while (end < length
                                && !hasPrefix(doc, length, end, QLatin1String("</td"))
                                && !hasPrefix(doc, length, end, QLatin1String("<td"))
                                && !hasPrefix(doc, length, end, QLatin1String("</th"))
                                && !hasPrefix(doc, length, end, QLatin1String("<th"))
                                && !hasPrefix(doc, length, end, QLatin1String("<td"))
                                && !hasPrefix(doc, length, end, QLatin1String("</tr"))
                                && !hasPrefix(doc, length, end, QLatin1String("<tr"))
                                && !hasPrefix(doc, length, end, QLatin1String("</table"))) {
                            if (hasPrefix(doc, length, end, QLatin1String("<table"))) { // nested table
                                int nested = 1;
                                ++end;
                                while (end < length && nested != 0) {
                                    if (hasPrefix(doc, length, end, QLatin1String("</table")))
                                        nested--;
                                    if (hasPrefix(doc, length, end, QLatin1String("<table")))
                                        nested++;
                                    end++;
                                }
                            }
                            end++;
                        }
                        Q3TextTableCell* cell = new Q3TextTableCell(table, row, col,
                                            attr2, s, fmt.makeTextFormat(s, attr2, scaleFontsFactor),
                                            contxt, *factory_, sheet_,
                                            QString::fromRawData(doc + pos, end - pos));
                        cell->richText()->parentPar = curpar;
                        if (cell->colspan() > 1 || cell->rowspan() > 1)
                            multicells.append(cell);
                        col += cell->colspan()-1;
                        pos = end;
                    }
                }
            }

        } else {
            ++pos;
        }
    }
    return table;
}
#endif // QT_NO_TEXTCUSTOMITEM

bool Q3TextDocument::eatSpace(const QChar* doc, int length, int& pos, bool includeNbsp)
{
    int old_pos = pos;
    while (pos < length && doc[pos].isSpace() && (includeNbsp || (doc[pos] != QChar(QChar::nbsp))))
        pos++;
    return old_pos < pos;
}

bool Q3TextDocument::eat(const QChar* doc, int length, int& pos, QChar c)
{
    bool ok = pos < length && doc[pos] == c;
    if (ok)
        pos++;
    return ok;
}
/*****************************************************************/

struct Entity {
    const char * name;
    Q_UINT16 code;
};

static const Entity entitylist [] = {
    { "AElig", 0x00c6 },
    { "Aacute", 0x00c1 },
    { "Acirc", 0x00c2 },
    { "Agrave", 0x00c0 },
    { "Alpha", 0x0391 },
    { "AMP", 38 },
    { "Aring", 0x00c5 },
    { "Atilde", 0x00c3 },
    { "Auml", 0x00c4 },
    { "Beta", 0x0392 },
    { "Ccedil", 0x00c7 },
    { "Chi", 0x03a7 },
    { "Dagger", 0x2021 },
    { "Delta", 0x0394 },
    { "ETH", 0x00d0 },
    { "Eacute", 0x00c9 },
    { "Ecirc", 0x00ca },
    { "Egrave", 0x00c8 },
    { "Epsilon", 0x0395 },
    { "Eta", 0x0397 },
    { "Euml", 0x00cb },
    { "Gamma", 0x0393 },
    { "GT", 62 },
    { "Iacute", 0x00cd },
    { "Icirc", 0x00ce },
    { "Igrave", 0x00cc },
    { "Iota", 0x0399 },
    { "Iuml", 0x00cf },
    { "Kappa", 0x039a },
    { "Lambda", 0x039b },
    { "LT", 60 },
    { "Mu", 0x039c },
    { "Ntilde", 0x00d1 },
    { "Nu", 0x039d },
    { "OElig", 0x0152 },
    { "Oacute", 0x00d3 },
    { "Ocirc", 0x00d4 },
    { "Ograve", 0x00d2 },
    { "Omega", 0x03a9 },
    { "Omicron", 0x039f },
    { "Oslash", 0x00d8 },
    { "Otilde", 0x00d5 },
    { "Ouml", 0x00d6 },
    { "Phi", 0x03a6 },
    { "Pi", 0x03a0 },
    { "Prime", 0x2033 },
    { "Psi", 0x03a8 },
    { "QUOT", 34 },
    { "Rho", 0x03a1 },
    { "Scaron", 0x0160 },
    { "Sigma", 0x03a3 },
    { "THORN", 0x00de },
    { "Tau", 0x03a4 },
    { "Theta", 0x0398 },
    { "Uacute", 0x00da },
    { "Ucirc", 0x00db },
    { "Ugrave", 0x00d9 },
    { "Upsilon", 0x03a5 },
    { "Uuml", 0x00dc },
    { "Xi", 0x039e },
    { "Yacute", 0x00dd },
    { "Yuml", 0x0178 },
    { "Zeta", 0x0396 },
    { "aacute", 0x00e1 },
    { "acirc", 0x00e2 },
    { "acute", 0x00b4 },
    { "aelig", 0x00e6 },
    { "agrave", 0x00e0 },
    { "alefsym", 0x2135 },
    { "alpha", 0x03b1 },
    { "amp", 38 },
    { "and", 0x22a5 },
    { "ang", 0x2220 },
    { "apos", 0x0027 },
    { "aring", 0x00e5 },
    { "asymp", 0x2248 },
    { "atilde", 0x00e3 },
    { "auml", 0x00e4 },
    { "bdquo", 0x201e },
    { "beta", 0x03b2 },
    { "brvbar", 0x00a6 },
    { "bull", 0x2022 },
    { "cap", 0x2229 },
    { "ccedil", 0x00e7 },
    { "cedil", 0x00b8 },
    { "cent", 0x00a2 },
    { "chi", 0x03c7 },
    { "circ", 0x02c6 },
    { "clubs", 0x2663 },
    { "cong", 0x2245 },
    { "copy", 0x00a9 },
    { "crarr", 0x21b5 },
    { "cup", 0x222a },
    { "cur" "ren", 0x00a4 },
    { "dArr", 0x21d3 },
    { "dagger", 0x2020 },
    { "darr", 0x2193 },
    { "deg", 0x00b0 },
    { "delta", 0x03b4 },
    { "diams", 0x2666 },
    { "divide", 0x00f7 },
    { "eacute", 0x00e9 },
    { "ecirc", 0x00ea },
    { "egrave", 0x00e8 },
    { "empty", 0x2205 },
    { "emsp", 0x2003 },
    { "ensp", 0x2002 },
    { "epsilon", 0x03b5 },
    { "equiv", 0x2261 },
    { "eta", 0x03b7 },
    { "eth", 0x00f0 },
    { "euml", 0x00eb },
    { "euro", 0x20ac },
    { "exist", 0x2203 },
    { "fnof", 0x0192 },
    { "forall", 0x2200 },
    { "frac12", 0x00bd },
    { "frac14", 0x00bc },
    { "frac34", 0x00be },
    { "frasl", 0x2044 },
    { "gamma", 0x03b3 },
    { "ge", 0x2265 },
    { "gt", 62 },
    { "hArr", 0x21d4 },
    { "harr", 0x2194 },
    { "hearts", 0x2665 },
    { "hellip", 0x2026 },
    { "iacute", 0x00ed },
    { "icirc", 0x00ee },
    { "iexcl", 0x00a1 },
    { "igrave", 0x00ec },
    { "image", 0x2111 },
    { "infin", 0x221e },
    { "int", 0x222b },
    { "iota", 0x03b9 },
    { "iquest", 0x00bf },
    { "isin", 0x2208 },
    { "iuml", 0x00ef },
    { "kappa", 0x03ba },
    { "lArr", 0x21d0 },
    { "lambda", 0x03bb },
    { "lang", 0x2329 },
    { "laquo", 0x00ab },
    { "larr", 0x2190 },
    { "lceil", 0x2308 },
    { "ldquo", 0x201c },
    { "le", 0x2264 },
    { "lfloor", 0x230a },
    { "lowast", 0x2217 },
    { "loz", 0x25ca },
    { "lrm", 0x200e },
    { "lsaquo", 0x2039 },
    { "lsquo", 0x2018 },
    { "lt", 60 },
    { "macr", 0x00af },
    { "mdash", 0x2014 },
    { "micro", 0x00b5 },
    { "middot", 0x00b7 },
    { "minus", 0x2212 },
    { "mu", 0x03bc },
    { "nabla", 0x2207 },
    { "nbsp", 0x00a0 },
    { "ndash", 0x2013 },
    { "ne", 0x2260 },
    { "ni", 0x220b },
    { "not", 0x00ac },
    { "notin", 0x2209 },
    { "nsub", 0x2284 },
    { "ntilde", 0x00f1 },
    { "nu", 0x03bd },
    { "oacute", 0x00f3 },
    { "ocirc", 0x00f4 },
    { "oelig", 0x0153 },
    { "ograve", 0x00f2 },
    { "oline", 0x203e },
    { "omega", 0x03c9 },
    { "omicron", 0x03bf },
    { "oplus", 0x2295 },
    { "or", 0x22a6 },
    { "ordf", 0x00aa },
    { "ordm", 0x00ba },
    { "oslash", 0x00f8 },
    { "otilde", 0x00f5 },
    { "otimes", 0x2297 },
    { "ouml", 0x00f6 },
    { "para", 0x00b6 },
    { "part", 0x2202 },
    { "percnt", 0x0025 },
    { "permil", 0x2030 },
    { "perp", 0x22a5 },
    { "phi", 0x03c6 },
    { "pi", 0x03c0 },
    { "piv", 0x03d6 },
    { "plusmn", 0x00b1 },
    { "pound", 0x00a3 },
    { "prime", 0x2032 },
    { "prod", 0x220f },
    { "prop", 0x221d },
    { "psi", 0x03c8 },
    { "quot", 34 },
    { "rArr", 0x21d2 },
    { "radic", 0x221a },
    { "rang", 0x232a },
    { "raquo", 0x00bb },
    { "rarr", 0x2192 },
    { "rceil", 0x2309 },
    { "rdquo", 0x201d },
    { "real", 0x211c },
    { "reg", 0x00ae },
    { "rfloor", 0x230b },
    { "rho", 0x03c1 },
    { "rlm", 0x200f },
    { "rsaquo", 0x203a },
    { "rsquo", 0x2019 },
    { "sbquo", 0x201a },
    { "scaron", 0x0161 },
    { "sdot", 0x22c5 },
    { "sect", 0x00a7 },
    { "shy", 0x00ad },
    { "sigma", 0x03c3 },
    { "sigmaf", 0x03c2 },
    { "sim", 0x223c },
    { "spades", 0x2660 },
    { "sub", 0x2282 },
    { "sube", 0x2286 },
    { "sum", 0x2211 },
    { "sup1", 0x00b9 },
    { "sup2", 0x00b2 },
    { "sup3", 0x00b3 },
    { "sup", 0x2283 },
    { "supe", 0x2287 },
    { "szlig", 0x00df },
    { "tau", 0x03c4 },
    { "there4", 0x2234 },
    { "theta", 0x03b8 },
    { "thetasym", 0x03d1 },
    { "thinsp", 0x2009 },
    { "thorn", 0x00fe },
    { "tilde", 0x02dc },
    { "times", 0x00d7 },
    { "trade", 0x2122 },
    { "uArr", 0x21d1 },
    { "uacute", 0x00fa },
    { "uarr", 0x2191 },
    { "ucirc", 0x00fb },
    { "ugrave", 0x00f9 },
    { "uml", 0x00a8 },
    { "upsih", 0x03d2 },
    { "upsilon", 0x03c5 },
    { "uuml", 0x00fc },
    { "weierp", 0x2118 },
    { "xi", 0x03be },
    { "yacute", 0x00fd },
    { "yen", 0x00a5 },
    { "yuml", 0x00ff },
    { "zeta", 0x03b6 },
    { "zwj", 0x200d },
    { "zwnj", 0x200c },
    { "", 0x0000 }
};





static QMap<QByteArray, QChar> *html_map = 0;
static void qt_cleanup_html_map()
{
    delete html_map;
    html_map = 0;
}

static QMap<QByteArray, QChar> *htmlMap()
{
    if (!html_map) {
        html_map = new QMap<QByteArray, QChar>;
        qAddPostRoutine(qt_cleanup_html_map);

        const Entity *ent = entitylist;
        while(ent->code) {
            html_map->insert(QByteArray(ent->name), QChar(ent->code));
            ent++;
        }
    }
    return html_map;
}

QChar Q3TextDocument::parseHTMLSpecialChar(const QChar* doc, int length, int& pos)
{
    QString s;
    pos++;
    int recoverpos = pos;
    while (pos < length && doc[pos] != QLatin1Char(';') && !doc[pos].isSpace() && pos < recoverpos + 8) {
        s += doc[pos];
        pos++;
    }
    if (doc[pos] != QLatin1Char(';') && !doc[pos].isSpace()) {
        pos = recoverpos;
        return QLatin1Char('&');
    }
    pos++;

    if (s.length() > 1 && s[0] == QLatin1Char('#')) {
        int off = 1;
        int base = 10;
        if (s[1] == QLatin1Char('x')) {
            off = 2;
            base = 16;
        }
        bool ok;
	int num = s.mid(off).toInt(&ok, base);
        if (num == 151) // ### hack for designer manual
            return QLatin1Char('-');
        return num;
    }

    QMap<QByteArray, QChar>::Iterator it = htmlMap()->find(s.toLatin1());
    if (it != htmlMap()->end()) {
        return *it;
    }

    pos = recoverpos;
    return QLatin1Char('&');
}

QString Q3TextDocument::parseWord(const QChar* doc, int length, int& pos, bool lower)
{
    QString s;

    if (doc[pos] == QLatin1Char('"')) {
        pos++;
        while (pos < length && doc[pos] != QLatin1Char('"')) {
            if (doc[pos] == QLatin1Char('&')) {
                s += parseHTMLSpecialChar(doc, length, pos);
            } else {
                s += doc[pos];
                pos++;
            }
        }
        eat(doc, length, pos, QLatin1Char('"'));
    } else if (doc[pos] == QLatin1Char('\'')) {
        pos++;
        while (pos < length  && doc[pos] != QLatin1Char('\'')) {
            s += doc[pos];
            pos++;
        }
        eat(doc, length, pos, QLatin1Char('\''));
    } else {
        static QString term = QString::fromLatin1("/>");
        while (pos < length
                && doc[pos] != QLatin1Char('>')
                && !hasPrefix(doc, length, pos, term)
                && doc[pos] != QLatin1Char('<')
                && doc[pos] != QLatin1Char('=')
                && !doc[pos].isSpace())
        {
            if (doc[pos] == QLatin1Char('&')) {
                s += parseHTMLSpecialChar(doc, length, pos);
            } else {
                s += doc[pos];
                pos++;
            }
        }
        if (lower)
            s = s.toLower();
    }
    return s;
}

QChar Q3TextDocument::parseChar(const QChar* doc, int length, int& pos, Q3StyleSheetItem::WhiteSpaceMode wsm)
{
    if (pos >=  length)
        return QChar::null;

    QChar c = doc[pos++];

    if (c == QLatin1Char('<'))
        return QChar::null;

    if (c.isSpace() && c != QChar(QChar::nbsp)) {
        if (wsm == Q3StyleSheetItem::WhiteSpacePre) {
            if (c == QLatin1Char('\n'))
                return QChar::LineSeparator;
            else
                return c;
        } else { // non-pre mode: collapse whitespace except nbsp
            while (pos< length &&
                    doc[pos].isSpace()  && doc[pos] != QChar(QChar::nbsp))
                pos++;
            return QLatin1Char(' ');
        }
    }
    else if (c == QLatin1Char('&'))
        return parseHTMLSpecialChar(doc, length, --pos);
    else
        return c;
}

QString Q3TextDocument::parseOpenTag(const QChar* doc, int length, int& pos,
                                  QMap<QString, QString> &attr, bool& emptyTag)
{
    emptyTag = false;
    pos++;
    if (hasPrefix(doc, length, pos, QLatin1Char('!'))) {
        if (hasPrefix(doc, length, pos+1, QLatin1String("--"))) {
            pos += 3;
            // eat comments
            QString pref = QString::fromLatin1("-->");
            while (!hasPrefix(doc, length, pos, pref) && pos < length)
                pos++;
            if (hasPrefix(doc, length, pos, pref)) {
                pos += 3;
                eatSpace(doc, length, pos, true);
            }
            emptyTag = true;
            return QString();
        }
        else {
            // eat strange internal tags
            while (!hasPrefix(doc, length, pos, QLatin1Char('>')) && pos < length)
                pos++;
            if (hasPrefix(doc, length, pos, QLatin1Char('>'))) {
                pos++;
                eatSpace(doc, length, pos, true);
            }
            return QString();
        }
    }

    QString tag = parseWord(doc, length, pos);
    eatSpace(doc, length, pos, true);
    static QString term = QString::fromLatin1("/>");
    static QString s_TRUE = QString::fromLatin1("TRUE");

    while (doc[pos] != QLatin1Char('>') && ! (emptyTag = hasPrefix(doc, length, pos, term))) {
        QString key = parseWord(doc, length, pos);
        eatSpace(doc, length, pos, true);
        if (key.isEmpty()) {
            // error recovery
            while (pos < length && doc[pos] != QLatin1Char('>'))
                pos++;
            break;
        }
        QString value;
        if (hasPrefix(doc, length, pos, QLatin1Char('='))){
            pos++;
            eatSpace(doc, length, pos);
            value = parseWord(doc, length, pos, false);
        }
        else
            value = s_TRUE;
        attr.insert(key.toLower(), value);
        eatSpace(doc, length, pos, true);
    }

    if (emptyTag) {
        eat(doc, length, pos, QLatin1Char('/'));
        eat(doc, length, pos, QLatin1Char('>'));
    }
    else
        eat(doc, length, pos, QLatin1Char('>'));

    return tag;
}

QString Q3TextDocument::parseCloseTag(const QChar* doc, int length, int& pos)
{
    pos++;
    pos++;
    QString tag = parseWord(doc, length, pos);
    eatSpace(doc, length, pos, true);
    eat(doc, length, pos, QLatin1Char('>'));
    return tag;
}

Q3TextFlow::Q3TextFlow()
{
    w = pagesize = 0;
}

Q3TextFlow::~Q3TextFlow()
{
    clear();
}

void Q3TextFlow::clear()
{
#ifndef QT_NO_TEXTCUSTOMITEM
    while (!leftItems.isEmpty())
        delete leftItems.takeFirst();
    while (!rightItems.isEmpty())
        delete rightItems.takeFirst();
#endif
}

void Q3TextFlow::setWidth(int width)
{
    w = width;
}

int Q3TextFlow::adjustLMargin(int yp, int, int margin, int space)
{
#ifndef QT_NO_TEXTCUSTOMITEM
    for (int idx = 0; idx < leftItems.size(); ++idx) {
        Q3TextCustomItem* item = leftItems.at(idx);
        if (item->ypos == -1)
            continue;
        if (yp >= item->ypos && yp < item->ypos + item->height)
            margin = qMax(margin, item->xpos + item->width + space);
    }
#endif
    return margin;
}

int Q3TextFlow::adjustRMargin(int yp, int, int margin, int space)
{
#ifndef QT_NO_TEXTCUSTOMITEM
    for (int idx = 0; idx < rightItems.size(); ++idx) {
        Q3TextCustomItem* item = rightItems.at(idx);
        if (item->ypos == -1)
            continue;
        if (yp >= item->ypos && yp < item->ypos + item->height)
            margin = qMax(margin, w - item->xpos - space);
    }
#endif
    return margin;
}


int Q3TextFlow::adjustFlow(int y, int /*w*/, int h)
{
    if (pagesize > 0) { // check pages
        int yinpage = y % pagesize;
        if (yinpage <= border_tolerance)
            return border_tolerance - yinpage;
        else
            if (yinpage + h > pagesize - border_tolerance)
                return (pagesize - yinpage) + border_tolerance;
    }
    return 0;
}

#ifndef QT_NO_TEXTCUSTOMITEM
void Q3TextFlow::unregisterFloatingItem(Q3TextCustomItem* item)
{
    leftItems.removeAll(item);
    rightItems.removeAll(item);
}

void Q3TextFlow::registerFloatingItem(Q3TextCustomItem* item)
{
    if (item->placement() == Q3TextCustomItem::PlaceRight) {
        if (!rightItems.contains(item))
            rightItems.append(item);
    } else if (item->placement() == Q3TextCustomItem::PlaceLeft &&
                !leftItems.contains(item)) {
        leftItems.append(item);
    }
}
#endif // QT_NO_TEXTCUSTOMITEM

QRect Q3TextFlow::boundingRect() const
{
    QRect br;
#ifndef QT_NO_TEXTCUSTOMITEM
    for (int idx = 0; idx < leftItems.size(); ++idx) {
        Q3TextCustomItem* item = leftItems.at(idx);
        br = br.united(item->geometry());
    }
    for (int idx = 0; idx < rightItems.size(); ++idx) {
        Q3TextCustomItem* item = rightItems.at(idx);
        br = br.united(item->geometry());
    }
#endif
    return br;
}


void Q3TextFlow::drawFloatingItems(QPainter* p, int cx, int cy, int cw, int ch,
                                   const QPalette &pal, bool selected)
{
#ifndef QT_NO_TEXTCUSTOMITEM
    for (int idx = 0; idx < leftItems.size(); ++idx) {
        Q3TextCustomItem* item = leftItems.at(idx);
        if (item->xpos == -1 || item->ypos == -1)
            continue;
        item->draw(p, item->xpos, item->ypos, cx, cy, cw, ch, pal, selected);
    }

    for (int idx = 0; idx < rightItems.size(); ++idx) {
        Q3TextCustomItem* item = rightItems.at(idx);
        if (item->xpos == -1 || item->ypos == -1)
            continue;
        item->draw(p, item->xpos, item->ypos, cx, cy, cw, ch, pal, selected);
    }
#endif
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifndef QT_NO_TEXTCUSTOMITEM
void Q3TextCustomItem::pageBreak(int /*y*/ , Q3TextFlow* /*flow*/)
{
}
#endif

#ifndef QT_NO_TEXTCUSTOMITEM
Q3TextTable::Q3TextTable(Q3TextDocument *p, const QMap<QString, QString> & attr )
    : Q3TextCustomItem(p)
{
    cellspacing = 2;
    cellpadding = 1;
    border = innerborder = 0;

    QMap<QString, QString>::ConstIterator it, end = attr.end();
    it = attr.find(QLatin1String("cellspacing"));
    if (it != end)
        cellspacing = (*it).toInt();
    it = attr.find(QLatin1String("cellpadding"));
    if (it != end)
        cellpadding = (*it).toInt();
    it = attr.find(QLatin1String("border"));
    if (it != end) {
        if (*it == QLatin1String("TRUE"))
            border = 1;
        else
            border = (*it).toInt();
    }
    us_b = border;

    innerborder = us_ib = border ? 1 : 0;

    if (border)
        cellspacing += 2;

    us_ib = innerborder;
    us_cs = cellspacing;
    us_cp = cellpadding;
    outerborder = cellspacing + border;
    us_ob = outerborder;
    layout = new QGridLayout(1, 1, cellspacing);

    fixwidth = 0;
    stretch = 0;
    it = attr.find(QLatin1String("width"));
    if (it != end) {
        bool b;
        QString s(*it);
        int w = s.toInt(&b);
        if (b) {
            fixwidth = w;
        } else {
            s = s.trimmed();
            if (s.length() > 1 && s[(int)s.length()-1] == QLatin1Char('%'))
                stretch = s.left(s.length()-1).toInt();
        }
    }
    us_fixwidth = fixwidth;

    place = PlaceInline;
    if (attr[QLatin1String("align")] == QLatin1String("left"))
        place = PlaceLeft;
    else if (attr[QLatin1String("align")] == QLatin1String("right"))
        place = PlaceRight;
    cachewidth = 0;
    attributes = attr;
    pageBreakFor = -1;
}

Q3TextTable::~Q3TextTable()
{
    delete layout;
}

QString Q3TextTable::richText() const
{
    QString s;
    s = QLatin1String("<table ");
    QMap<QString, QString>::ConstIterator it = attributes.begin();
    for (; it != attributes.end(); ++it)
        s += it.key() + QLatin1String("=") + *it + QLatin1String(" ");
    s += QLatin1String(">\n");

    int lastRow = -1;
    bool needEnd = false;
    for (int idx = 0; idx < cells.size(); ++idx) {
        Q3TextTableCell *cell = cells.at(idx);
        if (lastRow != cell->row()) {
            if (lastRow != -1)
                s += QLatin1String("</tr>\n");
            s += QLatin1String("<tr>");
            lastRow = cell->row();
            needEnd = true;
        }
        s += QLatin1String("<td");
        it = cell->attributes.constBegin();
        for (; it != cell->attributes.constEnd(); ++it)
            s += QLatin1String(" ") + it.key() + QLatin1String("=") + *it;
        s += QLatin1String(">");
        s += cell->richText()->richText();
        s += QLatin1String("</td>");
    }
    if (needEnd)
        s += QLatin1String("</tr>\n");
    s += QLatin1String("</table>\n");
    return s;
}

void Q3TextTable::adjustToPainter(QPainter* p)
{
    cellspacing = scale(us_cs, p);
    cellpadding = scale(us_cp, p);
    border = scale(us_b , p);
    innerborder = scale(us_ib, p);
    outerborder = scale(us_ob ,p);
    fixwidth = scale( us_fixwidth, p);
    width = 0;
    cachewidth = 0;
    for (int idx = 0; idx < cells.size(); ++idx) {
        Q3TextTableCell *cell = cells.at(idx);
        cell->adjustToPainter(p);
    }
}

void Q3TextTable::adjustCells(int y , int shift)
{
    bool enlarge = false;
    for (int idx = 0; idx < cells.size(); ++idx) {
        Q3TextTableCell *cell = cells.at(idx);
        QRect r = cell->geometry();
        if (y <= r.top()) {
            r.moveBy(0, shift);
            cell->setGeometry(r);
            enlarge = true;
        } else if (y <= r.bottom()) {
            r.rBottom() += shift;
            cell->setGeometry(r);
            enlarge = true;
        }
    }
    if (enlarge)
        height += shift;
}

void Q3TextTable::pageBreak(int  yt, Q3TextFlow* flow)
{
    if (flow->pageSize() <= 0)
        return;
    if (layout && pageBreakFor > 0 && pageBreakFor != yt) {
        layout->invalidate();
        int h = layout->heightForWidth(width-2*outerborder);
        layout->setGeometry(QRect(0, 0, width-2*outerborder, h) );
        height = layout->geometry().height()+2*outerborder;
    }
    pageBreakFor = yt;
    for (int idx = 0; idx < cells.size(); ++idx) {
        Q3TextTableCell *cell = cells.at(idx);
        int y = yt + outerborder + cell->geometry().y();
        int shift = flow->adjustFlow(y - cellspacing, width, cell->richText()->height() + 2*cellspacing);
        adjustCells(y - outerborder - yt, shift);
    }
}


void Q3TextTable::draw(QPainter* p, int x, int y, int cx, int cy, int cw, int ch,
                      const QPalette &pal, bool selected)
{
    if (placement() != PlaceInline) {
        x = xpos;
        y = ypos;
    }

    for (int idx = 0; idx < cells.size(); ++idx) {
        Q3TextTableCell *cell = cells.at(idx);
        if ((cx < 0 && cy < 0) ||
             QRect(cx, cy, cw, ch).intersects(QRect(x + outerborder + cell->geometry().x(),
                                                        y + outerborder + cell->geometry().y(),
                                                        cell->geometry().width(),
                                                        cell->geometry().height()))) {
            cell->draw(p, x+outerborder, y+outerborder, cx, cy, cw, ch, pal, selected);
            if (border) {
                QRect r(x+outerborder+cell->geometry().x() - innerborder,
                         y+outerborder+cell->geometry().y() - innerborder,
                         cell->geometry().width() + 2 * innerborder,
                         cell->geometry().height() + 2 * innerborder);
                if (is_printer(p)) {
                    QPen oldPen = p->pen();
                    QRect r2 = r;
                    r2.adjust(innerborder/2, innerborder/2, -innerborder/2, -innerborder/2);
                    p->setPen(QPen(pal.text().color(), innerborder));
                    p->drawRect(r2);
                    p->setPen(oldPen);
                } else {
                    int s =  qMax(cellspacing-2*innerborder, 0);
                    if (s) {
                        p->fillRect(r.left()-s, r.top(), s+1, r.height(), pal.button());
                        p->fillRect(r.right(), r.top(), s+1, r.height(), pal.button());
                        p->fillRect(r.left()-s, r.top()-s, r.width()+2*s, s, pal.button());
                        p->fillRect(r.left()-s, r.bottom(), r.width()+2*s, s, pal.button());
                    }
                    qDrawShadePanel(p, r, pal, true, innerborder);
                }
            }
        }
    }
    if (border) {
        QRect r (x, y, width, height);
        if (is_printer(p)) {
            QRect r2 = r;
            r2.adjust(border/2, border/2, -border/2, -border/2);
            QPen oldPen = p->pen();
            p->setPen(QPen(pal.text().color(), border));
            p->drawRect(r2);
            p->setPen(oldPen);
        } else {
            int s = border+qMax(cellspacing-2*innerborder, 0);
            if (s) {
                p->fillRect(r.left(), r.top(), s, r.height(), pal.button());
                p->fillRect(r.right()-s, r.top(), s, r.height(), pal.button());
                p->fillRect(r.left(), r.top(), r.width(), s, pal.button());
                p->fillRect(r.left(), r.bottom()-s, r.width(), s, pal.button());
            }
            qDrawShadePanel(p, r, pal, false, border);
        }
    }

}

int Q3TextTable::minimumWidth() const
{
    return qMax(fixwidth, ((layout ? layout->minimumSize().width() : 0) + 2 * outerborder));
}

void Q3TextTable::resize(int nwidth)
{
    if (fixwidth && cachewidth != 0)
        return;
    if (nwidth == cachewidth)
        return;


    cachewidth = nwidth;
    int w = nwidth;

    format(w);

    if (stretch)
        nwidth = nwidth * stretch / 100;

    width = nwidth;
    layout->invalidate();
    int shw = layout->sizeHint().width() + 2*outerborder;
    int mw = layout->minimumSize().width() + 2*outerborder;
    if (stretch)
        width = qMax(mw, nwidth);
    else
        width = qMax(mw, qMin(nwidth, shw));

    if (fixwidth)
        width = fixwidth;

    layout->invalidate();
    mw = layout->minimumSize().width() + 2*outerborder;
    width = qMax(width, mw);

    int h = layout->heightForWidth(width-2*outerborder);
    layout->setGeometry(QRect(0, 0, width-2*outerborder, h) );
    height = layout->geometry().height()+2*outerborder;
}

void Q3TextTable::format(int w)
{
    for (int i = 0; i < (int)cells.count(); ++i) {
        Q3TextTableCell *cell = cells.at(i);
        QRect r = cell->geometry();
        r.setWidth(w - 2*outerborder);
        cell->setGeometry(r);
    }
}

void Q3TextTable::addCell(Q3TextTableCell* cell)
{
    cells.append(cell);
    layout->addMultiCell(cell, cell->row(), cell->row() + cell->rowspan()-1,
                          cell->column(), cell->column() + cell->colspan()-1);
}

bool Q3TextTable::enter(Q3TextCursor *c, Q3TextDocument *&doc, Q3TextParagraph *&parag, int &idx, int &ox, int &oy, bool atEnd)
{
    currCell.remove(c);
    if (!atEnd)
        return next(c, doc, parag, idx, ox, oy);
    currCell.insert(c, cells.count());
    return prev(c, doc, parag, idx, ox, oy);
}

bool Q3TextTable::enterAt(Q3TextCursor *c, Q3TextDocument *&doc, Q3TextParagraph *&parag, int &idx, int &ox, int &oy, const QPoint &pos)
{
    currCell.remove(c);
    int lastCell = -1;
    int lastY = -1;
    int i;
    for (i = 0; i < (int)cells.count(); ++i) {
        Q3TextTableCell *cell = cells.at(i);
        if (!cell)
            continue;
        QRect r(cell->geometry().x(),
                 cell->geometry().y(),
                 cell->geometry().width() + 2 * innerborder + 2 * outerborder,
                 cell->geometry().height() + 2 * innerborder + 2 * outerborder);

        if (r.left() <= pos.x() && r.right() >= pos.x()) {
            if (cell->geometry().y() > lastY) {
                lastCell = i;
                lastY = cell->geometry().y();
            }
            if (r.top() <= pos.y() && r.bottom() >= pos.y()) {
                currCell.insert(c, i);
                break;
            }
        }
    }
    if (i == (int) cells.count())
        return false; // no cell found

    if (currCell.find(c) == currCell.end()) {
        if (lastY != -1)
            currCell.insert(c, lastCell);
        else
            return false;
    }

    Q3TextTableCell *cell = cells.at(*currCell.find(c));
    if (!cell)
        return false;
    doc = cell->richText();
    parag = doc->firstParagraph();
    idx = 0;
    ox += cell->geometry().x() + cell->horizontalAlignmentOffset() + outerborder + parent->x();
    oy += cell->geometry().y() + cell->verticalAlignmentOffset() + outerborder;
    return true;
}

bool Q3TextTable::next(Q3TextCursor *c, Q3TextDocument *&doc, Q3TextParagraph *&parag, int &idx, int &ox, int &oy)
{
    int cc = -1;
    if (currCell.find(c) != currCell.end())
        cc = *currCell.find(c);
    if (cc > (int)cells.count() - 1 || cc < 0)
        cc = -1;
    currCell.remove(c);
    currCell.insert(c, ++cc);
    if (cc >= (int)cells.count()) {
        currCell.insert(c, 0);
        Q3TextCustomItem::next(c, doc, parag, idx, ox, oy);
        Q3TextTableCell *cell = cells.first();
        if (!cell)
            return false;
        doc = cell->richText();
        idx = -1;
        return true;
    }

    if (currCell.find(c) == currCell.end())
        return false;
    Q3TextTableCell *cell = cells.at(*currCell.find(c));
    if (!cell)
        return false;
    doc = cell->richText();
    parag = doc->firstParagraph();
    idx = 0;
    ox += cell->geometry().x() + cell->horizontalAlignmentOffset() + outerborder + parent->x();
    oy += cell->geometry().y() + cell->verticalAlignmentOffset() + outerborder;
    return true;
}

bool Q3TextTable::prev(Q3TextCursor *c, Q3TextDocument *&doc, Q3TextParagraph *&parag, int &idx, int &ox, int &oy)
{
    int cc = -1;
    if (currCell.find(c) != currCell.end())
        cc = *currCell.find(c);
    if (cc > (int)cells.count() - 1 || cc < 0)
        cc = cells.count();
    currCell.remove(c);
    currCell.insert(c, --cc);
    if (cc < 0) {
        currCell.insert(c, 0);
        Q3TextCustomItem::prev(c, doc, parag, idx, ox, oy);
        Q3TextTableCell *cell = cells.first();
        if (!cell)
            return false;
        doc = cell->richText();
        idx = -1;
        return true;
    }

    if (currCell.find(c) == currCell.end())
        return false;
    Q3TextTableCell *cell = cells.at(*currCell.find(c));
    if (!cell)
        return false;
    doc = cell->richText();
    parag = doc->lastParagraph();
    idx = parag->length() - 1;
    ox += cell->geometry().x() + cell->horizontalAlignmentOffset() + outerborder + parent->x();
    oy += cell->geometry().y()  + cell->verticalAlignmentOffset() + outerborder;
    return true;
}

bool Q3TextTable::down(Q3TextCursor *c, Q3TextDocument *&doc, Q3TextParagraph *&parag, int &idx, int &ox, int &oy)
{
    if (currCell.find(c) == currCell.end())
        return false;
    Q3TextTableCell *cell = cells.at(*currCell.find(c));
    if (cell->row_ == layout->numRows() - 1) {
        currCell.insert(c, 0);
        Q3TextCustomItem::down(c, doc, parag, idx, ox, oy);
        Q3TextTableCell *cell = cells.first();
        if (!cell)
            return false;
        doc = cell->richText();
        idx = -1;
        return true;
    }

    int oldRow = cell->row_;
    int oldCol = cell->col_;
    if (currCell.find(c) == currCell.end())
        return false;
    int cc = *currCell.find(c);
    for (int i = cc; i < (int)cells.count(); ++i) {
        cell = cells.at(i);
        if (cell->row_ > oldRow && cell->col_ == oldCol) {
            currCell.insert(c, i);
            break;
        }
    }
    doc = cell->richText();
    if (!cell)
        return false;
    parag = doc->firstParagraph();
    idx = 0;
    ox += cell->geometry().x() + cell->horizontalAlignmentOffset() + outerborder + parent->x();
    oy += cell->geometry().y()  + cell->verticalAlignmentOffset() + outerborder;
    return true;
}

bool Q3TextTable::up(Q3TextCursor *c, Q3TextDocument *&doc, Q3TextParagraph *&parag, int &idx, int &ox, int &oy)
{
    if (currCell.find(c) == currCell.end())
        return false;
    Q3TextTableCell *cell = cells.at(*currCell.find(c));
    if (cell->row_ == 0) {
        currCell.insert(c, 0);
        Q3TextCustomItem::up(c, doc, parag, idx, ox, oy);
        Q3TextTableCell *cell = cells.first();
        if (!cell)
            return false;
        doc = cell->richText();
        idx = -1;
        return true;
    }

    int oldRow = cell->row_;
    int oldCol = cell->col_;
    if (currCell.find(c) == currCell.end())
        return false;
    int cc = *currCell.find(c);
    for (int i = cc; i >= 0; --i) {
        cell = cells.at(i);
        if (cell->row_ < oldRow && cell->col_ == oldCol) {
            currCell.insert(c, i);
            break;
        }
    }
    doc = cell->richText();
    if (!cell)
        return false;
    parag = doc->lastParagraph();
    idx = parag->length() - 1;
    ox += cell->geometry().x() + cell->horizontalAlignmentOffset() + outerborder + parent->x();
    oy += cell->geometry().y()  + cell->verticalAlignmentOffset() + outerborder;
    return true;
}

Q3TextTableCell::Q3TextTableCell(Q3TextTable* table,
                                int row, int column,
                                const QMap<QString, QString> &attr,
                                const Q3StyleSheetItem* style,
                                const Q3TextFormat& fmt, const QString& context,
                                Q3MimeSourceFactory &factory, Q3StyleSheet *sheet,
                                const QString& doc)
{
    cached_width = -1;
    cached_sizehint = -1;

    maxw = QWIDGETSIZE_MAX;
    minw = 0;

    parent = table;
    row_ = row;
    col_ = column;
    stretch_ = 0;
    richtext = new Q3TextDocument(table->parent);
    richtext->formatCollection()->setPaintDevice(table->parent->formatCollection()->paintDevice());
    richtext->bodyText = fmt.color();
    richtext->setTableCell(this);

    QMap<QString,QString>::ConstIterator it, end = attr.end();
    int halign = style->alignment();
    if (halign != Q3StyleSheetItem::Undefined)
        richtext->setAlignment(halign);
    it = attr.find(QLatin1String("align"));
    if (it != end && ! (*it).isEmpty()) {
        QString a = (*it).toLower();
        if (a == QLatin1String("left"))
            richtext->setAlignment(Qt::AlignLeft);
        else if (a == QLatin1String("center"))
            richtext->setAlignment(Qt::AlignHCenter);
        else if (a == QLatin1String("right"))
            richtext->setAlignment(Qt::AlignRight);
    }
    align = 0;
    it = attr.find(QLatin1String("valign"));
    if (it != end && ! (*it).isEmpty()) {
        QString va = (*it).toLower();
        if ( va == QLatin1String("top") )
	    align |= Qt::AlignTop;
	else if ( va == QLatin1String("center") || va == QLatin1String("middle") )
            align |= Qt::AlignVCenter;
        else if (va == QLatin1String("bottom"))
            align |= Qt::AlignBottom;
    }
    richtext->setFormatter(table->parent->formatter());
    richtext->setUseFormatCollection(table->parent->useFormatCollection());
    richtext->setMimeSourceFactory(&factory);
    richtext->setStyleSheet(sheet);
    richtext->setRichText(doc, context, &fmt);
    rowspan_ = 1;
    colspan_ = 1;

    it = attr.find(QLatin1String("colspan"));
    if (it != end)
        colspan_ = (*it).toInt();
    it = attr.find(QLatin1String("rowspan"));
    if (it != end)
        rowspan_ = (*it).toInt();

    background = 0;
    it = attr.find(QLatin1String("bgcolor"));
    if (it != end) {
        background = new QBrush(QColor(*it));
    }

    hasFixedWidth = false;
    it = attr.find(QLatin1String("width"));
    if (it != end) {
        bool b;
        QString s(*it);
        int w = s.toInt(&b);
        if (b) {
            maxw = w;
            minw = maxw;
            hasFixedWidth = true;
        } else {
            s = s.trimmed();
            if (s.length() > 1 && s[(int)s.length()-1] == QLatin1Char('%'))
                stretch_ = s.left(s.length()-1).toInt();
        }
    }

    attributes = attr;

    parent->addCell(this);
}

Q3TextTableCell::~Q3TextTableCell()
{
    delete background;
    background = 0;
    delete richtext;
    richtext = 0;
}

QSize Q3TextTableCell::sizeHint() const
{
    int extra = 2 * (parent->innerborder + parent->cellpadding + border_tolerance);
    int used = richtext->widthUsed() + extra;

    if  (stretch_) {
        int w = parent->width * stretch_ / 100 - 2*parent->cellspacing - 2*parent->cellpadding;
        return QSize(qMin(w, maxw), 0).expandedTo(minimumSize());
    }

    return QSize(used, 0).expandedTo(minimumSize());
}

QSize Q3TextTableCell::minimumSize() const
{
    int extra = 2 * (parent->innerborder + parent->cellpadding + border_tolerance);
    return QSize(qMax(richtext->minimumWidth() + extra, minw), 0);
}

QSize Q3TextTableCell::maximumSize() const
{
    return QSize(maxw, QWIDGETSIZE_MAX);
}

Qt::Orientations Q3TextTableCell::expandingDirections() const
{
    return Qt::Horizontal | Qt::Vertical;
}

bool Q3TextTableCell::isEmpty() const
{
    return false;
}
void Q3TextTableCell::setGeometry(const QRect& r)
{
    int extra = 2 * (parent->innerborder + parent->cellpadding);
    if (r.width() != cached_width)
        richtext->doLayout(Q3TextFormat::painter(), r.width() - extra);
    cached_width = r.width();
    geom = r;
}

QRect Q3TextTableCell::geometry() const
{
    return geom;
}

bool Q3TextTableCell::hasHeightForWidth() const
{
    return true;
}

int Q3TextTableCell::heightForWidth(int w) const
{
    int extra = 2 * (parent->innerborder + parent->cellpadding);
    w = qMax(minw, w);

    if (cached_width != w) {
        Q3TextTableCell* that = (Q3TextTableCell*) this;
        that->richtext->doLayout(Q3TextFormat::painter(), w - extra);
        that->cached_width = w;
    }
    return richtext->height() + extra;
}

void Q3TextTableCell::adjustToPainter(QPainter* p)
{
    Q3TextParagraph *parag = richtext->firstParagraph();
    while (parag) {
        parag->adjustToPainter(p);
        parag = parag->next();
    }
}

int Q3TextTableCell::horizontalAlignmentOffset() const
{
    return parent->cellpadding;
}

int Q3TextTableCell::verticalAlignmentOffset() const
{
    if ((align & Qt::AlignVCenter) == Qt::AlignVCenter)
        return (geom.height() - richtext->height()) / 2;
    else if ((align & Qt::AlignBottom) == Qt::AlignBottom)
        return geom.height() - parent->cellpadding - richtext->height()  ;
    return parent->cellpadding;
}

void Q3TextTableCell::draw(QPainter* p, int x, int y, int cx, int cy, int cw, int ch,
                           const QPalette &pal, bool)
{
    if (cached_width != geom.width()) {
        int extra = 2 * (parent->innerborder + parent->cellpadding);
        richtext->doLayout(p, geom.width() - extra);
        cached_width = geom.width();
    }
    QPalette pal2(pal);
    if (background)
        pal2.setBrush(QPalette::Base, *background);
    else if (richtext->paper())
        pal2.setBrush(QPalette::Base, *richtext->paper());

    p->save();
    p->translate(x + geom.x(), y + geom.y());
    if (background)
        p->fillRect(0, 0, geom.width(), geom.height(), *background);
    else if (richtext->paper())
        p->fillRect(0, 0, geom.width(), geom.height(), *richtext->paper());

    p->translate(horizontalAlignmentOffset(), verticalAlignmentOffset());

    QRegion r;
    if (cx >= 0 && cy >= 0)
        richtext->draw(p, cx - (x + horizontalAlignmentOffset() + geom.x()),
                        cy - (y + geom.y() + verticalAlignmentOffset()),
                        cw, ch, pal2, false, false, 0);
    else
        richtext->draw(p, -1, -1, -1, -1, pal2, false, false, 0);

    p->restore();
}
#endif

QT_END_NAMESPACE

#endif //QT_NO_RICHTEXT
