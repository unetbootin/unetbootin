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

#ifndef Phonon_GSTREAMER_AUDIOEFFECT_H
#define Phonon_GSTREAMER_AUDIOEFFECT_H

#include "common.h"
#include "effect.h"
#include "medianode.h"

#include <phonon/effectparameter.h>
#include <phonon/effectinterface.h>

#include <QtCore/QObject>

#include <gst/gst.h>

QT_BEGIN_NAMESPACE

namespace Phonon
{
namespace Gstreamer
{
    class AudioOutput;
    class EffectInfo;

    class AudioEffect : public Effect
    {
        Q_OBJECT
        public:
            AudioEffect (Backend *backend, int effectId, QObject *parent);
        protected:
            GstElement* createEffectBin();
            GstElement* audioElement() { return m_effectBin; }
            QString m_effectName;
    };
}} //namespace Phonon::Gstreamer

QT_END_NAMESPACE

#endif // Phonon_GSTREAMER_AUDIOEFFECT_H
