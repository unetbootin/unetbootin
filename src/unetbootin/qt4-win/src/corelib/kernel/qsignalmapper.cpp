/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License versions 2.0 or 3.0 as published by the Free Software
** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file.  Alternatively you may (at
** your option) use any later version of the GNU General Public
** License if such license has been publicly approved by Trolltech ASA
** (or its successors, if any) and the KDE Free Qt Foundation. In
** addition, as a special exception, Trolltech gives you certain
** additional rights. These rights are described in the Trolltech GPL
** Exception version 1.2, which can be found at
** http://www.trolltech.com/products/qt/gplexception/ and in the file
** GPL_EXCEPTION.txt in this package.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/. If
** you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** In addition, as a special exception, Trolltech, as the sole
** copyright holder for Qt Designer, grants users of the Qt/Eclipse
** Integration plug-in the right for the Qt/Eclipse Integration to
** link to functionality provided by Qt Designer and its related
** libraries.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not expressly
** granted herein.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "qsignalmapper.h"
#ifndef QT_NO_SIGNALMAPPER
#include "qhash.h"
#include "qobject_p.h"

QT_BEGIN_NAMESPACE

class QSignalMapperPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QSignalMapper)
public:
    void _q_senderDestroyed() {
        Q_Q(QSignalMapper);
        q->removeMappings(q->sender());
    }
    QHash<QObject *, int> intHash;
    QHash<QObject *, QString> stringHash;
    QHash<QObject *, QWidget*> widgetHash;
    QHash<QObject *, QObject*> objectHash;

};


/*!
    \class QSignalMapper
    \brief The QSignalMapper class bundles signals from identifiable senders.

    \ingroup io
    \mainclass

    This class collects a set of parameterless signals, and re-emits
    them with integer, string or widget parameters corresponding to
    the object that sent the signal.

    The class supports the mapping of particular strings or integers
    with particular objects using setMapping(). The objects' signals
    can then be connected to the map() slot which will emit the
    mapped() signal with the string or integer associated with the
    original signalling object. Mappings can be removed later using
    removeMappings().

    Example: Suppose we want to create a custom widget that contains
    a group of buttons (like a tool palette). One approach is to
    connect each button's \c clicked() signal to its own custom slot;
    but in this example we want to connect all the buttons to a
    single slot and parameterize the slot by the button that was
    clicked.

    Here's the definition of a simple custom widget that has a single
    signal, \c clicked(), which is emitted with the text of the button
    that was clicked:

    \snippet doc/src/snippets/qsignalmapper/buttonwidget.h 0
    \snippet doc/src/snippets/qsignalmapper/buttonwidget.h 1

    The only function that we need to implement is the constructor:

    \snippet doc/src/snippets/qsignalmapper/buttonwidget.cpp 0
    \snippet doc/src/snippets/qsignalmapper/buttonwidget.cpp 1
    \snippet doc/src/snippets/qsignalmapper/buttonwidget.cpp 2

    A list of texts is passed to the constructor. A signal mapper is
    constructed and for each text in the list a QPushButton is
    created. We connect each button's \c clicked() signal to the
    signal mapper's map() slot, and create a mapping in the signal
    mapper from each button to the button's text. Finally we connect
    the signal mapper's mapped() signal to the custom widget's \c
    clicked() signal. When the user clicks a button, the custom
    widget will emit a single \c clicked() signal whose argument is
    the text of the button the user clicked.

    \sa QObject, QButtonGroup, QActionGroup
*/

/*!
    Constructs a QSignalMapper with parent \a parent.
*/
QSignalMapper::QSignalMapper(QObject* parent)
    : QObject(*new QSignalMapperPrivate, parent)
{
}

#ifdef QT3_SUPPORT
/*!
    \overload
    \obsolete
 */
QSignalMapper::QSignalMapper(QObject *parent, const char *name)
    : QObject(*new QSignalMapperPrivate, parent)
{
    setObjectName(QString::fromAscii(name));
}
#endif

/*!
    Destroys the QSignalMapper.
*/
QSignalMapper::~QSignalMapper()
{
}

/*!
    Adds a mapping so that when map() is signalled from the given \a
    sender, the signal mapped(\a id) is emitted.

    There may be at most one integer ID for each sender.

    \sa mapping()
*/
void QSignalMapper::setMapping(QObject *sender, int id)
{
    Q_D(QSignalMapper);
    d->intHash.insert(sender, id);
    connect(sender, SIGNAL(destroyed()), this, SLOT(_q_senderDestroyed()));
}

/*!
    \overload

    Adds a mapping so that when map() is signalled from the given \a
    sender, the signal mapped(\a text ) is emitted.

    There may be at most one text for each sender.
*/
void QSignalMapper::setMapping(QObject *sender, const QString &text)
{
    Q_D(QSignalMapper);
    d->stringHash.insert(sender, text);
    connect(sender, SIGNAL(destroyed()), this, SLOT(_q_senderDestroyed()));
}

/*!
    \overload

    Adds a mapping so that when map() is signalled from the given \a
    sender, the signal mapped(\a widget ) is emitted.

    There may be at most one widget for each sender.
*/
void QSignalMapper::setMapping(QObject *sender, QWidget *widget)
{
    Q_D(QSignalMapper);
    d->widgetHash.insert(sender, widget);
    connect(sender, SIGNAL(destroyed()), this, SLOT(_q_senderDestroyed()));
}

/*!
    \overload

    Adds a mapping so that when map() is signalled from the given \a
    sender, the signal mapped(\a object ) is emitted.

    There may be at most one object for each sender.
*/
void QSignalMapper::setMapping(QObject *sender, QObject *object)
{
    Q_D(QSignalMapper);
    d->objectHash.insert(sender, object);
    connect(sender, SIGNAL(destroyed()), this, SLOT(_q_senderDestroyed()));
}

/*!
    Returns the sender QObject that is associated with the given \a
    id.

    \sa setMapping()
*/
QObject *QSignalMapper::mapping(int id) const
{
    Q_D(const QSignalMapper);
    return d->intHash.key(id);
}

/*!
    \overload
*/
QObject *QSignalMapper::mapping(const QString &id) const
{
    Q_D(const QSignalMapper);
    return d->stringHash.key(id);
}

/*!
    \overload

    Returns the sender QObject that is associated with the given \a
    widget.
*/
QObject *QSignalMapper::mapping(QWidget *widget) const
{
    Q_D(const QSignalMapper);
    return d->widgetHash.key(widget);
}

/*!
    \overload

    Returns the sender QObject that is associated with the given \a
    object.
*/
QObject *QSignalMapper::mapping(QObject *object) const
{
    Q_D(const QSignalMapper);
    return d->objectHash.key(object);
}

/*!
    Removes all mappings for \a sender.

    This is done automatically when mapped objects are destroyed.
*/
void QSignalMapper::removeMappings(QObject *sender)
{
    Q_D(QSignalMapper);

    d->intHash.remove(sender);
    d->stringHash.remove(sender);
    d->widgetHash.remove(sender);
    d->objectHash.remove(sender);
}

/*!
    This slot emits signals based on which object sends signals to it.
*/
void QSignalMapper::map() { map(sender()); }

/*!
    This slot emits signals based on the \a sender object.
*/
void QSignalMapper::map(QObject *sender)
{
    Q_D(QSignalMapper);
    if (d->intHash.contains(sender))
        emit mapped(d->intHash.value(sender));
    if (d->stringHash.contains(sender))
        emit mapped(d->stringHash.value(sender));
    if (d->widgetHash.contains(sender))
        emit mapped(d->widgetHash.value(sender));
    if (d->objectHash.contains(sender))
        emit mapped(d->objectHash.value(sender));
}


/*!
    \fn void QSignalMapper::mapped(int i)

    This signal is emitted when map() is signalled from an object that
    has an integer mapping set. The object's mapped integer is passed
    in \a i.

    \sa setMapping()
*/

/*!
    \fn void QSignalMapper::mapped(const QString &text)
    \overload

    This signal is emitted when map() is signalled from an object that
    has a string mapping set. The object's mapped string is passed in
    \a text.

    \sa setMapping()
*/

/*!
    \fn void QSignalMapper::mapped(QWidget *widget)
    \overload

    This signal is emitted when map() is signalled from an object that
    has a widget mapping set. The object's mapped widget is passed in
    \a widget.

    \sa setMapping()
*/

/*!
    \fn void QSignalMapper::mapped(QObject *object)
    \overload

    This signal is emitted when map() is signalled from an object that
    has an object mapping set. The object provided by the map is passed in
    \a object.

    \sa setMapping()
*/

QT_END_NAMESPACE

#include "moc_qsignalmapper.cpp"

#endif // QT_NO_SIGNALMAPPER

