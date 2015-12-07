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

QT_BEGIN_NAMESPACE

Q3TextCommand::~Q3TextCommand() {}
Q3TextCommand::Commands Q3TextCommand::type() const { return Invalid; }


#ifndef QT_NO_TEXTCUSTOMITEM
Q3TextCustomItem::~Q3TextCustomItem() {}
void Q3TextCustomItem::adjustToPainter(QPainter* p){ if (p) width = 0; }
Q3TextCustomItem::Placement Q3TextCustomItem::placement() const { return PlaceInline; }

bool Q3TextCustomItem::ownLine() const { return false; }
void Q3TextCustomItem::resize(int nwidth){ width = nwidth; }
void Q3TextCustomItem::invalidate() {}

bool Q3TextCustomItem::isNested() const { return false; }
int Q3TextCustomItem::minimumWidth() const { return 0; }

QString Q3TextCustomItem::richText() const { return QString(); }

bool Q3TextCustomItem::enter(Q3TextCursor *, Q3TextDocument*&, Q3TextParagraph *&, int &, int &, int &, bool)
{
    return true;
}
bool Q3TextCustomItem::enterAt(Q3TextCursor *, Q3TextDocument *&, Q3TextParagraph *&, int &, int &, int &, const QPoint &)
{
    return true;
}
bool Q3TextCustomItem::next(Q3TextCursor *, Q3TextDocument *&, Q3TextParagraph *&, int &, int &, int &)
{
    return true;
}
bool Q3TextCustomItem::prev(Q3TextCursor *, Q3TextDocument *&, Q3TextParagraph *&, int &, int &, int &)
{
    return true;
}
bool Q3TextCustomItem::down(Q3TextCursor *, Q3TextDocument *&, Q3TextParagraph *&, int &, int &, int &)
{
    return true;
}
bool Q3TextCustomItem::up(Q3TextCursor *, Q3TextDocument *&, Q3TextParagraph *&, int &, int &, int &)
{
    return true;
}
#endif // QT_NO_TEXTCUSTOMITEM

void Q3TextFlow::setPageSize(int ps) { pagesize = ps; }
#ifndef QT_NO_TEXTCUSTOMITEM
bool Q3TextFlow::isEmpty() { return leftItems.isEmpty() && rightItems.isEmpty(); }
#else
bool Q3TextFlow::isEmpty() { return true; }
#endif

#ifndef QT_NO_TEXTCUSTOMITEM
void Q3TextTableCell::invalidate() { cached_width = -1; cached_sizehint = -1; }

void Q3TextTable::invalidate() { cachewidth = -1; }
#endif

Q3TextParagraphData::~Q3TextParagraphData() {}
void Q3TextParagraphData::join(Q3TextParagraphData *) {}

Q3TextFormatter::~Q3TextFormatter() {}
void Q3TextFormatter::setWrapEnabled(bool b) { wrapEnabled = b; }
void Q3TextFormatter::setWrapAtColumn(int c) { wrapColumn = c; }



int Q3TextCursor::x() const
{
    if (idx >= para->length())
        return 0;
    Q3TextStringChar *c = para->at(idx);
    int curx = c->x;
    if (!c->rightToLeft &&
         c->c.isSpace() &&
         idx > 0 &&
         para->at(idx - 1)->c != QLatin1Char('\t') &&
         !c->lineStart &&
         (para->alignment() & Qt::AlignJustify) == Qt::AlignJustify)
        curx = para->at(idx - 1)->x + para->string()->width(idx - 1);
    if (c->rightToLeft)
        curx += para->string()->width(idx);
    return curx;
}

int Q3TextCursor::y() const
{
    int dummy, line;
    para->lineStartOfChar(idx, &dummy, &line);
    return para->lineY(line);
}

int Q3TextCursor::globalX() const { return totalOffsetX() + para->rect().x() + x(); }
int Q3TextCursor::globalY() const { return totalOffsetY() + para->rect().y() + y(); }

Q3TextDocument *Q3TextCursor::document() const
{
    return para ? para->document() : 0;
}

void Q3TextCursor::gotoPosition(Q3TextParagraph* p, int index)
{
    if (para && p != para) {
        while (!indices.isEmpty() && para->document() != p->document())
            pop();
        Q_ASSERT(indices.isEmpty() || para->document() == p->document());
    }
    para = p;
    if (index < 0 || index >= para->length()) {
        qWarning("Q3TextCursor::gotoParagraph Index: %d out of range", index);
        if (index < 0 || para->length() == 0)
            index = 0;
        else
            index = para->length() - 1;
    }

    tmpX = -1;
    idx = index;
    fixCursorPosition();
}

bool Q3TextDocument::hasSelection(int id, bool visible) const
{
    return (selections.find(id) != selections.end() &&
             (!visible ||
               ((Q3TextDocument*)this)->selectionStartCursor(id) !=
               ((Q3TextDocument*)this)->selectionEndCursor(id)));
}

void Q3TextDocument::setSelectionStart(int id, const Q3TextCursor &cursor)
{
    Q3TextDocumentSelection sel;
    sel.startCursor = cursor;
    sel.endCursor = cursor;
    sel.swapped = false;
    selections[id] = sel;
}

Q3TextParagraph *Q3TextDocument::paragAt(int i) const
{
    Q3TextParagraph* p = curParag;
    if (!p || p->paragId() > i)
        p = fParag;
    while (p && p->paragId() != i)
        p = p->next();
    ((Q3TextDocument*)this)->curParag = p;
    return p;
}


Q3TextFormat::~Q3TextFormat()
{
}

Q3TextFormat::Q3TextFormat()
    : fm(QFontMetrics(fn)), linkColor(true), logicalFontSize(3), stdSize(qApp->font().pointSize())
{
    ref = 0;

    usePixelSizes = false;
    if (stdSize == -1) {
        stdSize = qApp->font().pixelSize();
        usePixelSizes = true;
    }

    missp = false;
    ha = AlignNormal;
    collection = 0;
}

Q3TextFormat::Q3TextFormat(const Q3StyleSheetItem *style)
    : fm(QFontMetrics(fn)), linkColor(true), logicalFontSize(3), stdSize(qApp->font().pointSize())
{
    ref = 0;

    usePixelSizes = false;
    if (stdSize == -1) {
        stdSize = qApp->font().pixelSize();
        usePixelSizes = true;
    }

    missp = false;
    ha = AlignNormal;
    collection = 0;
    fn = QFont(style->fontFamily(),
                style->fontSize(),
                style->fontWeight(),
                style->fontItalic());
    fn.setUnderline(style->fontUnderline());
    fn.setStrikeOut(style->fontStrikeOut());
    col = style->color();
    fm = QFontMetrics(fn);
    leftBearing = fm.minLeftBearing();
    rightBearing = fm.minRightBearing();
    hei = fm.lineSpacing();
    asc = fm.ascent() + (fm.leading()+1)/2;
    dsc = fm.descent();
    missp = false;
    ha = AlignNormal;
    memset(widths, 0, 256);
    generateKey();
    addRef();
}

Q3TextFormat::Q3TextFormat(const QFont &f, const QColor &c, Q3TextFormatCollection *parent)
    : fn(f), col(c), fm(QFontMetrics(f)), linkColor(true),
      logicalFontSize(3), stdSize(f.pointSize())
{
    ref = 0;
    usePixelSizes = false;
    if (stdSize == -1) {
        stdSize = f.pixelSize();
        usePixelSizes = true;
    }
    collection = parent;
    leftBearing = fm.minLeftBearing();
    rightBearing = fm.minRightBearing();
    hei = fm.lineSpacing();
    asc = fm.ascent() + (fm.leading()+1)/2;
    dsc = fm.descent();
    missp = false;
    ha = AlignNormal;
    memset(widths, 0, 256);
    generateKey();
    addRef();
}

Q3TextFormat::Q3TextFormat(const Q3TextFormat &f)
    : fm(f.fm)
{
    ref = 0;
    collection = 0;
    fn = f.fn;
    col = f.col;
    leftBearing = f.leftBearing;
    rightBearing = f.rightBearing;
    memset(widths, 0, 256);
    hei = f.hei;
    asc = f.asc;
    dsc = f.dsc;
    stdSize = f.stdSize;
    usePixelSizes = f.usePixelSizes;
    logicalFontSize = f.logicalFontSize;
    missp = f.missp;
    ha = f.ha;
    k = f.k;
    linkColor = f.linkColor;
    addRef();
}

Q3TextFormat& Q3TextFormat::operator=(const Q3TextFormat &f)
{
    ref = 0;
    collection = f.collection;
    fn = f.fn;
    col = f.col;
    fm = f.fm;
    leftBearing = f.leftBearing;
    rightBearing = f.rightBearing;
    memset(widths, 0, 256);
    hei = f.hei;
    asc = f.asc;
    dsc = f.dsc;
    stdSize = f.stdSize;
    usePixelSizes = f.usePixelSizes;
    logicalFontSize = f.logicalFontSize;
    missp = f.missp;
    ha = f.ha;
    k = f.k;
    linkColor = f.linkColor;
    addRef();
    return *this;
}

void Q3TextFormat::update()
{
    fm = QFontMetrics(fn);
    leftBearing = fm.minLeftBearing();
    rightBearing = fm.minRightBearing();
    hei = fm.lineSpacing();
    asc = fm.ascent() + (fm.leading()+1)/2;
    dsc = fm.descent();
    memset(widths, 0, 256);
    generateKey();
}


QPainter* Q3TextFormat::pntr = 0;
QFontMetrics* Q3TextFormat::pntr_fm = 0;
int Q3TextFormat::pntr_ldg=-1;
int Q3TextFormat::pntr_asc=-1;
int Q3TextFormat::pntr_hei=-1;
int Q3TextFormat::pntr_dsc=-1;

void Q3TextFormat::setPainter(QPainter *p)
{
    pntr = p;
}

QPainter*  Q3TextFormat::painter()
{
    return pntr;
}

void Q3TextFormat::applyFont(const QFont &f)
{
    QFontMetrics fm(pntr->fontMetrics());
    if (!pntr_fm || pntr->font() != f) {
        pntr->setFont(f);
        delete pntr_fm;
        pntr_fm = new QFontMetrics(pntr->fontMetrics());
        pntr_ldg = pntr_fm->leading();
        pntr_asc = pntr_fm->ascent()+(pntr_ldg+1)/2;
        pntr_hei = pntr_fm->lineSpacing();
        pntr_dsc = -1;
    }
}

int Q3TextFormat::minLeftBearing() const
{
    if (!pntr || !pntr->isActive())
        return leftBearing;
    applyFont(fn);
    return pntr_fm->minLeftBearing();
}

int Q3TextFormat::minRightBearing() const
{
    if (!pntr || !pntr->isActive())
        return rightBearing;
    applyFont(fn);
    return pntr_fm->minRightBearing();
}

int Q3TextFormat::height() const
{
    if (!pntr || !pntr->isActive())
        return hei;
    applyFont(fn);
    return pntr_hei;
}

int Q3TextFormat::ascent() const
{
    if (!pntr || !pntr->isActive())
        return asc;
    applyFont(fn);
    return pntr_asc;
}

int Q3TextFormat::descent() const
{
    if (!pntr || !pntr->isActive())
        return dsc;
    applyFont(fn);
    if (pntr_dsc < 0)
        pntr_dsc = pntr_fm->descent();
    return pntr_dsc;
}

int Q3TextFormat::leading() const
{
    if (!pntr || !pntr->isActive())
        return fm.leading();
    applyFont(fn);
    return pntr_ldg;
}

void Q3TextFormat::generateKey()
{
    k = getKey(fn, col, isMisspelled(), vAlign());
}

QString Q3TextFormat::getKey(const QFont &fn, const QColor &col, bool misspelled, VerticalAlignment a)
{
    QString k = fn.key();
    k += QLatin1Char('/');
    k += QString::number((uint)col.rgb());
    k += QLatin1Char('/');
    k += QString::number((int)misspelled);
    k += QLatin1Char('/');
    k += QString::number((int)a);
    return k;
}

QString Q3TextString::toString(const QVector<Q3TextStringChar> &data)
{
    QString s;
    int l = data.size();
    s.setUnicode(0, l);
    const Q3TextStringChar *c = data.data();
    QChar *uc = (QChar *)s.unicode();
    while (l--)
        *(uc++) = (c++)->c;

    return s;
}

void Q3TextParagraph::setSelection(int id, int start, int end)
{
    QMap<int, Q3TextParagraphSelection>::ConstIterator it = selections().constFind(id);
    if (it != mSelections->constEnd()) {
        if (start == (*it).start && end == (*it).end)
            return;
    }

    Q3TextParagraphSelection sel;
    sel.start = start;
    sel.end = end;
    (*mSelections)[id] = sel;
    setChanged(true, true);
}

void Q3TextParagraph::removeSelection(int id)
{
    if (!hasSelection(id))
        return;
    if (mSelections)
        mSelections->remove(id);
    setChanged(true, true);
}

int Q3TextParagraph::selectionStart(int id) const
{
    if (!mSelections)
        return -1;
    QMap<int, Q3TextParagraphSelection>::ConstIterator it = mSelections->constFind(id);
    if (it == mSelections->constEnd())
        return -1;
    return (*it).start;
}

int Q3TextParagraph::selectionEnd(int id) const
{
    if (!mSelections)
        return -1;
    QMap<int, Q3TextParagraphSelection>::ConstIterator it = mSelections->constFind(id);
    if (it == mSelections->constEnd())
        return -1;
    return (*it).end;
}

bool Q3TextParagraph::hasSelection(int id) const
{
    return mSelections ? mSelections->contains(id) : false;
}

bool Q3TextParagraph::fullSelected(int id) const
{
    if (!mSelections)
        return false;
    QMap<int, Q3TextParagraphSelection>::ConstIterator it = mSelections->constFind(id);
    if (it == mSelections->constEnd())
        return false;
    return (*it).start == 0 && (*it).end == str->length() - 1;
}

int Q3TextParagraph::lineY(int l) const
{
    if (l > (int)lineStarts.count() - 1) {
        qWarning("Q3TextParagraph::lineY: line %d out of range!", l);
        return 0;
    }

    if (!isValid())
        ((Q3TextParagraph*)this)->format();

    QMap<int, QTextLineStart*>::ConstIterator it = lineStarts.begin();
    while (l-- > 0)
        ++it;
    return (*it)->y;
}

int Q3TextParagraph::lineBaseLine(int l) const
{
    if (l > (int)lineStarts.count() - 1) {
        qWarning("Q3TextParagraph::lineBaseLine: line %d out of range!", l);
        return 10;
    }

    if (!isValid())
        ((Q3TextParagraph*)this)->format();

    QMap<int, QTextLineStart*>::ConstIterator it = lineStarts.begin();
    while (l-- > 0)
        ++it;
    return (*it)->baseLine;
}

int Q3TextParagraph::lineHeight(int l) const
{
    if (l > (int)lineStarts.count() - 1) {
        qWarning("Q3TextParagraph::lineHeight: line %d out of range!", l);
        return 15;
    }

    if (!isValid())
        ((Q3TextParagraph*)this)->format();

    QMap<int, QTextLineStart*>::ConstIterator it = lineStarts.begin();
    while (l-- > 0)
        ++it;
    return (*it)->h;
}

void Q3TextParagraph::lineInfo(int l, int &y, int &h, int &bl) const
{
    if (l > (int)lineStarts.count() - 1) {
        qWarning("Q3TextParagraph::lineInfo: line %d out of range!", l);
        qDebug("%d %d", (int)lineStarts.count() - 1, l);
        y = 0;
        h = 15;
        bl = 10;
        return;
    }

    if (!isValid())
        ((Q3TextParagraph*)this)->format();

    QMap<int, QTextLineStart*>::ConstIterator it = lineStarts.begin();
    while (l-- > 0)
        ++it;
    y = (*it)->y;
    h = (*it)->h;
    bl = (*it)->baseLine;
}


void Q3TextParagraph::setAlignment(int a)
{
    if (a == (int)align)
        return;
    align = a;
    invalidate(0);
}

Q3TextFormatter *Q3TextParagraph::formatter() const
{
    if (hasdoc)
        return document()->formatter();
    if (pseudoDocument()->pFormatter)
        return pseudoDocument()->pFormatter;
    return (((Q3TextParagraph*)this)->pseudoDocument()->pFormatter = new Q3TextFormatterBreakWords);
}

void Q3TextParagraph::setTabArray(int *a)
{
    delete [] tArray;
    tArray = a;
}

void Q3TextParagraph::setTabStops(int tw)
{
    if (hasdoc)
        document()->setTabStops(tw);
    else
        tabStopWidth = tw;
}

QMap<int, Q3TextParagraphSelection> &Q3TextParagraph::selections() const
{
    if (!mSelections)
        ((Q3TextParagraph *)this)->mSelections = new QMap<int, Q3TextParagraphSelection>;
    return *mSelections;
}

#ifndef QT_NO_TEXTCUSTOMITEM
QList<Q3TextCustomItem *> &Q3TextParagraph::floatingItems() const
{
    if (!mFloatingItems)
        ((Q3TextParagraph *)this)->mFloatingItems = new QList<Q3TextCustomItem *>;
    return *mFloatingItems;
}
#endif

Q3TextStringChar::~Q3TextStringChar()
{
    if (format())
        format()->removeRef();
    if (type) // not Regular
        delete p.custom;
}

Q3TextParagraphPseudoDocument::Q3TextParagraphPseudoDocument():pFormatter(0),commandHistory(0), minw(0),wused(0),collection(){}
Q3TextParagraphPseudoDocument::~Q3TextParagraphPseudoDocument(){ delete pFormatter; delete commandHistory; }


QT_END_NAMESPACE

#endif //QT_NO_RICHTEXT
