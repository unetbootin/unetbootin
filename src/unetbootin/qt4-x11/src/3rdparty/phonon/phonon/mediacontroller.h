/*  This file is part of the KDE project
    Copyright (C) 2007 Matthias Kretz <kretz@kde.org>

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

#ifndef PHONON_MEDIACONTROLLER_H
#define PHONON_MEDIACONTROLLER_H

#include "phonon_export.h"
#include "objectdescription.h"

#include <QtCore/QObject>
#include <QtCore/QtGlobal>

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

#ifndef QT_NO_PHONON_MEDIACONTROLLER

namespace Phonon
{
class MediaControllerPrivate;
class MediaObject;

/** \class MediaController mediacontroller.h Phonon/MediaController
 * \brief Controls optional features of a media file/device like title, chapter, angle.
 *
 * \ingroup Playback
 * \author Matthias Kretz <kretz@kde.org>
 */
class PHONON_EXPORT MediaController : public QObject
{
    Q_OBJECT
    Q_FLAGS(Features)
    public:
        enum Feature {
            Angles = 1,
            Chapters = 2,
            Titles = 4
        };
        Q_DECLARE_FLAGS(Features, Feature)

        MediaController(MediaObject *parent);
        ~MediaController();

        Features supportedFeatures() const;

        int availableAngles() const;
        int currentAngle() const;

        int availableChapters() const;
        int currentChapter() const;

        int availableTitles() const;
        int currentTitle() const;

        bool autoplayTitles() const;

        /**
         * Returns the selected audio stream.
         *
         * \see availableAudioChannels
         * \see setCurrentAudioChannel
         */
        AudioChannelDescription currentAudioChannel() const;

        /**
         * Returns the selected subtitle stream.
         *
         * \see availableSubtitles
         * \see setCurrentSubtitle
         */
        SubtitleDescription currentSubtitle() const;

        /**
         * Returns the audio streams that can be selected by the user. The
         * strings can directly be used in the user interface.
         *
         * \see selectedAudioChannel
         * \see setCurrentAudioChannel
         */
        QList<AudioChannelDescription> availableAudioChannels() const;

        /**
         * Returns the subtitle streams that can be selected by the user. The
         * strings can directly be used in the user interface.
         *
         * \see selectedSubtitle
         * \see setCurrentSubtitle
         */
        QList<SubtitleDescription> availableSubtitles() const;

        /**
         * Selects an audio stream from the media.
         *
         * Some media formats allow multiple audio streams to be stored in
         * the same file. Normally only one should be played back.
         *
         * \param stream Description of an audio stream
         *
         * \see availableAudioChannels()
         * \see currentAudioChannel()
         */
        void setCurrentAudioChannel(const Phonon::AudioChannelDescription &stream);

        /**
         * Selects a subtitle stream from the media.
         *
         * Some media formats allow multiple subtitle streams to be stored in
         * the same file. Normally only one should be displayed.
         *
         * \param stream description of a subtitle stream
         *
         * \see availableSubtitles()
         * \see currentSubtitle()
         */
        void setCurrentSubtitle(const Phonon::SubtitleDescription &stream);

    public Q_SLOTS:
        void setCurrentAngle(int angleNumber);
        void setCurrentChapter(int chapterNumber);

        /**
         * Skips to the given title \p titleNumber.
         *
         * If it was playing before the title change it will start playback on the new title if
         * autoplayTitles is enabled.
         */
        void setCurrentTitle(int titleNumber);
        void setAutoplayTitles(bool);

        /**
         * Skips to the next title.
         *
         * If it was playing before the title change it will start playback on the next title if
         * autoplayTitles is enabled.
         */
        void nextTitle();

        /**
         * Skips to the previous title.
         *
         * If it was playing before the title change it will start playback on the previous title if
         * autoplayTitles is enabled.
         */
        void previousTitle();

    Q_SIGNALS:
        void availableSubtitlesChanged();
        void availableAudioChannelsChanged();
        void availableAnglesChanged(int availableAngles);
        void angleChanged(int angleNumber);
        void availableChaptersChanged(int availableChapters);
        void chapterChanged(int chapterNumber);
        void availableTitlesChanged(int availableTitles);
        void titleChanged(int titleNumber);

    protected:
        MediaControllerPrivate *const d;
};

} // namespace Phonon

Q_DECLARE_OPERATORS_FOR_FLAGS(Phonon::MediaController::Features)

#endif //QT_NO_PHONON_MEDIACONTROLLER

QT_END_NAMESPACE
QT_END_HEADER

#endif // PHONON_MEDIACONTROLLER_H
