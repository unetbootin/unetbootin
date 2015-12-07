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

#include "volumeslider.h"
#include "volumeslider_p.h"
#include "audiooutput.h"
#include "phonondefs_p.h"
#include "phononnamespace_p.h"
#include "factory_p.h"

QT_BEGIN_NAMESPACE

#ifndef QT_NO_PHONON_VOLUMESLIDER

namespace Phonon
{
VolumeSlider::VolumeSlider(QWidget *parent)
    : QWidget(parent),
    k_ptr(new VolumeSliderPrivate(this))
{
    K_D(VolumeSlider);
#ifndef QT_NO_TOOLTIP
    setToolTip(tr("Volume: %1%").arg(100));
#endif
#ifndef QT_NO_WHATSTHIS
    setWhatsThis(tr("Use this slider to adjust the volume. The leftmost position is 0%, the rightmost is %1%").arg(100));
#endif

    connect(&d->slider, SIGNAL(valueChanged(int)), SLOT(_k_sliderChanged(int)));
    connect(&d->muteButton, SIGNAL(clicked()), SLOT(_k_buttonClicked()));

    setFocusProxy(&d->slider);
}

VolumeSlider::VolumeSlider(AudioOutput *output, QWidget *parent)
    : QWidget(parent),
    k_ptr(new VolumeSliderPrivate(this))
{
    K_D(VolumeSlider);
#ifndef QT_NO_TOOLTIP
    setToolTip(tr("Volume: %1%").arg(100));
#endif
#ifndef QT_NO_WHATSTHIS
    setWhatsThis(tr("Use this slider to adjust the volume. The leftmost position is 0%, the rightmost is %1%").arg(100));
#endif

    connect(&d->slider, SIGNAL(valueChanged(int)), SLOT(_k_sliderChanged(int)));
    connect(&d->muteButton, SIGNAL(clicked()), SLOT(_k_buttonClicked()));

    if (output) {
        d->output = output;
        d->slider.setValue(qRound(100 * output->volume()));
        d->slider.setEnabled(true);
        d->muteButton.setEnabled(true);
        connect(output, SIGNAL(volumeChanged(qreal)), SLOT(_k_volumeChanged(qreal)));
        connect(output, SIGNAL(mutedChanged(bool)), SLOT(_k_mutedChanged(bool)));
    }

    setFocusProxy(&d->slider);
}

VolumeSlider::~VolumeSlider()
{
    delete k_ptr;
}

bool VolumeSlider::isMuteVisible() const
{
    return k_ptr->muteButton.isVisible();
}

void VolumeSlider::setMuteVisible(bool visible)
{
    k_ptr->muteButton.setVisible(visible);
}

QSize VolumeSlider::iconSize() const
{
    return k_ptr->muteButton.iconSize();
}

void VolumeSlider::setIconSize(const QSize &iconSize)
{
    pDebug() << Q_FUNC_INFO << iconSize;
    k_ptr->muteButton.setIconSize(iconSize);
}

qreal VolumeSlider::maximumVolume() const
{
    return k_ptr->slider.maximum() * 0.01;
}

void VolumeSlider::setMaximumVolume(qreal volume)
{
    int max = static_cast<int>(volume * 100);
    k_ptr->slider.setMaximum(max);
#ifndef QT_NO_WHATSTHIS
    setWhatsThis(tr("Use this slider to adjust the volume. The leftmost position is 0%, the rightmost is %1%")
            .arg(max));
#endif
}

Qt::Orientation VolumeSlider::orientation() const
{
    return k_ptr->slider.orientation();
}

void VolumeSlider::setOrientation(Qt::Orientation o)
{
    K_D(VolumeSlider);
    Qt::Alignment align = (o == Qt::Horizontal ? Qt::AlignVCenter : Qt::AlignHCenter);
    d->layout.setAlignment(&d->muteButton, align);
    d->layout.setAlignment(&d->slider, align);
    d->layout.setDirection(o == Qt::Horizontal ? QBoxLayout::LeftToRight : QBoxLayout::TopToBottom);
    d->slider.setOrientation(o);
}

AudioOutput *VolumeSlider::audioOutput() const
{
    K_D(const VolumeSlider);
    return d->output;
}

void VolumeSlider::setAudioOutput(AudioOutput *output)
{
    K_D(VolumeSlider);
    if (d->output) {
        disconnect(d->output, 0, this, 0);
    }
    d->output = output;
    if (output) {
        d->slider.setValue(qRound(100 * output->volume()));
        d->slider.setEnabled(true);
        d->muteButton.setEnabled(true);

        d->_k_volumeChanged(output->volume());
        d->_k_mutedChanged(output->isMuted());

        connect(output, SIGNAL(volumeChanged(qreal)), SLOT(_k_volumeChanged(qreal)));
        connect(output, SIGNAL(mutedChanged(bool)), SLOT(_k_mutedChanged(bool)));
    } else {
        d->slider.setValue(100);
        d->slider.setEnabled(false);
        d->muteButton.setEnabled(false);
    }
}

void VolumeSliderPrivate::_k_buttonClicked()
{
    if (output) {
        output->setMuted(!output->isMuted());
    } else {
        slider.setEnabled(false);
        muteButton.setEnabled(false);
    }
}

void VolumeSliderPrivate::_k_mutedChanged(bool muted)
{
#ifndef QT_NO_TOOLTIP
    Q_Q(VolumeSlider);
#endif
    if (muted) {
#ifndef QT_NO_TOOLTIP
        q->setToolTip(VolumeSlider::tr("Muted"));
#endif
        muteButton.setIcon(mutedIcon);
    } else {
#ifndef QT_NO_TOOLTIP
        q->setToolTip(VolumeSlider::tr("Volume: %1%").arg(static_cast<int>(output->volume() * 100.0)));
#endif
        muteButton.setIcon(volumeIcon);
    }
}

void VolumeSliderPrivate::_k_sliderChanged(int value)
{
#ifndef QT_NO_TOOLTIP
    Q_Q(VolumeSlider);
#endif

    if (output) {
#ifndef QT_NO_TOOLTIP
        if (!output->isMuted()) {
           q->setToolTip(VolumeSlider::tr("Volume: %1%").arg(value));
        }
#endif

        ignoreVolumeChange = true;
        output->setVolume((static_cast<qreal>(value)) * 0.01);
        ignoreVolumeChange = false;
    } else {
        slider.setEnabled(false);
        muteButton.setEnabled(false);
    }
}

void VolumeSliderPrivate::_k_volumeChanged(qreal value)
{
    if (!ignoreVolumeChange) {
        slider.setValue(qRound(100 * value));
    }
}

bool VolumeSlider::hasTracking() const
{
    return k_ptr->slider.hasTracking();
}

void VolumeSlider::setTracking(bool tracking)
{
    k_ptr->slider.setTracking(tracking);
}

int VolumeSlider::pageStep() const
{
    return k_ptr->slider.pageStep();
}

void VolumeSlider::setPageStep(int milliseconds)
{
    k_ptr->slider.setPageStep(milliseconds);
}

int VolumeSlider::singleStep() const
{
    return k_ptr->slider.singleStep();
}

void VolumeSlider::setSingleStep(int milliseconds)
{
    k_ptr->slider.setSingleStep(milliseconds);
}

} // namespace Phonon

#endif //QT_NO_PHONON_VOLUMESLIDER

QT_END_NAMESPACE

#include "moc_volumeslider.cpp"

// vim: sw=4 et
