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

#ifndef QLIBRARY_P_H
#define QLIBRARY_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of the QLibrary class.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//

#ifdef Q_WS_WIN
# include "QtCore/qt_windows.h"
#endif
#include "QtCore/qlibrary.h"
#include "QtCore/qpointer.h"
#include "QtCore/qstringlist.h"
#include "QtCore/qplugin.h"

#ifndef QT_NO_LIBRARY

QT_BEGIN_NAMESPACE

bool qt_debug_component();

class QSettings;
class QLibraryPrivate
{
public:

#ifdef Q_WS_WIN
    HINSTANCE
#else
    void *
#endif
    pHnd;

    QString fileName, qualifiedFileName;
    QString fullVersion;

    bool load();
    bool loadPlugin(); // loads and resolves instance
    bool unload();
    void release();
    void *resolve(const char *);

    static QLibraryPrivate *findOrCreate(const QString &fileName, const QString &version = QString());

    QtPluginInstanceFunction instance;
    uint qt_version;
    QString lastModified;

    QString errorString;
    QLibrary::LoadHints loadHints;

    bool isPlugin(QSettings *settings = 0);


private:
    explicit QLibraryPrivate(const QString &canonicalFileName, const QString &version);
    ~QLibraryPrivate();

    bool load_sys();
    bool unload_sys();
    void *resolve_sys(const char *);

    QAtomicInt libraryRefCount;
    QAtomicInt libraryUnloadCount;

    enum {IsAPlugin, IsNotAPlugin, MightBeAPlugin } pluginState;
    friend class QLibraryPrivateHasFriends;
};

QT_END_NAMESPACE

#endif // QT_NO_LIBRARY

#endif // QLIBRARY_P_H
