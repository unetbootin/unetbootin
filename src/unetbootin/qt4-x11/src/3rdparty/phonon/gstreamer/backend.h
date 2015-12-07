/*  This file is part of the KDE project.

    Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).

    This library is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 2.1 or 3 of the License.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef Phonon_GSTREAMER_BACKEND_H
#define Phonon_GSTREAMER_BACKEND_H

#include "common.h"
#include "devicemanager.h"
#include "medianode.h"

#include <phonon/objectdescription.h>
#include <phonon/backendinterface.h>

#include <QtCore/QList>
#include <QtCore/QPointer>
#include <QtCore/QStringList>
#include <QtCore/QTimer>

#include <gst/gst.h>

QT_BEGIN_NAMESPACE

namespace Phonon
{
namespace Gstreamer
{
class AudioOutput;
class MediaNode;
class MediaObject;
class EffectManager;

class Backend : public QObject, public BackendInterface
{
    Q_OBJECT
    Q_INTERFACES(Phonon::BackendInterface)

public:

    enum DebugLevel {NoDebug, Warning, Info, Debug};
    Backend(QObject *parent = 0, const QVariantList & = QVariantList());
    virtual ~Backend();

    DeviceManager* deviceManager() const;
    EffectManager* effectManager() const;

    QObject *createObject(BackendInterface::Class, QObject *parent, const QList<QVariant> &args);

    bool isValid() const;
    bool supportsVideo() const;
    QStringList availableMimeTypes() const;

    QList<int> objectDescriptionIndexes(ObjectDescriptionType type) const;
    QHash<QByteArray, QVariant> objectDescriptionProperties(ObjectDescriptionType type, int index) const;

    bool startConnectionChange(QSet<QObject *>);
    bool connectNodes(QObject *, QObject *);
    bool disconnectNodes(QObject *, QObject *);
    bool endConnectionChange(QSet<QObject *>);

    DebugLevel debugLevel() const;

    void addBusWatcher(MediaObject* node);
    void removeBusWatcher(MediaObject* node);
    void logMessage(const QString &message, int priority = 2, QObject *obj=0) const;
    bool checkDependencies() const;

Q_SIGNALS:
    void objectDescriptionChanged(ObjectDescriptionType);

private Q_SLOTS:
    void handleBusMessage(Message);

private:
    static gboolean busCall(GstBus *bus, GstMessage *msg, gpointer data);
    QList<QPointer<AudioOutput> > m_audioOutputs;

    DeviceManager *m_deviceManager;
    EffectManager *m_effectManager;
    DebugLevel m_debugLevel;
    bool m_isValid;
};
}
} // namespace Phonon::Gstreamer

QT_END_NAMESPACE

#endif // Phonon_GSTREAMER_BACKEND_H
