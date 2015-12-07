/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#include "qwindowsmobilestyle.h"
#include "qwindowsmobilestyle_p.h"

#if !defined(QT_NO_STYLE_WINDOWSMOBILE) || defined(QT_PLUGIN)

#include "qpainterpath.h"
#include "qapplication.h"
#include "qdesktopwidget.h"
#include "qwidget.h"
#include "qdockwidget.h"
#include "qframe.h"
#include "qmenu.h"
#include "qpaintengine.h"
#include "qpainter.h"
#include "qgroupbox.h"
#include "qstyleoption.h"
#include "qlistview.h"
#include "qdrawutil.h"
#include "qtoolbar.h"
#include "qabstractscrollarea.h"
#include "qabstractbutton.h"
#include "qcombobox.h"
#include "qabstractscrollarea.h"
#include "qframe.h"
#include "qscrollbar.h"
#include "qabstractitemview.h"
#include "qmenubar.h"
#include "qtoolbutton.h"
#include "qtextedit.h"
#include "qdialog.h"
#include "qdebug.h"
#include "qtabwidget.h"

#ifdef Q_OS_WINCE
#include "qt_windows.h"
#include "qguifunctions_wince.h"
extern bool qt_wince_is_high_dpi();   //defined in qguifunctions_wince.cpp
extern bool qt_wince_is_smartphone(); //defined in qguifunctions_wince.cpp
#endif // Q_OS_WINCE

QT_BEGIN_NAMESPACE

static const int windowsItemFrame        =  1; // menu item frame width
static const int windowsItemHMargin      =  2; // menu item hor text margin
static const int windowsItemVMargin      =  2; // menu item ver text margin
static const int windowsArrowHMargin	 =  6; // arrow horizontal margin
static const int windowsRightBorder      = 15; // right border on windows
static const int windowsCheckMarkWidth   = 14; // checkmarks width on windows

static const int windowsMobileitemViewCheckBoxSize   = 13;
static const int windowsMobileFrameGroupBoxOffset    = 9;
static const int windowsMobileIndicatorSize          = 14;
static const int windowsMobileExclusiveIndicatorSize = 14;
static const int windowsMobileSliderThickness        = 6;
static const int windowsMobileIconSize               = 16;
static const int PE_IndicatorArrowUpBig         = 0xf000101;
static const int PE_IndicatorArrowDownBig       = 0xf000102;  
static const int PE_IndicatorArrowLeftBig       = 0xf000103;
static const int PE_IndicatorArrowRightBig      = 0xf000104;

/* XPM */
static const char *const radiobutton_xpm[] = {
    "30 30 2 1",
    " 	c None",
    ".	c #000000",
    "           ........           ",
    "        ..............        ",
    "       ....        ....       ",
    "     ....            ....     ",
    "    ...                ...    ",
    "   ...                  ...   ",
    "   ..                    ..   ",
    "  ..                      ..  ",
    " ...                      ... ",
    " ..                        .. ",
    " ..                        .. ",
    "..                          ..",
    "..                          ..",
    "..                          ..",
    "..                          ..",
    "..                          ..",
    "..                          ..",
    "..                          ..",
    "..                          ..",
    " ..                        .. ",
    " ..                        .. ",
    " ...                      ... ",
    "  ..                      ..  ",
    "   ..                    ..   ",
    "   ...                  ...   ",
    "    ...                ...    ",
    "     ....            ....     ",
    "       ....        ....       ",
    "        ..............        ",
    "           ........           "};

/* XPM */
static const char * const radiobutton_low_xpm[] = {
    "15 15 2 1",
    " 	c None",
    ".	c #000000",
    "     .....     ",
    "   ..     ..   ",
    "  .         .  ",
    " .           . ",
    " .           . ",
    ".             .",
    ".             .",
    ".             .",
    ".             .",
    ".             .",
    " .           . ",
    " .           . ",
    "  .         .  ",
    "   ..     ..   ",
    "     .....     "};

/* XPM */
    static const char * const arrowleft_big_xpm[] = {
        "9 17 2 1",
        "  c None",
        ". c #000000",
        "        .",
        "       ..",
        "      ...",
        "     ....",
        "    .....",
        "   ......",
        "  .......",
        " ........",
        ".........",
        " ........",
        "  .......",
        "   ......",
        "    .....",
        "     ....",
        "      ...",
        "       ..",
        "        ."};

/* XPM */
     static const char * const arrowleft_xpm[] = {
        "8 15 2 1",
        "  c None",
        ". c #000000",
        "       .",
        "      ..",
        "     ...",
        "    ....",
        "   .....",
        "  ......",
        " .......",
        "........",
        " .......",
        "  ......",
        "   .....",
        "    ....",
        "     ...",
        "      ..",
        "       ."};



/* XPM */
static const char *const horlines_xpm[] = {
    "2 2 2 1",
    "  c None",
    ". c #000000",
    "  ",
    ".."};

/* XPM */
static const char *const vertlines_xpm[] = {
    "2 2 2 1",
    "  c None",
    ". c #000000",
    ". ",
    ". "};

/* XPM */
static const char *const radiochecked_xpm[] = {
    "18 18 2 1",
    " 	c None",
    ".	c #000000",
    "      ......      ",
    "    ..........    ",
    "  ..............  ",
    "  ..............  ",
    " ................ ",
    " ................ ",
    "..................",
    "..................",
    "..................",
    "..................",
    "..................",
    "..................",
    " ................ ",
    " ................ ",
    "  ..............  ",
    "  ..............  ",
    "    ..........    ",
    "      ......      "};

/* XPM */
static const char * const radiochecked_low_xpm[] = {
    "9 9 2 1",
    "         c None",
    ".        c #000000", 
    "   ...   ",
    " ....... ",
    " ....... ",
    ".........",
    ".........",
    ".........",
    " ....... ",
    " ....... ",
    "   ...   "};

static const char *const arrowdown_xpm[] = {
    "15 8 2 1",
    " 	c None",
    ".	c #000000",
    "...............",
    " ............. ",
    "  ...........  ",
    "   .........   ",
    "    .......    ",
    "     .....     ",
    "      ...      ",
    "       .       "};


static const char *const arrowdown_big_xpm[] = {
    "17 9 2 1",
    " 	c None",
    ".	c #000000",
    ".................",
    " ............... ",
    "  .............  ",
    "   ...........   ",
    "    .........    ",
    "     .......     ",
    "      .....      ",
    "       ...       ",
    "        .        "};


/* XPM */
static const char *const checkedlight_xpm[] = {
    "24 24 2 1",
    " 	c None",
    ".	c #000000",
    "                        ",
    "                        ",
    "                        ",
    "                        ",
    "                        ",
    "                  .     ",
    "                 ..     ",
    "                ...     ",
    "               ....     ",
    "              .....     ",
    "             ......     ",
    "    .       ......      ",
    "    ..     ......       ",
    "    ...   ......        ",
    "    .... ......         ",
    "    ..........          ",
    "    .........           ",
    "     .......            ",
    "      .....             ",
    "       ...              ",
    "        .               ",
    "                        ",
    "                        ",
    "                        "};


/* XPM */
static const char *const checkedbold_xpm[] = {
    "26 26 2 1",
    " 	c None",
    ".	c #000000",
    "                          ",
    "                          ",
    "                          ",
    "                          ",
    "                          ",
    "                          ",
    "                    ..    ",
    "                   ...    ",
    "                  ....    ",
    "                 .....    ",
    "    ..          ......    ",
    "    ...        .......    ",
    "    ....      .......     ",
    "    .....    .......      ",
    "    ......  .......       ",
    "    ..............        ",
    "     ............         ",
    "      ..........          ",
    "       ........           ",
    "        ......            ",
    "         ....             ",
    "          ..              ",
    "                          ",
    "                          ",
    "                          ",
    "                          "};

/* XPM */
static const char * const checkedbold_low_xpm[] = {
    "9 8 2 1",
    " 	c None",
    ".	c #000000",
    "        .",
    "       ..",
    ".     ...",
    "..   ... ",
    "... ...  ",
    " .....   ",
    "  ...    ",
    "   .     "};

/* XPM */
static const char * const checkedlight_low_xpm[] = {
    "8 8 2 1",
    " 	c None",
    ".	c #000000",
    "       .",
    "      ..",
    "     ...",
    ".   ... ",
    ".. ...  ",
    ".....   ",
    " ...    ",
    "  .     "};

/* XPM */
static const char * const highlightedradiobutton_xpm[] = {
    "30 30 3 1",
    " 	c None",
    ".	c #000000",
    "+	c #0078CC",
    "           ........           ",
    "        ..............        ",
    "       ....++++++++....       ",
    "     ....++++++++++++....     ",
    "    ...++++        ++++...    ",
    "   ...+++            +++...   ",
    "   ..++                ++..   ",
    "  ..++                  ++..  ",
    " ...++                  ++... ",
    " ..++                    ++.. ",
    " ..++                    ++.. ",
    "..++                      ++..",
    "..++                      ++..",
    "..++                      ++..",
    "..++                      ++..",
    "..++                      ++..",
    "..++                      ++..",
    "..++                      ++..",
    "..++                      ++..",
    " ..++                    ++.. ",
    " ..++                    ++.. ",
    " ...++                  ++... ",
    "  ..++                  ++..  ",
    "   ..++                ++..   ",
    "   ...+++            +++...   ",
    "    ...++++        ++++...    ",
    "     ....++++++++++++....     ",
    "       ....++++++++....       ",
    "        ..............        ",
    "           ........           "};

/* XPM */
static const char * const highlightedradiobutton_low_xpm[] = {
    "15 15 3 1",
    " 	c None",
    ".	c #000000",
    "+	c #3192D6",
    "     .....     ",
    "   ..+++++..   ",
    "  .++     ++.  ",
    " .+         +. ",
    " .+         +. ",
    ".+           +.",
    ".+           +.",
    ".+           +.",
    ".+           +.",
    ".+           +.",
    " .+         +. ",
    " .+         +. ",
    "  .++     ++.  ",
    "   ..+++++..   ",
    "     .....     "};

/* XPM */
static const char * const cross_big_xpm[] = {
"28 28 4 1",
" 	c #09454A",
".	c #218C98",
"+	c #47D8E5",
"@	c #FDFFFC",
"                            ",
"                            ",
"  ++++++++++++++++++++++++  ",
"  ++++++++++++++++++++++++  ",
"  ++....................++  ",
"  ++....................++  ",
"  ++..@@@..........@@@..++  ",
"  ++..@@@@........@@@@..++  ",
"  ++..@@@@@......@@@@@..++  ",
"  ++...@@@@@....@@@@@...++  ",
"  ++....@@@@@..@@@@@....++  ",
"  ++.....@@@@@@@@@@.....++  ",
"  ++......@@@@@@@@......++  ",
"  ++.......@@@@@@.......++  ",
"  ++.......@@@@@@.......++  ",
"  ++......@@@@@@@@......++  ",
"  ++.....@@@@@@@@@@.....++  ",
"  ++....@@@@@..@@@@@....++  ",
"  ++...@@@@@....@@@@@...++  ",
"  ++..@@@@@......@@@@@..++  ",
"  ++..@@@@........@@@@..++  ",
"  ++..@@@..........@@@..++  ",
"  ++....................++  ",
"  ++....................++  ",
"  ++++++++++++++++++++++++  ",
"  ++++++++++++++++++++++++  ",
"                            ",
"                            "};

/* XPM */
static const char * const cross_small_xpm[] = {
"14 14 4 1",
" 	c #09454A",
".	c #218C98",
"+	c #47D8E5",
"@	c #FCFFFC",
"              ",
" ++++++++++++ ",
" +..........+ ",
" +.@@....@@.+ ",
" +.@@@..@@@.+ ",
" +..@@@@@@..+ ",
" +...@@@@...+ ",
" +...@@@@...+ ",
" +..@@@@@@..+ ",
" +.@@@..@@@.+ ",
" +.@@....@@.+ ",
" +..........+ ",
" ++++++++++++ ",
"              "};

/* XPM */
static const char * const max_big_xpm[] = {
"28 28 4 1",
" 	c #09454A",
".	c #218C98",
"+	c #47D8E5",
"@	c #FDFFFC",
"                            ",
"                            ",
"  ++++++++++++++++++++++++  ",
"  ++++++++++++++++++++++++  ",
"  ++....................++  ",
"  ++....................++  ",
"  ++....................++  ",
"  ++....................++  ",
"  ++..@@@@@@@@@@@@@@@@..++  ",
"  ++..@@@@@@@@@@@@@@@@..++  ",
"  ++..@@@@@@@@@@@@@@@@..++  ",
"  ++..@@@@@@@@@@@@@@@@..++  ",
"  ++..@@............@@..++  ",
"  ++..@@............@@..++  ",
"  ++..@@............@@..++  ",
"  ++..@@............@@..++  ",
"  ++..@@............@@..++  ",
"  ++..@@............@@..++  ",
"  ++..@@@@@@@@@@@@@@@@..++  ",
"  ++..@@@@@@@@@@@@@@@@..++  ",
"  ++....................++  ",
"  ++....................++  ",
"  ++....................++  ",
"  ++....................++  ",
"  ++++++++++++++++++++++++  ",
"  ++++++++++++++++++++++++  ",
"                            ",
"                            "};

/* XPM */
static const char * const max_small_xpm[] = {
"14 14 4 1",
" 	c #09454A",
".	c #218C98",
"+	c #47D8E5",
"@	c #FCFFFC",
"              ",
" ++++++++++++ ",
" +..........+ ",
" +..........+ ",
" +.@@@@@@@@.+ ",
" +.@@@@@@@@.+ ",
" +.@......@.+ ",
" +.@......@.+ ",
" +.@......@.+ ",
" +.@@@@@@@@.+ ",
" +..........+ ",
" +..........+ ",
" ++++++++++++ ",
"              "};

/* XPM */
static const char * const normal_big_xpm[] = {
"28 28 4 1",
" 	c #09454A",
".	c #218C98",
"+	c #47D8E5",
"@	c #FDFFFC",
"                            ",
"                            ",
"  ++++++++++++++++++++++++  ",
"  ++++++++++++++++++++++++  ",
"  ++....................++  ",
"  ++....................++  ",
"  ++..@@@@@@@@@@@@@@@@..++  ",
"  ++..@@@@@@@@@@@@@@@@..++  ",
"  ++..@@............@@..++  ",
"  ++..@@............@@..++  ",
"  ++..@@............@@..++  ",
"  ++..@@............@@..++  ",
"  ++..@@............@@..++  ",
"  ++..@@............@@..++  ",
"  ++..@@............@@..++  ",
"  ++..@@............@@..++  ",
"  ++..@@............@@..++  ",
"  ++..@@............@@..++  ",
"  ++..@@............@@..++  ",
"  ++..@@............@@..++  ",
"  ++..@@@@@@@@@@@@@@@@..++  ",
"  ++..@@@@@@@@@@@@@@@@..++  ",
"  ++....................++  ",
"  ++....................++  ",
"  ++++++++++++++++++++++++  ",
"  ++++++++++++++++++++++++  ",
"                            ",
"                            "};

/* XPM */
static const char * const normal_small_xpm[] = {
"14 14 4 1",
" 	c #09454A",
".	c #218C98",
"+	c #47D8E5",
"@	c #FCFFFC",
"              ",
" ++++++++++++ ",
" +..........+ ",
" +.@@@@@@@@.+ ",
" +.@......@.+ ",
" +.@......@.+ ",
" +.@......@.+ ",
" +.@......@.+ ",
" +.@......@.+ ",
" +.@......@.+ ",
" +.@@@@@@@@.+ ",
" +..........+ ",
" ++++++++++++ ",
"              "};


/* XPM */
static const char * const min_big_xpm[] = {
"28 28 4 1",
" 	c #09454A",
".	c #218C98",
"+	c #47D8E5",
"@	c #FDFFFC",
"                            ",
"                            ",
"  ++++++++++++++++++++++++  ",
"  ++++++++++++++++++++++++  ",
"  ++....................++  ",
"  ++....................++  ",
"  ++....................++  ",
"  ++....................++  ",
"  ++....................++  ",
"  ++....................++  ",
"  ++....................++  ",
"  ++....................++  ",
"  ++....................++  ",
"  ++....................++  ",
"  ++....................++  ",
"  ++....................++  ",
"  ++....................++  ",
"  ++....................++  ",
"  ++..@@@@@@@@@@@@@@@@..++  ",
"  ++..@@@@@@@@@@@@@@@@..++  ",
"  ++....................++  ",
"  ++....................++  ",
"  ++....................++  ",
"  ++....................++  ",
"  ++++++++++++++++++++++++  ",
"  ++++++++++++++++++++++++  ",
"                            ",
"                            "};

/* XPM */
static const char * const min_small_xpm[] = {
"14 14 4 1",
" 	c #09454A",
".	c #218C98",
"+	c #47D8E5",
"@	c #FCFFFC",
"              ",
" ++++++++++++ ",
" +..........+ ",
" +..........+ ",
" +..........+ ",
" +..........+ ",
" +..........+ ",
" +..........+ ",
" +..........+ ",
" +.@@@@@@@@.+ ",
" +..........+ ",
" +..........+ ",
" ++++++++++++ ",
"              "};



enum QSliderDirection { SliderUp, SliderDown, SliderLeft, SliderRight };

QWindowsMobileStyle::QWindowsMobileStyle(QWindowsMobileStylePrivate &dd) : QWindowsStyle(dd) {
    qApp->setEffectEnabled(Qt::UI_FadeMenu, false);
    qApp->setEffectEnabled(Qt::UI_AnimateMenu, false);
}

QWindowsMobileStyle::QWindowsMobileStyle() : QWindowsStyle(*new QWindowsMobileStylePrivate) {
    qApp->setEffectEnabled(Qt::UI_FadeMenu, false);
    qApp->setEffectEnabled(Qt::UI_AnimateMenu, false);
}

QWindowsMobileStylePrivate::QWindowsMobileStylePrivate() :QWindowsStylePrivate() {

#ifdef Q_OS_WINCE
   doubleControls = qt_wince_is_high_dpi();
   smartphone = qt_wince_is_smartphone();
#else
   doubleControls = false;
   smartphone = false;
#endif //Q_OS_WINCE

#ifndef QT_NO_IMAGEFORMAT_XPM

   imageArrowDown = QImage(arrowdown_xpm);
   imageArrowUp = QImage(arrowdown_xpm).mirrored();
   imageArrowLeft = QImage(arrowleft_xpm);
   imageArrowRight = QImage(arrowleft_xpm).mirrored(true, false);
   if (doubleControls) {
       imageRadioButton = QImage(radiobutton_xpm);
       imageRadioButtonChecked = QImage(radiochecked_xpm);
       imageChecked = QImage(checkedlight_xpm);
       imageCheckedBold = QImage(checkedbold_xpm);
       imageRadioButtonHighlighted = QImage(highlightedradiobutton_xpm);
       imageClose = QImage(cross_big_xpm);
       imageMaximize = QImage(max_big_xpm);
       imageMinimize = QImage(min_big_xpm);
       imageNormalize = QImage(normal_big_xpm);
   } else {
       imageRadioButton = QImage(radiobutton_low_xpm);
       imageRadioButtonChecked = QImage(radiochecked_low_xpm);
       imageChecked = QImage(checkedlight_low_xpm);
       imageCheckedBold = QImage(checkedbold_low_xpm);
       imageRadioButtonHighlighted = QImage(highlightedradiobutton_low_xpm);
       imageClose = QImage(cross_small_xpm);
       imageMaximize = QImage(max_small_xpm);
       imageMinimize = QImage(min_small_xpm);
       imageNormalize = QImage(normal_small_xpm);
   }

   imageArrowDownBig = QImage(arrowdown_big_xpm);
   imageArrowUpBig = QImage(arrowdown_big_xpm).mirrored();
   imageArrowLeftBig = QImage(arrowleft_big_xpm);
   imageArrowRightBig = QImage(arrowleft_big_xpm).mirrored(true, false);

#endif
}

void QWindowsMobileStyle::drawPrimitive(PrimitiveElement element, const QStyleOption *option,
                                    QPainter *painter, const QWidget *widget) const {

   QWindowsMobileStylePrivate *d = const_cast<QWindowsMobileStylePrivate*>(d_func());

    bool doRestore = false;
    QRect rect = option->rect;
    painter->setClipping(false);

    switch (element) {
    case PE_PanelButtonTool: {
        int penSize = 1;
        if (d->doubleControls)
            penSize = 2;
        if (widget)
            if (QWidget *parent = widget->parentWidget())
#ifndef QT_NO_TABWIDGET
                if (qobject_cast<QTabWidget *>(parent->parentWidget())) {
#else
                if (false) {
#endif //QT_NO_TABBAR
                    rect.adjust(0,2*penSize,0,-1*penSize);
                        qDrawPlainRect(painter, rect, option->palette.shadow().color(), penSize, &option->palette.light());
                    if (option->state & (State_Sunken))
                        qDrawPlainRect(painter, rect, option->palette.shadow().color(), penSize, &option->palette.shadow());
                }
                else {
                    if (!(option->state & State_AutoRaise) || (option->state & (State_Sunken | State_On)))
                        qDrawPlainRect(painter,option->rect.adjusted(0, penSize, 0, -1 * penSize) ,
                                       option->palette.button().color(), 0, &option->palette.button());
                    if (option->state & (State_Sunken)) {
                        qDrawPlainRect(painter, rect, option->palette.shadow().color(), penSize, &option->palette.light());
                    }
                    if (option->state & (State_On)){
                        QBrush fill = QBrush(option->palette.light().color());
                        painter->fillRect(rect.adjusted(windowsItemFrame , windowsItemFrame ,
                                          -windowsItemFrame , -windowsItemFrame ), fill);
                        qDrawPlainRect(painter, rect, option->palette.shadow().color(), penSize, &option->palette.light());
                    }
                }
                break; }
    case PE_IndicatorButtonDropDown:
        if (d->doubleControls)
          qDrawPlainRect(painter, option->rect, option->palette.shadow().color(), 2, &option->palette.button());
        else
          qDrawPlainRect(painter, option->rect, option->palette.shadow().color(), 1, &option->palette.button());
        break;
#ifndef QT_NO_TABBAR
    case PE_IndicatorTabTear:
        if (const QStyleOptionTab *tab = qstyleoption_cast<const QStyleOptionTab *>(option)) {
            bool rtl = tab->direction == Qt::RightToLeft;
            QRect rect = tab->rect;
            QPainterPath path;
            rect.setTop(rect.top() + ((tab->state & State_Selected) ? 1 : 3));
            rect.setBottom(rect.bottom() - ((tab->state & State_Selected) ? 0 : 2));
            path.moveTo(QPoint(rtl ? rect.right() : rect.left(), rect.top()));
            int count = 3;
            for(int jags = 1; jags <= count; ++jags, rtl = !rtl)
                path.lineTo(QPoint(rtl ? rect.left() : rect.right(), rect.top() + jags * rect.height()/count));
            painter->setPen(QPen(tab->palette.light(), qreal(.8)));
            painter->setBrush(tab->palette.background());
            painter->setRenderHint(QPainter::Antialiasing);
            painter->drawPath(path);
        }
        break;
#endif //QT_NO_TABBAR

#ifndef QT_NO_TOOLBAR
    case PE_IndicatorToolBarSeparator: {
            painter->save();
            QPoint p1, p2;
            if (option->state & State_Horizontal) {
                p1 = QPoint(option->rect.width()/2, 0);
                p2 = QPoint(p1.x(), option->rect.height());
            } else {
                p1 = QPoint(0, option->rect.height()/2);
                p2 = QPoint(option->rect.width(), p1.y());
            }


            painter->setPen(option->palette.mid().color());
            if (d->doubleControls) {
                QPen pen = painter->pen();
                pen.setWidth(2);
                pen.setCapStyle(Qt::FlatCap);
                painter->setPen(pen);
            }
            painter->drawLine(p1, p2);
            painter->restore();
            break; }
#endif // QT_NO_TOOLBAR
    case PE_IndicatorToolBarHandle:
        painter->save();
        painter->translate(option->rect.x(), option->rect.y());
        if (option->state & State_Horizontal) {
            int x = option->rect.width() / 2 - 4;
            if (QApplication::layoutDirection() == Qt::RightToLeft)
                x -= 2;
            if (option->rect.height() > 4) {
                qDrawWinButton(painter,x-1,0,7,option->rect.height(), option->palette, false,  0);

                qDrawShadePanel(painter, x, 1, 3, option->rect.height() - 1,
                    option->palette, false,  0);
                qDrawShadePanel(painter, x + 3, 1, 3, option->rect.height() - 1,
                    option->palette, false,  0);
                painter->setPen(option->palette.button().color());
            }
        } else {
            if (option->rect.width() > 4) {
                int y = option->rect.height() / 2 - 4;
                qDrawShadePanel(painter, 2, y, option->rect.width() - 2, 3,
                    option->palette, false,  0);
                qDrawShadePanel(painter, 2, y + 3, option->rect.width() - 2, 3,
                    option->palette, false,  0);
            }
        }
        painter->restore();
        break;

#ifndef QT_NO_PROGRESSBAR
    case PE_IndicatorProgressChunk: {
            bool vertical = false;
            if (const QStyleOptionProgressBarV2 *pb2 = qstyleoption_cast<const QStyleOptionProgressBarV2 *>(option))
                vertical = (pb2->orientation == Qt::Vertical);
            if (!vertical) {
                painter->fillRect(option->rect.x(), option->rect.y()+2, option->rect.width(), option->rect.height()-4,
                            option->palette.brush(QPalette::Highlight));
            } else {
                painter->fillRect(option->rect.x()+2, option->rect.y(), option->rect.width()-4, option->rect.height(),
                            option->palette.brush(QPalette::Highlight));
            }
        }
        break;
#endif // QT_NO_PROGRESSBAR

    case PE_FrameButtonTool: {
#ifndef QT_NO_DOCKWIDGET
            if (widget && widget->inherits("QDockWidgetTitleButton")) {
                if (const QDockWidget *dw = qobject_cast<const QDockWidget *>(widget->parent()))
                    if (dw->isFloating()){
                        qDrawPlainRect(painter,option->rect.adjusted(1, 1, 0, 0),
                                       option->palette.shadow().color(),1,&option->palette.button());
                        return;
                    }
            }
#endif // QT_NO_DOCKWIDGET
        QBrush fill;
        bool stippled;
        bool panel = (element == PE_PanelButtonTool);
        if ((!(option->state & State_Sunken ))
            && (!(option->state & State_Enabled)
            || ((option->state & State_Enabled ) && !(option->state & State_MouseOver)))
            && (option->state & State_On)) {
                fill = QBrush(option->palette.light().color(), Qt::Dense4Pattern);
                stippled = true;
        } else {
            fill = option->palette.brush(QPalette::Button);
            stippled = false;
        }
        if (option->state & (State_Raised | State_Sunken | State_On)) {
            if (option->state & State_AutoRaise) {
                if(option->state & (State_Enabled | State_Sunken | State_On)){
                    if (panel)
                        qDrawPlainRect(painter, option->rect,option->palette.shadow().color(),d->doubleControls, &fill);
                    else
                        qDrawPlainRect(painter, option->rect,option->palette.shadow().color(),d->doubleControls, &fill);
                }
                if (stippled) {
                    painter->setPen(option->palette.button().color());
                    painter->drawRect(option->rect.adjusted(1, 1, -2, -2));
                }
            } else {
                qDrawPlainRect(painter, option->rect,option->palette.shadow().color(),d->doubleControls, &fill);
            }
        } else {
            painter->fillRect(option->rect, fill);
        }
        break; }

    case PE_FrameFocusRect:
        if (const QStyleOptionFocusRect *fropt = qstyleoption_cast<const QStyleOptionFocusRect *>(option)) {
            //### check for d->alt_down
            int penSize;
            d->doubleControls ? penSize = 2 : penSize = 1;
            bool alternateFocusStyle = false;
            if (!widget)
                alternateFocusStyle = true;
#ifndef QT_NO_COMBOBOX
            if (qobject_cast<const QComboBox*>(widget))
                alternateFocusStyle = true;
#endif
            if (!(fropt->state & State_KeyboardFocusChange) && !styleHint(SH_UnderlineShortcut, option))
                return;
            QRect r = option->rect;
            painter->save();
            painter->setBackgroundMode(Qt::TransparentMode);
            if (alternateFocusStyle) {
                QColor bg_col = fropt->backgroundColor;
                if (!bg_col.isValid())
                    bg_col = painter->background().color();
                // Create an "XOR" color.
                QColor patternCol((bg_col.red() ^ 0xff) & 0xff,
                    (bg_col.green() ^ 0xff) & 0xff,
                    (bg_col.blue() ^ 0xff) & 0xff);
                painter->setBrush(QBrush(patternCol, Qt::Dense4Pattern));
                painter->setBrushOrigin(r.topLeft());
            }
            else {
                painter->setPen(option->palette.highlight().color());
                painter->setBrush(option->palette.highlight());
            }
            painter->setPen(Qt::NoPen);
            painter->setBrushOrigin(r.topLeft());
            painter->drawRect(r.left(), r.top(), r.width(), penSize);    // Top
            painter->drawRect(r.left(), r.bottom(), r.width() + penSize - 1, penSize); // Bottom
            painter->drawRect(r.left(), r.top(), penSize, r.height());   // Left
            painter->drawRect(r.right(), r.top(), penSize, r.height());  // Right
            painter->restore();
        }
        break;

    case PE_PanelButtonBevel: {
        QBrush fill;
        bool panel = element != PE_FrameButtonBevel;
        painter->setBrushOrigin(option->rect.topLeft());
        if (!(option->state & State_Sunken) && (option->state & State_On))
            fill = QBrush(option->palette.light().color(), Qt::Dense4Pattern);
        else
            fill = option->palette.brush(QPalette::Button);

        if (option->state & (State_Raised | State_On | State_Sunken)) {
            if (d->doubleControls)
              qDrawPlainRect(painter, option->rect,option->palette.shadow().color(),2,&fill);
            else
              qDrawPlainRect(painter, option->rect,option->palette.shadow().color(),1,&fill);
        } else {
            if (panel)
                painter->fillRect(option->rect, fill);
            else
                painter->drawRect(option->rect);
        }
        break; }

    case PE_FrameGroupBox:
        if (const QStyleOptionFrame *frame = qstyleoption_cast<const QStyleOptionFrame *>(option)) {

            const QStyleOptionFrameV2 *frame2 = qstyleoption_cast<const QStyleOptionFrameV2 *>(option);
            if (frame2 && !(frame2->features & QStyleOptionFrameV2::Flat)) {
              QPen oldPen = painter->pen();
              QRect r = frame->rect;
              painter->setPen(frame->palette.shadow().color());
              painter->fillRect(r.x(), r.y(), r.x() + r.width()-1,
                                r.y() + r.height() - windowsMobileFrameGroupBoxOffset,
              frame->palette.light());
              painter ->drawLine(r.topLeft() + QPoint(-2, 1), r.topRight()+ QPoint(0, 1));
              if (d->doubleControls)
                painter ->drawLine(r.topLeft() + QPoint(-2, 2), r.topRight()+ QPoint(0, 2));
              painter->setPen(oldPen);
            }
        }
        break;

   case PE_IndicatorCheckBox: {
        QBrush fill;
        QRect r = d->doubleControls ? option->rect.adjusted(0,1,0,-1) : option->rect;
        if (option->state & State_NoChange)
            fill = QBrush(option->palette.shadow().color(), Qt::Dense4Pattern);
        else if (option->state & State_Sunken)
            fill = option->palette.button();
        else if (option->state & State_Enabled)
            fill = option->palette.base();
        else
            fill = option->palette.background();
        painter->save();
        doRestore = true;
        if (d->doubleControls && (option->state & State_NoChange))
            painter->fillRect(r, fill);
        else
            painter->fillRect(option->rect, fill);
        painter->setPen(option->palette.shadow().color());
        painter->drawLine(r.topLeft(), r.topRight());
        painter->drawLine(r.topRight(), r.bottomRight());
        painter->drawLine(r.bottomLeft(), r.bottomRight());
        painter->drawLine(r.bottomLeft(), r.topLeft());
        if (d->doubleControls) {
            QRect r0 = r.adjusted(1, 1, -1, -1);
            painter->drawLine(r0.topLeft(), r0.topRight());
            painter->drawLine(r0.topRight(), r0.bottomRight());
            painter->drawLine(r0.bottomLeft(), r0.bottomRight());
            painter->drawLine(r0.bottomLeft(), r0.topLeft());
        }
        if (option->state & State_HasFocus) {
            painter->setPen(option->palette.highlight().color());
            QRect r2 = d->doubleControls ? r.adjusted(2, 2, -2, -2) : r.adjusted(1, 1, -1, -1);
            painter->drawLine(r2.topLeft(), r2.topRight());
            painter->drawLine(r2.topRight(), r2.bottomRight());
            painter->drawLine(r2.bottomLeft(), r2.bottomRight());
            painter->drawLine(r2.bottomLeft(), r2.topLeft());
            if (d->doubleControls) {
                QRect r3 = r2.adjusted(1, 1, -1, -1);
                painter->drawLine(r3.topLeft(), r3.topRight());
                painter->drawLine(r3.topRight(), r3.bottomRight());
                painter->drawLine(r3.bottomLeft(), r3.bottomRight());
                painter->drawLine(r3.bottomLeft(), r3.topLeft());
            }
            painter->setPen(option->palette.shadow().color());
        }
        //fall through...
    }
    case PE_IndicatorViewItemCheck:
    case PE_Q3CheckListIndicator: {
        if (!doRestore) {
            painter->save();
            doRestore = true;
        }
        if (element == PE_Q3CheckListIndicator || element == PE_IndicatorViewItemCheck) {
            painter->setPen(option->palette.shadow().color());
            if (option->state & State_NoChange)
                painter->setBrush(option->palette.brush(QPalette::Button));
            if (d->doubleControls) {
                QRect r = QRect(option->rect.x(), option->rect.y(), windowsMobileitemViewCheckBoxSize * 2, windowsMobileitemViewCheckBoxSize * 2);
                qDrawPlainRect(painter, r, option->palette.shadow().color(), 2);
            } else {
                QRect r = QRect(option->rect.x(), option->rect.y(), windowsMobileitemViewCheckBoxSize, windowsMobileitemViewCheckBoxSize);
                qDrawPlainRect(painter, r, option->palette.shadow().color(), 1);
            }            
            if (option->state & State_Enabled)
                d->imageChecked.setColor(1, option->palette.shadow().color().rgba());
            else
              d->imageChecked.setColor(1, option->palette.dark().color().rgba());
            if (!(option->state & State_Off)) {
                if (d->doubleControls)
                    painter->drawImage(option->rect.x(), option->rect.y(), d->imageChecked);
                else
                    painter->drawImage(option->rect.x() + 3, option->rect.y() + 3, d->imageChecked);
            }
        }
        else {
            if (option->state & State_NoChange)
                d->imageCheckedBold.setColor(1, option->palette.dark().color().rgba());
            else if (option->state & State_Enabled)
                d->imageCheckedBold.setColor(1, option->palette.shadow().color().rgba());
            else
                d->imageCheckedBold.setColor(1, option->palette.dark().color().rgba());
            if (!(option->state & State_Off)) {
                if (d->doubleControls)
                    painter->drawImage(option->rect.x() + 2, option->rect.y(), d->imageCheckedBold);
                else
                    painter->drawImage(option->rect.x() + 3, option->rect.y() + 3, d->imageCheckedBold);
            }
        }
        if (doRestore)
            painter->restore();
        break; }
    case PE_IndicatorRadioButton: {
        painter->save();

        if (option->state & State_HasFocus) {
            d->imageRadioButtonHighlighted.setColor(1, option->palette.shadow().color().rgba());
            d->imageRadioButtonHighlighted.setColor(2, option->palette.highlight().color().rgba());
            painter->drawImage(option->rect.x(), option->rect.y(), d->imageRadioButtonHighlighted);
        }
        else {
            d->imageRadioButton.setColor(1, option->palette.shadow().color().rgba());
            painter->drawImage(option->rect.x(), option->rect.y(), d->imageRadioButton);
        }
        if (option->state & (State_Sunken | State_On)) {
            if  (option->state & State_Enabled)
                d->imageRadioButtonChecked.setColor(1, option->palette.shadow().color().rgba());
            else
                d->imageRadioButtonChecked.setColor(1, option->palette.dark().color().rgba());

            static const int offset = d->doubleControls ? 6 : 3;
            painter->drawImage(option->rect.x() + offset, option->rect.y() + offset, d->imageRadioButtonChecked);
        }
        painter->restore();
        break; }
   case PE_PanelButtonCommand:
       if (const QStyleOptionButton *button = qstyleoption_cast<const QStyleOptionButton *>(option)) {
           QBrush fill;
           State flags = option->state;
           QPalette pal = option->palette;
           QRect r = option->rect;
           if ((flags & State_Sunken || flags & State_On) )
               fill = pal.brush(QPalette::Shadow);
           else
               fill = pal.brush(QPalette::Button);
            int singleLine = 1;
            int doubleLine = 2;
            if (d->doubleControls) {
                singleLine = 2;
                doubleLine = 4;
            }
           if (button->features & QStyleOptionButton::DefaultButton && flags & State_Sunken) {
               if (d->doubleControls) {
                 qDrawPlainRect(painter, r, pal.shadow().color(), 1, &fill);
                 qDrawPlainRect(painter, r.adjusted(1, 1, -1, 1), pal.shadow().color(), 1, &fill);
               }
               else {
                    qDrawPlainRect(painter, r, pal.shadow().color(), 1, &fill);
               }
           } else if (flags & (State_Raised | State_Sunken | State_On | State_Sunken)) {
               qDrawPlainRect(painter, r, pal.shadow().color(), singleLine, &fill);
           } else {
               painter->fillRect(r, fill);
           }
       }
       break;
   case PE_FrameDefaultButton: {
       painter->save();
       painter->setPen(option->palette.shadow().color());
       QRect rect = option->rect;
       if (d->doubleControls) {
           rect.adjust(1, 1, -2, -2);
           painter->drawRect(rect);
           painter->drawRect(rect.adjusted(1, 1, -1, -1));
       }

       else {
            rect.adjust(2, 2, -3, -3);
            painter->drawRect(rect);
       }
       painter->restore();
       break; }
   case PE_IndicatorSpinPlus:
   case PE_IndicatorSpinMinus: {
       QRect r = option->rect;
       int fw = pixelMetric(PM_DefaultFrameWidth, option, widget)+2;
       QRect br = r.adjusted(fw, fw, -fw, -fw);
       int offset = (option->state & State_Sunken) ? 1 : 0;
       int step = (br.width() + 4) / 5;
       painter->fillRect(br.x() + offset, br.y() + offset +br.height() / 2 - step / 2,
                         br.width(), step, option->palette.buttonText());
       if (element == PE_IndicatorSpinPlus)
           painter->fillRect(br.x() + br.width() / 2 - step / 2 + offset, br.y() + offset+4,
                             step, br.height() - 7, option->palette.buttonText());
       break; }
    case PE_IndicatorSpinUp:
    case PE_IndicatorSpinDown: {
        painter->save();
            QPoint points[7];
            switch (element) {
                case PE_IndicatorSpinUp:
                    points[0] = QPoint(-2, -4);
                    points[1] = QPoint(-2, 2);
                    points[2] = QPoint(-1, -3);
                    points[3] = QPoint(-1, 1);
                    points[4] = QPoint(0, -2);
                    points[5] = QPoint(0, 0);
                    points[6] = QPoint(1, -1);
                break;
                case PE_IndicatorSpinDown:
                    points[0] = QPoint(0, -4);
                    points[1] = QPoint(0, 2);
                    points[2] = QPoint(-1, -3);
                    points[3] = QPoint(-1, 1);
                    points[4] = QPoint(-2, -2);
                    points[5] = QPoint(-2, 0);
                    points[6] = QPoint(-3, -1);
                break;
                default:
                break;
            }
            if (option->state & State_Sunken)
                painter->translate(pixelMetric(PM_ButtonShiftHorizontal),
                pixelMetric(PM_ButtonShiftVertical));
            if (option->state & State_Enabled) {
                painter->translate(option->rect.x() + option->rect.width() / 2,
                                   option->rect.y() + option->rect.height() / 2);
                painter->setPen(option->palette.buttonText().color());
                painter->drawLine(points[0], points[1]);
                painter->drawLine(points[2], points[3]);
                painter->drawLine(points[4], points[5]);
                painter->drawPoint(points[6]);
            } else {
                painter->translate(option->rect.x() + option->rect.width() / 2 + 1,
                                   option->rect.y() + option->rect.height() / 2 + 1);
                painter->setPen(option->palette.light().color());
                painter->drawLine(points[0], points[1]);
                painter->drawLine(points[2], points[3]);
                painter->drawLine(points[4], points[5]);
                painter->drawPoint(points[6]);
                painter->translate(-1, -1);
                painter->setPen(option->palette.mid().color());
                painter->drawLine(points[0], points[1]);
                painter->drawLine(points[2], points[3]);
                painter->drawLine(points[4], points[5]);
                painter->drawPoint(points[6]);
            }
        painter->restore();
        break; }

    case PE_IndicatorArrowUpBig:
    case PE_IndicatorArrowDownBig:
    case PE_IndicatorArrowLeftBig:
    case PE_IndicatorArrowRightBig:

    case PE_IndicatorArrowUp:
    case PE_IndicatorArrowDown:
    case PE_IndicatorArrowRight:
    case PE_IndicatorArrowLeft: {
            painter->save();

            if (d->doubleControls) {
                QColor color;
                if (option->state & State_Sunken)
                    color = option->palette.light().color();
                else
                    color = option->palette.buttonText().color();
                QImage image;
                int xoffset, yoffset;
                switch (element) {
                    case PE_IndicatorArrowUp:
                          image = d->imageArrowUp;
                          xoffset = 1;
                          yoffset = 12;
                          break;
                    case PE_IndicatorArrowDown:
                          image = d->imageArrowDown;
                          xoffset = 1;
                          yoffset =12;
                          break;
                    case PE_IndicatorArrowLeft:
                          image = d->imageArrowLeft;
                          xoffset = 8;
                          yoffset = 2;
                          break;
                    case PE_IndicatorArrowRight:
                          image = d->imageArrowRight;
                          xoffset = 8;
                          yoffset = 2;
                          break;
                     case PE_IndicatorArrowUpBig:
                          image = d->imageArrowUpBig;
                          xoffset = 3;
                          yoffset = 12;
                          break;
                    case PE_IndicatorArrowDownBig:
                          image = d->imageArrowDownBig;
                          xoffset = 2;
                          yoffset =12;
                          break;
                    case PE_IndicatorArrowLeftBig:
                          image = d->imageArrowLeftBig;
                          xoffset = 8;
                          yoffset = 2;
                          break;
                    case PE_IndicatorArrowRightBig:
                          image = d->imageArrowRightBig;
                          xoffset = 8;
                          yoffset = 2;
                          break;
                    default:
                          break;
                }
                image.setColor(1, color.rgba());
                painter->drawImage(option->rect.x() + xoffset, option->rect.y() + yoffset, image);
            }
            else {                
                 QPoint points[7];
                 switch (element) {
                     case PE_IndicatorArrowUp:
                     case PE_IndicatorArrowUpBig:
                          points[0] = QPoint(-3, 1);
                          points[1] = QPoint(3, 1);
                          points[2] = QPoint(-2, 0);
                          points[3] = QPoint(2, 0);
                          points[4] = QPoint(-1, -1);
                          points[5] = QPoint(1, -1);
                          points[6] = QPoint(0, -2);
                          break;
                     case PE_IndicatorArrowDown:
                     case PE_IndicatorArrowDownBig:
                         points[0] = QPoint(-3, -1);
                         points[1] = QPoint(3, -1);
                         points[2] = QPoint(-2, 0);
                         points[3] = QPoint(2, 0);
                         points[4] = QPoint(-1, 1);
                         points[5] = QPoint(1, 1);
                         points[6] = QPoint(0, 2);
                         break;
                     case PE_IndicatorArrowRight:
                     case PE_IndicatorArrowRightBig:
                         points[0] = QPoint(-2, -3);
                         points[1] = QPoint(-2, 3);
                         points[2] = QPoint(-1, -2);
                         points[3] = QPoint(-1, 2);
                         points[4] = QPoint(0, -1);
                         points[5] = QPoint(0, 1);
                         points[6] = QPoint(1, 0);
                         break;
                     case PE_IndicatorArrowLeft:
                     case PE_IndicatorArrowLeftBig:
                         points[0] = QPoint(0, -3);
                         points[1] = QPoint(0, 3);
                         points[2] = QPoint(-1, -2);
                         points[3] = QPoint(-1, 2);
                         points[4] = QPoint(-2, -1);
                         points[5] = QPoint(-2, 1);
                         points[6] = QPoint(-3, 0);
                         break;
                     default:
                     break;
                    }
                    if (option->state & State_Sunken)
                        painter->setPen(option->palette.light().color());
                    else
                        painter->setPen(option->palette.buttonText().color());
                    if (option->state & State_Enabled) {
                        painter->translate(option->rect.x() + option->rect.width() / 2,
                            option->rect.y() + option->rect.height() / 2 - 1);
                        painter->drawLine(points[0], points[1]);
                        painter->drawLine(points[2], points[3]);
                        painter->drawLine(points[4], points[5]);
                        painter->drawPoint(points[6]);
                    } else {
                        painter->translate(option->rect.x() + option->rect.width() / 2,
                            option->rect.y() + option->rect.height() / 2 - 1);
                        painter->setPen(option->palette.mid().color());
                        painter->drawLine(points[0], points[1]);
                        painter->drawLine(points[2], points[3]);
                        painter->drawLine(points[4], points[5]);
                        painter->drawPoint(points[6]);
                    }                
            }
        painter->restore();
        break; }
#ifndef QT_NO_TABWIDGET
    case PE_FrameTabWidget:
        if (const QStyleOptionTabWidgetFrame *tab = qstyleoption_cast<const QStyleOptionTabWidgetFrame *>(option)) {
          QRect rect = option->rect;
        QPalette pal = option->palette;
        painter->save();
        QBrush fill = pal.light();
          painter->fillRect(rect, fill);
          painter->setPen(pal.shadow().color());
          if (d->doubleControls) {
              QPen pen = painter->pen();
              pen.setWidth(2);
              pen.setCapStyle(Qt::FlatCap);
              painter->setPen(pen);
          }
          switch (tab->shape) {
            case QTabBar::RoundedNorth:
                if (d->doubleControls)
                  painter->drawLine(rect.topLeft() + QPoint(0, 1), rect.topRight() + QPoint(0, 1));
                else
                    painter->drawLine(rect.topLeft(), rect.topRight());
                break;
            case QTabBar::RoundedSouth:
                if (d->doubleControls)
                  painter->drawLine(rect.bottomLeft(), rect.bottomRight());
                else
                  painter->drawLine(rect.bottomLeft(), rect.bottomRight());
                break;
            case QTabBar::RoundedEast:
                painter->drawLine(rect.topRight(), rect.bottomRight());
                break;
            case QTabBar::RoundedWest:
                painter->drawLine(rect.topLeft(), rect.bottomLeft());
                break;
            case QTabBar::TriangularWest:
            case QTabBar::TriangularEast:
            case QTabBar::TriangularSouth:
            case QTabBar::TriangularNorth:
                if (d->doubleControls)
                  qDrawPlainRect(painter, rect.adjusted(0,-2,0,0), option->palette.shadow().color(),2,&pal.light());
                else
                  qDrawPlainRect(painter, rect, option->palette.shadow().color(),1,&pal.light());
                break;
            default:
                break;
            }
            painter->restore();
        }
        break;
#endif //QT_NO_TABBAR

    case PE_FrameWindow: {
        QPalette popupPal = option->palette;
        popupPal.setColor(QPalette::Light, option->palette.background().color());
        popupPal.setColor(QPalette::Midlight, option->palette.light().color());
        if (d->doubleControls)
            qDrawPlainRect(painter, option->rect, popupPal.shadow().color(),2,0);
        else
            qDrawPlainRect(painter, option->rect, popupPal.shadow().color(),1,0);
        break; }
    case PE_FrameTabBarBase: {
        break; }
    case PE_Widget:
        break;
    case PE_IndicatorMenuCheckMark: {
        int markW = option->rect.width() > 7 ? 7 : option->rect.width();
        int markH = markW;
        if (d->doubleControls)
            markW*=2;
            markH*=2;
        int posX = option->rect.x() + (option->rect.width() - markW)/2 + 1;
        int posY = option->rect.y() + (option->rect.height() - markH)/2;

        QVector<QLineF> a;
        a.reserve(markH);

        int i, xx, yy;
        xx = posX;
        yy = 3 + posY;
        for (i = 0; i < markW/2; ++i) {
            a << QLineF(xx, yy, xx, yy + 2);
            ++xx;
            ++yy;
        }
        yy -= 2;
        for (; i < markH; ++i) {
            a << QLineF(xx, yy, xx, yy + 2);
            ++xx;
            --yy;
        }
        if (!(option->state & State_Enabled) && !(option->state & State_On)) {
            int pnt;
            painter->setPen(option->palette.highlightedText().color());
            QPoint offset(1, 1);
            for (pnt = 0; pnt < a.size(); ++pnt)
                a[pnt].translate(offset.x(), offset.y());
            painter->drawLines(a);
            for (pnt = 0; pnt < a.size(); ++pnt)
                a[pnt].translate(offset.x(), offset.y());
        }
        painter->setPen(option->palette.text().color());
        painter->drawLines(a);
        break; }
    case PE_Frame:
       if (d->doubleControls)
            qDrawPlainRect(painter, option->rect, option->palette.shadow().color(),2,&option->palette.light());
        else
            qDrawPlainRect(painter, option->rect, option->palette.shadow().color(),1,&option->palette.light());
        break;
    case PE_FrameLineEdit:
    case PE_FrameMenu:
        if (d->doubleControls)
            qDrawPlainRect(painter, option->rect, option->palette.shadow().color(),2);
        else
            qDrawPlainRect(painter, option->rect, option->palette.shadow().color(),1);
        break;
    case PE_FrameStatusBar:
        if (d->doubleControls)
            qDrawPlainRect(painter, option->rect, option->palette.shadow().color(),2,0);
        else
            qDrawPlainRect(painter, option->rect, option->palette.shadow().color(),1,0);
        break;

    default:
        QWindowsStyle::drawPrimitive(element, option, painter, widget);
        break;
    }
}

void QWindowsMobileStyle::drawControl(ControlElement element, const QStyleOption *option,
                                      QPainter *painter, const QWidget *widget) const {


   QWindowsMobileStylePrivate *d = const_cast<QWindowsMobileStylePrivate*>(d_func());

   
   painter->setClipping(false);
   switch (element) {
   case CE_MenuBarEmptyArea:
        painter->setClipping(true);
        QWindowsStyle::drawControl(element, option, painter, widget);
        break;
   case CE_PushButtonBevel:
        if (const QStyleOptionButton *button = qstyleoption_cast<const QStyleOptionButton *>(option)) {
            QRect br = button->rect;
            int dbi = pixelMetric(PM_ButtonDefaultIndicator, button, widget);

            if (button->features & QStyleOptionButton::AutoDefaultButton)
                br.setCoords(br.left() + dbi, br.top() + dbi, br.right() - dbi, br.bottom() - dbi);
            QStyleOptionButton tmpBtn = *button;
            tmpBtn.rect = br;
            drawPrimitive(PE_PanelButtonCommand, &tmpBtn, painter, widget);
            if (button->features & QStyleOptionButton::HasMenu) {
                int mbi = pixelMetric(PM_MenuButtonIndicator, button, widget);
                QRect ir = button->rect;
                QStyleOptionButton newButton = *button;
                if (d->doubleControls)
                    newButton.rect = QRect(ir.right() - mbi, ir.height() - 30, mbi, ir.height() - 4);
                else
                    newButton.rect = QRect(ir.right() - mbi, ir.height() - 20, mbi, ir.height() - 4);
                drawPrimitive(PE_IndicatorArrowDown, &newButton, painter, widget);
            }
            if (button->features & QStyleOptionButton::DefaultButton)
                drawPrimitive(PE_FrameDefaultButton, option, painter, widget);
        }
        break;
   case CE_RadioButton:
   case CE_CheckBox:
       if (const QStyleOptionButton *button = qstyleoption_cast<const QStyleOptionButton *>(option)) {
           bool isRadio = (element == CE_RadioButton);
           QStyleOptionButton subopt = *button;
           subopt.rect = subElementRect(isRadio ? SE_RadioButtonIndicator
               : SE_CheckBoxIndicator, button, widget);
           drawPrimitive(isRadio ? PE_IndicatorRadioButton : PE_IndicatorCheckBox,
               &subopt, painter, widget);
           subopt.rect = subElementRect(isRadio ? SE_RadioButtonContents
               : SE_CheckBoxContents, button, widget);
           drawControl(isRadio ? CE_RadioButtonLabel : CE_CheckBoxLabel, &subopt, painter, widget);
           if (button->state & State_HasFocus) {
               QStyleOptionFocusRect fropt;
               fropt.QStyleOption::operator=(*button);
               fropt.rect = subElementRect(isRadio ? SE_RadioButtonFocusRect
                   : SE_CheckBoxFocusRect, button, widget);
               drawPrimitive(PE_FrameFocusRect, &fropt, painter, widget);
           }
       }
       break;
   case CE_RadioButtonLabel:
   case CE_CheckBoxLabel:
       if (const QStyleOptionButton *button = qstyleoption_cast<const QStyleOptionButton *>(option)) {
           uint alignment = visualAlignment(button->direction, Qt::AlignLeft | Qt::AlignVCenter);
           if (!styleHint(SH_UnderlineShortcut, button, widget))
               alignment |= Qt::TextHideMnemonic;
           QPixmap pix;
           QRect textRect = button->rect;
           if (!button->icon.isNull()) {
               pix = button->icon.pixmap(button->iconSize, button->state & State_Enabled ? QIcon::Normal : QIcon::Disabled);
               drawItemPixmap(painter, button->rect, alignment, pix);
               if (button->direction == Qt::RightToLeft)
                   textRect.setRight(textRect.right() - button->iconSize.width() - 4);
               else
                   textRect.setLeft(textRect.left() + button->iconSize.width() + 4);
           }
           if (!button->text.isEmpty()){
               if (button->state & State_Enabled)
                   drawItemText(painter, textRect, alignment | Qt::TextShowMnemonic,
                   button->palette, false, button->text, QPalette::WindowText);
               else
                   drawItemText(painter, textRect, alignment | Qt::TextShowMnemonic,
                   button->palette, false, button->text, QPalette::Mid);
           }
       }
       break;
#ifndef QT_NO_PROGRESSBAR
    case CE_ProgressBarGroove:
        if (d->doubleControls)
            qDrawPlainRect(painter, option->rect, option->palette.shadow().color(), 2, &option->palette.brush(QPalette::Window));
        else
            qDrawPlainRect(painter, option->rect, option->palette.shadow().color(), 1, &option->palette.brush(QPalette::Window));
        break;
#endif //QT_NO_PROGRESSBAR
#ifndef QT_NO_TABBAR
    case CE_TabBarTab:
        if (const QStyleOptionTab *tab = qstyleoption_cast<const QStyleOptionTab *>(option)) {
            drawControl(CE_TabBarTabShape, tab, painter, widget);
            drawControl(CE_TabBarTabLabel, tab, painter, widget);
        }
        break;
    case CE_TabBarTabShape:
        if (const QStyleOptionTab *tab = qstyleoption_cast<const QStyleOptionTab *>(option)) {
            if (tab->shape == QTabBar::RoundedNorth || tab->shape == QTabBar::RoundedEast ||
                tab->shape == QTabBar::RoundedSouth || tab->shape == QTabBar::RoundedWest) {

                    painter->save();
                    painter->setPen(tab->palette.shadow().color());
                    if (d->doubleControls) {
                        QPen pen = painter->pen();
                        pen.setWidth(2);
                        pen.setCapStyle(Qt::FlatCap);
                        painter->setPen(pen);
                    }
                    if(tab->shape == QTabBar::RoundedNorth) {
                          if (tab->state & State_Selected) {
                            painter->fillRect(tab->rect, tab->palette.light());
                            painter->drawLine(tab->rect.topRight(), tab->rect.bottomRight());
                        }
                        else {
                            painter->fillRect(tab->rect, tab->palette.button());
                            painter->drawLine(tab->rect.bottomLeft() , tab->rect.bottomRight());
                            painter->drawLine(tab->rect.topRight(), tab->rect.bottomRight());
                        }
                    }
                    else if(tab->shape == QTabBar::RoundedSouth) {

                        if (tab->state & State_Selected) {
                            painter->fillRect(tab->rect.adjusted(0,-2,0,0), tab->palette.light());
                            painter->drawLine(tab->rect.topRight(), tab->rect.bottomRight());
                        }
                        else {
                            painter->fillRect(tab->rect, tab->palette.button());
                            if (d->doubleControls)
                                painter->drawLine(tab->rect.topLeft() + QPoint(0,1), tab->rect.topRight() + QPoint(0,1));
                            else
                                painter->drawLine(tab->rect.topLeft(), tab->rect.topRight());
                            painter->drawLine(tab->rect.topRight(), tab->rect.bottomRight());
                        }
                    }
                    else if(tab->shape == QTabBar::RoundedEast) {
                        if (tab->state & State_Selected) {
                            painter->fillRect(tab->rect, tab->palette.light());
                            painter->drawLine(tab->rect.topLeft(), tab->rect.topRight());
                        }
                        else {
                            painter->fillRect(tab->rect, tab->palette.button());
                            painter->drawLine(tab->rect.topLeft(), tab->rect.bottomLeft());
                            painter->drawLine(tab->rect.topLeft(), tab->rect.topRight());
                        }
                    }
                    else if(tab->shape == QTabBar::RoundedWest) {
                        if (tab->state & State_Selected) {
                            painter->fillRect(tab->rect, tab->palette.light());
                            painter->drawLine(tab->rect.bottomLeft(), tab->rect.bottomRight());
                        }
                        else {
                            painter->fillRect(tab->rect, tab->palette.button());
                            painter->drawLine(tab->rect.topRight(), tab->rect.bottomRight());
                            painter->drawLine(tab->rect.bottomLeft(), tab->rect.bottomRight());
                        }
                    }

                    painter->restore();
            } else {
                QCommonStyle::drawControl(element, option, painter, widget);
            }
            break; }

#endif // QT_NO_TABBAR

#ifndef QT_NO_TOOLBAR
    case CE_ToolBar:
        if (const QStyleOptionToolBar *toolBar = qstyleoption_cast<const QStyleOptionToolBar *>(option)) {
        QRect rect = option->rect;
        painter->save();
        painter->setPen(option->palette.dark().color());
        painter->fillRect(rect,option->palette.button());
        if (d->doubleControls) {
            QPen pen = painter->pen();
            pen.setWidth(4);
            painter->setPen(pen);
        }
        if (toolBar->toolBarArea == Qt::TopToolBarArea)
            painter->drawLine(rect.bottomLeft(), rect.bottomRight());
        else
            painter->drawLine(rect.topLeft(), rect.topRight());
        painter->restore();
        break; }
#endif //QT_NO_TOOLBAR
    case CE_Header:
        if (const QStyleOptionHeader *header = qstyleoption_cast<const QStyleOptionHeader *>(option)) {
            QRegion clipRegion = painter->clipRegion();
            painter->setClipRect(option->rect);
            drawControl(CE_HeaderSection, header, painter, widget);
            QStyleOptionHeader subopt = *header;
            subopt.rect = subElementRect(SE_HeaderLabel, header, widget);
            if (header->state & State_Sunken) 
                subopt.palette.setColor(QPalette::ButtonText, header->palette.brightText().color());
            subopt.state |= QStyle::State_On;
            if (subopt.rect.isValid())
                drawControl(CE_HeaderLabel, &subopt, painter, widget);
            if (header->sortIndicator != QStyleOptionHeader::None) {
                subopt.rect = subElementRect(SE_HeaderArrow, option, widget);
                drawPrimitive(PE_IndicatorHeaderArrow, &subopt, painter, widget);
            }
            painter->setClipRegion(clipRegion);
        }
        break;

    case CE_HeaderSection:
        if (const QStyleOptionHeader *header = qstyleoption_cast<const QStyleOptionHeader *>(option)) {
            QBrush fill;
            QColor color;
            QRect rect = option->rect;
            painter->setPen(option->palette.shadow().color());

            int penSize = 1;

            if (d->doubleControls) {
                penSize = 2;
                QPen pen = painter->pen();
                pen.setWidth(2);
                pen.setCapStyle(Qt::FlatCap);
                painter->setPen(pen);
            }

            //fix Frame

            if (header->position == QStyleOptionHeader::End
                || (header->position == QStyleOptionHeader::OnlyOneSection
                && !header->text.isEmpty()))
                if (Qt::Horizontal == header->orientation )
                  rect.adjust(0, 0, penSize, 0);
                else
                  rect.adjust(0, 0, 0, penSize);

            if (option->state & State_Sunken) {
                fill = option->palette.brush(QPalette::Shadow);
                color = option->palette.light().color();
                painter->drawLine(rect.bottomLeft(), rect.bottomRight());
                painter->drawLine(rect.topRight(), rect.bottomRight());
                rect.adjust(0, 0, -penSize, -penSize);
            }
            else {
                fill = option->palette.brush(QPalette::Button);
                color = option->palette.shadow().color();
                if (Qt::Horizontal == header->orientation )
                    rect.adjust(-penSize, 0, 0, 0);
                else
                  rect.adjust(0, -penSize, 0, 0);
            }
            if (Qt::Horizontal == header->orientation )
                    rect.adjust(0,-penSize,0,0);
                else
                  rect.adjust(-penSize, 0, 0, 0);

            if (option->state & State_Sunken) {
                qDrawPlainRect(painter, rect, color, penSize, &fill);
            } else {
                //Corner
                rect.adjust(-penSize, 0, 0, 0);
                qDrawPlainRect(painter, rect, color, penSize, &fill);
            }

            //Hack to get rid of some double lines... StyleOptions need a clean flag for that
             rect = option->rect;
#ifndef QT_NO_SCROLLAREA
             if (const QAbstractScrollArea *abstractScrollArea = qobject_cast<const QAbstractScrollArea *> (widget) ) {
                 QRect rectScrollArea = abstractScrollArea->geometry();
                 if (Qt::Horizontal == header->orientation )
                     if ((rectScrollArea.right() - rect.right() ) > 1)
                         painter->drawLine(rect.topRight(), rect.bottomRight());
                     else ;
                 else
                     if ((rectScrollArea.bottom() - rect.bottom() ) > 1)
                         painter->drawLine(rect.bottomLeft(), rect.bottomRight());
             }
#endif // QT_NO_SCROLLAREA
            break; }

#ifndef QT_NO_DOCKWIDGET
    case CE_DockWidgetTitle:
        if (const QStyleOptionDockWidget *dwOpt = qstyleoption_cast<const QStyleOptionDockWidget *>(option)) {
            const QStyleOptionDockWidgetV2 *v2
                = qstyleoption_cast<const QStyleOptionDockWidgetV2*>(option);
            bool verticalTitleBar = v2 == 0 ? false : v2->verticalTitleBar;

            QRect rect = dwOpt->rect;
            QRect r = rect;

            if (verticalTitleBar) {
                QSize s = r.size();
                s.transpose();
                r.setSize(s);

                painter->save();
                painter->translate(r.left(), r.top() + r.width());
                painter->rotate(-90);
                painter->translate(-r.left(), -r.top());
            }

            bool floating = false;
            bool active = dwOpt->state & State_Active;
            int menuOffset = 0; //used to center text when floated
            QColor inactiveCaptionTextColor = option->palette.highlightedText().color();
            if (dwOpt->movable) {
                QColor left, right;

                //Titlebar gradient
                if (widget && widget->isWindow()) {
                    floating = true;
                    if (active) {
                        right = option->palette.highlight().color();
                        left = right.lighter(125);
                    } else {
                        left  = option->palette.highlight().color().lighter(125);
                        right = QColor(0xff, 0xff, 0xff);
                    }
                    menuOffset = 2;
                    QBrush fillBrush(left);
                    if (left != right) {
                        QPoint p1(r.x(), r.top() + r.height()/2);
                        QPoint p2(rect.right(), r.top() + r.height()/2);
                        QLinearGradient lg(p1, p2);
                        lg.setColorAt(0, left);
                        lg.setColorAt(1, right);
                        fillBrush = lg;
                    }
                    painter->fillRect(r.adjusted(0, 0, 0, -3), fillBrush);
                } else {
                    painter->fillRect(r.adjusted(0, 0, 0, -3), option->palette.button().color());
                }
                painter->setPen(dwOpt->palette.color(QPalette::Light));
                if (!widget || !widget->isWindow()) {
                    painter->drawLine(r.topLeft(), r.topRight());
                    painter->setPen(dwOpt->palette.color(QPalette::Dark));
                    painter->drawLine(r.bottomLeft(), r.bottomRight());            }
            }
            if (!dwOpt->title.isEmpty()) {
                QFont oldFont = painter->font();
                QFont newFont = oldFont;
                if (newFont.pointSize() > 2)
                    newFont.setPointSize(newFont.pointSize() - 2);
                if (floating)
                    newFont.setBold(true);
                painter->setFont(newFont);
                QPalette palette = dwOpt->palette;
                palette.setColor(QPalette::Window, inactiveCaptionTextColor);
                QRect titleRect = subElementRect(SE_DockWidgetTitleBarText, option, widget);
                if (verticalTitleBar) {
                    titleRect = QRect(r.left() + rect.bottom()
                                        - titleRect.bottom(),
                                    r.top() + titleRect.left() - rect.left(),
                                    titleRect.height(), titleRect.width());
                }
                drawItemText(painter, titleRect,
                            Qt::AlignLeft | Qt::AlignVCenter | Qt::TextShowMnemonic, palette,
                            dwOpt->state & State_Enabled, dwOpt->title,
                            floating ? (active ? QPalette::BrightText : QPalette::Window) : QPalette::WindowText);
                painter->setFont(oldFont);
            }
            if (verticalTitleBar)
                painter->restore();
        }
        return;
#endif // QT_NO_DOCKWIDGET

    case CE_PushButtonLabel:
        if (const QStyleOptionButton *button = qstyleoption_cast<const QStyleOptionButton *>(option)) {
            painter->save();
            QRect ir = button->rect;
            QPalette::ColorRole colorRole;
            uint tf = Qt::AlignVCenter | Qt::TextShowMnemonic;
            if (!styleHint(SH_UnderlineShortcut, button, widget))
                tf |= Qt::TextHideMnemonic;

            if (button->state & (State_On | State_Sunken))
                colorRole = QPalette::Light;
            else
                colorRole = QPalette::ButtonText;

            if (!button->icon.isNull()) {
                QIcon::Mode mode = button->state & State_Enabled ? QIcon::Normal
                    : QIcon::Disabled;
                if (mode == QIcon::Normal && button->state & State_HasFocus)
                    mode = QIcon::Active;
                QIcon::State state = QIcon::Off;
                if (button->state & State_On)
                    state = QIcon::On;
                QPixmap pixmap = button->icon.pixmap(button->iconSize, mode, state);
                int pixw = pixmap.width();
                int pixh = pixmap.height();
                //Center the icon if there is no text

                QPoint point;
                if (button->text.isEmpty()) {
                    point = QPoint(ir.x() + ir.width() / 2 - pixw / 2,
                        ir.y() + ir.height() / 2 - pixh / 2);
                } else {
                    point = QPoint(ir.x() + 2, ir.y() + ir.height() / 2 - pixh / 2);
                }
                if (button->direction == Qt::RightToLeft)
                    point.rx() += pixw;

                if ((button->state & (State_On | State_Sunken)) && button->direction == Qt::RightToLeft)
                    point.rx() -= pixelMetric(PM_ButtonShiftHorizontal, option, widget) * 2;

                painter->drawPixmap(visualPos(button->direction, button->rect, point), pixmap);

                if (button->direction == Qt::RightToLeft)
                    ir.translate(-4, 0);
                else
                    ir.translate(pixw + 4, 0);
                ir.setWidth(ir.width() - (pixw + 4));
                // left-align text if there is
                if (!button->text.isEmpty())
                    tf |= Qt::AlignLeft;
            } else {
                tf |= Qt::AlignHCenter;
            }
            if (button->state & State_Enabled)
                drawItemText(painter, ir, tf, button->palette, true, button->text, colorRole);
            else
            drawItemText(painter, ir, tf, button->palette, true, button->text, QPalette::Mid);
            painter->restore();
        }
        break;
        default:
            QWindowsStyle::drawControl(element, option, painter, widget);
            break;
    }
}

void QWindowsMobileStyle::drawComplexControl(ComplexControl control, const QStyleOptionComplex *option,
                                             QPainter *painter, const QWidget *widget) const {

    painter->setClipping(false);
    QWindowsMobileStylePrivate *d = const_cast<QWindowsMobileStylePrivate*>(d_func());

    switch (control) {
#ifndef QT_NO_SLIDER
    case CC_Slider:
        if (const QStyleOptionSlider *slider = qstyleoption_cast<const QStyleOptionSlider *>(option)) {
            int thickness  = pixelMetric(PM_SliderControlThickness, slider, widget);
            int len        = pixelMetric(PM_SliderLength, slider, widget);
            int ticks = slider->tickPosition;
            QRect groove = subControlRect(CC_Slider, slider, SC_SliderGroove, widget);
            QRect handle = subControlRect(CC_Slider, slider, SC_SliderHandle, widget);

            if ((slider->subControls & SC_SliderGroove) && groove.isValid()) {
                int mid = thickness / 2;
                if (ticks & QSlider::TicksAbove)
                    mid += len / 8;
                if (ticks & QSlider::TicksBelow)
                    mid -= len / 8;

                painter->setPen(slider->palette.shadow().color());
                if (slider->orientation == Qt::Horizontal) {
                    qDrawPlainRect(painter, groove.x(), groove.y() + mid - 2,
                        groove.width(), 4, option->palette.shadow().color(),1,0);
                } else {
                    qDrawPlainRect(painter, groove.x()+mid-2, groove.y(),
                        4, groove.height(), option->palette.shadow().color(),1,0);
                }
            }
            if (slider->subControls & SC_SliderTickmarks) {
                QStyleOptionSlider tmpSlider = *slider;
                tmpSlider.subControls = SC_SliderTickmarks;
                QCommonStyle::drawComplexControl(control, &tmpSlider, painter, widget);
            }

            if (slider->subControls & SC_SliderHandle) {
                const QColor c0 = slider->palette.shadow().color();
                const QColor c1 = slider->palette.dark().color();
                const QColor c3 = slider->palette.midlight().color();
                const QColor c4 = slider->palette.dark().color();
                QBrush handleBrush;

                if (slider->state & State_Enabled) {
                    handleBrush = slider->palette.color(QPalette::Light);
                } else {
                    handleBrush = QBrush(slider->palette.color(QPalette::Shadow),
                        Qt::Dense4Pattern);
                }
                int x = handle.x(), y = handle.y(),
                    wi = handle.width(), he = handle.height();
                int x1 = x;
                int x2 = x+wi-1;
                int y1 = y;
                int y2 = y+he-1;

                Qt::Orientation orient = slider->orientation;
                bool tickAbove = slider->tickPosition == QSlider::TicksAbove;
                bool tickBelow = slider->tickPosition == QSlider::TicksBelow;

                if (slider->state & State_HasFocus) {
                    QStyleOptionFocusRect fropt;
                    fropt.QStyleOption::operator=(*slider);
                    fropt.rect = subElementRect(SE_SliderFocusRect, slider, widget);
                    drawPrimitive(PE_FrameFocusRect, &fropt, painter, widget);
                }
                if ((tickAbove && tickBelow) || (!tickAbove && !tickBelow)) {
                    Qt::BGMode oldMode = painter->backgroundMode();
                    painter->setBackgroundMode(Qt::OpaqueMode);
                    qDrawPlainRect(painter, QRect(x, y, wi, he)
                        ,slider->palette.shadow().color(),1,&handleBrush);
                    painter->setBackgroundMode(oldMode);
                    QBrush fill = QBrush(option->palette.light().color(), Qt::Dense4Pattern);
                    if (slider->state & State_Sunken)
                        painter->fillRect(QRectF(x1 + 2, y1 + 2, x2 - x1 - 3, y2 - y1 - 3),fill);
                    return;
                }
                QSliderDirection dir;
                if (orient == Qt::Horizontal)
                    if (tickAbove)
                        dir = SliderUp;
                    else
                        dir = SliderDown;
                else
                    if (tickAbove)
                        dir = SliderLeft;
                    else
                        dir = SliderRight;
                QPolygon polygon;
                int d = 0;
                switch (dir) {
                  case SliderUp:
                      x2++;
                      y1 = y1 + wi / 2;
                      d =  (wi + 1) / 2 - 1;
                      polygon.setPoints(5, x1, y1, x1, y2, x2, y2, x2, y1, x1 + d,y1 - d);
                      break;
                  case SliderDown:
                      x2++;
                      y2 = y2 - wi/2;
                      d =  (wi + 1) / 2 - 1;
                      polygon.setPoints(5, x1, y1, x1, y2, x1 + d,y2 + d, x2, y2, x2, y1);
                  break;
                      case SliderLeft:
                      d =  (he + 1) / 2 - 1;
                      x1 = x1 + he/2;
                      polygon.setPoints(5, x1, y1, x1 - d, y1 + d, x1,y2, x2, y2, x2, y1);
                      y1--;
                      break;
                  case SliderRight:
                      d =  (he + 1) / 2 - 1;
                      x2 = x2 - he/2;
                      polygon.setPoints(5, x1, y1, x1, y2, x2,y2, x2 + d, y1 + d, x2, y1);
                      y1--;
                      break;
                }
                QBrush oldBrush = painter->brush();
                painter->setPen(Qt::NoPen);
                painter->setBrush(handleBrush);
                Qt::BGMode oldMode = painter->backgroundMode();
                painter->setBackgroundMode(Qt::OpaqueMode);
                painter->drawRect(x1, y1, x2-x1+1, y2-y1+1);
                painter->drawPolygon(polygon);
                QBrush fill = QBrush(option->palette.button().color(), Qt::Dense4Pattern);
                painter->setBrush(oldBrush);
                painter->setBackgroundMode(oldMode);
                if (slider->state & State_Sunken)
                    painter->fillRect(QRectF(x1, y1, x2 - x1 + 1, y2 - y1 + 1),fill);

                if (dir != SliderUp) {
                    painter->setPen(c0);
                    painter->drawLine(x1, y1, x2, y1);
                }
                if (dir != SliderLeft) {
                    painter->setPen(c0);
                    painter->drawLine(x1, y1, x1, y2);
                }
                if (dir != SliderRight) {
                    painter->setPen(c0);
                    painter->drawLine(x2, y1, x2, y2);
                }
                if (dir != SliderDown) {
                    painter->setPen(c0);
                    painter->drawLine(x1, y2, x2, y2);
                }
                switch (dir) {
                  case SliderUp:
                      if (slider->state & State_Sunken)
                          painter->fillRect(QRectF(x1 + 3, y1 - d + 2, x2 - x1 - 4, y1),fill);
                      painter->setPen(c0);
                      painter->drawLine(x1, y1, x1 + d, y1 - d);
                      d = wi - d - 1;
                      painter->drawLine(x2, y1, x2 -d , y1 -d );
                      d--;
                      break;
                  case SliderDown:
                      if (slider->state & State_Sunken)
                      painter->fillRect(QRectF(x1+3, y2 - d, x2 - x1  -4,y2 - 8),fill);
                      painter->setPen(c0);
                      painter->drawLine(x1, y2, x1 + d, y2 + d);
                      d = wi - d - 1;
                      painter->drawLine(x2, y2, x2 - d, y2 + d);
                      d--;
                      break;
                  case SliderLeft:
                      if (slider->state & State_Sunken)
                          painter->fillRect(QRectF(x1 - d + 2, y1 + 2, x1, y2 - y1 - 3),fill);
                      painter->setPen(c0);
                      painter->drawLine(x1, y1, x1 - d, y1 + d);
                      d = he - d - 1;
                      painter->drawLine(x1, y2, x1 - d, y2 - d);
                      d--;
                      break;
                  case SliderRight:
                      if (slider->state & State_Sunken)
                          painter->fillRect(QRectF(x2 - d - 4, y1 + 2, x2 - 4, y2 - y1 - 3),fill);
                      painter->setPen(c0);
                      painter->drawLine(x2, y1, x2 + d, y1 + d);
                      painter->setPen(c0);
                      d = he - d - 1;
                      painter->drawLine(x2, y2, x2 + d, y2 - d);
                      d--;
                      break;
                }
            }
        }
        break;
#endif //QT_NO_SLIDER
#ifndef QT_NO_SCROLLBAR
    case CC_ScrollBar:
        painter->save();
        painter->setPen(option->palette.shadow().color());
        if (d->doubleControls) {
            QPen pen = painter->pen();
            pen.setWidth(2);
            pen.setCapStyle(Qt::SquareCap);
            painter->setPen(pen);
        }
        if (const QStyleOptionSlider *scrollbar = qstyleoption_cast<const QStyleOptionSlider *>(option)) {
            // Make a copy here and reset it for each primitive.
            QBrush fill;
            if (d->smartphone) {
                fill = option->palette.light();
                painter->fillRect(option->rect,fill);
                fill = option->palette.button();
                QImage image;
#ifndef QT_NO_IMAGEFORMAT_XPM
                if (scrollbar->orientation == Qt::Horizontal)
                  image = QImage(vertlines_xpm);
                else
                  image = QImage(horlines_xpm);
#endif
                image.setColor(1, option->palette.button().color().rgb());
                fill.setTextureImage(image);
            }
            else {
              fill = option->palette.light();
            }
            painter->fillRect(option->rect,fill);
            QStyleOptionSlider newScrollbar = *scrollbar;
            State saveFlags = scrollbar->state;
            //Check if the scrollbar is part of an abstractItemView and draw the frame according
            bool drawCompleteFrame = true;
            bool secondScrollBar = false;
            if (widget)
                if (QWidget *parent = widget->parentWidget()) {
                    if (QAbstractScrollArea *abstractScrollArea = qobject_cast<QAbstractScrollArea *>(parent->parentWidget())) {
                        drawCompleteFrame = (abstractScrollArea->frameStyle() == QFrame::NoFrame) ||  (abstractScrollArea->frameStyle() == QFrame::StyledPanel);
                        secondScrollBar = (abstractScrollArea->horizontalScrollBar()->isVisible()
                            && abstractScrollArea->verticalScrollBar()->isVisible()) ;
                    }
#ifndef QT_NO_LISTVIEW
                    if (QListView *listView = qobject_cast<QListView *>(parent->parentWidget()))
                            drawCompleteFrame = false;
#endif
                }
                if (scrollbar->minimum == scrollbar->maximum)
                    saveFlags |= State_Enabled;
                if (scrollbar->subControls & SC_ScrollBarSubLine) {
                    newScrollbar.state = saveFlags;
                    newScrollbar.rect = subControlRect(control, &newScrollbar, SC_ScrollBarSubLine, widget);
                    if (newScrollbar.rect.isValid()) {
                        if (!(scrollbar->activeSubControls & SC_ScrollBarSubLine))
                            newScrollbar.state &= ~(State_Sunken | State_MouseOver);
                        QStyleOption arrowOpt = newScrollbar;
                        if (d->doubleControls)
                            arrowOpt.rect = newScrollbar.rect.adjusted(4, 6, -5, -3);
                        else
                            arrowOpt.rect = newScrollbar.rect.adjusted(5, 6, -4, -3);
                        QBrush fill = option->palette.button();
                        if (newScrollbar.state & State_Sunken)
                            fill = option->palette.shadow();
                        if (scrollbar->orientation == Qt::Horizontal) {
                            painter->fillRect(newScrollbar.rect,fill);
                            QRect r = newScrollbar.rect.adjusted(0,0,1,0);
                            painter->drawLine(r.topRight(), r.bottomRight());
                            if (d->doubleControls)
                               arrowOpt.rect.adjust(0, -2 ,0, -2);
                            drawPrimitive(PE_IndicatorArrowLeft, &arrowOpt, painter, widget);
                        }
                        else {
                            painter->fillRect(newScrollbar.rect,fill);
                            QRect r = newScrollbar.rect.adjusted(0, 0, 0, 1);
                            painter->drawLine(r.bottomLeft(), r.bottomRight());
                            if (drawCompleteFrame)
                                arrowOpt.rect.adjust(-2, 0, -2, 0);
                            if (d->doubleControls)
                               arrowOpt.rect.adjust(0, -4 , 0, -4);
                            if (drawCompleteFrame && d->doubleControls)
                                arrowOpt.rect.adjust(2, 0, 2, 0);
                            drawPrimitive(PE_IndicatorArrowUp, &arrowOpt, painter, widget);
                        }
                    }
                }
                if (scrollbar->subControls & SC_ScrollBarAddLine) {
                    newScrollbar.rect = scrollbar->rect;
                    newScrollbar.state = saveFlags;
                    newScrollbar.rect = subControlRect(control, &newScrollbar, SC_ScrollBarAddLine, widget);
                    if (newScrollbar.rect.isValid()) {
                        if (!(scrollbar->activeSubControls & SC_ScrollBarAddLine))
                            newScrollbar.state &= ~(State_Sunken | State_MouseOver);
                        QStyleOption arrowOpt = newScrollbar;
                        if (d->doubleControls)
                            arrowOpt.rect = newScrollbar.rect.adjusted(4, 0, -5, 3);
                        else
                            arrowOpt.rect = newScrollbar.rect.adjusted(5, 6, -4, -3);
                        QBrush fill = option->palette.button();
                        if (newScrollbar.state & State_Sunken)
                            fill = option->palette.shadow();
                        if (scrollbar->orientation == Qt::Horizontal) {
                            painter->fillRect(newScrollbar.rect,fill);
                            QRect r = newScrollbar.rect.adjusted(0, 0, 0, 0);
                            painter->drawLine(r.topLeft(), r.bottomLeft());
                            if (secondScrollBar)
                                painter->drawLine(r.topRight(), r.bottomRight());
                             if (d->doubleControls)
                               arrowOpt.rect.adjust(0, 4, 0, 4 );
                            drawPrimitive(PE_IndicatorArrowRight, &arrowOpt, painter, widget);
                        }
                        else {
                            painter->fillRect(newScrollbar.rect,fill);
                            QRect r = newScrollbar.rect.adjusted(0, -1, 0, -1);
                            painter->drawLine(r.topLeft(), r.topRight());
                            if (secondScrollBar)
                                painter->drawLine(r.bottomLeft() + QPoint(0,1), r.bottomRight() + QPoint(0, 1));
                            if (drawCompleteFrame)
                                arrowOpt.rect.adjust(-2, 0, -2, 0);
                            if (d->doubleControls)
                              arrowOpt.rect.adjust(1, 0, 1, 0 );
                            if (drawCompleteFrame && d->doubleControls)
                                arrowOpt.rect.adjust(1, 0, 1, 0);
                            drawPrimitive(PE_IndicatorArrowDown, &arrowOpt, painter, widget);
                        }
                    }
                }
                if (scrollbar->subControls & SC_ScrollBarSubPage) {
                    newScrollbar.rect = scrollbar->rect;
                    newScrollbar.state = saveFlags;
                    newScrollbar.rect = subControlRect(control, &newScrollbar, SC_ScrollBarSubPage, widget);
                    if (newScrollbar.rect.isValid()) {
                        if (!(scrollbar->activeSubControls & SC_ScrollBarSubPage))
                            newScrollbar.state &= ~(State_Sunken | State_MouseOver);
                        if (scrollbar->orientation == Qt::Horizontal) {
                            QRect r = newScrollbar.rect.adjusted(0, 0, 0, 0);
                        }
                        else{
                            QRect r = newScrollbar.rect.adjusted(0, 0, 0, 0);
                        }
                    }
                }
                if (scrollbar->subControls & SC_ScrollBarAddPage) {
                    newScrollbar.rect = scrollbar->rect;
                    newScrollbar.state = saveFlags;
                    newScrollbar.rect = subControlRect(control, &newScrollbar, SC_ScrollBarAddPage, widget);
                    if (newScrollbar.rect.isValid()) {
                        if (!(scrollbar->activeSubControls & SC_ScrollBarAddPage))
                            newScrollbar.state &= ~(State_Sunken | State_MouseOver);
                        if (scrollbar->orientation == Qt::Horizontal) {
                            QRect r = newScrollbar.rect.adjusted(0, 0, 0, -1);
                        }
                        else {
                            QRect r = newScrollbar.rect.adjusted(0, 0,- 1, 0);
                        }
                    }
                }
                if (scrollbar->subControls & SC_ScrollBarFirst) {
                    newScrollbar.rect = scrollbar->rect;
                    newScrollbar.state = saveFlags;
                    newScrollbar.rect = subControlRect(control, &newScrollbar, SC_ScrollBarFirst, widget);
                    if (newScrollbar.rect.isValid()) {
                        if (!(scrollbar->activeSubControls & SC_ScrollBarFirst))
                            newScrollbar.state &= ~(State_Sunken | State_MouseOver);
                        QRect r = newScrollbar.rect;
                    }
                }
                if (scrollbar->subControls & SC_ScrollBarLast) {
                    newScrollbar.rect = scrollbar->rect;
                    newScrollbar.state = saveFlags;
                    newScrollbar.rect = subControlRect(control, &newScrollbar, SC_ScrollBarLast, widget);
                    if (newScrollbar.rect.isValid()) {
                        if (!(scrollbar->activeSubControls & SC_ScrollBarLast))
                            newScrollbar.state &= ~(State_Sunken | State_MouseOver);
                        QRect r = newScrollbar.rect;
                    }
                }
                if (scrollbar->subControls & SC_ScrollBarSlider) {
                    newScrollbar.rect = scrollbar->rect;
                    newScrollbar.state = saveFlags;
                    newScrollbar.rect = subControlRect(control, &newScrollbar, SC_ScrollBarSlider, widget);
                    if (newScrollbar.rect.isValid()) {
                        if (!(scrollbar->activeSubControls & SC_ScrollBarSlider))
                            newScrollbar.state &= ~(State_Sunken | State_MouseOver);
                        if (scrollbar->orientation == Qt::Horizontal) {
                            painter->fillRect(newScrollbar.rect,option->palette.button());
                            QRect r = newScrollbar.rect;
                            painter->drawLine(r.topLeft(), r.bottomLeft());
                            painter->drawLine(r.topRight(), r.bottomRight());
                            if (d->smartphone) {
                                painter->drawLine(r.topLeft(), r.topRight());
                                painter->drawLine(r.bottomLeft(), r.bottomRight());
                            }
                        }
                        else {
                            painter->fillRect(newScrollbar.rect,option->palette.button());
                            QRect r = newScrollbar.rect;
                            painter->drawLine(r.topLeft(), r.topRight());
                            painter->drawLine(r.bottomLeft(), r.bottomRight());
                            if (d->smartphone) {
                                painter->drawLine(r.topLeft(), r.bottomLeft());
                                painter->drawLine(r.topRight(), r.bottomRight());
                            }
                        }
                        if (scrollbar->state & State_HasFocus) {
                            QStyleOptionFocusRect fropt;
                            fropt.QStyleOption::operator=(newScrollbar);
                            fropt.rect.setRect(newScrollbar.rect.x() + 2, newScrollbar.rect.y() + 2,
                                newScrollbar.rect.width() - 5,
                                newScrollbar.rect.height() - 5);
                        }
                    }
                }
                int gripMargin = d->doubleControls ? 4 : 2;
                int doubleLines = d->doubleControls ? 2 : 1;
                //If there is a frame around the scrollbar (abstractScrollArea),
                //then the margin is different, because of the missing frame
                int gripMarginFrame = d->doubleControls ? 3 : 1;
                if (drawCompleteFrame)
                    gripMarginFrame = 0;
                //draw grips
                if (!d->smartphone)
                    if (scrollbar->orientation == Qt::Horizontal) {
                        for (int i = -3; i < 3; i += 2) {
                            painter->drawLine(
                                QPoint(newScrollbar.rect.center().x() + i * doubleLines + 1,
                                newScrollbar.rect.top() + gripMargin +gripMarginFrame),
                                QPoint(newScrollbar.rect.center().x() + i * doubleLines + 1,
                                newScrollbar.rect.bottom() - gripMargin));
                        }
                    } else {
                        for (int i = -2; i < 4 ; i += 2) {
                            painter->drawLine(
                                QPoint(newScrollbar.rect.left() + gripMargin + gripMarginFrame ,
                                newScrollbar.rect.center().y() + 1 + i * doubleLines - 1),
                                QPoint(newScrollbar.rect.right() - gripMargin,
                                newScrollbar.rect.center().y() + 1 + i * doubleLines - 1));
                        }
                }
                if (!d->smartphone) {
                    QRect r;
                    if (d->doubleControls)
                        r = option->rect.adjusted(1, 1, -1, 0);
                    else
                        r = option->rect.adjusted(0, 0, -1, 0);
                    if (drawCompleteFrame && d->doubleControls)
                        r.adjust(0, 0, 0, -1);
                    //Check if the scrollbar is part of an abstractItemView and draw the frame according
                    if (drawCompleteFrame)
                        painter->drawRect(r);
                    else
                        if (scrollbar->orientation == Qt::Horizontal)
                            painter->drawLine(r.topLeft(), r.topRight());
                        else
                            painter->drawLine(r.topLeft(), r.bottomLeft());
                }
        }
        painter->restore();
        break;
#endif // QT_NO_SLIDER
    case CC_ToolButton:
        if (const QStyleOptionToolButton *toolbutton
                = qstyleoption_cast<const QStyleOptionToolButton *>(option)) {
            QRect button, menuarea;
            bool isTabWidget = false;
#ifndef QT_NO_TABWIDGET
            if (widget)
                if (QWidget *parent = widget->parentWidget())
                    isTabWidget = (qobject_cast<QTabWidget *>(parent->parentWidget()));
#endif //QT_NO_TABWIDGET

            button = subControlRect(control, toolbutton, SC_ToolButton, widget);
            menuarea = subControlRect(control, toolbutton, SC_ToolButtonMenu, widget);
            State buttonFlags = toolbutton->state;
            if (buttonFlags & State_AutoRaise) {
                if (!(buttonFlags & State_MouseOver)) {
                    buttonFlags &= ~State_Raised;
                }
            }
            State menuFlags = buttonFlags;
            if (toolbutton->activeSubControls & SC_ToolButton)
                buttonFlags |= State_Sunken;
            if (toolbutton->activeSubControls & SC_ToolButtonMenu)
                menuFlags |= State_On;
            QStyleOption tool(0);
            tool.palette = toolbutton->palette;
            if (toolbutton->subControls & SC_ToolButton) {
                    tool.rect = button;
                    tool.state = buttonFlags;
                    drawPrimitive(PE_PanelButtonTool, &tool, painter, widget);
            }
            if (toolbutton->subControls & SC_ToolButtonMenu) {
                tool.rect = menuarea;
                tool.state = buttonFlags & State_Enabled;
                QStyleOption toolMenu(0);
                toolMenu = *toolbutton;
                toolMenu.state = menuFlags;
                if (buttonFlags & State_Sunken)
                  drawPrimitive(PE_PanelButtonTool, &toolMenu, painter, widget);
                QStyleOption arrowOpt(0);
                arrowOpt.rect = tool.rect;
                arrowOpt.palette = tool.palette;
                State flags = State_None;
                if (menuFlags & State_Enabled)
                    flags |= State_Enabled;
                if ((menuFlags & State_On) && !(buttonFlags & State_Sunken)) {
                    flags |= State_Sunken;
                    painter->fillRect(menuarea, option->palette.shadow());
                }
                arrowOpt.state = flags;
                drawPrimitive(PE_IndicatorArrowDown, &arrowOpt, painter, widget);
            }
            if (toolbutton->state & State_HasFocus) {
                QStyleOptionFocusRect focusRect;
                focusRect.QStyleOption::operator=(*toolbutton);
                focusRect.rect.adjust(3, 3, -3, -3);
                if (toolbutton->features & QStyleOptionToolButton::Menu)
                    focusRect.rect.adjust(0, 0, -pixelMetric(QStyle::PM_MenuButtonIndicator,
                                                         toolbutton, widget), 0);
                drawPrimitive(PE_FrameFocusRect, &focusRect, painter, widget);
            }
            QStyleOptionToolButton label = *toolbutton;
            if (isTabWidget)
                label.state = toolbutton->state;
            else
              label.state = toolbutton->state & State_Enabled;
            int fw = pixelMetric(PM_DefaultFrameWidth, option, widget);
            label.rect = button.adjusted(fw, fw, -fw, -fw);
            drawControl(CE_ToolButtonLabel, &label, painter, widget);
        }
        break;

#ifndef QT_NO_GROUPBOX
    case CC_GroupBox:
        if (const QStyleOptionGroupBox *groupBox = qstyleoption_cast<const QStyleOptionGroupBox *>(option)) {
            // Draw frame
            painter->save();
            QFont font = painter->font();
            font.setBold(true);
            painter->setFont(font);
            QStyleOptionGroupBox groupBoxFont = *groupBox;
            groupBoxFont.fontMetrics = QFontMetrics(font);
            QRect textRect = subControlRect(CC_GroupBox, &groupBoxFont, SC_GroupBoxLabel, widget);
            QRect checkBoxRect = subControlRect(CC_GroupBox, option, SC_GroupBoxCheckBox, widget).adjusted(0,0,0,0);
            if (groupBox->subControls & QStyle::SC_GroupBoxFrame) {
                QStyleOptionFrameV2 frame;
                frame.QStyleOption::operator=(*groupBox);
                frame.features = groupBox->features;
                frame.lineWidth = groupBox->lineWidth;
                frame.midLineWidth = groupBox->midLineWidth;
                frame.rect = subControlRect(CC_GroupBox, option, SC_GroupBoxFrame, widget);
                painter->save();
                QRegion region(groupBox->rect);
                if (!groupBox->text.isEmpty()) {
                    bool ltr = groupBox->direction == Qt::LeftToRight;
                    QRect finalRect = checkBoxRect.united(textRect);
                    if (groupBox->subControls & QStyle::SC_GroupBoxCheckBox)
                        finalRect.adjust(ltr ? -4 : 0, 0, ltr ? 0 : 4, 0);
                    region -= finalRect;
                }
                drawPrimitive(PE_FrameGroupBox, &frame, painter, widget);
                painter->restore();
            }
             // Draw checkbox
            if (groupBox->subControls & SC_GroupBoxCheckBox) {
                QStyleOptionButton box;
                box.QStyleOption::operator=(*groupBox);
                box.rect = checkBoxRect;
                drawPrimitive(PE_IndicatorCheckBox, &box, painter, widget);
            }
            // Draw title
            if ((groupBox->subControls & QStyle::SC_GroupBoxLabel) && !groupBox->text.isEmpty()) {
                QColor textColor = groupBox->textColor;
                if (textColor.isValid())
                    painter->setPen(textColor);
                else
                    painter->setPen(groupBox->palette.link().color());
                painter->setPen(groupBox->palette.link().color());

                int alignment = int(groupBox->textAlignment);
                if (!styleHint(QStyle::SH_UnderlineShortcut, option, widget))
                    alignment |= Qt::TextHideMnemonic;

                if (groupBox->state & State_Enabled)
                    drawItemText(painter, textRect,  Qt::TextShowMnemonic | Qt::AlignHCenter | alignment,
                    groupBox->palette, true, groupBox->text,
                    textColor.isValid() ? QPalette::NoRole : QPalette::Link);
                else
                    drawItemText(painter, textRect,  Qt::TextShowMnemonic | Qt::AlignHCenter | alignment,
                    groupBox->palette, true, groupBox->text, QPalette::Mid);
                if (groupBox->state & State_HasFocus) {
                    QStyleOptionFocusRect fropt;
                    fropt.QStyleOption::operator=(*groupBox);
                    fropt.rect = textRect;
                    drawPrimitive(PE_FrameFocusRect, &fropt, painter, widget);
                }
            }
            painter->restore();
        }
        break;
#endif //QT_NO_GROUPBOX

#ifndef QT_NO_COMBOBOX
    case CC_ComboBox:
        if (const QStyleOptionComboBox *cmb = qstyleoption_cast<const QStyleOptionComboBox *>(option)) {
            QBrush editBrush = cmb->palette.brush(QPalette::Base);
            if ((cmb->subControls & SC_ComboBoxFrame) && cmb->frame)
                qDrawPlainRect(painter, option->rect, option->palette.shadow().color(), pixelMetric(PM_ComboBoxFrameWidth, option, widget), &editBrush);
            else
                painter->fillRect(option->rect, editBrush);
                State flags = State_None;
                QRect ar = subControlRect(CC_ComboBox, cmb, SC_ComboBoxArrow, widget);
                if ((option->state & State_On)) {
                  painter->fillRect(ar.adjusted(0, 0, 1, 1),cmb->palette.brush(QPalette::Shadow));
                }
                if (d->doubleControls)
                  ar.adjust(5, 0, 5, 0);
                else
                  ar.adjust(2, 0, -2, 0);
                if (option->state & State_Enabled)
                    flags |= State_Enabled;
                if (option->state & State_On)
                    flags |= State_Sunken;
                QStyleOption arrowOpt(0);
                arrowOpt.rect = ar;
                arrowOpt.palette = cmb->palette;
                arrowOpt.state = flags;
                drawPrimitive(PrimitiveElement(PE_IndicatorArrowDownBig), &arrowOpt, painter, widget);
            if (cmb->subControls & SC_ComboBoxEditField) {
                QRect re = subControlRect(CC_ComboBox, cmb, SC_ComboBoxEditField, widget);
                if (cmb->state & State_HasFocus && !cmb->editable)
                    painter->fillRect(re.x(), re.y(), re.width(), re.height(),
                 cmb->palette.brush(QPalette::Highlight));
                 if (cmb->state & State_HasFocus) {
                    painter->setPen(cmb->palette.highlightedText().color());
                    painter->setBackground(cmb->palette.highlight());
                 } else {
                    painter->setPen(cmb->palette.text().color());
                    painter->setBackground(cmb->palette.background());
                 }
                 if (cmb->state & State_HasFocus && !cmb->editable) {
                    QStyleOptionFocusRect focus;
                    focus.QStyleOption::operator=(*cmb);
                    focus.rect = subElementRect(SE_ComboBoxFocusRect, cmb, widget);
                    focus.state |= State_FocusAtBorder;
                    focus.backgroundColor = cmb->palette.highlight().color();
                    if ((option->state & State_On))
                      drawPrimitive(PE_FrameFocusRect, &focus, painter, widget);
               }
            }
        }
        break;
#endif // QT_NO_COMBOBOX


#ifndef QT_NO_SPINBOX
    case CC_SpinBox:
        if (const QStyleOptionSpinBox *spinBox = qstyleoption_cast<const QStyleOptionSpinBox *>(option)) {
            QStyleOptionSpinBox copy = *spinBox;
            //PrimitiveElement primitiveElement;
            int primitiveElement;

            if (spinBox->frame && (spinBox->subControls & SC_SpinBoxFrame)) {
                QRect r = subControlRect(CC_SpinBox, spinBox, SC_SpinBoxFrame, widget);
                qDrawPlainRect(painter, r, option->palette.shadow().color(),pixelMetric(PM_SpinBoxFrameWidth, option, widget),0);
            }
            QPalette shadePal(option->palette);
            shadePal.setColor(QPalette::Button, option->palette.light().color());
            shadePal.setColor(QPalette::Light, option->palette.base().color());
            if (spinBox->subControls & SC_SpinBoxUp) {
                copy.subControls = SC_SpinBoxUp;
                QPalette pal2 = spinBox->palette;
                if (!(spinBox->stepEnabled & QAbstractSpinBox::StepUpEnabled)) {
                    pal2.setCurrentColorGroup(QPalette::Disabled);
                    copy.state &= ~State_Enabled;
                }
                copy.palette = pal2;
                if (spinBox->activeSubControls == SC_SpinBoxUp && (spinBox->state & State_Sunken)) {
                    copy.state |= State_On;
                    copy.state |= State_Sunken;
                } else {
                    copy.state |= State_Raised;
                    copy.state &= ~State_Sunken;
                }
                primitiveElement = (spinBox->buttonSymbols == QAbstractSpinBox::PlusMinus ? PE_IndicatorArrowUpBig
                    : PE_IndicatorArrowUpBig);
                copy.rect = subControlRect(CC_SpinBox, spinBox, SC_SpinBoxUp, widget);
                if (copy.state & (State_Sunken | State_On))
                    qDrawPlainRect(painter, copy.rect, option->palette.shadow().color(),pixelMetric(PM_SpinBoxFrameWidth, option, widget), &copy.palette.brush(QPalette::Shadow));
                else
                  qDrawPlainRect(painter, copy.rect, option->palette.shadow().color(),pixelMetric(PM_SpinBoxFrameWidth, option, widget), &copy.palette.brush(QPalette::Base));
                copy.rect.adjust(pixelMetric(PM_SpinBoxFrameWidth, option, widget), 0, -pixelMetric(PM_SpinBoxFrameWidth, option, widget), 0);
                drawPrimitive(PrimitiveElement(primitiveElement), &copy, painter, widget);
            }
            if (spinBox->subControls & SC_SpinBoxDown) {
                copy.subControls = SC_SpinBoxDown;
                copy.state = spinBox->state;
                QPalette pal2 = spinBox->palette;
                if (!(spinBox->stepEnabled & QAbstractSpinBox::StepDownEnabled)) {
                    pal2.setCurrentColorGroup(QPalette::Disabled);
                    copy.state &= ~State_Enabled;
                }
                copy.palette = pal2;
                if (spinBox->activeSubControls == SC_SpinBoxDown && (spinBox->state & State_Sunken)) {
                    copy.state |= State_On;
                    copy.state |= State_Sunken;
                } else {
                    copy.state |= State_Raised;
                    copy.state &= ~State_Sunken;
                }
                primitiveElement = (spinBox->buttonSymbols == QAbstractSpinBox::PlusMinus ? PE_IndicatorArrowDownBig
                                                                       : PE_IndicatorArrowDownBig);
                copy.rect = subControlRect(CC_SpinBox, spinBox, SC_SpinBoxDown, widget);
                qDrawPlainRect(painter, copy.rect, option->palette.shadow().color(),pixelMetric(PM_SpinBoxFrameWidth, option, widget), &copy.palette.brush(QPalette::Base));
                if (copy.state & (State_Sunken | State_On))
                    qDrawPlainRect(painter, copy.rect, option->palette.shadow().color(),pixelMetric(PM_SpinBoxFrameWidth, option, widget), &copy.palette.brush(QPalette::Shadow));
                else
                    qDrawPlainRect(painter, copy.rect, option->palette.shadow().color(),pixelMetric(PM_SpinBoxFrameWidth, option, widget), &copy.palette.brush(QPalette::Base));
                copy.rect.adjust(3, 0, -4, 0);
                if (primitiveElement == PE_IndicatorArrowUp || primitiveElement == PE_IndicatorArrowDown) {
                    int frameWidth = pixelMetric(PM_SpinBoxFrameWidth, option, widget);
                    copy.rect = copy.rect.adjusted(frameWidth, frameWidth, -frameWidth, -frameWidth);
                    drawPrimitive(PrimitiveElement(primitiveElement), &copy, painter, widget);
                }
                else {
                    drawPrimitive(PrimitiveElement(primitiveElement), &copy, painter, widget);
                }
                if (spinBox->frame && (spinBox->subControls & SC_SpinBoxFrame)) {
                    QRect r = subControlRect(CC_SpinBox, spinBox, SC_SpinBoxEditField, widget);
                }
            }
        }
        break;
#endif // QT_NO_SPINBOX

    default:
        QWindowsStyle::drawComplexControl(control, option, painter, widget);
        break;
    }
}

QSize QWindowsMobileStyle::sizeFromContents(ContentsType type, const QStyleOption *option,
                                            const QSize &size, const QWidget *widget) const {

    QSize newSize = QWindowsStyle::sizeFromContents(type, option, size, widget);
    switch (type) {
    case CT_PushButton:
       if (const QStyleOptionButton *button = qstyleoption_cast<const QStyleOptionButton *>(option)) {
            newSize = QWindowsStyle::sizeFromContents(type, option, size, widget);
            int w = newSize.width(),
                h = newSize.height();
            int defwidth = 0;
            if (button->features & QStyleOptionButton::AutoDefaultButton)
                defwidth = 2 * pixelMetric(PM_ButtonDefaultIndicator, button, widget);
            if (w < 75 + defwidth && button->icon.isNull())
                w = 75 + defwidth;
            if (h < 23 + defwidth)
                h = 23 + defwidth;
            newSize = QSize(w + 4, h + 4);
        }
        break;

#ifndef QT_NO_GROUPBOX
    case CT_GroupBox:
        if (const QGroupBox *grb = static_cast<const QGroupBox *>(widget)) {
            newSize = size + QSize(!grb->isFlat() ? 16 : 0, !grb->isFlat() ? 16 : 0);
        }
        break;
#endif // QT_NO_GROUPBOX

    case CT_RadioButton:
    case CT_CheckBox:
        newSize = size;
        if (const QStyleOptionButton *button = qstyleoption_cast<const QStyleOptionButton *>(option)) {
            bool isRadio = (type == CT_RadioButton);
            QRect irect = visualRect(button->direction, button->rect,
                                     subElementRect(isRadio ? SE_RadioButtonIndicator
                                                            : SE_CheckBoxIndicator, button, widget));
            int h = pixelMetric(isRadio ? PM_ExclusiveIndicatorHeight
                                        : PM_IndicatorHeight, button, widget);
            int margins = (!button->icon.isNull() && button->text.isEmpty()) ? 0 : 10;
             if (d_func()->doubleControls)
                 margins *= 2;
            newSize += QSize(irect.right() + margins, 1);
            newSize.setHeight(qMax(newSize.height(), h));
        }
        break;
#ifndef QT_NO_COMBOBOX
    case CT_ComboBox:
        if (const QStyleOptionComboBox *comboBox = qstyleoption_cast<const QStyleOptionComboBox *>(option)) {
            int fw = comboBox->frame ? pixelMetric(PM_ComboBoxFrameWidth, option, widget) * 2 : 0;
            newSize = QSize(newSize.width() + fw + 9, newSize.height() + fw-4); //Nine is a magic Number - See CommonStyle for real magic (23)
        }
        break;
#endif
#ifndef QT_NO_SPINBOX
    case CT_SpinBox:
        if (const QStyleOptionSpinBox *spinBox = qstyleoption_cast<const QStyleOptionSpinBox *>(option)) {
            int fw = spinBox->frame ? pixelMetric(PM_SpinBoxFrameWidth, option, widget) * 2 : 0;
            newSize = QSize(newSize.width() + fw-5, newSize.height() + fw-6);
        }
        break;
#endif
#ifndef QT_NO_LINEEDIT
    case CT_LineEdit:
        newSize += QSize(0,1);
        break;
#endif
    case CT_ToolButton:
        newSize = QSize(newSize.width() + 1, newSize.height());
        break;
    case CT_TabBarTab:
        newSize += QSize(0,0);
        break;
    case CT_HeaderSection:
        newSize += QSize(4, 2);
        break;
    default:
        break;
    }
    return newSize;
}

QRect QWindowsMobileStyle::subElementRect(SubElement element, const QStyleOption *option, const QWidget *widget) const {

    QWindowsMobileStylePrivate *d = const_cast<QWindowsMobileStylePrivate*>(d_func());
    QRect rect = QWindowsStyle::subElementRect(element, option, widget);
    switch (element) {
#ifndef QT_NO_TABWIDGET
    case SE_TabWidgetTabBar:
        if (d->doubleControls)
            rect.adjust(-2, 0, 2, 0);
        else
            rect.adjust(-2, 0, 2, 0);
        break;
#endif //QT_NO_TABWIDGET
    case SE_CheckBoxFocusRect:
        rect.adjust(1,0,-2,-1);
        break;
    case SE_RadioButtonFocusRect:
        rect.adjust(1,1,-2,-2);
        break;
    default:
        break;
        #ifndef QT_NO_SLIDER
    case SE_SliderFocusRect:
        if (const QStyleOptionSlider *slider = qstyleoption_cast<const QStyleOptionSlider *>(option)) {
            rect = slider->rect;
        }
        break;
    case SE_PushButtonFocusRect:
        if (d->doubleControls)
            rect.adjust(-1, -1, 0, 0);
        break;
#endif // QT_NO_SLIDER
    }
    return rect;
}

QRect QWindowsMobileStyle::subControlRect(ComplexControl control, const QStyleOptionComplex *option,
                                          SubControl subControl, const QWidget *widget) const {

    QWindowsMobileStylePrivate *d = const_cast<QWindowsMobileStylePrivate*>(d_func());

    QRect rect = QCommonStyle::subControlRect(control, option, subControl, widget);
    switch (control) {

#ifndef QT_NO_SCROLLBAR
    case CC_ScrollBar:
        if (const QStyleOptionSlider *scrollbar = qstyleoption_cast<const QStyleOptionSlider *>(option)) {
            int sliderButtonExtent = pixelMetric(PM_ScrollBarExtent, scrollbar, widget);
            int sliderlen;
            float stretchFactor = 1.4f;
            int sliderButtonExtentDir = int (sliderButtonExtent * stretchFactor);
            int maxlen = ((scrollbar->orientation == Qt::Horizontal) ?
                scrollbar->rect.width() : scrollbar->rect.height()) - (sliderButtonExtentDir * 2);
            // calculate slider length
            if (scrollbar->maximum != scrollbar->minimum) {
                uint range = scrollbar->maximum - scrollbar->minimum;
                sliderlen = (qint64(scrollbar->pageStep) * maxlen) / (range + scrollbar->pageStep);

                int slidermin = pixelMetric(PM_ScrollBarSliderMin, scrollbar, widget);
                if (sliderlen < slidermin || range > INT_MAX / 2)
                    sliderlen = slidermin;
                if (sliderlen > maxlen)
                    sliderlen = maxlen;
            } else {
                sliderlen = maxlen;
            }
            int sliderstart = sliderButtonExtentDir + sliderPositionFromValue(scrollbar->minimum,
                scrollbar->maximum,
                scrollbar->sliderPosition,
                maxlen - sliderlen,
                scrollbar->upsideDown);
            if (d->smartphone) {
                sliderstart -= sliderButtonExtentDir;
                sliderlen += 2*sliderButtonExtent;
            }
            switch (subControl) {
            case SC_ScrollBarSubLine:            // top/left button
                if (scrollbar->orientation == Qt::Horizontal) {
                    int buttonWidth = qMin(scrollbar->rect.width() / 2, sliderButtonExtentDir );
                    rect.setRect(0, 0, buttonWidth, sliderButtonExtent);
                } else {
                    int buttonHeight = qMin(scrollbar->rect.height() / 2, sliderButtonExtentDir);
                    rect.setRect(0, 0, sliderButtonExtent, buttonHeight);
                }
                if (d->smartphone)
                    rect.setRect(0, 0, 0, 0);
                break;
            case SC_ScrollBarAddLine:            // bottom/right button
                if (scrollbar->orientation == Qt::Horizontal) {
                    int buttonWidth = qMin(scrollbar->rect.width()/2, sliderButtonExtentDir);
                    rect.setRect(scrollbar->rect.width() - buttonWidth, 0, buttonWidth, sliderButtonExtent);
                } else {
                    int buttonHeight = qMin(scrollbar->rect.height()/2, sliderButtonExtentDir );
                    rect.setRect(0, scrollbar->rect.height() - buttonHeight, sliderButtonExtent, buttonHeight);
                }
                if (d->smartphone)
                    rect.setRect(0, 0, 0, 0);
                break;
            case SC_ScrollBarSubPage:            // between top/left button and slider
                if (scrollbar->orientation == Qt::Horizontal)
                    if (d->smartphone)
                       rect.setRect(0, 0, sliderstart, sliderButtonExtent);
                    else
                      rect.setRect(sliderButtonExtent, 0, sliderstart - sliderButtonExtent, sliderButtonExtent);
                else
                    if (d->smartphone)
                      rect.setRect(0, 0, sliderButtonExtent, sliderstart);
                    else
                      rect.setRect(0, sliderButtonExtent, sliderButtonExtent, sliderstart - sliderButtonExtent);
                break;
            case SC_ScrollBarAddPage:            // between bottom/right button and slider
                if (scrollbar->orientation == Qt::Horizontal)
                    if (d->smartphone)
                      rect.setRect(sliderstart + sliderlen, 0,
                                  maxlen - sliderstart - sliderlen + 2*sliderButtonExtent, sliderButtonExtent);
                    else
                      rect.setRect(sliderstart + sliderlen, 0,
                                   maxlen - sliderstart - sliderlen + sliderButtonExtent, sliderButtonExtent);
                else
                    if (d->smartphone)
                      rect.setRect(0, sliderstart + sliderlen, sliderButtonExtent,
                                  maxlen - sliderstart - sliderlen + 2*sliderButtonExtent);
                    else
                       rect.setRect(0, sliderstart + sliderlen, sliderButtonExtent,
                                  maxlen - sliderstart - sliderlen + sliderButtonExtent);
                break;
            case SC_ScrollBarGroove:
                if (scrollbar->orientation == Qt::Horizontal)
                    rect.setRect(sliderButtonExtent, 0, scrollbar->rect.width() - sliderButtonExtent * 2,
                                scrollbar->rect.height());
                else
                    rect.setRect(0, sliderButtonExtent, scrollbar->rect.width(),
                                scrollbar->rect.height() - sliderButtonExtent * 2);
                break;
            case SC_ScrollBarSlider:
                if (scrollbar->orientation == Qt::Horizontal)
                    rect.setRect(sliderstart, 0, sliderlen, sliderButtonExtent);
                else
                    rect.setRect(0, sliderstart, sliderButtonExtent, sliderlen);
                break;
            default:
                break;
            }
            rect = visualRect(scrollbar->direction, scrollbar->rect, rect);
        }
        break;
#endif // QT_NO_SCROLLBAR



#ifndef QT_NO_TOOLBUTTON
    case CC_ToolButton:
        if (const QStyleOptionToolButton *toolButton = qstyleoption_cast<const QStyleOptionToolButton *>(option)) {
            int mbi = pixelMetric(PM_MenuButtonIndicator, toolButton, widget);
            rect = toolButton->rect;
            switch (subControl) {
            case SC_ToolButton:
                if ((toolButton->features
                     & (QStyleOptionToolButton::Menu | QStyleOptionToolButton::PopupDelay))
                    == QStyleOptionToolButton::Menu)
                    rect.adjust(0, 0, -mbi, 0);
                break;
            case SC_ToolButtonMenu:
                if ((toolButton->features
                     & (QStyleOptionToolButton::Menu | QStyleOptionToolButton::PopupDelay))
                    == QStyleOptionToolButton::Menu)
                    rect.adjust(rect.width() - mbi, 1, 0, 1);
                break;
            default:
                break;
            }
            rect = visualRect(toolButton->direction, toolButton->rect, rect);
        }
        break;
#endif // QT_NO_TOOLBUTTON

#ifndef QT_NO_SLIDER
        case CC_Slider:
        if (const QStyleOptionSlider *slider = qstyleoption_cast<const QStyleOptionSlider *>(option)) {
            int tickOffset = pixelMetric(PM_SliderTickmarkOffset, slider, widget);
            int thickness = pixelMetric(PM_SliderControlThickness, slider, widget);
            switch (subControl) {
            case SC_SliderHandle: {
                int sliderPos = 0;
                int len = pixelMetric(PM_SliderLength, slider, widget);
                bool horizontal = slider->orientation == Qt::Horizontal;
                sliderPos = sliderPositionFromValue(slider->minimum, slider->maximum,
                                                    slider->sliderPosition,
                                                    (horizontal ? slider->rect.width()
                                                                : slider->rect.height()) - len,
                                                    slider->upsideDown);
                if (horizontal)
                    rect.setRect(slider->rect.x() + sliderPos, slider->rect.y() + tickOffset, len, thickness);
                else
                    rect.setRect(slider->rect.x() + tickOffset, slider->rect.y() + sliderPos, thickness, len);
                break; }
            default:
                break;
            }
            rect = visualRect(slider->direction, slider->rect, rect);
        }
        break;
#endif //QT_NO_SLIDER
#ifndef QT_NO_COMBOBOX
    case CC_ComboBox:
        if (const QStyleOptionComboBox *comboBox = qstyleoption_cast<const QStyleOptionComboBox *>(option)) {
        int x = comboBox->rect.x(),
            y = comboBox->rect.y(),
            wi = comboBox->rect.width(),
            he = comboBox->rect.height();
        int xpos = x;
        int margin = comboBox->frame ? (d->doubleControls ? 2 : 1) : 0;
        int bmarg = comboBox->frame ? (d->doubleControls ? 2 : 1) :  0;
        if (subControl == SC_ComboBoxArrow)
          xpos += wi - int((he - 2*bmarg)*0.9) - bmarg;
        else
          xpos += wi - (he - 2*bmarg) - bmarg;
        switch (subControl) {
        case SC_ComboBoxArrow:
            rect.setRect(xpos, y + bmarg, he - 2*bmarg, he - 2*bmarg);
            rect.setRect(xpos, y + bmarg, int((he - 2*bmarg)), he - 2*bmarg);
            break;
         case SC_ComboBoxEditField:
             rect.setRect(x + margin+4, y + margin+2, wi - 4 * margin - int((he - 2*bmarg) * 0.84f) -2, he - 2 * margin-4);
             break;
          case SC_ComboBoxFrame:
              rect = comboBox->rect;
              break;
        default:
        break;
        }
    }
#endif //QT_NO_COMBOBOX
#ifndef QT_NO_SPINBOX
    case CC_SpinBox:
        if (const QStyleOptionSpinBox *spinBox = qstyleoption_cast<const QStyleOptionSpinBox *>(option)) {
            QSize bs;
            int fw = spinBox->frame ? pixelMetric(PM_SpinBoxFrameWidth, spinBox, widget) : 0;
            bs.setHeight(qMax(d->doubleControls ? 28 : 14, (spinBox->rect.height())));
            // 1.6 -approximate golden mean
            bs.setWidth(qMax(d->doubleControls ? 28 : 14, qMin((bs.height()*7/8), (spinBox->rect.width() / 8))));
            bs = bs.expandedTo(QApplication::globalStrut());
            int x, lx, rx;
            x = spinBox->rect.width() - bs.width()*2;
            lx = fw;
            rx = x - fw;
            switch (subControl) {
            case SC_SpinBoxUp:
                rect = QRect(x+pixelMetric(PM_SpinBoxFrameWidth, option, widget), 0 , bs.width(), bs.height());
                break;
            case SC_SpinBoxDown:
                rect = QRect(x + bs.width(), 0, bs.width(), bs.height());
                break;
            case SC_SpinBoxEditField:
                    rect = QRect(lx, fw, rx-2, spinBox->rect.height() - 2*fw);
                break;
            case SC_SpinBoxFrame:
                rect = spinBox->rect;
            default:
                break;
          }
          rect = visualRect(spinBox->direction, spinBox->rect, rect);
        }
        break;
#endif // Qt_NO_SPINBOX
#ifndef QT_NO_GROUPBOX
    case CC_GroupBox: {
        if (const QStyleOptionGroupBox *groupBox = qstyleoption_cast<const QStyleOptionGroupBox *>(option)) {
            switch (subControl) {
            case SC_GroupBoxFrame:
                // FALL THROUGH
            case SC_GroupBoxContents: {
                int topMargin = 0;
                int topHeight = 0;
                int bottomMargin = 0;
                int labelMargin = 2;

                QRect frameRect = groupBox->rect;
                int verticalAlignment = styleHint(SH_GroupBox_TextLabelVerticalAlignment, groupBox, widget);
                if (groupBox->text.size()) {
                    topHeight = groupBox->fontMetrics.height();
                    if (verticalAlignment & Qt::AlignVCenter)
                        topMargin = topHeight+5;
                    else if (verticalAlignment & Qt::AlignTop)
                        topMargin = -topHeight+5;
                }
                if (subControl == SC_GroupBoxFrame) {
                    frameRect.setTop(topMargin);
                    frameRect.setBottom(frameRect.height() + bottomMargin);
                    rect = frameRect;
                    break;
                }
                int frameWidth = 0;
                if (groupBox->text.size()) {
                    frameWidth = pixelMetric(PM_DefaultFrameWidth, groupBox, widget);
                    rect = frameRect.adjusted(frameWidth, frameWidth + topHeight + labelMargin, -frameWidth, -frameWidth);
                }
                else {
                   rect = groupBox->rect;
                }
                break;
            }
            case SC_GroupBoxCheckBox:
                // FALL THROUGH
            case SC_GroupBoxLabel: {
                QFontMetrics fontMetrics = groupBox->fontMetrics;
                int h = fontMetrics.height();
                int textWidth = fontMetrics.size(Qt::TextShowMnemonic, groupBox->text + QLatin1Char(' ')).width();
                int margX = (groupBox->features & QStyleOptionFrameV2::Flat) ? 0 : 2;
                int margY = (groupBox->features & QStyleOptionFrameV2::Flat) ? 0 : 2;
                rect = groupBox->rect.adjusted(margX, margY, -margX, 0);
                if (groupBox->text.size())
                    rect.setHeight(h);
                else
                  rect.setHeight(0);
                int indicatorWidth = pixelMetric(PM_IndicatorWidth, option, widget);
                int indicatorSpace = pixelMetric(PM_CheckBoxLabelSpacing, option, widget) - 1;
                bool hasCheckBox = groupBox->subControls & QStyle::SC_GroupBoxCheckBox;
                int checkBoxSize = hasCheckBox ? (indicatorWidth + indicatorSpace) : 0;

                // Adjusted rect for label + indicatorWidth + indicatorSpace
                QRect totalRect = alignedRect(groupBox->direction, groupBox->textAlignment,
                                              QSize(textWidth + checkBoxSize, h), rect);

                // Adjust totalRect if checkbox is set
                if (hasCheckBox) {
                    bool ltr = groupBox->direction == Qt::LeftToRight;
                    int left = 2;
                    // Adjust for check box
                    if (subControl == SC_GroupBoxCheckBox) {
                        int indicatorHeight = pixelMetric(PM_IndicatorHeight, option, widget);
                        left = ltr ? totalRect.left() : (totalRect.right() - indicatorWidth);
                        int top = totalRect.top() + (fontMetrics.height() - indicatorHeight) / 2;
                        totalRect.setRect(left, top, indicatorWidth, indicatorHeight);
                    // Adjust for label
                    } else {
                        left = ltr ? (totalRect.left() + checkBoxSize - 2) : totalRect.left();
                        totalRect.setRect(left, totalRect.top(),
                                          totalRect.width() - checkBoxSize, totalRect.height());
                    }
                }
                  if ((subControl== SC_GroupBoxLabel))
                      totalRect.adjust(-2,0,6,0);
                  rect = totalRect;
                break;
            }
            default:
                break;
            }
        }
        break;
    }
#endif // QT_NO_GROUPBOX
    default:
        break;
    }
    return rect;
}

QPalette QWindowsMobileStyle::standardPalette() const {
    QPalette palette (Qt::black,QColor(198, 195, 198), QColor(222, 223, 222 ),
                      QColor(132, 130, 132), QColor(198, 195, 198), Qt::black, Qt::white, Qt::white, QColor(198, 195, 198));
    palette.setColor(QPalette::Window, QColor(206, 223, 239));
    palette.setColor(QPalette::Link, QColor(8,77,123)); //Alternate TextColor for labels...
    palette.setColor(QPalette::Base, Qt::white);
    palette.setColor(QPalette::Button, QColor(206, 223, 239));
    palette.setColor(QPalette::Highlight, QColor(49, 146, 214));
    palette.setColor(QPalette::Light, Qt::white);
    palette.setColor(QPalette::Text, Qt::black);
    palette.setColor(QPalette::ButtonText, Qt::black);
    palette.setColor(QPalette::Midlight, QColor(222, 223, 222 ));
    palette.setColor(QPalette::Dark, QColor(132, 130, 132));
    palette.setColor(QPalette::Mid, QColor(189, 190, 189));
    palette.setColor(QPalette::Shadow, QColor(0, 0, 0));
    palette.setColor(QPalette::BrightText, QColor(33, 162, 33)); //color for ItemView checked indicator (arrow)
    return palette;
}


/*! \reimp */
void QWindowsMobileStyle::polish(QApplication *application) {
    QWindowsStyle::polish(application);
}

/*! \reimp */
void QWindowsMobileStyle::polish(QWidget *widget) {

#ifndef QT_NO_TOOLBAR
    if (QToolBar *toolBar = qobject_cast<QToolBar*>(widget)) {
        QPalette pal = toolBar->palette();
        pal.setColor(QPalette::Background, pal.button().color());
        toolBar->setPalette(pal);
    }
    else
#endif //QT_NO_TOOLBAR

#ifndef QT_NO_PROPERTIES
        if (QAbstractButton *pushButton = qobject_cast<QAbstractButton*>(widget)) {
            QVariant oldFont = widget->property("_q_styleWindowsMobileFont");
            if (!oldFont.isValid()) {
                QFont f = pushButton->font();
                widget->setProperty("_q_styleWindowsMobileFont", f);
                f.setBold(true);
                int p = f.pointSize();
                if (p > 2)
                    f.setPointSize(p-1);
                pushButton->setFont(f);
            }
        }
#endif
        QWindowsStyle::polish(widget);
}

void QWindowsMobileStyle::unpolish(QWidget *widget)
{
#ifndef QT_NO_PROPERTIES
    if (QAbstractButton *pushButton = qobject_cast<QAbstractButton*>(widget)) {
        QVariant oldFont = widget->property("_q_styleWindowsMobileFont");
        if (oldFont.isValid()) {
            widget->setFont(qVariantValue<QFont>(oldFont));
        widget->setProperty("_q_styleWindowsMobileFont", QVariant());
        }
    }
#endif
    QWindowsStyle::unpolish(widget);
}

void QWindowsMobileStyle::unpolish(QApplication *app)
{
    QWindowsStyle::unpolish(app);
}

/*! \reimp */
void QWindowsMobileStyle::polish(QPalette &palette) {
    QWindowsStyle::polish(palette);
}

int QWindowsMobileStyle::pixelMetric(PixelMetric pm, const QStyleOption *opt, const QWidget *widget) const {

    QWindowsMobileStylePrivate *d = const_cast<QWindowsMobileStylePrivate*>(d_func());
    int ret;

    switch (pm) {
    case PM_DefaultTopLevelMargin:
        ret =0;
        break;
    case PM_DefaultLayoutSpacing:
        d->doubleControls ? ret = 8 : ret = 4;
        break;
    case PM_HeaderMargin:
        d->doubleControls ? ret = 2 : ret = 1;
        break;
    case PM_DefaultChildMargin:
        d->doubleControls ? ret = 10 : ret = 5;
        break;
    case PM_ToolBarSeparatorExtent:
        d->doubleControls ? ret = 6 : ret = 3;
        break;
    case PM_DefaultFrameWidth:
        d->doubleControls ? ret = 2 : ret = 1;
        break;
    case PM_MenuVMargin:
        ret = 1;
        break;
    case PM_MenuHMargin:
        ret = 1;
        break;
    case PM_MenuButtonIndicator:
        ret = d->doubleControls ? 24 : 14;
        break;
    case PM_ComboBoxFrameWidth:
        d->doubleControls ? ret = 2 : ret = 1;
        break;
    case PM_SpinBoxFrameWidth:
        d->doubleControls ? ret = 2 : ret = 1;
        break;
    case PM_ButtonDefaultIndicator:
    case PM_ButtonShiftHorizontal:
    case PM_ButtonShiftVertical:
        d->doubleControls ? ret = 2 : ret = 1;
        break;
#ifndef QT_NO_TABBAR
   case PM_TabBarTabShiftHorizontal:
        ret = 0;
        break;
    case PM_TabBarTabShiftVertical:
        ret = 0;
        break;
#endif
    case PM_MaximumDragDistance:
        ret = 60;
        break;
   case PM_TabBarTabVSpace:
        ret = d->doubleControls ? 12 : 6;
        break;
   case PM_TabBarBaseHeight:
       ret = 0;
       break;
      case PM_IndicatorWidth:
          ret = d->doubleControls ? windowsMobileIndicatorSize * 2 : windowsMobileIndicatorSize;
        break;
    case PM_IndicatorHeight:
        ret = d->doubleControls ? windowsMobileIndicatorSize * 2 : windowsMobileIndicatorSize;
        break;
    case PM_ExclusiveIndicatorWidth:
        ret = d->doubleControls ? windowsMobileExclusiveIndicatorSize * 2 + 4: windowsMobileExclusiveIndicatorSize + 2;
        break;
    case PM_ExclusiveIndicatorHeight:
        ret = d->doubleControls ? windowsMobileExclusiveIndicatorSize * 2 + 4: windowsMobileExclusiveIndicatorSize + 2;
        break;
#ifndef QT_NO_SLIDER
    case PM_SliderLength:
        ret = d->doubleControls ? 16 : 8;
        break;
    case PM_FocusFrameHMargin:
        ret = d->doubleControls ? 1 : 2;
        break;
    case PM_SliderThickness:
        ret = d->doubleControls ? windowsMobileSliderThickness * 2: windowsMobileSliderThickness;
        break;
     case PM_TabBarScrollButtonWidth:
         ret = d->doubleControls ? 14 * 2 : 18;
      case PM_CheckBoxLabelSpacing:
      case PM_RadioButtonLabelSpacing:
        ret = d->doubleControls ? 6 * 2 : 6;
        break;
        break;
        // Returns the number of pixels to use for the business part of the
        // slider (i.e., the non-tickmark portion). The remaining space is shared
        // equally between the tickmark regions.
    case PM_SliderControlThickness:
        if (const QStyleOptionSlider *sl = qstyleoption_cast<const QStyleOptionSlider *>(opt)) {
            int space = (sl->orientation == Qt::Horizontal) ? sl->rect.height() : sl->rect.width();
            int ticks = sl->tickPosition;
            int n = 0;
            if (ticks & QSlider::TicksAbove)
                ++n;
            if (ticks & QSlider::TicksBelow)
                ++n;
            if (!n) {
                ret = space;
                break;
            }
            int thick = 8;
            if (ticks != QSlider::TicksBothSides && ticks != QSlider::NoTicks)
                thick += pixelMetric(PM_SliderLength, sl, widget) / 4;

            space -= thick;
            if (space > 0)
                thick += (space * 2) / (n + 2);
            ret = thick;
        } else {
            ret = 0;
        }
        break;
#endif // QT_NO_SLIDER
#ifndef QT_NO_MENU
    case PM_SmallIconSize:
        d->doubleControls ? ret = windowsMobileIconSize * 2 : ret = windowsMobileIconSize;
        break;
    case PM_ButtonMargin:
        d->doubleControls ? ret = 8 : ret = 4;
        break;
    case PM_LargeIconSize:
        d->doubleControls ? ret = 64 : ret = 32;
        break;
    case PM_IconViewIconSize:
        ret = pixelMetric(PM_LargeIconSize, opt, widget);
        break;
    case PM_ToolBarIconSize:
        d->doubleControls ? ret = 2 * windowsMobileIconSize : ret = windowsMobileIconSize;
        break;
    case PM_DockWidgetTitleMargin:
        ret = 2;
        break;
#if defined(Q_WS_WIN)
#else
    case PM_DockWidgetFrameWidth:
        ret = 4;
        break;
#endif // Q_WS_WIN
    break;
#endif // QT_NO_MENU

   case PM_TitleBarHeight:
        d->doubleControls ? ret = 42 : ret = 21;
        break;
   case PM_ScrollBarSliderMin:
        d->doubleControls ? ret = 36 : ret = 18;
        break;        
   case PM_ScrollBarExtent: {
       //Check if the scrollbar is part of an abstractItemView and set size according
       if (d->smartphone)
           ret = 9;
       else
         d->doubleControls ? ret = 25 : ret = 13;
#ifndef QT_NO_SCROLLAREA
       if (widget)
       if (QWidget *parent = widget->parentWidget())
           if (qobject_cast<QAbstractScrollArea *>(parent->parentWidget()))
               if (d->smartphone)
                 ret = 8;
               else
                 d->doubleControls ? ret = 24 : ret = 12;
#endif
       }
       break;
    case PM_SplitterWidth:
        ret = qMax(4, QApplication::globalStrut().width());
        break;

#if defined(Q_WS_WIN)
    case PM_MDIFrameWidth:
        ret = 1;
        break;
#endif
    case PM_ToolBarExtensionExtent:
        d->doubleControls ? ret = 32 : ret = 16;
        break;
    case PM_ToolBarItemMargin:
        d->doubleControls ? ret = 2 : ret = 1;
        break;
    case PM_ToolBarItemSpacing:
        d->doubleControls ? ret = 2 : ret = 1;
        break;
    case PM_ToolBarHandleExtent:
        d->doubleControls ? ret = 16 : ret = 8;
        break;
    case PM_ButtonIconSize:
        d->doubleControls ? ret = 32 : ret = 16;
        break;
    case PM_TextCursorWidth:
        ret = 2;
        break;        
    case PM_TabBar_ScrollButtonOverlap:
        ret = 0;
        break;
    default:
        ret = QWindowsStyle::pixelMetric(pm, opt, widget);
        break;
    }
    return ret;
}

int QWindowsMobileStyle::styleHint(StyleHint hint, const QStyleOption *opt, const QWidget *widget,
                                   QStyleHintReturn *returnData) const {

    int ret;
    switch (hint) {
    case SH_Menu_MouseTracking:
    case SH_ComboBox_ListMouseTracking:
    case SH_EtchDisabledText:
        ret = 0;
        break;
    case SH_DitherDisabledText:
        ret = 0;
        break;
    case SH_ItemView_ShowDecorationSelected:
        ret = 0;
        break;
#ifndef QT_NO_TABWIDGET
    case SH_TabWidget_DefaultTabPosition:
        ret = QTabWidget::South;
        break;
#endif
    case SH_ToolBar_Movable:
        ret = false;
        break;    
    case SH_ScrollBar_ContextMenu:
        ret = false;
        break;
    default:
          ret = QWindowsStyle::styleHint(hint, opt, widget, returnData);
       break;
    }
    return ret;
}

QPixmap QWindowsMobileStyle::standardPixmap(StandardPixmap sp, const QStyleOption *option,
                                     const QWidget *widget) const {

    QWindowsMobileStylePrivate *d = const_cast<QWindowsMobileStylePrivate*>(d_func());
    switch (sp) {
#ifndef QT_NO_IMAGEFORMAT_XPM
    case SP_ToolBarHorizontalExtensionButton: {
        QPixmap pixmap = QCommonStyle::standardPixmap(sp, option, widget);
        if (d->doubleControls)
            return pixmap.scaledToHeight(pixmap.height() * 2);
        else
          return pixmap;
     }
     case SP_TitleBarMaxButton:
     case SP_TitleBarCloseButton:
     case SP_TitleBarNormalButton:
     case SP_TitleBarMinButton: {
         QImage image;
         switch (sp) {
            case SP_TitleBarMaxButton:
                image = d->imageMaximize;
            break;
            case SP_TitleBarCloseButton:
                image = d->imageClose;
            break;
            case SP_TitleBarNormalButton:
                image = d->imageNormalize;
            break;
            case SP_TitleBarMinButton:
                image = d->imageMinimize;
            break;
            default:
            break;
         }
         if (option) {
             image.setColor(0, option->palette.shadow().color().rgba());
             image.setColor(1, option->palette.highlight().color().rgba());
             image.setColor(2, option->palette.highlight().color().lighter(150).rgba());
             image.setColor(3, option->palette.highlightedText().color().rgba());
         }

         return QPixmap::fromImage(image);
     }

#endif
    default:
        return QWindowsStyle::standardPixmap(sp, option, widget);
    }
}

QPixmap QWindowsMobileStyle::generatedIconPixmap(QIcon::Mode iconMode, const QPixmap &pixmap,
                                                 const QStyleOption *option) const {

    switch (iconMode) {
    case QIcon::Selected: {
        QImage img = pixmap.toImage().convertToFormat(QImage::Format_ARGB32);
        int imgh = img.height();
        int imgw = img.width();
        for (int y = 0; y < imgh; y += 2) {
            for (int x = 0; x < imgw; x += 2) {
                QColor c = option->palette.highlight().color().rgb();
                c.setAlpha( qAlpha(img.pixel(x, y)));
                QRgb pixel = c.rgba();
                img.setPixel(x, y, pixel);
            }
        }
        return QPixmap::fromImage(img);
    }
    default:
    break;
    }
    return QWindowsStyle::generatedIconPixmap(iconMode, pixmap, option);
}


bool QWindowsMobileStyle::doubleControls() const {

    QWindowsMobileStylePrivate *d = const_cast<QWindowsMobileStylePrivate*>(d_func());

    return d->doubleControls;
}

void QWindowsMobileStyle::setDoubleControls(bool doubleControls) {

   QWindowsMobileStylePrivate *d = const_cast<QWindowsMobileStylePrivate*>(d_func());

   d->doubleControls = doubleControls;
}

QT_END_NAMESPACE

#endif // QT_NO_STYLE_WINDOWSMOBILE

