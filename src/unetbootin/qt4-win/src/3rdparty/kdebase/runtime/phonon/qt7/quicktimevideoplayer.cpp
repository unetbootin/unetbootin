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

#include "quicktimevideoplayer.h"
#include "backendheader.h"
#include "mediaobject.h"
#include "videowidget.h"
#include "audiodevice.h"
#include "quicktimestreamreader.h"

#include <private/qcore_mac_p.h>

#include <QGLContext>
#include <QtOpenGL/private/qgl_p.h>

QT_BEGIN_NAMESPACE

extern QGLShareRegister* qgl_share_reg();

QGLContextPrivate *qt_phonon_get_dptr(const QGLContext *context){
    return context->d_ptr;
}

namespace Phonon
{
namespace QT7
{

// Defined in videowidget.cpp:
QGLWidget *PhononSharedQGLWidget();
    
QuickTimeVideoPlayer::QuickTimeVideoPlayer() : QObject(0)
{
    m_state = NoMedia;
    m_mediaSource = MediaSource();
    m_visualContext = 0;
    m_qGLContext = 0;
    m_movieRef = 0;
    m_streamReader = 0;
    m_playbackRate = 1.0f;
    m_masterVolume = 1.0f;
    m_relativeVolume = 1.0f;
    m_currentTime = 0;
    m_mute = false;
    m_audioEnabled = false;
    m_hasVideo = false;
    m_playbackRateSat = false;
    m_isDrmProtected = false;
    m_isDrmAuthorized = true;
}

QuickTimeVideoPlayer::~QuickTimeVideoPlayer()
{
    unsetVideo();
    if (m_visualContext)
        CFRelease(m_visualContext);
}

bool QuickTimeVideoPlayer::setGLContext(const QGLContext *qGLContext)
{
    if (qgl_share_reg()->checkSharing(m_qGLContext, qGLContext))
        return false;

    m_qGLContext = qGLContext;

    AGLContext aglContext = static_cast<AGLContext>(qt_phonon_get_dptr(m_qGLContext)->cx);
    AGLPixelFormat aglPixelFormat = static_cast<AGLPixelFormat>(qt_phonon_get_dptr(m_qGLContext)->vi);
    return setGLContext(aglContext, aglPixelFormat);
}

bool QuickTimeVideoPlayer::setCurrentGLContext()
{
    return setGLContext(QGLContext::currentContext());
}

bool QuickTimeVideoPlayer::setGLContext(AGLContext aglContext, AGLPixelFormat aglPixelFormat)
{
    OSStatus err = noErr;
    bool ok = true;
    
    if (m_visualContext){
        CFRelease(m_visualContext);
        m_visualContext = 0;
    }

    CGLContextObj cglContext = 0;
    CGLPixelFormatObj cglPixelFormat = 0;

    if (QSysInfo::MacintoshVersion >= QSysInfo::MV_10_4){
        ok = aglGetCGLContext(aglContext, (void **)&cglContext);
        BACKEND_ASSERT3(ok, "Could not CoreVideo GL context (OpenGL)", FATAL_ERROR, false)
        ok = aglGetCGLPixelFormat(aglPixelFormat, (void **)&cglPixelFormat);
        BACKEND_ASSERT3(ok, "Could not get CoreVideo pixel format (OpenGL)", FATAL_ERROR, false)
    } else {
        ok = aglSetCurrentContext(aglContext);
        BACKEND_ASSERT3(ok, "Could not set current GL context (OpenGL)", FATAL_ERROR, false)
        cglContext = CGLGetCurrentContext();
        BACKEND_ASSERT3(cglContext, "Could not get current CoreVideo GL context (OpenGL)", FATAL_ERROR, false)

        Q_UNUSED(aglPixelFormat);
#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_5
        GLint attributeCount = 2;
#else
        long attributeCount = 2;
#endif
        CGLPixelFormatAttribute pfa[attributeCount];
        pfa[0] = kCGLPFAWindow;
        pfa[1] = (CGLPixelFormatAttribute) 0;
        err = CGLChoosePixelFormat(pfa, &cglPixelFormat, &attributeCount);
        BACKEND_ASSERT3(err == noErr, "Could not create pixel format (OpenGL)", FATAL_ERROR, false)
    }

    CFTypeRef keys[] = { kQTVisualContextWorkingColorSpaceKey };
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    CFDictionaryRef textureContextAttributes = CFDictionaryCreate(kCFAllocatorDefault,
        (const void **)keys,
        (const void **)&colorSpace, 1,
        &kCFTypeDictionaryKeyCallBacks,
        &kCFTypeDictionaryValueCallBacks);
        
	err = QTOpenGLTextureContextCreate(kCFAllocatorDefault, cglContext,
        cglPixelFormat, textureContextAttributes, &m_visualContext);
    CFRelease(textureContextAttributes);
    BACKEND_ASSERT3(err == noErr, "Could not create visual context (OpenGL)", FATAL_ERROR, false)

    if (m_movieRef && m_visualContext)
        SetMovieVisualContext(m_movieRef, m_visualContext);
    return true;    
}

bool QuickTimeVideoPlayer::videoFrameChanged(const LinkTimeProxy &timeStamp)
{
    if (!m_movieRef || m_state != Playing || !m_hasVideo)
        return false;

    if (!m_visualContext)
        setGLContext(PhononSharedQGLWidget()->context());
    
    QTVisualContextTask(m_visualContext);
    CVTimeStamp cvStamp = timeStamp.getCVTimeStamp(); 
    return QTVisualContextIsNewImageAvailable(m_visualContext, &cvStamp);
}

CVOpenGLTextureRef QuickTimeVideoPlayer::createCvTexture(const LinkTimeProxy &timeStamp)
{
    if (!m_visualContext)
        return 0;
    
    CVOpenGLTextureRef texture = 0;
    CVTimeStamp cvStamp = timeStamp.getCVTimeStamp(); 
    OSStatus err = QTVisualContextCopyImageForTime(m_visualContext, 0, &cvStamp, &texture);
    BACKEND_ASSERT3(err == noErr, "Could not copy image for time in QuickTime player", FATAL_ERROR, 0)
    return texture;
}

void QuickTimeVideoPlayer::setMasterVolume(float volume)
{
    setVolume(volume, m_relativeVolume);
}

void QuickTimeVideoPlayer::setRelativeVolume(float volume)
{
    setVolume(m_masterVolume, volume);
}

void QuickTimeVideoPlayer::setVolume(float masterVolume, float relativeVolume)
{
    m_masterVolume = masterVolume;
    m_relativeVolume = relativeVolume;
    if (!m_movieRef || !m_audioEnabled || m_mute)
        return;                
    SetMovieAudioGain(m_movieRef, m_masterVolume * m_relativeVolume, 0);
}

void QuickTimeVideoPlayer::setMute(bool mute)
{
    m_mute = mute;
    if (!m_movieRef || m_state != Playing || !m_audioEnabled)
        return;

    // Work-around bug that happends if you set/unset mute
    // before movie is playing, and audio is not played 
    // through graph. Then audio is delayed.
    SetMovieAudioMute(m_movieRef, mute, 0);
    SetMovieAudioGain(m_movieRef, mute ? 0 : m_masterVolume * m_relativeVolume, 0);
}

void QuickTimeVideoPlayer::enableAudio(bool enable)
{
    m_audioEnabled = enable;
    if (!m_movieRef || m_state != Playing)
        return;

    // Work-around bug that happends if you set/unset mute
    // before movie is playing, and audio is not played 
    // through graph. Then audio is delayed.
    SetMovieAudioMute(m_movieRef, (!enable || m_mute), 0);
    SetMovieAudioGain(m_movieRef, (!enable || m_mute) ? 0 : m_masterVolume * m_relativeVolume, 0);
}

bool QuickTimeVideoPlayer::audioEnabled()
{
    return m_audioEnabled;
}

bool QuickTimeVideoPlayer::setAudioDevice(int id)
{
    if (!m_movieRef)
        return false;

    // The following code will not work for some media codecs that
    // typically mingle audio/video frames (e.g mpeg).
    CFStringRef idString = QCFString::toCFStringRef(AudioDevice::deviceUID(id));        
    QTAudioContextRef context;
    QTAudioContextCreateForAudioDevice(kCFAllocatorDefault, idString, 0, &context);
    OSStatus err = SetMovieAudioContext(m_movieRef, context);
    CFRelease(context);
    if (err != noErr)
        return false;
    return true;
}

void QuickTimeVideoPlayer::setColors(qreal brightness, qreal contrast, qreal hue, qreal saturation)
{
    if (!m_movieRef)
        return;

    // 0 is default value for the colors
    // in phonon, so adjust scale:
    contrast += 1;
    saturation += 1;

    Float32 value;
    value = brightness;
    SetMovieVisualBrightness(m_movieRef, value, 0);
    value = contrast;
    SetMovieVisualContrast(m_movieRef, value, 0);
    value = hue;
    SetMovieVisualHue(m_movieRef, value, 0);
    value = saturation;
    SetMovieVisualSaturation(m_movieRef, value, 0);
}

QRect QuickTimeVideoPlayer::videoRect() const
{
    if (!m_movieRef)
        return QRect();

    Rect rect;
    GetMovieBox(m_movieRef, &rect);
    return QRect(rect.left, rect.top, rect.left + rect.right, rect.top + rect.bottom);
}

void QuickTimeVideoPlayer::unsetVideo()
{
    if (!m_movieRef)
        return;

    if (m_streamReader){
        // Streamreader owns the movie.
        delete m_streamReader;
        m_streamReader = 0;
    } else {
        DisposeMovie(m_movieRef);
    }

	m_movieRef = 0;
    m_currentTime = 0;
    m_state = NoMedia;
    m_mediaSource = MediaSource();
    ClearMoviesStickyError();
}

QuickTimeVideoPlayer::State QuickTimeVideoPlayer::state() const
{
    return m_state;
}

quint64 QuickTimeVideoPlayer::timeLoaded()
{
    if (!m_movieRef)
        return 0;
    TimeValue value;
    GetMaxLoadedTimeInMovie(m_movieRef, &value);
    quint64 loaded = static_cast<quint64>(float(value) / float(GetMovieTimeScale(m_movieRef)) * 1000.0f);
    return (loaded == INT_MAX) ? 0 : loaded;
}

float QuickTimeVideoPlayer::percentageLoaded()
{
    if (!m_movieRef || !isSeekable())
        return 0;
    TimeValue loaded;
    GetMaxLoadedTimeInMovie(m_movieRef, &loaded);
    float duration = GetMovieDuration(m_movieRef);
    return duration ? float(loaded) / duration : 0;
}

void QuickTimeVideoPlayer::waitForState(long waitState)
{
    long state = GetMovieLoadState(m_movieRef);
    while (state != kMovieLoadStateError && state < waitState){
        MoviesTask(0, 0);
        state = GetMovieLoadState(m_movieRef);
    }
}

bool QuickTimeVideoPlayer::checkForError()
{
    // Only call this method if we're supposed
    // to have a valid movie loaded.
    if (gGetErrorType() != NO_ERROR)
        return true;
        
    OSStatus err = GetMoviesStickyError();
    if (err == noErr){
        if (!m_movieRef){
            SET_ERROR("Could not open media source.", FATAL_ERROR)
            return true;
        } else if (GetMovieLoadState(m_movieRef) == kMovieLoadStateError){
            SET_ERROR("Could not open media source.", FATAL_ERROR)
            return true;
        }
        return false; // No error occured!        
    }

    ClearMoviesStickyError();
    
    switch(err){
    case noMovieFound:
        SET_ERROR("Could not decode media source.", FATAL_ERROR)
        break;
    case 404:
    case -19160:
    case urlDataHFTPServerDisconnectedErr:
        SET_ERROR("Could not locate media source.", FATAL_ERROR)
        break;
    case afpAccessDenied:
        SET_ERROR("Could not open media source.", FATAL_ERROR)
        break;
    default:
        return false; // Ignore unknown errors.
    }
    return true;
}

void QuickTimeVideoPlayer::setMediaSource(const MediaSource &mediaSource)
{
    unsetVideo();
    m_mediaSource = mediaSource;
    if (mediaSource.type() == MediaSource::Invalid){
        m_state = NoMedia;
        return;
    }
    openMovieFromCurrentMediaSource();
    if (checkForError()){
        unsetVideo();
        return;
    }

    if (m_visualContext)
        SetMovieVisualContext(m_movieRef, m_visualContext);
    waitForState(kMovieLoadStatePlayable);
    if (checkForError()){
        unsetVideo();
        return;
    }

    readProtection();
    preRollMovie();
    if (checkForError()){
        unsetVideo();
        return;
    }

    SetTimeBaseFlags(GetMovieTimeBase(m_movieRef), 0);
    SetMoviePlayHints(m_movieRef, hintsLoop | hintsHighQuality, hintsLoop | hintsHighQuality);
    if (!m_playbackRateSat)
        m_playbackRate = prefferedPlaybackRate();
    checkIfVideoAwailable();
    enableAudio(m_audioEnabled);
    setMute(m_mute);
    setVolume(m_masterVolume, m_relativeVolume);
    pause();
    checkForError();

    // Let the QuickTime work. Why I need to
    // call MoviesTask twize is unknown, but if
    // I don't do it, audio will sometimes not 
    // play immidiatly for movies.
    MoviesTask(m_movieRef, 0);
    MoviesTask(m_movieRef, 0);
}

void QuickTimeVideoPlayer::openMovieFromCurrentMediaSource()
{
    switch (m_mediaSource.type()){
    case MediaSource::LocalFile:
        openMovieFromFile();
        break;
    case MediaSource::Url:
        openMovieFromUrl();
        break;
    case MediaSource::Disc:
        CASE_UNSUPPORTED("Could not open media source.", FATAL_ERROR)
        break;
    case MediaSource::Stream:
        openMovieFromStream();
        break;
    case MediaSource::Invalid:
        break;
    }
}

void QuickTimeVideoPlayer::openMovieFromDataRef(DataReferenceRecord &dataRef)
{
	Boolean boolTrue = true;
	QTNewMoviePropertyElement props[] = {
		{kQTPropertyClass_DataLocation, kQTDataLocationPropertyID_DataReference, sizeof(dataRef), &dataRef, 0},
		{kQTPropertyClass_NewMovieProperty, kQTNewMoviePropertyID_Active, sizeof(boolTrue), &boolTrue, 0},
        {kQTPropertyClass_NewMovieProperty, kQTNewMoviePropertyID_DontInteractWithUser, sizeof(boolTrue), &boolTrue, 0},
        {kQTPropertyClass_MovieInstantiation, kQTMovieInstantiationPropertyID_AsyncOK, sizeof(boolTrue), &boolTrue, 0},
	};
    int propCount = sizeof(props) / sizeof(props[0]);
    NewMovieFromProperties(propCount, props, 0, 0, &m_movieRef);
    DisposeHandle(dataRef.dataRef);
}

void QuickTimeVideoPlayer::openMovieFromFile()
{
    QFileInfo fileInfo(m_mediaSource.fileName());
    QCFString filename = fileInfo.isSymLink() ? fileInfo.symLinkTarget() : fileInfo.canonicalFilePath();
    DataReferenceRecord dataRef = {0, 0};
    QTNewDataReferenceFromFullPathCFString(filename, kQTPOSIXPathStyle, 0, &dataRef.dataRef, &dataRef.dataRefType);
    openMovieFromDataRef(dataRef);
}

void QuickTimeVideoPlayer::openMovieFromUrl()
{
    QCFString urlString = QCFString(m_mediaSource.url().toString());
    CFURLRef url = CFURLCreateWithString(0, urlString, 0);
    DataReferenceRecord dataRef = {0, 0};
    QTNewDataReferenceFromCFURL(url, 0, &dataRef.dataRef, &dataRef.dataRefType);
    openMovieFromDataRef(dataRef);
}

void QuickTimeVideoPlayer::openMovieFromStream()
{
    m_streamReader = new QuickTimeStreamReader(m_mediaSource);
    m_movieRef = m_streamReader->movieRef();
    if (m_movieRef){
	    Boolean boolFalse = true;
        QTSetMovieProperty(m_movieRef, kQTPropertyClass_DRM, kQTDRMPropertyID_InteractWithUser, sizeof(boolFalse), &boolFalse);
    }
}

MediaSource QuickTimeVideoPlayer::mediaSource() const
{
    return m_mediaSource;
}

Movie QuickTimeVideoPlayer::movieRef() const
{
    return m_movieRef;
}

void QuickTimeVideoPlayer::setPlaybackRate(float rate)
{
    m_playbackRateSat = true;
    m_playbackRate = rate;
    if (m_movieRef)
        SetMovieRate(m_movieRef, FloatToFixed(m_playbackRate));
}

float QuickTimeVideoPlayer::playbackRate() const
{
    return m_playbackRate;
}

quint64 QuickTimeVideoPlayer::currentTime() const
{
    if (!m_movieRef || m_state == Paused)
        return m_currentTime;

    TimeRecord timeRec;
    GetMovieTime (m_movieRef, &timeRec);
    quint64 t = static_cast<quint64>(float(timeRec.value.lo) / float(timeRec.scale) * 1000.0f);
    const_cast<QuickTimeVideoPlayer *>(this)->m_currentTime = t;
    return m_currentTime;
}

QString QuickTimeVideoPlayer::timeToString(quint64 ms)
{
    int sec = ms/1000;
    int min = sec/60;
    int hour = min/60;
    return QString(QLatin1String("%1:%2:%3:%4")).arg(hour%60).arg(min%60).arg(sec%60).arg(ms%1000);
}

QString QuickTimeVideoPlayer::currentTimeString()
{
    return timeToString(currentTime());
}

quint64 QuickTimeVideoPlayer::duration() const
{
    if (!m_movieRef)
        return 0;

    return static_cast<quint64>(float(GetMovieDuration(m_movieRef)) / float(GetMovieTimeScale(m_movieRef)) * 1000.0f);
}

void QuickTimeVideoPlayer::play()
{
    if (!canPlayMedia())
        return;

    m_state = Playing;
    enableAudio(m_audioEnabled);
    setMute(m_mute);
    
    if (isSeekable()){
        // The following lines is a work-around for
        // audio-starting-late-bug when playing mpeg:
        TimeRecord timeRec;
        GetMovieTime(m_movieRef, &timeRec);
        SetMovieTime(m_movieRef, &timeRec);
    }
    SetMovieRate(m_movieRef, FloatToFixed(m_playbackRate));    
}

void QuickTimeVideoPlayer::pause()
{
    if (!canPlayMedia())
        return;

    currentTime();
    m_state = Paused;

    if (isSeekable())
        SetMovieRate(m_movieRef, FloatToFixed(0.0f));
    else // pretend to be paused:
        SetMovieAudioMute(m_movieRef, true, 0);
}

void QuickTimeVideoPlayer::seek(quint64 milliseconds)
{
    if (!canPlayMedia() || !isSeekable() || milliseconds == currentTime())
        return;
    if (milliseconds > duration())
        milliseconds = duration();

    m_currentTime = milliseconds;
    TimeRecord timeRec;
    GetMovieTime(m_movieRef, &timeRec);
	timeRec.value.lo = (milliseconds / 1000.0f) * timeRec.scale;

    if (milliseconds < timeLoaded()){
        SetMovieTime(m_movieRef, &timeRec);
    } else {
        SetMovieRate(m_movieRef, FloatToFixed(0.0f));
        preRollMovie(timeRec.value.lo);
        SetMovieTime(m_movieRef, &timeRec);
        if (m_state == Playing)
            SetMovieRate(m_movieRef, FloatToFixed(m_playbackRate));
    }  
}

bool QuickTimeVideoPlayer::canPlayMedia() const
{
    if (!m_movieRef)
        return false;
    return m_isDrmAuthorized;
}

bool QuickTimeVideoPlayer::isPlaying() const
{
    return m_state == Playing;
}

bool QuickTimeVideoPlayer::isSeekable() const
{
    return canPlayMedia() && (duration()-1) != INT_MAX;
}

float QuickTimeVideoPlayer::prefferedPlaybackRate() const
{
    if (!m_movieRef)
        return -1;

    return FixedToFloat(GetMoviePreferredRate(m_movieRef));
}

void MoviePrePrerollCompleteCallBack(Movie /*theMovie*/, OSErr /*thePrerollErr*/, void */*userData*/)
{
    // QuickTimeVideoPlayer *player = static_cast<QuickTimeVideoPlayer *>(userData);
}

bool QuickTimeVideoPlayer::preRollMovie(qint64 startTime)
{
    if (!canPlayMedia())
        return false;

    if (PrePrerollMovie(m_movieRef, startTime, FloatToFixed(m_playbackRate),
        0 /*MoviePrePrerollCompleteCallBack*/, this) != noErr) // No callback means wait (synch)
        return false;

    if (PrerollMovie(m_movieRef, startTime, FloatToFixed(m_playbackRate)) != noErr)
        return false;

    return true;
}

bool QuickTimeVideoPlayer::hasAudio() const
{
    if (!m_movieRef)
        return false;

    Track track = GetMovieIndTrackType(
        m_movieRef, 1, AudioMediaCharacteristic, movieTrackCharacteristic | movieTrackEnabledOnly);
    return (track != 0);
}

bool QuickTimeVideoPlayer::hasVideo() const
{
    return m_hasVideo;
}

void QuickTimeVideoPlayer::checkIfVideoAwailable()
{
    Track track = GetMovieIndTrackType(
        m_movieRef, 1, VisualMediaCharacteristic, movieTrackCharacteristic | movieTrackEnabledOnly);
    m_hasVideo = bool(track != 0);
}

bool QuickTimeVideoPlayer::isDrmProtected() const
{
    return m_isDrmProtected;
}

bool QuickTimeVideoPlayer::isDrmAuthorized() const
{
    return m_isDrmAuthorized;
}

void QuickTimeVideoPlayer::readProtection()
{
    if (!m_movieRef){
        m_isDrmProtected = false;
        m_isDrmAuthorized = true;
        return;
    }
        
    Track track = GetMovieIndTrackType(m_movieRef, 1,
        AudioMediaCharacteristic, movieTrackCharacteristic | movieTrackEnabledOnly);
    if (track){
        getTrackProtection(track, m_isDrmProtected, m_isDrmAuthorized);
    } else {        
        track = GetMovieIndTrackType(m_movieRef, 1,
            VisualMediaCharacteristic, movieTrackCharacteristic | movieTrackEnabledOnly);
        if (track){
            getTrackProtection(track, m_isDrmProtected, m_isDrmAuthorized);
        }
    }    
}

void QuickTimeVideoPlayer::getTrackProtection(const Track &track, bool &isDrmProtected, bool &isDrmAuthorized) const
{
    isDrmProtected = false;
    isDrmAuthorized = true;
    
    if (track){
        Media media = GetTrackMedia(track);
        if (media){
            MediaHandler mediaHandler = GetMediaHandler(media);
            if (mediaHandler){
                // Regardless, skip message boxes pointing to iTunes regarding DRM:
                Boolean boolFalse = false;
                QTSetComponentProperty(mediaHandler, 
                    kQTPropertyClass_DRM, kQTDRMPropertyID_InteractWithUser,
                    sizeof(boolFalse), &boolFalse);

                // Check track:
                Boolean value;
                OSStatus err = QTGetComponentProperty(mediaHandler,
                    kQTPropertyClass_DRM, kQTDRMPropertyID_IsProtected,
                    sizeof(value), &value, 0);
                isDrmProtected = (err == noErr) ? bool(value) : false;
                err = QTGetComponentProperty(mediaHandler,
                    kQTPropertyClass_DRM, kQTDRMPropertyID_IsAuthorized,
                    sizeof(value), &value, 0);
                isDrmAuthorized = (err == noErr) ? bool(value) : true;
            }
        }
    }
}

}}

QT_END_NAMESPACE
