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

#include "q3combobox.h"
#ifndef QT_NO_COMBOBOX
#include "qpainter.h"
#include "qdrawutil.h"
#include "qpixmap.h"
#include "qtimer.h"
#include "qapplication.h"
#include "qlineedit.h"
#include "qbitmap.h"
#include "qstringlist.h"
#include "qstyle.h"
#include "qevent.h"
#include "qmenu.h"
#include "qmenudata.h"
#include "qstyleoption.h"
#include "qdesktopwidget.h"
#include "q3popupmenu.h"
#include "q3listbox.h"
#include "q3strlist.h"
#include "q3frame.h"
#include <limits.h>
#include <qdebug.h>
#ifndef QT_NO_EFFECTS
# include <private/qeffects_p.h>
#endif
#if defined(QT_ACCESSIBILITY_SUPPORT)
#include "qaccessible.h"
#endif

QT_BEGIN_NAMESPACE

/*!
    \class Q3ComboBox q3combobox.h
    \brief The Q3ComboBox widget is a combined button and popup list.
    \since 4.1
    \compat

    A combobox is a selection widget which displays the current item
    and can pop up a list of items. A combobox may be editable in
    which case the user can enter arbitrary strings.

    Comboboxes provide a means of showing the user's current choice
    out of a list of options in a way that takes up the minimum amount
    of screen space.

    Q3ComboBox supports three different display styles: Aqua/Motif 1.x,
    Motif 2.0 and Windows. In Motif 1.x, a combobox was called
    XmOptionMenu. In Motif 2.0, OSF introduced an improved combobox
    and named that XmComboBox. Q3ComboBox provides both.

    Q3ComboBox provides two different constructors. The simplest
    constructor creates an "old-style" combobox in Motif (or Aqua)
    style:
    \snippet doc/src/snippets/code/src_qt3support_widgets_q3combobox.cpp 0

    The other constructor creates a new-style combobox in Motif style,
    and can create both read-only and editable comboboxes:
    \snippet doc/src/snippets/code/src_qt3support_widgets_q3combobox.cpp 1

    New-style comboboxes use a list box in both Motif and Windows
    styles, and both the content size and the on-screen size of the
    list box can be limited with sizeLimit() and setMaxCount()
    respectively. Old-style comboboxes use a popup in Aqua and Motif
    style, and that popup will happily grow larger than the desktop if
    you put enough data into it.

    The two constructors create identical-looking comboboxes in
    Windows style.

    Comboboxes can contain pixmaps as well as strings; the
    insertItem() and changeItem() functions are suitably overloaded.
    For editable comboboxes, the function clearEdit() is provided,
    to clear the displayed string without changing the combobox's
    contents.

    A combobox emits two signals, activated() and highlighted(), when
    a new item has been activated (selected) or highlighted (made
    current). Both signals exist in two versions, one with a \c
    QString argument and one with an \c int argument. If the user
    highlights or activates a pixmap, only the \c int signals are
    emitted. Whenever the text of an editable combobox is changed the
    textChanged() signal is emitted.

    When the user enters a new string in an editable combobox, the
    widget may or may not insert it, and it can insert it in several
    locations. The default policy is is \c AtBottom but you can change
    this using setInsertionPolicy().

    It is possible to constrain the input to an editable combobox
    using QValidator; see setValidator(). By default, any input is
    accepted.

    If the combobox is not editable then it has a default
    focusPolicy() of \c TabFocus, i.e. it will not grab focus if
    clicked. This differs from both Windows and Motif. If the combobox
    is editable then it has a default focusPolicy() of \c StrongFocus,
    i.e. it will grab focus if clicked.

    A combobox can be populated using the insert functions,
    insertStringList() and insertItem() for example. Items can be
    changed with changeItem(). An item can be removed with
    removeItem() and all items can be removed with clear(). The text
    of the current item is returned by currentText(), and the text of
    a numbered item is returned with text(). The current item can be
    set with setCurrentItem() or setCurrentText(). The number of items
    in the combobox is returned by count(); the maximum number of
    items can be set with setMaxCount(). You can allow editing using
    setEditable(). For editable comboboxes you can set auto-completion
    using setAutoCompletion() and whether or not the user can add
    duplicates is set with setDuplicatesEnabled().

    Depending on the style, Q3ComboBox will use a list box or a
    popup menu to display the list of items. See setListBox() for
    more information.

    \sa QComboBox, QLineEdit, QSpinBox
        {GUI Design Handbook}{GUI Design Handbook: Combo Box, Drop-Down List Box}
*/


/*!
    \enum Q3ComboBox::Policy

    This enum specifies what the Q3ComboBox should do when a new string
    is entered by the user.

    \value NoInsertion the string will not be inserted into the
    combobox.

    \value AtTop insert the string as the first item in the combobox.

    \value AtCurrent replace the previously selected item with the
    string the user has entered.

    \value AtBottom insert the string as the last item in the
    combobox.

    \value AfterCurrent insert the string after the previously
    selected item.

    \value BeforeCurrent insert the string before the previously
    selected item.

    activated() is always emitted when the string is entered.

    If inserting the new string would cause the combobox to breach its
    content size limit, the item at the other end of the list is
    deleted. The definition of "other end" is
    implementation-dependent.

    \omitvalue NoInsert
    \omitvalue InsertAtTop
    \omitvalue InsertAtCurrent
    \omitvalue InsertAtBottom
    \omitvalue InsertAfterCurrent
    \omitvalue InsertBeforeCurrent
*/


/*!
    \fn void Q3ComboBox::activated( int index )

    This signal is emitted when a new item has been activated
    (selected). The \a index is the position of the item in the
    combobox.

    This signal is not emitted if the item is changed
    programmatically, e.g. using setCurrentItem().
*/

/*!
    \overload
    \fn void Q3ComboBox::activated( const QString &string )

    This signal is emitted when a new item has been activated
    (selected). \a string is the selected string.

    You can also use the activated(int) signal, but be aware that its
    argument is meaningful only for selected strings, not for user
    entered strings.
*/

/*!
    \fn void Q3ComboBox::highlighted( int index )

    This signal is emitted when a new item has been set to be the
    current item. The \a index is the position of the item in the
    combobox.

    This signal is not emitted if the item is changed
    programmatically, e.g. using setCurrentItem().
*/

/*!
    \overload
    \fn void Q3ComboBox::highlighted( const QString &string )

    This signal is emitted when a new item has been set to be the
    current item. \a string is the item's text.

    You can also use the highlighted(int) signal.
*/

/*!
    \fn void Q3ComboBox::textChanged( const QString &string )

    This signal is used for editable comboboxes. It is emitted
    whenever the contents of the text entry field changes. \a string
    contains the new text.
*/

/*!
    \property Q3ComboBox::autoCompletion
    \brief whether auto-completion is enabled

    This property can only be set for editable comboboxes, for
    non-editable comboboxes it has no effect. It is false by default.
*/

/*!
    \property Q3ComboBox::autoResize
    \brief whether auto-resize is enabled
    \obsolete

    If this property is set to true then the combobox will resize
    itself whenever its contents change. The default is false.
*/

/*!
    \property Q3ComboBox::count
    \brief the number of items in the combobox
*/

/*!
    \property Q3ComboBox::currentItem
    \brief the index of the current item in the combobox

    Note that the activated() and highlighted() signals are only
    emitted when the user changes the current item, not when it is
    changed programmatically.
*/

/*!
    \property Q3ComboBox::currentText
    \brief the text of the combobox's current item
*/

/*!
    \property Q3ComboBox::duplicatesEnabled
    \brief whether duplicates are allowed

    If the combobox is editable and the user enters some text in the
    combobox's lineedit and presses Enter (and the insertionPolicy()
    is not \c NoInsertion), then what happens is this:
    \list
    \i If the text is not already in the list, the text is inserted.
    \i If the text is in the list and this property is true (the
    default), the text is inserted.
    \i If the text is in the list and this property is false, the text
    is \e not inserted; instead the item which has matching text becomes
    the current item.
    \endlist

    This property only affects user-interaction. You can use
    insertItem() to insert duplicates if you wish regardless of this
    setting.
*/

/*!
    \property Q3ComboBox::editable
    \brief whether the combobox is editable

    This property's default is false. Note that the combobox will be
    cleared if this property is set to true for a 1.x Motif style
    combobox. To avoid this, use setEditable() before inserting any
    items. Also note that the 1.x version of Motif didn't have any
    editable comboboxes, so the combobox will change its appearance
    to a 2.0 style Motif combobox is it is set to be editable.
*/

/*!
    \property Q3ComboBox::insertionPolicy
    \brief the position of the items inserted by the user

    The default insertion policy is \c AtBottom. See \l Policy.
*/

/*!
    \property Q3ComboBox::maxCount
    \brief the maximum number of items allowed in the combobox
*/

/*!
    \property Q3ComboBox::sizeLimit
    \brief the maximum on-screen size of the combobox.

    This property is ignored for both Motif 1.x style and non-editable
    comboboxes in Mac style. The default limit is ten
    lines. If the number of items in the combobox is or grows larger
    than lines, a scroll bar is added.
*/

class Q3ComboBoxPopup : public Q3PopupMenu
{
public:
    Q3ComboBoxPopup( QWidget *parent=0, const char *name=0 )
	: Q3PopupMenu( parent, name )
    {
    }

    int itemHeight( int index )
    {
	return Q3PopupMenu::itemHeight( index );
    }
};

static inline QString escapedComboString(const QString &str)
{
    QString stringToReturn = str;
    return stringToReturn.replace(QLatin1Char('&'), QLatin1String("&&"));
}

class Q3ComboBoxPopupItem : public QMenuItem
{
    Q3ListBoxItem *li;
    QSize sc; // Size cache optimization
public:
    Q3ComboBoxPopupItem(Q3ListBoxItem *i) : QMenuItem(), li(i), sc(0, 0) {  }
    virtual bool fullSpan() const { return true; }
    virtual void paint( QPainter*, const QColorGroup&, bool, bool, int, int, int, int);
    virtual QSize sizeHint() { if (sc.isNull()) sc = QSize(li->width(li->listBox()), QMAX(25, li->height(li->listBox()))); return sc; }
};
void Q3ComboBoxPopupItem::paint( QPainter* p, const QColorGroup&, bool,
				bool, int x, int y, int, int)
{
    p->save();
    p->translate(x, y + ((sizeHint().height() / 2) - (li->height(li->listBox()) / 2)));
    li->paint(p);
    p->restore();
}


class Q3ComboBoxData
{
public:
    Q3ComboBoxData( Q3ComboBox *cb ): current( 0 ), arrowDown(false), ed( 0 ), usingLBox( false ), pop( 0 ), lBox( 0 ), combo( cb )
    {
	duplicatesEnabled = true;
	cb->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed ) );
    }

    inline bool usingListBox()  { return usingLBox; }
    inline Q3ListBox * listBox() { return lBox; }
    inline Q3ComboBoxPopup * popup() { return pop; }
    void updateLinedGeometry();

    void setListBox( Q3ListBox *l ) { lBox = l ; usingLBox = true;
				l->setMouseTracking( true );}

    void setPopupMenu( Q3ComboBoxPopup * pm, bool isPopup=true )
	{ pop = pm; if(isPopup) usingLBox = false; }

    QStyleOptionComboBox getStyleOption() const
        {
            QStyleOptionComboBox opt;
            opt.init(combo);
            if (!combo->editable() && combo->hasFocus())
                opt.state |= QStyle::State_Selected;
            opt.subControls = QStyle::SC_All;
            if (arrowDown) {
                opt.activeSubControls = QStyle::SC_ComboBoxArrow;
                opt.state |= QStyle::State_Sunken;
            }
            opt.editable = combo->editable();
            opt.frame = 1; // ### get from style?
            if (current > -1 && current < combo->count()) {
                opt.currentText = combo->text(current);
                if (combo->pixmap(current))
                    opt.currentIcon = QIcon(*combo->pixmap(current));
            }
            opt.iconSize = QSize(22, 22); // ### need a sane value here
//             if (container && container->isVisible())
//                 opt.state |= QStyle::State_On;
            return opt;
        }

    int		current;
    int		maxCount;
    int		sizeLimit;
    Q3ComboBox::Policy p;
    bool	autoresize;
    bool	poppedUp;
    bool	mouseWasInsidePopup;
    bool	arrowPressed;
    bool	arrowDown;
    bool	discardNextMousePress;
    bool	shortClick;
    bool	useCompletion;
    bool	completeNow;
    int		completeAt;
    bool duplicatesEnabled;
    int fullHeight, currHeight;

    QLineEdit * ed;  // /bin/ed rules!
    QTimer *completionTimer;

    QSize sizeHint;
    QHash<int, QPixmap> popupPixmaps;

private:
    bool	usingLBox;
    Q3ComboBoxPopup *pop;
    Q3ListBox   *lBox;
    Q3ComboBox *combo;
};

void Q3ComboBoxData::updateLinedGeometry()
{
    if ( !ed || !combo )
	return;

    QStyleOptionComboBox opt = getStyleOption();
    QRect r = combo->style()->subControlRect(
        QStyle::CC_ComboBox, &opt, QStyle::SC_ComboBoxEditField, combo);

    const QPixmap *pix = current < combo->count() ? combo->pixmap( current ) : 0;
    if ( pix && pix->width() < r.width() )
	r.setLeft( r.left() + pix->width() + 4 );
    if ( r != ed->geometry() )
	ed->setGeometry( r );
}

static inline bool checkInsertIndex( const char *method, const char * name,
				     int count, int *index)
{
    bool range_err = (*index > count);
#if defined(QT_CHECK_RANGE)
    if ( range_err )
	qWarning( "Q3ComboBox::%s: (%s) Index %d out of range",
		 method, name ? name : "<no name>", *index );
#else
    Q_UNUSED( method )
    Q_UNUSED( name )
#endif
    if ( *index < 0 )				// append
	*index = count;
    return !range_err;
}


static inline bool checkIndex( const char *method, const char * name,
			       int count, int index )
{
    bool range_err = (index >= count);
#if defined(QT_CHECK_RANGE)
    if ( range_err )
	qWarning( "Q3ComboBox::%s: (%s) Index %i out of range",
		 method, name ? name : "<no name>", index );
#else
    Q_UNUSED( method )
    Q_UNUSED( name )
#endif
    return !range_err;
}



/*!
    Constructs a combobox widget with parent \a parent called \a name.

    This constructor creates a popup list if the program uses Motif
    (or Aqua) look and feel; this is compatible with Motif 1.x and
    Aqua.

    Note: If you use this constructor to create your Q3ComboBox, then
    the pixmap() function will always return 0. To workaround this,
    use the other constructor.

*/



Q3ComboBox::Q3ComboBox( QWidget *parent, const char *name )
    : QWidget( parent, name, Qt::WNoAutoErase )
{
    d = new Q3ComboBoxData( this );
    QStyleOptionComboBox opt;
    opt.init(this);
    if ( style()->styleHint(QStyle::SH_ComboBox_Popup, &opt, this) ||
	 style()->styleHint(QStyle::SH_GUIStyle, &opt, this) == Qt::MotifStyle ) {
	d->setPopupMenu( new Q3ComboBoxPopup( this, "in-combo" ) );
	d->popup()->setFont( font() );
	connect( d->popup(), SIGNAL(activated(int)),
			     SLOT(internalActivate(int)) );
	connect( d->popup(), SIGNAL(highlighted(int)),
			     SLOT(internalHighlight(int)) );
    } else {
	setUpListBox();
    }
    d->ed                    = 0;
    d->current               = 0;
    d->maxCount              = INT_MAX;
    d->sizeLimit	     = 10;
    d->p                     = AtBottom;
    d->autoresize            = false;
    d->poppedUp              = false;
    d->arrowDown             = false;
    d->arrowPressed          = false;
    d->discardNextMousePress = false;
    d->shortClick            = false;
    d->useCompletion         = false;
    d->completeAt            = 0;
    d->completeNow           = false;
    d->completionTimer       = new QTimer( this );

    setFocusPolicy( Qt::TabFocus );
    setBackgroundMode( Qt::PaletteButton );
}


/*!
    Constructs a combobox with a maximum size and either Motif 2.0 or
    Windows look and feel.

    The input field can be edited if \a rw is true, otherwise the user
    may only choose one of the items in the combobox.

    The \a parent and \a name arguments are passed on to the QWidget
    constructor.
*/


Q3ComboBox::Q3ComboBox( bool rw, QWidget *parent, const char *name )
    : QWidget( parent, name, Qt::WNoAutoErase )
{
    d = new Q3ComboBoxData( this );
    setUpListBox();

    QStyleOptionComboBox opt = d->getStyleOption();
    if(d->popup() && style()->styleHint(QStyle::SH_ComboBox_Popup, &opt, this))
	d->popup()->setItemChecked(d->current, false);
    d->maxCount = INT_MAX;
    setSizeLimit(10);
    d->p = AtBottom;
    d->autoresize = false;
    d->poppedUp = false;
    d->arrowDown = false;
    d->discardNextMousePress = false;
    d->shortClick = false;
    d->useCompletion = false;
    d->completeAt = 0;
    d->completeNow = false;
    d->completionTimer = new QTimer( this );

    setFocusPolicy( Qt::StrongFocus );

    d->ed = 0;
    if ( rw )
	setUpLineEdit();
    setBackgroundMode( Qt::PaletteButton, Qt::PaletteBase );
}



/*!
    Destroys the combobox.
*/

Q3ComboBox::~Q3ComboBox()
{
    delete d;
}

void Q3ComboBox::setDuplicatesEnabled( bool enable )
{
   d->duplicatesEnabled = enable;
}

bool Q3ComboBox::duplicatesEnabled() const
{
    return d->duplicatesEnabled;
}

int Q3ComboBox::count() const
{
    if ( d->usingListBox() )
	return d->listBox()->count();
    else if (d->popup())
	return d->popup()->count();
    else
        return 0;
}


/*!
    \overload

    Inserts the \a list of strings at position \a index in the
    combobox.

    This is only for compatibility since it does not support Unicode
    strings. See insertStringList().
*/

void Q3ComboBox::insertStrList( const Q3StrList &list, int index )
{
    insertStrList( &list, index );
}

/*!
    \overload

    Inserts the \a list of strings at position \a index in the
    combobox.

    This is only for compatibility since it does not support Unicode
    strings. See insertStringList().
*/

void Q3ComboBox::insertStrList( const Q3StrList *list, int index )
{
    if ( !list ) {
#if defined(QT_CHECK_NULL)
	Q_ASSERT( list != 0 );
#endif
	return;
    }
    Q3StrListIterator it( *list );
    const char* tmp;
    if ( index < 0 )
	index = count();
    while ( (tmp=it.current()) ) {
	++it;
	if ( d->usingListBox() )
	    d->listBox()->insertItem( QString::fromLatin1(tmp), index );
	else
	    d->popup()->insertItem( escapedComboString(QString::fromLatin1(tmp)), index, index );
	if ( index++ == d->current && d->current < count() ) {
	    if ( d->ed ) {
		d->ed->setText( text( d->current ) );
		d->updateLinedGeometry();
	    } else
		update();
	    currentChanged();
	}
    }
    if ( index != count() )
	reIndex();
}

/*!
    Inserts the \a list of strings at position \a index in the
    combobox.
*/

void Q3ComboBox::insertStringList( const QStringList &list, int index )
{
    QStringList::ConstIterator it = list.begin();
    if ( index < 0 )
	index = count();
    while ( it != list.end() ) {
	if ( d->usingListBox() )
	    d->listBox()->insertItem( *it, index );
	else
	    d->popup()->insertItem( escapedComboString(*it), index, index );
	if ( index++ == d->current && d->current < count() ) {
	    if ( d->ed ) {
		d->ed->setText( text( d->current ) );
		d->updateLinedGeometry();
	    } else
		update();
	    currentChanged();
	}
	++it;
    }
    if ( index != count() )
	reIndex();
}

/*!
    Inserts the array of char * \a strings at position \a index in the
    combobox.

    The \a numStrings argument is the number of strings. If \a
    numStrings is -1 (default), the \a strings array must be
    terminated with 0.

    Example:
    \snippet doc/src/snippets/code/src_qt3support_widgets_q3combobox.cpp 2

    \sa insertStringList()
*/

void Q3ComboBox::insertStrList( const char **strings, int numStrings, int index)
{
    if ( !strings ) {
#if defined(QT_CHECK_NULL)
	Q_ASSERT( strings != 0 );
#endif
	return;
    }
    if ( index < 0 )
	index = count();
    int i = 0;
    while ( (numStrings<0 && strings[i]!=0) || i<numStrings ) {
	if ( d->usingListBox() )
	    d->listBox()->insertItem( QString::fromLatin1(strings[i]), index );
	else
	    d->popup()->insertItem( escapedComboString(QString::fromLatin1(strings[i])), index, index );
	i++;
	if ( index++ == d->current && d->current < count()  ) {
	    if ( d->ed ) {
		d->ed->setText( text( d->current ) );
		d->updateLinedGeometry();
	    } else
		update();
	    currentChanged();
	}
    }
    if ( index != count() )
	reIndex();
}


/*!
    Inserts a text item with text \a t, at position \a index. The item
    will be appended if \a index is negative.
*/

void Q3ComboBox::insertItem( const QString &t, int index )
{
    int cnt = count();
    if ( !checkInsertIndex( "insertItem", name(), cnt, &index ) )
	return;
    if ( d->usingListBox() )
	d->listBox()->insertItem( t, index );
    else
	d->popup()->insertItem( escapedComboString(t), index, index );
    if ( index != cnt )
	reIndex();
    if ( index == d->current && d->current < count()  ) {
	if ( d->ed ) {
	    d->ed->setText( text( d->current ) );
	    d->updateLinedGeometry();
	} else
	    update();
    }
    if ( index == d->current )
	currentChanged();
}

/*!
    \overload

    Inserts a \a pixmap item at position \a index. The item will be
    appended if \a index is negative.
*/

void Q3ComboBox::insertItem( const QPixmap &pixmap, int index )
{
    int cnt = count();
    if ( !checkInsertIndex( "insertItem", name(), cnt, &index ) )
	return;
    if ( d->usingListBox() )
	d->listBox()->insertItem( pixmap, index );
    else
	d->popup()->insertItem( pixmap, index, index );
    if ( index != cnt )
	reIndex();
    if ( index == d->current && d->current < count()  ) {
	if ( d->ed ) {
	    d->ed->setText( text( d->current ) );
	    d->updateLinedGeometry();
	} else
	    update();
    }
    if ( index == d->current )
	currentChanged();
}

/*!
    \overload

    Inserts a \a pixmap item with additional text \a text at position
    \a index. The item will be appended if \a index is negative.
*/

void Q3ComboBox::insertItem( const QPixmap &pixmap, const QString& text, int index )
{
    int cnt = count();
    if ( !checkInsertIndex( "insertItem", name(), cnt, &index ) )
	return;
    if ( d->usingListBox() )
	d->listBox()->insertItem( pixmap, text, index );
    else
	d->popup()->insertItem( pixmap, escapedComboString(text), index, index );
    if ( index != cnt )
	reIndex();
    if ( index == d->current && d->current < count()  ) {
	if ( d->ed ) {
	    d->ed->setText( this->text( d->current ) );
	    d->updateLinedGeometry();
	} else
	    update();
    }
    if ( index == d->current )
	currentChanged();
}


/*!
    Removes the item at position \a index.
*/

void Q3ComboBox::removeItem( int index )
{
    int cnt = count();
    if ( !checkIndex( "removeItem", name(), cnt, index ) )
	return;
    if ( d->usingListBox() ) {
        QStyleOptionComboBox opt = d->getStyleOption();
	if ( style()->styleHint(QStyle::SH_ComboBox_Popup, &opt, this) && d->popup() )
	    d->popup()->removeItemAt( index );
	d->listBox()->removeItem( index );
    } else {
	d->popup()->removeItemAt( index );
    }
    if ( index != cnt-1 )
	reIndex();
    if ( index == d->current ) {
	if ( d->ed ) {
	    QString s = QString::fromLatin1("");
	    if (d->current < cnt - 1)
		s = text( d->current );
	    d->ed->setText( s );
	    d->updateLinedGeometry();
	}
	else {
	    if ( d->usingListBox() ) {
		d->current = d->listBox()->currentItem();
	    } else {
		if (d->current > count()-1 && d->current > 0)
		    d->current--;
	    }
	    update();
	}
	currentChanged();
    }
    else {
	if ( !d->ed ) {
	    if (d->current < cnt - 1)
		setCurrentItem( d->current );
	    else
		setCurrentItem( d->current - 1 );
	}
    }

}


/*!
    Removes all combobox items.
*/

void Q3ComboBox::clear()
{
    QStyleOptionComboBox opt = d->getStyleOption();
    if ( d->usingListBox() ) {
	if ( style()->styleHint(QStyle::SH_ComboBox_Popup, &opt, this) && d->popup() )
	    d->popup()->clear();
	d->listBox()->resize( 0, 0 );
	d->listBox()->clear();
    } else {
	d->popup()->clear();
    }

    if(d->popup() && style()->styleHint(QStyle::SH_ComboBox_Popup, &opt, this))
	d->popup()->setItemChecked(d->current, false);
    d->current = 0;
    if ( d->ed ) {
	d->ed->setText( QString::fromLatin1("") );
	d->updateLinedGeometry();
    }
    currentChanged();
}


QString Q3ComboBox::currentText() const
{
    if ( d->ed )
	return d->ed->text();
    else if ( d->current < count() )
	return text( currentItem() );
    else
	return QString::null;
}

void Q3ComboBox::setCurrentText( const QString& txt )
{
    int i;
    for ( i = 0; i < count(); i++)
	if ( text( i ) == txt )
	    break;
    if ( i < count() )
	setCurrentItem( i );
    else if ( d->ed )
	d->ed->setText( txt );
    else
	changeItem( txt, currentItem() );
}


/*!
    Returns the text item at position \a index, or QString::null if
    the item is not a string.

    \sa currentText()
*/

QString Q3ComboBox::text( int index ) const
{
    if ( !checkIndex( "text", name(), count(), index ) )
	return QString::null;
    if ( d->usingListBox() ) {
	return d->listBox()->text( index );
    } else {
        QString retText = d->popup()->text(index);
        retText.replace(QLatin1String("&&"), QString(QLatin1Char('&')));
	return retText;
    }
}

/*!
    Returns the pixmap item at position \a index, or 0 if the item is
    not a pixmap.
*/

const QPixmap *Q3ComboBox::pixmap( int index ) const
{
    if ( !checkIndex( "pixmap", name(), count(), index ) )
	return 0;

    if (d->usingListBox()) {
        return d->listBox()->pixmap( index );
    } else {
        d->popupPixmaps[index] = d->popup()->pixmap(index);
        return d->popupPixmaps[index].isNull() ? 0 : &d->popupPixmaps[index];
    }
}

/*!
    Replaces the item at position \a index with the text \a t.
*/

void Q3ComboBox::changeItem( const QString &t, int index )
{
    if ( !checkIndex( "changeItem", name(), count(), index ) )
	return;
    if ( d->usingListBox() )
	d->listBox()->changeItem( t, index );
    else
	d->popup()->changeItem(index, t);
    if ( index == d->current ) {
	if ( d->ed ) {
	    d->ed->setText( text( d->current ) );
	    d->updateLinedGeometry();
	} else
	    update();
    }
}

/*!
    \overload

    Replaces the item at position \a index with the pixmap \a im,
    unless the combobox is editable.

    \sa insertItem()
*/

void Q3ComboBox::changeItem( const QPixmap &im, int index )
{
    if ( !checkIndex( "changeItem", name(), count(), index ) )
	return;
    if ( d->usingListBox() )
	d->listBox()->changeItem( im, index );
    else
	d->popup()->changeItem(index, im);
    if ( index == d->current )
	update();
}

/*!
    \overload

    Replaces the item at position \a index with the pixmap \a im and
    the text \a t.

    \sa insertItem()
*/

void Q3ComboBox::changeItem( const QPixmap &im, const QString &t, int index )
{
    if ( !checkIndex( "changeItem", name(), count(), index ) )
	return;
    if ( d->usingListBox() )
	d->listBox()->changeItem( im, t, index );
    else
	d->popup()->changeItem(index, im, t);
    if ( index == d->current )
	update();
}


int Q3ComboBox::currentItem() const
{
    return d->current;
}

void Q3ComboBox::setCurrentItem( int index )
{
    if ( index == d->current && !d->ed ) {
	return;
    }
    if ( !checkIndex( "setCurrentItem", name(), count(), index ) ) {
	return;
    }

    if ( d->usingListBox() && !( listBox()->item(index) && listBox()->item(index)->isSelectable() ) )
	return;

    QStyleOptionComboBox opt = d->getStyleOption();
    if(d->popup() && style()->styleHint(QStyle::SH_ComboBox_Popup, &opt, this))
	d->popup()->setItemChecked(d->current, false);
    d->current = index;
    d->completeAt = 0;
    if ( d->ed ) {
	d->ed->setText( text( index ) );
	d->updateLinedGeometry();
    }
    // ### We want to keep ListBox's currentItem in sync, even if NOT popuped...
    if ( d->usingListBox() && d->listBox() ) {
	d->listBox()->setCurrentItem( index );
    } else {
	internalHighlight( index );
	// internalActivate( index ); ### this leads to weird behavior, as in 3.0.1
    }

    currentChanged();
}

/*!
    Returns true if auto-resize is enabled; otherwise returns false.

    \sa autoResize
*/

bool Q3ComboBox::autoResize() const
{
    return d->autoresize;
}

/*!
    If \a enable is true, enable auto-resize; disable it otherwise.

    \sa autoResize
*/

void Q3ComboBox::setAutoResize( bool enable )
{
    if ( (bool)d->autoresize != enable ) {
	d->autoresize = enable;
	if ( enable )
	    adjustSize();
    }
}


/*!
    \reimp

    This implementation caches the size hint to avoid resizing when
    the contents change dynamically. To invalidate the cached value
    call setFont().
*/
QSize Q3ComboBox::sizeHint() const
{
    if ( isVisible() && d->sizeHint.isValid() )
	return d->sizeHint;

    constPolish();
    int i, w;
    QFontMetrics fm = fontMetrics();

    int maxW = count() ? 18 : 7 * fm.width(QLatin1Char('x')) + 18;
    int maxH = QMAX( fm.lineSpacing(), 14 ) + 2;

    if ( !d->usingListBox() ) {
	w = d->popup()->sizeHint().width() - 2* d->popup()->frameWidth();
	if ( w > maxW )
	    maxW = w;
    } else {
	for( i = 0; i < count(); i++ ) {
	    w = d->listBox()->item( i )->width( d->listBox() );
	    if ( w > maxW )
		maxW = w;
	}
    }

    QStyleOptionComboBox opt = d->getStyleOption();
    d->sizeHint = (style()->sizeFromContents(QStyle::CT_ComboBox, &opt, QSize(maxW, maxH), this).
                   expandedTo(QApplication::globalStrut()));

    return d->sizeHint;
}


/*!
  \internal
  Receives activated signals from an internal popup list and emits
  the activated() signal.
*/

void Q3ComboBox::internalActivate( int index )
{
    QStyleOptionComboBox opt = d->getStyleOption();
    if ( d->current != index ) {
	if ( !d->usingListBox() || listBox()->item( index )->isSelectable() ) {
	    if (d->popup() && style()->styleHint(QStyle::SH_ComboBox_Popup, &opt, this))
		d->popup()->setItemChecked(d->current, false);
	    d->current = index;
	    currentChanged();
	}
    }
    if ( d->usingListBox() )
	popDownListBox();
    else
	d->popup()->removeEventFilter( this );
    d->poppedUp = false;

    QString t( text( index ) );
    if ( d->ed ) {
	d->ed->setText( t );
	d->updateLinedGeometry();
    }
    emit activated( index );
    emit activated( t );
}

/*!
  \internal
  Receives highlighted signals from an internal popup list and emits
  the highlighted() signal.
*/

void Q3ComboBox::internalHighlight( int index )
{
    emit highlighted( index );
    QString t = text( index );
    if ( !t.isNull() )
	emit highlighted( t );
}

/*!
  \internal
  Receives timeouts after a click. Used to decide if a Motif style
  popup should stay up or not after a click.
*/
void Q3ComboBox::internalClickTimeout()
{
    d->shortClick = false;
}

/*!
    Sets the palette for both the combobox button and the combobox
    popup list to \a palette.
*/

void Q3ComboBox::setPalette( const QPalette &palette )
{
    QWidget::setPalette( palette );
    if ( d->listBox() )
	d->listBox()->setPalette( palette );
    if ( d->popup() )
	d->popup()->setPalette( palette );
}

/*!
    Sets the font for both the combobox button and the combobox popup
    list to \a font.
*/

void Q3ComboBox::setFont( const QFont &font )
{
    d->sizeHint = QSize();		// invalidate size hint
    QWidget::setFont( font );
    if ( d->usingListBox() )
	d->listBox()->setFont( font );
    else
	d->popup()->setFont( font );
    if (d->ed)
	d->ed->setFont( font );
    if ( d->autoresize )
	adjustSize();
}


/*!\reimp
*/

void Q3ComboBox::resizeEvent( QResizeEvent * e )
{
    if ( d->ed )
	d->updateLinedGeometry();
    if ( d->listBox() )
	d->listBox()->resize( width(), d->listBox()->height() );
    QWidget::resizeEvent( e );
}

/*!\reimp
*/

void Q3ComboBox::paintEvent( QPaintEvent * )
{
    QPainter p( this );
    const QColorGroup & g = colorGroup();
    p.setPen(g.text());

    if ( width() < 5 || height() < 5 ) {
	qDrawShadePanel( &p, rect(), g, false, 2,
			 &g.brush( QColorGroup::Button ) );
	return;
    }

    QStyleOptionComboBox opt = d->getStyleOption();
    bool reverse = QApplication::reverseLayout();
    if ( !d->usingListBox() &&
	 style()->styleHint(QStyle::SH_GUIStyle) == Qt::MotifStyle) {			// motif 1.x style
	int dist, buttonH, buttonW;
	dist     = 8;
	buttonH  = 7;
	buttonW  = 11;
	int xPos;
	int x0;
	int w = width() - dist - buttonW - 1;
	if ( reverse ) {
	    xPos = dist + 1;
	    x0 = xPos + 4;
	} else {
	    xPos = w;
	    x0 = 4;
	}
	qDrawShadePanel( &p, rect(), g, false,
			 style()->pixelMetric(QStyle::PM_DefaultFrameWidth, &opt, this),
			 &g.brush( QColorGroup::Button ) );
	qDrawShadePanel( &p, xPos, (height() - buttonH)/2,
			 buttonW, buttonH, g, false,
			 style()->pixelMetric(QStyle::PM_DefaultFrameWidth, &opt, this) );
	QRect clip( x0, 2, w - 2 - 4 - 5, height() - 4 );
	QString str = d->popup()->text( this->d->current );
	if ( !str.isNull() ) {
	    p.drawText( clip, Qt::AlignCenter | Qt::TextSingleLine, str );
	}

	QPixmap pix = d->popup()->pixmap( this->d->current );
	QIcon iconSet = d->popup()->iconSet( this->d->current );
	if (!pix.isNull() || !iconSet.isNull()) {
	    QPixmap pm = ( !pix.isNull() ? pix : iconSet.pixmap() );
	    p.setClipRect( clip );
	    p.drawPixmap( 4, (height()-pm.height())/2, pm );
	    p.setClipping( false );
	}

	if ( hasFocus() )
	    p.drawRect( xPos - 5, 4, width() - xPos + 1 , height() - 8 );
    } else if(!d->usingListBox()) {
	style()->drawComplexControl(QStyle::CC_ComboBox, &opt, &p, this);
        QRect re = style()->subControlRect(QStyle::CC_ComboBox, &opt,
                                           QStyle::SC_ComboBoxEditField, this);
	p.setClipRect( re );

	QString str = d->popup()->text( this->d->current );
	QPixmap pix = d->popup()->pixmap( this->d->current );
	if ( !str.isNull() ) {
	    p.save();
	    p.setFont(font());
	    QFontMetrics fm(font());
	    int x = re.x(), y = re.y() + fm.ascent();
            x += pix.width() + 5;
	    p.drawText( x, y, str );
	    p.restore();
	}
	if (!pix.isNull()) {
	    p.fillRect(re.x(), re.y(), pix.width() + 4, re.height(),
                        colorGroup().brush(QColorGroup::Base));
	    p.drawPixmap(re.x() + 2, re.y() + (re.height() - pix.height()) / 2, pix);
	}
    } else {
	style()->drawComplexControl(QStyle::CC_ComboBox, &opt, &p, this);
	QRect re = style()->subControlRect(QStyle::CC_ComboBox, &opt,
                                            QStyle::SC_ComboBoxEditField, this);
	p.setClipRect(re);

	if ( !d->ed ) {
	    Q3ListBoxItem * item = d->listBox()->item( d->current );
	    if ( item ) {
		int itemh = item->height( d->listBox() );
		p.translate( re.x(), re.y() + (re.height() - itemh)/2  );
		item->paint( &p );
	    }
	} else if ( d->listBox() && d->listBox()->item( d->current ) ) {
	    Q3ListBoxItem * item = d->listBox()->item( d->current );
	    const QPixmap *pix = item->pixmap();
	    if ( pix ) {
		p.fillRect( re.x(), re.y(), pix->width() + 4, re.height(),
			    colorGroup().brush( QColorGroup::Base ) );
		p.drawPixmap( re.x() + 2, re.y() +
			      ( re.height() - pix->height() ) / 2, *pix );
	    }
	}
	p.setClipping( false );
    }
}


/*!\reimp
*/

void Q3ComboBox::mousePressEvent( QMouseEvent *e )
{
    if ( e->button() != Qt::LeftButton )
	return;
    if ( d->discardNextMousePress ) {
	d->discardNextMousePress = false;
	return;
    }

    QStyleOptionComboBox opt = d->getStyleOption();
    QRect arrowRect = style()->subControlRect(QStyle::CC_ComboBox, &opt, QStyle::SC_ComboBoxArrow
                                               , this);

    // Correction for motif style, where arrow is smaller
    // and thus has a rect that doesn't fit the button.
    arrowRect.setHeight( QMAX(  height() - (2 * arrowRect.y()), arrowRect.height() ) );

    if ( count() && ( !editable() || arrowRect.contains( e->pos() ) ) ) {
	d->arrowPressed = false;

	if ( d->usingListBox() ) {
	    listBox()->blockSignals( true );
	    qApp->sendEvent( listBox(), e ); // trigger the listbox's autoscroll
	    listBox()->setCurrentItem(d->current);
	    listBox()->blockSignals( false );
	    popup();
	    if ( arrowRect.contains( e->pos() ) ) {
		d->arrowPressed = true;
		d->arrowDown    = true;
		repaint( false );
	    }
	} else {
	    popup();
	}
	QTimer::singleShot( 200, this, SLOT(internalClickTimeout()));
	d->shortClick = true;
    }
}

/*!\reimp
*/

void Q3ComboBox::mouseMoveEvent( QMouseEvent * )
{
}

/*!\reimp
*/

void Q3ComboBox::mouseReleaseEvent( QMouseEvent * )
{
}

/*!\reimp
*/

void Q3ComboBox::mouseDoubleClickEvent( QMouseEvent *e )
{
    mousePressEvent( e );
}


/*!\reimp
*/

void Q3ComboBox::keyPressEvent( QKeyEvent *e )
{
    bool handleEventHere = d->usingListBox() || !d->poppedUp;

    int c = currentItem();
    if ( ( e->key() == Qt::Key_F4 && e->state() == 0 ) ||
	 ( e->key() == Qt::Key_Down && (e->state() & Qt::AltModifier) ) ||
	 ( !d->ed && e->key() == Qt::Key_Space ) ) {
	if ( count() ) {
	    if ( !d->usingListBox() )
		d->popup()->setActiveItem( this->d->current );
	    popup();
	}
	return;
    } else if ( handleEventHere && e->key() == Qt::Key_Up ) {
	if ( c > 0 )
	    setCurrentItem( c-1 );
    } else if ( handleEventHere && e->key() == Qt::Key_Down ) {
	if ( ++c < count() )
	    setCurrentItem( c );
    } else if ( handleEventHere && e->key() == Qt::Key_Home && ( !d->ed || !d->ed->hasFocus() ) ) {
	setCurrentItem( 0 );
    } else if ( handleEventHere && e->key() == Qt::Key_End && ( !d->ed || !d->ed->hasFocus() ) ) {
	setCurrentItem( count()-1 );
    } else if ( !d->ed && e->ascii() >= 32 && !e->text().isEmpty() ) {
	if ( !d->completionTimer->isActive() ) {
	    d->completeAt = 0;
	    c = completionIndex( e->text(), ++c );
	    if ( c >= 0 ) {
		setCurrentItem( c );
		d->completeAt = e->text().length();
	    }
	} else {
	    d->completionTimer->stop();
	    QString ct = currentText().left( d->completeAt ) + e->text();
	    c = completionIndex( ct, c );
	    if ( c < 0 && d->completeAt > 0 ) {
		c = completionIndex( e->text(), 0 );
		ct = e->text();
	    }
	    d->completeAt = 0;
	    if ( c >= 0 ) {
		setCurrentItem( c );
		d->completeAt = ct.length();
	    }
	}
	d->completionTimer->start( 400, true );
    } else {
	e->ignore();
	return;
    }

    c = currentItem();
    if ( count() && !text( c ).isNull() )
	emit activated( text( c ) );
    emit activated( c );
}


/*!\reimp
*/

void Q3ComboBox::focusInEvent( QFocusEvent * e )
{
    QWidget::focusInEvent( e );
    d->completeNow = false;
    d->completeAt = 0;
}

/*!\reimp
*/

void Q3ComboBox::focusOutEvent( QFocusEvent * e )
{
    QWidget::focusOutEvent( e );
    d->completeNow = false;
    d->completeAt = 0;
}

/*!\reimp
*/
#ifndef QT_NO_WHEELEVENT
void Q3ComboBox::wheelEvent( QWheelEvent *e )
{
    if ( d->poppedUp ) {
	if ( d->usingListBox() ) {
	    QApplication::sendEvent( d->listBox(), e );
	}
    } else {
	if ( e->delta() > 0 ) {
	    int c = currentItem();
	    if ( c > 0 ) {
		setCurrentItem( c-1 );
		emit activated( currentItem() );
		emit activated( currentText() );
	    }
	} else {
	    int c = currentItem();
	    if ( ++c < count() ) {
		setCurrentItem( c );
		emit activated( currentItem() );
		emit activated( currentText() );
	    }
	}
	e->accept();
    }
}
#endif

/*!
  \internal
   Calculates the listbox height needed to contain all items, or as
   many as the list box is supposed to contain.
*/
static int listHeight( Q3ListBox *l, int sl )
{
    if ( l->count() > 0 )
	return QMIN( l->count(), (uint)sl) * l->item( 0 )->height(l);
    else
	return l->sizeHint().height();
}


/*!
    Pops up the combobox popup list.

    If the list is empty, no items appear.
*/

void Q3ComboBox::popup()
{
    if ( !count() || d->poppedUp )
	return;

    QStyleOptionComboBox opt = d->getStyleOption();
    if( !d->usingListBox() || style()->styleHint(QStyle::SH_ComboBox_Popup, &opt, this) ) {
	if(d->usingListBox()) {
	    if(!d->popup()) {
		Q3ComboBoxPopup *p = new Q3ComboBoxPopup( this, "in-combo" );
		d->setPopupMenu( p, false );
		p->setFont( font() );
		connect( p, SIGNAL(activated(int)), SLOT(internalActivate(int)) );
		connect( p, SIGNAL(highlighted(int)), SLOT(internalHighlight(int)) );
	    }
	    d->popup()->clear();
	    for(unsigned int i = 0; i < d->listBox()->count(); i++) {
		Q3ListBoxItem *item = d->listBox()->item(i);
		if(item->rtti() == Q3ListBoxText::RTTI) {
		    d->popup()->insertItem(escapedComboString(item->text()), i, i);
		} else if(item->rtti() == Q3ListBoxPixmap::RTTI) {
		    if(item->pixmap())
			d->popup()->insertItem(QIcon(*item->pixmap()), escapedComboString(item->text()), i, i);
		    else
			d->popup()->insertItem(escapedComboString(item->text()), i, i);
		} else {
		    d->popup()->insertItem(new Q3ComboBoxPopupItem(item), i, i);
		}
	    }
	}
	d->popup()->installEventFilter( this );
	if(d->popup() && style()->styleHint(QStyle::SH_ComboBox_Popup, &opt, this))
	    d->popup()->setItemChecked(this->d->current, true);
	d->popup()->popup( mapToGlobal( QPoint(0,0) ), this->d->current );
	update();
    } else {
	// Send all listbox events to eventFilter():
	Q3ListBox* lb = d->listBox();
	lb->triggerUpdate( true );
	lb->installEventFilter( this );
	d->mouseWasInsidePopup = false;
	int w = lb->variableWidth() ? lb->sizeHint().width() : width();
	int h = listHeight( lb, d->sizeLimit ) + 2;
	QRect screen = QApplication::desktop()->availableGeometry( this );

	int sx = screen.x();				// screen pos
	int sy = screen.y();
	int sw = screen.width();			// screen width
	int sh = screen.height();			// screen height
	QPoint pos = mapToGlobal( QPoint(0,height()) );
	// ## Similar code is in QPopupMenu
	int x = pos.x();
	int y = pos.y();

	// the complete widget must be visible
	if ( x + w > sx + sw )
	    x = sx+sw - w;
	if ( x < sx )
	    x = sx;
	if (y + h > sy+sh && y - h - height() >= 0 )
	    y = y - h - height();

        opt.rect = QRect(x, y, w, h);
       	QRect rect = style()->subControlRect(QStyle::CC_ComboBox, &opt,
                                             QStyle::SC_ComboBoxListBoxPopup, this);

	// work around older styles that don't implement the combobox
	// listbox popup subcontrol
	if ( rect.isNull() )
	    rect.setRect( x, y, w, h );
	lb->setGeometry( rect );

	lb->raise();
	bool block = lb->signalsBlocked();
	lb->blockSignals( true );
	Q3ListBoxItem* currentLBItem = 0;
	if ( editable() && currentText() != text( currentItem() ) )
	    currentLBItem = lb->findItem( currentText() );

	currentLBItem = currentLBItem ? currentLBItem : lb->item( d->current );

	lb->setCurrentItem( currentLBItem );
	lb->setContentsPos( lb->contentsX(),
			    lb->viewportToContents( lb->itemRect( currentLBItem ).topLeft() ).y() );

	// set the current item to also be the selected item if it isn't already
	if ( currentLBItem && currentLBItem->isSelectable() && !currentLBItem->isSelected() )
	    lb->setSelected( currentLBItem, true );
	lb->blockSignals( block );
	lb->setVScrollBarMode(Q3ScrollView::Auto);

#ifndef QT_NO_EFFECTS
	if ( QApplication::isEffectEnabled( Qt::UI_AnimateCombo ) ) {
	    if ( lb->y() < mapToGlobal(QPoint(0,0)).y() )
		qScrollEffect( lb, QEffects::UpScroll );
	    else
		qScrollEffect( lb );
	} else
#endif
	    lb->show();
    }
    d->poppedUp = true;
}


/*!
  Updates the widget mask.

  \sa QWidget::setMask()
*/
void Q3ComboBox::updateMask()
{
    QBitmap bm( size() );
    bm.fill( Qt::color0 );

    QStyleOptionComboBox opt = d->getStyleOption();
    {
	QPainter p(&bm);
        p.initFrom(this);
        p.fillRect(opt.rect, Qt::color1); // qcommonstyle old drawComplexControl implementation
    }

    setMask( bm );
}

/*!
  \internal
  Pops down (removes) the combobox popup list box.
*/
void Q3ComboBox::popDownListBox()
{
    Q_ASSERT( d->usingListBox() );
    d->listBox()->removeEventFilter( this );
    d->listBox()->viewport()->removeEventFilter( this );
    d->listBox()->hide();
    d->listBox()->setCurrentItem( d->current );
    if ( d->arrowDown ) {
	d->arrowDown = false;
	repaint( false );
    }
    d->poppedUp = false;
}


/*!
  \internal
  Re-indexes the identifiers in the popup list.
*/

void Q3ComboBox::reIndex()
{
    if ( !d->usingListBox() ) {
	int cnt = count();
	while ( cnt-- )
	    d->popup()->setId( cnt, cnt );
    }
}

/*!
  \internal
  Repaints the combobox.
*/

void Q3ComboBox::currentChanged()
{
    if ( d->autoresize )
	adjustSize();
    update();

#if defined(QT_ACCESSIBILITY_SUPPORT)
    QAccessible::updateAccessibility( this, 0, QAccessible::ValueChanged );
#endif
}

/*! \reimp

  \internal

  The event filter steals events from the popup or listbox when they
  are popped up. It makes the popup stay up after a short click in
  motif style. In windows style it toggles the arrow button of the
  combobox field, and activates an item and takes down the listbox
  when the mouse button is released.
*/

bool Q3ComboBox::eventFilter( QObject *object, QEvent *event )
{
    QStyleOptionComboBox opt = d->getStyleOption();
    if ( !event )
	return true;
    else if ( object == d->ed ) {
	if ( event->type() == QEvent::KeyPress ) {
	    bool isAccepted = ( (QKeyEvent*)event )->isAccepted();
	    keyPressEvent( (QKeyEvent *)event );
	    if ( ((QKeyEvent *)event)->isAccepted() ) {
		d->completeNow = false;
		return true;
	    } else if ( ((QKeyEvent *)event)->key() != Qt::Key_End ) {
		d->completeNow = true;
		d->completeAt = d->ed->cursorPosition();
	    }
	    if ( isAccepted )
		( (QKeyEvent*)event )->accept();
	    else
		( (QKeyEvent*)event )->ignore();
	} else if ( event->type() == QEvent::KeyRelease ) {
	    keyReleaseEvent( (QKeyEvent *)event );
	    return ((QKeyEvent *)event)->isAccepted();
	} else if ( event->type() == QEvent::FocusIn ) {
	    focusInEvent( (QFocusEvent *)event );
	} else if ( event->type() == QEvent::FocusOut ) {
	    focusOutEvent( (QFocusEvent *)event );
	} else if ( d->useCompletion && d->completeNow ) {
	    d->completeNow = false;
	    if ( !d->ed->text().isNull() &&
		 d->ed->cursorPosition() > d->completeAt &&
		 d->ed->cursorPosition() == (int)d->ed->text().length() ) {
		QString ct( d->ed->text() );
		int i = completionIndex( ct, currentItem() );
		if ( i > -1 ) {
		    QString it = text( i );
		    d->ed->validateAndSet( it, ct.length(),
					   ct.length(), it.length() );
		    d->current = i;
                    // ### sets current item without emitting signals. This is to
		    // make sure the right item is current if you change current with
		    // wheel/up/down. While typing current is not valid anyway. Fix properly
		    // in 4.0.
		}
	    }
	}
    } else if ( d->usingListBox() && ( object == d->listBox() ||
                                       object == d->listBox()->viewport() )) {
	QMouseEvent *e = (QMouseEvent*)event;
	switch( event->type() ) {
	case QEvent::MouseMove:
	    if ( !d->mouseWasInsidePopup  ) {
		QPoint pos = e->pos();
		if ( d->listBox()->rect().contains( pos ) )
		    d->mouseWasInsidePopup = true;
		// Check if arrow button should toggle
		if ( d->arrowPressed ) {
		    QPoint comboPos;
		    comboPos = mapFromGlobal( d->listBox()->mapToGlobal(pos) );
		    QRect arrowRect = style()->subControlRect(QStyle::CC_ComboBox, &opt,
                                                              QStyle::SC_ComboBoxArrow, this);
		    if ( arrowRect.contains( comboPos ) ) {
			if ( !d->arrowDown  ) {
			    d->arrowDown = true;
			    repaint( false );
			}
		    } else {
			if ( d->arrowDown  ) {
			    d->arrowDown = false;
			    repaint( false );
			}
		    }
		}
	    } else if ((e->state() & ( Qt::RightButton | Qt::LeftButton | Qt::MidButton ) ) == 0 &&
		       style()->styleHint(QStyle::SH_ComboBox_ListMouseTracking, &opt, this)) {
		QWidget *mouseW = QApplication::widgetAt( e->globalPos(), true );
		if ( mouseW == d->listBox()->viewport() ) { //###
		    QMouseEvent m( QEvent::MouseMove, e->pos(), e->globalPos(),
				   Qt::NoButton, Qt::LeftButton );
		    QApplication::sendEvent( object, &m ); //### Evil
		    return true;
		}
	    }

	    break;
	case QEvent::MouseButtonRelease:
	    if ( d->listBox()->rect().contains( e->pos() ) ) {
		QMouseEvent tmp( QEvent::MouseButtonDblClick,
				 e->pos(), e->button(), e->state() ) ;
		// will hide popup
		QApplication::sendEvent( object, &tmp );
		return true;
	    } else {
		if ( d->mouseWasInsidePopup ) {
		    popDownListBox();
		} else {
		    d->arrowPressed = false;
		    if ( d->arrowDown  ) {
			d->arrowDown = false;
			repaint( false );
		    }
		}
	    }
	    break;
	case QEvent::MouseButtonDblClick:
	case QEvent::MouseButtonPress:
	    if ( !d->listBox()->rect().contains( e->pos() ) ) {
		QPoint globalPos = d->listBox()->mapToGlobal(e->pos());
		if ( QApplication::widgetAt( globalPos, true ) == this ) {
		    d->discardNextMousePress = true;
		    // avoid popping up again
		}
		popDownListBox();
		return true;
	    }
	    break;
	case QEvent::KeyPress:
	    switch( ((QKeyEvent *)event)->key() ) {
	    case Qt::Key_Up:
	    case Qt::Key_Down:
		if ( !(((QKeyEvent *)event)->state() & Qt::AltModifier) )
		    break;
	    case Qt::Key_F4:
	    case Qt::Key_Escape:
		if ( d->poppedUp ) {
		    popDownListBox();
		    return true;
		}
		break;
	    case Qt::Key_Enter:
	    case Qt::Key_Return:
		// work around QDialog's enter handling
		return false;
	    default:
		break;
	    }
	    break;
	case QEvent::Hide:
	    popDownListBox();
	    break;
	default:
	    break;
	}
    } else if ( (!d->usingListBox() || style()->styleHint(QStyle::SH_ComboBox_Popup, &opt, this)) &&
		object == d->popup() ) {
	QMouseEvent *e = (QMouseEvent*)event;
	switch ( event->type() ) {
	case QEvent::MouseButtonRelease:
	    if ( d->shortClick ) {
		QMouseEvent tmp( QEvent::MouseMove,
				 e->pos(), e->button(), e->state() ) ;
		// highlight item, but don't pop down:
		QApplication::sendEvent( object, &tmp );
		return true;
	    }
	    break;
	case QEvent::MouseButtonDblClick:
	case QEvent::MouseButtonPress:
	    if ( !d->popup()->rect().contains( e->pos() ) ) {
                d->poppedUp = false;
                d->arrowDown = false;
		// remove filter, event will take down popup:
		d->popup()->removeEventFilter( this );
		// ### uglehack!
		// call internalHighlight so the highlighed signal
		// will be emitted at least as often as necessary.
		// it may be called more often than necessary
		internalHighlight( d->current );
	    }
	    break;
	case QEvent::Hide:
	    d->poppedUp = false;
	    break;
	default:
	    break;
	}
    }
    return QWidget::eventFilter( object, event );
}


/*!
    Returns the index of the first item \e after \a startingAt of
    which \a prefix is a case-insensitive prefix. Returns -1 if no
    items start with \a prefix.
*/

int Q3ComboBox::completionIndex( const QString & prefix,
				int startingAt = 0 ) const
{
    int start = startingAt;
    if ( start < 0 || start >= count() )
	start = 0;
    if ( start >= count() )
	return -1;
    QString match = prefix.lower();
    if ( match.length() < 1 )
	return start;

    QString current;
    int i = start;
    do {
	current = text( i ).lower();
	if ( current.startsWith( match ) )
	    return i;
	i++;
	if ( i == count() )
	    i = 0;
    } while ( i != start );
    return -1;
}

int Q3ComboBox::sizeLimit() const
{
    return d ? d->sizeLimit : INT_MAX;
}

void Q3ComboBox::setSizeLimit( int lines )
{
    d->sizeLimit = lines;
}


int Q3ComboBox::maxCount() const
{
    return d ? d->maxCount : INT_MAX;
}

void Q3ComboBox::setMaxCount( int count )
{
    int l = this->count();
    while( --l > count )
	removeItem( l );
    d->maxCount = count;
}

Q3ComboBox::Policy Q3ComboBox::insertionPolicy() const
{
    return d->p;
}

void Q3ComboBox::setInsertionPolicy( Policy policy )
{
    d->p = policy;
}



/*!
  Internal slot to keep the line editor up to date.
*/

void Q3ComboBox::returnPressed()
{
    QString s( d->ed->text() );

    if ( s.isEmpty() )
	return;

    int c = 0;
    bool doInsert = true;
    if ( !d->duplicatesEnabled ) {
	for ( int i = 0; i < count(); ++i ) {
	    if ( s == text( i ) ) {
		doInsert = false;
		c = i;
		break;
	    }
	}
    }

    if ( doInsert ) {
	if ( insertionPolicy() != NoInsert ) {
	    int cnt = count();
	    while ( cnt >= d->maxCount ) {
		removeItem( --cnt );
	    }
	}

	switch ( insertionPolicy() ) {
	case InsertAtCurrent:
	    if (count() == 0)
		insertItem(s);
	    else if ( s != text( currentItem() ) )
		changeItem( s, currentItem() );
	    emit activated( currentItem() );
	    emit activated( s );
	    return;
	case NoInsert:
	    emit activated( s );
	    return;
	case InsertAtTop:
	    c = 0;
	    break;
	case InsertAtBottom:
	    c = count();
	    break;
	case InsertBeforeCurrent:
	    c = currentItem();
	    break;
	case InsertAfterCurrent:
	    c = count() == 0 ? 0 : currentItem() + 1;
	    break;
	}
	insertItem( s, c );
    }

    setCurrentItem( c );
    emit activated( c );
    emit activated( s );
}


/*!
  Enables the combobox if \a enable is true; otherwise disables it.

  \sa QWidget::enabled
*/

void Q3ComboBox::setEnabled( bool enable )
{
    if ( !enable ) {
	if ( d->usingListBox() ) {
	    popDownListBox();
	} else {
	    d->popup()->removeEventFilter( this );
	    d->popup()->close();
	    d->poppedUp = false;
	}
    }
    QWidget::setEnabled( enable );
}



/*!
    Applies the validator \a v to the combobox so that only text which
    is valid according to \a v is accepted.

    This function does nothing if the combobox is not editable.

    \sa validator() clearValidator() QValidator
*/

void Q3ComboBox::setValidator( const QValidator * v )
{
    if ( d && d->ed )
	d->ed->setValidator( v );
}


/*!
    Returns the validator which constrains editing for this combobox
    if there is one; otherwise returns 0.

    \sa setValidator() clearValidator() QValidator
*/

const QValidator * Q3ComboBox::validator() const
{
    return d && d->ed ? d->ed->validator() : 0;
}


/*!
    This slot is equivalent to setValidator( 0 ).
*/

void Q3ComboBox::clearValidator()
{
    if ( d && d->ed )
	d->ed->setValidator( 0 );
}


/*!
    Sets the combobox to use \a newListBox instead of the current list
    box or popup. As a side effect, it clears the combobox of its
    current contents.

    \warning Q3ComboBox assumes that newListBox->text(n) returns
    non-null for 0 \<= n \< newListbox->count(). This assumption is
    necessary because of the line edit in Q3ComboBox.
*/

void Q3ComboBox::setListBox( Q3ListBox * newListBox )
{
    clear();

    if ( d->usingListBox() ) {
	delete d->listBox();
    } else {
	delete d->popup();
        d->setPopupMenu(0, false);
    }

    newListBox->reparent( this, Qt::WType_Popup, QPoint(0,0), false );
    d->setListBox( newListBox );
    d->listBox()->setFont( font() );
    d->listBox()->setPalette( palette() );
    d->listBox()->setVScrollBarMode(Q3ScrollView::AlwaysOff);
    d->listBox()->setHScrollBarMode(Q3ScrollView::AlwaysOff);
    d->listBox()->setFrameStyle( Q3Frame::Box | Q3Frame::Plain );
    d->listBox()->setLineWidth( 1 );
    d->listBox()->resize( 100, 10 );

    connect( d->listBox(), SIGNAL(selected(int)),
	     SLOT(internalActivate(int)) );
    connect( d->listBox(), SIGNAL(highlighted(int)),
	     SLOT(internalHighlight(int)));
}


/*!
    Returns the current list box, or 0 if there is no list box.
    (Q3ComboBox can use QPopupMenu instead of QListBox.) Provided to
    match setListBox().

    \sa setListBox()
*/

Q3ListBox * Q3ComboBox::listBox() const
{
    return d && d->usingListBox() ? d->listBox() : 0;
}

/*!
    Returns the line edit, or 0 if there is no line edit.

    Only editable listboxes have a line editor.
*/
QLineEdit* Q3ComboBox::lineEdit() const
{
    return d->ed;
}



/*!
    Clears the line edit without changing the combobox's contents.
    Does nothing if the combobox isn't editable.

    This is particularly useful when using a combobox as a line edit
    with history. For example you can connect the combobox's
    activated() signal to clearEdit() in order to present the user
    with a new, empty line as soon as Enter is pressed.

    \sa setEditText()
*/

void Q3ComboBox::clearEdit()
{
    if ( d && d->ed )
	d->ed->clear();
}


/*!
    Sets the text in the line edit to \a newText without changing the
    combobox's contents. Does nothing if the combobox isn't editable.

    This is useful e.g. for providing a good starting point for the
    user's editing and entering the change in the combobox only when
    the user presses Enter.

    \sa clearEdit() insertItem()
*/

void Q3ComboBox::setEditText( const QString &newText )
{
    if ( d && d->ed ) {
	d->updateLinedGeometry();
	d->ed->setText( newText );
    }
}

void Q3ComboBox::setAutoCompletion( bool enable )
{
    d->useCompletion = enable;
    d->completeNow = false;
}


bool Q3ComboBox::autoCompletion() const
{
    return d->useCompletion;
}

/*!\reimp
 */
void Q3ComboBox::styleChange( QStyle& s )
{
    d->sizeHint = QSize();		// invalidate size hint...
    if ( d->ed )
	d->updateLinedGeometry();
    QWidget::styleChange( s );
}

bool Q3ComboBox::editable() const
{
    return d->ed != 0;
}

void Q3ComboBox::setEditable( bool y )
{
    if ( y == editable() )
	return;
    if ( y ) {
	if ( !d->usingListBox() )
	    setUpListBox();
	setUpLineEdit();
	d->ed->show();
	if ( currentItem() )
	    setEditText( currentText() );
    } else {
	delete d->ed;
	d->ed = 0;
    }

    setFocusPolicy(Qt::StrongFocus);
    updateGeometry();
    update();
}


void Q3ComboBox::setUpListBox()
{
    d->setListBox( new Q3ListBox( this, "in-combo", Qt::WType_Popup ) );
    d->listBox()->setFont( font() );
    d->listBox()->setPalette( palette() );
    d->listBox()->setVScrollBarMode( Q3ListBox::AlwaysOff );
    d->listBox()->setHScrollBarMode( Q3ListBox::AlwaysOff );
    d->listBox()->setFrameStyle( Q3Frame::Box | Q3Frame::Plain );
    d->listBox()->setLineWidth( 1 );
    d->listBox()->resize( 100, 10 );

    connect( d->listBox(), SIGNAL(selected(int)),
	     SLOT(internalActivate(int)) );
    connect( d->listBox(), SIGNAL(highlighted(int)),
	     SLOT(internalHighlight(int)));
}


void Q3ComboBox::setUpLineEdit()
{
    if ( !d->ed )
	setLineEdit( new QLineEdit( this, "combo edit" ) );
}

/*!
    Sets the line edit to use \a edit instead of the current line edit.
*/

void Q3ComboBox::setLineEdit( QLineEdit *edit )
{
    if ( !edit ) {
#if defined(QT_CHECK_NULL)
	Q_ASSERT( edit != 0 );
#endif
	return;
    }

    edit->setText( currentText() );
    delete d->ed;
    d->ed = edit;

    if ( edit->parent() != this )
	edit->reparent( this, QPoint(0,0), false );

    connect (edit, SIGNAL(textChanged(QString)),
	     this, SIGNAL(textChanged(QString)) );
    connect( edit, SIGNAL(returnPressed()), SLOT(returnPressed()) );

    edit->setFrame( false );
    d->updateLinedGeometry();
    edit->installEventFilter( this );
    setFocusProxy( edit );
    setFocusPolicy(Qt::StrongFocus);
    setInputMethodEnabled( true );

    if ( !d->usingListBox() )
	setUpListBox();

    if ( isVisible() )
	edit->show();

    updateGeometry();
    update();
}

/*!
  Hides the combobox.

  \sa QWidget::hide()
*/
void Q3ComboBox::hide()
{
    QWidget::hide();

    if (listBox())
	listBox()->hide();
    else if (d->popup())
	d->popup()->hide();
}

QT_END_NAMESPACE

#endif // QT_NO_COMBOBOX
