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

#ifndef Q3RICHTEXT_P_H
#define Q3RICHTEXT_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of a number of Qt sources files.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//

#include "QtGui/qapplication.h"
#include "QtGui/qcolor.h"
#include "QtCore/qhash.h"
#include "QtGui/qfont.h"
#include "QtGui/qfontmetrics.h"
#include "QtGui/qlayout.h"
#include "QtCore/qmap.h"
#include "QtCore/qvector.h"
#include "QtCore/qstack.h"
#include "QtCore/qlist.h"
#include "QtCore/qobject.h"
#include "QtGui/qpainter.h"
#include "QtGui/qpixmap.h"
#include "QtCore/qrect.h"
#include "QtCore/qsize.h"
#include "QtCore/qstring.h"
#include "QtCore/qstringlist.h"
#include "Qt3Support/q3stylesheet.h"
#include "Qt3Support/q3mimefactory.h"

QT_BEGIN_NAMESPACE

#ifndef QT_NO_RICHTEXT

class Q3TextDocument;
class Q3TextString;
class Q3TextPreProcessor;
class Q3TextFormat;
class Q3TextCursor;
class Q3TextParagraph;
class Q3TextFormatter;
class Q3TextIndent;
class Q3TextFormatCollection;
class Q3StyleSheetItem;
#ifndef QT_NO_TEXTCUSTOMITEM
class Q3TextCustomItem;
#endif
class Q3TextFlow;
struct QBidiContext;

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class Q_COMPAT_EXPORT Q3TextStringChar
{
    friend class Q3TextString;

public:
    // this is never called, initialize variables in Q3TextString::insert()!!!
    Q3TextStringChar() : nobreak(false), lineStart(0), type(Regular) {p.format=0;}
    ~Q3TextStringChar();

    struct CustomData
    {
        Q3TextFormat *format;
#ifndef QT_NO_TEXTCUSTOMITEM
        Q3TextCustomItem *custom;
#endif
        QString anchorName;
        QString anchorHref;
    };
    enum Type { Regular=0, Custom=1, Anchor=2, CustomAnchor=3 };

    QChar c;
    // this is the same struct as in qtextengine_p.h. Don't change!
    uchar softBreak      :1;     // Potential linebreak point
    uchar whiteSpace     :1;     // A unicode whitespace character, except NBSP, ZWNBSP
    uchar charStop       :1;     // Valid cursor position (for left/right arrow)
    uchar nobreak        :1;

    uchar lineStart : 1;
    uchar /*Type*/ type : 2;
    uchar bidiLevel       :7;
    uchar rightToLeft : 1;

    int x;
    union {
        Q3TextFormat* format;
        CustomData* custom;
    } p;


    int height() const;
    int ascent() const;
    int descent() const;
    bool isCustom() const { return (type & Custom) != 0; }
    Q3TextFormat *format() const;
#ifndef QT_NO_TEXTCUSTOMITEM
    Q3TextCustomItem *customItem() const;
#endif
    void setFormat(Q3TextFormat *f);
#ifndef QT_NO_TEXTCUSTOMITEM
    void setCustomItem(Q3TextCustomItem *i);
#endif

#ifndef QT_NO_TEXTCUSTOMITEM
    void loseCustomItem();
#endif


    bool isAnchor() const { return (type & Anchor) != 0; }
    bool isLink() const { return isAnchor() && p.custom->anchorHref.count(); }
    QString anchorName() const;
    QString anchorHref() const;
    void setAnchor(const QString& name, const QString& href);

    Q3TextStringChar(const Q3TextStringChar &) {
        Q_ASSERT(false);
    }
private:
    Q3TextStringChar &operator=(const Q3TextStringChar &) {
        //abort();
        return *this;
    }
    friend class Q3TextParagraph;
};

Q_DECLARE_TYPEINFO(Q3TextStringChar, Q_PRIMITIVE_TYPE);

class Q_COMPAT_EXPORT Q3TextString
{
public:

    Q3TextString();
    Q3TextString(const Q3TextString &s);
    virtual ~Q3TextString();

    static QString toString(const QVector<Q3TextStringChar> &data);
    QString toString() const;

    inline Q3TextStringChar &at(int i) const {
        return const_cast<Q3TextString *>(this)->data[i];
    }
    inline int length() const { return data.size(); }

    int width(int idx) const;

    void insert(int index, const QString &s, Q3TextFormat *f);
    void insert(int index, const QChar *unicode, int len, Q3TextFormat *f);
    void insert(int index, Q3TextStringChar *c, bool doAddRefFormat = false);
    void truncate(int index);
    void remove(int index, int len);
    void clear();

    void setFormat(int index, Q3TextFormat *f, bool useCollection);

    void setBidi(bool b) { bidi = b; }
    bool isBidi() const;
    bool isRightToLeft() const;
    QChar::Direction direction() const;
    void setDirection(QChar::Direction dr) { dir = dr; bidiDirty = true; }

    QVector<Q3TextStringChar> rawData() const { return data; }

    void operator=(const QString &s) { clear(); insert(0, s, 0); }
    void operator+=(const QString &s) { insert(length(), s, 0); }
    void prepend(const QString &s) { insert(0, s, 0); }
    int appendParagraphs( Q3TextParagraph *start, Q3TextParagraph *end );

    // return next and previous valid cursor positions.
    bool validCursorPosition(int idx);
    int nextCursorPosition(int idx);
    int previousCursorPosition(int idx);

private:
    void checkBidi() const;

    QVector<Q3TextStringChar> data;
    QString stringCache;
    uint bidiDirty : 1;
    uint bidi : 1; // true when the paragraph has right to left characters
    uint rightToLeft : 1;
    uint dir : 5;
};

inline bool Q3TextString::isBidi() const
{
    if (bidiDirty)
        checkBidi();
    return bidi;
}

inline bool Q3TextString::isRightToLeft() const
{
    if (bidiDirty)
        checkBidi();
    return rightToLeft;
}

inline QString Q3TextString::toString() const
{
    if (bidiDirty)
        checkBidi();
    return stringCache;
}

inline QChar::Direction Q3TextString::direction() const
{
    return rightToLeft ? QChar::DirR : QChar::DirL;
}

inline int Q3TextString::nextCursorPosition(int next)
{
    if (bidiDirty)
        checkBidi();

    const Q3TextStringChar *c = data.data();
    int len = length();

    if (next < len - 1) {
        next++;
        while (next < len - 1 && !c[next].charStop)
            next++;
    }
    return next;
}

inline int Q3TextString::previousCursorPosition(int prev)
{
    if (bidiDirty)
        checkBidi();

    const Q3TextStringChar *c = data.data();

    if (prev) {
        prev--;
        while (prev && !c[prev].charStop)
            prev--;
    }
    return prev;
}

inline bool Q3TextString::validCursorPosition(int idx)
{
    if (bidiDirty)
        checkBidi();

    return (at(idx).charStop);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class Q_COMPAT_EXPORT Q3TextCursor
{
public:
    Q3TextCursor(Q3TextDocument * = 0);
    Q3TextCursor(const Q3TextCursor &c);
    Q3TextCursor &operator=(const Q3TextCursor &c);
    virtual ~Q3TextCursor();

    bool operator==(const Q3TextCursor &c) const;
    bool operator!=(const Q3TextCursor &c) const { return !(*this == c); }

    inline Q3TextParagraph *paragraph() const { return para; }

    Q3TextDocument *document() const;
    int index() const;

    void gotoPosition(Q3TextParagraph* p, int index = 0);
    void setIndex(int index) { gotoPosition(paragraph(), index); }
    void setParagraph(Q3TextParagraph*p) { gotoPosition(p, 0); }

    void gotoLeft();
    void gotoRight();
    void gotoNextLetter();
    void gotoPreviousLetter();
    void gotoUp();
    void gotoDown();
    void gotoLineEnd();
    void gotoLineStart();
    void gotoHome();
    void gotoEnd();
    void gotoPageUp(int visibleHeight);
    void gotoPageDown(int visibleHeight);
    void gotoNextWord(bool onlySpace = false);
    void gotoPreviousWord(bool onlySpace = false);
    void gotoWordLeft();
    void gotoWordRight();

    void insert(const QString &s, bool checkNewLine, QVector<Q3TextStringChar> *formatting = 0);
    void splitAndInsertEmptyParagraph(bool ind = true, bool updateIds = true);
    bool remove();
    bool removePreviousChar();
    void indent();

    bool atParagStart();
    bool atParagEnd();

    int x() const; // x in current paragraph
    int y() const; // y in current paragraph

    int globalX() const;
    int globalY() const;

    Q3TextParagraph *topParagraph() const { return paras.isEmpty() ? para : paras.first(); }
    int offsetX() const { return ox; } // inner document  offset
    int offsetY() const { return oy; } // inner document offset
    int totalOffsetX() const; // total document offset
    int totalOffsetY() const; // total document offset

    bool place(const QPoint &pos, Q3TextParagraph *s) { return place(pos, s, false); }
    bool place(const QPoint &pos, Q3TextParagraph *s, bool link);
    void restoreState();


    int nestedDepth() const { return (int)indices.count(); } //### size_t/int cast
    void oneUp() { if (!indices.isEmpty()) pop(); }
    void setValid(bool b) { valid = b; }
    bool isValid() const { return valid; }

    void fixCursorPosition();
private:
    enum Operation { EnterBegin, EnterEnd, Next, Prev, Up, Down };

    void push();
    void pop();
    bool processNesting(Operation op);
    void invalidateNested();
    void gotoIntoNested(const QPoint &globalPos);

    Q3TextParagraph *para;
    int idx, tmpX;
    int ox, oy;
    QStack<int> indices;
    QStack<Q3TextParagraph*> paras;
    QStack<int> xOffsets;
    QStack<int> yOffsets;
    uint valid : 1;

};

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class Q_COMPAT_EXPORT Q3TextCommand
{
public:
    enum Commands { Invalid, Insert, Delete, Format, Style };

    Q3TextCommand(Q3TextDocument *dc) : doc(dc), cursor(dc) {}
    virtual ~Q3TextCommand();

    virtual Commands type() const;

    virtual Q3TextCursor *execute(Q3TextCursor *c) = 0;
    virtual Q3TextCursor *unexecute(Q3TextCursor *c) = 0;

protected:
    Q3TextDocument *doc;
    Q3TextCursor cursor;

};

class Q_COMPAT_EXPORT Q3TextCommandHistory
{
public:
    Q3TextCommandHistory(int s) : current(-1), steps(s) {  }
    virtual ~Q3TextCommandHistory(); // ### why is it virtual?

    void clear();

    void addCommand(Q3TextCommand *cmd);
    Q3TextCursor *undo(Q3TextCursor *c);
    Q3TextCursor *redo(Q3TextCursor *c);

    bool isUndoAvailable();
    bool isRedoAvailable();

    void setUndoDepth(int depth) { steps = depth; }
    int undoDepth() const { return steps; }

    int historySize() const { return history.count(); }
    int currentPosition() const { return current; }

private:
    QList<Q3TextCommand *> history;
    int current, steps;
};

inline Q3TextCommandHistory::~Q3TextCommandHistory()
{
    clear();
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifndef QT_NO_TEXTCUSTOMITEM
class Q_COMPAT_EXPORT Q3TextCustomItem
{
public:
    Q3TextCustomItem(Q3TextDocument *p)
        :  xpos(0), ypos(-1), width(-1), height(0), parent(p)
    {}
    virtual ~Q3TextCustomItem();
    virtual void draw(QPainter* p, int x, int y, int cx, int cy, int cw, int ch,
                      const QPalette &pal, bool selected) = 0;

    virtual void adjustToPainter(QPainter*);

    enum Placement { PlaceInline = 0, PlaceLeft, PlaceRight };
    virtual Placement placement() const;
    bool placeInline() { return placement() == PlaceInline; }

    virtual bool ownLine() const;
    virtual void resize(int nwidth);
    virtual void invalidate();
    virtual int ascent() const { return height; }

    virtual bool isNested() const;
    virtual int minimumWidth() const;

    virtual QString richText() const;

    int xpos; // used for floating items
    int ypos; // used for floating items
    int width;
    int height;

    QRect geometry() const { return QRect(xpos, ypos, width, height); }

    virtual bool enter(Q3TextCursor *, Q3TextDocument *&doc, Q3TextParagraph *&parag, int &idx, int &ox, int &oy, bool atEnd = false);
    virtual bool enterAt(Q3TextCursor *, Q3TextDocument *&doc, Q3TextParagraph *&parag, int &idx, int &ox, int &oy, const QPoint &);
    virtual bool next(Q3TextCursor *, Q3TextDocument *&doc, Q3TextParagraph *&parag, int &idx, int &ox, int &oy);
    virtual bool prev(Q3TextCursor *, Q3TextDocument *&doc, Q3TextParagraph *&parag, int &idx, int &ox, int &oy);
    virtual bool down(Q3TextCursor *, Q3TextDocument *&doc, Q3TextParagraph *&parag, int &idx, int &ox, int &oy);
    virtual bool up(Q3TextCursor *, Q3TextDocument *&doc, Q3TextParagraph *&parag, int &idx, int &ox, int &oy);

    void setParagraph(Q3TextParagraph *p) { parag = p; }
    Q3TextParagraph *paragraph() const { return parag; }

    Q3TextDocument *parent;
    Q3TextParagraph *parag;

    virtual void pageBreak(int  y, Q3TextFlow* flow);
};
#endif


#ifndef QT_NO_TEXTCUSTOMITEM
class Q_COMPAT_EXPORT Q3TextImage : public Q3TextCustomItem
{
public:
    Q3TextImage(Q3TextDocument *p, const QMap<QString, QString> &attr, const QString& context,
                Q3MimeSourceFactory &factory);
    virtual ~Q3TextImage();

    Placement placement() const { return place; }
    void adjustToPainter(QPainter*);
    int minimumWidth() const { return width; }

    QString richText() const;

    void draw(QPainter* p, int x, int y, int cx, int cy, int cw, int ch,
               const QPalette &pal, bool selected);

private:
    QRegion* reg;
    QPixmap pm;
    Placement place;
    int tmpwidth, tmpheight;
    QMap<QString, QString> attributes;
    QString imgId;

};
#endif

#ifndef QT_NO_TEXTCUSTOMITEM
class Q_COMPAT_EXPORT Q3TextHorizontalLine : public Q3TextCustomItem
{
public:
    Q3TextHorizontalLine(Q3TextDocument *p, const QMap<QString, QString> &attr, const QString& context,
                         Q3MimeSourceFactory &factory);
    virtual ~Q3TextHorizontalLine();

    void adjustToPainter(QPainter*);
    void draw(QPainter* p, int x, int y, int cx, int cy, int cw, int ch,
              const QPalette &pal, bool selected);
    QString richText() const;

    bool ownLine() const { return true; }

private:
    int tmpheight;
    QColor color;
    bool shade;

};
#endif

class Q_COMPAT_EXPORT Q3TextFlow
{
    friend class Q3TextDocument;
#ifndef QT_NO_TEXTCUSTOMITEM
    friend class Q3TextTableCell;
#endif

public:
    Q3TextFlow();
    virtual ~Q3TextFlow();

    virtual void setWidth(int width);
    int width() const;

    virtual void setPageSize(int ps);
    int pageSize() const { return pagesize; }

    virtual int adjustLMargin(int yp, int h, int margin, int space);
    virtual int adjustRMargin(int yp, int h, int margin, int space);

#ifndef QT_NO_TEXTCUSTOMITEM
    virtual void registerFloatingItem(Q3TextCustomItem* item);
    virtual void unregisterFloatingItem(Q3TextCustomItem* item);
#endif
    virtual QRect boundingRect() const;
    virtual void drawFloatingItems(QPainter* p, int cx, int cy, int cw, int ch,
                                   const QPalette &pal, bool selected);

    virtual int adjustFlow(int  y, int w, int h); // adjusts y according to the defined pagesize. Returns the shift.

    virtual bool isEmpty();

    void clear();

private:
    int w;
    int pagesize;

#ifndef QT_NO_TEXTCUSTOMITEM
    QList<Q3TextCustomItem *> leftItems;
    QList<Q3TextCustomItem *> rightItems;
#endif
};

inline int Q3TextFlow::width() const { return w; }

#ifndef QT_NO_TEXTCUSTOMITEM
class Q3TextTable;

class Q_COMPAT_EXPORT Q3TextTableCell : public QLayoutItem
{
    friend class Q3TextTable;

public:
    Q3TextTableCell(Q3TextTable* table,
                    int row, int column,
                    const QMap<QString, QString> &attr,
                    const Q3StyleSheetItem* style,
                    const Q3TextFormat& fmt, const QString& context,
                    Q3MimeSourceFactory &factory, Q3StyleSheet *sheet, const QString& doc);
    virtual ~Q3TextTableCell();

    QSize sizeHint() const ;
    QSize minimumSize() const ;
    QSize maximumSize() const ;
    Qt::Orientations expandingDirections() const;
    bool isEmpty() const;
    void setGeometry(const QRect&) ;
    QRect geometry() const;

    bool hasHeightForWidth() const;
    int heightForWidth(int) const;

    void adjustToPainter(QPainter*);

    int row() const { return row_; }
    int column() const { return col_; }
    int rowspan() const { return rowspan_; }
    int colspan() const { return colspan_; }
    int stretch() const { return stretch_; }

    Q3TextDocument* richText()  const { return richtext; }
    Q3TextTable* table() const { return parent; }

    void draw(QPainter* p, int x, int y, int cx, int cy, int cw, int ch,
               const QPalette &cg, bool selected);

    QBrush *backGround() const { return background; }
    virtual void invalidate();

    int verticalAlignmentOffset() const;
    int horizontalAlignmentOffset() const;

private:
    QRect geom;
    Q3TextTable* parent;
    Q3TextDocument* richtext;
    int row_;
    int col_;
    int rowspan_;
    int colspan_;
    int stretch_;
    int maxw;
    int minw;
    bool hasFixedWidth;
    QBrush *background;
    int cached_width;
    int cached_sizehint;
    QMap<QString, QString> attributes;
    int align;
};
#endif


#ifndef QT_NO_TEXTCUSTOMITEM
class Q_COMPAT_EXPORT Q3TextTable: public Q3TextCustomItem
{
    friend class Q3TextTableCell;

public:
    Q3TextTable(Q3TextDocument *p, const QMap<QString, QString> &attr);
    virtual ~Q3TextTable();

    void adjustToPainter(QPainter *p);
    void pageBreak(int  y, Q3TextFlow* flow);
    void draw(QPainter* p, int x, int y, int cx, int cy, int cw, int ch,
               const QPalette &pal, bool selected);

    bool noErase() const { return true; }
    bool ownLine() const { return true; }
    Placement placement() const { return place; }
    bool isNested() const { return true; }
    void resize(int nwidth);
    virtual void invalidate();

    virtual bool enter(Q3TextCursor *c, Q3TextDocument *&doc, Q3TextParagraph *&parag, int &idx, int &ox, int &oy, bool atEnd = false);
    virtual bool enterAt(Q3TextCursor *c, Q3TextDocument *&doc, Q3TextParagraph *&parag, int &idx, int &ox, int &oy, const QPoint &pos);
    virtual bool next(Q3TextCursor *c, Q3TextDocument *&doc, Q3TextParagraph *&parag, int &idx, int &ox, int &oy);
    virtual bool prev(Q3TextCursor *c, Q3TextDocument *&doc, Q3TextParagraph *&parag, int &idx, int &ox, int &oy);
    virtual bool down(Q3TextCursor *c, Q3TextDocument *&doc, Q3TextParagraph *&parag, int &idx, int &ox, int &oy);
    virtual bool up(Q3TextCursor *c, Q3TextDocument *&doc, Q3TextParagraph *&parag, int &idx, int &ox, int &oy);

    QString richText() const;

    int minimumWidth() const;

    QList<Q3TextTableCell *> tableCells() const { return cells; }

    bool isStretching() const { return stretch; }

private:
    void format(int w);
    void addCell(Q3TextTableCell* cell);

private:
    QGridLayout* layout;
    QList<Q3TextTableCell *> cells;
    int cachewidth;
    int fixwidth;
    int cellpadding;
    int cellspacing;
    int border;
    int outerborder;
    int stretch;
    int innerborder;
    int us_cp, us_ib, us_b, us_ob, us_cs;
    int us_fixwidth;
    QMap<QString, QString> attributes;
    QMap<Q3TextCursor*, int> currCell;
    Placement place;
    void adjustCells(int y , int shift);
    int pageBreakFor;
};
#endif
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifndef QT_NO_TEXTCUSTOMITEM
class Q3TextTableCell;
class Q3TextParagraph;
#endif

struct Q_COMPAT_EXPORT Q3TextDocumentSelection
{
    Q3TextCursor startCursor, endCursor;
    bool swapped;
    Q_DUMMY_COMPARISON_OPERATOR(Q3TextDocumentSelection)
};

class Q_COMPAT_EXPORT Q3TextDocument : public QObject
{
    Q_OBJECT

#ifndef QT_NO_TEXTCUSTOMITEM
    friend class Q3TextTableCell;
#endif
    friend class Q3TextCursor;
    friend class Q3TextEdit;
    friend class Q3TextParagraph;

public:
    enum SelectionIds {
        Standard = 0,
        Temp = 32000 // This selection must not be drawn, it's used e.g. by undo/redo to
        // remove multiple lines with removeSelectedText()
    };

    Q3TextDocument(Q3TextDocument *p);
    virtual ~Q3TextDocument();

    Q3TextDocument *parent() const { return par; }
    Q3TextParagraph *parentParagraph() const { return parentPar; }

    void setText(const QString &text, const QString &context);
    QMap<QString, QString> attributes() const { return attribs; }
    void setAttributes(const QMap<QString, QString> &attr) { attribs = attr; }

    QString text() const;
    QString text(int parag) const;
    QString originalText() const;

    int x() const;
    int y() const;
    int width() const;
    int widthUsed() const;
    int visibleWidth() const;
    int height() const;
    void setWidth(int w);
    int minimumWidth() const;
    bool setMinimumWidth(int needed, int used = -1, Q3TextParagraph *parag = 0);

    void setY(int y);
    int leftMargin() const;
    void setLeftMargin(int lm);
    int rightMargin() const;
    void setRightMargin(int rm);

    Q3TextParagraph *firstParagraph() const;
    Q3TextParagraph *lastParagraph() const;
    void setFirstParagraph(Q3TextParagraph *p);
    void setLastParagraph(Q3TextParagraph *p);

    void invalidate();

    void setPreProcessor(Q3TextPreProcessor *sh);
    Q3TextPreProcessor *preProcessor() const;

    void setFormatter(Q3TextFormatter *f);
    Q3TextFormatter *formatter() const;

    void setIndent(Q3TextIndent *i);
    Q3TextIndent *indent() const;

    QColor selectionColor(int id) const;
    QColor selectionTextColor(int id) const;
    bool hasSelectionTextColor(int id) const;
    void setSelectionColor(int id, const QColor &c);
    void setSelectionTextColor(int id, const QColor &b);
    bool hasSelection(int id, bool visible = false) const;
    void setSelectionStart(int id, const Q3TextCursor &cursor);
    bool setSelectionEnd(int id, const Q3TextCursor &cursor);
    void selectAll(int id);
    bool removeSelection(int id);
    void selectionStart(int id, int &paragId, int &index);
    Q3TextCursor selectionStartCursor(int id);
    Q3TextCursor selectionEndCursor(int id);
    void selectionEnd(int id, int &paragId, int &index);
    void setFormat(int id, Q3TextFormat *f, int flags);
    int numSelections() const { return nSelections; }
    void addSelection(int id);

    QString selectedText(int id, bool asRichText = false) const;
    void removeSelectedText(int id, Q3TextCursor *cursor);
    void indentSelection(int id);

    Q3TextParagraph *paragAt(int i) const;

    void addCommand(Q3TextCommand *cmd);
    Q3TextCursor *undo(Q3TextCursor *c = 0);
    Q3TextCursor *redo(Q3TextCursor *c  = 0);
    Q3TextCommandHistory *commands() const { return commandHistory; }

    Q3TextFormatCollection *formatCollection() const;

    bool find(Q3TextCursor &cursor, const QString &expr, bool cs, bool wo, bool forward);

    void setTextFormat(Qt::TextFormat f);
    Qt::TextFormat textFormat() const;

    bool inSelection(int selId, const QPoint &pos) const;

    Q3StyleSheet *styleSheet() const { return sheet_; }
#ifndef QT_NO_MIME
    Q3MimeSourceFactory *mimeSourceFactory() const { return factory_; }
#endif
    QString context() const { return contxt; }

    void setStyleSheet(Q3StyleSheet *s);
    void setDefaultFormat(const QFont &font, const QColor &color);
#ifndef QT_NO_MIME
    void setMimeSourceFactory(Q3MimeSourceFactory *f) { if (f) factory_ = f; }
#endif
    void setContext(const QString &c) { if (!c.isEmpty()) contxt = c; }

    void setUnderlineLinks(bool b);
    bool underlineLinks() const { return underlLinks; }

    void setPaper(QBrush *brush) { if (backBrush) delete backBrush; backBrush = brush; }
    QBrush *paper() const { return backBrush; }

    void doLayout(QPainter *p, int w);
    void draw(QPainter *p, const QRect& rect, const QPalette &pal, const QBrush *paper = 0);

    void drawParagraph(QPainter *p, Q3TextParagraph *parag, int cx, int cy, int cw, int ch,
                    QPixmap *&doubleBuffer, const QPalette &pal,
                    bool drawCursor, Q3TextCursor *cursor, bool resetChanged = true);
    Q3TextParagraph *draw(QPainter *p, int cx, int cy, int cw, int ch, const QPalette &pal,
                      bool onlyChanged = false, bool drawCursor = false, Q3TextCursor *cursor = 0,
                      bool resetChanged = true);

#ifndef QT_NO_TEXTCUSTOMITEM
    static Q3TextCustomItem* tag(Q3StyleSheet *sheet, const QString& name,
                                 const QMap<QString, QString> &attr,
                                 const QString& context,
                                 const Q3MimeSourceFactory& factory,
                                 bool emptyTag, Q3TextDocument *doc);
#endif

#ifndef QT_NO_TEXTCUSTOMITEM
    void registerCustomItem(Q3TextCustomItem *i, Q3TextParagraph *p);
    void unregisterCustomItem(Q3TextCustomItem *i, Q3TextParagraph *p);
#endif

    void setFlow(Q3TextFlow *f);
    void takeFlow();
    Q3TextFlow *flow() const { return flow_; }
    bool isPageBreakEnabled() const { return pages; }
    void setPageBreakEnabled(bool b) { pages = b; }

    void setUseFormatCollection(bool b) { useFC = b; }
    bool useFormatCollection() const { return useFC; }

#ifndef QT_NO_TEXTCUSTOMITEM
    Q3TextTableCell *tableCell() const { return tc; }
    void setTableCell(Q3TextTableCell *c) { tc = c; }
#endif

    void setPlainText(const QString &text);
    void setRichText(const QString &text, const QString &context, const Q3TextFormat *initialFormat = 0);
    QString richText() const;
    QString plainText() const;

    bool focusNextPrevChild(bool next);

    int alignment() const;
    void setAlignment(int a);

    int *tabArray() const;
    int tabStopWidth() const;
    void setTabArray(int *a);
    void setTabStops(int tw);

    void setUndoDepth(int depth) { commandHistory->setUndoDepth(depth); }
    int undoDepth() const { return commandHistory->undoDepth(); }

    int length() const;
    void clear(bool createEmptyParag = false);

    virtual Q3TextParagraph *createParagraph(Q3TextDocument *, Q3TextParagraph *pr = 0, Q3TextParagraph *nx = 0, bool updateIds = true);
    void insertChild(Q3TextDocument *dc) { childList.append(dc); }
    void removeChild(Q3TextDocument *dc) { childList.removeAll(dc); }
    QList<Q3TextDocument *> children() const { return childList; }

    bool hasFocusParagraph() const;
    QString focusHref() const;
    QString focusName() const;

    void invalidateOriginalText() { oTextValid = false; oText = QLatin1String(""); }

Q_SIGNALS:
    void minimumWidthChanged(int);

private:
    Q_DISABLE_COPY(Q3TextDocument)

    void init();
    QPixmap *bufferPixmap(const QSize &s);
    // HTML parser
    bool hasPrefix(const QChar* doc, int length, int pos, QChar c);
    bool hasPrefix(const QChar* doc, int length, int pos, const QString& s);
#ifndef QT_NO_TEXTCUSTOMITEM
    Q3TextCustomItem* parseTable(const QMap<QString, QString> &attr, const Q3TextFormat &fmt,
                                 const QChar* doc, int length, int& pos, Q3TextParagraph *curpar);
#endif
    bool eatSpace(const QChar* doc, int length, int& pos, bool includeNbsp = false);
    bool eat(const QChar* doc, int length, int& pos, QChar c);
    QString parseOpenTag(const QChar* doc, int length, int& pos, QMap<QString, QString> &attr, bool& emptyTag);
    QString parseCloseTag(const QChar* doc, int length, int& pos);
    QChar parseHTMLSpecialChar(const QChar* doc, int length, int& pos);
    QString parseWord(const QChar* doc, int length, int& pos, bool lower = true);
    QChar parseChar(const QChar* doc, int length, int& pos, Q3StyleSheetItem::WhiteSpaceMode wsm);
    void setRichTextInternal(const QString &text, Q3TextCursor* cursor = 0, const Q3TextFormat *initialFormat = 0);
    void setRichTextMarginsInternal(QList< QVector<Q3StyleSheetItem *> *>& styles, Q3TextParagraph* stylesPar);

    struct Q_COMPAT_EXPORT Focus {
        Q3TextParagraph *parag;
        int start, len;
        QString href;
        QString name;
    };

    int cx, cy, cw, vw;
    Q3TextParagraph *fParag, *lParag;
    Q3TextPreProcessor *pProcessor;
    struct SelectionColor {
        QColor background;
        QColor text;
    };
    QMap<int, SelectionColor> selectionColors;
    QMap<int, Q3TextDocumentSelection> selections;
    Q3TextCommandHistory *commandHistory;
    Q3TextFormatter *pFormatter;
    Q3TextIndent *indenter;
    Q3TextFormatCollection *fCollection;
    Qt::TextFormat txtFormat;
    uint preferRichText : 1;
    uint pages : 1;
    uint useFC : 1;
    uint withoutDoubleBuffer : 1;
    uint underlLinks : 1;
    uint nextDoubleBuffered : 1;
    uint oTextValid : 1;
    uint mightHaveCustomItems : 1;
    int align;
    int nSelections;
    Q3TextFlow *flow_;
    Q3TextDocument *par;
    Q3TextParagraph *parentPar;
#ifndef QT_NO_TEXTCUSTOMITEM
    Q3TextTableCell *tc;
#endif
    QBrush *backBrush;
    QPixmap *buf_pixmap;
    Focus focusIndicator;
    int minw;
    int wused;
    int leftmargin;
    int rightmargin;
    Q3TextParagraph *minwParag, *curParag;
    Q3StyleSheet* sheet_;
#ifndef QT_NO_MIME
    Q3MimeSourceFactory* factory_;
#endif
    QString contxt;
    QMap<QString, QString> attribs;
    int *tArray;
    int tStopWidth;
    int uDepth;
    QString oText;
    QList<Q3TextDocument *> childList;
    QColor linkColor, bodyText;
    double scaleFontsFactor;

    short list_tm,list_bm, list_lm, li_tm, li_bm, par_tm, par_bm;
};

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


class Q_COMPAT_EXPORT Q3TextDeleteCommand : public Q3TextCommand
{
public:
    Q3TextDeleteCommand(Q3TextDocument *dc, int i, int idx, const QVector<Q3TextStringChar> &str,
                        const QByteArray& oldStyle);
    Q3TextDeleteCommand(Q3TextParagraph *p, int idx, const QVector<Q3TextStringChar> &str);
    virtual ~Q3TextDeleteCommand();

    Commands type() const { return Delete; }
    Q3TextCursor *execute(Q3TextCursor *c);
    Q3TextCursor *unexecute(Q3TextCursor *c);

protected:
    int id, index;
    Q3TextParagraph *parag;
    QVector<Q3TextStringChar> text;
    QByteArray styleInformation;

};

class Q_COMPAT_EXPORT Q3TextInsertCommand : public Q3TextDeleteCommand
{
public:
    Q3TextInsertCommand(Q3TextDocument *dc, int i, int idx, const QVector<Q3TextStringChar> &str,
                        const QByteArray& oldStyleInfo)
        : Q3TextDeleteCommand(dc, i, idx, str, oldStyleInfo) {}
    Q3TextInsertCommand(Q3TextParagraph *p, int idx, const QVector<Q3TextStringChar> &str)
        : Q3TextDeleteCommand(p, idx, str) {}
    virtual ~Q3TextInsertCommand() {}

    Commands type() const { return Insert; }
    Q3TextCursor *execute(Q3TextCursor *c) { return Q3TextDeleteCommand::unexecute(c); }
    Q3TextCursor *unexecute(Q3TextCursor *c) { return Q3TextDeleteCommand::execute(c); }

};

class Q_COMPAT_EXPORT Q3TextFormatCommand : public Q3TextCommand
{
public:
    Q3TextFormatCommand(Q3TextDocument *dc, int sid, int sidx, int eid, int eidx, const QVector<Q3TextStringChar> &old, Q3TextFormat *f, int fl);
    virtual ~Q3TextFormatCommand();

    Commands type() const { return Format; }
    Q3TextCursor *execute(Q3TextCursor *c);
    Q3TextCursor *unexecute(Q3TextCursor *c);

protected:
    int startId, startIndex, endId, endIndex;
    Q3TextFormat *format;
    QVector<Q3TextStringChar> oldFormats;
    int flags;

};

class Q_COMPAT_EXPORT Q3TextStyleCommand : public Q3TextCommand
{
public:
    Q3TextStyleCommand(Q3TextDocument *dc, int fParag, int lParag, const QByteArray& beforeChange );
    virtual ~Q3TextStyleCommand() {}

    Commands type() const { return Style; }
    Q3TextCursor *execute(Q3TextCursor *c);
    Q3TextCursor *unexecute(Q3TextCursor *c);

    static QByteArray readStyleInformation( Q3TextDocument* dc, int fParag, int lParag);
    static void writeStyleInformation( Q3TextDocument* dc, int fParag, const QByteArray& style);

private:
    int firstParag, lastParag;
    QByteArray before;
    QByteArray after;
};

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

struct Q_COMPAT_EXPORT Q3TextParagraphSelection
{
    int start, end;
    Q_DUMMY_COMPARISON_OPERATOR(Q3TextParagraphSelection)
};

struct Q_COMPAT_EXPORT QTextLineStart
{
    QTextLineStart() : y(0), baseLine(0), h(0)
    {  }
    QTextLineStart(int y_, int bl, int h_) : y(y_), baseLine(bl), h(h_),
        w(0)
    {  }

public:
    int y, baseLine, h;
    int w;
};

class Q_COMPAT_EXPORT Q3TextParagraphData
{
public:
    Q3TextParagraphData() {}
    virtual ~Q3TextParagraphData();
    virtual void join(Q3TextParagraphData *);
};

class Q3TextParagraphPseudoDocument;

class Q3SyntaxHighlighter;

class Q_COMPAT_EXPORT Q3TextParagraph
{
    friend class Q3TextDocument;
    friend class Q3TextCursor;
    friend class Q3SyntaxHighlighter;

public:
    Q3TextParagraph(Q3TextDocument *dc, Q3TextParagraph *pr = 0, Q3TextParagraph *nx = 0, bool updateIds = true);
    ~Q3TextParagraph();

    Q3TextString *string() const;
    Q3TextStringChar *at(int i) const; // maybe remove later
    int leftGap() const;
    int length() const; // maybe remove later

    void setListStyle(Q3StyleSheetItem::ListStyle ls) { lstyle = ls; changed = true; }
    Q3StyleSheetItem::ListStyle listStyle() const { return (Q3StyleSheetItem::ListStyle)lstyle; }
    void setListItem(bool li);
    bool isListItem() const { return litem; }
    void setListValue(int v) { list_val = v; }
    int listValue() const { return list_val > 0 ? list_val : -1; }

    void setListDepth(int depth);
    int listDepth() const { return ldepth; }

//     void setFormat(Q3TextFormat *fm);
//     Q3TextFormat *paragFormat() const;

    inline Q3TextDocument *document() const {
        if (hasdoc) return (Q3TextDocument*) docOrPseudo;
        return 0;
    }
    Q3TextParagraphPseudoDocument *pseudoDocument() const;

    QRect rect() const;
    void setHeight(int h) { r.setHeight(h); }
    void show();
    void hide();
    bool isVisible() const { return visible; }

    Q3TextParagraph *prev() const;
    Q3TextParagraph *next() const;
    void setPrev(Q3TextParagraph *s);
    void setNext(Q3TextParagraph *s);

    void insert(int index, const QString &s);
    void insert(int index, const QChar *unicode, int len);
    void append(const QString &s, bool reallyAtEnd = false);
    void truncate(int index);
    void remove(int index, int len);
    void join(Q3TextParagraph *s);

    void invalidate(int chr);

    void move(int &dy);
    void format(int start = -1, bool doMove = true);

    bool isValid() const;
    bool hasChanged() const;
    void setChanged(bool b, bool recursive = false);

    int lineHeightOfChar(int i, int *bl = 0, int *y = 0) const;
    Q3TextStringChar *lineStartOfChar(int i, int *index = 0, int *line = 0) const;
    int lines() const;
    Q3TextStringChar *lineStartOfLine(int line, int *index = 0) const;
    int lineY(int l) const;
    int lineBaseLine(int l) const;
    int lineHeight(int l) const;
    void lineInfo(int l, int &y, int &h, int &bl) const;

    void setSelection(int id, int start, int end);
    void removeSelection(int id);
    int selectionStart(int id) const;
    int selectionEnd(int id) const;
    bool hasSelection(int id) const;
    bool hasAnySelection() const;
    bool fullSelected(int id) const;

    void setEndState(int s);
    int endState() const;

    void setParagId(int i);
    int paragId() const;

    bool firstPreProcess() const;
    void setFirstPreProcess(bool b);

    void indent(int *oldIndent = 0, int *newIndent = 0);

    void setExtraData(Q3TextParagraphData *data);
    Q3TextParagraphData *extraData() const;

    QMap<int, QTextLineStart*> &lineStartList();

    void setFormat(int index, int len, Q3TextFormat *f, bool useCollection = true, int flags = -1);

    void setAlignment(int a);
    int alignment() const;

    void paint(QPainter &painter, const QPalette &pal, Q3TextCursor *cursor = 0,
                bool drawSelections = false, int clipx = -1, int clipy = -1,
                int clipw = -1, int cliph = -1);

    int topMargin() const;
    int bottomMargin() const;
    int leftMargin() const;
    int firstLineMargin() const;
    int rightMargin() const;
    int lineSpacing() const;

#ifndef QT_NO_TEXTCUSTOMITEM
    void registerFloatingItem(Q3TextCustomItem *i);
    void unregisterFloatingItem(Q3TextCustomItem *i);
#endif

    void setFullWidth(bool b) { fullWidth = b; }
    bool isFullWidth() const { return fullWidth; }

#ifndef QT_NO_TEXTCUSTOMITEM
    Q3TextTableCell *tableCell() const;
#endif

    QBrush *background() const;

    int documentWidth() const;
    int documentVisibleWidth() const;
    int documentX() const;
    int documentY() const;
    Q3TextFormatCollection *formatCollection() const;
    Q3TextFormatter *formatter() const;

    int nextTab(int i, int x);
    int *tabArray() const;
    void setTabArray(int *a);
    void setTabStops(int tw);

    void adjustToPainter(QPainter *p);

    void setNewLinesAllowed(bool b);
    bool isNewLinesAllowed() const;

    QString richText() const;

    void addCommand(Q3TextCommand *cmd);
    Q3TextCursor *undo(Q3TextCursor *c = 0);
    Q3TextCursor *redo(Q3TextCursor *c  = 0);
    Q3TextCommandHistory *commands() const;
    void copyParagData(Q3TextParagraph *parag);

    void setBreakable(bool b) { breakable = b; }
    bool isBreakable() const { return breakable; }

    void setBackgroundColor(const QColor &c);
    QColor *backgroundColor() const { return bgcol; }
    void clearBackgroundColor();

    void setMovedDown(bool b) { movedDown = b; }
    bool wasMovedDown() const { return movedDown; }

    void setDirection(QChar::Direction);
    QChar::Direction direction() const;
    void setPaintDevice(QPaintDevice *pd) { paintdevice = pd; }

    void readStyleInformation(QDataStream& stream);
    void writeStyleInformation(QDataStream& stream) const;

protected:
    void setColorForSelection(QColor &c, QPainter &p, const QPalette &pal, int selection);
    void drawLabel(QPainter* p, int x, int y, int w, int h, int base, const QPalette &pal);
    void drawString(QPainter &painter, const QString &str, int start, int len, int xstart,
                             int y, int baseLine, int w, int h, bool drawSelections, int fullSelectionWidth,
                             Q3TextStringChar *formatChar, const QPalette &pal,
                             bool rightToLeft);

private:
    QMap<int, Q3TextParagraphSelection> &selections() const;
#ifndef QT_NO_TEXTCUSTOMITEM
    QList<Q3TextCustomItem *> &floatingItems() const;
#endif
    inline QBrush backgroundBrush(const QPalette &pal) {
        if (bgcol)
            return *bgcol;
        return pal.brush(QPalette::Base);
    }
    void invalidateStyleCache();

    QMap<int, QTextLineStart*> lineStarts;
    QRect r;
    Q3TextParagraph *p, *n;
    void *docOrPseudo;
    uint changed : 1;
    uint firstFormat : 1;
    uint firstPProcess : 1;
    uint needPreProcess : 1;
    uint fullWidth : 1;
    uint lastInFrame : 1;
    uint visible : 1;
    uint breakable : 1;
    uint movedDown : 1;
    uint mightHaveCustomItems : 1;
    uint hasdoc : 1;
    uint litem : 1; // whether the paragraph is a list item
    uint rtext : 1; // whether the paragraph needs rich text margin
    signed int align : 5;
    uint /*Q3StyleSheetItem::ListStyle*/ lstyle : 4;
    int invalid;
    int state, id;
    Q3TextString *str;
    QMap<int, Q3TextParagraphSelection> *mSelections;
#ifndef QT_NO_TEXTCUSTOMITEM
    QList<Q3TextCustomItem *> *mFloatingItems;
#endif
    short utm, ubm, ulm, urm, uflm, ulinespacing;
    short tabStopWidth, minwidth;
    int *tArray;
    Q3TextParagraphData *eData;
    short list_val;
    ushort ldepth;
    QColor *bgcol;
    QPaintDevice *paintdevice;
};

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class Q_COMPAT_EXPORT Q3TextFormatter
{
public:
    Q3TextFormatter();
    virtual ~Q3TextFormatter();

    virtual int format(Q3TextDocument *doc, Q3TextParagraph *parag, int start, const QMap<int, QTextLineStart*> &oldLineStarts) = 0;
    virtual int formatVertically(Q3TextDocument* doc, Q3TextParagraph* parag);

    bool isWrapEnabled(Q3TextParagraph *p) const { if (!wrapEnabled) return false; if (p && !p->isBreakable()) return false; return true;}
    int wrapAtColumn() const { return wrapColumn;}
    virtual void setWrapEnabled(bool b);
    virtual void setWrapAtColumn(int c);
    virtual void setAllowBreakInWords(bool b) { biw = b; }
    bool allowBreakInWords() const { return biw; }

    int minimumWidth() const { return thisminw; }
    int widthUsed() const { return thiswused; }

protected:
    virtual QTextLineStart *formatLine(Q3TextParagraph *parag, Q3TextString *string, QTextLineStart *line, Q3TextStringChar *start,
                                               Q3TextStringChar *last, int align = Qt::AlignAuto, int space = 0);
#ifndef QT_NO_COMPLEXTEXT
    virtual QTextLineStart *bidiReorderLine(Q3TextParagraph *parag, Q3TextString *string, QTextLineStart *line, Q3TextStringChar *start,
                                                    Q3TextStringChar *last, int align, int space);
#endif
    void insertLineStart(Q3TextParagraph *parag, int index, QTextLineStart *ls);

    int thisminw;
    int thiswused;

private:
    bool wrapEnabled;
    int wrapColumn;
    bool biw;
};

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class Q_COMPAT_EXPORT Q3TextFormatterBreakInWords : public Q3TextFormatter
{
public:
    Q3TextFormatterBreakInWords();
    virtual ~Q3TextFormatterBreakInWords() {}

    int format(Q3TextDocument *doc, Q3TextParagraph *parag, int start, const QMap<int, QTextLineStart*> &oldLineStarts);

};

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class Q_COMPAT_EXPORT Q3TextFormatterBreakWords : public Q3TextFormatter
{
public:
    Q3TextFormatterBreakWords();
    virtual ~Q3TextFormatterBreakWords() {}

    int format(Q3TextDocument *doc, Q3TextParagraph *parag, int start, const QMap<int, QTextLineStart*> &oldLineStarts);

};

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class Q_COMPAT_EXPORT Q3TextIndent
{
public:
    Q3TextIndent();
    virtual ~Q3TextIndent() {}

    virtual void indent(Q3TextDocument *doc, Q3TextParagraph *parag, int *oldIndent = 0, int *newIndent = 0) = 0;

};

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class Q_COMPAT_EXPORT Q3TextPreProcessor
{
public:
    enum Ids {
        Standard = 0
    };

    Q3TextPreProcessor();
    virtual ~Q3TextPreProcessor() {}

    virtual void process(Q3TextDocument *doc, Q3TextParagraph *, int, bool = true) = 0;
    virtual Q3TextFormat *format(int id) = 0;

};

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class Q_COMPAT_EXPORT Q3TextFormat
{
    friend class Q3TextFormatCollection;
    friend class Q3TextDocument;

public:
    enum Flags {
        NoFlags,
        Bold = 1,
        Italic = 2,
        Underline = 4,
        Family = 8,
        Size = 16,
        Color = 32,
        Misspelled = 64,
        VAlign = 128,
        StrikeOut= 256,
        Font = Bold | Italic | Underline | Family | Size | StrikeOut,
        Format = Font | Color | Misspelled | VAlign
    };

    enum VerticalAlignment { AlignNormal, AlignSuperScript, AlignSubScript };

    Q3TextFormat();
    virtual ~Q3TextFormat();

    Q3TextFormat(const Q3StyleSheetItem *s);
    Q3TextFormat(const QFont &f, const QColor &c, Q3TextFormatCollection *parent = 0);
    Q3TextFormat(const Q3TextFormat &fm);
    Q3TextFormat makeTextFormat(const Q3StyleSheetItem *style, const QMap<QString,QString>& attr, double scaleFontsFactor) const;
    Q3TextFormat& operator=(const Q3TextFormat &fm);
    QColor color() const;
    QFont font() const;
    QFontMetrics fontMetrics() const { return fm; }
    bool isMisspelled() const;
    VerticalAlignment vAlign() const;
    int minLeftBearing() const;
    int minRightBearing() const;
    int width(const QChar &c) const;
    int width(const QString &str, int pos) const;
    int height() const;
    int ascent() const;
    int descent() const;
    int leading() const;
    bool useLinkColor() const;

    void setBold(bool b);
    void setItalic(bool b);
    void setUnderline(bool b);
    void setStrikeOut(bool b);
    void setFamily(const QString &f);
    void setPointSize(int s);
    void setFont(const QFont &f);
    void setColor(const QColor &c);
    void setMisspelled(bool b);
    void setVAlign(VerticalAlignment a);

    bool operator==(const Q3TextFormat &f) const;
    Q3TextFormatCollection *parent() const;
    const QString &key() const;

    static QString getKey(const QFont &f, const QColor &c, bool misspelled, VerticalAlignment vAlign);

    void addRef();
    void removeRef();

    QString makeFormatChangeTags(Q3TextFormat* defaultFormat, Q3TextFormat *f, const QString& oldAnchorHref, const QString& anchorHref) const;
    QString makeFormatEndTags(Q3TextFormat* defaultFormat, const QString& anchorHref) const;

    static void setPainter(QPainter *p);
    static QPainter* painter();

    bool fontSizesInPixels() { return usePixelSizes; }

protected:
    virtual void generateKey();

private:
    void update();
    static void applyFont(const QFont &f);

private:
    QFont fn;
    QColor col;
    QFontMetrics fm;
    uint missp : 1;
    uint linkColor : 1;
    uint usePixelSizes : 1;
    int leftBearing, rightBearing;
    VerticalAlignment ha;
    uchar widths[256];
    int hei, asc, dsc;
    Q3TextFormatCollection *collection;
    int ref;
    QString k;
    int logicalFontSize;
    int stdSize;
    static QPainter *pntr;
    static QFontMetrics *pntr_fm;
    static int pntr_asc;
    static int pntr_hei;
    static int pntr_ldg;
    static int pntr_dsc;

};

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class Q_COMPAT_EXPORT Q3TextFormatCollection
{
    friend class Q3TextDocument;
    friend class Q3TextFormat;

public:
    Q3TextFormatCollection();
    virtual ~Q3TextFormatCollection();

    void setDefaultFormat(Q3TextFormat *f);
    Q3TextFormat *defaultFormat() const;
    virtual Q3TextFormat *format(Q3TextFormat *f);
    virtual Q3TextFormat *format(Q3TextFormat *of, Q3TextFormat *nf, int flags);
    virtual Q3TextFormat *format(const QFont &f, const QColor &c);
    virtual void remove(Q3TextFormat *f);
    virtual Q3TextFormat *createFormat(const Q3TextFormat &f) { return new Q3TextFormat(f); }
    virtual Q3TextFormat *createFormat(const QFont &f, const QColor &c) { return new Q3TextFormat(f, c, this); }

    void updateDefaultFormat(const QFont &font, const QColor &c, Q3StyleSheet *sheet);

    QPaintDevice *paintDevice() const { return paintdevice; }
    void setPaintDevice(QPaintDevice *);

private:
    void updateKeys();

private:
    Q3TextFormat *defFormat, *lastFormat, *cachedFormat;
    QHash<QString, Q3TextFormat *> cKey;
    Q3TextFormat *cres;
    QFont cfont;
    QColor ccol;
    QString kof, knf;
    int cflags;

    QPaintDevice *paintdevice;
};

class Q_COMPAT_EXPORT Q3TextParagraphPseudoDocument
{
public:
    Q3TextParagraphPseudoDocument();
    ~Q3TextParagraphPseudoDocument();
    QRect docRect;
    Q3TextFormatter *pFormatter;
    Q3TextCommandHistory *commandHistory;
    int minw;
    int wused;
    Q3TextFormatCollection collection;
};

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

inline int Q3TextParagraph::length() const
{
    return str->length();
}

inline QRect Q3TextParagraph::rect() const
{
    return r;
}

inline int Q3TextCursor::index() const
{
    return idx;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

inline int Q3TextDocument::x() const
{
    return cx;
}

inline int Q3TextDocument::y() const
{
    return cy;
}

inline int Q3TextDocument::width() const
{
    return qMax(cw, flow_->width());
}

inline int Q3TextDocument::visibleWidth() const
{
    return vw;
}

inline Q3TextParagraph *Q3TextDocument::firstParagraph() const
{
    return fParag;
}

inline Q3TextParagraph *Q3TextDocument::lastParagraph() const
{
    return lParag;
}

inline void Q3TextDocument::setFirstParagraph(Q3TextParagraph *p)
{
    fParag = p;
}

inline void Q3TextDocument::setLastParagraph(Q3TextParagraph *p)
{
    lParag = p;
}

inline void Q3TextDocument::setWidth(int w)
{
    cw = qMax(w, minw);
    flow_->setWidth(cw);
    vw = w;
}

inline int Q3TextDocument::minimumWidth() const
{
    return minw;
}

inline void Q3TextDocument::setY(int y)
{
    cy = y;
}

inline int Q3TextDocument::leftMargin() const
{
    return leftmargin;
}

inline void Q3TextDocument::setLeftMargin(int lm)
{
    leftmargin = lm;
}

inline int Q3TextDocument::rightMargin() const
{
    return rightmargin;
}

inline void Q3TextDocument::setRightMargin(int rm)
{
    rightmargin = rm;
}

inline Q3TextPreProcessor *Q3TextDocument::preProcessor() const
{
    return pProcessor;
}

inline void Q3TextDocument::setPreProcessor(Q3TextPreProcessor * sh)
{
    pProcessor = sh;
}

inline void Q3TextDocument::setFormatter(Q3TextFormatter *f)
{
    delete pFormatter;
    pFormatter = f;
}

inline Q3TextFormatter *Q3TextDocument::formatter() const
{
    return pFormatter;
}

inline void Q3TextDocument::setIndent(Q3TextIndent *i)
{
    indenter = i;
}

inline Q3TextIndent *Q3TextDocument::indent() const
{
    return indenter;
}

inline QColor Q3TextDocument::selectionColor(int id) const
{
    const Q3TextDocument *p = this;
    while (p->par)
        p = p->par;
    return p->selectionColors[id].background;
}

inline QColor Q3TextDocument::selectionTextColor(int id) const
{
    const Q3TextDocument *p = this;
    while (p->par)
        p = p->par;
    return p->selectionColors[id].text;
}

inline bool Q3TextDocument::hasSelectionTextColor(int id) const
{
    const Q3TextDocument *p = this;
    while (p->par)
        p = p->par;
    return p->selectionColors.contains(id);
}

inline void Q3TextDocument::setSelectionColor(int id, const QColor &c)
{
    Q3TextDocument *p = this;
    while (p->par)
        p = p->par;
    p->selectionColors[id].background = c;
}

inline void Q3TextDocument::setSelectionTextColor(int id, const QColor &c)
{
    Q3TextDocument *p = this;
    while (p->par)
        p = p->par;
    p->selectionColors[id].text = c;
}

inline Q3TextFormatCollection *Q3TextDocument::formatCollection() const
{
    return fCollection;
}

inline int Q3TextDocument::alignment() const
{
    return align;
}

inline void Q3TextDocument::setAlignment(int a)
{
    align = a;
}

inline int *Q3TextDocument::tabArray() const
{
    return tArray;
}

inline int Q3TextDocument::tabStopWidth() const
{
    return tStopWidth;
}

inline void Q3TextDocument::setTabArray(int *a)
{
    tArray = a;
}

inline void Q3TextDocument::setTabStops(int tw)
{
    tStopWidth = tw;
}

inline QString Q3TextDocument::originalText() const
{
    if (oTextValid)
        return oText;
    return text();
}

inline void Q3TextDocument::setFlow(Q3TextFlow *f)
{
    if (flow_)
        delete flow_;
    flow_ = f;
}

inline void Q3TextDocument::takeFlow()
{
    flow_ = 0;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

inline QColor Q3TextFormat::color() const
{
    return col;
}

inline QFont Q3TextFormat::font() const
{
    return fn;
}

inline bool Q3TextFormat::isMisspelled() const
{
    return missp;
}

inline Q3TextFormat::VerticalAlignment Q3TextFormat::vAlign() const
{
    return ha;
}

inline bool Q3TextFormat::operator==(const Q3TextFormat &f) const
{
    return k == f.k;
}

inline Q3TextFormatCollection *Q3TextFormat::parent() const
{
    return collection;
}

inline void Q3TextFormat::addRef()
{
    ref++;
}

inline void Q3TextFormat::removeRef()
{
    ref--;
    if (!collection)
        return;
    if (this == collection->defFormat)
        return;
    if (ref == 0)
        collection->remove(this);
}

inline const QString &Q3TextFormat::key() const
{
    return k;
}

inline bool Q3TextFormat::useLinkColor() const
{
    return linkColor;
}

inline Q3TextStringChar *Q3TextParagraph::at(int i) const
{
    return &str->at(i);
}

inline bool Q3TextParagraph::isValid() const
{
    return invalid == -1;
}

inline bool Q3TextParagraph::hasChanged() const
{
    return changed;
}

inline void Q3TextParagraph::setBackgroundColor(const QColor & c)
{
    delete bgcol;
    bgcol = new QColor(c);
    setChanged(true);
}

inline void Q3TextParagraph::clearBackgroundColor()
{
    delete bgcol; bgcol = 0; setChanged(true);
}

inline void Q3TextParagraph::append(const QString &s, bool reallyAtEnd)
{
    if (reallyAtEnd) {
        insert(str->length(), s);
    } else {
        int str_end = str->length() - 1;
        insert(str_end > 0 ? str_end : 0, s);
    }
}

inline Q3TextParagraph *Q3TextParagraph::prev() const
{
    return p;
}

inline Q3TextParagraph *Q3TextParagraph::next() const
{
    return n;
}

inline bool Q3TextParagraph::hasAnySelection() const
{
    return mSelections ? !selections().isEmpty() : false;
}

inline void Q3TextParagraph::setEndState(int s)
{
    if (s == state)
        return;
    state = s;
}

inline int Q3TextParagraph::endState() const
{
    return state;
}

inline void Q3TextParagraph::setParagId(int i)
{
    id = i;
}

inline int Q3TextParagraph::paragId() const
{
    if (id == -1)
        qWarning("invalid parag id!!!!!!!! (%p)", (void*)this);
    return id;
}

inline bool Q3TextParagraph::firstPreProcess() const
{
    return firstPProcess;
}

inline void Q3TextParagraph::setFirstPreProcess(bool b)
{
    firstPProcess = b;
}

inline QMap<int, QTextLineStart*> &Q3TextParagraph::lineStartList()
{
    return lineStarts;
}

inline Q3TextString *Q3TextParagraph::string() const
{
    return str;
}

inline Q3TextParagraphPseudoDocument *Q3TextParagraph::pseudoDocument() const
{
    if (hasdoc)
        return 0;
    return (Q3TextParagraphPseudoDocument*) docOrPseudo;
}


#ifndef QT_NO_TEXTCUSTOMITEM
inline Q3TextTableCell *Q3TextParagraph::tableCell() const
{
    return hasdoc ? document()->tableCell () : 0;
}
#endif

inline Q3TextCommandHistory *Q3TextParagraph::commands() const
{
    return hasdoc ? document()->commands() : pseudoDocument()->commandHistory;
}


inline int Q3TextParagraph::alignment() const
{
    return align;
}

#ifndef QT_NO_TEXTCUSTOMITEM
inline void Q3TextParagraph::registerFloatingItem(Q3TextCustomItem *i)
{
    floatingItems().append(i);
}

inline void Q3TextParagraph::unregisterFloatingItem(Q3TextCustomItem *i)
{
    floatingItems().removeAll(i);
}
#endif

inline QBrush *Q3TextParagraph::background() const
{
#ifndef QT_NO_TEXTCUSTOMITEM
    return tableCell() ? tableCell()->backGround() : 0;
#else
    return 0;
#endif
}

inline int Q3TextParagraph::documentWidth() const
{
    return hasdoc ? document()->width() : pseudoDocument()->docRect.width();
}

inline int Q3TextParagraph::documentVisibleWidth() const
{
    return hasdoc ? document()->visibleWidth() : pseudoDocument()->docRect.width();
}

inline int Q3TextParagraph::documentX() const
{
    return hasdoc ? document()->x() : pseudoDocument()->docRect.x();
}

inline int Q3TextParagraph::documentY() const
{
    return hasdoc ? document()->y() : pseudoDocument()->docRect.y();
}

inline void Q3TextParagraph::setExtraData(Q3TextParagraphData *data)
{
    eData = data;
}

inline Q3TextParagraphData *Q3TextParagraph::extraData() const
{
    return eData;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

inline void Q3TextFormatCollection::setDefaultFormat(Q3TextFormat *f)
{
    defFormat = f;
}

inline Q3TextFormat *Q3TextFormatCollection::defaultFormat() const
{
    return defFormat;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

inline Q3TextFormat *Q3TextStringChar::format() const
{
    return (type == Regular) ? p.format : p.custom->format;
}


#ifndef QT_NO_TEXTCUSTOMITEM
inline Q3TextCustomItem *Q3TextStringChar::customItem() const
{
    return isCustom() ? p.custom->custom : 0;
}
#endif

inline int Q3TextStringChar::height() const
{
#ifndef QT_NO_TEXTCUSTOMITEM
    return !isCustom() ? format()->height() : (customItem()->placement() == Q3TextCustomItem::PlaceInline ? customItem()->height : 0);
#else
    return format()->height();
#endif
}

inline int Q3TextStringChar::ascent() const
{
#ifndef QT_NO_TEXTCUSTOMITEM
    return !isCustom() ? format()->ascent() : (customItem()->placement() == Q3TextCustomItem::PlaceInline ? customItem()->ascent() : 0);
#else
    return format()->ascent();
#endif
}

inline int Q3TextStringChar::descent() const
{
#ifndef QT_NO_TEXTCUSTOMITEM
    return !isCustom() ? format()->descent() : 0;
#else
    return format()->descent();
#endif
}

#endif // QT_NO_RICHTEXT

QT_END_NAMESPACE

#endif // Q3RICHTEXT_P_H
