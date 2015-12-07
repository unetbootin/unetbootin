/*  This file is part of the KDE project
Copyright (C) 2007 Matthias Kretz <kretz@kde.org>

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

#ifndef PHONON_MEDIANODE_P_H
#define PHONON_MEDIANODE_P_H

#include <QtCore/QtGlobal>
#include <QtCore/QList>
#include <QtCore/QObject>
#include "path.h"
#include "phonon_export.h"

QT_BEGIN_NAMESPACE

class QObject;

namespace Phonon
{
    class MediaNode;
    class MediaNodeDestructionHandler;

    class PHONON_EXPORT MediaNodePrivate
    {
        Q_DECLARE_PUBLIC(MediaNode)

        friend class AudioOutputPrivate;
        friend class FactoryPrivate;

    protected:
        enum CastId {
            MediaNodePrivateType,
            AbstractAudioOutputPrivateType,
            AudioOutputType
        };
    public:
        /**
        * Returns the backend object. If the object does not exist it tries to
        * create it before returning.
        *
        * \return the Iface object, might return \c 0
        */
        QObject *backendObject();

        const CastId castId;

    protected:
        MediaNodePrivate(CastId _castId = MediaNodePrivateType);

        virtual ~MediaNodePrivate();

        /**
        * \internal
        * This method cleanly deletes the Iface object. It is called on
        * destruction and before a backend change.
        */
        void deleteBackendObject();

        virtual bool aboutToDeleteBackendObject() = 0;

        /**
        * \internal
        * Creates the Iface object belonging to this class. For most cases the
        * implementation is
        * \code
        * Q_Q(ClassName);
        * m_iface = Factory::createClassName(this);
        * return m_iface;
        * \endcode
        *
        * This function should not be called except from slotCreateIface.
        *
        * \see slotCreateIface
        */
        virtual void createBackendObject() = 0;

    public:
        /**
        * \internal
        * This class has its own destroyed signal since some cleanup calls
        * need the pointer to the backend object intact. The
        * QObject::destroyed signals comes after the backend object was
        * deleted.
        *
        * As this class cannot derive from QObject a simple handler
        * interface is used.
        */
        void addDestructionHandler(MediaNodeDestructionHandler *handler);

        /**
        * \internal
        * This class has its own destroyed signal since some cleanup calls
        * need the pointer to the backend object intact. The
        * QObject::destroyed signals comes after the backend object was
        * deleted.
        *
        * As this class cannot derive from QObject a simple handler
        * interface is used.
        */
        void removeDestructionHandler(MediaNodeDestructionHandler *handler);

        void addOutputPath(const Path &);
        void addInputPath(const Path &);
        void removeOutputPath(const Path &);
        void removeInputPath(const Path &);

        const QObject *qObject() const { return const_cast<MediaNodePrivate *>(this)->qObject(); }
        virtual QObject *qObject() { return 0; }

    protected:
        MediaNode *q_ptr;
    public:
        QObject *m_backendObject;
    protected:
        QList<Path> outputPaths;
        QList<Path> inputPaths;

    private:
        QList<MediaNodeDestructionHandler *> handlers;
        Q_DISABLE_COPY(MediaNodePrivate)
    };

} // namespace Phonon

QT_END_NAMESPACE

#endif // PHONON_MEDIANODE_P_H
