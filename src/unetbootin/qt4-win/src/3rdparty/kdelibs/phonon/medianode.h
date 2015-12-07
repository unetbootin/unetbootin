/*  This file is part of the KDE project
    Copyright (C) 2007 Matthias Kretz <kretz@kde.org>
    Copyright (C) 2007 Trolltech ASA. <thierry.bastian@trolltech.com>

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

#ifndef PHONON_MEDIANODE_H
#define PHONON_MEDIANODE_H

#include "phonon/phonondefs.h"
#include "phonon/phonon_export.h"
#include "phonon/path.h"

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

namespace Phonon
{

    class Path;
    class MediaNodePrivate;
    class PHONON_EXPORT MediaNode
    {
        friend class Path;
        friend class PathPrivate;
        friend PHONON_EXPORT Path createPath(MediaNode *source, MediaNode *sink);
        K_DECLARE_PRIVATE(MediaNode)
    public:
        virtual ~MediaNode();
        /**
         * Tells whether the backend provides an implementation of this
         * class.
         *
         * \return \c true if backend provides an implementation
         * \return \c false if the object is not implemented by the backend
         */
        bool isValid() const;

        QList<Path> inputPaths() const;
        QList<Path> outputPaths() const;

    protected:
        MediaNode(MediaNodePrivate &dd);
        MediaNodePrivate *const k_ptr;
    };

} // namespace Phonon

QT_END_NAMESPACE
QT_END_HEADER

#endif // PHONON_MEDIANODE_H
