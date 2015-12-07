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

#include "qobjectcleanuphandler.h"

QT_BEGIN_NAMESPACE

/*!
    \class QObjectCleanupHandler
    \brief The QObjectCleanupHandler class watches the lifetime of multiple QObjects.

    \ingroup objectmodel

    A QObjectCleanupHandler is useful whenever you need to know when a
    number of \l{QObject}s that are owned by someone else have been
    deleted. This is important, for example, when referencing memory
    in an application that has been allocated in a shared library.

    To keep track of some \l{QObject}s, create a
    QObjectCleanupHandler, and add() the objects you are interested
    in. If you are no longer interested in tracking a particular
    object, use remove() to remove it from the cleanup handler. If an
    object being tracked by the cleanup handler gets deleted by
    someone else it will automatically be removed from the cleanup
    handler. You can delete all the objects in the cleanup handler
    with clear(), or by destroying the cleanup handler. isEmpty()
    returns true if the QObjectCleanupHandler has no objects to keep
    track of.

    \sa QPointer
*/

/*!
    Constructs an empty QObjectCleanupHandler.
*/
QObjectCleanupHandler::QObjectCleanupHandler()
{
}

/*!
    Destroys the cleanup handler. All objects in this cleanup handler
    will be deleted.

    \sa clear()
*/
QObjectCleanupHandler::~QObjectCleanupHandler()
{
    clear();
}

/*!
    Adds \a object to this cleanup handler and returns the pointer to
    the object.

    \sa remove()
*/
QObject *QObjectCleanupHandler::add(QObject* object)
{
    if (!object)
        return 0;

    connect(object, SIGNAL(destroyed(QObject*)), this, SLOT(objectDestroyed(QObject*)));
    cleanupObjects.insert(0, object);
    return object;
}

/*!
    Removes the \a object from this cleanup handler. The object will
    not be destroyed.

    \sa add()
*/
void QObjectCleanupHandler::remove(QObject *object)
{
    int index;
    if ((index = cleanupObjects.indexOf(object)) != -1) {
        cleanupObjects.removeAt(index);
        disconnect(object, SIGNAL(destroyed(QObject*)), this, SLOT(objectDestroyed(QObject*)));
    }
}

/*!
    Returns true if this cleanup handler is empty or if all objects in
    this cleanup handler have been destroyed; otherwise return false.

    \sa add() remove() clear()
*/
bool QObjectCleanupHandler::isEmpty() const
{
    return cleanupObjects.isEmpty();
}

/*!
    Deletes all objects in this cleanup handler. The cleanup handler
    becomes empty.

    \sa isEmpty()
*/
void QObjectCleanupHandler::clear()
{
    while (!cleanupObjects.isEmpty())
        delete cleanupObjects.takeFirst();
}

void QObjectCleanupHandler::objectDestroyed(QObject *object)
{
    remove(object);
}

QT_END_NAMESPACE
