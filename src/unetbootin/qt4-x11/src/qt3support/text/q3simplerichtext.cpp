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

#include "q3simplerichtext.h"

#ifndef QT_NO_RICHTEXT
#include "q3richtext_p.h"
#include "qapplication.h"

QT_BEGIN_NAMESPACE

class Q3SimpleRichTextData
{
public:
    Q3TextDocument *doc;
    QFont font;
    int cachedWidth;
    bool cachedWidthWithPainter;
    void adjustSize();
};

// Pull this private function in from qglobal.cpp
Q_CORE_EXPORT unsigned int qt_int_sqrt(unsigned int n);

void Q3SimpleRichTextData::adjustSize() {
    QFontMetrics fm(font);
    int mw =  fm.width(QString(QLatin1Char('x'))) * 80;
    int w = mw;
    doc->doLayout(0,w);
    if (doc->widthUsed() != 0) {
        w = qt_int_sqrt(5 * doc->height() * doc->widthUsed() / 3);
        doc->doLayout(0, qMin(w, mw));

        if (w*3 < 5*doc->height()) {
            w = qt_int_sqrt(2 * doc->height() * doc->widthUsed());
            doc->doLayout(0,qMin(w, mw));
        }
    }
    cachedWidth = doc->width();
    cachedWidthWithPainter = false;
}

/*!
    \class Q3SimpleRichText qsimplerichtext.h
    \brief The Q3SimpleRichText class provides a small displayable piece of rich text.

    \compat

    This class encapsulates simple rich text usage in which a string
    is interpreted as rich text and can be drawn. This is particularly
    useful if you want to display some rich text in a custom widget. A
    Q3StyleSheet is needed to interpret the tags and format the rich
    text. Qt provides a default HTML-like style sheet, but you may
    define custom style sheets.

    Once created, the rich text object can be queried for its width(),
    height(), and the actual width used (see widthUsed()). Most
    importantly, it can be drawn on any given QPainter with draw().
    Q3SimpleRichText can also be used to implement hypertext or active
    text facilities by using anchorAt(). A hit test through inText()
    makes it possible to use simple rich text for text objects in
    editable drawing canvases.

    Once constructed from a string the contents cannot be changed,
    only resized. If the contents change, just throw the rich text
    object away and make a new one with the new contents.

    For large documents use QTextEdit or QTextBrowser. For very small
    items of rich text you can use a QLabel.

    If you are using Q3SimpleRichText to print in high resolution you
    should call setWidth(QPainter, int) so that the content will be
    laid out properly on the page.
*/

/*!
    Constructs a Q3SimpleRichText from the rich text string \a text and
    the font \a fnt.

    The font is used as a basis for the text rendering. When using
    rich text rendering on a widget \e w, you would normally specify
    the widget's font, for example:

    \snippet doc/src/snippets/code/src_qt3support_text_q3simplerichtext.cpp 0

    \a context is the optional context of the rich text object. This
    becomes important if \a text contains relative references, for
    example within image tags. Q3SimpleRichText always uses the default
    mime source factory (see \l{Q3MimeSourceFactory::defaultFactory()})
    to resolve those references. The context will then be used to
    calculate the absolute path. See
    Q3MimeSourceFactory::makeAbsolute() for details.

    The \a sheet is an optional style sheet. If it is 0, the default
    style sheet will be used (see \l{Q3StyleSheet::defaultSheet()}).
*/

Q3SimpleRichText::Q3SimpleRichText(const QString& text, const QFont& fnt,
                                  const QString& context, const Q3StyleSheet* sheet)
{
    d = new Q3SimpleRichTextData;
    d->cachedWidth = -1;
    d->cachedWidthWithPainter = false;
    d->font = fnt;
    d->doc = new Q3TextDocument(0);
    d->doc->setTextFormat(Qt::RichText);
    d->doc->setLeftMargin(0);
    d->doc->setRightMargin(0);
    d->doc->setFormatter(new Q3TextFormatterBreakWords);
    d->doc->setStyleSheet((Q3StyleSheet*)sheet);
    d->doc->setDefaultFormat(fnt, QColor());
    d->doc->setText(text, context);
}


/*!
    Constructs a Q3SimpleRichText from the rich text string \a text and
    the font \a fnt.

    This is a slightly more complex constructor for Q3SimpleRichText
    that takes an additional mime source factory \a factory, a page
    break parameter \a pageBreak and a bool \a linkUnderline. \a
    linkColor is only provided for compatibility, but has no effect,
    as QPalette::link() color is used now.

    \a context is the optional context of the rich text object. This
    becomes important if \a text contains relative references, for
    example within image tags. Q3SimpleRichText always uses the default
    mime source factory (see \l{Q3MimeSourceFactory::defaultFactory()})
    to resolve those references. The context will then be used to
    calculate the absolute path. See
    Q3MimeSourceFactory::makeAbsolute() for details.

    The \a sheet is an optional style sheet. If it is 0, the default
    style sheet will be used (see \l{Q3StyleSheet::defaultSheet()}).

    This constructor is useful for creating a Q3SimpleRichText object
    suitable for printing. Set \a pageBreak to be the height of the
    contents area of the pages.
*/

Q3SimpleRichText::Q3SimpleRichText(const QString& text, const QFont& fnt,
                                  const QString& context, const Q3StyleSheet* sheet,
                                  const Q3MimeSourceFactory* factory, int pageBreak,
                                  const QColor& /*linkColor*/, bool linkUnderline)
{
    d = new Q3SimpleRichTextData;
    d->cachedWidth = -1;
    d->cachedWidthWithPainter = false;
    d->font = fnt;
    d->doc = new Q3TextDocument(0);
    d->doc->setTextFormat(Qt::RichText);
    d->doc->setFormatter(new Q3TextFormatterBreakWords);
    d->doc->setStyleSheet((Q3StyleSheet*)sheet);
    d->doc->setDefaultFormat(fnt, QColor());
    d->doc->flow()->setPageSize(pageBreak);
    d->doc->setPageBreakEnabled(true);
#ifndef QT_NO_MIME
    d->doc->setMimeSourceFactory((Q3MimeSourceFactory*)factory);
#endif
    d->doc->setUnderlineLinks(linkUnderline);
    d->doc->setText(text, context);
}

/*!
    Destroys the rich text object, freeing memory.
*/

Q3SimpleRichText::~Q3SimpleRichText()
{
    delete d->doc;
    delete d;
}

/*!
    \overload

    Sets the width of the rich text object to \a w pixels.

    \sa height(), adjustSize()
*/

void Q3SimpleRichText::setWidth(int w)
{
    if (w == d->cachedWidth && !d->cachedWidthWithPainter)
        return;
    d->doc->formatter()->setAllowBreakInWords(d->doc->isPageBreakEnabled());
    d->cachedWidth = w;
    d->cachedWidthWithPainter = false;
    d->doc->doLayout(0, w);
}

/*!
    Sets the width of the rich text object to \a w pixels,
    recalculating the layout as if it were to be drawn with painter \a
    p.

    Passing a painter is useful when you intend drawing on devices
    other than the screen, for example a QPrinter.

    \sa height(), adjustSize()
*/

void Q3SimpleRichText::setWidth(QPainter *p, int w)
{
    if (w == d->cachedWidth  && d->cachedWidthWithPainter)
        return;
    d->doc->formatter()->setAllowBreakInWords(d->doc->isPageBreakEnabled() ||
                       (p && p->device() &&
                     p->device()->devType() == QInternal::Printer));
    p->save();
    d->cachedWidth = w;
    d->cachedWidthWithPainter = true;
    d->doc->doLayout(p, w);
    p->restore();
}

/*!
    Returns the set width of the rich text object in pixels.

    \sa widthUsed()
*/

int Q3SimpleRichText::width() const
{
    if (d->cachedWidth < 0)
        d->adjustSize();
    return d->doc->width();
}

/*!
    Returns the width in pixels that is actually used by the rich text
    object. This can be smaller or wider than the set width.

    It may be wider, for example, if the text contains images or
    non-breakable words that are already wider than the available
    space. It's smaller when the object only consists of lines that do
    not fill the width completely.

    \sa width()
*/

int Q3SimpleRichText::widthUsed() const
{
    if (d->cachedWidth < 0)
        d->adjustSize();
    return d->doc->widthUsed();
}

/*!
    Returns the height of the rich text object in pixels.

    \sa setWidth()
*/

int Q3SimpleRichText::height() const
{
    if (d->cachedWidth < 0)
        d->adjustSize();
    return d->doc->height();
}

/*!
    Adjusts the rich text object to a reasonable size.

    \sa setWidth()
*/

void Q3SimpleRichText::adjustSize()
{
    d->adjustSize();
}

/*!
    Draws the formatted text with painter \a p, at position (\a x, \a
    y), clipped to \a clipRect. The clipping rectangle is given in the
    rich text object's coordinates translated by (\a x, \a y). Passing
    an null rectangle results in no clipping. Colors from the color
    group \a cg are used as needed, and if not 0, *\a{paper} is
    used as the background brush.

    Note that the display code is highly optimized to reduce flicker,
    so passing a brush for \a paper is preferable to simply clearing
    the area to be painted and then calling this without a brush.
*/

void Q3SimpleRichText::draw(QPainter *p, int x, int y, const QRect& clipRect,
                            const QColorGroup &cg, const QBrush* paper) const
{
    p->save();
    if (d->cachedWidth < 0)
        d->adjustSize();
    QRect r = clipRect;
    if (!r.isNull())
        r.moveBy(-x, -y);

    if (paper)
        d->doc->setPaper(new QBrush(*paper));
    QPalette pal2 = cg;
    if (d->doc->paper())
        pal2.setBrush(QPalette::Base, *d->doc->paper());

    if (!clipRect.isNull())
        p->setClipRect(clipRect);
    p->translate(x, y);
    d->doc->draw(p, r, pal2, paper);
    p->translate(-x, -y);
    p->restore();
}


/*! \fn void Q3SimpleRichText::draw(QPainter *p, int x, int y, const QRegion& clipRegion,
  const QColorGroup &cg, const QBrush* paper) const

  Use the version with clipRect instead of this \a clipRegion version,
  since this region version has problems with larger documents on some
  platforms (on X11 regions internally are represented with 16-bit
  coordinates).
*/



/*!
    Returns the context of the rich text object. If no context has
    been specified in the constructor, an empty string is returned. The
    context is the path to use to look up relative links, such as
    image tags and anchor references.
*/

QString Q3SimpleRichText::context() const
{
    return d->doc->context();
}

/*!
    Returns the anchor at the requested position, \a pos. An empty
    string is returned if no anchor is specified for this position.
*/

QString Q3SimpleRichText::anchorAt(const QPoint& pos) const
{
    if (d->cachedWidth < 0)
        d->adjustSize();
    Q3TextCursor c(d->doc);
    c.place(pos, d->doc->firstParagraph(), true);
    return c.paragraph()->at(c.index())->anchorHref();
}

/*!
    Returns true if \a pos is within a text line of the rich text
    object; otherwise returns false.
*/

bool Q3SimpleRichText::inText(const QPoint& pos) const
{
    if (d->cachedWidth < 0)
        d->adjustSize();
    if (pos.y()  > d->doc->height())
        return false;
    Q3TextCursor c(d->doc);
    c.place(pos, d->doc->firstParagraph());
    return c.totalOffsetX() + c.paragraph()->at(c.index())->x +
        c.paragraph()->at(c.index())->format()->width(c.paragraph()->at(c.index())->c) > pos.x();
}

/*!
    Sets the default font for the rich text object to \a f
*/

void Q3SimpleRichText::setDefaultFont(const QFont &f)
{
    if (d->font == f)
        return;
    d->font = f;
    d->cachedWidth = -1;
    d->cachedWidthWithPainter = false;
    d->doc->setDefaultFormat(f, QColor());
    d->doc->setText(d->doc->originalText(), d->doc->context());
}

QT_END_NAMESPACE

#endif //QT_NO_RICHTEXT
