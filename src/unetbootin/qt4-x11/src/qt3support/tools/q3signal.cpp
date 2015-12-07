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

#include "q3signal.h"
#include "qmetaobject.h"
#include "qpointer.h"
#include "q3cstring.h"

QT_BEGIN_NAMESPACE

/*!
    \class Q3Signal q3signal.h
    \brief The Q3Signal class can be used to send signals for classes
    that don't inherit QObject.

    \compat

    If you want to send signals from a class that does not inherit
    QObject, you can create an internal Q3Signal object to emit the
    signal. You must also provide a function that connects the signal
    to an outside object slot.  This is how we used to implement
    signals in Qt 3's QMenuData class, which was not a QObject. In Qt
    4, menus contain actions, which are QObjects.

    In general, we recommend inheriting QObject instead. QObject
    provides much more functionality.

    You can set a single QVariant parameter for the signal with
    setValue().

    Note that QObject is a \e private base class of Q3Signal, i.e. you
    cannot call any QObject member functions from a Q3Signal object.

    Example:
    \snippet doc/src/snippets/code/src_qt3support_tools_q3signal.cpp 0
*/

/*!
    Constructs a signal object called \a name, with the parent object
    \a parent. These arguments are passed directly to QObject.
*/

Q3Signal::Q3Signal(QObject *parent, const char *name)
    : QObject(parent, name)
{
#ifndef QT_NO_VARIANT
    val = 0;
#endif
}

/*!
    Destroys the signal. All connections are removed, as is the case
    with all QObjects.
*/
Q3Signal::~Q3Signal()
{
}
#ifndef QT_NO_VARIANT
// Returns true if it matches ".+(.*int.*"
static inline bool intSignature(const char *member)
{
    Q3CString s(member);
    int p = s.find('(');
    return p > 0 && p < s.findRev("int");
}
#endif
/*!
    Connects the signal to \a member in object \a receiver.
    Returns true if the connection is successful.

    \sa disconnect(), QObject::connect()
*/

bool Q3Signal::connect(const QObject *receiver, const char *member)
{
#ifndef QT_NO_VARIANT
    if (intSignature(member))
#endif
	return QObject::connect((QObject *)this, SIGNAL(intSignal(int)), receiver, member);
#ifndef QT_NO_VARIANT
    return QObject::connect((QObject *)this, SIGNAL(signal(QVariant)),
			     receiver, member);
#endif
}

/*!
    Disonnects the signal from \a member in object \a receiver.
    Returns true if the connection existed and the disconnect
    was successful.

    \sa connect(), QObject::disconnect()
*/

bool Q3Signal::disconnect(const QObject *receiver, const char *member)
{
    if (!member)
	return QObject::disconnect((QObject *)this, 0, receiver, member);
#ifndef QT_NO_VARIANT
    if (intSignature(member))
#endif
	return QObject::disconnect((QObject *)this, SIGNAL(intSignal(int)), receiver, member);
#ifndef QT_NO_VARIANT
    return QObject::disconnect((QObject *)this, SIGNAL(signal(QVariant)),
				receiver, member);
#endif
}


/*!
  \fn bool Q3Signal::isBlocked() const
  \obsolete
  Returns true if the signal is blocked, or false if it is not blocked.

  The signal is not blocked by default.

  \sa block(), QObject::signalsBlocked()
*/

/*!
  \fn void Q3Signal::block(bool b)
  \obsolete
  Blocks the signal if \a b is true, or unblocks the signal if \a b is false.

  An activated signal disappears into hyperspace if it is blocked.

  \sa isBlocked(), activate(), QObject::blockSignals()
*/


/*!
    \fn void Q3Signal::activate()

    Emits the signal. If the platform supports QVariant and a
    parameter has been set with setValue(), this value is passed in
    the signal.
*/
void  Q3Signal::activate()
{
#ifndef QT_NO_VARIANT
    /* Create this Q3GuardedPtr on this, if we get destroyed after the intSignal (but before the variant signal)
       we cannot just emit the signal (because val has been destroyed already) */
    QPointer<Q3Signal> me = this;
    if(me)
	emit intSignal(val.toInt());
    if(me)
	emit signal(val);
#else
    emit intSignal(0);
#endif
}

#ifndef QT_NO_VARIANT
/*!
    Sets the signal's parameter to \a value
*/
void Q3Signal::setValue(const QVariant &value)
{
    val = value;
}

/*!
    Returns the signal's parameter
*/
QVariant Q3Signal::value() const
{
    return val;
}
/*! \fn void Q3Signal::signal(const QVariant &)
    \internal
*/
/*! \fn void Q3Signal::intSignal(int)
    \internal
*/

/*! \obsolete */
void Q3Signal::setParameter(int value)
{
    val = value;
}

/*! \obsolete */
int Q3Signal::parameter() const
{
    return val.toInt();
}
#endif //QT_NO_VARIANT

QT_END_NAMESPACE
