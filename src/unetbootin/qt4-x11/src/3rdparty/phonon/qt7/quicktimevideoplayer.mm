/*  This file is part of the KDE project.

    Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).

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
#include "mediaobject.h"
#include "videowidget.h"
#include "audiodevice.h"
#include "quicktimestreamreader.h"

#include <QGLContext>
#include <QtOpenGL/private/qgl_p.h>

#import <QTKit/QTTrack.h>
#import <QTKit/QTMedia.h>
#import <QuartzCore/CIContext.h>
#import <QuartzCore/CIFilter.h>

#ifdef QUICKTIME_C_API_AVAILABLE
    #include <QuickTime/QuickTime.h>
    #undef check // avoid name clash;
    #include <AGL/agl.h>
#endif

QT_BEGIN_NAMESPACE

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
    m_QTMovie = 0;
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
	m_primaryRenderingTarget = 0;
	m_primaryRenderingCIImage = 0;
    m_QImagePixelBuffer = 0;

#ifdef QUICKTIME_C_API_AVAILABLE
    OSStatus err = EnterMovies();
    BACKEND_ASSERT2(err == noErr, "Could not initialize QuickTime", FATAL_ERROR)
	createVisualContext();
#endif
}

QuickTimeVideoPlayer::~QuickTimeVideoPlayer()
{
    unsetVideo();
    [(NSObject*)m_primaryRenderingTarget release];
    m_primaryRenderingTarget = 0;
#ifdef QUICKTIME_C_API_AVAILABLE
    if (m_visualContext)
        CFRelease(m_visualContext);
#endif
}

void QuickTimeVideoPlayer::createVisualContext()
{
#ifdef QUICKTIME_C_API_AVAILABLE
	PhononSharedQGLWidget()->makeCurrent();

	PhononAutoReleasePool pool;
    CGLContextObj cglContext = CGLGetCurrentContext();
	NSOpenGLPixelFormat *nsglPixelFormat = [NSOpenGLView defaultPixelFormat];
    CGLPixelFormatObj cglPixelFormat = static_cast<CGLPixelFormatObj>([nsglPixelFormat CGLPixelFormatObj]);
	BACKEND_ASSERT2(cglContext, "Could not get current CoreVideo GL context (OpenGL)", FATAL_ERROR)
	BACKEND_ASSERT2(cglPixelFormat, "Could not get current CoreVideo pixel format (OpenGL)", FATAL_ERROR)

    CFTypeRef keys[] = { kQTVisualContextWorkingColorSpaceKey };
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    CFDictionaryRef textureContextAttributes = CFDictionaryCreate(kCFAllocatorDefault,
        (const void **)keys,
        (const void **)&colorSpace, 1,
        &kCFTypeDictionaryKeyCallBacks,
        &kCFTypeDictionaryValueCallBacks);

	OSStatus err = QTOpenGLTextureContextCreate(kCFAllocatorDefault, cglContext,
        cglPixelFormat, textureContextAttributes, &m_visualContext);
    CFRelease(textureContextAttributes);
    BACKEND_ASSERT2(err == noErr, "Could not create visual context (OpenGL)", FATAL_ERROR)
#endif // QUICKTIME_C_API_AVAILABLE
}

bool QuickTimeVideoPlayer::videoFrameChanged()
{
    if (!m_QTMovie || !m_hasVideo)
        return false;

#ifdef QUICKTIME_C_API_AVAILABLE
	if (m_primaryRenderingTarget)
		return true;
    if (!m_visualContext)
		return false;

    QTVisualContextTask(m_visualContext);
    return QTVisualContextIsNewImageAvailable(m_visualContext, 0);

#elif defined(QT_MAC_USE_COCOA)
    return true;

#else
    return false;
#endif
}

CVOpenGLTextureRef QuickTimeVideoPlayer::currentFrameAsCVTexture()
{
#ifdef QUICKTIME_C_API_AVAILABLE
    if (!m_visualContext)
        return 0;
    CVOpenGLTextureRef texture = 0;
    OSStatus err = QTVisualContextCopyImageForTime(m_visualContext, 0, 0, &texture);
    BACKEND_ASSERT3(err == noErr, "Could not copy image for time in QuickTime player", FATAL_ERROR, 0)
    return texture;

#else
    return 0;
#endif
}

QImage QuickTimeVideoPlayer::currentFrameAsQImage()
{
#ifdef QUICKTIME_C_API_AVAILABLE
    QGLContext *prevContext = const_cast<QGLContext *>(QGLContext::currentContext());
    CVOpenGLTextureRef texture = currentFrameAsCVTexture();
    GLenum target = CVOpenGLTextureGetTarget(texture);
    GLfloat lowerLeft[2], lowerRight[2], upperRight[2], upperLeft[2];

    if (!m_QImagePixelBuffer){
        m_QImagePixelBuffer = new QGLPixelBuffer(videoRect().size(), QGLFormat::defaultFormat(), PhononSharedQGLWidget());
        m_QImagePixelBuffer->makeCurrent();
        glEnable(target);
        glDisable(GL_BLEND);
        glDisable(GL_CULL_FACE);
    } else {
        m_QImagePixelBuffer->makeCurrent();
    }

    CVOpenGLTextureGetCleanTexCoords(texture, upperLeft, upperRight, lowerRight, lowerLeft);
    glBindTexture(target, CVOpenGLTextureGetName(texture));
    glBegin(GL_QUADS);
        glTexCoord2f(lowerLeft[0], lowerLeft[1]);
        glVertex2i(-1, 1);
        glTexCoord2f(lowerRight[0], lowerRight[1]);
        glVertex2i(1, 1);
        glTexCoord2f(upperRight[0], upperRight[1]);
        glVertex2i(1, -1);
        glTexCoord2f(upperLeft[0], upperLeft[1]);
        glVertex2i(-1, -1);
    glEnd();

    QImage image = m_QImagePixelBuffer->toImage();
    CVOpenGLTextureRelease(texture);
    // Because of QuickTime, m_QImagePixelBuffer->doneCurrent() will fail.
    // So we store, and restore, the context our selves:
    prevContext->makeCurrent();
    return image;
#else
	CIImage *img = (CIImage *)currentFrameAsCIImage();
	if (!img)
		return QImage();

	NSBitmapImageRep* bitmap = [[NSBitmapImageRep alloc] initWithCIImage:img];
	CGRect bounds = [img extent];
	QImage qImg([bitmap bitmapData], bounds.size.width, bounds.size.height, QImage::Format_ARGB32);
	QImage swapped = qImg.rgbSwapped();
	[bitmap release];
	[img release];
	return swapped;
#endif
}

void QuickTimeVideoPlayer::setPrimaryRenderingCIImage(void *ciImage)
{
	[(CIImage *)m_primaryRenderingCIImage release];
	m_primaryRenderingCIImage = ciImage;
	[(CIImage *)m_primaryRenderingCIImage retain];
}

void QuickTimeVideoPlayer::setPrimaryRenderingTarget(NSObject *target)
{
	[(NSObject*)m_primaryRenderingTarget release];
	m_primaryRenderingTarget = target;
	[(NSObject*)m_primaryRenderingTarget retain];
}

void *QuickTimeVideoPlayer::primaryRenderingCIImage()
{
	return m_primaryRenderingCIImage;
}

void *QuickTimeVideoPlayer::currentFrameAsCIImage()
{
    if (!m_QTMovie)
        return 0;

#if defined(QT_MAC_USE_COCOA)
	if (m_primaryRenderingCIImage){
		CIImage *img = (CIImage *)m_primaryRenderingCIImage;
		if (m_brightness || m_contrast || m_saturation){
			CIFilter *colorFilter = [CIFilter filterWithName:@"CIColorControls"];
			[colorFilter setValue:[NSNumber numberWithFloat:m_brightness] forKey:@"inputBrightness"];
			[colorFilter setValue:[NSNumber numberWithFloat:(m_contrast < 1) ? m_contrast : 1 + ((m_contrast-1)*3)] forKey:@"inputContrast"];
			[colorFilter setValue:[NSNumber numberWithFloat:m_saturation] forKey:@"inputSaturation"];
			[colorFilter setValue:img forKey:@"inputImage"];
			img = [colorFilter valueForKey:@"outputImage"];
		}
		if (m_hue){
			CIFilter *colorFilter = [CIFilter filterWithName:@"CIHueAdjust"];
			[colorFilter setValue:[NSNumber numberWithFloat:(m_hue * 3.14)] forKey:@"inputAngle"];
			[colorFilter setValue:img forKey:@"inputImage"];
			img = [colorFilter valueForKey:@"outputImage"];
		}
		return [img retain];
	}
#endif

#ifdef QUICKTIME_C_API_AVAILABLE
	CVOpenGLTextureRef cvImg = currentFrameAsCVTexture();
	CIImage *img = [[CIImage alloc] initWithCVImageBuffer:cvImg];
	CVOpenGLTextureRelease(cvImg);
	return img;	
#else
	return 0;
#endif
}

GLuint QuickTimeVideoPlayer::currentFrameAsGLTexture()
{
	CIImage *img = (CIImage *)currentFrameAsCIImage();
	if (!img)
		return 0;

	NSBitmapImageRep* bitmap = [[NSBitmapImageRep alloc] initWithCIImage:img];
    GLuint texName = 0;
    glPixelStorei(GL_UNPACK_ROW_LENGTH, [bitmap pixelsWide]);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &texName);
    glBindTexture(GL_TEXTURE_RECTANGLE_EXT, texName);
    glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_MIN_FILTER,  GL_LINEAR);

    int samplesPerPixel = [bitmap samplesPerPixel];
    if (![bitmap isPlanar] && (samplesPerPixel == 3 || samplesPerPixel == 4)){
        glTexImage2D(GL_TEXTURE_RECTANGLE_EXT, 0, 
            samplesPerPixel == 4 ? GL_RGBA8 : GL_RGB8,
            [bitmap pixelsWide], [bitmap pixelsHigh],
            0, samplesPerPixel == 4 ? GL_RGBA : GL_RGB,
            GL_UNSIGNED_BYTE, [bitmap bitmapData]);
    } else {
        // Handle other bitmap formats.
    }

    [bitmap release];
	[img release];
    return texName;
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
    if (!m_QTMovie || !m_audioEnabled || m_mute)
        return;                
    [m_QTMovie setVolume:(m_masterVolume * m_relativeVolume)];
}

void QuickTimeVideoPlayer::setMute(bool mute)
{
    m_mute = mute;
    if (!m_QTMovie || m_state != Playing || !m_audioEnabled)
        return;

    // Work-around bug that happends if you set/unset mute
    // before movie is playing, and audio is not played 
    // through graph. Then audio is delayed.
    [m_QTMovie setMuted:mute];
    [m_QTMovie setVolume:(mute ? 0 : m_masterVolume * m_relativeVolume)];
}

void QuickTimeVideoPlayer::enableAudio(bool enable)
{
    m_audioEnabled = enable;
    if (!m_QTMovie || m_state != Playing)
        return;

    // Work-around bug that happends if you set/unset mute
    // before movie is playing, and audio is not played 
    // through graph. Then audio is delayed.
    [m_QTMovie setMuted:(!enable || m_mute)];
    [m_QTMovie setVolume:((!enable || m_mute) ? 0 : m_masterVolume * m_relativeVolume)];
}

bool QuickTimeVideoPlayer::audioEnabled()
{
    return m_audioEnabled;
}

bool QuickTimeVideoPlayer::setAudioDevice(int id)
{
    if (!m_QTMovie)
        return false;

#ifdef QUICKTIME_C_API_AVAILABLE
    // The following code will not work for some media codecs that
    // typically mingle audio/video frames (e.g mpeg).
    CFStringRef idString = PhononCFString::toCFStringRef(AudioDevice::deviceUID(id));        
    QTAudioContextRef context;
    QTAudioContextCreateForAudioDevice(kCFAllocatorDefault, idString, 0, &context);
    OSStatus err = SetMovieAudioContext([m_QTMovie quickTimeMovie], context);
    CFRelease(context);
    if (err != noErr)
        return false;
    return true;
#else
    Q_UNUSED(id);
    return false;
#endif
}

void QuickTimeVideoPlayer::setColors(qreal brightness, qreal contrast, qreal hue, qreal saturation)
{
    if (!m_QTMovie)
        return;

    // 0 is default value for the colors
    // in phonon, so adjust scale:
    contrast += 1;
    saturation += 1;

	m_brightness = brightness;
	m_contrast = contrast;
	m_hue = hue;
	m_saturation = saturation;
	
#ifdef QUICKTIME_C_API_AVAILABLE
    Float32 value;
    value = brightness;
    SetMovieVisualBrightness([m_QTMovie quickTimeMovie], value, 0);
    value = contrast;
    SetMovieVisualContrast([m_QTMovie quickTimeMovie], value, 0);
    value = hue;
    SetMovieVisualHue([m_QTMovie quickTimeMovie], value, 0);
    value = saturation;
    SetMovieVisualSaturation([m_QTMovie quickTimeMovie], value, 0);
#endif
}

QRect QuickTimeVideoPlayer::videoRect() const
{
    if (!m_QTMovie)
        return QRect();

	PhononAutoReleasePool pool;
    NSSize size = [[m_QTMovie attributeForKey:@"QTMovieCurrentSizeAttribute"] sizeValue];
    return QRect(0, 0, size.width, size.height);
}

void QuickTimeVideoPlayer::unsetVideo()
{
    if (!m_QTMovie)
        return;

    [m_QTMovie release];
	m_QTMovie = 0;
    delete m_streamReader;
    m_streamReader = 0;
    m_currentTime = 0;
    m_state = NoMedia;
    m_isDrmProtected = false;
    m_isDrmAuthorized = true;
    m_mediaSource = MediaSource();
	[(CIImage *)m_primaryRenderingCIImage release];
	m_primaryRenderingCIImage = 0;
    delete m_QImagePixelBuffer;
    m_QImagePixelBuffer = 0;
}

QuickTimeVideoPlayer::State QuickTimeVideoPlayer::state() const
{
    return m_state;
}

quint64 QuickTimeVideoPlayer::timeLoaded()
{
    if (!m_QTMovie)
        return 0;
#ifdef QUICKTIME_C_API_AVAILABLE
    TimeValue value;
    GetMaxLoadedTimeInMovie([m_QTMovie quickTimeMovie], &value);
    quint64 loaded = static_cast<quint64>(float(value) / float(GetMovieTimeScale([m_QTMovie quickTimeMovie])) * 1000.0f);
    return (loaded == INT_MAX) ? 0 : loaded;
#else
    return 0;
#endif
}

float QuickTimeVideoPlayer::percentageLoaded()
{
    if (!m_QTMovie || !isSeekable())
        return 0;
#ifdef QUICKTIME_C_API_AVAILABLE
    TimeValue loaded;
    GetMaxLoadedTimeInMovie([m_QTMovie quickTimeMovie], &loaded);
    float duration = GetMovieDuration([m_QTMovie quickTimeMovie]);
    return duration ? float(loaded) / duration : 0;
#else
    return 0;
#endif
}

void QuickTimeVideoPlayer::waitStatePlayable()
{
#if defined(QT_MAC_USE_COCOA)
    long state = [[m_QTMovie attributeForKey:@"QTMovieLoadStateAttribute"] longValue];
    while (state != QTMovieLoadStateError && state < QTMovieLoadStatePlayable)
        state = [[m_QTMovie attributeForKey:@"QTMovieLoadStateAttribute"] longValue];
#elif defined(QUICKTIME_C_API_AVAILABLE)
    long state = GetMovieLoadState([m_QTMovie quickTimeMovie]);
    while (state != kMovieLoadStateError && state < kMovieLoadStatePlayable){
        MoviesTask(0, 0);
        state = GetMovieLoadState([m_QTMovie quickTimeMovie]);
    }
#endif
}

bool QuickTimeVideoPlayer::movieNotLoaded()
{
    if (!m_QTMovie)
        return true;

#if defined(QT_MAC_USE_COCOA)
    long state = [[m_QTMovie attributeForKey:@"QTMovieLoadStateAttribute"] longValue];
    return state == QTMovieLoadStateError;
#elif defined(QUICKTIME_C_API_AVAILABLE)
    long state = GetMovieLoadState([m_QTMovie quickTimeMovie]);
    return state == kMovieLoadStateError;
#endif
}

void QuickTimeVideoPlayer::setError(NSError *error)
{
    if (!error)
        return;
    QString desc = QString::fromUtf8([[error localizedDescription] UTF8String]);
    if (desc == "The file is not a movie file.")
        desc = QLatin1String("Could not decode media source.");
    else if (desc == "A necessary data reference could not be resolved."){
		if (codecExistsAccordingToSuffix(mediaSourcePath()))
            desc = QLatin1String("Could not locate media source.");
		else
            desc = QLatin1String("Could not decode media source.");
    } else if (desc == "You do not have sufficient permissions for this operation.")
        desc = QLatin1String("Could not open media source.");
    SET_ERROR(desc, FATAL_ERROR)
}

bool QuickTimeVideoPlayer::errorOccured()
{
    if (gGetErrorType() != NO_ERROR){
        return true;
    } else if (movieNotLoaded()){
        SET_ERROR("Could not open media source.", FATAL_ERROR)
        return true;
    }
	return false;
}

bool QuickTimeVideoPlayer::codecExistsAccordingToSuffix(const QString &fileName)
{
	PhononAutoReleasePool pool;
	NSArray *fileTypes = [QTMovie movieFileTypes:QTIncludeAllTypes];
	for (uint i=0; i<[fileTypes count]; ++i){
		NSString *type = [fileTypes objectAtIndex:i];
		QString formattedType = QString::fromUtf8([type UTF8String]);
		formattedType.remove('\'').remove('.');
		if (fileName.endsWith(QChar('.') + formattedType, Qt::CaseInsensitive))
			return true;
	}
	return false;
}

void QuickTimeVideoPlayer::setMediaSource(const MediaSource &mediaSource)
{
    PhononAutoReleasePool pool;
    unsetVideo();
    m_mediaSource = mediaSource;
    if (mediaSource.type() == MediaSource::Empty || mediaSource.type() == MediaSource::Invalid){
        m_state = NoMedia;
        return;
    }
    openMovieFromCurrentMediaSource();
    if (errorOccured()){
        unsetVideo();
        return;
    }

#ifdef QUICKTIME_C_API_AVAILABLE
    if (m_visualContext)
        SetMovieVisualContext([m_QTMovie quickTimeMovie], m_visualContext);
#endif

    waitStatePlayable();
    if (errorOccured()){
        unsetVideo();
        return;
    }

    readProtection();
    preRollMovie();
    if (errorOccured()){
        unsetVideo();
        return;
    }

    if (!m_playbackRateSat)
        m_playbackRate = prefferedPlaybackRate();
    checkIfVideoAwailable();
    enableAudio(m_audioEnabled);
    setMute(m_mute);
    setVolume(m_masterVolume, m_relativeVolume);
    pause();
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
    case MediaSource::Empty:
    case MediaSource::Invalid:
        break;
    }
}

QString QuickTimeVideoPlayer::mediaSourcePath()
{
    switch (m_mediaSource.type()){
    case MediaSource::LocalFile:{
        QFileInfo fileInfo(m_mediaSource.fileName());
        return fileInfo.isSymLink() ? fileInfo.symLinkTarget() : fileInfo.canonicalFilePath();
        break;}
    case MediaSource::Url:
		return m_mediaSource.url().toEncoded();
        break;
    default:
        break;
    }
	return QString();
}

void QuickTimeVideoPlayer::openMovieFromDataRef(QTDataReference *dataRef)
{
    PhononAutoReleasePool pool;
    NSDictionary *attr = [NSDictionary dictionaryWithObjectsAndKeys:
                dataRef, QTMovieDataReferenceAttribute,
                [NSNumber numberWithBool:YES], QTMovieOpenAsyncOKAttribute,
                [NSNumber numberWithBool:YES], QTMovieIsActiveAttribute,
                [NSNumber numberWithBool:YES], QTMovieResolveDataRefsAttribute,
                [NSNumber numberWithBool:YES], QTMovieDontInteractWithUserAttribute,
                nil];

    NSError *err = 0;
    m_QTMovie = [[QTMovie movieWithAttributes:attr error:&err] retain];
    if (err){
        [m_QTMovie release];
        m_QTMovie = 0;
        setError(err);
    }
}

void QuickTimeVideoPlayer::openMovieFromData(QByteArray *data, char *fileType)
{
    PhononAutoReleasePool pool;
    NSString *type = [NSString stringWithUTF8String:fileType];
    NSData *nsData = [NSData dataWithBytesNoCopy:data->data() length:data->size() freeWhenDone:NO];
    QTDataReference *dataRef = [QTDataReference dataReferenceWithReferenceToData:nsData name:type MIMEType:@""];
    openMovieFromDataRef(dataRef);
}

void QuickTimeVideoPlayer::openMovieFromDataGuessType(QByteArray *data)
{
    // It turns out to be better to just try the standard file types rather
    // than using e.g [QTMovie movieFileTypes:QTIncludeCommonTypes]. Some
    // codecs *think* they can decode the stream, and crash...
#define TryOpenMovieWithCodec(type) gClearError(); \
    openMovieFromData(data, "."type); \
    if (m_QTMovie) return;

    TryOpenMovieWithCodec("avi");
    TryOpenMovieWithCodec("mp4");
    TryOpenMovieWithCodec("m4p");
    TryOpenMovieWithCodec("m1s");
    TryOpenMovieWithCodec("mp3");
    TryOpenMovieWithCodec("mpeg");
    TryOpenMovieWithCodec("mov");
    TryOpenMovieWithCodec("ogg");
    TryOpenMovieWithCodec("wav");
    TryOpenMovieWithCodec("wmv");
#undef TryOpenMovieWithCodec(type)
}

void QuickTimeVideoPlayer::openMovieFromFile()
{
    NSString *nsFilename = (NSString *)PhononCFString::toCFStringRef(mediaSourcePath());
    QTDataReference *dataRef = [QTDataReference dataReferenceWithReferenceToFile:nsFilename];
    openMovieFromDataRef(dataRef);
}

void QuickTimeVideoPlayer::openMovieFromUrl()
{
    PhononAutoReleasePool pool;
    NSString *urlString = (NSString *)PhononCFString::toCFStringRef(mediaSourcePath());
    NSURL *url = [NSURL URLWithString: urlString];
    QTDataReference *dataRef = [QTDataReference dataReferenceWithReferenceToURL:url];
    openMovieFromDataRef(dataRef);
}

void QuickTimeVideoPlayer::openMovieFromStream()
{
    m_streamReader = new QuickTimeStreamReader(m_mediaSource);
    if (!m_streamReader->readAllData())
        return;
    openMovieFromDataGuessType(m_streamReader->pointerToData());
}

MediaSource QuickTimeVideoPlayer::mediaSource() const
{
    return m_mediaSource;
}

QTMovie *QuickTimeVideoPlayer::qtMovie() const
{
    return m_QTMovie;
}

void QuickTimeVideoPlayer::setPlaybackRate(float rate)
{
	PhononAutoReleasePool pool;
    m_playbackRateSat = true;
    m_playbackRate = rate;
    if (m_QTMovie)
        [m_QTMovie setRate:m_playbackRate];
}

float QuickTimeVideoPlayer::playbackRate() const
{
    return m_playbackRate;
}

quint64 QuickTimeVideoPlayer::currentTime() const
{
    if (!m_QTMovie || m_state == Paused)
        return m_currentTime;

	PhononAutoReleasePool pool;
    QTTime qtTime = [m_QTMovie currentTime];
    quint64 t = static_cast<quint64>(float(qtTime.timeValue) / float(qtTime.timeScale) * 1000.0f);
    const_cast<QuickTimeVideoPlayer *>(this)->m_currentTime = t;
    return m_currentTime;
}

long QuickTimeVideoPlayer::timeScale() const
{
    if (!m_QTMovie)
        return 0;

	PhononAutoReleasePool pool;
    return [[m_QTMovie attributeForKey:@"QTMovieTimeScaleAttribute"] longValue];
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
    if (!m_QTMovie)
        return 0;

	PhononAutoReleasePool pool;
    QTTime qtTime = [m_QTMovie duration];
    return static_cast<quint64>(float(qtTime.timeValue) / float(qtTime.timeScale) * 1000.0f);
}

void QuickTimeVideoPlayer::play()
{
    if (!canPlayMedia())
        return;

	PhononAutoReleasePool pool;
    m_state = Playing;
    enableAudio(m_audioEnabled);
    setMute(m_mute);
    [m_QTMovie setRate:m_playbackRate];
}

void QuickTimeVideoPlayer::pause()
{
    if (!canPlayMedia())
        return;

	PhononAutoReleasePool pool;
    currentTime();
    m_state = Paused;

    if (isSeekable())
        [m_QTMovie setRate:0];
    else // pretend to be paused:
        [m_QTMovie setMuted:0];
}

void QuickTimeVideoPlayer::seek(quint64 milliseconds)
{
    if (!canPlayMedia() || !isSeekable() || milliseconds == currentTime())
        return;
    if (milliseconds > duration())
        milliseconds = duration();

	PhononAutoReleasePool pool;
    QTTime newQTTime = [m_QTMovie currentTime];
    newQTTime.timeValue = (milliseconds / 1000.0f) * newQTTime.timeScale;
    [m_QTMovie setCurrentTime:newQTTime];

    // The movie might not have been able to seek
    // to the exact point we told it to. So set
    // the current time according to what the movie says:
    newQTTime = [m_QTMovie currentTime];
    m_currentTime = static_cast<quint64>
        (float(newQTTime.timeValue) / float(newQTTime.timeScale) * 1000.0f);

    if (m_state == Paused){
        // We need (for reasons unknown) to task
        // the movie twize to make sure that
        // a subsequent call to frameAsCvTexture
        // returns the correct frame:
#ifdef QUICKTIME_C_API_AVAILABLE
        MoviesTask(0, 0);
        MoviesTask(0, 0);
#elif defined(QT_MAC_USE_COCOA)
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents | QEventLoop::ExcludeSocketNotifiers);
#endif
    }
}

bool QuickTimeVideoPlayer::canPlayMedia() const
{
    if (!m_QTMovie)
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
    if (!m_QTMovie)
        return 0;

	PhononAutoReleasePool pool;
    return [[m_QTMovie attributeForKey:@"QTMoviePreferredRateAttribute"] floatValue];
}

#ifdef QUICKTIME_C_API_AVAILABLE
void MoviePrePrerollCompleteCallBack(Movie /*theMovie*/, OSErr /*thePrerollErr*/, void * /*userData*/)
{
    // QuickTimeVideoPlayer *player = static_cast<QuickTimeVideoPlayer *>(userData);
}
#endif

bool QuickTimeVideoPlayer::preRollMovie(qint64 startTime)
{
    if (!canPlayMedia())
        return false;

#ifdef QUICKTIME_C_API_AVAILABLE
    if (PrePrerollMovie([m_QTMovie quickTimeMovie], startTime, FloatToFixed(m_playbackRate),
        0 /*MoviePrePrerollCompleteCallBack*/, this) != noErr) // No callback means wait (synch)
        return false;

    if (PrerollMovie([m_QTMovie quickTimeMovie], startTime, FloatToFixed(m_playbackRate)) != noErr)
        return false;

    return true;
#else
    Q_UNUSED(startTime);
    return false;
#endif
}

bool QuickTimeVideoPlayer::hasAudio() const
{
    if (!m_QTMovie)
        return false;

	PhononAutoReleasePool pool;
    return [[m_QTMovie attributeForKey:@"QTMovieHasAudioAttribute"] boolValue] == YES;
}

bool QuickTimeVideoPlayer::hasVideo() const
{
    return m_hasVideo;
}

bool QuickTimeVideoPlayer::hasMovie() const
{
    return m_QTMovie != 0;
}

void QuickTimeVideoPlayer::checkIfVideoAwailable()
{
	PhononAutoReleasePool pool;
    m_hasVideo = [[m_QTMovie attributeForKey:@"QTMovieHasVideoAttribute"] boolValue] == YES;
}

bool QuickTimeVideoPlayer::isDrmProtected() const
{
    return m_isDrmProtected;
}

bool QuickTimeVideoPlayer::isDrmAuthorized() const
{
    return m_isDrmAuthorized;
}
/*
void QuickTimeVideoPlayer::movieCodecIsMPEG()
{
    NSArray *tracks = [m_QTMovie tracks];
    for (QTTrack *track in tracks)
        if ([[track media] hasCharacteristic:QTMediaTypeMPEG])
            return true;
    return false;
}
*/

static void QtGetTrackProtection(QTTrack *track, bool &isDrmProtected, bool &isDrmAuthorized)
{
    isDrmProtected = false;
    isDrmAuthorized = true;

#ifdef QUICKTIME_C_API_AVAILABLE
    QTMedia *media = [track media];
    MediaHandler mediaHandler = GetMediaHandler([media quickTimeMedia]);
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
#else
    Q_UNUSED(track);
#endif // QUICKTIME_C_API_AVAILABLE
}

void QuickTimeVideoPlayer::readProtection()
{
    m_isDrmProtected = false;
    m_isDrmAuthorized = true;

    NSArray *tracks = [m_QTMovie tracks];
	for (uint i=0; i<[tracks count]; ++i){
		QTTrack *track = [tracks objectAtIndex:i];
        bool isDrmProtected = false;
        bool isDrmAuthorized = true;
        QtGetTrackProtection(track, isDrmProtected, isDrmAuthorized);
        if (isDrmProtected)
            m_isDrmProtected = true;
        if (!isDrmAuthorized)
            m_isDrmAuthorized = false;
    }
}

}}

QT_END_NAMESPACE
