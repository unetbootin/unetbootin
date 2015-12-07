/*  This file is part of the KDE project
    Copyright (C) 2007 Matthias Kretz <kretz@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef PHONON_EXPORT_H
#define PHONON_EXPORT_H

#include <QtCore/QtGlobal>

#ifndef PHONON_EXPORT
# if defined _WIN32 || defined _WIN64
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
