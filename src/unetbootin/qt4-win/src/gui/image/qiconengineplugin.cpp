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

#include "qiconengineplugin.h"
#include "qiconengine.h"

QT_BEGIN_NAMESPACE

/*!
    \class QIconEnginePlugin
    \brief The QIconEnginePlugin class provides an abstract base for custom QIconEngine plugins.

    \ingroup plugins

    \bold {Use QIconEnginePluginV2 instead.}

    The icon engine plugin is a simple plugin interface that makes it easy to
    create custom icon engines that can be loaded dynamically into applications
    through QIcon. QIcon uses the file or resource name's suffix to determine
    what icon engine to use.

    Writing a icon engine plugin is achieved by subclassing this base class,
    reimplementing the pure virtual functions keys() and create(), and
    exporting the class with the Q_EXPORT_PLUGIN2() macro.

    \sa {How to Create Qt Plugins}
*/

/*!
    \fn QStringList QIconEnginePlugin::keys() const

    Returns a list of icon engine keys that this plugin supports. The keys correspond
    to the suffix of the file or resource name used when the plugin was created.
    Keys are case insensitive.

    \sa create()
*/

/*!
    \fn QIconEngine* QIconEnginePlugin::create(const QString& filename)

    Creates and returns a QIconEngine object for the icon with the given
    \a filename.

    \sa keys()
*/

/*!
    Constructs a icon engine plugin with the given \a parent. This is invoked
    automatically by the Q_EXPORT_PLUGIN2() macro.
*/
QIconEnginePlugin::QIconEnginePlugin(QObject *parent)
    : QObject(parent)
{
}

/*!
    Destroys the icon engine plugin.

    You never have to call this explicitly. Qt destroys a plugin
    automatically when it is no longer used.
*/
QIconEnginePlugin::~QIconEnginePlugin()
{
}

// version 2

/*!
    \class QIconEnginePluginV2
    \brief The QIconEnginePluginV2 class provides an abstract base for custom QIconEngineV2 plugins.

    \ingroup plugins
    \since 4.3
 
    Icon engine plugins produces \l{QIconEngine}s for \l{QIcon}s; an
    icon engine is used to render the icon. The keys that identifies
    the engines the plugin can create are suffixes of 
    icon filenames; they are returned by keys(). The create() function
    receives the icon filename to return an engine for; it should
    return 0 if it cannot produce an engine for the file.

    Writing an icon engine plugin is achieved by inheriting
    QIconEnginePluginV2, reimplementing keys() and create(), and
    adding the Q_EXPORT_PLUGIN2() macro.

    You should ensure that you do not duplicate keys. Qt will query
    the plugins for icon engines in the order in which the plugins are
    found during plugin search (see the plugins \l{How to Create Qt
    Plugins}{overview document}).

    \sa {How to Create Qt Plugins}
*/

/*!
    \fn QStringList QIconEnginePluginV2::keys() const

    Returns a list of icon engine keys that this plugin supports. The keys correspond
    to the suffix of the file or resource name used when the plugin was created.
    Keys are case insensitive.

    \sa create()
*/

/*!
    \fn QIconEngineV2* QIconEnginePluginV2::create(const QString& filename = QString())

    Creates and returns a QIconEngine object for the icon with the given
    \a filename.

    \sa keys()
*/

/*!
    Constructs a icon engine plugin with the given \a parent. This is invoked
    automatically by the Q_EXPORT_PLUGIN2() macro.
*/
QIconEnginePluginV2::QIconEnginePluginV2(QObject *parent)
    : QObject(parent)
{
}

/*!
    Destroys the icon engine plugin.

    You never have to call this explicitly. Qt destroys a plugin
    automatically when it is no longer used.
*/
QIconEnginePluginV2::~QIconEnginePluginV2()
{
}

QT_END_NAMESPACE
