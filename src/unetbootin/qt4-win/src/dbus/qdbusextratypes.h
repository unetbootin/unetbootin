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

#ifndef QDBUSEXTRATYPES_H
#define QDBUSEXTRATYPES_H

// define some useful types for D-BUS

#include <QtCore/qvariant.h>
#include <QtCore/qstring.h>
#include <QtDBus/qdbusmacros.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(DBus)

class QDBUS_EXPORT QDBusObjectPath : private QString
{
public:
    inline QDBusObjectPath() { }

    inline explicit QDBusObjectPath(const char *path);
    inline explicit QDBusObjectPath(const QLatin1String &path);
    inline explicit QDBusObjectPath(const QString &path);
    inline QDBusObjectPath &operator=(const QDBusObjectPath &path);

    inline void setPath(const QString &path);

    inline QString path() const
    { return *this; }

private:
    void check();
};

inline QDBusObjectPath::QDBusObjectPath(const char *objectPath)
    : QString(QString::fromLatin1(objectPath))
{ check(); }

inline QDBusObjectPath::QDBusObjectPath(const QLatin1String &objectPath)
    : QString(objectPath)
{ check(); }

inline QDBusObjectPath::QDBusObjectPath(const QString &objectPath)
    : QString(objectPath)
{ check(); }

inline QDBusObjectPath &QDBusObjectPath::operator=(const QDBusObjectPath &_path)
{ QString::operator=(_path); check(); return *this; }

inline void QDBusObjectPath::setPath(const QString &objectPath)
{ QString::operator=(objectPath); check(); }


class QDBUS_EXPORT QDBusSignature : private QString
{
public:
    inline QDBusSignature() { }

    inline explicit QDBusSignature(const char *signature);
    inline explicit QDBusSignature(const QLatin1String &signature);
    inline explicit QDBusSignature(const QString &signature);
    inline QDBusSignature &operator=(const QDBusSignature &signature);

    inline void setSignature(const QString &signature);

    inline QString signature() const
    { return *this; }

private:
    void check();
};

inline QDBusSignature::QDBusSignature(const char *dBusSignature)
    : QString(QString::fromAscii(dBusSignature))
{ check(); }

inline QDBusSignature::QDBusSignature(const QLatin1String &dBusSignature)
    : QString(dBusSignature)
{ check(); }

inline QDBusSignature::QDBusSignature(const QString &dBusSignature)
    : QString(dBusSignature)
{ check(); }

inline QDBusSignature &QDBusSignature::operator=(const QDBusSignature &dbusSignature)
{ QString::operator=(dbusSignature); check(); return *this; }

inline void QDBusSignature::setSignature(const QString &dBusSignature)
{ QString::operator=(dBusSignature); check(); }

class QDBusVariant : private QVariant
{
public:
    inline QDBusVariant() { }
    inline explicit QDBusVariant(const QVariant &variant);

    inline void setVariant(const QVariant &variant);

    inline QVariant variant() const
    { return *this; }
};

inline  QDBusVariant::QDBusVariant(const QVariant &dBusVariant)
    : QVariant(dBusVariant) { }

inline void QDBusVariant::setVariant(const QVariant &dBusVariant)
{ QVariant::operator=(dBusVariant); }

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QDBusVariant)
Q_DECLARE_METATYPE(QDBusObjectPath)
Q_DECLARE_METATYPE(QList<QDBusObjectPath>)
Q_DECLARE_METATYPE(QDBusSignature)
Q_DECLARE_METATYPE(QList<QDBusSignature>)

QT_END_HEADER

#endif
