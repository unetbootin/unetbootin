/*  This file is part of the KDE project
    Copyright (C) 2006-2007 Matthias Kretz <kretz@kde.org>

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

#ifndef VOLUMESLIDER_P_H
#define VOLUMESLIDER_P_H

#include "volumeslider.h"
#include <QtGui/QBoxLayout>
#include <QtGui/QSlider>
#include <QtGui/QLabel>
#include <QtGui/QPixmap>
#include <QtGui/QToolButton>
#include "factory_p.h"
#include "audiooutput.h"
#include <QtGui/QIcon>
#include <QtCore/QPointer>
#include "platform_p.h"

QT_BEGIN_NAMESPACE

#ifndef QT_NO_PHONON_VOLUMESLIDER

namespace Phonon
{
class VolumeSliderPrivate
{
    Q_DECLARE_PUBLIC(VolumeSlider)
    protected:
        VolumeSliderPrivate(VolumeSlider *parent)
            : q_ptr(parent),
            layout(QBoxLayout::LeftToRight, parent),
            slider(Qt::Horizontal, parent),
            muteButton(parent),
            volumeIcon(Platform::icon(QLatin1String("player-volume"), parent->style())),
            mutedIcon(Platform::icon(QLatin1String("player-volume-muted"), parent->style())),
            output(0),
            ignoreVolumeChange(false)
        {
            slider.setRange(0, 100);
            slider.setPageStep(5);
            slider.setSingleStep(1);

            muteButton.setIcon(volumeIcon);
            muteButton.setAutoRaise(true);
            layout.setMargin(0);
            layout.setSpacing(2);
            layout.addWidget(&muteButton, 0, Qt::AlignVCenter);
            layout.addWidget(&slider, 0, Qt::AlignVCenter);

            slider.setEnabled(false);
            muteButton.setEnabled(false);

            if (volumeIcon.isNull()) {
                muteButton.setVisible(false);
            }
        }

        VolumeSlider *q_ptr;

        void _k_sliderChanged(int);
        void _k_volumeChanged(qreal);
        void _k_mutedChanged(bool);
        void _k_buttonClicked();

    private:
        QBoxLayout layout;
        QSlider slider;
        QToolButton muteButton;
        QIcon volumeIcon;
        QIcon mutedIcon;

        QPointer<AudioOutput> output;
        bool ignoreVolumeChange;
};
} // namespace Phonon

#endif //QT_NO_PHONON_VOLUMESLIDER

QT_END_NAMESPACE

#endif // VOLUMESLIDER_P_H
// vim: sw=4 sts=4 et tw=100
