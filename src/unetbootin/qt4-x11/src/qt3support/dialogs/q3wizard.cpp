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

#include "q3wizard.h"

#include "qlayout.h"
#include "qpushbutton.h"
#include "qcursor.h"
#include "qlabel.h"
#include "qapplication.h"
#include "qlist.h"
#include "qpainter.h"
#include "q3accel.h"

QT_BEGIN_NAMESPACE

using namespace Qt;

/*!
    \class Q3Wizard
    \compat
    \brief The Q3Wizard class provides a framework for wizard dialogs.

    A wizard is a special type of input dialog that consists of a
    sequence of dialog pages. A wizard's purpose is to walk the user
    through a process step by step. Wizards are useful for complex or
    infrequently occurring tasks that people may find difficult to
    learn or do.

    Q3Wizard provides page titles and displays Next, Back, Finish,
    Cancel, and Help push buttons, as appropriate to the current
    position in the page sequence. These buttons can be
    enabled/disabled using setBackEnabled(), setNextEnabled(),
    setFinishEnabled() and setHelpEnabled().

    Create and populate dialog pages that inherit from QWidget and add
    them to the wizard using addPage(). Use insertPage() to add a
    dialog page at a certain position in the page sequence. Use
    removePage() to remove a page from the page sequence.

    Use currentPage() to retrieve a pointer to the currently displayed
    page. page() returns a pointer to the page at a certain position
    in the page sequence.

    Use pageCount() to retrieve the total number of pages in the page
    sequence. indexOf() will return the index of a page in the page
    sequence.

    Q3Wizard provides functionality to mark pages as appropriate (or
    not) in the current context with setAppropriate(). The idea is
    that a page may be irrelevant and should be skipped depending on
    the data entered by the user on a preceding page.

    It is generally considered good design to provide a greater number
    of simple pages with fewer choices rather than a smaller number of
    complex pages.
*/


class Q3WizardPrivate
{
public:

    virtual ~Q3WizardPrivate()
    {
        foreach(Page *page, pages)
            delete page;
    }

    struct Page {
	Page( QWidget * widget, const QString & title ):
	    w( widget ), t( title ),
	    backEnabled( true ), nextEnabled( true ), finishEnabled( false ),
	    helpEnabled( true ),
	    appropriate( true )
	{}
	QWidget * w;
	QString t;
	bool backEnabled;
	bool nextEnabled;
	bool finishEnabled;
	bool helpEnabled;
	bool appropriate;
    };

    QVBoxLayout * v;
    Page * current;
    QList<Page *> pages;
    QLabel * title;
    QPushButton * backButton;
    QPushButton * nextButton;
    QPushButton * finishButton;
    QPushButton * cancelButton;
    QPushButton * helpButton;

    QFrame * hbar1, * hbar2;

#ifndef QT_NO_ACCEL
    Q3Accel * accel;
    int backAccel;
    int nextAccel;
#endif

    Page * page( const QWidget * w )
    {
	if ( !w )
	    return 0;
	int i = pages.count();
	while( --i >= 0 && pages.at( i ) && pages.at( i )->w != w ) { }
	return i >= 0 ? pages.at( i ) : 0;
    }

};


/*!
    Constructs an empty wizard dialog. The \a parent, \a name, \a
    modal and \a f arguments are passed to the QDialog constructor.
*/

Q3Wizard::Q3Wizard(QWidget *parent, const char *name, bool modal, Qt::WindowFlags f )
    : QDialog( parent, name, modal, f )
{
    d = new Q3WizardPrivate();
    d->current = 0; // not quite true, but...
    d->title = new QLabel( this, "title label" );

    // create in nice tab order
    d->nextButton = new QPushButton( this, "next" );
    d->finishButton = new QPushButton( this, "finish" );
    d->helpButton = new QPushButton( this, "help" );
    d->backButton = new QPushButton( this, "back" );
    d->cancelButton = new QPushButton( this, "cancel" );

    d->v = 0;
    d->hbar1 = 0;
    d->hbar2 = 0;

    d->cancelButton->setText( tr( "&Cancel" ) );
    d->backButton->setText( tr( "< &Back" ) );
    d->nextButton->setText( tr( "&Next >" ) );
    d->finishButton->setText( tr( "&Finish" ) );
    d->helpButton->setText( tr( "&Help" ) );

    d->nextButton->setDefault( true );

    connect( d->backButton, SIGNAL(clicked()),
	     this, SLOT(back()) );
    connect( d->nextButton, SIGNAL(clicked()),
	     this, SLOT(next()) );
    connect( d->finishButton, SIGNAL(clicked()),
	     this, SLOT(accept()) );
    connect( d->cancelButton, SIGNAL(clicked()),
	     this, SLOT(reject()) );
    connect( d->helpButton, SIGNAL(clicked()),
	     this, SLOT(help()) );

#ifndef QT_NO_ACCEL
    d->accel = new Q3Accel( this, "arrow-key accel" );
    d->backAccel = d->accel->insertItem( Qt::ALT + Qt::Key_Left );
    d->accel->connectItem( d->backAccel, this, SLOT(back()) );
    d->nextAccel = d->accel->insertItem( Qt::ALT + Qt::Key_Right );
    d->accel->connectItem( d->nextAccel, this, SLOT(next()) );
#endif
}


/*!
    Destroys the object and frees any allocated resources, including
    all pages and controllers.
*/

Q3Wizard::~Q3Wizard()
{
    delete d;
}


/*!
    \internal
*/

void Q3Wizard::setVisible(bool show)
{
    if ( show && !d->current ) {
	// No page yet
	if ( pageCount() > 0 )
	    showPage( d->pages.at( 0 )->w );
	else
	    showPage( 0 );
    }

    QDialog::setVisible(show);
}


/*!
    \internal
*/

void Q3Wizard::setFont( const QFont & font )
{
    QApplication::postEvent( this, new QEvent( QEvent::LayoutHint ) );
    QDialog::setFont( font );
}

/*!
    Adds \a page to the end of the page sequence, with the title, \a
    title.
*/

void Q3Wizard::addPage( QWidget * page, const QString & title )
{
    if ( !page )
	return;
    if ( d->page( page ) ) {
#if defined(QT_CHECK_STATE)
	qWarning( "Q3Wizard::addPage(): already added %s/%s to %s/%s",
		  page->className(), page->name(),
		  className(), name() );
#endif
	return;
    }
    int i = d->pages.count();

    if( i > 0 )
	d->pages.at( i - 1 )->nextEnabled = true;

    Q3WizardPrivate::Page * p = new Q3WizardPrivate::Page( page, title );
    p->backEnabled = ( i > 0 );
    d->pages.append( p );
}

/*!
    Inserts \a page at position \a index into the page sequence, with
    title \a title. If \a index is -1, the page will be appended to
    the end of the wizard's page sequence.
*/

void Q3Wizard::insertPage( QWidget * page, const QString & title, int index )
{
    if ( !page )
	return;
    if ( d->page( page ) ) {
#if defined(QT_CHECK_STATE)
	qWarning( "Q3Wizard::insertPage(): already added %s/%s to %s/%s",
		  page->className(), page->name(),
		  className(), name() );
#endif
	return;
    }

    if ( index < 0  || index > (int)d->pages.count() )
	index = d->pages.count();

    if( index > 0 && ( index == (int)d->pages.count() ) )
	d->pages.at( index - 1 )->nextEnabled = true;

    Q3WizardPrivate::Page * p = new Q3WizardPrivate::Page( page, title );
    p->backEnabled = ( index > 0 );
    p->nextEnabled = ( index < (int)d->pages.count() );

    d->pages.insert( index, p );
}

/*!
    \fn void Q3Wizard::selected(const QString &title)

    This signal is emitted when the current page changes. The
    \a title parameter contains the title of the selected page.
*/


/*!
    Makes \a page the current page and emits the selected() signal.

    This virtual function is called whenever a different page is to
    be shown, including the first time the Q3Wizard is shown.
    By reimplementing it (and calling Q3Wizard::showPage()),
    you can prepare each page prior to it being shown.
*/

void Q3Wizard::showPage( QWidget * page )
{
    Q3WizardPrivate::Page * p = d->page( page );
    if ( p ) {
	int i;
	for( i = 0; i < (int)d->pages.count() && d->pages.at( i ) != p; i++ ) {}
	bool notFirst( false );

	if( i ) {
	    i--;
	    while( ( i >= 0 ) && !notFirst ) {
		notFirst |= appropriate( d->pages.at( i )->w );
		i--;
	    }
	}
	setBackEnabled( notFirst );
	setNextEnabled( true );

        page->show();
        foreach(Q3WizardPrivate::Page *ppage, d->pages) {
            if (ppage->w != page)
                ppage->w->hide();
        }

	d->current = p;
    }

    layOut();
    updateButtons();
    emit selected( p ? p->t : QString() );
}


/*!
    Returns the number of pages in the wizard.
*/

int Q3Wizard::pageCount() const
{
    return d->pages.count();
}

/*!
    Returns the position of page \a page. If the page is not part of
    the wizard -1 is returned.
*/

int Q3Wizard::indexOf( QWidget* page ) const
{
    Q3WizardPrivate::Page * p = d->page( page );
    if ( !p ) return -1;

    return d->pages.indexOf( p );
}

/*!
    Called when the user clicks the Back button; this function shows
    the preceding relevant page in the sequence.

    \sa appropriate()
*/
void Q3Wizard::back()
{
    int i = 0;

    while( i < (int)d->pages.count() && d->pages.at( i ) &&
	   d->current && d->pages.at( i )->w != d->current->w )
	i++;

    i--;
    while( i >= 0 && ( !d->pages.at( i ) || !appropriate( d->pages.at( i )->w ) ) )
	i--;

    if( i >= 0 )
       if( d->pages.at( i ) )
	    showPage( d->pages.at( i )->w );
}


/*!
    Called when the user clicks the Next button, this function shows
    the next relevant page in the sequence.

    \sa appropriate()
*/
void Q3Wizard::next()
{
    int i = 0;
    while( i < (int)d->pages.count() && d->pages.at( i ) &&
	   d->current && d->pages.at( i )->w != d->current->w )
	i++;
    i++;
    while( i <= (int)d->pages.count()-1 &&
	   ( !d->pages.at( i ) || !appropriate( d->pages.at( i )->w ) ) )
	i++;
    // if we fell of the end of the world, step back
    while ( i > 0 && (i >= (int)d->pages.count() || !d->pages.at( i ) ) )
	i--;
    if ( d->pages.at( i ) )
	showPage( d->pages.at( i )->w );
}


/*!
    \fn void Q3Wizard::helpClicked()

    This signal is emitted when the user clicks on the Help button.
*/

/*!
    Called when the user clicks the Help button, this function emits
    the helpClicked() signal.
*/

void Q3Wizard::help()
{
    QWidget *page = d->current ? d->current->w : 0;
    if ( !page )
	return;

#if 0
    Q3WizardPage *wpage = qobject_cast<Q3WizardPage*>(page);
    if ( wpage )
	emit wpage->helpClicked();
#endif
    emit helpClicked();
}


void Q3Wizard::setBackEnabled( bool enable )
{
    d->backButton->setEnabled( enable );
#ifndef QT_NO_ACCEL
    d->accel->setItemEnabled( d->backAccel, enable );
#endif
}


void Q3Wizard::setNextEnabled( bool enable )
{
    d->nextButton->setEnabled( enable );
#ifndef QT_NO_ACCEL
    d->accel->setItemEnabled( d->nextAccel, enable );
#endif
}


void Q3Wizard::setHelpEnabled( bool enable )
{
    d->helpButton->setEnabled( enable );
}


/*!
    \fn void Q3Wizard::setFinish(QWidget *widget, bool finish)
    \obsolete

    Use setFinishEnabled() instead.
*/

/*!
    If \a enable is true, page \a page has a Back button; otherwise \a
    page has no Back button. By default all pages have this button.
*/
void Q3Wizard::setBackEnabled( QWidget * page, bool enable )
{
    Q3WizardPrivate::Page * p = d->page( page );
    if ( !p )
	return;

    p->backEnabled = enable;
    updateButtons();
}


/*!
    If \a enable is true, page \a page has a Next button; otherwise
    the Next button on \a page is disabled. By default all pages have
    this button.
*/

void Q3Wizard::setNextEnabled( QWidget * page, bool enable )
{
    Q3WizardPrivate::Page * p = d->page( page );
    if ( !p )
	return;

    p->nextEnabled = enable;
    updateButtons();
}


/*!
    If \a enable is true, page \a page has a Finish button; otherwise
    \a page has no Finish button. By default \e no page has this
    button.
*/
void Q3Wizard::setFinishEnabled( QWidget * page, bool enable )
{
    Q3WizardPrivate::Page * p = d->page( page );
    if ( !p )
	return;

    p->finishEnabled = enable;
    updateButtons();
}


/*!
    If \a enable is true, page \a page has a Help button; otherwise \a
    page has no Help button. By default all pages have this button.
*/
void Q3Wizard::setHelpEnabled( QWidget * page, bool enable )
{
    Q3WizardPrivate::Page * p = d->page( page );
    if ( !p )
	return;

    p->helpEnabled = enable;
    updateButtons();
}


/*!
    Called when the Next button is clicked; this virtual function
    returns true if \a page is relevant for display in the current
    context; otherwise it is ignored by Q3Wizard and returns false. The
    default implementation returns the value set using
    setAppropriate(). The ultimate default is true.

    \warning The last page of the wizard will be displayed if no page
    is relevant in the current context.
*/

bool Q3Wizard::appropriate( QWidget * page ) const
{
    Q3WizardPrivate::Page * p = d->page( page );
    return p ? p->appropriate : true;
}


/*!
    If \a appropriate is true then page \a page is considered relevant
    in the current context and should be displayed in the page
    sequence; otherwise \a page should not be displayed in the page
    sequence.

    \sa appropriate()
*/
void Q3Wizard::setAppropriate( QWidget * page, bool appropriate )
{
    Q3WizardPrivate::Page * p = d->page( page );
    if ( p )
	p->appropriate = appropriate;
}


void Q3Wizard::updateButtons()
{
    if ( !d->current )
	return;

    int i;
    for( i = 0; i < (int)d->pages.count() && d->pages.at( i ) != d->current; i++ ) {}
    bool notFirst( false );
    if( i ) {
	i--;
	while( ( i >= 0 ) && !notFirst ) {
	    notFirst |= appropriate( d->pages.at( i )->w );
	    i--;
	}
    }
    setBackEnabled( d->current->backEnabled && notFirst );
    setNextEnabled( d->current->nextEnabled );
    d->finishButton->setEnabled( d->current->finishEnabled );
    d->helpButton->setEnabled( d->current->helpEnabled );

    if ( ( d->current->finishEnabled && !d->finishButton->isVisible() ) ||
	 ( d->current->backEnabled && !d->backButton->isVisible() ) ||
	 ( d->current->nextEnabled && !d->nextButton->isVisible() ) ||
	 ( d->current->helpEnabled && !d->helpButton->isVisible() ) )
	layOut();
}


/*!
    Returns a pointer to the current page in the sequence. Although
    the wizard does its best to make sure that this value is never 0,
    it can be if you try hard enough.
*/

QWidget * Q3Wizard::currentPage() const
{
    return d->current ? d->current->w : 0;
}


/*!
    Returns the title of page \a page.
*/

QString Q3Wizard::title( QWidget * page ) const
{
    Q3WizardPrivate::Page * p = d->page( page );
    return p ? p->t : QString();
}

/*!
    Sets the title for page \a page to \a title.
*/

void Q3Wizard::setTitle( QWidget *page, const QString &title )
{
    Q3WizardPrivate::Page * p = d->page( page );
    if ( p )
	p->t = title;
    if ( page == currentPage() )
	d->title->setText( title );
}

/*!
    \property Q3Wizard::titleFont
    \brief the font used for page titles

    The default is QApplication::font().
*/
QFont Q3Wizard::titleFont() const
{
    return d->title->font();
}

void Q3Wizard::setTitleFont( const QFont & font )
{
    d->title->setFont( font );
}


/*!
    Returns a pointer to the dialog's Back button

    By default, this button is connected to the back() slot, which is
    virtual so you can reimplement it in a Q3Wizard subclass. Use
    setBackEnabled() to enable/disable this button.
*/
QPushButton * Q3Wizard::backButton() const
{
    return d->backButton;
}


/*!
    Returns a pointer to the dialog's Next button

    By default, this button is connected to the next() slot, which is
    virtual so you can reimplement it in a Q3Wizard subclass. Use
    setNextEnabled() to enable/disable this button.
*/
QPushButton * Q3Wizard::nextButton() const
{
    return d->nextButton;
}


/*!
    Returns a pointer to the dialog's Finish button

    By default, this button is connected to the QDialog::accept()
    slot, which is virtual so you can reimplement it in a Q3Wizard
    subclass. Use setFinishEnabled() to enable/disable this button.
*/
QPushButton * Q3Wizard::finishButton() const
{
    return d->finishButton;
}


/*!
    Returns a pointer to the dialog's Cancel button

    By default, this button is connected to the QDialog::reject()
    slot, which is virtual so you can reimplement it in a Q3Wizard
    subclass.
*/
QPushButton * Q3Wizard::cancelButton() const
{
    return d->cancelButton;
}


/*!
    Returns a pointer to the dialog's Help button

    By default, this button is connected to the help() slot, which is
    virtual so you can reimplement it in a Q3Wizard subclass. Use
    setHelpEnabled() to enable/disable this button.
*/
QPushButton * Q3Wizard::helpButton() const
{
    return d->helpButton;
}


/*!
    This virtual function is responsible for adding the buttons below
    the bottom divider.

    \a layout is the horizontal layout of the entire wizard.
*/

void Q3Wizard::layOutButtonRow( QHBoxLayout * layout )
{
    bool hasHelp = false;
    bool hasEarlyFinish = false;

    int i = d->pages.count() - 2;
    while ( !hasEarlyFinish && i >= 0 ) {
	if ( d->pages.at( i ) && d->pages.at( i )->finishEnabled )
	    hasEarlyFinish = true;
	i--;
    }
    i = 0;
    while ( !hasHelp && i < (int)d->pages.count() ) {
	if ( d->pages.at( i ) && d->pages.at( i )->helpEnabled )
	    hasHelp = true;
	i++;
    }

    QBoxLayout * h = new QBoxLayout( QBoxLayout::LeftToRight );
    layout->addLayout( h );

    if ( hasHelp )
	h->addWidget( d->helpButton );
    else
	d->helpButton->hide();

    h->addStretch( 42 );

    h->addWidget( d->backButton );

    h->addSpacing( 6 );

    if (d->current == d->pages.at( d->pages.count()-1 ))
        hasEarlyFinish = false;

    if ( hasEarlyFinish ) {
	d->nextButton->show();
	d->finishButton->show();
	h->addWidget( d->nextButton );
	h->addSpacing( 12 );
	h->addWidget( d->finishButton );
    } else if ( d->pages.count() == 0 ||
		(d->current && d->current->finishEnabled) ||
		d->current == d->pages.at( d->pages.count()-1 ) ) {
	d->nextButton->hide();
	d->finishButton->show();
	h->addWidget( d->finishButton );
    } else {
	d->nextButton->show();
	d->finishButton->hide();
	h->addWidget( d->nextButton );
    }

    // if last page is disabled - show finished btn. at lastpage-1
    i = d->pages.count()-1;
    if ( i >= 0 && !appropriate( d->pages.at( i )->w ) &&
	 d->current == d->pages.at( d->pages.count()-2 ) ) {
	d->nextButton->hide();
	d->finishButton->show();
	h->addWidget( d->finishButton );
    }

    h->addSpacing( 12 );
    h->addWidget( d->cancelButton );
}


/*!
    This virtual function is responsible for laying out the title row.

    \a layout is the horizontal layout for the wizard, and \a
    title is the title for this page. This function is called every
    time \a title changes.
*/

void Q3Wizard::layOutTitleRow( QHBoxLayout * layout, const QString & title )
{
    d->title->setText( title );
    layout->addWidget( d->title, 10 );
}


/*

*/

void Q3Wizard::layOut()
{
    delete d->v;
    d->v = new QVBoxLayout( this, 6, 0, "top-level layout" );

    QHBoxLayout * l;
    l = new QHBoxLayout( 6 );
    d->v->addLayout( l, 0 );
    layOutTitleRow( l, d->current ? d->current->t : QString() );

    if ( ! d->hbar1 ) {
	d->hbar1 = new QFrame( this, "<hr>", 0 );
	d->hbar1->setFrameStyle( QFrame::Sunken + QFrame::HLine );
	d->hbar1->setFixedHeight( 12 );
    }

    d->v->addWidget( d->hbar1 );

    if (d->current)
        d->v->addWidget(d->current->w, 10);

    if ( ! d->hbar2 ) {
	d->hbar2 = new QFrame( this, "<hr>", 0 );
	d->hbar2->setFrameStyle( QFrame::Sunken + QFrame::HLine );
	d->hbar2->setFixedHeight( 12 );
    }
    d->v->addWidget( d->hbar2 );

    l = new QHBoxLayout( 6 );
    d->v->addLayout( l );
    layOutButtonRow( l );
    d->v->activate();
}


/*!
    \reimp
*/

bool Q3Wizard::eventFilter( QObject * o, QEvent * e )
{
    return QDialog::eventFilter( o, e );
}


/*!
    Removes \a page from the page sequence but does not delete the
    page. If \a page is currently being displayed, Q3Wizard will
    display the page that precedes it, or the first page if this was
    the first page.
*/

void Q3Wizard::removePage( QWidget * page )
{
    if ( !page )
	return;

    int i = d->pages.count();
    QWidget* cp = currentPage();
    while( --i >= 0 && d->pages.at( i ) && d->pages.at( i )->w != page ) { }
    if ( i < 0 )
	return;
    delete d->pages.takeAt(i);
    page->hide();

    if( cp == page ) {
	i--;
	if( i < 0 )
	    i = 0;
	if ( pageCount() > 0 )
	    showPage( Q3Wizard::page( i ) );
    } else if (pageCount() > 0) {
        showPage(cp);
    }
}


/*!
    Returns a pointer to the page at position \a index in the
    sequence, or 0 if \a index is out of range. The first page has
    index 0.
*/

QWidget* Q3Wizard::page( int index ) const
{
    if ( index >= pageCount() || index < 0 )
      return 0;
    return d->pages.at( index )->w;
}

QT_END_NAMESPACE
