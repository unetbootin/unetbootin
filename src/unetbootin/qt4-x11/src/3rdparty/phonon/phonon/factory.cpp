/*  This file is part of the KDE project
    Copyright (C) 2004-2007 Matthias Kretz <kretz@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), Trolltech ASA 
    (or its successors, if any) and the KDE Free Qt Foundation, which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public 
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "factory_p.h"

#include "backendinterface.h"
#include "medianode_p.h"
#include "mediaobject.h"
#include "audiooutput.h"
#include "globalstatic_p.h"
#include "objectdescription.h"
#include "platformplugin.h"
#include "phononnamespace_p.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QList>
#include <QtCore/QPluginLoader>
#include <QtCore/QPointer>
#ifndef QT_NO_DBUS
#include <QtDBus/QtDBus>
#endif

QT_BEGIN_NAMESPACE

namespace Phonon
{

class PlatformPlugin;
class FactoryPrivate : public Phonon::Factory::Sender
{
    friend QObject *Factory::backend(bool);
    Q_OBJECT
    public:
        FactoryPrivate();
        ~FactoryPrivate();
        bool createBackend();
#ifndef QT_NO_PHONON_PLATFORMPLUGIN
        PlatformPlugin *platformPlugin();

        PlatformPlugin *m_platformPlugin;
        bool m_noPlatformPlugin;
#endif //QT_NO_PHONON_PLATFORMPLUGIN
        QPointer<QObject> m_backendObject;

        QList<QObject *> objects;
        QList<MediaNodePrivate *> mediaNodePrivateList;

    private Q_SLOTS:
        /**
         * This is called via DBUS when the user changes the Phonon Backend.
         */
#ifndef QT_NO_DBUS
        void phononBackendChanged();
#endif //QT_NO_DBUS

        /**
         * unregisters the backend object
         */
        void objectDestroyed(QObject *);

        void objectDescriptionChanged(ObjectDescriptionType);
};

PHONON_GLOBAL_STATIC(Phonon::FactoryPrivate, globalFactory)

static inline void ensureLibraryPathSet()
{
#ifdef PHONON_LIBRARY_PATH
    static bool done = false;
    if (!done) {
        done = true;
        QCoreApplication::addLibraryPath(QLatin1String(PHONON_LIBRARY_PATH));
    }
#endif // PHONON_LIBRARY_PATH
}

void Factory::setBackend(QObject *b)
{
    Q_ASSERT(globalFactory->m_backendObject == 0);
    globalFactory->m_backendObject = b;
}

/*void Factory::createBackend(const QString &library, const QString &version)
{
    Q_ASSERT(globalFactory->m_backendObject == 0);
    PlatformPlugin *f = globalFactory->platformPlugin();
    if (f) {
        globalFactory->m_backendObject = f->createBackend(library, version);
    }
}*/

bool FactoryPrivate::createBackend()
{
    Q_ASSERT(m_backendObject == 0);
#ifndef QT_NO_PHONON_PLATFORMPLUGIN
    PlatformPlugin *f = globalFactory->platformPlugin();
    if (f) {
        m_backendObject = f->createBackend();
    }
#endif //QT_NO_PHONON_PLATFORMPLUGIN
    if (!m_backendObject) {
        ensureLibraryPathSet();

        // could not load a backend through the platform plugin. Falling back to the default
        // (finding the first loadable backend).
        const QLatin1String suffix("/phonon_backend/");
        foreach (QString libPath, QCoreApplication::libraryPaths()) {
            libPath += suffix;
            const QDir dir(libPath);
            if (!dir.exists()) {
                pDebug() << Q_FUNC_INFO << dir.absolutePath() << "does not exist";
                continue;
            }
            foreach (const QString &pluginName, dir.entryList(QDir::Files)) {
                QPluginLoader pluginLoader(libPath + pluginName);
                if (!pluginLoader.load()) {
                    pDebug() << Q_FUNC_INFO << "  load failed:"
                             << pluginLoader.errorString();
                    continue;
                }
                pDebug() << pluginLoader.instance();
                m_backendObject = pluginLoader.instance();
                if (m_backendObject) {
                    break;
                }

                // no backend found, don't leave an unused plugin in memory
                pluginLoader.unload();
            }

            if (m_backendObject) {
                break;
            }
        }
        if (!m_backendObject) {
            pWarning() << Q_FUNC_INFO << "phonon backend plugin could not be loaded";
            return false;
        }
    }

    connect(m_backendObject, SIGNAL(objectDescriptionChanged(ObjectDescriptionType)),
            SLOT(objectDescriptionChanged(ObjectDescriptionType)));

    return true;
}

FactoryPrivate::FactoryPrivate()
#ifndef QT_NO_PHONON_PLATFORMPLUGIN
    : m_platformPlugin(0),
    m_noPlatformPlugin(false)
#endif //QT_NO_PHONON_PLATFORMPLUGIN
    , m_backendObject(0)
{
    // Add the post routine to make sure that all other global statics (especially the ones from Qt)
    // are still available. If the FactoryPrivate dtor is called too late many bad things can happen
    // as the whole backend might still be alive.
    qAddPostRoutine(globalFactory.destroy);
#ifndef QT_NO_DBUS
    QDBusConnection::sessionBus().connect(QString(), QString(), QLatin1String("org.kde.Phonon.Factory"),
        QLatin1String("phononBackendChanged"), this, SLOT(phononBackendChanged()));
#endif
}

FactoryPrivate::~FactoryPrivate()
{
    foreach (QObject *o, objects) {
        MediaObject *m = qobject_cast<MediaObject *>(o);
        if (m) {
            m->stop();
        }
    }
    foreach (MediaNodePrivate *bp, mediaNodePrivateList) {
        bp->deleteBackendObject();
    }
    if (objects.size() > 0) {
        pError() << "The backend objects are not deleted as was requested.";
        qDeleteAll(objects);
    }
    delete m_backendObject;
#ifndef QT_NO_PHONON_PLATFORMPLUGIN
    delete m_platformPlugin;
#endif //QT_NO_PHONON_PLATFORMPLUGIN
}

void FactoryPrivate::objectDescriptionChanged(ObjectDescriptionType type)
{
#ifdef PHONON_METHODTEST
    Q_UNUSED(type);
#else
    pDebug() << Q_FUNC_INFO << type;
    switch (type) {
    case AudioOutputDeviceType:
        emit availableAudioOutputDevicesChanged();
        break;
    case AudioCaptureDeviceType:
        emit availableAudioCaptureDevicesChanged();
        break;
    default:
        break;
    }
    //emit capabilitiesChanged();
#endif // PHONON_METHODTEST
}

Factory::Sender *Factory::sender()
{
    return globalFactory;
}

bool Factory::isMimeTypeAvailable(const QString &mimeType)
{
#ifndef QT_NO_PHONON_PLATFORMPLUGIN
    PlatformPlugin *f = globalFactory->platformPlugin();
    if (f) {
        return f->isMimeTypeAvailable(mimeType);
    }
#else
    Q_UNUSED(mimeType);
#endif //QT_NO_PHONON_PLATFORMPLUGIN
    return true; // the MIME type might be supported, let BackendCapabilities find out
}

void Factory::registerFrontendObject(MediaNodePrivate *bp)
{
    globalFactory->mediaNodePrivateList.prepend(bp); // inserted last => deleted first
}

void Factory::deregisterFrontendObject(MediaNodePrivate *bp)
{
    // The Factory can already be cleaned up while there are other frontend objects still alive.
    // When those are deleted they'll call deregisterFrontendObject through ~BasePrivate
    if (!globalFactory.isDestroyed()) {
        globalFactory->mediaNodePrivateList.removeAll(bp);
    }
}

#ifndef QT_NO_DBUS
void FactoryPrivate::phononBackendChanged()
{
    if (m_backendObject) {
        foreach (MediaNodePrivate *bp, mediaNodePrivateList) {
            bp->deleteBackendObject();
        }
        if (objects.size() > 0) {
            pDebug() << "WARNING: we were asked to change the backend but the application did\n"
                "not free all references to objects created by the factory. Therefore we can not\n"
                "change the backend without crashing. Now we have to wait for a restart to make\n"
                "backendswitching possible.";
            // in case there were objects deleted give 'em a chance to recreate
            // them now
            foreach (MediaNodePrivate *bp, mediaNodePrivateList) {
                bp->createBackendObject();
            }
            return;
        }
        delete m_backendObject;
        m_backendObject = 0;
    }
    createBackend();
    foreach (MediaNodePrivate *bp, mediaNodePrivateList) {
        bp->createBackendObject();
    }
    emit backendChanged();
}
#endif //QT_NO_DBUS

//X void Factory::freeSoundcardDevices()
//X {
//X     if (globalFactory->backend) {
//X         globalFactory->backend->freeSoundcardDevices();
//X     }
//X }

void FactoryPrivate::objectDestroyed(QObject * obj)
{
    //pDebug() << Q_FUNC_INFO << obj;
    objects.removeAll(obj);
}

#define FACTORY_IMPL(classname) \
QObject *Factory::create ## classname(QObject *parent) \
{ \
    if (backend()) { \
        return registerQObject(qobject_cast<BackendInterface *>(backend())->createObject(BackendInterface::classname##Class, parent)); \
    } \
    return 0; \
}
#define FACTORY_IMPL_1ARG(classname) \
QObject *Factory::create ## classname(int arg1, QObject *parent) \
{ \
    if (backend()) { \
        return registerQObject(qobject_cast<BackendInterface *>(backend())->createObject(BackendInterface::classname##Class, parent, QList<QVariant>() << arg1)); \
    } \
    return 0; \
}

FACTORY_IMPL(MediaObject)
#ifndef QT_NO_PHONON_EFFECT
FACTORY_IMPL_1ARG(Effect)
#endif //QT_NO_PHONON_EFFECT
#ifndef QT_NO_PHONON_VOLUMEFADEREFFECT
FACTORY_IMPL(VolumeFaderEffect)
#endif //QT_NO_PHONON_VOLUMEFADEREFFECT
FACTORY_IMPL(AudioOutput)
#ifndef QT_NO_PHONON_VIDEO
FACTORY_IMPL(VideoWidget)
#endif //QT_NO_PHONON_VIDEO

#undef FACTORY_IMPL

#ifndef QT_NO_PHONON_PLATFORMPLUGIN
PlatformPlugin *FactoryPrivate::platformPlugin()
{
    if (m_platformPlugin) {
        return m_platformPlugin;
    }
    if (m_noPlatformPlugin) {
        return 0;
    }
#ifndef QT_NO_DBUS
    if (!QCoreApplication::instance() || QCoreApplication::applicationName().isEmpty()) {
        pWarning() << "Phonon needs QCoreApplication::applicationName to be set to export audio output names through the DBUS interface";
    }
#endif
    Q_ASSERT(QCoreApplication::instance());
    const QByteArray platform_plugin_env = qgetenv("PHONON_PLATFORMPLUGIN");
    if (!platform_plugin_env.isEmpty()) {
        QPluginLoader pluginLoader(QString::fromLocal8Bit(platform_plugin_env.constData()));
        if (pluginLoader.load()) {
            m_platformPlugin = qobject_cast<PlatformPlugin *>(pluginLoader.instance());
            if (m_platformPlugin) {
                return m_platformPlugin;
            }
        }
    }
    const QString suffix(QLatin1String("/phonon_platform/"));
    ensureLibraryPathSet();
    QDir dir;
    dir.setNameFilters(
            !qgetenv("KDE_FULL_SESSION").isEmpty() ? QStringList(QLatin1String("kde.*")) :
            (!qgetenv("GNOME_DESKTOP_SESSION_ID").isEmpty() ? QStringList(QLatin1String("gnome.*")) :
             QStringList())
            );
    dir.setFilter(QDir::Files);
    forever {
        foreach (QString libPath, QCoreApplication::libraryPaths()) {
            libPath += suffix;
            dir.setPath(libPath);
            if (!dir.exists()) {
                continue;
            }
            foreach (const QString &pluginName, dir.entryList()) {
                QPluginLoader pluginLoader(libPath + pluginName);
                if (!pluginLoader.load()) {
                    pDebug() << Q_FUNC_INFO << "  platform plugin load failed:"
                        << pluginLoader.errorString();
                    continue;
                }
                pDebug() << pluginLoader.instance();
                QObject *qobj = pluginLoader.instance();
                m_platformPlugin = qobject_cast<PlatformPlugin *>(qobj);
                pDebug() << m_platformPlugin;
                if (m_platformPlugin) {
                    connect(qobj, SIGNAL(objectDescriptionChanged(ObjectDescriptionType)),
                            SLOT(objectDescriptionChanged(ObjectDescriptionType)));
                    return m_platformPlugin;
                } else {
                    delete qobj;
                    pDebug() << Q_FUNC_INFO << dir.absolutePath() << "exists but the platform plugin was not loadable:" << pluginLoader.errorString();
                    pluginLoader.unload();
                }
            }
        }
        if (dir.nameFilters().isEmpty()) {
            break;
        }
        dir.setNameFilters(QStringList());
    }
    pDebug() << Q_FUNC_INFO << "platform plugin could not be loaded";
    m_noPlatformPlugin = true;
    return 0;
}

PlatformPlugin *Factory::platformPlugin()
{
    return globalFactory->platformPlugin();
}
#endif // QT_NO_PHONON_PLATFORMPLUGIN

QObject *Factory::backend(bool createWhenNull)
{
    if (globalFactory.isDestroyed()) {
        return 0;
    }
    if (createWhenNull && globalFactory->m_backendObject == 0) {
        globalFactory->createBackend();
        // XXX: might create "reentrancy" problems:
        // a method calls this method and is called again because the
        // backendChanged signal is emitted
        emit globalFactory->backendChanged();
    }
    return globalFactory->m_backendObject;
}

#define GET_STRING_PROPERTY(name) \
QString Factory::name() \
{ \
    if (globalFactory->m_backendObject) { \
        return globalFactory->m_backendObject->property(#name).toString(); \
    } \
    return QString(); \
} \

GET_STRING_PROPERTY(identifier)
GET_STRING_PROPERTY(backendName)
GET_STRING_PROPERTY(backendComment)
GET_STRING_PROPERTY(backendVersion)
GET_STRING_PROPERTY(backendIcon)
GET_STRING_PROPERTY(backendWebsite)

QObject *Factory::registerQObject(QObject *o)
{
    if (o) {
        QObject::connect(o, SIGNAL(destroyed(QObject *)), globalFactory, SLOT(objectDestroyed(QObject *)), Qt::DirectConnection);
        globalFactory->objects.append(o);
    }
    return o;
}

} //namespace Phonon

QT_END_NAMESPACE

#include "factory.moc"
#include "moc_factory_p.cpp"

// vim: sw=4 ts=4
