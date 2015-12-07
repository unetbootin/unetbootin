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

#include "q3accel.h"

#include "q3signal.h"
#include "qapplication.h"
#include "qwidget.h"
#include "q3ptrlist.h"
#include "qwhatsthis.h"
#include "qpointer.h"
#include "qstatusbar.h"
#include "qdockwidget.h"
#include "qevent.h"
#include "qkeysequence.h"
#include "private/qapplication_p.h"

QT_BEGIN_NAMESPACE

using namespace Qt;

/*!
    \class Q3Accel
    \brief The Q3Accel class handles keyboard accelerator and shortcut keys.

    \compat

    A keyboard accelerator triggers an action when a certain key
    combination is pressed. The accelerator handles all keyboard
    activity for all the children of one top-level widget, so it is
    not affected by the keyboard focus.

    In most cases, you will not need to use this class directly. Use
    the QAction class to create actions with accelerators that can be
    used in both menus and toolbars. If you're only interested in
    menus use Q3MenuData::insertItem() or Q3MenuData::setAccel() to make
    accelerators for operations that are also available on menus. Many
    widgets automatically generate accelerators, such as QAbstractButton,
    QGroupBox, QLabel (with QLabel::setBuddy()), QMenuBar, and QTabBar.
    Example:
    \snippet doc/src/snippets/code/src_qt3support_other_q3accel.cpp 0

    A Q3Accel contains a list of accelerator items that can be
    manipulated using insertItem(), removeItem(), clear(), key() and
    findKey().

    Each accelerator item consists of an identifier and a \l
    QKeySequence. A single key sequence consists of a keyboard code
    combined with modifiers (Qt::SHIFT, Qt::CTRL, Qt::ALT, or
    Qt::UNICODE_ACCEL). For example, Qt::CTRL + Qt::Key_P could be a shortcut
    for printing a document. As an alternative, use Qt::UNICODE_ACCEL with the
    unicode code point of the character. For example, Qt::UNICODE_ACCEL
    + 'A' gives the same accelerator as Qt::Key_A.

    When an accelerator key is pressed, the accelerator sends out the
    signal activated() with a number that identifies this particular
    accelerator item. Accelerator items can also be individually
    connected, so that two different keys will activate two different
    slots (see connectItem() and disconnectItem()).

    The activated() signal is \e not emitted when two or more
    accelerators match the same key.  Instead, the first matching
    accelerator sends out the activatedAmbiguously() signal. By
    pressing the key multiple times, users can navigate between all
    matching accelerators. Some standard controls like QPushButton and
    QCheckBox connect the activatedAmbiguously() signal to the
    harmless setFocus() slot, whereas activated() is connected to a
    slot invoking the button's action. Most controls, like QLabel and
    QTabBar, treat activated() and activatedAmbiguously() as
    equivalent.

    Use setEnabled() to enable or disable all the items in an
    accelerator, or setItemEnabled() to enable or disable individual
    items. An item is active only when both the Q3Accel and the item
    itself are enabled.

    The function setWhatsThis() specifies a help text that appears
    when the user presses an accelerator key in What's This mode.

    The accelerator will be deleted when \e parent is deleted,
    and will consume relevant key events until then.

    Please note that the accelerator
    \snippet doc/src/snippets/code/src_qt3support_other_q3accel.cpp 1
    can be triggered with both the 'M' key, and with Shift+M,
    unless a second accelerator is defined for the Shift+M
    combination.


    Example:
    \snippet doc/src/snippets/code/src_qt3support_other_q3accel.cpp 2

    \sa QKeyEvent QWidget::keyPressEvent()
    QAbstractButton::setAccel() QLabel::setBuddy() QKeySequence
*/


struct Q3AccelItem {                                // internal accelerator item
    Q3AccelItem(const QKeySequence &k, int i)
        { key=k; id=i; enabled=true; signal=0; }
   ~Q3AccelItem()               { delete signal; }
    int id;
    QKeySequence key;
    bool enabled;
    Q3Signal *signal;
    QString whatsthis;
};


typedef Q3PtrList<Q3AccelItem> Q3AccelList; // internal accelerator list

class Q3AccelPrivate {
public:
    Q3AccelPrivate(Q3Accel* p);
    ~Q3AccelPrivate();
    Q3AccelList aitems;
    bool enabled;
    QPointer<QWidget> watch;
    bool ignorewhatsthis;
    Q3Accel* parent;

    void activate(Q3AccelItem* item);
    void activateAmbiguously(Q3AccelItem* item);
};

class Q3AccelManager {
public:
    static Q3AccelManager* self() { return self_ptr ? self_ptr : new Q3AccelManager; }
    void registerAccel(Q3AccelPrivate* a) { accels.append(a); }
    void unregisterAccel(Q3AccelPrivate* a) { accels.removeRef(a); if (accels.isEmpty()) delete this; }
    bool tryAccelEvent(QWidget* w, QKeyEvent* e);
    bool dispatchAccelEvent(QWidget* w, QKeyEvent* e);
    bool tryComposeUnicode(QWidget* w, QKeyEvent* e);

private:
    Q3AccelManager()
        : currentState(QKeySequence::NoMatch), clash(-1), metaComposeUnicode(false),composedUnicode(0)
        { setFuncPtr(); self_ptr = this; }
    ~Q3AccelManager() { self_ptr = 0; }
    void setFuncPtr();

    bool correctSubWindow(QWidget *w, Q3AccelPrivate* d);
    QKeySequence::SequenceMatch match(QKeyEvent* e, Q3AccelItem* item, QKeySequence& temp);
    int translateModifiers(ButtonState state);

    Q3PtrList<Q3AccelPrivate> accels;
    static Q3AccelManager* self_ptr;
    QKeySequence::SequenceMatch currentState;
    QKeySequence intermediate;
    int clash;
    bool metaComposeUnicode;
    int composedUnicode;
};
Q3AccelManager* Q3AccelManager::self_ptr = 0;

bool Q_COMPAT_EXPORT qt_tryAccelEvent(QWidget* w, QKeyEvent*  e){
    return Q3AccelManager::self()->tryAccelEvent(w, e);
}

bool Q_COMPAT_EXPORT qt_dispatchAccelEvent(QWidget* w, QKeyEvent*  e){
    return Q3AccelManager::self()->dispatchAccelEvent(w, e);
}

bool Q_COMPAT_EXPORT qt_tryComposeUnicode(QWidget* w, QKeyEvent*  e){
    return Q3AccelManager::self()->tryComposeUnicode(w, e);
}

void Q3AccelManager::setFuncPtr() {
    if (qApp->d_func()->qt_compat_used)
        return;
    QApplicationPrivate *data = static_cast<QApplicationPrivate*>(qApp->d_ptr);
    data->qt_tryAccelEvent = qt_tryAccelEvent;
    data->qt_tryComposeUnicode = qt_tryComposeUnicode;
    data->qt_dispatchAccelEvent = qt_dispatchAccelEvent;
    data->qt_compat_used = true;
}

#ifdef Q_WS_MAC
static bool qt_accel_no_shortcuts = true;
#else
static bool qt_accel_no_shortcuts = false;
#endif
void Q_COMPAT_EXPORT qt_set_accel_auto_shortcuts(bool b) { qt_accel_no_shortcuts = b; }

/*
    \internal
    Returns true if the accel is in the current subwindow, else false.
*/
bool Q3AccelManager::correctSubWindow(QWidget* w, Q3AccelPrivate* d) {
#if !defined (Q_OS_MACX)
     if (!d->watch || !d->watch->isVisible() || !d->watch->isEnabled())
#else
    if (!d->watch || (!d->watch->isVisible() && !d->watch->inherits("QMenuBar")) || !d->watch->isEnabled())
#endif
        return false;
    QWidget* tlw = w->window();
    QWidget* wtlw = d->watch->window();

    /* if we live in a floating dock window, keep our parent's
     * accelerators working */
#ifndef QT_NO_MAINWINDOW
    if ((tlw->windowType() == Qt::Dialog) && tlw->parentWidget() && qobject_cast<QDockWidget*>(tlw))
        return tlw->parentWidget()->window() == wtlw;

    if (wtlw  != tlw)
        return false;
#endif
    /* if we live in a MDI subwindow, ignore the event if we are
       not the active document window */
    QWidget* sw = d->watch;
    while (sw && sw->windowType() != Qt::SubWindow)
        sw = sw->parentWidget(true);
    if (sw)  { // we are in a subwindow indeed
        QWidget* fw = w;
        while (fw && fw != sw)
            fw = fw->parentWidget(true);
        if (fw != sw) // focus widget not in our subwindow
            return false;
    }
    return true;
}

inline int Q3AccelManager::translateModifiers(ButtonState state)
{
    int result = 0;
    if (state & ShiftButton)
        result |= SHIFT;
    if (state & ControlButton)
        result |= CTRL;
    if (state & MetaButton)
        result |= META;
    if (state & AltButton)
        result |= ALT;
    return result;
}

/*
    \internal
    Matches the current intermediate key sequence + the latest
    keyevent, with and AccelItem. Returns Identical,
    PartialMatch or NoMatch, and fills \a temp with the
    resulting key sequence.
*/
QKeySequence::SequenceMatch Q3AccelManager::match(QKeyEvent *e, Q3AccelItem* item, QKeySequence& temp)
{
    QKeySequence::SequenceMatch result = QKeySequence::NoMatch;
    int index = intermediate.count();
    temp = intermediate;

    int modifier = translateModifiers(e->state());

    if (e->key() && e->key() != Key_unknown) {
        int key = e->key()  | modifier;
        if (e->key() == Key_BackTab) {
            /*
            In QApplication, we map shift+tab to shift+backtab.
            This code here reverts the mapping in a way that keeps
            backtab and shift+tab accelerators working, in that
            order, meaning backtab has priority.*/
            key &= ~SHIFT;

            temp.setKey(key, index);
            if (QKeySequence::NoMatch != (result = temp.matches(item->key)))
                return result;
            if (e->state() & ShiftButton)
                key |= SHIFT;
            key = Key_Tab | (key & MODIFIER_MASK);
            temp.setKey(key, index);
            if (QKeySequence::NoMatch != (result = temp.matches(item->key)))
                return result;
        } else {
            temp.setKey(key, index);
            if (QKeySequence::NoMatch != (result = temp.matches(item->key)))
                return result;
        }

        if (key == Key_BackTab) {
            if (e->state() & ShiftButton)
                key |= SHIFT;
            temp.setKey(key, index);
            if (QKeySequence::NoMatch != (result = temp.matches(item->key)))
                return result;
        }
    }
    if (!e->text().isEmpty()) {
        temp.setKey((int)e->text()[0].unicode() | UNICODE_ACCEL | modifier, index);
        result = temp.matches(item->key);
    }
    return result;
}

bool Q3AccelManager::tryAccelEvent(QWidget* w, QKeyEvent* e)
{
    if (QKeySequence::NoMatch == currentState) {
        e->t = QEvent::AccelOverride;
        e->ignore();
        QApplication::sendSpontaneousEvent(w, e);
        if (e->isAccepted())
            return false;
    }
    e->t = QEvent::Accel;
    e->ignore();
    QApplication::sendSpontaneousEvent(w, e);
    return e->isAccepted();
}

bool Q3AccelManager::tryComposeUnicode(QWidget* w, QKeyEvent* e)
{
    if (metaComposeUnicode) {
        int value = e->key() - Key_0;
        // Ignore acceloverrides so we don't trigger
        // accels on keypad when Meta compose is on
        if ((e->type() == QEvent::AccelOverride) &&
             (e->state() == Qt::Keypad + Qt::MetaButton)) {
            e->accept();
        // Meta compose start/continue
        } else if ((e->type() == QEvent::KeyPress) &&
             (e->state() == Qt::Keypad + Qt::MetaButton)) {
            if (value >= 0 && value <= 9) {
                composedUnicode *= 10;
                composedUnicode += value;
                return true;
            } else {
                // Composing interrupted, dispatch!
                if (composedUnicode) {
                    QChar ch(composedUnicode);
                    QString s(ch);
                    QKeyEvent kep(QEvent::KeyPress, 0, ch.row() ? 0 : ch.cell(), 0, s);
                    QKeyEvent ker(QEvent::KeyRelease, 0, ch.row() ? 0 : ch.cell(), 0, s);
                    QApplication::sendEvent(w, &kep);
                    QApplication::sendEvent(w, &ker);
                }
                composedUnicode = 0;
                return true;
            }
        // Meta compose end, dispatch
        } else if ((e->type() == QEvent::KeyRelease) &&
                    (e->key() == Key_Meta) &&
                    (composedUnicode != 0)) {
            if ((composedUnicode > 0) &&
                 (composedUnicode < 0xFFFE)) {
                QChar ch(composedUnicode);
                QString s(ch);
                QKeyEvent kep(QEvent::KeyPress, 0, ch.row() ? 0 : ch.cell(), 0, s);
                QKeyEvent ker(QEvent::KeyRelease, 0, ch.row() ? 0 : ch.cell(), 0, s);
                QApplication::sendEvent(w, &kep);
                QApplication::sendEvent(w, &ker);
            }
            composedUnicode = 0;
            return true;
        }
    }
    return false;
}

/*
    \internal
    Checks for possible accelerators, if no widget
    ate the keypres, or we are in the middle of a
    partial key sequence.
*/
bool Q3AccelManager::dispatchAccelEvent(QWidget* w, QKeyEvent* e)
{
#ifndef QT_NO_STATUSBAR
    // Needs to be declared and used here because of "goto doclash"
    QStatusBar* mainStatusBar = 0;
#endif

    // Modifiers can NOT be accelerators...
    if (e->key() >= Key_Shift &&
         e->key() <= Key_Alt)
         return false;

    QKeySequence::SequenceMatch result = QKeySequence::NoMatch;
    QKeySequence tocheck, partial;
    Q3AccelPrivate* accel = 0;
    Q3AccelItem* item = 0;
    Q3AccelPrivate* firstaccel = 0;
    Q3AccelItem* firstitem = 0;
    Q3AccelPrivate* lastaccel = 0;
    Q3AccelItem* lastitem = 0;

    QKeyEvent pe = *e;
    int n = -1;
    int hasShift = (e->state()&Qt::ShiftButton)?1:0;
    bool identicalDisabled = false;
    bool matchFound = false;
    do {
        accel = accels.first();
        matchFound = false;
        while (accel) {
            if (correctSubWindow(w, accel)) {
                if (accel->enabled) {
                    item = accel->aitems.last();
                    while(item) {
                        if (QKeySequence::Identical == (result = match(&pe, item, tocheck))) {
                            if (item->enabled) {
                                if (!firstaccel) {
                                    firstaccel = accel;
                                    firstitem = item;
                                }
                                lastaccel = accel;
                                lastitem = item;
                                n++;
                                matchFound = true;
                                if (n > QMAX(clash,0))
                                    goto doclash;
                            } else {
                                identicalDisabled = true;
                            }
                        }
                        if (item->enabled && QKeySequence::PartialMatch == result) {
                            partial = tocheck;
                            matchFound = true;
                        }
                        item = accel->aitems.prev();
                    }
                } else {
                    item = accel->aitems.last();
                    while(item) {
                        if (QKeySequence::Identical == match(&pe, item, tocheck))
                            identicalDisabled = true;
                        item = accel->aitems.prev();
                    }
                }
            }
            accel = accels.next();
        }
        pe = QKeyEvent(QEvent::Accel, pe.key(), pe.ascii(), pe.state()&~Qt::ShiftButton, pe.text());
    } while (hasShift-- && !matchFound && !identicalDisabled);

#ifndef QT_NO_STATUSBAR
    mainStatusBar = (QStatusBar*) w->window()->child(0, "QStatusBar");
#endif
    if (n < 0) { // no match found
        currentState = partial.count() ? QKeySequence::PartialMatch : QKeySequence::NoMatch;
#ifndef QT_NO_STATUSBAR
        // Only display message if we are, or were, in a partial match
        if (mainStatusBar && (QKeySequence::PartialMatch == currentState || intermediate.count())) {
            if (currentState == QKeySequence::PartialMatch) {
                mainStatusBar->showMessage((QString)partial + QLatin1String(", ..."));
            } else if (!identicalDisabled) {
                QString message = Q3Accel::tr("%1, %2 not defined").
                    arg((QString)intermediate).
                    arg(QKeySequence::encodeString(e->key() | translateModifiers(e->state())));
                mainStatusBar->showMessage(message, 2000);
                // Since we're a NoMatch, reset the clash count
                clash = -1;
            } else {
            	mainStatusBar->clearMessage();
            }
        }
#endif

        bool eatKey = (QKeySequence::PartialMatch == currentState || intermediate.count());
        intermediate = partial;
        if (eatKey)
            e->accept();
        return eatKey;
    } else if (n == 0) { // found exactly one match
        clash = -1; // reset
#ifndef QT_NO_STATUSBAR
        if (currentState == QKeySequence::PartialMatch && mainStatusBar)
                mainStatusBar->clearMessage();
#endif
        currentState = QKeySequence::NoMatch; // Free sequence keylock
        intermediate = QKeySequence();
        lastaccel->activate(lastitem);
        e->accept();
        return true;
    }

 doclash: // found more than one match
#ifndef QT_NO_STATUSBAR
    if (!mainStatusBar) // if "goto doclash", we need to get status bar again.
        mainStatusBar = (QStatusBar*) w->window()->child(0, "QStatusBar");
#endif

    QString message = Q3Accel::tr("Ambiguous %1 not handled").arg((QString)tocheck);
    if (clash >= 0 && n > clash) { // pick next  match
        intermediate = QKeySequence();
        currentState = QKeySequence::NoMatch; // Free sequence keylock
        clash++;
#ifndef QT_NO_STATUSBAR
        if (mainStatusBar &&
             !lastitem->signal &&
             !(lastaccel->parent->receivers(SIGNAL(activatedAmbiguously(int)))))
            mainStatusBar->showMessage(message, 2000);
#endif
        lastaccel->activateAmbiguously(lastitem);
    } else { // start (or wrap) with the first matching
        intermediate = QKeySequence();
        currentState = QKeySequence::NoMatch; // Free sequence keylock
        clash = 0;
#ifndef QT_NO_STATUSBAR
        if (mainStatusBar &&
             !firstitem->signal &&
             !(firstaccel->parent->receivers(SIGNAL(activatedAmbiguously(int)))))
            mainStatusBar->showMessage(message, 2000);
#endif
        firstaccel->activateAmbiguously(firstitem);
    }
    e->accept();
    return true;
}

Q3AccelPrivate::Q3AccelPrivate(Q3Accel* p)
    : parent(p)
{
    Q3AccelManager::self()->registerAccel(this);
    aitems.setAutoDelete(true);
    ignorewhatsthis = false;
}

Q3AccelPrivate::~Q3AccelPrivate()
{
    Q3AccelManager::self()->unregisterAccel(this);
}

static Q3AccelItem *find_id(Q3AccelList &list, int id)
{
    register Q3AccelItem *item = list.first();
    while (item && item->id != id)
        item = list.next();
    return item;
}

static Q3AccelItem *find_key(Q3AccelList &list, const QKeySequence &key)
{
    register Q3AccelItem *item = list.first();
    while (item && !(item->key == key))
        item = list.next();
    return item;
}

/*!
    Constructs a Q3Accel object called \a name, with parent \a parent.
    The accelerator operates on \a parent.
*/

Q3Accel::Q3Accel(QWidget *parent, const char *name)
    : QObject(parent, name)
{
    d = new Q3AccelPrivate(this);
    d->enabled = true;
    d->watch = parent;
#if defined(QT_CHECK_NULL)
    if (!d->watch)
        qWarning("Q3Accel: An accelerator must have a parent or a watch widget");
#endif
}

/*!
    Constructs a Q3Accel object called \a name, that operates on \a
    watch, and is a child of \a parent.

    This constructor is not needed for normal application programming.
*/
Q3Accel::Q3Accel(QWidget* watch, QObject *parent, const char *name)
    : QObject(parent, name)
{
    d = new Q3AccelPrivate(this);
    d->enabled = true;
    d->watch = watch;
#if defined(QT_CHECK_NULL)
    if (!d->watch)
        qWarning("Q3Accel: An accelerator must have a parent or a watch widget");
#endif
}

/*!
    Destroys the accelerator object and frees all allocated resources.
*/

Q3Accel::~Q3Accel()
{
    delete d;
}


/*!
    \fn void Q3Accel::activated(int id)

    This signal is emitted when the user types the shortcut's key
    sequence. \a id is a number that identifies this particular
    accelerator item.

    \sa activatedAmbiguously()
*/

/*!
    \fn void Q3Accel::activatedAmbiguously(int id)

    This signal is emitted when the user types a shortcut key
    sequence that is ambiguous. For example, if one key sequence is a
    "prefix" for another and the user types these keys it isn't clear
    if they want the shorter key sequence, or if they're about to
    type more to complete the longer key sequence. \a id is a number
    that identifies this particular accelerator item.

    \sa activated()
*/

/*!
    Returns true if the accelerator is enabled; otherwise returns
    false.

    \sa setEnabled(), isItemEnabled()
*/

bool Q3Accel::isEnabled() const
{
    return d->enabled;
}


/*!
    Enables the accelerator if \a enable is true, or disables it if \a
    enable is false.

    Individual keys can also be enabled or disabled using
    setItemEnabled(). To work, a key must be an enabled item in an
    enabled Q3Accel.

    \sa isEnabled(), setItemEnabled()
*/

void Q3Accel::setEnabled(bool enable)
{
    d->enabled = enable;
}


/*!
    Returns the number of accelerator items in this accelerator.
*/

uint Q3Accel::count() const
{
    return d->aitems.count();
}


static int get_seq_id()
{
    static int seq_no = -2;  // -1 is used as return value in findKey()
    return seq_no--;
}

/*!
    Inserts an accelerator item and returns the item's identifier.

    \a key is a key code and an optional combination of SHIFT, CTRL
    and ALT. \a id is the accelerator item id.

    If \a id is negative, then the item will be assigned a unique
    negative identifier less than -1.

    \snippet doc/src/snippets/code/src_qt3support_other_q3accel.cpp 3
*/

int Q3Accel::insertItem(const QKeySequence& key, int id)
{
    if (id == -1)
        id = get_seq_id();
    d->aitems.insert(0, new Q3AccelItem(key,id));
    return id;
}

/*!
    Removes the accelerator item with the identifier \a id.
*/

void Q3Accel::removeItem(int id)
{
    if (find_id(d->aitems, id))
        d->aitems.remove();
}


/*!
    Removes all accelerator items.
*/

void Q3Accel::clear()
{
    d->aitems.clear();
}


/*!
    Returns the key sequence of the accelerator item with identifier
    \a id, or an invalid key sequence (0) if the id cannot be found.
*/

QKeySequence Q3Accel::key(int id)
{
    Q3AccelItem *item = find_id(d->aitems, id);
    return item ? item->key : QKeySequence(0);
}


/*!
    Returns the identifier of the accelerator item with the key code
    \a key, or -1 if the item cannot be found.
*/

int Q3Accel::findKey(const QKeySequence& key) const
{
    Q3AccelItem *item = find_key(d->aitems, key);
    return item ? item->id : -1;
}


/*!
    Returns true if the accelerator item with the identifier \a id is
    enabled. Returns false if the item is disabled or cannot be found.

    \sa setItemEnabled(), isEnabled()
*/

bool Q3Accel::isItemEnabled(int id) const
{
    Q3AccelItem *item = find_id(d->aitems, id);
    return item ? item->enabled : false;
}


/*!
    Enables the accelerator item with the identifier \a id if \a
    enable is true, and disables item \a id if \a enable is false.

    To work, an item must be enabled and be in an enabled Q3Accel.

    \sa isItemEnabled(), isEnabled()
*/

void Q3Accel::setItemEnabled(int id, bool enable)
{
    Q3AccelItem *item = find_id(d->aitems, id);
    if (item)
        item->enabled = enable;
}


/*!
    Connects the accelerator item \a id to the slot \a member of \a
    receiver. Returns true if the connection is successful.

    \snippet doc/src/snippets/code/src_qt3support_other_q3accel.cpp 4

    Of course, you can also send a signal as \a member.

    Normally accelerators are connected to slots which then receive
    the \c activated(int id) signal with the id of the accelerator
    item that was activated. If you choose to connect a specific
    accelerator item using this function, the \c activated() signal is
    emitted if the associated key sequence is pressed but no \c
    activated(int id) signal is emitted.

    \sa disconnectItem(), QObject::connect()
*/

bool Q3Accel::connectItem(int id, const QObject *receiver, const char *member)
{
    Q3AccelItem *item = find_id(d->aitems, id);
    if (item) {
        if (!item->signal) {
            item->signal = new Q3Signal;
            Q_CHECK_PTR(item->signal);
        }
        return item->signal->connect(receiver, member);
    }
    return false;
}

/*!
    Disconnects the accelerator item identified by \a id from
    the function called \a member in the \a receiver object.
    Returns true if the connection existed and the disconnect
    was successful.

    \sa connectItem(), QObject::disconnect()
*/

bool Q3Accel::disconnectItem(int id, const QObject *receiver,
                             const char *member)
{
    Q3AccelItem *item = find_id(d->aitems, id);
    if (item && item->signal)
        return item->signal->disconnect(receiver, member);
    return false;
}

void Q3AccelPrivate::activate(Q3AccelItem* item)
{
#ifndef QT_NO_WHATSTHIS
    if (QWhatsThis::inWhatsThisMode() && !ignorewhatsthis) {
        QWhatsThis::showText(QCursor::pos(), item->whatsthis);
        return;
    }
#endif
    if (item->signal)
        item->signal->activate();
    else
        emit parent->activated(item->id);
}

void Q3AccelPrivate::activateAmbiguously(Q3AccelItem* item)
{
    if (item->signal)
        item->signal->activate();
    else
        emit parent->activatedAmbiguously(item->id);
}


/*!
    Returns the shortcut key sequence for \a str, or an invalid key
    sequence (0) if \a str has no shortcut sequence.

    For example, shortcutKey("E&xit") returns QKeySequence(Qt::ALT +
    Qt::Key_X), shortcutKey("&Quit") returns QKeySequence(Qt::ALT +
    Qt::Key_Q), and shortcutKey("Quit") returns QKeySequence().
*/

QKeySequence Q3Accel::shortcutKey(const QString &str)
{
    if(qt_accel_no_shortcuts)
        return QKeySequence();

    int p = 0;
    while (p >= 0) {
        p = str.find(QLatin1Char('&'), p) + 1;
        if (p <= 0 || p >= (int)str.length())
            return 0;
        if (str[p] != QLatin1Char('&')) {
            QChar c = str[p];
            if (c.isPrint()) {
                char ltr = c.upper().latin1();
                if (ltr >= (char)Key_A && ltr <= (char)Key_Z)
                    c = QLatin1Char(ltr);
                else
                    c = c.lower();
                return QKeySequence(c.unicode() + ALT + UNICODE_ACCEL);
            }
        }
        p++;
    }
    return QKeySequence();
}

/*! \obsolete

   Creates an accelerator string for the key \a k.
   For instance CTRL+Key_O gives "Ctrl+O". The "Ctrl" etc.
   are translated (using QObject::tr()) in the "Q3Accel" context.

   The function is superfluous. Cast the QKeySequence \a k to a
   QString for the same effect.
*/
QString Q3Accel::keyToString(QKeySequence k)
{
    return (QString) k;
}

/*!\obsolete

  Returns an accelerator code for the string \a s. For example
  "Ctrl+O" gives CTRL+UNICODE_ACCEL+'O'. The strings "Ctrl",
  "Shift", "Alt" are recognized, as well as their translated
  equivalents in the "Q3Accel" context (using QObject::tr()). Returns 0
  if \a s is not recognized.

  This function is typically used with \link QObject::tr() tr
  \endlink(), so that accelerator keys can be replaced in
  translations:

  \snippet doc/src/snippets/code/src_qt3support_other_q3accel.cpp 5

  Notice the "File|Open" translator comment. It is by no means
  necessary, but it provides some context for the human translator.

  The function is superfluous. Construct a QKeySequence from the
  string \a s for the same effect.

  \sa QObject::tr(), {Internationalization with Qt}
*/
QKeySequence Q3Accel::stringToKey(const QString & s)
{
    return QKeySequence(s);
}


/*!
    Sets a What's This help text for the accelerator item \a id to \a
    text.

    The text will be shown when the application is in What's This mode
    and the user hits the accelerator key.

    To set What's This help on a menu item (with or without an
    accelerator key), use Q3MenuData::setWhatsThis().

    \sa whatsThis(), QWhatsThis::inWhatsThisMode(), QAction::setWhatsThis()
*/
void Q3Accel::setWhatsThis(int id, const QString& text)
{
    Q3AccelItem *item = find_id(d->aitems, id);
    if (item)
        item->whatsthis = text;
}

/*!
    Returns the What's This help text for the specified item \a id or
    an empty string if no text has been specified.

    \sa setWhatsThis()
*/
QString Q3Accel::whatsThis(int id) const
{

    Q3AccelItem *item = find_id(d->aitems, id);
    return item? item->whatsthis : QString();
}

/*!\internal */
void Q3Accel::setIgnoreWhatsThis(bool b)
{
    d->ignorewhatsthis = b;
}

/*!\internal */
bool Q3Accel::ignoreWhatsThis() const
{
    return d->ignorewhatsthis;
}

/*!
    \fn void Q3Accel::repairEventFilter()
    \internal
*/

QT_END_NAMESPACE
