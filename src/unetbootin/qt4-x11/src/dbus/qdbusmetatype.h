/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtDBus module of the Qt Toolkit.
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

#ifndef QDBUSMETATYPE_H
#define QDBUSMETATYPE_H

#include "QtCore/qmetatype.h"
#include <QtDBus/qdbusargument.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(DBus)

class QDBUS_EXPORT QDBusMetaType
{
public:
    typedef void (*MarshallFunction)(QDBusArgument &, const void *);
    typedef void (*DemarshallFunction)(const QDBusArgument &, void *);

    static void registerMarshallOperators(int typeId, MarshallFunction, DemarshallFunction);
    static bool marshall(QDBusArgument &, int id, const void *data);
    static bool demarshall(const QDBusArgument &, int id, void *data);

    static int signatureToType(const char *signature);
    static const char *typeToSignature(int type);
};

template<typename T>
void qDBusMarshallHelper(QDBusArgument &arg, const T *t)
{ arg << *t; }

template<typename T>
void qDBusDemarshallHelper(const QDBusArgument &arg, T *t)
{ arg >> *t; }

template<typename T>
int qDBusRegisterMetaType(
#ifndef qdoc
    T * /* dummy */ = 0
#endif
)
{
    void (*mf)(QDBusArgument &, const T *) = qDBusMarshallHelper<T>;
    void (*df)(const QDBusArgument &, T *) = qDBusDemarshallHelper<T>;

    int id = qRegisterMetaType<T>(); // make sure it's registered
    QDBusMetaType::registerMarshallOperators(id,
        reinterpret_cast<QDBusMetaType::MarshallFunction>(mf),
        reinterpret_cast<QDBusMetaType::DemarshallFunction>(df));
    return id;
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
