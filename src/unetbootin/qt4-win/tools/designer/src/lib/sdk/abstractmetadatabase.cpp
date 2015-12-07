/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt Designer of the Qt Toolkit.
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

// sdk
#include "abstractmetadatabase.h"

QT_BEGIN_NAMESPACE

/*!
    \class QDesignerMetaDataBaseInterface
    \brief The QDesignerMetaDataBaseInterface class provides an interface to Qt Designer's
    object meta database.
    \inmodule QtDesigner
    \internal
*/

/*!
    Constructs an interface to the meta database with the given \a parent.
*/
QDesignerMetaDataBaseInterface::QDesignerMetaDataBaseInterface(QObject *parent)
    : QObject(parent)
{
}

/*!
    Destroys the interface to the meta database.
*/
QDesignerMetaDataBaseInterface::~QDesignerMetaDataBaseInterface()
{
}

/*!
    \fn QDesignerMetaDataBaseItemInterface *QDesignerMetaDataBaseInterface::item(QObject *object) const

    Returns the item in the meta database associated with the given \a object.
*/

/*!
    \fn void QDesignerMetaDataBaseInterface::add(QObject *object)

    Adds the specified \a object to the meta database.
*/

/*!
    \fn void QDesignerMetaDataBaseInterface::remove(QObject *object)

    Removes the specified \a object from the meta database.
*/

/*!
    \fn QList<QObject*> QDesignerMetaDataBaseInterface::objects() const

    Returns the list of objects that have corresponding items in the meta database.
*/

/*!
    \fn QDesignerFormEditorInterface *QDesignerMetaDataBaseInterface::core() const

    Returns the core interface that is associated with the meta database.
*/


// Doc: Interface only

/*!
    \class QDesignerMetaDataBaseItemInterface
    \brief The QDesignerMetaDataBaseItemInterface class provides an interface to individual
    items in Qt Designer's meta database.
    \inmodule QtDesigner
    \internal

    This class allows individual items in \QD's meta-data database to be accessed and modified.
    Use the QDesignerMetaDataBaseInterface class to change the properties of the database itself.
*/

/*!
    \fn QDesignerMetaDataBaseItemInterface::~QDesignerMetaDataBaseItemInterface()

    Destroys the item interface to the meta-data database.
*/

/*!
    \fn QString QDesignerMetaDataBaseItemInterface::name() const

    Returns the name of the item in the database.

    \sa setName()
*/

/*!
    \fn void QDesignerMetaDataBaseItemInterface::setName(const QString &name)

    Sets the name of the item to the given \a name.

    \sa name()
*/

/*!
    \fn QList<QWidget*> QDesignerMetaDataBaseItemInterface::tabOrder() const

    Returns a list of widgets in the order defined by the form's tab order.

    \sa setTabOrder()
*/


/*!
    \fn void QDesignerMetaDataBaseItemInterface::setTabOrder(const QList<QWidget*> &tabOrder)

    Sets the tab order in the form using the list of widgets defined by \a tabOrder.

    \sa tabOrder()
*/

    
/*!
    \fn bool QDesignerMetaDataBaseItemInterface::enabled() const

    Returns whether the item is enabled.

    \sa setEnabled()
*/

/*!
    \fn void QDesignerMetaDataBaseItemInterface::setEnabled(bool enabled)

    If \a enabled is true, the item is enabled; otherwise it is disabled.

    \sa enabled()
*/

QT_END_NAMESPACE
