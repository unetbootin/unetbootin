/*  This file is part of the KDE project
Copyright (C) 2007 Matthias Kretz <kretz@kde.org>

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License version 2 as published by the Free Software Foundation.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public License
along with this library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
Boston, MA 02110-1301, USA.

*/

#ifndef PHONON_MEDIANODE_P_H
#define PHONON_MEDIANODE_P_H

#include <qglobal.h>
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
        virtual QObject *qObject() = 0;

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
