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

#include "qstyleplugin.h"
#include "qstyle.h"

QT_BEGIN_NAMESPACE

/*!
    \class QStylePlugin
    \brief The QStylePlugin class provides an abstract base for custom QStyle plugins.

    \ingroup plugins

    QStylePlugin is a simple plugin interface that makes it easy
    to create custom styles that can be loaded dynamically into
    applications using the QStyleFactory class.

    Writing a style plugin is achieved by subclassing this base class,
    reimplementing the pure virtual keys() and create() functions, and
    exporting the class using the Q_EXPORT_PLUGIN2() macro. See \l
    {How to Create Qt Plugins} for details.

    \sa QStyleFactory, QStyle
*/

/*!
    \fn QStringList QStylePlugin::keys() const

    Returns the list of style keys this plugin supports.

    These keys are usually the class names of the custom styles that
    are implemented in the plugin.

    \sa create()
*/

/*!
    \fn QStyle* QStylePlugin::create(const QString& key)

    Creates and returns a QStyle object for the given style \a key.

    The style key is usually the class name of the required
    style. Note that the keys are case insensitive. For example:

    \snippet doc/src/snippets/qstyleplugin/main.cpp 0
    \codeline
    \snippet doc/src/snippets/qstyleplugin/main.cpp 1
    \snippet doc/src/snippets/qstyleplugin/main.cpp 2

    \sa keys()
*/

/*!
    Constructs a style plugin with the given \a parent.

    Note that this constructor is invoked automatically by the
    Q_EXPORT_PLUGIN2() macro, so there is no need for calling it
    explicitly.
*/
QStylePlugin::QStylePlugin(QObject *parent)
    : QObject(parent)
{
}

/*!
    Destroys the style plugin.

    Note that Qt destroys a plugin automatically when it is no longer
    used, so there is no need for calling the destructor explicitly.
*/
QStylePlugin::~QStylePlugin()
{
}

QT_END_NAMESPACE
