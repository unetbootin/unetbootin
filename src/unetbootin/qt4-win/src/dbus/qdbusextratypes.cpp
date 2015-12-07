/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtDBus module of the Qt Toolkit.
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

#include "qdbusextratypes.h"
#include "qdbusutil_p.h"

QT_BEGIN_NAMESPACE

void QDBusObjectPath::check()
{
    if (!QDBusUtil::isValidObjectPath(*this)) {
        qWarning("QDBusObjectPath: invalid path \"%s\"", qPrintable(*this));
        clear();
    }
}

void QDBusSignature::check()
{
    if (!QDBusUtil::isValidSignature(*this)) {
        qWarning("QDBusSignature: invalid signature \"%s\"", qPrintable(*this));
        clear();
    }
}

/*!
    \class QDBusVariant
    \inmodule QtDBus
    \since 4.2

    \brief The QDBusVariant class enables the programmer to identify
    the variant type provided by the D-BUS typesystem.

    \sa {The QtDBus type system}
*/

/*!
    \fn QDBusVariant::QDBusVariant()

    Constructs a new variant.
*/

/*!
    \fn QDBusVariant::QDBusVariant(const QVariant &variant)

    Constructs a new variant from the given \a variant.

    \sa setVariant()
*/

/*!
    \fn QVariant QDBusVariant::variant() const

    Returns this variant as a QVariant object.

    \sa setVariant()
*/

/*!
    \fn void QDBusVariant::setVariant(const QVariant &variant)

    Assigns the value of the given \a variant to this variant.

    \sa variant()
*/

/*!
    \class QDBusObjectPath
    \inmodule QtDBus
    \since 4.2

    \brief The QDBusObjectPath class enables the programmer to
    identify the \c OBJECT_PATH type provided by the D-BUS typesystem.

    \sa {The QtDBus type system}
*/

/*!
    \fn QDBusObjectPath::QDBusObjectPath()

    Constructs a new object path.
*/

/*!
    \fn QDBusObjectPath::QDBusObjectPath(const char *path)

    Constructs a new object path from the given \a path.

    \sa setPath()
*/

/*!
    \fn QDBusObjectPath::QDBusObjectPath(const QLatin1String &path)

    Constructs a new object path from the given \a path.
*/

/*!
    \fn QDBusObjectPath::QDBusObjectPath(const QString &path)

    Constructs a new object path from the given \a path.
*/

/*!
    \fn QString QDBusObjectPath::path() const

    Returns this object path.

    \sa setPath()
*/

/*!
    \fn void QDBusObjectPath::setPath(const QString &path)

    Assigns the value of the given \a path to this object path.

    \sa path()
*/

/*!
    \fn QDBusObjectPath &QDBusObjectPath::operator=(const QDBusObjectPath &path)

    Assigns the value of the given \a path to this object path.

    \sa setPath()
*/


/*!
    \class QDBusSignature
    \inmodule QtDBus
    \since 4.2

    \brief The QDBusSignature class enables the programmer to
    identify the \c SIGNATURE type provided by the D-BUS typesystem.

    \sa {The QtDBus type system}
*/

/*!
    \fn QDBusSignature::QDBusSignature()

    Constructs a new signature.

    \sa setSignature()
*/

/*!
    \fn QDBusSignature::QDBusSignature(const char *signature)

    Constructs a new signature from the given \a signature.
*/

/*!
    \fn QDBusSignature::QDBusSignature(const QLatin1String &signature)

    Constructs a new signature from the given \a signature.
*/

/*!
    \fn QDBusSignature::QDBusSignature(const QString &signature)

    Constructs a new signature from the given \a signature.
*/

/*!
    \fn QString QDBusSignature::signature() const

    Returns this signature.

    \sa setSignature()
*/

/*!
    \fn void QDBusSignature::setSignature(const QString &signature)

    Assigns the value of the given \a signature to this signature.
    \sa signature()
*/

/*!
    \fn QDBusSignature &QDBusSignature::operator=(const QDBusSignature &signature)

    Assigns the value of the given \a signature to this signature.

    \sa setSignature()
*/

QT_END_NAMESPACE

