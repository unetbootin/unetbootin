/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

/****************************************************************************
**
** Implementation of QInputContextFactory class
**
** Copyright (C) 2003-2004 immodule for Qt Project.  All rights reserved.
**
** This file is written to contribute to Nokia Corporation and/or its subsidiary(-ies) under their own
** license. You may use this file under your Qt license. Following
** description is copied from their original file headers. Contact
** immodule-qt@freedesktop.org if any conditions of this licensing are
** not clear to you.
**
****************************************************************************/

#include "qinputcontextfactory.h"

#ifndef QT_NO_IM

#include "qcoreapplication.h"
#include "qinputcontext.h"
#include "qinputcontextplugin.h"

#ifdef Q_WS_X11
#include "private/qt_x11_p.h"
#include "qximinputcontext_p.h"
#endif
#ifdef Q_WS_WIN
#include "qwininputcontext_p.h"
#endif
#ifdef Q_WS_MAC
#include "qmacinputcontext_p.h"
#endif

#include "private/qfactoryloader_p.h"
#include "qmutex.h"

QT_BEGIN_NAMESPACE

#if !defined(QT_NO_LIBRARY) && !defined(QT_NO_SETTINGS)
Q_GLOBAL_STATIC_WITH_ARGS(QFactoryLoader, loader,
    (QInputContextFactoryInterface_iid, QLatin1String("/inputmethods")))
#endif

/*!
    \class QInputContextFactory
    \brief The QInputContextFactory class creates QInputContext objects.

    \ingroup appearance

    The input context factory creates a QInputContext object for a
    given key with QInputContextFactory::create().

    The input contexts are either built-in or dynamically loaded from
    an input context plugin (see QInputContextPlugin).

    keys() returns a list of valid keys. The
    keys are the names used, for example, to identify and specify
    input methods for the input method switching mechanism. The names
    have to be consistent with QInputContext::identifierName(), and
    may only contain ASCII characters.

    A key can be used to retrieve the associated input context's
    supported languages using languages(). You
    can retrieve the input context's description using
    description() and finally you can get a user
    friendly internationalized name of the QInputContext object
    specified by the key using displayName().

    \legalese
    Copyright (C) 2003-2004 immodule for Qt Project.  All rights reserved.

    This file is written to contribute to Nokia Corporation and/or its subsidiary(-ies) under their own
    license. You may use this file under your Qt license. Following
    description is copied from their original file headers. Contact
    immodule-qt@freedesktop.org if any conditions of this licensing are
    not clear to you.
    \endlegalese

    \sa QInputContext, QInputContextPlugin
*/

/*!
    Creates and returns a QInputContext object for the input context
    specified by \a key with the given \a parent. Keys are case
    sensitive.

    \sa keys()
*/
QInputContext *QInputContextFactory::create( const QString& key, QObject *parent )
{
    QInputContext *result = 0;
#if defined(Q_WS_X11) && !defined(QT_NO_XIM)
    if (key == QLatin1String("xim")) {
        result = new QXIMInputContext;
    }
#endif
#if defined(Q_WS_WIN)
    if (key == QLatin1String("win")) {
        result = new QWinInputContext;
    }
#endif
#if defined(Q_WS_MAC)
    if (key == QLatin1String("mac")) {
        result = new QMacInputContext;
    }
#endif
#if defined(QT_NO_LIBRARY) || defined(QT_NO_SETTINGS)
    Q_UNUSED(key);
#else
    if (QInputContextFactoryInterface *factory =
        qobject_cast<QInputContextFactoryInterface*>(loader()->instance(key))) {
        result = factory->create(key);
    }
#endif
    if (result)
        result->setParent(parent);
    return result;
}


/*!
    Returns the list of keys this factory can create input contexts
    for.

    The keys are the names used, for example, to identify and specify
    input methods for the input method switching mechanism.  The names
    have to be consistent with QInputContext::identifierName(), and
    may only contain ASCII characters.

    \sa create(), displayName(), QInputContext::identifierName()
*/
QStringList QInputContextFactory::keys()
{
    QStringList result;
#if defined(Q_WS_X11) && !defined(QT_NO_XIM)
    result << QLatin1String("xim");
#endif
#if defined(Q_WS_WIN) && !defined(QT_NO_XIM)
    result << QLatin1String("win");
#endif
#if defined(Q_WS_MAC)
    result << QLatin1String("mac");
#endif
#if !defined(QT_NO_LIBRARY) && !defined(QT_NO_SETTINGS)
    result += loader()->keys();
#endif // QT_NO_LIBRARY
    return result;
}

/*!
    Returns the languages supported by the QInputContext object
    specified by \a key.

    The languages are expressed as language code (e.g. "zh_CN",
    "zh_TW", "zh_HK", "ja", "ko", ...). An input context that supports
    multiple languages can return all supported languages as a
    QStringList. The name has to be consistent with
    QInputContext::language().

    This information may be used to optimize a user interface.

    \sa keys(), QInputContext::language(), QLocale
*/
QStringList QInputContextFactory::languages( const QString &key )
{
    QStringList result;
#if defined(Q_WS_X11) && !defined(QT_NO_XIM)
    if (key == QLatin1String("xim"))
        return QStringList(QString());
#endif
#if defined(Q_WS_WIN)
    if (key == QLatin1String("win"))
        return QStringList(QString());
#endif
#if defined(Q_WS_MAC)
    if (key == QLatin1String("mac"))
        return QStringList(QString());
#endif
#if defined(QT_NO_LIBRARY) || defined(QT_NO_SETTINGS)
    Q_UNUSED(key);
#else
    if (QInputContextFactoryInterface *factory =
        qobject_cast<QInputContextFactoryInterface*>(loader()->instance(key)))
        result = factory->languages(key);
#endif // QT_NO_LIBRARY
    return result;
}

/*!
    Returns a user friendly internationalized name of the
    QInputContext object specified by \a key. You can, for example,
    use this name in a menu.

    \sa keys(), QInputContext::identifierName()
*/
QString QInputContextFactory::displayName( const QString &key )
{
    QString result;
#if defined(Q_WS_X11) && !defined(QT_NO_XIM)
    if (key == QLatin1String("xim"))
        return QInputContext::tr( "XIM" );
#endif
#if defined(QT_NO_LIBRARY) || defined(QT_NO_SETTINGS)
    Q_UNUSED(key);
#else
    if (QInputContextFactoryInterface *factory =
        qobject_cast<QInputContextFactoryInterface*>(loader()->instance(key)))
        return factory->displayName(key);
#endif // QT_NO_LIBRARY
    return QString();
}

/*!
    Returns an internationalized brief description of the QInputContext
    object specified by \a key. You can, for example, use this
    description in a user interface.

    \sa keys(), displayName()
*/
QString QInputContextFactory::description( const QString &key )
{
#if defined(Q_WS_X11) && !defined(QT_NO_XIM)
    if (key == QLatin1String("xim"))
        return QInputContext::tr( "XIM input method" );
#endif
#if defined(Q_WS_WIN) && !defined(QT_NO_XIM)
    if (key == QLatin1String("win"))
        return QInputContext::tr( "Windows input method" );
#endif
#if defined(Q_WS_MAC)
    if (key == QLatin1String("mac"))
        return QInputContext::tr( "Mac OS X input method" );
#endif
#if defined(QT_NO_LIBRARY) || defined(QT_NO_SETTINGS)
    Q_UNUSED(key);
#else
    if (QInputContextFactoryInterface *factory =
        qobject_cast<QInputContextFactoryInterface*>(loader()->instance(key)))
        return factory->description(key);
#endif // QT_NO_LIBRARY
    return QString();
}

QT_END_NAMESPACE

#endif // QT_NO_IM
