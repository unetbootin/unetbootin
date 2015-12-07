/*  This file is part of the KDE project
Copyright (C) 2005-2006 Matthias Kretz <kretz@kde.org>

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
#ifndef Phonon_ABSTRACTAUDIOOUTPUTBASE_H
#define Phonon_ABSTRACTAUDIOOUTPUTBASE_H

#include "phonon/phonondefs.h"
#include "phonon/phonon_export.h"
#include "phonon/medianode.h"
#include <QtCore/QObject>

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

namespace Phonon
{
    class AbstractAudioOutputPrivate;

    /** \class AbstractAudioOutput abstractaudiooutput.h Phonon/AbstractAudioOutput
    * Common base class for all audio outputs.
    *
    * \see AudioOutput
    */
    class PHONON_EXPORT AbstractAudioOutput : public QObject, public MediaNode
    {
        Q_OBJECT
            K_DECLARE_PRIVATE(AbstractAudioOutput)
    protected:
        AbstractAudioOutput(AbstractAudioOutputPrivate &dd, QObject *parent);
    public:
        ~AbstractAudioOutput();
    };
} //namespace Phonon

QT_END_NAMESPACE
QT_END_HEADER

// vim: sw=4 ts=4 tw=80
#endif // Phonon_ABSTRACTAUDIOOUTPUTBASE_H
