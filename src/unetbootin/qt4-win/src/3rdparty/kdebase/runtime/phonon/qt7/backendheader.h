/*  This file is part of the KDE project.

    Copyright (C) 2007 Trolltech ASA. All rights reserved.

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

#ifndef Phonon_QT7_BACKENDHEADER_H
#define Phonon_QT7_BACKENDHEADER_H

#include <QString>
 
QT_BEGIN_NAMESPACE

namespace Phonon
{
namespace QT7
{

// Implemented in error.cpp:
void gSetErrorString(const QString &errorString);
QString gGetErrorString();
void gSetErrorLocation(const QString &errorLocation);
void gSetErrorType(int type);
int gGetErrorType();
void gClearError();

#define NO_ERROR 0
#define NORMAL_ERROR 1
#define FATAL_ERROR 2

#define ERROR_LOCATION                                              \
    QLatin1String("Function: ") + QLatin1String(__FUNCTION__)       \
    + QLatin1String(", File: ") + QLatin1String(__FILE__)           \
    + QLatin1String(", Line: ") + QString::number(__LINE__)

#define SET_ERROR(string, type){                                \
        Phonon::QT7::gSetErrorString(QLatin1String(string));    \
        Phonon::QT7::gSetErrorType(type);                       \
        Phonon::QT7::gSetErrorLocation(ERROR_LOCATION); }

#define BACKEND_ASSERT(test, string, type)      \
    bool fail = !test;                          \
    if (fail)                                   \
        SET_ERROR(QLatin1String(string), type)  \
    if (fail)

#define BACKEND_ASSERT2(test, string, type)     \
    if (!(test)) {                              \
        SET_ERROR(QLatin1String(string), type)  \
        return;                                 \
    }

#define BACKEND_ASSERT3(test, string, type, ret)    \
    if (!(test)) {                                  \
        SET_ERROR(QLatin1String(string), type)      \
        return ret;                                 \
    }

#define ARGUMENT_UNSUPPORTED(a, x, type, ret)                           \
    if ((a) == (x)) {                                                   \
        SET_ERROR("Argument value not supported: "#a" == "#x, type);    \
        return ret;                                                     \
    }

#define CASE_UNSUPPORTED(string, type) SET_ERROR(string, type)

#if SET_DEBUG_IMPLEMENTED
#define IMPLEMENTED qDebug() << "QT7:" << __FUNCTION__ << "(" << __FILE__ << "):"
#else
#define IMPLEMENTED if (1); else qDebug()
#endif

#if SET_DEBUG_HALF_IMPLEMENTED
#define HALF_IMPLEMENTED qDebug() << "QT7: --- HALF IMPLEMENTED:" << __FUNCTION__ << "(" << __FILE__ << "," << __LINE__ << "):"
#else
#define HALF_IMPLEMENTED if (1); else qDebug()
#endif

#ifdef SET_DEBUG_NOT_IMPLEMENTED
#define NOT_IMPLEMENTED qDebug() << "QT7: *** NOT IMPLEMENTED:" << __FUNCTION__ << "(" << __FILE__ << "," << __LINE__ << "):"
#else
#define NOT_IMPLEMENTED if (1); else qDebug()
#endif

#ifdef SET_DEBUG_IMPLEMENTED_SILENT
#define IMPLEMENTED_SILENT qDebug() << "QT7: (silent)" << __FUNCTION__ << "(" << __FILE__ << "," << __LINE__ << "):"
#else
#define IMPLEMENTED_SILENT if (1); else qDebug()
#endif

#ifdef SET_DEBUG_AUDIO_GRAPH
#define DEBUG_AUDIO_GRAPH(x) qDebug() << "QT7 DEBUG GRAPH:" << x;
#else
#define DEBUG_AUDIO_GRAPH(x) {}
#endif

#ifdef SET_DEBUG_AUDIO_STREAM
#define DEBUG_AUDIO_STREAM(x) qDebug() << "QT7 DEBUG STREAM:" << x;
#else
#define DEBUG_AUDIO_STREAM(x) {}
#endif

}} //namespace Phonon::QT7

QT_END_NAMESPACE

#ifdef Q_CC_INTEL
#pragma warning (disable : 1899) // mute icc warning for the use of 4cc
#endif

#endif // Phonon_QT7_BACKENDHEADER_H
