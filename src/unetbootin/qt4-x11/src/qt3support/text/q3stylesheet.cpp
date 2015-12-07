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

#include "q3stylesheet.h"

#ifndef QT_NO_RICHTEXT

#include "qlayout.h"
#include "qpainter.h"
#include "q3cleanuphandler.h"
#include <qtextdocument.h>

#include <stdio.h>

QT_BEGIN_NAMESPACE

class Q3StyleSheetItemData
{
public:
    Q3StyleSheetItem::DisplayMode disp;
    int fontitalic;
    int fontunderline;
    int fontstrikeout;
    int fontweight;
    int fontsize;
    int fontsizelog;
    int fontsizestep;
    int lineSpacing;
    QString fontfamily;
    Q3StyleSheetItem *parentstyle;
    QString stylename;
    int ncolumns;
    QColor col;
    bool anchor;
    int align;
    Q3StyleSheetItem::VerticalAlignment valign;
    int margin[5];
    Q3StyleSheetItem::ListStyle list;
    Q3StyleSheetItem::WhiteSpaceMode whitespacemode;
    QString contxt;
    bool selfnest;
    Q3StyleSheet* sheet;
};

/*!
    \class Q3StyleSheetItem qstylesheet.h
    \brief The Q3StyleSheetItem class provides an encapsulation of a set of text styles.

    \compat

    A style sheet item consists of a name and a set of attributes that
    specify its font, color, etc. When used in a \link Q3StyleSheet
    style sheet\endlink (see styleSheet()), items define the name() of
    a rich text tag and the display property changes associated with
    it.

    The \link Q3StyleSheetItem::DisplayMode display mode\endlink
    attribute indicates whether the item is a block, an inline element
    or a list element; see setDisplayMode(). The treatment of
    whitespace is controlled by the \link
    Q3StyleSheetItem::WhiteSpaceMode white space mode\endlink; see
    setWhiteSpaceMode(). An item's margins are set with setMargin(),
    In the case of list items, the list style is set with
    setListStyle(). An item may be a hypertext link anchor; see
    setAnchor(). Other attributes are set with setAlignment(),
    setVerticalAlignment(), setFontFamily(), setFontSize(),
    setFontWeight(), setFontItalic(), setFontUnderline(),
    setFontStrikeOut and setColor().
*/

/*! \enum Q3StyleSheetItem::AdditionalStyleValues
    \internal
*/

/*!
    \enum Q3StyleSheetItem::WhiteSpaceMode

    This enum defines the ways in which Q3StyleSheet can treat
    whitespace.

    \value WhiteSpaceNormal  any sequence of whitespace (including
        line-breaks) is equivalent to a single space.

    \value WhiteSpacePre  whitespace must be output exactly as given
        in the input.

    \value WhiteSpaceNoWrap  multiple spaces are collapsed as with
        WhiteSpaceNormal, but no automatic line-breaks occur. To break
        lines manually, use the \c{<br>} tag.

    \omitvalue WhiteSpaceModeUndefined
*/

/*!
    \enum Q3StyleSheetItem::Margin

    \value MarginLeft  left margin
    \value MarginRight  right margin
    \value MarginTop  top margin
    \value MarginBottom  bottom margin
    \value MarginAll  all margins (left, right, top and bottom)
    \value MarginVertical  top and bottom margins
    \value MarginHorizontal  left and right margins
    \value MarginFirstLine  margin (indentation) of the first line of
        a paragarph (in addition to the MarginLeft of the paragraph)
    \value MarginUndefined
*/

/*!
    Constructs a new style called \a name for the stylesheet \a
    parent.

    All properties in Q3StyleSheetItem are initially in the "do not
    change" state, except \link Q3StyleSheetItem::DisplayMode display
    mode\endlink, which defaults to \c DisplayInline.
*/
Q3StyleSheetItem::Q3StyleSheetItem(Q3StyleSheet* parent, const QString& name)
{
    d = new Q3StyleSheetItemData;
    d->stylename = name.toLower();
    d->sheet = parent;
    init();
    if (parent)
        parent->insert(this);
}

/*!
    Copy constructor. Constructs a copy of \a other that is not bound
    to any style sheet.
*/
Q3StyleSheetItem::Q3StyleSheetItem(const Q3StyleSheetItem & other)
{
    d = new Q3StyleSheetItemData;
    *d = *other.d;
}


/*!
    Destroys the style. Note that Q3StyleSheetItem objects become
    owned by Q3StyleSheet when they are created.
*/
Q3StyleSheetItem::~Q3StyleSheetItem()
{
    delete d;
}

/*!
  Assignment. Assings a copy of \a other that is not bound to any style sheet.
  Unbounds first from previous style sheet.
 */
Q3StyleSheetItem& Q3StyleSheetItem::operator=(const Q3StyleSheetItem& other)
{
    if (&other == this)
        return *this;
    delete d;
    d = new Q3StyleSheetItemData;
    *d = *other.d;
    return *this;
}

/*!
    Returns the style sheet this item is in.
*/
Q3StyleSheet* Q3StyleSheetItem::styleSheet()
{
    return d->sheet;
}

/*!
    \overload

    Returns the style sheet this item is in.
*/
const Q3StyleSheet* Q3StyleSheetItem::styleSheet() const
{
    return d->sheet;
}

/*!
  \internal
  Internal initialization
 */
void Q3StyleSheetItem::init()
{
    d->disp = DisplayInline;

    d->fontitalic = Undefined;
    d->fontunderline = Undefined;
    d->fontstrikeout = Undefined;
    d->fontweight = Undefined;
    d->fontsize = Undefined;
    d->fontsizelog = Undefined;
    d->fontsizestep = 0;
    d->ncolumns = Undefined;
    d->col = QColor(); // !isValid()
    d->anchor = false;
    d->align = Undefined;
    d->valign = VAlignBaseline;
    d->margin[0] = Undefined;
    d->margin[1] = Undefined;
    d->margin[2] = Undefined;
    d->margin[3] = Undefined;
    d->margin[4] = Undefined;
    d->list = ListStyleUndefined;
    d->whitespacemode = WhiteSpaceModeUndefined;
    d->selfnest = true;
    d->lineSpacing = Undefined;
}

/*!
    Returns the name of the style item.
*/
QString Q3StyleSheetItem::name() const
{
    return d->stylename;
}

/*!
    Returns the \link Q3StyleSheetItem::DisplayMode display
    mode\endlink of the style.

    \sa setDisplayMode()
*/
Q3StyleSheetItem::DisplayMode Q3StyleSheetItem::displayMode() const
{
    return d->disp;
}

/*!
    \enum Q3StyleSheetItem::DisplayMode

    This enum type defines the way adjacent elements are displayed.

    \value DisplayBlock  elements are displayed as a rectangular block
        (e.g. \c{<p>...</p>}).

    \value DisplayInline  elements are displayed in a horizontally
        flowing sequence (e.g. \c{<em>...</em>}).

    \value DisplayListItem  elements are displayed in a vertical
        sequence (e.g. \c{<li>...</li>}).

    \value DisplayNone  elements are not displayed at all.

    \omitvalue DisplayModeUndefined
*/

/*!
    Sets the display mode of the style to \a m.

    \sa displayMode()
 */
void Q3StyleSheetItem::setDisplayMode(DisplayMode m)
{
    d->disp=m;
}


/*!
    Returns the alignment of this style. Possible values are
    Qt::AlignAuto, Qt::AlignLeft, Qt::AlignRight, Qt::AlignCenter or
    Qt::AlignJustify.

    \sa setAlignment(), Qt::Alignment
*/
int Q3StyleSheetItem::alignment() const
{
    return d->align;
}

/*!
    Sets the alignment to \a f. This only makes sense for styles with
    a \link Q3StyleSheetItem::DisplayMode display mode\endlink of
    DisplayBlock. Possible values are Qt::AlignAuto, Qt::AlignLeft,
    Qt::AlignRight, Qt::AlignCenter or Qt::AlignJustify.

    \sa alignment(), displayMode(), Qt::Alignment
*/
void Q3StyleSheetItem::setAlignment(int f)
{
    d->align = f;
}


/*!
    Returns the vertical alignment of the style. Possible values are
    VAlignBaseline, VAlignSub or VAlignSuper.

    \sa setVerticalAlignment()
*/
Q3StyleSheetItem::VerticalAlignment Q3StyleSheetItem::verticalAlignment() const
{
    return d->valign;
}

/*!
    \enum Q3StyleSheetItem::VerticalAlignment

    This enum type defines the way elements are aligned vertically.
    This is only supported for text elements.

    \value VAlignBaseline align the baseline of the element (or the
        bottom, if the element doesn't have a baseline) with the
        baseline of the parent

    \value VAlignSub subscript the element

    \value VAlignSuper superscript the element

*/


/*!
    Sets the vertical alignment to \a valign. Possible values are
    VAlignBaseline, VAlignSub or VAlignSuper.

    The vertical alignment property is not inherited.

    \sa verticalAlignment()
*/
void Q3StyleSheetItem::setVerticalAlignment(VerticalAlignment valign)
{
    d->valign = valign;
}


/*!
    Returns true if the style sets an italic font; otherwise returns
    false.

    \sa setFontItalic(), definesFontItalic()
*/
bool Q3StyleSheetItem::fontItalic() const
{
    return d->fontitalic > 0;
}

/*!
    If \a italic is true sets italic for the style; otherwise sets
    upright.

    \sa fontItalic(), definesFontItalic()
*/
void Q3StyleSheetItem::setFontItalic(bool italic)
{
    d->fontitalic = italic?1:0;
}

/*!
    Returns true if the style defines a font shape; otherwise returns
    false. A style does not define any shape until setFontItalic() is
    called.

    \sa setFontItalic(), fontItalic()
*/
bool Q3StyleSheetItem::definesFontItalic() const
{
    return d->fontitalic != Undefined;
}

/*!
    Returns true if the style sets an underlined font; otherwise
    returns false.

    \sa setFontUnderline(), definesFontUnderline()
*/
bool Q3StyleSheetItem::fontUnderline() const
{
    return d->fontunderline > 0;
}

/*!
    If \a underline is true, sets underline for the style; otherwise
    sets no underline.

    \sa fontUnderline(), definesFontUnderline()
*/
void Q3StyleSheetItem::setFontUnderline(bool underline)
{
    d->fontunderline = underline?1:0;
}

/*!
    Returns true if the style defines a setting for the underline
    property of the font; otherwise returns false. A style does not
    define this until setFontUnderline() is called.

    \sa setFontUnderline(), fontUnderline()
*/
bool Q3StyleSheetItem::definesFontUnderline() const
{
    return d->fontunderline != Undefined;
}


/*!
    Returns true if the style sets a strike out font; otherwise
    returns false.

    \sa setFontStrikeOut(), definesFontStrikeOut()
*/
bool Q3StyleSheetItem::fontStrikeOut() const
{
    return d->fontstrikeout > 0;
}

/*!
    If \a strikeOut is true, sets strike out for the style; otherwise
    sets no strike out.

    \sa fontStrikeOut(), definesFontStrikeOut()
*/
void Q3StyleSheetItem::setFontStrikeOut(bool strikeOut)
{
    d->fontstrikeout = strikeOut?1:0;
}

/*!
    Returns true if the style defines a setting for the strikeOut
    property of the font; otherwise returns false. A style does not
    define this until setFontStrikeOut() is called.

    \sa setFontStrikeOut(), fontStrikeOut()
*/
bool Q3StyleSheetItem::definesFontStrikeOut() const
{
    return d->fontstrikeout != Undefined;
}


/*!
    Returns the font weight setting of the style. This is either a
    valid QFont::Weight or the value Q3StyleSheetItem::Undefined.

    \sa setFontWeight(), QFont
*/
int Q3StyleSheetItem::fontWeight() const
{
    return d->fontweight;
}

/*!
    Sets the font weight setting of the style to \a w. Valid values
    are those defined by QFont::Weight.

    \sa QFont, fontWeight()
*/
void Q3StyleSheetItem::setFontWeight(int w)
{
    d->fontweight = w;
}

/*!
    Returns the logical font size setting of the style. This is either
    a valid size between 1 and 7 or Q3StyleSheetItem::Undefined.

    \sa setLogicalFontSize(), setLogicalFontSizeStep(), QFont::pointSize(), QFont::setPointSize()
*/
int Q3StyleSheetItem::logicalFontSize() const
{
    return d->fontsizelog;
}


/*!
    Sets the logical font size setting of the style to \a s. Valid
    logical sizes are 1 to 7.

    \sa logicalFontSize(), QFont::pointSize(), QFont::setPointSize()
*/
void Q3StyleSheetItem::setLogicalFontSize(int s)
{
    d->fontsizelog = s;
}

/*!
    Returns the logical font size step of this style.

    The default is 0. Tags such as \c big define \c +1; \c small
    defines \c -1.

    \sa setLogicalFontSizeStep()
*/
int Q3StyleSheetItem::logicalFontSizeStep() const
{
    return d->fontsizestep;
}

/*!
    Sets the logical font size step of this style to \a s.

    \sa logicalFontSizeStep()
*/
void Q3StyleSheetItem::setLogicalFontSizeStep(int s)
{
    d->fontsizestep = s;
}



/*!
    Sets the font size setting of the style to \a s points.

    \sa fontSize(), QFont::pointSize(), QFont::setPointSize()
*/
void Q3StyleSheetItem::setFontSize(int s)
{
    d->fontsize = s;
}

/*!
    Returns the font size setting of the style. This is either a valid
    point size or Q3StyleSheetItem::Undefined.

    \sa setFontSize(), QFont::pointSize(), QFont::setPointSize()
*/
int Q3StyleSheetItem::fontSize() const
{
    return d->fontsize;
}


/*!
    Returns the style's font family setting. This is either a valid
    font family or an empty string if no family has been set.

    \sa setFontFamily(), QFont::family(), QFont::setFamily()
*/
QString Q3StyleSheetItem::fontFamily() const
{
    return d->fontfamily;
}

/*!
    Sets the font family setting of the style to \a fam.

    \sa fontFamily(), QFont::family(), QFont::setFamily()
*/
void Q3StyleSheetItem::setFontFamily(const QString& fam)
{
    d->fontfamily = fam;
}


/*!
  Returns the number of columns for this style.

  \sa setNumberOfColumns(), displayMode(), setDisplayMode()

 */
int Q3StyleSheetItem::numberOfColumns() const
{
    return d->ncolumns;
}


/*!
  Sets the number of columns for this style to \a ncols. Elements in the style
  are divided into columns.

  This makes sense only if the style uses a block display mode
  (see Q3StyleSheetItem::DisplayMode).

  \sa numberOfColumns()
 */
void Q3StyleSheetItem::setNumberOfColumns(int ncols)
{
    if (ncols > 0)
        d->ncolumns = ncols;
}


/*!
    Returns the text color of this style or an invalid color if no
    color has been set.

    \sa setColor() QColor::isValid()
*/
QColor Q3StyleSheetItem::color() const
{
    return d->col;
}

/*!
    Sets the text color of this style to \a c.

    \sa color()
*/
void Q3StyleSheetItem::setColor(const QColor &c)
{
    d->col = c;
}

/*!
    Returns whether this style is an anchor.

    \sa setAnchor()
*/
bool Q3StyleSheetItem::isAnchor() const
{
    return d->anchor;
}

/*!
    If \a anc is true, sets this style to be an anchor (hypertext
    link); otherwise sets it to not be an anchor. Elements in this
    style link to other documents or anchors.

    \sa isAnchor()
*/
void Q3StyleSheetItem::setAnchor(bool anc)
{
    d->anchor = anc;
}


/*!
    Returns the whitespace mode.

    \sa setWhiteSpaceMode() WhiteSpaceMode
*/
Q3StyleSheetItem::WhiteSpaceMode Q3StyleSheetItem::whiteSpaceMode() const
{
    return d->whitespacemode;
}

/*!
    Sets the whitespace mode to \a m.

    \sa WhiteSpaceMode
*/
void Q3StyleSheetItem::setWhiteSpaceMode(WhiteSpaceMode m)
{
    d->whitespacemode = m;
}


/*!
    Returns the width of margin \a m in pixels.

    The margin, \a m, can be MarginLeft, MarginRight,
    MarginTop, MarginBottom, or MarginFirstLine

    \sa setMargin() Margin
*/
int Q3StyleSheetItem::margin(Margin m) const
{
    if (m == MarginAll) {
        return d->margin[MarginLeft];
    } else if (m == MarginVertical) {
        return d->margin[MarginTop];
    } else if (m == MarginHorizontal) {
        return d->margin[MarginLeft];
    } else {
        return d->margin[m];
    }
}


/*!
    Sets the width of margin \a m to \a v pixels.

    The margin, \a m, can be \c MarginLeft, \c MarginRight, \c
    MarginTop, \c MarginBottom, MarginFirstLine, \c MarginAll,
    \c MarginVertical or \c MarginHorizontal. The value \a v must
    be >= 0.

    \sa margin()
*/
void Q3StyleSheetItem::setMargin(Margin m, int v)
{
    if (m == MarginAll) {
        d->margin[MarginLeft] = v;
        d->margin[MarginRight] = v;
        d->margin[MarginTop] = v;
        d->margin[MarginBottom] = v;
    } else if (m == MarginVertical) {
        d->margin[MarginTop] = v;
        d->margin[MarginBottom] = v;
    } else if (m == MarginHorizontal) {
        d->margin[MarginLeft] = v;
        d->margin[MarginRight] = v;
    } else {
        d->margin[m] = v;
    }
}


/*!
    Returns the list style of the style.

    \sa setListStyle() ListStyle
 */
Q3StyleSheetItem::ListStyle Q3StyleSheetItem::listStyle() const
{
    return d->list;
}

/*!
    \enum Q3StyleSheetItem::ListStyle

    This enum type defines how the items in a list are prefixed when
    displayed.

    \value ListDisc  a filled circle (i.e. a bullet)
    \value ListCircle  an unfilled circle
    \value ListSquare  a filled square
    \value ListDecimal  an integer in base 10: \e 1, \e 2, \e 3, ...
    \value ListLowerAlpha  a lowercase letter: \e a, \e b, \e c, ...
    \value ListUpperAlpha  an uppercase letter: \e A, \e B, \e C, ...
    \omitvalue ListStyleUndefined
*/

/*!
    Sets the list style of the style to \a s.

    This is used by nested elements that have a display mode of \c
    DisplayListItem.

    \sa listStyle() DisplayMode ListStyle
*/
void Q3StyleSheetItem::setListStyle(ListStyle s)
{
    d->list=s;
}


/*!
    Returns a space-separated list of names of styles that may contain
    elements of this style. If nothing has been set, contexts()
    returns an empty string, which indicates that this style can be
    nested everywhere.

    \sa setContexts()
*/
QString Q3StyleSheetItem::contexts() const
{
    return d->contxt;
}

/*!
    Sets a space-separated list of names of styles that may contain
    elements of this style. If \a c is empty, the style can be nested
    everywhere.

    \sa contexts()
*/
void Q3StyleSheetItem::setContexts(const QString& c)
{
    d->contxt = QLatin1Char(' ') + c + QLatin1Char(' ');
}

/*!
    Returns true if this style can be nested into an element of style
    \a s; otherwise returns false.

    \sa contexts(), setContexts()
*/
bool Q3StyleSheetItem::allowedInContext(const Q3StyleSheetItem* s) const
{
    if (d->contxt.isEmpty())
        return true;
    return d->contxt.contains(QLatin1Char(' ')+s->name()+QLatin1Char(' '));
}


/*!
    Returns true if this style has self-nesting enabled; otherwise
    returns false.

    \sa setSelfNesting()
*/
bool Q3StyleSheetItem::selfNesting() const
{
    return d->selfnest;
}

/*!
    Sets the self-nesting property for this style to \a nesting.

    In order to support "dirty" HTML, paragraphs \c{<p>} and list
    items \c{<li>} are not self-nesting. This means that starting a
    new paragraph or list item automatically closes the previous one.

    \sa selfNesting()
*/
void Q3StyleSheetItem::setSelfNesting(bool nesting)
{
    d->selfnest = nesting;
}

/*!
   \internal
    Sets the linespacing to be at least \a ls pixels.

    For compatibility with previous Qt releases, small values get
    treated differently: If \a ls is smaller than the default font
    line spacing in pixels at parse time, the resulting line spacing
    is the sum of the default line spacing plus \a ls. We recommend
    not relying on this behavior.
*/

void Q3StyleSheetItem::setLineSpacing(int ls)
{
    d->lineSpacing = ls;
}

/*!
  Returns the line spacing.
*/

int Q3StyleSheetItem::lineSpacing() const
{
    return d->lineSpacing;
}

//************************************************************************




//************************************************************************


/*!
    \class Q3StyleSheet qstylesheet.h
    \brief The Q3StyleSheet class is a collection of styles for rich text
    rendering and a generator of tags.

    \compat

    By creating Q3StyleSheetItem objects for a style sheet you build a
    definition of a set of tags. This definition will be used by the
    internal rich text rendering system to parse and display text
    documents to which the style sheet applies. Rich text is normally
    visualized in a QTextEdit or a QTextBrowser. However, QLabel,
    QWhatsThis and QMessageBox also support it, and other classes are
    likely to follow. With QSimpleRichText it is possible to use the
    rich text renderer for custom widgets as well.

    The default Q3StyleSheet object has the following style bindings,
    sorted by structuring bindings, anchors, character style bindings
    (i.e. inline styles), special elements such as horizontal lines or
    images, and other tags. In addition, rich text supports simple
    HTML tables.

    The structuring tags are
    \table
    \header \i Structuring tags \i Notes
    \row \i \c{<qt>}...\c{</qt>}
         \i A Qt rich text document. It understands the following
            attributes:
            \list
            \i \c title -- The caption of the document. This attribute is
                easily accessible with QTextEdit::documentTitle().
            \i \c type -- The type of the document. The default type is \c
                page. It indicates that the document is displayed in a
                page of its own. Another style is \c detail, which can be
                used to explain certain expressions in more detail in a
                few sentences. For \c detail, QTextBrowser will then keep
                the current page and display the new document in a small
                popup similar to QWhatsThis. Note that links will not work
                in documents with \c{<qt type="detail">...</qt>}.
            \i \c bgcolor -- The background color, for example \c
                bgcolor="yellow" or \c bgcolor="#0000FF".
            \i \c background -- The background pixmap, for example \c
                background="granite.xpm". The pixmap name will be resolved
                by a Q3MimeSourceFactory().
            \i \c text -- The default text color, for example \c text="red".
            \i \c link -- The link color, for example \c link="green".
            \endlist
    \row \i \c{<h1>...</h1>}
         \i A top-level heading.
    \row \i \c{<h2>...</h2>}
         \i A sublevel heading.
    \row \i \c{<h3>...</h3>}
         \i A sub-sublevel heading.
    \row \i \c{<p>...</p>}
         \i A left-aligned paragraph. Adjust the alignment with the \c
            align attribute. Possible values are \c left, \c right and
            \c center.
    \row \i \c{<center>...}<br>\c{</center>}
         \i A centered paragraph.
    \row \i \c{<blockquote>...}<br>\c{</blockquote>}
         \i An indented paragraph that is useful for quotes.
    \row \i \c{<ul>...</ul>}
         \i An unordered list. You can also pass a type argument to
            define the bullet style. The default is \c type=disc;
            other types are \c circle and \c square.
    \row \i \c{<ol>...</ol>}
         \i An ordered list. You can also pass a type argument to
            define the enumeration label style. The default is \c
            type="1"; other types are \c "a" and \c "A".
    \row \i \c{<li>...</li>}
         \i A list item. This tag can be used only within the context
            of \c{<ol>} or \c{<ul>}.
    \row \i \c{<pre>...</pre>}
         \i For larger chunks of code. Whitespaces in the contents are
            preserved. For small bits of code use the inline-style \c
            code.
    \endtable

    Anchors and links are done with a single tag:
    \table
    \header \i Anchor tags \i Notes
    \row \i \c{<a>...</a>}
         \i An anchor or link.
            \list
            \i A link is created by using an \c href
                attribute, for example
                <br>\c{<a href="target.qml">Link Text</a>}. Links to
                targets within a document are achieved in the same way
                as for HTML, e.g.
                <br>\c{<a href="target.qml#subtitle">Link Text</a>}.
            \i A target is created by using a \c name
                attribute, for example
                <br>\c{<a name="subtitle"><h2>Sub Title</h2></a>}.
            \endlist
    \endtable

    The default character style bindings are
    \table
    \header \i Style tags \i Notes
    \row \i \c{<em>...</em>}
         \i Emphasized. By default this is the same as \c{<i>...</i>}
            (italic).
    \row \i \c{<strong>...</strong>}
         \i Strong. By default this is the same as \c{<b>...</b>}
            (bold).
    \row \i \c{<i>...</i>}
         \i Italic font style.
    \row \i \c{<b>...</b>}
         \i Bold font style.
    \row \i \c{<u>...</u>}
         \i Underlined font style.
    \row \i \c{<s>...</s>}
         \i Strike out font style.
    \row \i \c{<big>...</big>}
         \i A larger font size.
    \row \i \c{<small>...</small>}
         \i A smaller font size.
    \row \i \c{<sub>...</sub>}
         \i Subscripted text
    \row \i \c{<sup>...</sup>}
         \i Superscripted text
    \row \i \c{<code>...</code>}
         \i Indicates code. By default this is the same as
            \c{<tt>...</tt>} (typewriter). For larger chunks of code
            use the block-tag \c{<}\c{pre>}.
    \row \i \c{<tt>...</tt>}
         \i Typewriter font style.
    \row \i \c{<font>...</font>}
         \i Customizes the font size, family  and text color. The tag
            understands the following  attributes:
            \list
            \i \c color -- The text color, for example \c color="red" or
                \c color="#FF0000".
            \i \c size -- The logical size of the font. Logical sizes 1
                to 7 are supported. The value may either be absolute
                (for example, \c size=3) or relative (\c size=-2). In
                the latter case the sizes are simply added.
            \i \c face -- The family of the font, for example \c face=times.
            \endlist
    \endtable

    Special elements are:
    \table
    \header \i Special tags \i Notes
    \row \i \c{<img>}
         \i An image. The image name for the mime source factory is
            given in the source attribute, for example
            \c{<img src="qt.xpm">} The image tag also understands the
            attributes \c width and \c height that determine the size
            of the image. If the pixmap does not fit the specified
            size it will be scaled automatically (by using
            QImage::smoothScale()).

            The \c align attribute determines where the image is
            placed. By default, an image is placed inline just like a
            normal character. Specify \c left or \c right to place the
            image at the respective side.
    \row \i \c{<hr>}
         \i A horizontal line.
    \row \i \c{<br>}
         \i A line break.
    \row \i \c{<nobr>...</nobr>}
         \i No break. Prevents word wrap.
    \endtable

    In addition, rich text supports simple HTML tables. A table
    consists of one or more rows each of which contains one or more
    cells. Cells are either data cells or header cells, depending on
    their content. Cells which span rows and columns are supported.

    \table
    \header \i Table tags \i Notes
    \row \i \c{<table>...</table>}
         \i A table. Tables support the following attributes:
            \list
            \i \c bgcolor -- The background color.
            \i \c width -- The table width. This is either an absolute
                pixel width or a relative percentage of the table's
                width, for example \c width=80%.
            \i \c border -- The width of the table border. The default is
                0 (= no border).
            \i \c cellspacing -- Additional space around the table cells.
                The default is 2.
            \i \c cellpadding -- Additional space around the contents of
                table cells. The default is 1.
            \endlist
    \row \i \c{<tr>...</tr>}
         \i A table row. This is only valid within a \c table. Rows
            support the following attribute:
            \list
            \i \c bgcolor -- The background color.
            \endlist
    \row \i \c{<th>...</th>}
         \i A table header cell. Similar to \c td, but defaults to
            center alignment and a bold font.
    \row \i \c{<td>...</td>}
         \i A table data cell. This is only valid within a \c tr.
            Cells support the following attributes:
            \list
            \i \c bgcolor -- The background color.
            \i \c width -- The cell width. This is either an absolute
                pixel width or a relative percentage of table's width,
                for example \c width=50%.
            \i \c colspan -- Specifies how many columns this cell spans.
                The default is 1.
            \i \c rowspan -- Specifies how many rows this cell spans. The
                default is 1.
            \i \c align -- Qt::Alignment; possible values are \c left, \c
                right, and \c center. The default is \c left.
            \i \c valign -- Qt::Vertical alignment; possible values are \c
            top, \c middle, and \c bottom. The default is \c middle.
            \endlist
    \endtable
*/

/*!
    Creates a style sheet called \a name, with parent \a parent. Like
    any QObject it will be deleted when its parent is destroyed (if
    the child still exists).

    By default the style sheet has the tag definitions defined above.
*/
Q3StyleSheet::Q3StyleSheet(QObject *parent, const char *name)
    : QObject(parent)
{
    setObjectName(QLatin1String(name));
    init();
}

/*!
    Destroys the style sheet. All styles inserted into the style sheet
    will be deleted.
*/
Q3StyleSheet::~Q3StyleSheet()
{
    QHash<QString, Q3StyleSheetItem *>::iterator it = styles.begin();
    while (it != styles.end()) {
        delete it.value();
        ++it;
    }
}

/*!
  \internal
  Initialized the style sheet to the basic Qt style.
*/
void Q3StyleSheet::init()
{
    nullstyle = new Q3StyleSheetItem(this, QString::fromLatin1(""));

    Q3StyleSheetItem *style;

    style = new Q3StyleSheetItem(this, QLatin1String("qml")); // compatibility
    style->setDisplayMode(Q3StyleSheetItem::DisplayBlock);

    style = new Q3StyleSheetItem(this, QString::fromLatin1("qt"));
    style->setDisplayMode(Q3StyleSheetItem::DisplayBlock);

    style = new Q3StyleSheetItem(this, QString::fromLatin1("a"));
    style->setAnchor(true);

    style = new Q3StyleSheetItem(this, QString::fromLatin1("em"));
    style->setFontItalic(true);

    style = new Q3StyleSheetItem(this, QString::fromLatin1("i"));
    style->setFontItalic(true);

    style = new Q3StyleSheetItem(this, QString::fromLatin1("big"));
    style->setLogicalFontSizeStep(1);
    style = new Q3StyleSheetItem(this, QString::fromLatin1("large")); // compatibility
    style->setLogicalFontSizeStep(1);

    style = new Q3StyleSheetItem(this, QString::fromLatin1("small"));
    style->setLogicalFontSizeStep(-1);

    style = new Q3StyleSheetItem(this, QString::fromLatin1("strong"));
    style->setFontWeight(QFont::Bold);

    style = new Q3StyleSheetItem(this, QString::fromLatin1("b"));
    style->setFontWeight(QFont::Bold);

    style = new Q3StyleSheetItem(this, QString::fromLatin1("h1"));
    style->setFontWeight(QFont::Bold);
    style->setLogicalFontSize(6);
    style->setDisplayMode(Q3StyleSheetItem::DisplayBlock);
    style-> setMargin(Q3StyleSheetItem::MarginTop, 18);
    style-> setMargin(Q3StyleSheetItem::MarginBottom, 12);

    style = new Q3StyleSheetItem(this, QString::fromLatin1("h2"));
    style->setFontWeight(QFont::Bold);
    style->setLogicalFontSize(5);
    style->setDisplayMode(Q3StyleSheetItem::DisplayBlock);
    style-> setMargin(Q3StyleSheetItem::MarginTop, 16);
    style-> setMargin(Q3StyleSheetItem::MarginBottom, 12);

    style = new Q3StyleSheetItem(this, QString::fromLatin1("h3"));
    style->setFontWeight(QFont::Bold);
    style->setLogicalFontSize(4);
    style->setDisplayMode(Q3StyleSheetItem::DisplayBlock);
    style-> setMargin(Q3StyleSheetItem::MarginTop, 14);
    style-> setMargin(Q3StyleSheetItem::MarginBottom, 12);

    style = new Q3StyleSheetItem(this, QString::fromLatin1("h4"));
    style->setFontWeight(QFont::Bold);
    style->setLogicalFontSize(3);
    style->setDisplayMode(Q3StyleSheetItem::DisplayBlock);
    style-> setMargin(Q3StyleSheetItem::MarginVertical, 12);

    style = new Q3StyleSheetItem(this, QString::fromLatin1("h5"));
    style->setFontWeight(QFont::Bold);
    style->setLogicalFontSize(2);
    style->setDisplayMode(Q3StyleSheetItem::DisplayBlock);
    style-> setMargin(Q3StyleSheetItem::MarginTop, 12);
    style-> setMargin(Q3StyleSheetItem::MarginBottom, 4);

    style = new Q3StyleSheetItem(this, QString::fromLatin1("p"));
    style->setDisplayMode(Q3StyleSheetItem::DisplayBlock);
    style-> setMargin(Q3StyleSheetItem::MarginVertical, 12);
    style->setSelfNesting(false);

    style = new Q3StyleSheetItem(this, QString::fromLatin1("center"));
    style->setDisplayMode(Q3StyleSheetItem::DisplayBlock);
    style->setAlignment(Qt::AlignCenter);

    style = new Q3StyleSheetItem(this, QString::fromLatin1("twocolumn"));
    style->setDisplayMode(Q3StyleSheetItem::DisplayBlock);
    style->setNumberOfColumns(2);

    style =  new Q3StyleSheetItem(this, QString::fromLatin1("multicol"));
    style->setDisplayMode(Q3StyleSheetItem::DisplayBlock);
    (void) new Q3StyleSheetItem(this, QString::fromLatin1("font"));

    style = new Q3StyleSheetItem(this, QString::fromLatin1("ul"));
    style->setDisplayMode(Q3StyleSheetItem::DisplayBlock);
    style->setListStyle(Q3StyleSheetItem::ListDisc);
    style-> setMargin(Q3StyleSheetItem::MarginVertical, 12);
    style->setMargin(Q3StyleSheetItem::MarginLeft, 40);

    style = new Q3StyleSheetItem(this, QString::fromLatin1("ol"));
    style->setDisplayMode(Q3StyleSheetItem::DisplayBlock);
    style->setListStyle(Q3StyleSheetItem::ListDecimal);
    style-> setMargin(Q3StyleSheetItem::MarginVertical, 12);
    style->setMargin(Q3StyleSheetItem::MarginLeft, 40);

    style = new Q3StyleSheetItem(this, QString::fromLatin1("li"));
    style->setDisplayMode(Q3StyleSheetItem::DisplayListItem);
    style->setSelfNesting(false);

    style = new Q3StyleSheetItem(this, QString::fromLatin1("code"));
    style->setFontFamily(QString::fromLatin1("Courier New,courier"));

    style = new Q3StyleSheetItem(this, QString::fromLatin1("tt"));
    style->setFontFamily(QString::fromLatin1("Courier New,courier"));

    new Q3StyleSheetItem(this, QString::fromLatin1("img"));
    new Q3StyleSheetItem(this, QString::fromLatin1("br"));
    new Q3StyleSheetItem(this, QString::fromLatin1("hr"));

    style = new Q3StyleSheetItem(this, QString::fromLatin1("sub"));
    style->setVerticalAlignment(Q3StyleSheetItem::VAlignSub);
    style = new Q3StyleSheetItem(this, QString::fromLatin1("sup"));
    style->setVerticalAlignment(Q3StyleSheetItem::VAlignSuper);

    style = new Q3StyleSheetItem(this, QString::fromLatin1("pre"));
    style->setFontFamily(QString::fromLatin1("Courier New,courier"));
    style->setDisplayMode(Q3StyleSheetItem::DisplayBlock);
    style->setWhiteSpaceMode(Q3StyleSheetItem::WhiteSpacePre);
    style-> setMargin(Q3StyleSheetItem::MarginVertical, 12);

    style = new Q3StyleSheetItem(this, QString::fromLatin1("blockquote"));
    style->setDisplayMode(Q3StyleSheetItem::DisplayBlock);
    style->setMargin(Q3StyleSheetItem::MarginHorizontal, 40);

     style = new Q3StyleSheetItem(this, QString::fromLatin1("head"));
     style->setDisplayMode(Q3StyleSheetItem::DisplayNone);
     style = new Q3StyleSheetItem(this, QString::fromLatin1("body"));
     style->setDisplayMode(Q3StyleSheetItem::DisplayBlock);
     style = new Q3StyleSheetItem(this, QString::fromLatin1("div"));
     style->setDisplayMode(Q3StyleSheetItem::DisplayBlock) ;
     style = new Q3StyleSheetItem(this, QString::fromLatin1("span"));
     style = new Q3StyleSheetItem(this, QString::fromLatin1("dl"));
    style-> setMargin(Q3StyleSheetItem::MarginVertical, 8);
     style->setDisplayMode(Q3StyleSheetItem::DisplayBlock);
     style = new Q3StyleSheetItem(this, QString::fromLatin1("dt"));
     style->setDisplayMode(Q3StyleSheetItem::DisplayBlock);
     style->setContexts(QString::fromLatin1("dl"));
     style = new Q3StyleSheetItem(this, QString::fromLatin1("dd"));
     style->setDisplayMode(Q3StyleSheetItem::DisplayBlock);
     style->setMargin(Q3StyleSheetItem::MarginLeft, 30);
     style->setContexts(QString::fromLatin1("dt dl"));
     style = new Q3StyleSheetItem(this, QString::fromLatin1("u"));
     style->setFontUnderline(true);
     style = new Q3StyleSheetItem(this, QString::fromLatin1("s"));
     style->setFontStrikeOut(true);
     style = new Q3StyleSheetItem(this, QString::fromLatin1("nobr"));
     style->setWhiteSpaceMode(Q3StyleSheetItem::WhiteSpaceNoWrap);

     // compatibily with some minor 3.0.x Qt versions that had an
     // undocumented <wsp> tag. ### Remove 3.1
     style = new Q3StyleSheetItem(this, QString::fromLatin1("wsp"));
     style->setWhiteSpaceMode(Q3StyleSheetItem::WhiteSpacePre);

     // tables
     style = new Q3StyleSheetItem(this, QString::fromLatin1("table"));
     style = new Q3StyleSheetItem(this, QString::fromLatin1("tr"));
     style->setContexts(QString::fromLatin1("table"));
     style = new Q3StyleSheetItem(this, QString::fromLatin1("td"));
     style->setContexts(QString::fromLatin1("tr"));
     style = new Q3StyleSheetItem(this, QString::fromLatin1("th"));
     style->setFontWeight(QFont::Bold);
     style->setAlignment(Qt::AlignCenter);
     style->setContexts(QString::fromLatin1("tr"));

     style = new Q3StyleSheetItem(this, QString::fromLatin1("html"));
}



static Q3StyleSheet* defaultsheet = 0;
static Q3SingleCleanupHandler<Q3StyleSheet> qt_cleanup_stylesheet;

/*!
    Returns the application-wide default style sheet. This style sheet
    is used by rich text rendering classes such as QSimpleRichText,
    QWhatsThis and QMessageBox to define the rendering style and
    available tags within rich text documents. It also serves as the
    initial style sheet for the more complex render widgets, QTextEdit
    and QTextBrowser.

    \sa setDefaultSheet()
*/
Q3StyleSheet* Q3StyleSheet::defaultSheet()
{
    if (!defaultsheet) {
        defaultsheet = new Q3StyleSheet();
        qt_cleanup_stylesheet.set(&defaultsheet);
    }
    return defaultsheet;
}

/*!
    Sets the application-wide default style sheet to \a sheet,
    deleting any style sheet previously set. The ownership is
    transferred to Q3StyleSheet.

    \sa defaultSheet()
*/
void Q3StyleSheet::setDefaultSheet(Q3StyleSheet* sheet)
{
    if (defaultsheet != sheet) {
        if (defaultsheet)
            qt_cleanup_stylesheet.reset();
        delete defaultsheet;
    }
    defaultsheet = sheet;
    if (defaultsheet)
        qt_cleanup_stylesheet.set(&defaultsheet);
}

/*!\internal
  Inserts \a style. Any tags generated after this time will be
  bound to this style. Note that \a style becomes owned by the
  style sheet and will be deleted when the style sheet is destroyed.
*/
void Q3StyleSheet::insert(Q3StyleSheetItem* style)
{
    styles.insert(style->name(), style);
}


/*!
    Returns the style called \a name or 0 if there is no such style.
*/
Q3StyleSheetItem* Q3StyleSheet::item(const QString& name)
{
    if (name.isNull())
        return 0;
    return styles.value(name);
}

/*!
    \overload

    Returns the style called \a name or 0 if there is no such style
    (const version)
*/
const Q3StyleSheetItem* Q3StyleSheet::item(const QString& name) const
{
    if (name.isNull())
        return 0;
    return styles.value(name);
}

/*!  Auxiliary function. Converts the plain text string \a plain to a
    rich text formatted paragraph while preserving most of its look.

    \a mode defines the whitespace mode. Possible values are \c
    Q3StyleSheetItem::WhiteSpacePre (no wrapping, all whitespaces
    preserved) and Q3StyleSheetItem::WhiteSpaceNormal (wrapping,
    simplified whitespaces).

    \sa escape()
*/
QString Q3StyleSheet::convertFromPlainText(const QString& plain, Q3StyleSheetItem::WhiteSpaceMode mode)
{
    return Qt::convertFromPlainText(plain, Qt::WhiteSpaceMode(mode));
}

/*!
    Auxiliary function. Converts the plain text string \a plain to a
    rich text formatted string with any HTML meta-characters escaped.

    \sa convertFromPlainText()
*/
QString Q3StyleSheet::escape(const QString& plain)
{
    return Qt::escape(plain);
}

// Must doc this enum somewhere, and it is logically related to Q3StyleSheet

/*!
    Returns true if the string \a text is likely to be rich text;
    otherwise returns false.

    This function uses a fast and therefore simple heuristic. It
    mainly checks whether there is something that looks like a tag
    before the first line break. Although the result may be correct
    for common cases, there is no guarantee.
*/
bool Q3StyleSheet::mightBeRichText(const QString& text)
{
    return Qt::mightBeRichText(text);
}


/*!
    \fn void Q3StyleSheet::error(const QString& msg) const

    This virtual function is called when an error occurs when
    processing rich text. Reimplement it if you need to catch error
    messages.

    Errors might occur if some rich text strings contain tags that are
    not understood by the stylesheet, if some tags are nested
    incorrectly, or if tags are not closed properly.

    \a msg is the error message.
*/
void Q3StyleSheet::error(const QString&) const
{
}


/*!
    Scales the font \a font to the appropriate physical point size
    corresponding to the logical font size \a logicalSize.

    When calling this function, \a font has a point size corresponding
    to the logical font size 3.

    Logical font sizes range from 1 to 7, with 1 being the smallest.

    \sa Q3StyleSheetItem::logicalFontSize(), Q3StyleSheetItem::logicalFontSizeStep(), QFont::setPointSize()
 */
void Q3StyleSheet::scaleFont(QFont& font, int logicalSize) const
{
    if (logicalSize < 1)
        logicalSize = 1;
    if (logicalSize > 7)
        logicalSize = 7;
    int baseSize = font.pointSize();
    bool pixel = false;
    if (baseSize == -1) {
        baseSize = font.pixelSize();
        pixel = true;
    }
    int s;
    switch (logicalSize) {
    case 1:
        s =  7*baseSize/10;
        break;
    case 2:
        s = (8 * baseSize) / 10;
        break;
    case 4:
        s =  (12 * baseSize) / 10;
        break;
    case 5:
        s = (15 * baseSize) / 10;
        break;
    case 6:
        s = 2 * baseSize;
        break;
    case 7:
        s = (24 * baseSize) / 10;
        break;
    default:
        s = baseSize;
    }
    if (pixel)
        font.setPixelSize(qMax(1, s));
    else
        font.setPointSize(qMax(1, s));
}

QT_END_NAMESPACE

#endif // QT_NO_RICHTEXT
