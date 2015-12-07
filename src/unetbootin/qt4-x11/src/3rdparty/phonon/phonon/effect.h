/*  This file is part of the KDE project
    Copyright (C) 2005-2007 Matthias Kretz <kretz@kde.org>

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


#ifndef PHONON_EFFECT_H
#define PHONON_EFFECT_H

#include "phonondefs.h"
#include <QtCore/QObject>
#include "objectdescription.h"
#include "medianode.h"

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

#ifndef QT_NO_PHONON_EFFECT

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
             * type of effect. See BackendCapabilities::availableAudioEffects().
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

#endif // QT_NO_EFFECT

QT_END_NAMESPACE
QT_END_HEADER

// vim: sw=4 ts=4 tw=80
#endif // PHONON_EFFECT_H

