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

#ifndef PHONON_BACKEND_H
#define PHONON_BACKEND_H

#include <phonon/backendinterface.h>
#include <phonon/phononnamespace.h>

#include <QtCore/QList>



QT_BEGIN_NAMESPACE

namespace Phonon
{
    namespace WaveOut
    {
        class AudioOutput;
        class MediaObject;

        class Backend : public QObject, public Phonon::BackendInterface
        {
            Q_OBJECT
            Q_INTERFACES(Phonon::BackendInterface)
        public:
            Backend(QObject *parent = 0, const QVariantList & = QVariantList());
            virtual ~Backend();

            QObject *createObject(Phonon::BackendInterface::Class, QObject *parent, const QList<QVariant> &args);

            bool supportsVideo() const;
            QStringList availableMimeTypes() const;

            QList<int> objectDescriptionIndexes(Phonon::ObjectDescriptionType type) const;
            QHash<QByteArray, QVariant> objectDescriptionProperties(Phonon::ObjectDescriptionType type, int index) const;

            bool connectNodes(QObject *, QObject *);
            bool disconnectNodes(QObject *, QObject *);

            //transaction management
            bool startConnectionChange(QSet<QObject *>);
            bool endConnectionChange(QSet<QObject *>);

        Q_SIGNALS:
            void objectDescriptionChanged(ObjectDescriptionType);

        };
    }
}

QT_END_NAMESPACE

#endif // PHONON_BACKEND_H
