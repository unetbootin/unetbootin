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

#include "pluginmanager_p.h"
#include "qdesigner_utils_p.h"

#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QDesignerCustomWidgetInterface>

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QSet>
#include <QtCore/QPluginLoader>
#include <QtCore/QLibrary>
#include <QtCore/QLibraryInfo>
#include <QtCore/qdebug.h>
#include <QtCore/QMap>
#include <QtCore/QSettings>
#include <QtCore/QCoreApplication>

QT_BEGIN_NAMESPACE

static QStringList unique(const QStringList &lst)
{
    const QSet<QString> s = QSet<QString>::fromList(lst);
    return s.toList();
}

QStringList QDesignerPluginManager::defaultPluginPaths()
{
    QStringList result;

    const QStringList path_list = QCoreApplication::libraryPaths();

    const QString designer = QLatin1String("designer");
    foreach (const QString &path, path_list) {
        QString libPath = path;
        libPath += QDir::separator();
        libPath += designer;
        result.append(libPath);
    }

    QString homeLibPath = QDir::homePath();
    homeLibPath += QDir::separator();
    homeLibPath += QLatin1String(".designer");
    homeLibPath += QDir::separator();
    homeLibPath += QLatin1String("plugins");

    result.append(homeLibPath);
    return result;
}

// ---------------- QDesignerPluginManagerPrivate

class QDesignerPluginManagerPrivate {
    public:
    QDesignerPluginManagerPrivate(QDesignerFormEditorInterface *core);

    QDesignerFormEditorInterface *m_core;
    QStringList m_pluginPaths;
    QStringList m_registeredPlugins;
    QStringList m_disabledPlugins;

    typedef QMap<QString, QString> FailedPluginMap;
    FailedPluginMap m_failedPlugins;

    typedef QList<QDesignerCustomWidgetInterface*> CustomWidgetList;
    CustomWidgetList m_customWidgets;

    QStringList defaultPluginPaths() const;
};

QDesignerPluginManagerPrivate::QDesignerPluginManagerPrivate(QDesignerFormEditorInterface *core) :
   m_core(core)
{
}


// ---------------- QDesignerPluginManager
// As of 4.4, the header will be distributed with the Eclipse plugin.

QDesignerPluginManager::QDesignerPluginManager(QDesignerFormEditorInterface *core) :
    QObject(core),
    m_d(new QDesignerPluginManagerPrivate(core))
{
    QSettings settings;

    settings.beginGroup(QLatin1String("PluginManager"));

    m_d->m_pluginPaths = defaultPluginPaths();
    m_d->m_disabledPlugins
        = unique(settings.value(QLatin1String("DisabledPlugins")).toStringList());
    updateRegisteredPlugins();

    settings.endGroup();
}

QDesignerPluginManager::~QDesignerPluginManager()
{
    syncSettings();
    delete m_d;
}

QDesignerFormEditorInterface *QDesignerPluginManager::core() const
{
    return m_d->m_core;
}

QStringList QDesignerPluginManager::findPlugins(const QString &path)
{
    const QDir dir(path);
    if (!dir.exists())
        return QStringList();

    const QFileInfoList infoList = dir.entryInfoList(QDir::Files);
    if (infoList.empty())
        return QStringList();

    // Load symbolic links but make sure all file names are unique as not
    // to fall for something like 'libplugin.so.1 -> libplugin.so'
    QStringList result;
    const QFileInfoList::const_iterator icend = infoList.constEnd();
    for (QFileInfoList::const_iterator it = infoList.constBegin(); it != icend; ++it) {
        QString fileName;
        if (it->isSymLink()) {
            const QFileInfo linkTarget = QFileInfo(it->symLinkTarget());
            if (linkTarget.exists() && linkTarget.isFile())
                fileName = linkTarget.absoluteFilePath();
        } else {
            fileName = it->absoluteFilePath();
        }
        if (!fileName.isEmpty() && QLibrary::isLibrary(fileName) && !result.contains(fileName))
            result += fileName;
    }
    return result;
}

void QDesignerPluginManager::setDisabledPlugins(const QStringList &disabled_plugins)
{
    m_d->m_disabledPlugins = disabled_plugins;
    updateRegisteredPlugins();
}

void QDesignerPluginManager::setPluginPaths(const QStringList &plugin_paths)
{
    m_d->m_pluginPaths = plugin_paths;
    updateRegisteredPlugins();
}

QStringList QDesignerPluginManager::disabledPlugins() const
{
    return m_d->m_disabledPlugins;
}

QStringList QDesignerPluginManager::failedPlugins() const
{
    return m_d->m_failedPlugins.keys();
}

QString QDesignerPluginManager::failureReason(const QString &pluginName) const
{
    return m_d->m_failedPlugins.value(pluginName);
}

QStringList QDesignerPluginManager::registeredPlugins() const
{
    return m_d->m_registeredPlugins;
}

QStringList QDesignerPluginManager::pluginPaths() const
{
    return m_d->m_pluginPaths;
}

QObject *QDesignerPluginManager::instance(const QString &plugin) const
{
    if (m_d->m_disabledPlugins.contains(plugin))
        return 0;

    QPluginLoader loader(plugin);
    return loader.instance();
}

void QDesignerPluginManager::updateRegisteredPlugins()
{
    m_d->m_registeredPlugins.clear();
    foreach (QString path,  m_d->m_pluginPaths)
        registerPath(path);
}

bool QDesignerPluginManager::registerNewPlugins()
{
    const int before = m_d->m_registeredPlugins.size();
    foreach (QString path,  m_d->m_pluginPaths)
        registerPath(path);
    const bool newPluginsFound = m_d->m_registeredPlugins.size() > before;
    if (newPluginsFound)
        ensureInitialized();
    return newPluginsFound;
}

void QDesignerPluginManager::registerPath(const QString &path)
{
    QStringList candidates = findPlugins(path);

    foreach (QString plugin, candidates)
        registerPlugin(plugin);
}

void QDesignerPluginManager::registerPlugin(const QString &plugin)
{
    if (m_d->m_disabledPlugins.contains(plugin))
        return;
    if (m_d->m_registeredPlugins.contains(plugin))
        return;

    QPluginLoader loader(plugin);
    if (loader.isLoaded() || loader.load()) {
        m_d->m_registeredPlugins += plugin;
        QDesignerPluginManagerPrivate::FailedPluginMap::iterator fit = m_d->m_failedPlugins.find(plugin);
        if (fit != m_d->m_failedPlugins.end())
            m_d->m_failedPlugins.erase(fit);
        return;
    }

    const QString errorMessage = loader.errorString();
    m_d->m_failedPlugins.insert(plugin, errorMessage);
}

bool QDesignerPluginManager::syncSettings()
{
    QSettings settings;
    settings.beginGroup(QLatin1String("PluginManager"));
    settings.setValue(QLatin1String("DisabledPlugins"), m_d->m_disabledPlugins);
    settings.endGroup();
    return settings.status() == QSettings::NoError;
}

void QDesignerPluginManager::ensureInitialized()
{
    m_d->m_customWidgets.clear();
    // Complete list of plugins, static and dynamic
    QObjectList pluginObjects = QPluginLoader::staticInstances();

    const QStringList plugins = registeredPlugins();
    foreach (const QString &plugin, plugins)
        if (QObject *o = instance(plugin))
            pluginObjects.push_back(o);

    foreach(QObject *o, pluginObjects) {
        if (QDesignerCustomWidgetInterface *c = qobject_cast<QDesignerCustomWidgetInterface*>(o)) {
            m_d->m_customWidgets.append(c);
        } else if (QDesignerCustomWidgetCollectionInterface *coll = qobject_cast<QDesignerCustomWidgetCollectionInterface*>(o)) {
            m_d->m_customWidgets += coll->customWidgets();
        }
    }

    foreach (QDesignerCustomWidgetInterface *c, m_d->m_customWidgets) {
        if (!c->isInitialized()) {
            c->initialize(core());
        }
    }
}

QList<QDesignerCustomWidgetInterface*> QDesignerPluginManager::registeredCustomWidgets() const
{
    const_cast<QDesignerPluginManager*>(this)->ensureInitialized();
    return m_d->m_customWidgets;
}

QList<QObject*> QDesignerPluginManager::instances() const
{
    QStringList plugins = registeredPlugins();

    QList<QObject*> lst;
    foreach (QString plugin, plugins) {
        if (QObject *o = instance(plugin))
            lst.append(o);
    }

    return lst;
}

QT_END_NAMESPACE
