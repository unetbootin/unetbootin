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

#include "backendheader.h"
#include <QString>
#include <QDebug>

#include <CoreFoundation/CoreFoundation.h>
#include <QVarLengthArray>

QT_BEGIN_NAMESPACE

namespace Phonon
{
namespace QT7
{

Q_GLOBAL_STATIC(QString, gErrorString)
int gErrorType = NO_ERROR;

void gSetErrorString(const QString &errorString)
{
    if (qgetenv("PHONON_DEBUG") == "1"){
        qDebug() << "Error:" << errorString;
    }

    if (!gErrorString()->isEmpty())
        return; // not yet caught.
        
    *gErrorString() = errorString;   
}

QString gGetErrorString()
{
    return *gErrorString();
}

void gSetErrorLocation(const QString &errorLocation)
{
    if (qgetenv("PHONON_DEBUG") == "1"){
        qDebug() << "Location:" << errorLocation;
    }
}

void gSetErrorType(int errorType)
{
    if (gErrorType != NO_ERROR)
        return; // not yet caught.
    gErrorType = errorType;
}

int gGetErrorType()
{
    return gErrorType;
}

void gClearError()
{
    gErrorString()->clear();
    gErrorType = NO_ERROR;
}

/////////////////////////////////////////////////////////////////////////////////////////

PhononAutoReleasePool::PhononAutoReleasePool()
{
    pool = (void*)[[NSAutoreleasePool alloc] init];
}

PhononAutoReleasePool::~PhononAutoReleasePool()
{
    [(NSAutoreleasePool*)pool release];
}

/////////////////////////////////////////////////////////////////////////////////////////

QString PhononCFString::toQString(CFStringRef str)
{
    if(!str)
        return QString();
    CFIndex length = CFStringGetLength(str);
    const UniChar *chars = CFStringGetCharactersPtr(str);
    if (chars)
        return QString(reinterpret_cast<const QChar *>(chars), length);

    QVarLengthArray<UniChar> buffer(length);
    CFStringGetCharacters(str, CFRangeMake(0, length), buffer.data());
    return QString(reinterpret_cast<const QChar *>(buffer.constData()), length);
}

PhononCFString::operator QString() const
{
    if (string.isEmpty() && type)
        const_cast<PhononCFString*>(this)->string = toQString(type);
    return string;
}

CFStringRef PhononCFString::toCFStringRef(const QString &string)
{
    return CFStringCreateWithCharacters(0, reinterpret_cast<const UniChar *>(string.unicode()),
                                        string.length());
}

PhononCFString::operator CFStringRef() const
{
    if (!type)
        const_cast<PhononCFString*>(this)->type = toCFStringRef(string);
    return type;
}

}}

QT_END_NAMESPACE
