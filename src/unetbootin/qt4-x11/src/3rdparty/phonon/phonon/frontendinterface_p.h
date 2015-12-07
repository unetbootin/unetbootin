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

#ifndef PHONON_FRONTENDINTERFACEPRIVATE_H
#define PHONON_FRONTENDINTERFACEPRIVATE_H

#include "addoninterface.h"
#include "mediaobject_p.h"
#include "phononnamespace_p.h"
#include <QtCore/QPointer>

QT_BEGIN_NAMESPACE

#ifndef QT_NO_PHONON_MEDIACONTROLLER

namespace Phonon
{
class FrontendInterfacePrivate
{
    public:
        FrontendInterfacePrivate(MediaObject *mp) : media(mp) {
            Q_ASSERT(media);
            MediaObjectPrivate *d = media->k_func();
            d->interfaceList << this;
        }
        virtual ~FrontendInterfacePrivate() {
            if (media) {
                MediaObjectPrivate *d = media->k_func();
                d->interfaceList << this;
            }
        }
        virtual void backendObjectChanged(QObject *iface) = 0;
        void _backendObjectChanged() {
            pDebug() << Q_FUNC_INFO;
            QObject *x = media->k_ptr->backendObject();
            if (x) {
                backendObjectChanged(x);
            }
        }
        AddonInterface *iface() { return qobject_cast<AddonInterface *>(media->k_ptr->backendObject()); }
        QPointer<MediaObject> media;
};
} // namespace Phonon

#endif //QT_NO_PHONON_MEDIACONTROLLER

QT_END_NAMESPACE

#endif // PHONON_FRONTENDINTERFACEPRIVATE_H
