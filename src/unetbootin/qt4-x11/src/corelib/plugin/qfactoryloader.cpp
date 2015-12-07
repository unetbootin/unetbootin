/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#include "qfactoryloader_p.h"

#if !defined (QT_NO_LIBRARY) && !defined(QT_NO_SETTINGS)
#include "qfactoryinterface.h"
#include "qmap.h"
#include <qdir.h>
#include <qsettings.h>
#include <qdebug.h>
#include "qmutex.h"
#include "qplugin.h"
#include "qpluginloader.h"
#include "private/qobject_p.h"
#include "private/qcoreapplication_p.h"

QT_BEGIN_NAMESPACE

Q_GLOBAL_STATIC(QList<QFactoryLoader *>, qt_factory_loaders);

Q_GLOBAL_STATIC_WITH_ARGS(QMutex, qt_factoryloader_mutex, (QMutex::Recursive))

class QFactoryLoaderPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QFactoryLoader)
public:
    QFactoryLoaderPrivate(){}
    mutable QMutex mutex;
    QByteArray iid;
    QList<QLibraryPrivate*> libraryList;
    QMap<QString,QLibraryPrivate*> keyMap;
    QStringList keyList;
    QString suffix;
    Qt::CaseSensitivity cs;
    QStringList loadedPaths;

    void unloadPath(const QString &path);
};

QFactoryLoader::QFactoryLoader(const char *iid,
                               const QString &suffix,
                               Qt::CaseSensitivity cs)
    : QObject(*new QFactoryLoaderPrivate)
{
    moveToThread(QCoreApplicationPrivate::mainThread());
    Q_D(QFactoryLoader);
    d->iid = iid;
    d->cs = cs;
    d->suffix = suffix;


    QMutexLocker locker(qt_factoryloader_mutex());
    qt_factory_loaders()->append(this);
    update();
}



void QFactoryLoader::update()
{
#ifdef QT_SHARED
    Q_D(QFactoryLoader);
    QStringList paths = QCoreApplication::libraryPaths();
    QSettings settings(QSettings::UserScope, QLatin1String("Trolltech"));
    for (int i = 0; i < paths.count(); ++i) {
        const QString &pluginDir = paths.at(i);
        // Already loaded, skip it...
        if (d->loadedPaths.contains(pluginDir))
            continue;
        d->loadedPaths << pluginDir;

        QString path = pluginDir + d->suffix;
        if (!QDir(path).exists(QLatin1String(".")))
            continue;

        QStringList plugins = QDir(path).entryList(QDir::Files);
        QLibraryPrivate *library = 0;
        for (int j = 0; j < plugins.count(); ++j) {
            QString fileName = QDir::cleanPath(path + QLatin1Char('/') + plugins.at(j));
            if (qt_debug_component()) {
                qDebug() << "QFactoryLoader::QFactoryLoader() looking at" << fileName;
            }
            library = QLibraryPrivate::findOrCreate(QFileInfo(fileName).canonicalFilePath());
            if (!library->isPlugin(&settings)) {
                if (qt_debug_component()) {
                    qDebug() << library->errorString;
                    qDebug() << "         not a plugin";
                }
                library->release();
                continue;
            }
            QString regkey = QString::fromLatin1("Qt Factory Cache %1.%2/%3:/%4")
                             .arg((QT_VERSION & 0xff0000) >> 16)
                             .arg((QT_VERSION & 0xff00) >> 8)
                             .arg(QLatin1String(d->iid))
                             .arg(fileName);
            QStringList reg, keys;
            reg = settings.value(regkey).toStringList();
            if (reg.count() && library->lastModified == reg[0]) {
                keys = reg;
                keys.removeFirst();
            } else {
                if (!library->loadPlugin()) {
                    if (qt_debug_component()) {
                        qDebug() << library->errorString;
                        qDebug() << "           could not load";
                    }
                    library->release();
                    continue;
                }
                QObject *instance = library->instance();
                if (!instance)
                    // ignore plugins that have a valid signature but cannot be loaded.
                    continue;
                QFactoryInterface *factory = qobject_cast<QFactoryInterface*>(instance);
                if (instance && factory && instance->qt_metacast(d->iid))
                    keys = factory->keys();
                if (keys.isEmpty())
                    library->unload();
                reg.clear();
                reg << library->lastModified;
                reg += keys;
                settings.setValue(regkey, reg);
            }
            if (qt_debug_component()) {
                qDebug() << "keys" << keys;
            }

            if (keys.isEmpty()) {
                library->release();
                continue;
            }
            d->libraryList += library;
            for (int k = 0; k < keys.count(); ++k) {
                // first come first serve, unless the first
                // library was built with a future Qt version,
                // whereas the new one has a Qt version that fits
                // better
                QString key = keys.at(k);
                if (!d->cs)
                    key = key.toLower();
                QLibraryPrivate *previous = d->keyMap.value(key);
                if (!previous || (previous->qt_version > QT_VERSION && library->qt_version <= QT_VERSION)) {
                    d->keyMap[key] = library;
                    d->keyList += keys.at(k);
                }
            }
        }
    }
#else
    Q_D(QFactoryLoader);
    if (qt_debug_component()) {
        qDebug() << "QFactoryLoader::QFactoryLoader() ignoring" << d->iid
                 << "since plugins are disabled in static builds";
    }
#endif
}

QFactoryLoader::~QFactoryLoader()
{
    Q_D(QFactoryLoader);
    for (int i = 0; i < d->libraryList.count(); ++i)
        d->libraryList.at(i)->release();

    QMutexLocker locker(qt_factoryloader_mutex());
    qt_factory_loaders()->removeAll(this);
}

QStringList QFactoryLoader::keys() const
{
    Q_D(const QFactoryLoader);
    QMutexLocker locker(&d->mutex);
    QStringList keys = d->keyList;
    QObjectList instances = QPluginLoader::staticInstances();
    for (int i = 0; i < instances.count(); ++i)
        if (QFactoryInterface *factory = qobject_cast<QFactoryInterface*>(instances.at(i)))
            if (instances.at(i)->qt_metacast(d->iid))
                keys += factory->keys();
    return keys;
}

QObject *QFactoryLoader::instance(const QString &key) const
{
    Q_D(const QFactoryLoader);
    QMutexLocker locker(&d->mutex);
    QObjectList instances = QPluginLoader::staticInstances();
    for (int i = 0; i < instances.count(); ++i)
        if (QFactoryInterface *factory = qobject_cast<QFactoryInterface*>(instances.at(i)))
            if (instances.at(i)->qt_metacast(d->iid) && factory->keys().contains(key, Qt::CaseInsensitive))
                return instances.at(i);

    QString lowered = d->cs ? key : key.toLower();
    if (QLibraryPrivate* library = d->keyMap.value(lowered)) {
        if (library->instance || library->loadPlugin()) {
            if (QObject *obj = library->instance()) {
                if (obj && !obj->parent())
                    obj->moveToThread(QCoreApplicationPrivate::mainThread());
                return obj;
            }
        }
    }
    return 0;
}

void QFactoryLoader::refreshAll()
{
    QMutexLocker locker(qt_factoryloader_mutex());
    QList<QFactoryLoader *> *loaders = qt_factory_loaders();
    for (QList<QFactoryLoader *>::const_iterator it = loaders->constBegin();
         it != loaders->constEnd(); ++it) {
        (*it)->update();
    }
}

QT_END_NAMESPACE

#endif // QT_NO_LIBRARY
