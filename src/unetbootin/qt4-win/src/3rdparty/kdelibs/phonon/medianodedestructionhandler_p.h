/*  This file is part of the KDE project
    Copyright (C) 2006 Matthias Kretz <kretz@kde.org>

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

#ifndef MEDIANODEDESTRUCTIONHANDLER_P_H
#define MEDIANODEDESTRUCTIONHANDLER_P_H

#include <QtCore/qglobal.h>

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

namespace Phonon
{
/**
 * \internal
 *
 * Callback interface to keep track of Phonon frontend object destruction.
 *
 * \author Matthias Kretz <kretz@kde.org>
 */

class MediaNodePrivate;

class MediaNodeDestructionHandler
{
    friend class MediaNodePrivate;

public:
    virtual ~MediaNodeDestructionHandler() {}
protected:
    /**
     * \internal
     * called from Base::~Base if this object was registered
     * using BasePrivate::addDestructionHandler().
     */
    virtual void phononObjectDestroyed(MediaNodePrivate *) = 0;
};
}

QT_END_NAMESPACE
QT_END_HEADER

#endif // MEDIANODEDESTRUCTIONHANDLER_P_H
