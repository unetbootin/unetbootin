/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtScript module of the Qt Toolkit.
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

#include "qscriptextensionplugin.h"

#ifndef QT_NO_SCRIPT

#include "qscriptvalue.h"
#include "qscriptengine.h"

QT_BEGIN_NAMESPACE

/*!
    \since 4.3
    \class QScriptExtensionPlugin
    \brief The QScriptExtensionPlugin class provides an abstract base for custom QScript extension plugins.

    \ingroup plugins

    QScriptExtensionPlugin is a plugin interface that makes it
    possible to offer extensions that can be loaded dynamically into
    applications using the QScriptEngine class.

    Writing a script extension plugin is achieved by subclassing this
    base class, reimplementing the pure virtual keys() and initialize()
    functions, and exporting the class using the Q_EXPORT_PLUGIN2()
    macro. See \l {How to Create Qt Plugins} for details.

    \sa QScriptEngine::importExtension(), {Creating QtScript Extensions}
*/

/*!
    \fn QStringList QScriptExtensionPlugin::keys() const

    Returns the list of keys this plugin supports.

    These keys are usually the names of the "modules" or "packages"
    that are implemented in the plugin (e.g. \c{com.mycompany.MyProduct}).

    \sa initialize()
*/

/*!
    \fn void QScriptExtensionPlugin::initialize(const QString& key, QScriptEngine *engine)

    Initializes the extension specified by \a key in the given \a engine.
    The key must come from the set of keys().

    \sa keys()
*/

/*!
    Constructs a script extension plugin with the given \a parent.

    Note that this constructor is invoked automatically by the
    Q_EXPORT_PLUGIN2() macro, so there is no need for calling it
    explicitly.
*/
QScriptExtensionPlugin::QScriptExtensionPlugin(QObject *parent)
    : QObject(parent)
{
}

/*!
    Destroys the script extension plugin.

    Note that Qt destroys a plugin automatically when it is no longer
    used, so there is no need for calling the destructor explicitly.
*/
QScriptExtensionPlugin::~QScriptExtensionPlugin()
{
}

/*!

  This function is provided for convenience when reimplementing
  initialize(). It splits the given \a key on \c{'.'} (dot), and
  ensures that there's a corresponding path of objects in the
  environment of the given \a engine, creating new objects to complete
  the path if necessary.  E.g. if the key is "com.trolltech", after
  the call to setupPackage() the script expression \c{com.trolltech}
  will evaluate to an object. More specifically, the engine's Global
  Object will have a property called "com", which in turn has a
  property called "trolltech".

  Use this function to avoid global namespace pollution when installing
  your extensions in the engine.

  \sa initialize()
*/
QScriptValue QScriptExtensionPlugin::setupPackage(
    const QString &key, QScriptEngine *engine) const
{
    QStringList components = key.split(QLatin1Char('.'));
    QScriptValue o = engine->globalObject();
    for (int i = 0; i < components.count(); ++i) {
        QScriptValue oo = o.property(components.at(i));
        if (!oo.isValid()) {
            oo = engine->newObject();
            o.setProperty(components.at(i), oo);
        }
        o = oo;
    }
    return o;
}

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT
