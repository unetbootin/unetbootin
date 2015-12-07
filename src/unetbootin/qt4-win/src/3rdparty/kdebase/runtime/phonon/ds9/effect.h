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

#ifndef PHONON_AUDIOEFFECT_H
#define PHONON_AUDIOEFFECT_H

#include <QtCore/QObject>
#include <phonon/effectinterface.h>
#include "backendnode.h"

QT_BEGIN_NAMESPACE

namespace Phonon
{
    namespace DS9
    {
        class EffectInterface;

        class Effect : public BackendNode, public Phonon::EffectInterface
        {
            Q_OBJECT
                Q_INTERFACES(Phonon::EffectInterface)
        public:
            Effect(CLSID effectClass, QObject *parent);
            ~Effect();

            QList<Phonon::EffectParameter> parameters() const;
            QVariant parameterValue(const Phonon::EffectParameter &) const;
            void setParameterValue(const Phonon::EffectParameter &, const QVariant &);


        protected:
            //this is the constructor called by the explicit subclasses of effect
            Effect(QObject *parent);
        };
    }
}

QT_END_NAMESPACE

#endif // PHONON_AUDIOEFFECT_H
