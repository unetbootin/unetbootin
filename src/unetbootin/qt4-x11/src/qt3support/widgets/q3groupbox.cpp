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

#include "q3groupbox.h"

#include "qlayout.h"
#include "qpainter.h"
#include "qbitmap.h"
#include "q3accel.h"
#include "qradiobutton.h"
#include "qdrawutil.h"
#include "qapplication.h"
#include "qstyle.h"
#include "qcheckbox.h"
#include "qaccessible.h"
#include "qstyleoption.h"
#include "qdebug.h"

QT_BEGIN_NAMESPACE

/*!
    \class Q3GroupBox
    \brief The Q3GroupBox widget provides a group box frame with a title.

    \compat

    A group box provides a frame, a title and a keyboard shortcut, and
    displays various other widgets inside itself. The title is on top,
    the keyboard shortcut moves keyboard focus to one of the group
    box's child widgets, and the child widgets are usually laid out
    horizontally (or vertically) inside the frame.

    The simplest way to use it is to create a group box with the
    desired number of columns (or rows) and orientation, and then just
    create widgets with the group box as parent.

    It is also possible to change the orientation() and number of
    columns() after construction, or to ignore all the automatic
    layout support and manage the layout yourself. You can add 'empty'
    spaces to the group box with addSpace().

    Q3GroupBox also lets you set the title() (normally set in the
    constructor) and the title's alignment().

    You can change the spacing used by the group box with
    setInsideMargin() and setInsideSpacing(). To minimize space
    consumption, you can remove the right, left and bottom edges of
    the frame with setFlat().

    \sa QButtonGroup
*/

class QCheckBox;

class Q3GroupBoxPrivate
{
public:
    Q3GroupBoxPrivate(Q3GroupBox *w):
      q(w), vbox(0), grid(0), row(0), col(0), nRows(0), nCols(0), dir(Qt::Horizontal),
        spac(5), marg(11),
        checkbox(0),
        frameStyle(Q3GroupBox::GroupBoxPanel | Q3GroupBox::Sunken),
        lineWidth(1), midLineWidth(0), frameWidth(0),
        leftFrameWidth(0), rightFrameWidth(0),
        topFrameWidth(0), bottomFrameWidth(0) {}

    void updateFrameWidth();
    void updateStyledFrameWidths();

    Q3GroupBox *q;
    QVBoxLayout *vbox;
    QGridLayout *grid;
    int row;
    int col;
    int nRows, nCols;
    Qt::Orientation dir;
    int spac, marg;

    QCheckBox *checkbox;

    int frameStyle;
    int oldFrameStyle;
    short lineWidth, //line width
        midLineWidth; //midline width
    int frameWidth;
    short leftFrameWidth, rightFrameWidth, 
      topFrameWidth, bottomFrameWidth;
};

/*!
  \internal
  Updates the frame widths from the style.
*/
void Q3GroupBoxPrivate::updateStyledFrameWidths()
{
    QStyleOptionFrameV2 opt;
    opt.initFrom(q);
    QRect cr = q->style()->subElementRect(QStyle::SE_FrameContents, &opt, q);
    leftFrameWidth = cr.left() - opt.rect.left();
    topFrameWidth = cr.top() - opt.rect.top();
    rightFrameWidth = opt.rect.right() - cr.right(), 
    bottomFrameWidth = opt.rect.bottom() - cr.bottom();
    frameWidth = qMax(qMax(leftFrameWidth, rightFrameWidth), 
                      qMax(topFrameWidth, bottomFrameWidth));
}

/*!
  \internal
  Updated the frameWidth parameter.
*/

void Q3GroupBoxPrivate::updateFrameWidth()
{
    QRect fr = q->frameRect();

    int frameShape  = frameStyle & QFrame::Shape_Mask;
    int frameShadow = frameStyle & QFrame::Shadow_Mask;

    frameWidth = -1;

    switch (frameShape)  {

    case QFrame::NoFrame:
        frameWidth = 0;
        break;

    case QFrame::Box:
    case QFrame::HLine:
    case QFrame::VLine:
        switch (frameShadow) {
    case QFrame::Plain:
        frameWidth = lineWidth;
        break;
    case QFrame::Raised:
    case QFrame::Sunken:
        frameWidth = (short)(lineWidth*2 + midLineWidth);
        break;
        }
        break;

    case QFrame::StyledPanel:
        updateStyledFrameWidths();
        break;

    case QFrame::WinPanel:
        frameWidth = 2;
        break;


    case QFrame::Panel:
        switch (frameShadow) {
    case QFrame::Plain:
    case QFrame::Raised:
    case QFrame::Sunken:
        frameWidth = lineWidth;
        break;
        }
        break;
    }

    if (frameWidth == -1)                                // invalid style
        frameWidth = 0;

    q->setFrameRect(fr);
}





/*!
    Constructs a group box widget with no title.

    The \a parent and \a name arguments are passed to the QWidget
    constructor.

    This constructor does not do automatic layout.
*/

Q3GroupBox::Q3GroupBox(QWidget *parent, const char *name)
    : QGroupBox(parent, name)
{
    init();
}

/*!
    Constructs a group box with the title \a title.

    The \a parent and \a name arguments are passed to the QWidget
    constructor.

    This constructor does not do automatic layout.
*/

Q3GroupBox::Q3GroupBox(const QString &title, QWidget *parent, const char *name)
    : QGroupBox(parent, name)
{
    init();
    setTitle(title);
}

/*!
    Constructs a group box with no title. Child widgets will be
    arranged in \a strips rows or columns (depending on \a
    orientation).

    The \a parent and \a name arguments are passed to the QWidget
    constructor.
*/

Q3GroupBox::Q3GroupBox(int strips, Qt::Orientation orientation,
                    QWidget *parent, const char *name)
    : QGroupBox(parent, name)
{
    init();
    setColumnLayout(strips, orientation);
}

/*!
    Constructs a group box titled \a title. Child widgets will be
    arranged in \a strips rows or columns (depending on \a
    orientation).

    The \a parent and \a name arguments are passed to the QWidget
    constructor.
*/

Q3GroupBox::Q3GroupBox(int strips, Qt::Orientation orientation,
                    const QString &title, QWidget *parent,
                    const char *name)
    : QGroupBox(parent, name)
{
    init();
    setTitle(title);
    setColumnLayout(strips, orientation);
}

/*!
    Destroys the group box.
*/
Q3GroupBox::~Q3GroupBox()
{
    delete d;
}

void Q3GroupBox::init()
{
    d = new Q3GroupBoxPrivate(this);
}


/*! \reimp
*/
void Q3GroupBox::resizeEvent(QResizeEvent *e)
{
    QGroupBox::resizeEvent(e);
}


/*!
    Adds an empty cell at the next free position. If \a size is
    greater than 0, the empty cell takes \a size to be its fixed width
    (if orientation() is \c Horizontal) or height (if orientation() is
    \c Vertical).

    Use this method to separate the widgets in the group box or to
    skip the next free cell. For performance reasons, call this method
    after calling setColumnLayout() or by changing the \l
    Q3GroupBox::columns or \l Q3GroupBox::orientation properties. It is
    generally a good idea to call these methods first (if needed at
    all), and insert the widgets and spaces afterwards.
*/
void Q3GroupBox::addSpace(int size)
{
    QApplication::sendPostedEvents(this, QEvent::ChildInserted);

    if (d->nCols <= 0 || d->nRows <= 0)
        return;

    if (d->row >= d->nRows || d->col >= d->nCols)
        d->grid->expand(d->row+1, d->col+1);

    if (size > 0) {
        QSpacerItem *spacer
            = new QSpacerItem((d->dir == Qt::Horizontal) ? 0 : size,
                              (d->dir == Qt::Vertical) ? 0 : size,
                              QSizePolicy::Fixed, QSizePolicy::Fixed);
        d->grid->addItem(spacer, d->row, d->col);
    }

    skip();
}

/*!
    \property Q3GroupBox::columns
    \brief the number of columns or rows (depending on \l Q3GroupBox::orientation) in the group box

    Usually it is not a good idea to set this property because it is
    slow (it does a complete layout). It is best to set the number
    of columns directly in the constructor.
*/
int Q3GroupBox::columns() const
{
    if (d->dir == Qt::Horizontal)
        return d->nCols;
    return d->nRows;
}

void Q3GroupBox::setColumns(int c)
{
    setColumnLayout(c, d->dir);
}

/*!
    Returns the width of the empty space between the items in the
    group and the frame of the group.

    Only applies if the group box has a defined orientation.

    The default is usually 11, by may vary depending on the platform
    and style.

    \sa setInsideMargin(), orientation
*/
int Q3GroupBox::insideMargin() const
{
    return d->marg;
}

/*!
    Returns the width of the empty space between each of the items
    in the group.

    Only applies if the group box has a defined orientation.

    The default is usually 5, by may vary depending on the platform
    and style.

    \sa setInsideSpacing(), orientation
*/
int Q3GroupBox::insideSpacing() const
{
    return d->spac;
}

/*!
    Sets the the width of the inside margin to \a m pixels.

    \sa insideMargin()
*/
void Q3GroupBox::setInsideMargin(int m)
{
    d->marg = m;
    setColumnLayout(columns(), d->dir);
}

/*!
    Sets the width of the empty space between each of the items in
    the group to \a s pixels.

    \sa insideSpacing()
*/
void Q3GroupBox::setInsideSpacing(int s)
{
    d->spac = s;
    setColumnLayout(columns(), d->dir);
}

/*!
    \property Q3GroupBox::orientation
    \brief the group box's orientation

    A horizontal group box arranges its children in columns, while a
    vertical group box arranges them in rows.

    Usually it is not a good idea to set this property because it is
    slow (it does a complete layout). It is better to set the
    orientation directly in the constructor.
*/
void Q3GroupBox::setOrientation(Qt::Orientation o)
{
    setColumnLayout(columns(), o);
}


Qt::Orientation Q3GroupBox::orientation() const
{
    return d->dir;
}

/*!
    Changes the layout of the group box. This function is only useful
    in combination with the default constructor that does not take any
    layout information. This function will put all existing children
    in the new layout. It is not good Qt programming style to call
    this function after children have been inserted. Sets the number
    of columns or rows to be \a strips, depending on \a direction.

    \sa orientation columns
*/
void Q3GroupBox::setColumnLayout(int strips, Qt::Orientation direction)
{
    if (layout())
        delete layout();

    d->vbox = 0;
    d->grid = 0;

    if (strips < 0) // if 0, we create the d->vbox but not the d->grid. See below.
        return;

    d->vbox = new QVBoxLayout(this, d->marg, 0);

    d->nCols = 0;
    d->nRows = 0;
    d->dir = direction;

    // Send all child events and ignore them. Otherwise we will end up
    // with doubled insertion. This won't do anything because d->nCols ==
    // d->nRows == 0.
    QApplication::sendPostedEvents(this, QEvent::ChildInserted);

    // if 0 or smaller , create a vbox-layout but no grid. This allows
    // the designer to handle its own grid layout in a group box.
    if (strips <= 0)
        return;

    d->dir = direction;
    if (d->dir == Qt::Horizontal) {
        d->nCols = strips;
        d->nRows = 1;
    } else {
        d->nCols = 1;
        d->nRows = strips;
    }
    d->grid = new QGridLayout(d->nRows, d->nCols, d->spac);
    d->row = d->col = 0;
    d->grid->setAlignment(Qt::AlignTop);
    d->vbox->addLayout(d->grid);

    // Add all children
    QObjectList childList = children();
    if (!childList.isEmpty()) {
        for (int i = 0; i < childList.size(); ++i) {
            QObject *o = childList.at(i);
            if (o->isWidgetType() && o != d->checkbox)
                insertWid(static_cast<QWidget *>(o));
        }
    }
}

/*!\reimp */
void Q3GroupBox::childEvent(QChildEvent *c)
{
    QGroupBox::childEvent(c);
    if (!c->inserted() || !c->child()->isWidgetType())
        return;
    if (d->grid) {
        insertWid((QWidget*)c->child());
    }
}

void Q3GroupBox::insertWid(QWidget* w)
{
    if (d->row >= d->nRows || d->col >= d->nCols)
        d->grid->expand(d->row+1, d->col+1);
    d->grid->addWidget(w, d->row, d->col);
    skip();
}


void Q3GroupBox::skip()
{
    // Same as QGrid::skip()
    if (d->dir == Qt::Horizontal) {
        if (d->col+1 < d->nCols) {
            d->col++;
        } else {
            d->col = 0;
            d->row++;
        }
    } else { //Vertical
        if (d->row+1 < d->nRows) {
            d->row++;
        } else {
            d->row = 0;
            d->col++;
        }
    }
}


/*! \reimp */
void Q3GroupBox::changeEvent(QEvent *ev)
{
    QGroupBox::changeEvent(ev);
}

/*! \reimp */
bool Q3GroupBox::event(QEvent *e)
{
    if (e->type()==QEvent::Paint)
    {
        QStyleOptionGroupBox opt;
        initStyleOption(&opt);
        opt.lineWidth=d->lineWidth;
        opt.midLineWidth=d->midLineWidth;
        QPainter p(this);
        if (frameShape()==GroupBoxPanel)
        {
            style()->drawComplexControl(QStyle::CC_GroupBox, &opt, &p, this);
        }
        else {
            //in case it is a Paint event with a frame shape different from the group box
            const QRect textRect = style()->subControlRect(QStyle::CC_GroupBox, &opt, QStyle::SC_GroupBoxLabel, this);
            const QRect checkBoxRect = style()->subControlRect(QStyle::CC_GroupBox, &opt, QStyle::SC_GroupBoxCheckBox, this);

            // Draw title
            if ((opt.subControls & QStyle::SC_GroupBoxLabel) && !opt.text.isEmpty()) {
                QColor textColor = opt.textColor;
                if (textColor.isValid())
                    p.setPen(textColor);
                int alignment = int(opt.textAlignment);
                if (!style()->styleHint(QStyle::SH_UnderlineShortcut, &opt, this))
                    alignment |= Qt::TextHideMnemonic;

                style()->drawItemText(&p, textRect,  Qt::TextShowMnemonic | Qt::AlignHCenter | alignment,
                    opt.palette, opt.state & QStyle::State_Enabled, opt.text,
                    textColor.isValid() ? QPalette::NoRole : QPalette::WindowText);

                if (opt.state & QStyle::State_HasFocus) {
                    QStyleOptionFocusRect fropt;
                    fropt.QStyleOption::operator=(opt);
                    fropt.rect = textRect;
                    style()->drawPrimitive(QStyle::PE_FrameFocusRect, &fropt, &p, this);
                }
            }

            // Draw checkbox
            if (opt.subControls & QStyle::SC_GroupBoxCheckBox) {
                QStyleOptionButton box;
                box.QStyleOption::operator=(opt);
                box.rect = checkBoxRect;
                style()->drawPrimitive(QStyle::PE_IndicatorCheckBox, &box, &p, this);
            }

            //sets clipping
            QRegion region(rect());
            if (!title().isEmpty()) {
                bool ltr = layoutDirection() == Qt::LeftToRight;
                QRect finalRect = checkBoxRect.united(textRect);
                if (isCheckable())
                    finalRect.adjust(ltr ? -4 : 0, 0, ltr ? 0 : 4, 0);
                region -= finalRect;
            }
            p.setClipRegion(region);

            drawFrame(&p);
        }
        return false;
    }
    return QGroupBox::event(e);
}

/*!
    \fn void Q3GroupBox::drawFrame(QPainter *p)
    \internal
*/

void Q3GroupBox::drawFrame(QPainter *p)
{
    QPoint      p1, p2;
    QStyleOptionFrame opt;
    opt.init(this);

    int frameShape  = d->frameStyle & QFrame::Shape_Mask;
    int frameShadow = d->frameStyle & QFrame::Shadow_Mask;

    int lw = 0;
    int mlw = 0;
    opt.rect = frameRect();

    switch (frameShape) {
    case QFrame::Box:
    case QFrame::HLine:
    case QFrame::VLine:
    case QFrame::StyledPanel:
        lw = d->lineWidth;
        mlw = d->midLineWidth;
        break;
    default:
        // most frame styles do not handle customized line and midline widths
        // (see updateFrameWidth()).
        lw = d->frameWidth;
        break;
    }
    opt.lineWidth = lw;
    opt.midLineWidth = mlw;
    if (frameShadow == Sunken)
        opt.state |= QStyle::State_Sunken;
    else if (frameShadow == Raised)
        opt.state |= QStyle::State_Raised;

    switch (frameShape) {
    case Box:
        if (frameShadow == Plain)
            qDrawPlainRect(p, opt.rect, opt.palette.foreground().color(), lw);
        else
            qDrawShadeRect(p, opt.rect, opt.palette, frameShadow == Sunken, lw, mlw);
        break;

    case StyledPanel:
        style()->drawPrimitive(QStyle::PE_Frame, &opt, p, this);
        break;

    case Panel:
        if (frameShadow == Plain)
            qDrawPlainRect(p, opt.rect, opt.palette.foreground().color(), lw);
        else
            qDrawShadePanel(p, opt.rect, opt.palette, frameShadow == Sunken, lw);
        break;

    case WinPanel:
        if (frameShadow == Plain)
            qDrawPlainRect(p, opt.rect, opt.palette.foreground().color(), lw);
        else
            qDrawWinPanel(p, opt.rect, opt.palette, frameShadow == Sunken);
        break;
    case HLine:
    case VLine:
        if (frameShape == HLine) {
            p1 = QPoint(opt.rect.x(), opt.rect.height() / 2);
            p2 = QPoint(opt.rect.x() + opt.rect.width(), p1.y());
        } else {
            p1 = QPoint(opt.rect.x()+opt.rect.width() / 2, 0);
            p2 = QPoint(p1.x(), opt.rect.height());
        }
        if (frameShadow == Plain) {
            QPen oldPen = p->pen();
            p->setPen(QPen(opt.palette.foreground().color(), lw));
            p->drawLine(p1, p2);
            p->setPen(oldPen);
        } else {
            qDrawShadeLine(p, p1, p2, opt.palette, frameShadow == Sunken, lw, mlw);
        }
        break;
    }

#ifdef QT_KEYPAD_NAVIGATION
    if (QApplication::keypadNavigationEnabled() && hasFocus()) {
        QStyleOptionFocusRect fopt;
        fopt.init(this);
        fopt.state |= QStyle::State_KeyboardFocusChange;
        fopt.rect = frameRect();
        style()->drawPrimitive(QStyle::PE_FrameFocusRect, &fopt, p, this);
    }
#endif
}

/*!
    \property Q3GroupBox::frameShadow
    \brief the frame shadow value from the frame style

    \sa frameStyle() 
*/

/*
    \enum Q3GroupBox::FrameShape
    
    This enum defines the available frame shapes a group box can
    have. All values have equivalents in QFrame.

    \value Box QFrame::Box
    \value Sunken QFrame::Sunken
    \value Plain QFrame::Plain
    \value Raised QFrame::Raised
    \value MShadow QFrame::Shadow_Mask
    \value NoFrame QFrame::NoFrame
    \value Panel QFrame::Panel
    \value StyledPanel QFrame::StyledPanel
    \value HLine QFrame::HLine
    \value VLine QFrame::VLine
    \value WinPanel QFrame::WinPanel
    \value ToolBarPanel QFrame::StyledPanel
    \value MenuBarPanel = QFrame::StyledPanel
    \value PopupPanel QFrame::StyledPanel
    \value LineEditPanel QFrame::StyledPanel
    \value TabWidgetPanel QFrame::StyledPanel
    \value GroupBoxPanel 0x0007
    \value MShape QFrame::Shape_Mask
*/


void Q3GroupBox::setFrameShadow(DummyFrame s)
{
    setFrameStyle((d->frameStyle & MShape) | s);
}

Q3GroupBox::DummyFrame Q3GroupBox::frameShadow() const
{
    return (DummyFrame) (d->frameStyle & MShadow);
}

/*!
    \property Q3GroupBox::frameShape
    \brief the frame shape value from the frame style

    \sa frameStyle(), frameShadow()
*/

void Q3GroupBox::setFrameShape(DummyFrame s)
{
    setFrameStyle((d->frameStyle & MShadow) | s);
}

Q3GroupBox::DummyFrame Q3GroupBox::frameShape() const
{
    return (DummyFrame) (d->frameStyle & MShape);
}

/*!
    \fn void Q3GroupBox::setFrameStyle(int style)
    
    Sets the frame style to \a style.  The style is the bitwise OR
    between a frame shape and a frame shadow style.
*/

void Q3GroupBox::setFrameStyle(int style)
{
    if (!testAttribute(Qt::WA_WState_OwnSizePolicy)) {
        switch (style & MShape) {
        case HLine:
            setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
            break;
        case VLine:
            setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
            break;
        default:
            if ((d->frameStyle & MShape) == HLine || (d->frameStyle & MShape) == VLine)
                setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        }
        setAttribute(Qt::WA_WState_OwnSizePolicy, false);
    }
    d->frameStyle = style;
    update();
    d->updateFrameWidth();
    d->oldFrameStyle = style;
}

/*!
    \fn int Q3GroupBox::frameStyle() const
    
    Returns the frame style.
*/

int Q3GroupBox::frameStyle() const
{
    return d->frameStyle;
}

/*!
    \property Q3GroupBox::lineWidth
    \brief This property holds the width of the line.

    \sa frameStyle(), frameShadow()
*/

void Q3GroupBox::setLineWidth(int w) 
{
    if (short(w) == d->lineWidth)
        return;
    d->lineWidth = short(w);
    d->updateFrameWidth();
}

int Q3GroupBox::lineWidth() const
{
    return d->lineWidth;
}    

/*!
    \property Q3GroupBox::midLineWidth
    \brief This property holds the width of the mid-line.

    \sa frameStyle(), frameShadow()
*/

void Q3GroupBox::setMidLineWidth(int w)
{
    if (short(w) == d->midLineWidth)
        return;
    d->midLineWidth = short(w);
    d->updateFrameWidth();
}

int Q3GroupBox::midLineWidth() const
{
    return d->midLineWidth;
}

/*!
    \property Q3GroupBox::frameRect
    \brief the bounding rectangle of the frame of the group box.
*/

/*!
    \fn QRect Q3GroupBox::frameRect() const
    \internal
*/

QRect Q3GroupBox::frameRect() const
{
    QStyleOptionGroupBox opt;
    initStyleOption(&opt);
    QRect fr = style()->subControlRect(QStyle::CC_GroupBox, &opt, QStyle::SC_GroupBoxFrame, this);
    return fr;
}

/*!
    \fn void Q3GroupBox::setFrameRect(QRect)
    \internal
*/

void Q3GroupBox::setFrameRect(QRect r)
{
    QRect cr = r.isValid() ? r : rect();
    if ((d->frameStyle & QFrame::Shape_Mask) == StyledPanel) {
        cr.adjust(d->leftFrameWidth, d->topFrameWidth, -d->rightFrameWidth, -d->bottomFrameWidth);
    } else
        cr.adjust(d->frameWidth, d->frameWidth, -d->frameWidth, -d->frameWidth);
    setContentsMargins(cr.left(), cr.top(), rect().right() - cr.right(), rect().bottom() - cr.bottom());
}

/*!
    \fn int Q3GroupBox::frameWidth() const
    \internal
*/

int Q3GroupBox::frameWidth() const
{
    return d->frameWidth;
}

#if defined(Q_MOC_RUN)
/*!
    \enum Q3GroupBox::FrameShape
    \internal

    \value Box
    \value Sunken
    \value Plain
    \value Raised
    \value MShadow
    \value NoFrame
    \value Panel 
    \value StyledPanel
    \value HLine 
    \value VLine 
    \value GroupBoxPanel
    \value WinPanel 
    \value ToolBarPanel 
    \value MenuBarPanel 
    \value PopupPanel 
    \value LineEditPanel 
    \value TabWidgetPanel 
    \value MShape
*/
#else
/*!
    \enum Q3GroupBox::DummyFrame
    \internal

    \value Box
    \value Sunken
    \value Plain
    \value Raised
    \value MShadow
    \value NoFrame
    \value Panel 
    \value StyledPanel
    \value HLine 
    \value VLine 
    \value GroupBoxPanel
    \value WinPanel 
    \value ToolBarPanel 
    \value MenuBarPanel 
    \value PopupPanel 
    \value LineEditPanel 
    \value TabWidgetPanel 
    \value MShape
*/
#endif

/*!
    \typedef Q3GroupBox::FrameShape
    \internal 
*/

/*!
    \property Q3GroupBox::margin
    \brief the width of the margin around the contents of the
           group box.
*/

/*!
    \fn void Q3GroupBox::setMargin(int margin)
    \since 4.2

    Sets the width of the margin around the contents of the widget to \a margin.
    
    This function uses QWidget::setContentsMargins() to set the margin.
    \sa margin(), QWidget::setContentsMargins()
*/

/*!
    \fn int Q3GroupBox::margin() const 
    \since 4.2

    Returns the width of the the margin around the contents of the widget.
    
    This function uses QWidget::getContentsMargins() to get the margin.

    \sa setMargin(), QWidget::getContentsMargins()
*/

QT_END_NAMESPACE
