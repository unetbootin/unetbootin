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

#ifndef PHONON_UI_VOLUMESLIDER_H
#define PHONON_UI_VOLUMESLIDER_H

#include "phonon_export.h"
#include "phonondefs.h"
#include <QtGui/QWidget>

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

#ifndef QT_NO_PHONON_VOLUMESLIDER

namespace Phonon
{
class AudioOutput;
class VolumeSliderPrivate;

/** \class VolumeSlider volumeslider.h Phonon/VolumeSlider
 * \short Widget providing a slider to control the volume of an AudioOutput.
 *
 * \ingroup PhononWidgets
 * \author Matthias Kretz <kretz@kde.org>
 */
class PHONON_EXPORT VolumeSlider : public QWidget
{
    Q_OBJECT
    K_DECLARE_PRIVATE(VolumeSlider)
    /**
     * This property holds the maximum volume that can be set with this slider.
     *
     * By default the maximum value is 1.0 (100%).
     */
    Q_PROPERTY(qreal maximumVolume READ maximumVolume WRITE setMaximumVolume)
    /**
     * This property holds the orientation of the slider.
     *
     * The orientation must be Qt::Vertical (the default) or Qt::Horizontal.
     */
    Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation)

    /**
     * This property holds whether slider tracking is enabled.
     *
     * If tracking is enabled (the default), the volume changes
     * while the slider is being dragged. If tracking is
     * disabled, the volume changes only when the user
     * releases the slider.
     */
    Q_PROPERTY(bool tracking READ hasTracking WRITE setTracking)

    /**
     * This property holds the page step.
     *
     * The larger of two natural steps that a slider provides and
     * typically corresponds to the user pressing PageUp or PageDown.
     *
     * Defaults to 5 (5% of the voltage).
     */
    Q_PROPERTY(int pageStep READ pageStep WRITE setPageStep)

    /**
     * This property holds the single step.
     *
     * The smaller of two natural steps that a slider provides and
     * typically corresponds to the user pressing an arrow key.
     *
     * Defaults to 1 (1% of the voltage).
     */
    Q_PROPERTY(int singleStep READ singleStep WRITE setSingleStep)

    /**
     * This property holds whether the mute button/icon next to the slider is visible.
     *
     * By default the mute button/icon is visible.
     */
    Q_PROPERTY(bool muteVisible READ isMuteVisible WRITE setMuteVisible)

    /**
     * \brief the icon size used for the mute button/icon.
     *
     * The default size is defined by the GUI style.
     */
    Q_PROPERTY(QSize iconSize READ iconSize WRITE setIconSize)
    public:
        /**
         * Constructs a new volume slider with a \p parent.
         */
        explicit VolumeSlider(QWidget *parent = 0);
        explicit VolumeSlider(AudioOutput *, QWidget *parent = 0);
        ~VolumeSlider();

        bool hasTracking() const;
        void setTracking(bool tracking);
        int pageStep() const;
        void setPageStep(int milliseconds);
        int singleStep() const;
        void setSingleStep(int milliseconds);
        bool isMuteVisible() const;
        QSize iconSize() const;
        qreal maximumVolume() const;
        Qt::Orientation orientation() const;
        AudioOutput *audioOutput() const;

    public Q_SLOTS:
        void setMaximumVolume(qreal);
        void setOrientation(Qt::Orientation);
        void setMuteVisible(bool);
        void setIconSize(const QSize &size);

        /**
         * Sets the audio output object to be controlled by this slider.
         */
        void setAudioOutput(Phonon::AudioOutput *);

    protected:
        VolumeSliderPrivate *const k_ptr;

    private:
        Q_PRIVATE_SLOT(k_ptr, void _k_sliderChanged(int))
        Q_PRIVATE_SLOT(k_ptr, void _k_volumeChanged(qreal))
        Q_PRIVATE_SLOT(k_ptr, void _k_mutedChanged(bool))
        Q_PRIVATE_SLOT(k_ptr, void _k_buttonClicked())
};

} // namespace Phonon

#endif //QT_NO_PHONON_VOLUMESLIDER

QT_END_NAMESPACE
QT_END_HEADER

// vim: sw=4 ts=4 et
#endif // PHONON_UI_VOLUMESLIDER_H
