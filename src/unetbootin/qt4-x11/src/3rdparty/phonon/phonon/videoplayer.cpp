/*  This file is part of the KDE project
    Copyright (C) 2004-2007 Matthias Kretz <kretz@kde.org>

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

#include "videoplayer.h"
#include "mediaobject.h"
#include "audiooutput.h"
#include "videowidget.h"
#include "path.h"
#include <QtGui/QBoxLayout>

QT_BEGIN_NAMESPACE

#ifndef QT_NO_PHONON_VIDEOPLAYER

namespace Phonon
{

class VideoPlayerPrivate
{
    public:
        VideoPlayerPrivate()
            : player(0)
            , aoutput(0)
            , voutput(0) {}

        void init(VideoPlayer *q, Phonon::Category category);

        MediaObject *player;
        AudioOutput *aoutput;
        VideoWidget *voutput;

        MediaSource src;
};

void VideoPlayerPrivate::init(VideoPlayer *q, Phonon::Category category)
{
    QVBoxLayout *layout = new QVBoxLayout(q);
    layout->setMargin(0);

    aoutput = new AudioOutput(category, q);

    voutput = new VideoWidget(q);
    layout->addWidget(voutput);

    player = new MediaObject(q);
    Phonon::createPath(player, aoutput);
    Phonon::createPath(player, voutput);

    q->connect(player, SIGNAL(finished()), SIGNAL(finished()));
}

VideoPlayer::VideoPlayer(Phonon::Category category, QWidget *parent)
    : QWidget(parent)
    , d(new VideoPlayerPrivate)
{
    d->init(this, category);
}

VideoPlayer::VideoPlayer(QWidget *parent)
    : QWidget(parent)
    , d(new VideoPlayerPrivate)
{
    d->init(this, Phonon::VideoCategory);
}

VideoPlayer::~VideoPlayer()
{
    delete d;
}

MediaObject *VideoPlayer::mediaObject() const
{
    return d->player;
}

AudioOutput *VideoPlayer::audioOutput() const
{
    return d->aoutput;
}

VideoWidget *VideoPlayer::videoWidget() const
{
    return d->voutput;
}

void VideoPlayer::load(const MediaSource &source)
{
    d->player->setCurrentSource(source);
}

void VideoPlayer::play(const MediaSource &source)
{
    if (source == d->player->currentSource()) {
        if (!isPlaying())
            d->player->play();
        return;
    }
    // new URL
    d->player->setCurrentSource(source);
        
    if (ErrorState == d->player->state())
        return;

    d->player->play();
}

void VideoPlayer::play()
{
    d->player->play();
}

void VideoPlayer::pause()
{
    d->player->pause();
}

void VideoPlayer::stop()
{
    d->player->stop();
}

qint64 VideoPlayer::totalTime() const
{
    return d->player->totalTime();
}

qint64 VideoPlayer::currentTime() const
{
    return d->player->currentTime();
}

void VideoPlayer::seek(qint64 ms)
{
    d->player->seek(ms);
}

float VideoPlayer::volume() const
{
    return d->aoutput->volume();
}

void VideoPlayer::setVolume(float v)
{
    d->aoutput->setVolume(v);
}

bool VideoPlayer::isPlaying() const
{
    return (d->player->state() == PlayingState);
}

bool VideoPlayer::isPaused() const
{
    return (d->player->state() == PausedState);
}

} // namespaces

#endif //QT_NO_PHONON_VIDEOPLAYER

QT_END_NAMESPACE

#include "moc_videoplayer.cpp"

// vim: sw=4 ts=4
