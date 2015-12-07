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

#ifndef PHONON_UI_SEEKSLIDER_H
#define PHONON_UI_SEEKSLIDER_H

#include "phonon_export.h"
#include "phonondefs.h"
#include "phononnamespace.h"
#include <QtGui/QWidget>

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

#ifndef QT_NO_PHONON_SEEKSLIDER

namespace Phonon
{
class MediaObject;

class SeekSliderPrivate;

/** \class SeekSlider seekslider.h Phonon/SeekSlider
 * \short Widget providing a slider for seeking in MediaObject objects.
 *
 * \ingroup PhononWidgets
 * \author Matthias Kretz <kretz@kde.org>
 */
class PHONON_EXPORT SeekSlider : public QWidget
{
    Q_OBJECT
    K_DECLARE_PRIVATE(SeekSlider)
    /**
     * This property holds whether the icon next to the slider is visible.
     *
     * By default the icon is visible if the platform provides an icon; else
     * it's hidden.
     */
    Q_PROPERTY(bool iconVisible READ isIconVisible WRITE setIconVisible)

    /**
     * This property holds whether slider tracking is enabled.
     *
     * If tracking is enabled (the default), the media seeks
     * while the slider is being dragged. If tracking is
     * disabled, the media seeks only when the user
     * releases the slider.
     */
    Q_PROPERTY(bool tracking READ hasTracking WRITE setTracking)

    /**
     * This property holds the page step.
     *
     * The larger of two natural steps that a slider provides and
     * typically corresponds to the user pressing PageUp or PageDown.
     *
     * Defaults to 5 seconds.
     */
    Q_PROPERTY(int pageStep READ pageStep WRITE setPageStep)

    /**
     * This property holds the single step.
     *
     * The smaller of two natural steps that a slider provides and
     * typically corresponds to the user pressing an arrow key.
     *
     * Defaults to 0.5 seconds.
     */
    Q_PROPERTY(int singleStep READ singleStep WRITE setSingleStep)

    /**
     * This property holds the orientation of the slider.
     *
     * The orientation must be Qt::Vertical or Qt::Horizontal (the default).
     */
    Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation)

    /**
     * \brief the icon size used for the mute button/icon.
     *
     * The default size is defined by the GUI style.
     */
    Q_PROPERTY(QSize iconSize READ iconSize WRITE setIconSize)

    public:
        /**
         * Constructs a seek slider widget with the given \p parent.
         */
        explicit SeekSlider(QWidget *parent = 0);
        explicit SeekSlider(MediaObject *media, QWidget *parent = 0);

        /**
         * Destroys the seek slider.
         */
        ~SeekSlider();

        bool hasTracking() const;
        void setTracking(bool tracking);
        int pageStep() const;
        void setPageStep(int milliseconds);
        int singleStep() const;
        void setSingleStep(int milliseconds);
        Qt::Orientation orientation() const;
        bool isIconVisible() const;
        QSize iconSize() const;
        MediaObject *mediaObject() const;

    public Q_SLOTS:
        void setOrientation(Qt::Orientation);
        void setIconVisible(bool);
        void setIconSize(const QSize &size);

        /**
         * Sets the media object to be controlled by this slider.
         */
        void setMediaObject(MediaObject *);

    protected:
        SeekSliderPrivate *const k_ptr;

    private:
        Q_PRIVATE_SLOT(k_func(), void _k_stateChanged(Phonon::State))
        Q_PRIVATE_SLOT(k_func(), void _k_seek(int))
        Q_PRIVATE_SLOT(k_func(), void _k_tick(qint64))
        Q_PRIVATE_SLOT(k_func(), void _k_length(qint64))
        Q_PRIVATE_SLOT(k_func(), void _k_seekableChanged(bool))
        Q_PRIVATE_SLOT(k_func(), void _k_currentSourceChanged())
};

} // namespace Phonon

#endif //QT_NO_PHONON_SEEKSLIDER

QT_END_NAMESPACE
QT_END_HEADER

// vim: sw=4 ts=4 tw=80
#endif // PHONON_UI_SEEKSLIDER_H
