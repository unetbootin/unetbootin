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

#ifndef EXTENSION_H
#define EXTENSION_H

#include <QtCore/QString>
#include <QtCore/QObject>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

#define Q_TYPEID(IFace) QLatin1String(IFace##_iid)

class QAbstractExtensionFactory
{
public:
    virtual ~QAbstractExtensionFactory() {}

    virtual QObject *extension(QObject *object, const QString &iid) const = 0;
};
Q_DECLARE_INTERFACE(QAbstractExtensionFactory, "com.trolltech.Qt.QAbstractExtensionFactory")

class QAbstractExtensionManager
{
public:
    virtual ~QAbstractExtensionManager() {}

    virtual void registerExtensions(QAbstractExtensionFactory *factory, const QString &iid) = 0;
    virtual void unregisterExtensions(QAbstractExtensionFactory *factory, const QString &iid) = 0;

    virtual QObject *extension(QObject *object, const QString &iid) const = 0;
};
Q_DECLARE_INTERFACE(QAbstractExtensionManager, "com.trolltech.Qt.QAbstractExtensionManager")

#if defined(Q_CC_MSVC) && (_MSC_VER < 1300)

template <class T>
inline T qt_extension_helper(QAbstractExtensionManager *, QObject *, T)
{ return 0; }

template <class T>
inline T qt_extension(QAbstractExtensionManager* manager, QObject *object)
{ return qt_extension_helper(manager, object, T(0)); }

#define Q_DECLARE_EXTENSION_INTERFACE(IFace, IId) \
const char * const IFace##_iid = IId; \
Q_DECLARE_INTERFACE(IFace, IId) \
template <> inline IFace *qt_extension_helper<IFace *>(QAbstractExtensionManager *manager, QObject *object, IFace *) \
{ QObject *extension = manager->extension(object, Q_TYPEID(IFace)); return (IFace *)(extension ? extension->qt_metacast(IFace##_iid) : 0); }

#else

template <class T>
inline T qt_extension(QAbstractExtensionManager* manager, QObject *object)
{ return 0; }

#define Q_DECLARE_EXTENSION_INTERFACE(IFace, IId) \
const char * const IFace##_iid = IId; \
Q_DECLARE_INTERFACE(IFace, IId) \
template <> inline IFace *qt_extension<IFace *>(QAbstractExtensionManager *manager, QObject *object) \
{ QObject *extension = manager->extension(object, Q_TYPEID(IFace)); return extension ? static_cast<IFace *>(extension->qt_metacast(IFace##_iid)) : static_cast<IFace *>(0); }

#endif

QT_END_NAMESPACE

QT_END_HEADER

#endif // EXTENSION_H
