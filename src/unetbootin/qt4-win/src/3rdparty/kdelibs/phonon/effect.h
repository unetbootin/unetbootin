/*  This file is part of the KDE project
    Copyright (C) 2005-2007 Matthias Kretz <kretz@kde.org>

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
#ifndef PHONON_EFFECT_H
#define PHONON_EFFECT_H

#include "phonon/phonondefs.h"
#include <QtCore/QObject>
#include "phonon/objectdescription.h"
#include "phonon/medianode.h"

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

class QString;
template<class T> class QList;

namespace Phonon
{
    class EffectParameter;
    class EffectPrivate;

    /** \class Effect effect.h Phonon/Effect
     * \short Effects that can be inserted into a Path.
     * An effect is a special object which can perform
     * transformations on the specified path. Examples may include simple
     * modifiers such as fading or pitch shifting, or more complex mathematical
     * transformations.
     *
     * In order to use an effect, insert it into the path as follows:
     * \code
     * Path path = Phonon::createPath(...);
     * Effect *effect = new Effect(this);
     * path.insertEffect(effect);
     * \endcode
     *
     * The effect will immediately begin applying it's transformations on
     * the path. To stop it, remove the Effect from the path.
     *
     * \ingroup PhononEffects
     * \author Matthias Kretz <kretz@kde.org>
     */
    class PHONON_EXPORT Effect : public QObject, public MediaNode
    {
        Q_OBJECT
        K_DECLARE_PRIVATE(Effect)

        public:
            ~Effect();

//X             enum Type {
//X                 AudioEffect,
//X                 VideoEffect
//X             };

            /**
             * QObject constructor.
             *
             * \param description An EffectDescription object to determine the
             * type of effect. See \ref
             * BackendCapabilities::availableAudioEffects().
             * \param parent QObject parent
             */
            explicit Effect(const EffectDescription &description, QObject *parent = 0);

//X             Type type() const;

            /**
             * Returns the description of this effect. This is the same type as was
             * passed to the constructor.
             */
            EffectDescription description() const;

            /**
             * Returns a list of parameters that this effect provides to control
             * its behaviour.
             *
             * \see EffectParameter
             * \see EffectWidget
             */
            QList<EffectParameter> parameters() const;

            QVariant parameterValue(const EffectParameter&) const;
            void setParameterValue(const EffectParameter&, const QVariant &value);

        protected:
            Effect(EffectPrivate &dd, QObject *parent);
    };
} //namespace Phonon

QT_END_NAMESPACE
QT_END_HEADER

// vim: sw=4 ts=4 tw=80
#endif // PHONON_EFFECT_H
