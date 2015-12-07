/*  This file is part of the KDE project
    Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies). <thierry.bastian@trolltech.com>

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

#ifndef PHONON_VOLUMEFADERINTERFACE_H
#define PHONON_VOLUMEFADERINTERFACE_H

#include "volumefadereffect.h"
#include <QtCore/QObject>

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

#ifndef QT_NO_PHONON_VOLUMEFADEREFFECT

namespace Phonon
{
class VolumeFaderInterface
{
    public:
        virtual ~VolumeFaderInterface() {}

        virtual float volume() const { return 1.0; }
        virtual void setVolume(float) {}
        virtual Phonon::VolumeFaderEffect::FadeCurve fadeCurve() const {
            return VolumeFaderEffect::Fade3Decibel;
        }
        virtual void setFadeCurve(Phonon::VolumeFaderEffect::FadeCurve) {}
        virtual void fadeTo(float, int) {}
};
}

Q_DECLARE_INTERFACE(Phonon::VolumeFaderInterface, "VolumeFaderInterface4.phonon.kde.org")

#endif //QT_NO_PHONON_VOLUMEFADEREFFECT

QT_END_NAMESPACE
QT_END_HEADER

#endif // PHONON_VOLUMEFADERINTERFACE_H
