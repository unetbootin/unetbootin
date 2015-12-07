/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#include "qstylefactory.h"
#include "qstyleplugin.h"
#include "private/qfactoryloader_p.h"
#include "qmutex.h"

#include "qapplication.h"
#include "qwindowsstyle.h"
#include "qmotifstyle.h"
#include "qcdestyle.h"
#ifndef QT_NO_STYLE_PLASTIQUE
#include "qplastiquestyle.h"
#endif
#ifndef QT_NO_STYLE_CLEANLOOKS
#include "qcleanlooksstyle.h"
#endif
#ifndef QT_NO_STYLE_WINDOWSXP
#include "qwindowsxpstyle.h"
#endif
#ifndef QT_NO_STYLE_WINDOWSVISTA
#include "qwindowsvistastyle.h"
#endif
#ifndef QT_NO_STYLE_WINDOWSCE
#include "qwindowscestyle.h"
#endif
#ifndef QT_NO_STYLE_WINDOWSMOBILE
#include "qwindowsmobilestyle.h"
#endif

QT_BEGIN_NAMESPACE

#if !defined(QT_NO_STYLE_MAC) && defined(Q_WS_MAC)
QT_BEGIN_INCLUDE_NAMESPACE
#  include <private/qt_mac_p.h>
#  include "qmacstyle_mac.h"
QT_END_INCLUDE_NAMESPACE

QString qt_mac_get_style_name()
{
    QString ret;
    Collection c = NewCollection();
    if (c) {
        GetTheme(c);
        Str255 str;
        SInt32 s = 256;
        if(!GetCollectionItem(c, kThemeNameTag, 0, &s, &str)) {
            extern QString qt_mac_from_pascal_string(const Str255); //qglobal.cpp
            ret = qt_mac_from_pascal_string(str);
        }
    }
    DisposeCollection(c);
    return ret;
}
#endif

#if !defined(QT_NO_LIBRARY) && !defined(QT_NO_SETTINGS)
Q_GLOBAL_STATIC_WITH_ARGS(QFactoryLoader, loader,
    (QStyleFactoryInterface_iid, QLatin1String("/styles"), Qt::CaseInsensitive))
#endif

/*!
    \class QStyleFactory
    \brief The QStyleFactory class creates QStyle objects.

    \ingroup appearance

    The QStyle class is an abstract base class that encapsulates the
    look and feel of a GUI. QStyleFactory creates a QStyle object
    using the create() function and a key identifying the style. The
    styles are either built-in or dynamically loaded from a style
    plugin (see QStylePlugin).

    The valid keys can be retrieved using the keys()
    function. Typically they include "windows", "motif", "cde",
    "plastique" and "cleanlooks".  Depending on the platform,
    "windowsxp", "windowsvista" and "macintosh" may be available.
    Note that keys are case insensitive.

    \sa QStyle
*/

/*!
    Creates a QStyle object that matches the given \a key. This is either a
    built-in style, or a style from a style plugin.

    Note that keys are case insensitive.

    \sa keys()
*/
QStyle *QStyleFactory::create(const QString& key)
{
    QStyle *ret = 0;
    QString style = key.toLower();
#ifndef QT_NO_STYLE_WINDOWS
    if (style == QLatin1String("windows"))
        ret = new QWindowsStyle;
    else
#endif
#ifndef QT_NO_STYLE_WINDOWSCE
    if (style == QLatin1String("windowsce"))
        ret = new QWindowsCEStyle;
    else
#endif
#ifndef QT_NO_STYLE_WINDOWSMOBILE
    if (style == QLatin1String("windowsmobile"))
        ret = new QWindowsMobileStyle;
    else
#endif
#ifndef QT_NO_STYLE_WINDOWSXP
    if (style == QLatin1String("windowsxp"))
        ret = new QWindowsXPStyle;
    else
#endif
#ifndef QT_NO_STYLE_WINDOWSVISTA
    if (style == QLatin1String("windowsvista"))
        ret = new QWindowsVistaStyle;
    else
#endif
#ifndef QT_NO_STYLE_MOTIF
    if (style == QLatin1String("motif"))
        ret = new QMotifStyle;
    else
#endif
#ifndef QT_NO_STYLE_CDE
    if (style == QLatin1String("cde"))
        ret = new QCDEStyle;
    else
#endif
#ifndef QT_NO_STYLE_PLASTIQUE
    if (style == QLatin1String("plastique"))
        ret = new QPlastiqueStyle;
    else
#endif
#ifndef QT_NO_STYLE_CLEANLOOKS
    if (style == QLatin1String("cleanlooks"))
        ret = new QCleanlooksStyle;
    else
#endif
#ifndef QT_NO_STYLE_MAC
    if (style.left(9) == QLatin1String("macintosh")) {
        ret = new QMacStyle;
#  ifdef Q_WS_MAC
        if (style == QLatin1String("macintosh"))
            style += QLatin1String(" (") + qt_mac_get_style_name() + QLatin1Char(')');
#  endif
    } else
#endif
    { } // Keep these here - they make the #ifdefery above work
#if !defined(QT_NO_LIBRARY) && !defined(QT_NO_SETTINGS)
    if(!ret) {
        if (QStyleFactoryInterface *factory = qobject_cast<QStyleFactoryInterface*>(loader()->instance(style)))
            ret = factory->create(style);
    }
#endif
    if(ret)
        ret->setObjectName(style);
    return ret;
}

/*!
    Returns the list of valid keys, i.e. the keys this factory can
    create styles for.

    \sa create()
*/
QStringList QStyleFactory::keys()
{
#if !defined(QT_NO_LIBRARY) && !defined(QT_NO_SETTINGS)
    QStringList list = loader()->keys();
#else
    QStringList list;
#endif
#ifndef QT_NO_STYLE_WINDOWS
    if (!list.contains(QLatin1String("Windows")))
        list << QLatin1String("Windows");
#endif
#ifndef QT_NO_STYLE_WINDOWSCE
    if (!list.contains(QLatin1String("WindowsCE")))
        list << QLatin1String("WindowsCE");
#endif
#ifndef QT_NO_STYLE_WINDOWSMOBILE
    if (!list.contains(QLatin1String("WindowsMobile")))
        list << QLatin1String("WindowsMobile");
#endif
#ifndef QT_NO_STYLE_WINDOWSXP
    if (!list.contains(QLatin1String("WindowsXP")) &&
        (QSysInfo::WindowsVersion >= QSysInfo::WV_XP && QSysInfo::WindowsVersion < QSysInfo::WV_NT_based))
        list << QLatin1String("WindowsXP");
#endif
#ifndef QT_NO_STYLE_WINDOWSVISTA
    if (!list.contains(QLatin1String("WindowsVista")) &&
        (QSysInfo::WindowsVersion >= QSysInfo::WV_VISTA && QSysInfo::WindowsVersion < QSysInfo::WV_NT_based))
        list << QLatin1String("WindowsVista");
#endif
#ifndef QT_NO_STYLE_MOTIF
    if (!list.contains(QLatin1String("Motif")))
        list << QLatin1String("Motif");
#endif
#ifndef QT_NO_STYLE_CDE
    if (!list.contains(QLatin1String("CDE")))
        list << QLatin1String("CDE");
#endif
#ifndef QT_NO_STYLE_PLASTIQUE
    if (!list.contains(QLatin1String("Plastique")))
        list << QLatin1String("Plastique");
#endif
#ifndef QT_NO_STYLE_CLEANLOOKS
    if (!list.contains(QLatin1String("Cleanlooks")))
        list << QLatin1String("Cleanlooks");
#endif
#ifndef QT_NO_STYLE_MAC
    QString mstyle = QLatin1String("Macintosh");
# ifdef Q_WS_MAC
    mstyle += QLatin1String(" (") + qt_mac_get_style_name() + QLatin1String(")");
# endif
    if (!list.contains(mstyle))
        list << mstyle;
#endif
    return list;
}

QT_END_NAMESPACE
