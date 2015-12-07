/*  This file is part of the KDE project
    Copyright (C) 2006 Matthias Kretz <kretz@kde.org>

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

#ifndef PHONON_EFFECTPARAMETER_H
#define PHONON_EFFECTPARAMETER_H

#include "phonon_export.h"

#include <QtCore/QExplicitlySharedDataPointer>
#include <QtCore/QVariant>

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

#ifndef QT_NO_PHONON_EFFECT

namespace Phonon
{

class Effect;
class EffectParameterPrivate;

/** \class EffectParameter effectparameter.h Phonon/EffectParameter
 * \brief This class describes one parameter of an effect.
 *
 * \ingroup PhononEffects
 * \author Matthias Kretz <kretz@kde.org>
 * \see Effect
 */
class PHONON_EXPORT EffectParameter
{
    friend class BrightnessControl;
    public:
        /**
         * \internal
         *
         * Creates an invalid effect parameter.
         */
        EffectParameter();

        /**
         * The name of the parameter. Can be used as the label.
         *
         * \return A label for the parameter.
         */
        const QString &name() const;

        /**
         * The parameter may come with a description (LADSPA doesn't have a
         * field for this, so don't expect many effects to provide a
         * description).
         *
         * The description can be used for a tooltip or WhatsThis help.
         *
         * \return A text describing the parameter.
         */
        const QString &description() const;

        /**
         * Returns the parameter type.
         *
         * Common types are QVariant::Int, QVariant::Double, QVariant::Bool and QVariant::String. When
         * QVariant::String is returned you get the possible values from possibleValues.
         */
        QVariant::Type type() const;

        /**
         * Returns whether the parameter should be
         * displayed using a logarithmic scale. This is particularly useful for
         * frequencies and gains.
         */
        bool isLogarithmicControl() const;

        /**
         * The minimum value to be used for the control to edit the parameter.
         *
         * If the returned QVariant is invalid the value is not bounded from
         * below.
         */
        QVariant minimumValue() const;

        /**
         * The maximum value to be used for the control to edit the parameter.
         *
         * If the returned QVariant is invalid the value is not bounded from
         * above.
         */
        QVariant maximumValue() const;

        /**
         * The default value.
         */
        QVariant defaultValue() const;

        /**
         * The possible values to be used for the control to edit the parameter.
         *
         * if the value of this parameter is to be picked from predefined values
         * this returns the list (otherwise it returns an empty QVariantList).
         */
        QVariantList possibleValues() const;

        /**
         * \internal
         * compares the ids of the parameters
         */
        bool operator<(const EffectParameter &rhs) const;

        /**
         * \internal
         * compares the ids of the parameters
         */
        bool operator>(const EffectParameter &rhs) const;

        /**
         * \internal
         * compares the ids of the parameters
         */
        bool operator==(const EffectParameter &rhs) const;

        /* dtor, cctor and operator= for forward decl of EffectParameterPrivate */
        ~EffectParameter();
        EffectParameter(const EffectParameter &rhs);
        EffectParameter &operator=(const EffectParameter &rhs);

        /**
         * Only for backend developers:
         *
         * Flags to set the return values of isToggleControl(),
         * isLogarithmicControl(), isIntegerControl(), isBoundedBelow() and
         * isBoundedAbove(). The values of the flags correspond to the values
         * used for LADSPA effects.
         */
        enum Hint {
            /**
             * If this hint is set it means that
             * the the control has only two states: zero and non-zero.
             *
             * \see isToggleControl()
             */
            ToggledHint      = 0x04,

            /* LADSPA's SAMPLE_RATE hint needs to be translated by the backend
             * to normal bounds, as the backend knows the sample rate - and the
             * frontend doesn't */

            /**
             * \see isLogarithmicControl()
             */
            LogarithmicHint  = 0x10,
            /**
             * \see isIntegerControl
             */
            IntegerHint      = 0x20
        };
        Q_DECLARE_FLAGS(Hints, Hint)

        /**
         * Only to be used by backend implementations:
         *
         * Creates a new effect parameter.
         *
         * \param parameterId This is a number to uniquely identify the
         * parameter. The id is used for value() and setValue().
         *
         * \param name The name/label for this parameter.
         *
         * \param hints Sets the hints for the type of parameter.
         *
         * \param defaultValue The value that should be used as a default.
         *
         * \param min The minimum value allowed for this parameter. You only
         * need to set this if the BoundedBelowHint is set.
         *
         * \param max The maximum value allowed for this parameter. You only
         * need to set this if the BoundedAboveHint is set.
         *
         * \param description A descriptive text for the parameter
         * (explaining what it controls) to be used as a tooltip or
         * WhatsThis help.
         */
        EffectParameter(int parameterId, const QString &name, Hints hints,
                const QVariant &defaultValue, const QVariant &min = QVariant(),
                const QVariant &max = QVariant(), const QVariantList &values = QVariantList(),
                const QString &description = QString());

        /**
         * \internal
         *
         * Returns the parameter's id.
         */
        int id() const;

    protected:
        /**
         * The data is implicitly shared.
         */
        QExplicitlySharedDataPointer<EffectParameterPrivate> d;
};

uint PHONON_EXPORT qHash(const Phonon::EffectParameter &param);

} // namespace Phonon

#if defined(Q_CC_MSVC) && _MSC_VER <= 1300
//this ensures that code outside Phonon can use the hash function
//it also a workaround for some compilers
inline uint qHash(const Phonon::EffectParameter &param) { return Phonon::qHash(param); } //krazy:exclude=inline
#endif
Q_DECLARE_OPERATORS_FOR_FLAGS(Phonon::EffectParameter::Hints)

#endif //QT_NO_PHONON_EFFECT

QT_END_NAMESPACE
QT_END_HEADER

#endif // PHONON_EFFECTPARAMETER_H
// vim: sw=4 ts=4 tw=80
