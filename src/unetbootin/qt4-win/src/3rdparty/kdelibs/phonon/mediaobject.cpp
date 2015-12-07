/*  This file is part of the KDE project
    Copyright (C) 2005-2007 Matthias Kretz <kretz@kde.org>

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
#include "mediaobject.h"
#include "mediaobject_p.h"

#include "factory_p.h"
#include "mediaobjectinterface.h"
#include "frontendinterface_p.h"
#include "mediasource.h"
#include "abstractmediastream.h"
#include "audiooutput.h"
#include "abstractmediastream_p.h"
#include "phonondefs_p.h"

#include <QtCore/QStringList>
#include <QtCore/QCoreApplication>
#include <QtCore/QUrl>
#include <QtCore/QTimer>

#include "phononnamespace_p.h"
#include "platform_p.h"

#define PHONON_CLASSNAME MediaObject
#define PHONON_INTERFACENAME MediaObjectInterface

QT_BEGIN_NAMESPACE

namespace Phonon
{
PHONON_OBJECT_IMPL

MediaObject::~MediaObject()
{
    K_D(MediaObject);
    if (d->m_backendObject) {
        switch (state()) {
        case PlayingState:
        case BufferingState:
        case PausedState:
            stop();
            break;
        case ErrorState:
        case StoppedState:
        case LoadingState:
            break;
        }
    }
}

Phonon::State MediaObject::state() const
{
    K_D(const MediaObject);
    if (d->errorOverride) {
        return d->state;
    }
    if (d->ignoreLoadingToBufferingStateChange) {
        return BufferingState;
    }
    if (d->ignoreErrorToLoadingStateChange) {
        return LoadingState;
    }
    if (!d->m_backendObject) {
        return d->state;
    }
    return INTERFACE_CALL(state());
}

PHONON_INTERFACE_SETTER(setTickInterval, tickInterval, qint32)
PHONON_INTERFACE_GETTER(qint32, tickInterval, d->tickInterval)
PHONON_INTERFACE_GETTER(bool, hasVideo, false)
PHONON_INTERFACE_GETTER(bool, isSeekable, false)
PHONON_INTERFACE_GETTER(qint64, currentTime, d->currentTime)


void MediaObject::play()
{
    K_D(MediaObject);
    if (d->backendObject() && d->mediaSource.type() != MediaSource::Invalid) {
        INTERFACE_CALL(play());
    }
}

void MediaObject::pause()
{
    K_D(MediaObject);
    if (d->backendObject() && d->mediaSource.type() != MediaSource::Invalid) {
        INTERFACE_CALL(pause());
    }
}

void MediaObject::stop()
{
    K_D(MediaObject);
    if (d->backendObject() && d->mediaSource.type() != MediaSource::Invalid) {
        INTERFACE_CALL(stop());
    }
}

void MediaObject::seek(qint64 time)
{
    K_D(MediaObject);
    if (d->backendObject() && d->mediaSource.type() != MediaSource::Invalid) {
        INTERFACE_CALL(seek(time));
    }
}

QString MediaObject::errorString() const
{
    if (state() == Phonon::ErrorState) {
        K_D(const MediaObject);
        if (d->errorOverride) {
            return d->errorString;
        }
        return INTERFACE_CALL(errorString());
    }
    return QString();
}

ErrorType MediaObject::errorType() const
{
    if (state() == Phonon::ErrorState) {
        K_D(const MediaObject);
        if (d->errorOverride) {
            return d->errorType;
        }
        return INTERFACE_CALL(errorType());
    }
    return Phonon::NoError;
}

QStringList MediaObject::metaData(Phonon::MetaData f) const
{
    switch (f) {
    case ArtistMetaData:
        return metaData(QLatin1String("ARTIST"     ));
    case AlbumMetaData:
        return metaData(QLatin1String("ALBUM"      ));
    case TitleMetaData:
        return metaData(QLatin1String("TITLE"      ));
    case DateMetaData:
        return metaData(QLatin1String("DATE"       ));
    case GenreMetaData:
        return metaData(QLatin1String("GENRE"      ));
    case TracknumberMetaData:
        return metaData(QLatin1String("TRACKNUMBER"));
    case DescriptionMetaData:
        return metaData(QLatin1String("DESCRIPTION"));
    case MusicBrainzDiscIdMetaData:
        return metaData(QLatin1String("MUSICBRAINZ_DISCID"));
    }
    return QStringList();
}

QStringList MediaObject::metaData(const QString &key) const
{
    K_D(const MediaObject);
    return d->metaData.values(key);
}

QMultiMap<QString, QString> MediaObject::metaData() const
{
    K_D(const MediaObject);
    return d->metaData;
}

PHONON_INTERFACE_GETTER(qint32, prefinishMark, d->prefinishMark)
PHONON_INTERFACE_SETTER(setPrefinishMark, prefinishMark, qint32)

PHONON_INTERFACE_GETTER(qint32, transitionTime, d->transitionTime)
PHONON_INTERFACE_SETTER(setTransitionTime, transitionTime, qint32)

qint64 MediaObject::totalTime() const
{
    K_D(const MediaObject);
    if (!d->m_backendObject) {
        return -1;
    }
    return INTERFACE_CALL(totalTime());
}

qint64 MediaObject::remainingTime() const
{
    K_D(const MediaObject);
    if (!d->m_backendObject) {
        return -1;
    }
    qint64 ret = INTERFACE_CALL(remainingTime());
    if (ret < 0) {
        return -1;
    }
    return ret;
}

MediaSource MediaObject::currentSource() const
{
    K_D(const MediaObject);
    return d->mediaSource;
}

void MediaObject::setCurrentSource(const MediaSource &newSource)
{
    K_D(MediaObject);
    if (!k_ptr->backendObject()) {
        d->mediaSource = newSource;
        return;
    }

    pDebug() << Q_FUNC_INFO << newSource.url();

    stop(); // first call stop as that often is the expected state
            // for setting a new URL

    MediaSource::Type oldSourceType = d->mediaSource.type();
    d->mediaSource = newSource;
    d->kiofallback = 0; // kiofallback auto-deletes

//X         if (url.scheme() == "http") {
//X             d->kiofallback = Platform::createMediaStream(url, this);
//X             if (d->kiofallback) {
//X                 ...
//X                 return;
//X             }
//X         }

    if (d->mediaSource.type() == MediaSource::Stream) {
        Q_ASSERT(d->mediaSource.stream());
        d->mediaSource.stream()->d_func()->setMediaObjectPrivate(d);
    } else if (d->mediaSource.type() == MediaSource::Invalid) {
        pWarning() << "requested invalid MediaSource for the current source of MediaObject";
        return;
    }
    if (d->mediaSource.type() == MediaSource::Url && oldSourceType != MediaSource::Url) {
        disconnect(d->m_backendObject, SIGNAL(stateChanged(Phonon::State, Phonon::State)), this, SIGNAL(stateChanged(Phonon::State, Phonon::State)));
        connect(d->m_backendObject, SIGNAL(stateChanged(Phonon::State, Phonon::State)), this, SLOT(_k_stateChanged(Phonon::State, Phonon::State)));
    } else if (d->mediaSource.type() != MediaSource::Url && oldSourceType == MediaSource::Url) {
        disconnect(d->m_backendObject, SIGNAL(stateChanged(Phonon::State, Phonon::State)), this, SLOT(_k_stateChanged(Phonon::State, Phonon::State)));
        connect(d->m_backendObject, SIGNAL(stateChanged(Phonon::State, Phonon::State)), this, SIGNAL(stateChanged(Phonon::State, Phonon::State)));
    }
    INTERFACE_CALL(setSource(d->mediaSource));
}

QList<MediaSource> MediaObject::queue() const
{
    K_D(const MediaObject);
    return d->sourceQueue;
}

void MediaObject::setQueue(const QList<MediaSource> &sources)
{
    K_D(MediaObject);
    d->sourceQueue.clear();
    enqueue(sources);
}

void MediaObject::setQueue(const QList<QUrl> &urls)
{
    K_D(MediaObject);
    d->sourceQueue.clear();
    enqueue(urls);
}

void MediaObject::enqueue(const MediaSource &source)
{
    K_D(MediaObject);
    if (d->mediaSource.type() == MediaSource::Invalid) {
        setCurrentSource(source);
    } else {
        d->sourceQueue << source;
    }
}

void MediaObject::enqueue(const QList<MediaSource> &sources)
{
    foreach (const MediaSource &m, sources) {
        enqueue(m);
    }
}

void MediaObject::enqueue(const QList<QUrl> &urls)
{
    foreach (const QUrl &url, urls) {
        enqueue(url);
    }
}

void MediaObject::clearQueue()
{
    K_D(MediaObject);
    d->sourceQueue.clear();
}

bool MediaObjectPrivate::aboutToDeleteBackendObject()
{
    //pDebug() << Q_FUNC_INFO;
    prefinishMark = pINTERFACE_CALL(prefinishMark());
    transitionTime = pINTERFACE_CALL(transitionTime());
    //pDebug() << Q_FUNC_INFO;
    if (m_backendObject) {
        state = pINTERFACE_CALL(state());
        currentTime = pINTERFACE_CALL(currentTime());
        tickInterval = pINTERFACE_CALL(tickInterval());
    }
    return true;
}

void MediaObjectPrivate::streamError(Phonon::ErrorType type, const QString &text)
{
    Q_Q(MediaObject);
    State lastState = q->state();
    errorOverride = true;
    errorType = type;
    errorString = text;
    state = ErrorState;
    QMetaObject::invokeMethod(q, "stateChanged", Qt::QueuedConnection, Q_ARG(Phonon::State, Phonon::ErrorState), Q_ARG(Phonon::State, lastState));
    //emit q->stateChanged(ErrorState, lastState);
}

void MediaObjectPrivate::_k_stateChanged(Phonon::State newstate, Phonon::State oldstate)
{
    Q_Q(MediaObject);
    if (mediaSource.type() != MediaSource::Url) {
        // special handling only necessary for URLs because of the fallback
        emit q->stateChanged(newstate, oldstate);
    }
    if (errorOverride) {
        errorOverride = false;
        if (newstate == ErrorState) {
            return;
        }
        oldstate = ErrorState;
    }

    // backend MediaObject reached ErrorState, try a KioMediaSource
    if (newstate == Phonon::ErrorState && !kiofallback) {
        kiofallback = Platform::createMediaStream(mediaSource.url(), q);
        if (!kiofallback) {
            pDebug() << "backend MediaObject reached ErrorState, no KIO fallback available";
            emit q->stateChanged(newstate, oldstate);
            return;
        }
        pDebug() << "backend MediaObject reached ErrorState, trying Platform::createMediaStream now";
        ignoreLoadingToBufferingStateChange = false;
        ignoreErrorToLoadingStateChange = false;
        switch (oldstate) {
        case Phonon::BufferingState:
            // play() has already been called, we need to make sure it is called
            // on the backend with the KioMediaStream MediaSource now, too
            ignoreLoadingToBufferingStateChange = true;
            break;
        case Phonon::LoadingState:
            ignoreErrorToLoadingStateChange = true;
            // no extras
            break;
        default:
            pError() << "backend MediaObject reached ErrorState after " << oldstate
                << ". It seems a KioMediaStream will not help here, trying anyway.";
            emit q->stateChanged(Phonon::LoadingState, oldstate);
            break;
        }
        kiofallback->d_func()->setMediaObjectPrivate(this);
        MediaSource mediaSource(kiofallback);
        mediaSource.setAutoDelete(true);
        pINTERFACE_CALL(setSource(mediaSource));
        if (oldstate == Phonon::BufferingState) {
            q->play();
        }
        return;
    } else if (ignoreLoadingToBufferingStateChange &&
            kiofallback &&
            oldstate == Phonon::LoadingState) {
        if (newstate != Phonon::BufferingState) {
            emit q->stateChanged(newstate, Phonon::BufferingState);
        }
        return;
    } else if (ignoreErrorToLoadingStateChange && kiofallback && oldstate == ErrorState) {
        if (newstate != LoadingState) {
            emit q->stateChanged(newstate, Phonon::LoadingState);
        }
        return;
    }

    emit q->stateChanged(newstate, oldstate);
}

void MediaObjectPrivate::_k_aboutToFinish()
{
    Q_Q(MediaObject);
    pDebug() << Q_FUNC_INFO;

    kiofallback = 0; // kiofallback auto-deletes

    if (sourceQueue.isEmpty()) {
        emit q->aboutToFinish();
        if (sourceQueue.isEmpty()) {
            pINTERFACE_CALL(setNextSource(MediaSource()));
            return;
        }
    }

    mediaSource = sourceQueue.head();
    pINTERFACE_CALL(setNextSource(mediaSource));
}

void MediaObjectPrivate::_k_currentSourceChanged(const MediaSource &source)
{
    Q_Q(MediaObject);
    pDebug() << Q_FUNC_INFO;

    if (!sourceQueue.isEmpty() && sourceQueue.head() == source)
        sourceQueue.dequeue();

    emit q->currentSourceChanged(source);
}

void MediaObjectPrivate::setupBackendObject()
{
    Q_Q(MediaObject);
    Q_ASSERT(m_backendObject);
    //pDebug() << Q_FUNC_INFO;

    if (mediaSource.type() == MediaSource::Url) {
        QObject::connect(m_backendObject, SIGNAL(stateChanged(Phonon::State, Phonon::State)), q, SLOT(_k_stateChanged(Phonon::State, Phonon::State)));
    } else {
        QObject::connect(m_backendObject, SIGNAL(stateChanged(Phonon::State, Phonon::State)), q, SIGNAL(stateChanged(Phonon::State, Phonon::State)));
    }
    QObject::connect(m_backendObject, SIGNAL(tick(qint64)),             q, SIGNAL(tick(qint64)));
    QObject::connect(m_backendObject, SIGNAL(seekableChanged(bool)),    q, SIGNAL(seekableChanged(bool)));
    QObject::connect(m_backendObject, SIGNAL(hasVideoChanged(bool)),    q, SIGNAL(hasVideoChanged(bool)));
    QObject::connect(m_backendObject, SIGNAL(bufferStatus(int)),        q, SIGNAL(bufferStatus(int)));
    QObject::connect(m_backendObject, SIGNAL(finished()),               q, SIGNAL(finished()));
    QObject::connect(m_backendObject, SIGNAL(aboutToFinish()),          q, SLOT(_k_aboutToFinish()));
    QObject::connect(m_backendObject, SIGNAL(prefinishMarkReached(qint32)), q, SIGNAL(prefinishMarkReached(qint32)));
    QObject::connect(m_backendObject, SIGNAL(totalTimeChanged(qint64)), q, SIGNAL(totalTimeChanged(qint64)));
    QObject::connect(m_backendObject, SIGNAL(metaDataChanged(const QMultiMap<QString, QString> &)),
            q, SLOT(_k_metaDataChanged(const QMultiMap<QString, QString> &)));
    QObject::connect(m_backendObject, SIGNAL(currentSourceChanged(const MediaSource&)), 
        q, SLOT(_k_currentSourceChanged(const MediaSource&)));

    // set up attributes
    pINTERFACE_CALL(setTickInterval(tickInterval));
    pINTERFACE_CALL(setPrefinishMark(prefinishMark));
    pINTERFACE_CALL(setTransitionTime(transitionTime));

    switch(state)
    {
    case LoadingState:
    case StoppedState:
    case ErrorState:
        break;
    case PlayingState:
    case BufferingState:
        QTimer::singleShot(0, q, SLOT(_k_resumePlay()));
        break;
    case PausedState:
        QTimer::singleShot(0, q, SLOT(_k_resumePause()));
        break;
    }
    const State backendState = pINTERFACE_CALL(state());
    if (state != backendState && state != ErrorState) {
        // careful: if state is ErrorState we might be switching from a
        // MediaObject to a ByteStream for KIO fallback. In that case the state
        // change to ErrorState was already suppressed.
        pDebug() << "emitting a state change because the backend object has been replaced";
        emit q->stateChanged(backendState, state);
        state = backendState;
    }

    foreach (FrontendInterfacePrivate *f, interfaceList) {
        f->_backendObjectChanged();
    }

    // set up attributes
    if (mediaSource.type() != MediaSource::Invalid) {
        if (mediaSource.type() == MediaSource::Stream) {
            Q_ASSERT(mediaSource.stream());
            mediaSource.stream()->d_func()->setMediaObjectPrivate(this);
        }
        pINTERFACE_CALL(setSource(mediaSource));
    }
}

void MediaObjectPrivate::_k_resumePlay()
{
    qobject_cast<MediaObjectInterface *>(m_backendObject)->play();
    if (currentTime > 0) {
        qobject_cast<MediaObjectInterface *>(m_backendObject)->seek(currentTime);
    }
}

void MediaObjectPrivate::_k_resumePause()
{
    pINTERFACE_CALL(pause());
    if (currentTime > 0) {
        qobject_cast<MediaObjectInterface *>(m_backendObject)->seek(currentTime);
    }
}

void MediaObjectPrivate::_k_metaDataChanged(const QMultiMap<QString, QString> &newMetaData)
{
    metaData = newMetaData;
    emit q_func()->metaDataChanged();
}

void MediaObjectPrivate::phononObjectDestroyed(MediaNodePrivate *bp)
{
    // this method is called from Phonon::Base::~Base(), meaning the AudioPath
    // dtor has already been called, also virtual functions don't work anymore
    // (therefore qobject_cast can only downcast from Base)
    Q_ASSERT(bp);
    Q_UNUSED(bp);
}

MediaObject *createPlayer(Phonon::Category category, const MediaSource &source)
{
    MediaObject *mo = new MediaObject;
    AudioOutput *ao = new AudioOutput(category, mo);
    createPath(mo, ao);
    if (source.type() != MediaSource::Invalid) {
        mo->setCurrentSource(source);
    }
    return mo;
}

} //namespace Phonon

QT_END_NAMESPACE

#include "moc_mediaobject.cpp"

#undef PHONON_CLASSNAME
#undef PHONON_INTERFACENAME
// vim: sw=4 tw=100 et
