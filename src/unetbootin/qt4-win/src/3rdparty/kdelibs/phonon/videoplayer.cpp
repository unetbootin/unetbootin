/*  This file is part of the KDE project
    Copyright (C) 2004-2007 Matthias Kretz <kretz@kde.org>

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

#include "videoplayer.h"
#include "mediaobject.h"
#include "audiooutput.h"
#include "videowidget.h"
#include "path.h"
#include <QtGui/QBoxLayout>

QT_BEGIN_NAMESPACE

namespace Phonon
{

class VideoPlayerPrivate
{
    public:
        VideoPlayerPrivate()
            : player(0)
        {
        }

        MediaObject *player;
        AudioOutput *aoutput;
        VideoWidget *voutput;

        MediaSource src;
};

VideoPlayer::VideoPlayer(Phonon::Category category, QWidget *parent)
    : QWidget(parent)
    , d(new VideoPlayerPrivate)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(0);

    d->aoutput = new AudioOutput(category, this);

    d->voutput = new VideoWidget(this);
    layout->addWidget(d->voutput);


    d->player = new MediaObject(this);
    Phonon::createPath(d->player, d->aoutput);
    Phonon::createPath(d->player, d->voutput);

    connect(d->player, SIGNAL(finished()), SIGNAL(finished()));
}

VideoPlayer::~VideoPlayer()
{
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

    if (StoppedState == d->player->state())
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

QT_END_NAMESPACE

#include "moc_videoplayer.cpp"

// vim: sw=4 ts=4
