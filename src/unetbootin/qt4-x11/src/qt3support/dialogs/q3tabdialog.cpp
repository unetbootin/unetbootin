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

#include "q3tabdialog.h"

#include "qtabbar.h"
#include "qtabwidget.h"
#include "qpushbutton.h"
#include "qpainter.h"
#include "qpixmap.h"
#include "qapplication.h"
#include "q3widgetstack.h"
#include "qlayout.h"

QT_BEGIN_NAMESPACE

using namespace Qt;

/*!
    \class Q3TabDialog
    \compat
    \brief The Q3TabDialog class provides a stack of tabbed widgets.

    A tabbed dialog is one in which several "tab pages" are available.
    By clicking on a tab page's tab or by pressing the indicated
    Alt+\e{letter} key combination, the user can select which tab page
    they want to use.

    Q3TabDialog provides a tab bar consisting of single row of tabs at
    the top; each tab has an associated widget which is that tab's
    tab page. In addition, Q3TabDialog provides an OK button and the
    following optional buttons: Apply, Cancel, Defaults and Help.

    The normal way to use Q3TabDialog is to do the following in the
    constructor:
    \list 1
    \i Create a Q3TabDialog.
    \i Create a QWidget for each of the pages in the tab dialog, insert
    children into it, set up geometry management for it, and use
    addTab() (or insertTab()) to set up a tab and keyboard accelerator
    for it.
    \i Set up the buttons for the tab dialog using setOkButton(),
    setApplyButton(), setDefaultsButton(), setCancelButton() and
    setHelpButton().
    \i Connect to the signals and slots.
    \endlist

    If you don't call addTab() the page you have created will not be
    visible. Don't confuse the object name you supply to the
    QWidget constructor and the tab label you supply to addTab();
    addTab() takes user-visible name that appears on the widget's tab
    and may identify an accelerator, whereas the widget name is used
    primarily for debugging.

    Almost all applications have to connect the applyButtonPressed()
    signal to something. applyButtonPressed() is emitted when either OK
    or Apply is clicked, and your slot must copy the dialog's state into
    the application.

    There are also several other signals which may be useful:
    \list
    \i cancelButtonPressed() is emitted when the user clicks Cancel.
    \i defaultButtonPressed() is emitted when the user clicks Defaults;
    the slot it is connected to should reset the state of the dialog to
    the application defaults.
    \i helpButtonPressed() is emitted when the user clicks Help.
    \i aboutToShow() is emitted at the start of show(); if there is any
    chance that the state of the application may change between the
    creation of the tab dialog and the time show() is called, you must
    connect this signal to a slot that resets the state of the dialog.
    \i currentChanged() is emitted when the user selects a page.
    \endlist

    Each tab is either enabled or disabled at any given time (see
    setTabEnabled()). If a tab is enabled the tab text is drawn in
    black and the user can select that tab. If it is disabled the tab
    is drawn in a different way and the user cannot select that tab.
    Note that even if a tab is disabled, the page can still be visible;
    for example, if all of the tabs happen to be disabled.

    You can change a tab's label and iconset using changeTab(). A tab
    page can be removed with removePage() and shown with showPage(). The
    current page is given by currentPage().

    Q3TabDialog does not support tabs on the sides or bottom, nor can
    you set or retrieve the visible page. If you need more functionality
    than Q3TabDialog provides, consider creating a QDialog and using a
    QTabBar with QTabWidgets.

    Most of the functionality in Q3TabDialog is provided by a QTabWidget.
*/

/*!
    \fn void Q3TabDialog::selected(const QString &name)

    This signal is emitted whenever a tab is selected (raised),
    including during the first show(). \a name is the name of the
    selected tab.

    \sa raise()
*/

/*! \fn void Q3TabDialog::currentChanged(QWidget *widget)

    This signal is emitted whenever the current page changes. \a widget
    is the new current page.

    \sa currentPage(), showPage(), tabLabel()
*/

class Q3TabDialogPrivate
{
public:
    Q3TabDialogPrivate();

    QTabWidget* tw;

    QPushButton * ok;
    QPushButton * cb;
    QPushButton * db;
    QPushButton * hb;
    QPushButton * ab;

    QBoxLayout * tll;
};

Q3TabDialogPrivate::Q3TabDialogPrivate()
	: tw(0),
	  ok(0), cb(0), db(0), hb(0), ab(0),
	  tll(0)
{ }

/*!
  Constructs a Q3TabDialog with only an OK button.
  The \a parent, \a name, \a modal and widget flag, \a f, arguments
  are passed on to the QDialog constructor.
*/

Q3TabDialog::Q3TabDialog(QWidget *parent, const char *name, bool modal, Qt::WindowFlags f)
    : QDialog(parent, name, modal, f)
{
    d = new Q3TabDialogPrivate;
    Q_CHECK_PTR(d);

    d->tw = new QTabWidget(this, "tab widget");
    connect (d->tw, SIGNAL (selected(QString)), this, SIGNAL(selected(QString)));
    connect (d->tw, SIGNAL (currentChanged(QWidget*)), this, SIGNAL(currentChanged(QWidget*)));

    d->ok = new QPushButton(this, "ok");
    Q_CHECK_PTR(d->ok);
    d->ok->setText(tr("OK"));
    d->ok->setDefault(true);
    connect(d->ok, SIGNAL(clicked()),
	     this, SIGNAL(applyButtonPressed()));
    connect(d->ok, SIGNAL(clicked()),
	     this, SLOT(accept()));
}


/*!
  Destroys the tab dialog.
*/

Q3TabDialog::~Q3TabDialog()
{
    delete d;
}


/*!
  Sets the font for the tabs to \a font.

  If the widget is visible, the display is updated with the new font
  immediately. There may be some geometry changes, depending on the
  size of the old and new fonts.
*/

void Q3TabDialog::setFont(const QFont & font)
{
    QDialog::setFont(font);
    setSizes();
}


/*!
  \fn void Q3TabDialog::applyButtonPressed();

  This signal is emitted when either the Apply or OK button is clicked.

  It should be connected to a slot (or several slots) that change the
  application's state according to the state of the dialog.

  \sa cancelButtonPressed() defaultButtonPressed() setApplyButton()
*/


/*!
  Returns true if the tab dialog has a Defaults button; otherwise
  returns false.

  \sa setDefaultButton() defaultButtonPressed() hasApplyButton()
  hasCancelButton()
*/

bool Q3TabDialog::hasDefaultButton() const
{
     return d->db != 0;
}


/*!
  Returns true if the tab dialog has a Help button; otherwise returns
  false.

  \sa setHelpButton() helpButtonPressed() hasApplyButton()
  hasCancelButton()
*/

bool Q3TabDialog::hasHelpButton() const
{
     return d->hb != 0;
}


/*!
  \fn void Q3TabDialog::cancelButtonPressed();

  This signal is emitted when the Cancel button is clicked. It is
  automatically connected to QDialog::reject(), which will hide the
  dialog.

  The Cancel button should not change the application's state at all,
  so you should generally not need to connect it to any slot.

  \sa applyButtonPressed() defaultButtonPressed() setCancelButton()
*/


/*!
  Returns true if the tab dialog has a Cancel button; otherwise
  returns false.

  \sa setCancelButton() cancelButtonPressed() hasApplyButton()
  hasDefaultButton()
*/

bool Q3TabDialog::hasCancelButton() const
{
     return d->cb != 0;
}


/*!
  \fn void Q3TabDialog::defaultButtonPressed();

  This signal is emitted when the Defaults button is pressed. It
  should reset the dialog (but not the application) to the "factory
  defaults".

  The application's state should not be changed until the user clicks
  Apply or OK.

  \sa applyButtonPressed() cancelButtonPressed() setDefaultButton()
*/


/*!
  \fn void Q3TabDialog::helpButtonPressed();

  This signal is emitted when the Help button is pressed. It
  could be used to present information about how to use the dialog.

  \sa applyButtonPressed() cancelButtonPressed() setHelpButton()
*/


/*!
  Returns true if the tab dialog has an Apply button; otherwise
  returns false.

  \sa setApplyButton() applyButtonPressed() hasCancelButton()
  hasDefaultButton()
*/

bool Q3TabDialog::hasApplyButton() const
{
    return d->ab != 0;
}


/*!
  Returns true if the tab dialog has an OK button; otherwise returns
  false.

  \sa setOkButton() hasApplyButton() hasCancelButton()
  hasDefaultButton()
*/

bool Q3TabDialog::hasOkButton() const
{
    return d->ok != 0;
}


/*!
  \fn void Q3TabDialog::aboutToShow()

  This signal is emitted by show() when it is time to set the state of
  the dialog's contents. The dialog should reflect the current state
  of the application when it appears; if there is any possibility that
  the state of the application may change between the time you call
  Q3TabDialog() and show(), you should set the
  dialog's state in a slot and connect this signal to it.

  This applies mainly to Q3TabDialog objects that are kept around
  hidden, rather than being created, shown, and deleted afterwards.

  \sa applyButtonPressed(), QWidget::show(), cancelButtonPressed()
*/


/*!
    \internal

    Implemented to delay show()'ing of every page.
*/
void Q3TabDialog::show()
{
    //   Reimplemented in order to delay show()'ing of every page
    //   except the initially visible one, and in order to emit the
    //   aboutToShow() signal.
    if (window() == this)
	d->tw->setFocus();
    emit aboutToShow();
    setSizes();
    setUpLayout();
    QDialog::show();
}



/*!
  Adds another tab and page to the tab view.

  The new page is \a child; the tab's label is \a label.
  Note the difference between the widget name (which you supply to
  widget constructors and to setTabEnabled(), for example) and the tab
  label. The name is internal to the program and invariant, whereas
  the label is shown on-screen and may vary according to language and
  other factors.

  If the tab's \a label contains an ampersand, the letter following
  the ampersand is used as an accelerator for the tab, e.g. if the
  label is "Bro&wse" then Alt+W becomes an accelerator which will
  move the focus to this tab.

  If you call addTab() after show() the screen will flicker and the
  user may be confused.

  \sa insertTab()
*/

void Q3TabDialog::addTab(QWidget * child, const QString &label)
{
    d->tw->addTab(child, label);
}



/*! \overload

  This version of the function shows the \a iconset as well as the \a
  label on the tab of \a child.
*/
void Q3TabDialog::addTab(QWidget *child, const QIcon& iconset, const QString &label)
{
    d->tw->addTab(child, iconset, label);
}


/*!
  Inserts another tab and page to the tab view.

  The new page is \a child; the tab's label is \a label.
  Note the difference between the widget name (which you supply to
  widget constructors and to setTabEnabled(), for example) and the tab
  label. The name is internal to the program and invariant, whereas
  the label is shown on-screen and may vary according to language and
  other factors.

  If the tab's \a label contains an ampersand, the letter following
  the ampersand is used as an accelerator for the tab, e.g. if the
  label is "Bro&wse" then Alt+W becomes an accelerator which will
  move the focus to this tab.

  If \a index is not specified, the tab is simply added. Otherwise
  it is inserted at the specified position.

  If you call insertTab() after show(), the screen will flicker and the
  user may be confused.

  \sa addTab()
*/

void Q3TabDialog::insertTab(QWidget * child, const QString &label, int index)
{
    d->tw->insertTab(child, label, index);
}


/*! \overload

  This version of the function shows the \a iconset as well as the \a
  label on the tab of \a child.
 */
void Q3TabDialog::insertTab(QWidget *child, const QIcon& iconset, const QString &label, int index)
{
    d->tw->insertTab(child, iconset, label, index);
}

/*!
  Replaces the QTabBar heading the dialog by the given tab bar, \a tb.
  Note that this must be called \e before any tabs have been added,
  or the behavior is undefined.
  \sa tabBar()
*/
void Q3TabDialog::setTabBar(QTabBar* tb)
{
    if (tb == 0){
        qWarning("Q3TabDialog::setTabBar() called with null QTabBar pointer");
        return;
    }
    
    d->tw->setTabBar(tb);
    setUpLayout();
}

/*!
  Returns the currently set QTabBar.
  \sa setTabBar()
*/
QTabBar* Q3TabDialog::tabBar() const
{
    return d->tw->tabBar();
}

/*!  Ensures that widget \a w is shown. This is mainly useful for accelerators.

  \warning If used carelessly, this function can easily surprise or
  confuse the user.

  \sa QTabBar::setCurrentTab()
*/

void Q3TabDialog::showPage(QWidget * w)
{
    d->tw->showPage(w);
}


/*! \obsolete
  Returns true if the page with object name \a name is enabled and
  false if it is disabled.

  If \a name is 0 or not the name of any of the pages, isTabEnabled()
  returns false.

  \sa setTabEnabled(), QWidget::isEnabled()
*/

bool Q3TabDialog::isTabEnabled(const char* name) const
{
    if (!name)
	return false;
    QObjectList l = this->queryList("QWidget", name, false, true);
    if (!l.isEmpty()) {
        for (int i = 0; i < l.size(); ++i) {
            QObject *o = l.at(i);
            if (!o->isWidgetType())
                continue;
            QWidget *w = static_cast<QWidget *>(o);
            return d->tw->isTabEnabled(w);
        }
    }
    return false;
}


/*!\obsolete

  Finds the page with object name \a name, enables/disables it
  according to the value of \a enable and redraws the page's tab
  appropriately.

  Q3TabDialog uses QWidget::setEnabled() internally, rather than keeping a
  separate flag.

  Note that even a disabled tab/page may be visible. If the page is
  already visible Q3TabDialog will not hide it; if all the pages
  are disabled Q3TabDialog will show one of them.

  The object name is used (rather than the tab label) because the tab
  text may not be invariant in multi-language applications.

  \sa isTabEnabled(), QWidget::setEnabled()
*/

void Q3TabDialog::setTabEnabled(const char* name, bool enable)
{
    if (!name)
	return;
    QObjectList l = this->queryList("QWidget", name, false, true);
    if (!l.isEmpty()) {
        for (int i = 0; i < l.size(); ++i) {
            QObject *o = l.at(i);
            if(o->isWidgetType())
                d->tw->setTabEnabled(static_cast<QWidget*>(o), enable);
        }
    }
}


/* ### SHOULD THIS BE HERE?
  Adds an Apply button to the dialog. The button's text is set to \e
  text (and defaults to "Apply").

  The Apply button should apply the current settings in the dialog box
  to the application, while keeping the dialog visible.

  When Apply is clicked, the applyButtonPressed() signal is emitted.

  If \a text is an empty string, no button is shown.

  \sa setCancelButton() setDefaultButton() applyButtonPressed()
*/


/*!
  Returns true if the page \a w is enabled; otherwise returns false.

  \sa setTabEnabled(), QWidget::isEnabled()
*/

bool Q3TabDialog::isTabEnabled(QWidget* w) const
{
    return d->tw->isTabEnabled(w);
}

/*!
  If \a enable is true the page \a w is enabled; otherwise \a w is
  disabled. The page's tab is redrawn appropriately.

  QTabWidget uses QWidget::setEnabled() internally, rather than keeping a
  separate flag.

  Note that even a disabled tab and tab page may be visible. If the
  page is already visible QTabWidget will not hide it; if all the
  pages are disabled QTabWidget will show one of them.

  \sa isTabEnabled(), QWidget::setEnabled()
*/

void Q3TabDialog::setTabEnabled(QWidget* w, bool enable)
{
    d->tw->setTabEnabled(w, enable);
}


/*!
  Adds an Apply button to the dialog. The button's text is set to \a
  text.

  The Apply button should apply the current settings in the dialog box
  to the application while keeping the dialog visible.

  When Apply is clicked, the applyButtonPressed() signal is emitted.

  If \a text is an empty string, no button is shown.

  \sa setCancelButton() setDefaultButton() applyButtonPressed()
*/
void Q3TabDialog::setApplyButton(const QString &text)
{
    if (text.isEmpty() && d->ab) {
	delete d->ab;
	d->ab = 0;
	setSizes();
    } else {
	if (!d->ab) {
	    d->ab = new QPushButton(this, "apply settings");
	    connect(d->ab, SIGNAL(clicked()),
		     this, SIGNAL(applyButtonPressed()));
	    setUpLayout();
	}
	d->ab->setText(text);
	setSizes();
	//d->ab->show();
    }
}

/*!
    \overload

  Adds an Apply button to the dialog. The button's text is set to
  a localizable "Apply".
 */
void Q3TabDialog::setApplyButton()
{
    setApplyButton(tr("Apply"));
}


/*!
  Adds a Help button to the dialog. The button's text is set to \a
  text.

  When Help is clicked, the helpButtonPressed() signal is emitted.

  If \a text is an empty string, no button is shown.

  \sa setApplyButton() setCancelButton() helpButtonPressed()
*/

void Q3TabDialog::setHelpButton(const QString &text)
{
    if (text.isEmpty()) {
	delete d->hb;
	d->hb = 0;
	setSizes();
    } else {
	if (!d->hb) {
	    d->hb = new QPushButton(this, "give help");
	    connect(d->hb, SIGNAL(clicked()),
		     this, SIGNAL(helpButtonPressed()));
	    setUpLayout();
	}
	d->hb->setText(text);
	setSizes();
	//d->hb->show();
    }
}


/*!
    \overload

  Adds a Help button to the dialog. The button's text is set to
  a localizable "Help".
 */
void Q3TabDialog::setHelpButton()
{
    setHelpButton(tr("Help"));
}

/*!
  Adds a Defaults button to the dialog. The button's text is set to \a
  text.

  The Defaults button should set the dialog (but not the application)
  back to the application defaults.

  When Defaults is clicked, the defaultButtonPressed() signal is emitted.

  If \a text is an empty string, no button is shown.

  \sa setApplyButton() setCancelButton() defaultButtonPressed()
*/

void Q3TabDialog::setDefaultButton(const QString &text)
{
    if (text.isEmpty()) {
	delete d->db;
	d->db = 0;
	setSizes();
    } else {
	if (!d->db) {
	    d->db = new QPushButton(this, "back to default");
	    connect(d->db, SIGNAL(clicked()),
		     this, SIGNAL(defaultButtonPressed()));
	    setUpLayout();
	}
	d->db->setText(text);
	setSizes();
	//d->db->show();
    }
}


/*!
    \overload

  Adds a Defaults button to the dialog. The button's text is set to
  a localizable "Defaults".
 */
void Q3TabDialog::setDefaultButton()
{
    setDefaultButton(tr("Defaults"));
}

/*!
  Adds a Cancel button to the dialog. The button's text is set to \a
  text.

  The cancel button should always return the application to the state
  it was in before the tab view popped up, or if the user has clicked
  Apply, back to the state immediately after the last Apply.

  When Cancel is clicked, the cancelButtonPressed() signal is emitted.
  The dialog is closed at the same time.

  If \a text is an empty string, no button is shown.

  \sa setApplyButton() setDefaultButton() cancelButtonPressed()
*/

void Q3TabDialog::setCancelButton(const QString &text)
{
    if (text.isEmpty()) {
	delete d->cb;
	d->cb = 0;
	setSizes();
    } else {
	if (!d->cb) {
	    d->cb = new QPushButton(this, "cancel dialog");
	    connect(d->cb, SIGNAL(clicked()),
		     this, SIGNAL(cancelButtonPressed()));
	    connect(d->cb, SIGNAL(clicked()),
		     this, SLOT(reject()));
	    setUpLayout();
	}
	d->cb->setText(text);
	setSizes();
	//d->cb->show();
    }
}


/*!
    \overload

  Adds a Cancel button to the dialog. The button's text is set to
  a localizable "Cancel".
 */

void Q3TabDialog::setCancelButton()
{
    setCancelButton(tr("Cancel"));
}

/*!  Sets up the layout manager for the tab dialog.

  \sa setSizes() setApplyButton() setCancelButton() setDefaultButton()
*/

void Q3TabDialog::setUpLayout()
{
    // the next four are probably the same, really?
    const int topMargin = 6;
    const int leftMargin = 6;
    const int rightMargin = 6;
    const int bottomMargin = 6;
    const int betweenButtonsMargin = 7;
    const int aboveButtonsMargin = 8;

    delete d->tll;
    d->tll = new QBoxLayout(this, QBoxLayout::Down);

    // top margin
    d->tll->addSpacing(topMargin);

    QBoxLayout * tmp = new QHBoxLayout();
    d->tll->addLayout(tmp, 1);
    tmp->addSpacing(leftMargin);
    tmp->addWidget(d->tw, 1);
    tmp->addSpacing(rightMargin + 2);

    d->tll->addSpacing(aboveButtonsMargin + 2);
    QBoxLayout * buttonRow = new QBoxLayout(QBoxLayout::RightToLeft);
    d->tll->addLayout(buttonRow, 0);
    d->tll->addSpacing(bottomMargin);

    buttonRow->addSpacing(rightMargin);
    if (d->cb) {
	buttonRow->addWidget(d->cb, 0);
	buttonRow->addSpacing(betweenButtonsMargin);
	d->cb->raise();
    }

    if (d->ab) {
	buttonRow->addWidget(d->ab, 0);
	buttonRow->addSpacing(betweenButtonsMargin);
	d->ab->raise();
    }

    if (d->db) {
	buttonRow->addWidget(d->db, 0);
	buttonRow->addSpacing(betweenButtonsMargin);
	d->db->raise();
    }

    if (d->hb) {
	buttonRow->addWidget(d->hb, 0);
	buttonRow->addSpacing(betweenButtonsMargin);
	d->hb->raise();
    }

    if (d->ok) {
	buttonRow->addWidget(d->ok, 0);
	buttonRow->addSpacing(betweenButtonsMargin);
	d->ok->raise();
    }

    // add one custom widget here
    buttonRow->addStretch(1);
    // add another custom widget here

    d->tll->activate();
}


/*!  Sets up the minimum and maximum sizes for each child widget.

  \sa setUpLayout() setFont()
*/

void Q3TabDialog::setSizes()
{
    // compute largest button size
    QSize s(0, 0);
    int bw = s.width();
    int bh = s.height();

    if (d->ok) {
	s = d->ok->sizeHint();
	if (s.width() > bw)
	    bw = s.width();
	if (s.height() > bh)
	    bh = s.height();
    }

    if (d->ab) {
	s = d->ab->sizeHint();
	if (s.width() > bw)
	    bw = s.width();
	if (s.height() > bh)
	    bh = s.height();
    }

    if (d->db) {
	s = d->db->sizeHint();
	if (s.width() > bw)
	    bw = s.width();
	if (s.height() > bh)
	    bh = s.height();
    }

    if (d->hb) {
	s = d->hb->sizeHint();
	if (s.width() > bw)
	    bw = s.width();
	if (s.height() > bh)
	    bh = s.height();
    }

    if (d->cb) {
	s = d->cb->sizeHint();
	if (s.width() > bw)
	    bw = s.width();
	if (s.height() > bh)
	    bh = s.height();
    }

    // and set all the buttons to that size
    if (d->ok)
	d->ok->setFixedSize(bw, bh);
    if (d->ab)
	d->ab->setFixedSize(bw, bh);
    if (d->db)
	d->db->setFixedSize(bw, bh);
    if (d->hb)
	d->hb->setFixedSize(bw, bh);
    if (d->cb)
	d->cb->setFixedSize(bw, bh);

    // fiddle the tab chain so the buttons are in their natural order
    QWidget * w = d->ok;

    if (d->hb) {
	if (w)
	    setTabOrder(w, d->hb);
	w = d->hb;
    }
    if (d->db) {
	if (w)
	    setTabOrder(w, d->db);
	w = d->db;
    }
    if (d->ab) {
	if (w)
	    setTabOrder(w, d->ab);
	w = d->ab;
    }
    if (d->cb) {
	if (w)
	    setTabOrder(w, d->cb);
	w = d->cb;
    }
    setTabOrder(w, d->tw);
}

/*!\reimp
*/
void Q3TabDialog::resizeEvent(QResizeEvent * e)
{
    QDialog::resizeEvent(e);
}


/*!\reimp
*/
void Q3TabDialog::paintEvent(QPaintEvent *)
{
}


/*!
  Adds an OK button to the dialog and sets the button's text to \a text.

  When the OK button is clicked, the applyButtonPressed() signal is emitted,
  and the current settings in the dialog box should be applied to
  the application. The dialog then closes.

  If \a text is an empty string, no button is shown.

  \sa setCancelButton() setDefaultButton() applyButtonPressed()
*/

void Q3TabDialog::setOkButton(const QString &text)
{
    if (text.isEmpty()) {
	delete d->ok;
	d->ok = 0;
	setSizes();
    } else {
	if (!d->ok) {
	    d->ok = new QPushButton(this, "ok");
	    connect(d->ok, SIGNAL(clicked()),
		     this, SIGNAL(applyButtonPressed()));
	    setUpLayout();
	}
	d->ok->setText(text);
	setSizes();
	//d->ok->show();
    }
}
/*!
  \overload

  Adds an OK button to the dialog. The button's text is set to
  a localizable "OK".
 */

void Q3TabDialog::setOkButton()
{
    setOkButton(tr("OK"));
}


/*
    \overload
  Old version of setOkButton(), provided for backward compatibility.
*/
void Q3TabDialog::setOKButton(const QString &text)
{
    // Ugly workaround for original "OK" default argument
    QString newText(text);
    if (text.isNull())
	newText = QString::fromLatin1("OK");
    setOkButton(newText);
}


/*!  Returns the text in the tab for page \a w.
*/

QString Q3TabDialog::tabLabel(QWidget * w)
{
    return d->tw->tabLabel(w);
}


/*!  \reimp
*/
void Q3TabDialog::styleChange(QStyle& s)
{
    QDialog::styleChange(s);
    setSizes();
}


/*!  Returns a pointer to the page currently being displayed by the
tab dialog. The tab dialog does its best to make sure that this value
is never 0 (but if you try hard enough, it can be).
*/

QWidget * Q3TabDialog::currentPage() const
{
    return d->tw->currentPage();
}

/*!
 \overload
  Defines a new \a label for the tab of page \a w
 */
void Q3TabDialog::changeTab(QWidget *w, const QString &label)
{
    d->tw->changeTab(w, label);
}

/*!
    Changes tab page \a w's iconset to \a iconset and label to \a label.

 */
void Q3TabDialog::changeTab(QWidget *w, const QIcon& iconset, const QString &label)
{
    d->tw->changeTab(w, iconset, label);
}

/*! Removes page \a w from this stack of widgets. Does not
  delete \a w.
  \sa showPage(), QTabWidget::removePage()
*/
void Q3TabDialog::removePage(QWidget * w)
{
    d->tw->removePage(w);
}

QT_END_NAMESPACE
