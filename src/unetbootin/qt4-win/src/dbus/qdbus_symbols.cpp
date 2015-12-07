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

#include <QtCore/qglobal.h>
#include <QtCore/qlibrary.h>
#include <QtCore/qmutex.h>
#include <private/qmutexpool_p.h>

QT_BEGIN_NAMESPACE

void *qdbus_resolve_me(const char *name);

#if !defined QT_LINKED_LIBDBUS

static QLibrary *qdbus_libdbus = 0;

void qdbus_unloadLibDBus()
{
    delete qdbus_libdbus;
    qdbus_libdbus = 0;
}

bool qdbus_loadLibDBus()
{
    static volatile bool triedToLoadLibrary = false;
#ifndef QT_NO_THREAD
    QMutexLocker locker(QMutexPool::globalInstanceGet((void *)&qdbus_resolve_me));
#endif
    QLibrary *&lib = qdbus_libdbus;
    if (triedToLoadLibrary)
        return lib && lib->isLoaded();

    lib = new QLibrary;
    triedToLoadLibrary = true;

    static int majorversions[] = { 3, 2, -1 };
    lib->unload();
    lib->setFileName(QLatin1String("dbus-1"));
    for (uint i = 0; i < sizeof(majorversions) / sizeof(majorversions[0]); ++i) {
        lib->setFileNameAndVersion(lib->fileName(), majorversions[i]);
        if (lib->load() && lib->resolve("dbus_connection_open_private"))
            return true;

        lib->unload();
    }

    delete lib;
    lib = 0;
    return false;
}

void *qdbus_resolve_conditionally(const char *name)
{
    if (qdbus_loadLibDBus())
        return qdbus_libdbus->resolve(name);
    return 0;
}

void *qdbus_resolve_me(const char *name)
{
    void *ptr = 0;
    if (!qdbus_loadLibDBus())
        qFatal("Cannot find libdbus-1 in your system to resolve symbol '%s'.", name);

    ptr = qdbus_libdbus->resolve(name);
    if (!ptr)
        qFatal("Cannot resolve '%s' in your libdbus-1.", name);

    return ptr;
}

Q_DESTRUCTOR_FUNCTION(qdbus_unloadLibDBus)

QT_END_NAMESPACE

#endif
