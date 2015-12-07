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

#ifndef PHONON_EXPORT_H
#define PHONON_EXPORT_H

#include <QtCore/QtGlobal>

#ifndef PHONON_EXPORT
# if defined Q_WS_WIN
#  ifdef MAKE_PHONON_LIB /* We are building this library */
#   define PHONON_EXPORT Q_DECL_EXPORT
#  else /* We are using this library */
#   define PHONON_EXPORT Q_DECL_IMPORT
#  endif
# else /* UNIX */
#  define PHONON_EXPORT Q_DECL_EXPORT
# endif
#endif

#ifndef PHONON_EXPORT_DEPRECATED
# define PHONON_EXPORT_DEPRECATED Q_DECL_DEPRECATED PHONON_EXPORT
#endif

// QT_(BEGIN|END)_NAMESPACE appeared in 4.4
#ifndef QT_BEGIN_NAMESPACE
#  define QT_BEGIN_NAMESPACE
#endif
#ifndef QT_END_NAMESPACE
#  define QT_END_NAMESPACE
#endif

// silence syncqt
QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

QT_END_NAMESPACE
QT_END_HEADER

#endif
