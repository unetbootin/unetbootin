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

#ifndef Phonon_GSTREAMER_COMMON_H
#define Phonon_GSTREAMER_COMMON_H

#include <phonon/phonon_export.h>

#include <QtCore/QDebug>

#ifdef DEBUG_IMPLEMENTED
#define IMPLEMENTED qDebug() << "PGst:" << __FUNCTION__ << "(" << __FILE__ << "):"
#else
#define IMPLEMENTED if (1); else qDebug()
#endif

#ifdef DEBUG_HALF_IMPLEMENTED
#define HALF_IMPLEMENTED qDebug() << "PGst: --- HALF IMPLEMENTED:" << __FUNCTION__ << "(" << __FILE__ << "):"
#else
#define HALF_IMPLEMENTED if (1); else qDebug()
#endif

#ifdef DEBUG_NOT_IMPLEMENTED
#define NOT_IMPLEMENTED qDebug() << "PGst: *** NOT IMPLEMENTED:" << __FUNCTION__ << "(" << __FILE__ << "):"
#else
#define NOT_IMPLEMENTED if (1); else qDebug()
#endif

#ifdef DEBUG_IMPLEMENTED_SILENT
#define IMPLEMENTED_SILENT qDebug() << "PGst: (silent)" << __FUNCTION__ << "(" << __FILE__ << "):"
#else
#define IMPLEMENTED_SILENT if (1); else qDebug()
#endif

#define BACKEND_WARNING qDebug() << "PGst: ____ WARNING ____" << Q_FUNC_INFO << ":"

#endif // Phonon_GSTREAMER_COMMON_H
