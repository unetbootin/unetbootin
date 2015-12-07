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

#include <QtCore/qglobal.h>
#ifdef QT_MAC_USE_COCOA
#import <QTKit/QTMovieLayer.h>
#endif
#undef check

#include "videowidget.h"
#include "backendheader.h"
#include "quicktimevideoplayer.h"
#include "medianode.h"
#include "medianodeevent.h"
#include "mediaobject.h"

#include <QGLWidget>
#include <QTime>

#import <AppKit/NSImage.h>
#import <QTKit/QTMovieView.h>

/////////////////////////////////////////////////////////////////////////////////////////

#ifdef QT_MAC_USE_COCOA // Rendering to a QTMovieView can only be done in Cocoa

#define VIDEO_TRANSPARENT(m) -(void)m:(NSEvent *)e{[[self superview] m:e];}

@interface SharedQTMovieView : QTMovieView
{
@private
    Phonon::QT7::QuickTimeVideoPlayer *m_player;
    QList<QWidget *> *m_parents;
    QWidget *m_window;
    QRect *m_drawRect;
    bool m_newImageReady;
    bool m_usingWindow;
}

- (SharedQTMovieView *) init;
- (void) registerParent:(QWidget *)parent;
- (void) unregisterParent:(QWidget *)parent;
- (void) setDrawRect:(QRect &)rect;
- (void) drawVideoFrame:(Phonon::QT7::VideoFrame &)frame forWidget:(QWidget *)widget shareImages:(bool)share;
- (void) useOffscreenWindow:(bool)offscreen;
- (void) applyDrawRectOnSelf;
@end

/////////////////////////////////////////////////////////////////////////////////////////

@implementation SharedQTMovieView

- (SharedQTMovieView *) init
{
    self = [super initWithFrame:NSZeroRect];
	if (self){
        [self setControllerVisible:NO];
        m_parents = new QList<QWidget *>();
        m_drawRect = new QRect(0, 0, 1, 1);
        [self applyDrawRectOnSelf];
        m_usingWindow = false;
	}
	return self;
}

- (void) dealloc
{
    Phonon::QT7::PhononAutoReleasePool pool;
    delete m_window;
    delete m_drawRect;
    delete m_parents;
    [super dealloc];
}

- (void) applyDrawRectOnSelf
{
    NSRect nsrect;
    nsrect.origin.x = m_drawRect->x();
    nsrect.origin.y = m_drawRect->y();
    nsrect.size.width = m_drawRect->width();
    nsrect.size.height = m_drawRect->height();
    [self setFrame:nsrect];
}

- (void) setDrawRect:(QRect &)rect
{
    *m_drawRect = rect;
    if (!m_usingWindow)
        [self applyDrawRectOnSelf];
}

- (void) waitForFrame
{
    if (m_usingWindow){
        QTMovie *movie = [self movie];
        if (movie){
            // CIImages are expected, but not received.
            // Try to wait a couple of seconds for them:
            m_newImageReady = false;
            float rate = [movie rate];
            if (!rate)
                [movie setRate:1];
            QTime t; t.start();
            while (!m_newImageReady && t.elapsed() < 2000)
                ;
            [movie setRate:rate];
        }
    }
}

- (void) useOffscreenWindow:(bool)offscreen
{
    if (offscreen == m_usingWindow)
        return;
    if (offscreen){
        if (!m_window){
            m_window = new QWidget;
            m_window->setWindowOpacity(0.0);
            m_window->show();
            m_window->hide();
        }
        m_usingWindow = true;
        [self setDelegate:self];
        [self waitForFrame];
        foreach(QWidget *w, *m_parents)
            w->repaint();
        qApp->processEvents();
        [self removeFromSuperview];
        [(NSView *)m_window->winId() addSubview:self];
    } else if (!m_parents->isEmpty()){
        m_usingWindow = false;
        [self removeFromSuperview];
        [(NSView*)m_parents->first()->winId() addSubview:self];
        [self setDelegate:0];
        [self setDrawRect:*m_drawRect];
    }
}

- (void) drawVideoFrame:(Phonon::QT7::VideoFrame &)frame forWidget:(QWidget *)widget shareImages:(bool)share;
{
    // Detect if the video that produces the frame has changed:
    Phonon::QT7::QuickTimeVideoPlayer *player = frame.videoPlayer();
    if (player && player->qtMovie() != [self movie]){
        m_player = player;
        [self setMovie:player->qtMovie()];
        [self waitForFrame];
    }

    [self useOffscreenWindow:(share || m_parents->size() > 1)];
    if (m_usingWindow)
        widget->update();
}

// Override this method so that the movie doesn't stop if
// the window becomes invisible
- (void)viewWillMoveToWindow:(NSWindow *)newWindow
{
    Q_UNUSED(newWindow);
}

- (CIImage *) view:(QTMovieView *)view willDisplayImage:(CIImage *)img
{
	// This method is called from QTMovieView just
	// before the image will be drawn.
    Q_UNUSED(view);
    m_player->setPrimaryRenderingCIImage(img);
    m_newImageReady = true;
	return img;
}

- (void) registerParent:(QWidget *)parent
{
    if (m_parents->contains(parent))
        return;
    m_parents->append(parent);
    if (m_parents->size() == 1){
        Phonon::QT7::PhononAutoReleasePool pool;
        m_usingWindow = true;
        [self applyDrawRectOnSelf];
        [self useOffscreenWindow:NO];
    }
}

- (void) unregisterParent:(QWidget *)parent
{
    m_parents->removeAll(parent);
    if (m_parents->size() == 1)
        [self applyDrawRectOnSelf];
}

VIDEO_TRANSPARENT(mouseDown);
VIDEO_TRANSPARENT(mouseDragged);
VIDEO_TRANSPARENT(mouseUp);
VIDEO_TRANSPARENT(mouseMoved);
VIDEO_TRANSPARENT(mouseEntered);
VIDEO_TRANSPARENT(mouseExited);
VIDEO_TRANSPARENT(rightMouseDown);
VIDEO_TRANSPARENT(rightMouseDragged);
VIDEO_TRANSPARENT(rightMouseUp);
VIDEO_TRANSPARENT(otherMouseDown);
VIDEO_TRANSPARENT(otherMouseDragged);
VIDEO_TRANSPARENT(otherMouseUp);
VIDEO_TRANSPARENT(keyDown);
VIDEO_TRANSPARENT(keyUp);
VIDEO_TRANSPARENT(scrollWheel)

@end

#endif // QT_MAC_USE_COCOA

/////////////////////////////////////////////////////////////////////////////////////////

QT_BEGIN_NAMESPACE

namespace Phonon
{
namespace QT7
{

class IVideoRenderDrawWidget
{
public:
	virtual ~IVideoRenderDrawWidget(){}
	virtual void setVideoFrame(VideoFrame &) = 0;
	virtual void setDrawFrameRect(const QRect &) = 0;
	virtual void updateVideoOutputCount(int){}
	virtual void setMovieIsPaused(bool){}
};

/////////////////////////////////////////////////////////////////////////////////////////

QGLWidget *PhononSharedQGLWidget(){
	static QGLWidget *glWidget = 0;
	if (!glWidget)
		glWidget = new QGLWidget();
	return glWidget;
}

/////////////////////////////////////////////////////////////////////////////////////////

class RenderOpenGL : public QGLWidget, public IVideoRenderDrawWidget
{
public:
    VideoFrame m_currentFrame;
    QRect m_drawFrameRect;

    RenderOpenGL(QWidget *parent, const QGLFormat &format, const QSize &size) : QGLWidget(format, parent, PhononSharedQGLWidget())
    {
        resize(size);
        setAutoFillBackground(false);
        show();
    }

    void initializeGL()
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    }

    void resizeGL(int w, int h)
    {
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glViewport(0, 0, GLsizei(w), GLsizei(h));
        gluOrtho2D(0, GLsizei(w), 0, GLsizei(h));
        updateGL();
    }

    void paintGL()
    {
        glClear(GL_COLOR_BUFFER_BIT);
        m_currentFrame.drawCVTexture(m_drawFrameRect);
    }

    void setVideoFrame(VideoFrame &frame)
    {
        m_currentFrame = frame;
        makeCurrent();
        paintGL();
        swapBuffers();
    }

    void setDrawFrameRect(const QRect &rect)
    {
        m_drawFrameRect = rect;
    }
};

/////////////////////////////////////////////////////////////////////////////////////////

class RenderQTMovieView : public QWidget, public IVideoRenderDrawWidget
{
public:
#if defined(QT_MAC_USE_COCOA)
	QRect m_drawRect;
	VideoFrame m_videoFrame;
    SharedQTMovieView *m_currentView;
    bool m_setDrawRectPending;
    bool m_share;

    RenderQTMovieView(bool share, QWidget *parent, const QSize &size=QSize()) : QWidget(parent), m_currentView(0)
    {
        m_setDrawRectPending = true;
        m_share = share;
        setAutoFillBackground(false);
        if (share){
            // In 'share' mode, this widget will only make sure
            // that CIIImages are produced, and not actually
            // draw anything:
            hide();
        } else {
            resize(size);
            show();
        }
    }

    ~RenderQTMovieView()
    {
        [m_currentView unregisterParent:this];
    }

    void showEvent(QShowEvent *)
    {
        if (m_share)
            return;
        [m_currentView registerParent:this];
    }

    void hideEvent(QHideEvent *)
    {
        if (m_share)
            return;
        [m_currentView unregisterParent:this];
    }

    void paintEvent(QPaintEvent *)
    {
        if (m_share)
            return;
		QPainter p(this);
		p.fillRect(rect(), Qt::black);
        m_videoFrame.drawCIImage(m_drawRect);
    }

	void updateVideoOutputCount(int count)
	{
		Q_UNUSED(count);
	}

	void setMovieIsPaused(bool paused)
	{
        Q_UNUSED(paused);
	}

    void setVideoFrame(VideoFrame &frame)
    {
        m_videoFrame = frame;

        if (!m_videoFrame.isEmpty()){
            Phonon::QT7::QuickTimeVideoPlayer *player = m_videoFrame.videoPlayer();
            if (!player->m_primaryRenderingTarget){
                // First movie view. Create the shared resource:
                SharedQTMovieView *view = [[[SharedQTMovieView alloc] init] autorelease];
                player->setPrimaryRenderingTarget(view);
            }

            SharedQTMovieView *view = static_cast<SharedQTMovieView *>(player->m_primaryRenderingTarget);
            if (!m_share && view != m_currentView){
                [m_currentView unregisterParent:this];
                m_currentView = view;
                [m_currentView registerParent:this];
            }

            [view drawVideoFrame:m_videoFrame forWidget:this shareImages:m_share || m_videoFrame.hasColorAdjustments()];

            if (m_setDrawRectPending){
                m_setDrawRectPending = false;
                [view setDrawRect:m_drawRect];
            }
        }
    }

    void setDrawFrameRect(const QRect &rect)
    {
        m_drawRect = rect;
        Phonon::QT7::QuickTimeVideoPlayer *player = m_videoFrame.videoPlayer();
        if (player && player->m_primaryRenderingTarget){
            SharedQTMovieView *view = static_cast<SharedQTMovieView *>(player->m_primaryRenderingTarget);
            [view setDrawRect:m_drawRect];
        } else
            m_setDrawRectPending = true;
    }

#else // QT_MAC_USE_COCOA == false
	RenderQTMovieView(bool, QWidget *, const QSize& = QSize()){}
	void setVideoFrame(VideoFrame &){}
	void setDrawFrameRect(const QRect &){}
#endif
};

/////////////////////////////////////////////////////////////////////////////////////////

class RenderQTMovieLayer : public QWidget, public IVideoRenderDrawWidget
{
public:
#ifdef QT_MAC_USE_COCOA
    QTMovieLayer *m_movieLayer;

    RenderQTMovieLayer(QWidget *parent, const QSize&) : QWidget(parent)
    {
		PhononAutoReleasePool pool;
        setAutoFillBackground(false);
        m_movieLayer = 0;
        [(NSView *)winId() setWantsLayer:YES];
    }

    void setVideoFrame(VideoFrame &frame)
    {
        QuickTimeVideoPlayer *player = frame.videoPlayer();
        if (!player || player->qtMovie() == [m_movieLayer movie])
            return;

        if (m_movieLayer)
            [m_movieLayer setMovie:player->qtMovie()];
        else {
            m_movieLayer = [QTMovieLayer layerWithMovie:player->qtMovie()];
            [(NSView *)winId() setLayer:m_movieLayer];
        }
    }

    void setDrawFrameRect(const QRect &rect)
    {
        m_movieLayer.frame.origin.x = rect.x();
        m_movieLayer.frame.origin.y = rect.y();
        m_movieLayer.frame.size.width = rect.width();
        m_movieLayer.frame.size.height = rect.height();
    }

#else // QT_MAC_USE_COCOA == false
	RenderQTMovieLayer(QWidget *, const QSize&){}
	void setVideoFrame(VideoFrame &){}
	void setDrawFrameRect(const QRect &){}
#endif
};

/////////////////////////////////////////////////////////////////////////////////////////

class VideoRenderWidget : public QWidget
{
public:
    enum RenderSystem {	RS_NoRendering	= 0,
						RS_QGLWidget	= 1,
						RS_QPainter		= 2,
						RS_CIImage		= 3,
						RS_CVTexture	= 4,
						RS_QImage		= 5,
						RS_QTMovieView	= 6,
						RS_QTMovieLayer = 7
	} m_renderSystem;

    VideoFrame m_currentFrame;
    QRect m_movieFrameRect;
    QRect m_drawFrameRect;
    Phonon::VideoWidget::ScaleMode m_scaleMode;
    Phonon::VideoWidget::AspectRatio m_aspect;
	IVideoRenderDrawWidget *m_renderDrawWidget;

    qreal m_brightness;
    qreal m_contrast;
    qreal m_hue;
    qreal m_saturation;
    qreal m_opacity;

    VideoRenderWidget() : QWidget(0),
        m_scaleMode(Phonon::VideoWidget::FitInView), m_aspect(Phonon::VideoWidget::AspectRatioAuto)
    {
		PhononAutoReleasePool pool;
        m_brightness = 0;
        m_contrast = 0;
        m_hue = 0;
        m_saturation = 0;
        m_opacity = 1;
		m_renderDrawWidget = 0;
		m_renderSystem = RS_NoRendering;

        setAutoFillBackground(false);
        updateDrawFrameRect();
    }

    RenderSystem selectBestRenderSystem(){
        if (!isVisible())
            return RS_NoRendering;
        else if (window() && window()->testAttribute(Qt::WA_DontShowOnScreen))
            return RS_QPainter;
        else {
#ifdef QUICKTIME_C_API_AVAILABLE
            return RS_QGLWidget;
#else
            return RS_QTMovieView;
#endif
        }
    }

    void setRenderSystem(RenderSystem renderSystem){
		PhononAutoReleasePool pool;
		static QString userSystem = qgetenv("PHONON_RENDER_SYSTEM");
	    if (!userSystem.isEmpty())
			renderSystem = RenderSystem(userSystem.toInt());

        if (m_renderSystem == renderSystem)
            return;

        m_renderSystem = renderSystem;
        if (m_renderDrawWidget){
            delete m_renderDrawWidget;
			m_renderDrawWidget = 0;
		}

        switch (m_renderSystem){
            case RS_QGLWidget:{
			    QGLFormat format = QGLFormat::defaultFormat();
			    format.setSwapInterval(1); // Vertical sync (avoid tearing)
			    m_renderDrawWidget = new RenderOpenGL(this, format, size());
                break;}
            case RS_QTMovieView:{
			    m_renderDrawWidget = new RenderQTMovieView(false, this, size());
                break;}
            case RS_QTMovieLayer:{
			    m_renderDrawWidget = new RenderQTMovieLayer(this, size());
				break;}
            case RS_QPainter:
			case RS_CIImage:
			case RS_CVTexture:
			case RS_QImage:
#ifndef QUICKTIME_C_API_AVAILABLE
                // On cocoa-64, let QTMovieView produce
                // video frames for us:
				m_renderDrawWidget = new RenderQTMovieView(true, this);
#endif
				break;
            case RS_NoRendering:
                break;
        }

		if (m_renderDrawWidget){
            m_renderDrawWidget->setVideoFrame(m_currentFrame);
            m_renderDrawWidget->setDrawFrameRect(m_drawFrameRect);
        }
    }

    QSize sizeHint() const
    {
        return m_movieFrameRect.size();
    }

    bool event(QEvent *event)
    {
        switch (event->type()){
            // Try to detect if one of this objects
            // anchestors might have changed:
            case QEvent::Resize:{
                PhononAutoReleasePool pool;
                updateDrawFrameRect();
                if (m_renderDrawWidget)
                    dynamic_cast<QWidget *>(m_renderDrawWidget)->resize(size());
                break; }
            case QEvent::Paint:{
                PhononAutoReleasePool pool;
                float opacity = parentWidget() ? parentWidget()->windowOpacity() : 1;
                switch (m_renderSystem){
                    case RS_QPainter:{
                        QPainter p(this);
                        p.fillRect(rect(), Qt::black);
                        if (p.paintEngine()->type() == QPaintEngine::OpenGL)
                            m_currentFrame.drawCVTexture(m_drawFrameRect, opacity);
                        else
							m_currentFrame.drawQImage(&p, m_drawFrameRect);
                        break; }
					case RS_CIImage:
                        m_currentFrame.drawCIImage(m_drawFrameRect, opacity);
						break;
                    case RS_CVTexture:
	                   m_currentFrame.drawCVTexture(m_drawFrameRect, opacity);
					   break;
					case RS_QImage:{
                        QPainter p(this);
                        p.fillRect(rect(), Qt::black);
						m_currentFrame.drawQImage(&p, m_drawFrameRect);
                        break; }
                    case RS_QGLWidget:
                    case RS_QTMovieView:
                    case RS_QTMovieLayer:
                        // draw in separate widget
                        break;
                    case RS_NoRendering:
                        QPainter p(this);
                        p.fillRect(rect(), Qt::black);
                        break;
                }
                break; }
            default:
                break;
        }

        return QWidget::event(event);
    }

    void setVideoFrame(VideoFrame &frame)
    {
		PhononAutoReleasePool pool;
        m_currentFrame = frame;
        m_currentFrame.setColors(m_brightness, m_contrast, m_hue, m_saturation);

		if (m_renderDrawWidget)
			m_renderDrawWidget->setVideoFrame(m_currentFrame);

        setRenderSystem(selectBestRenderSystem());
        switch (m_renderSystem){
            case RS_QGLWidget:
            case RS_QTMovieView:
            case RS_QTMovieLayer:
            case RS_NoRendering:
                break;
			case RS_CIImage:
			case RS_CVTexture:
			case RS_QImage:
            case RS_QPainter:
                repaint();
                break;
        }
    }

    void updateVideoFrame()
    {
        setVideoFrame(m_currentFrame);
    }

    void setMovieRect(const QRect &mrect)
    {
        if (mrect == m_movieFrameRect)
            return;
        m_movieFrameRect = mrect;
        updateDrawFrameRect();
        updateGeometry();
        if (isVisible())
            qApp->processEvents(QEventLoop::ExcludeUserInputEvents | QEventLoop::ExcludeSocketNotifiers);
    }

    void setScaleMode(Phonon::VideoWidget::ScaleMode scaleMode)
    {
        m_scaleMode = scaleMode;
        updateDrawFrameRect();
        updateVideoFrame();
        repaint();
    }

    void setAspectRatio(Phonon::VideoWidget::AspectRatio aspect)
    {
        m_aspect = aspect;
        updateDrawFrameRect();
        updateVideoFrame();
        repaint();
    }

	void updateVideoOutputCount(int count)
	{
		if (m_renderDrawWidget)
			m_renderDrawWidget->updateVideoOutputCount(count);
	}

	void setMovieIsPaused(bool paused)
	{
		if (m_renderDrawWidget)
			m_renderDrawWidget->setMovieIsPaused(paused);
	}

    void updateDrawFrameRect()
    {
        if (m_movieFrameRect.width() <= 0 || m_movieFrameRect.height() <= 0)
            m_movieFrameRect = QRect(0, 0, 640, 480);

        // Set m_drawFrameRect to be the size of the smallest possible
        // rect conforming to the aspect and containing the whole frame:
        switch(m_aspect){
        case Phonon::VideoWidget::AspectRatioWidget:
            m_drawFrameRect = rect();
            break;
        case Phonon::VideoWidget::AspectRatio4_3:
            m_drawFrameRect = scaleToAspect(m_movieFrameRect, 4, 3);
            break;
        case Phonon::VideoWidget::AspectRatio16_9:
            m_drawFrameRect = scaleToAspect(m_movieFrameRect, 16, 9);
            break;
        case Phonon::VideoWidget::AspectRatioAuto:
        default:
            m_drawFrameRect = m_movieFrameRect;
            break;
        }

        // Scale m_drawFrameRect to fill the widget
        // without breaking aspect:
        int widgetWidth = rect().width();
        int widgetHeight = rect().height();
        int frameWidth = widgetWidth;
        int frameHeight = m_drawFrameRect.height() * float(widgetWidth) / float(m_drawFrameRect.width());

        switch(m_scaleMode){
        case Phonon::VideoWidget::ScaleAndCrop:
            if (frameHeight < widgetHeight){
                frameWidth *= float(widgetHeight) / float(frameHeight);
                frameHeight = widgetHeight;
            }
            break;
        case Phonon::VideoWidget::FitInView:
        default:
            if (frameHeight > widgetHeight){
                frameWidth *= float(widgetHeight) / float(frameHeight);
                frameHeight = widgetHeight;
            }
            break;
        }

        m_drawFrameRect.setSize(QSize(frameWidth, frameHeight));
        m_drawFrameRect.moveTo((widgetWidth - frameWidth) / 2.0f, (widgetHeight - frameHeight) / 2.0f);

		if (m_renderDrawWidget)
			m_renderDrawWidget->setDrawFrameRect(m_drawFrameRect);
    }

    QRect scaleToAspect(QRect srcRect, int w, int h)
    {
        int width = srcRect.width();
        int height = srcRect.width() * (float(h) / float(w));
        if (height > srcRect.height()){
            height = srcRect.height();
            width = srcRect.height() * (float(w) / float(h));
        }
        return QRect(0, 0, width, height);
    }
};

/////////////////////////////////////////////////////////////////////////////////////////

VideoWidget::VideoWidget(QObject *parent) : MediaNode(VideoSink, parent)
{
    m_videoRenderWidget = new VideoRenderWidget();
}

VideoWidget::~VideoWidget()
{
    delete m_videoRenderWidget;
}

QWidget *VideoWidget::widget()
{
    IMPLEMENTED;
    return m_videoRenderWidget;
}

Phonon::VideoWidget::AspectRatio VideoWidget::aspectRatio() const
{
    IMPLEMENTED;
    return  m_videoRenderWidget->m_aspect;
}

void VideoWidget::setAspectRatio(Phonon::VideoWidget::AspectRatio aspect)
{
    IMPLEMENTED;
    m_videoRenderWidget->setAspectRatio(aspect);
}

Phonon::VideoWidget::ScaleMode VideoWidget::scaleMode() const
{
    IMPLEMENTED;
    return m_videoRenderWidget->m_scaleMode;
}

void VideoWidget::setScaleMode(Phonon::VideoWidget::ScaleMode scaleMode)
{
    IMPLEMENTED;
    m_videoRenderWidget->setScaleMode(scaleMode);
}

qreal VideoWidget::brightness() const
{
    IMPLEMENTED;
    return m_videoRenderWidget->m_brightness;
}

void VideoWidget::setBrightness(qreal value)
{
    IMPLEMENTED;
    m_videoRenderWidget->m_brightness = value;
    if (m_owningMediaObject && m_owningMediaObject->state() == Phonon::PausedState)
        m_videoRenderWidget->updateVideoFrame();
}

qreal VideoWidget::contrast() const
{
    IMPLEMENTED;
    return m_videoRenderWidget->m_contrast;
}

void VideoWidget::setContrast(qreal value)
{
    IMPLEMENTED;
    m_videoRenderWidget->m_contrast = value;
    if (m_owningMediaObject && m_owningMediaObject->state() == Phonon::PausedState)
        m_videoRenderWidget->updateVideoFrame();
}

qreal VideoWidget::hue() const
{
    IMPLEMENTED;
    return m_videoRenderWidget->m_hue;
}

void VideoWidget::setHue(qreal value)
{
    IMPLEMENTED;
    m_videoRenderWidget->m_hue = value;
    if (m_owningMediaObject && m_owningMediaObject->state() == Phonon::PausedState)
        m_videoRenderWidget->updateVideoFrame();
}

qreal VideoWidget::saturation() const
{
    IMPLEMENTED;
    return m_videoRenderWidget->m_saturation;
}

void VideoWidget::setSaturation(qreal value)
{
    IMPLEMENTED;
    m_videoRenderWidget->m_saturation = value;
    if (m_owningMediaObject && m_owningMediaObject->state() == Phonon::PausedState)
        m_videoRenderWidget->updateVideoFrame();
}

void VideoWidget::mediaNodeEvent(const MediaNodeEvent *event)
{
    switch (event->type()){
    case MediaNodeEvent::VideoFrameSizeChanged:
        m_videoRenderWidget->setMovieRect(*static_cast<QRect *>(event->data()));
        break;
	case MediaNodeEvent::VideoOutputCountChanged:
	     m_videoRenderWidget->updateVideoOutputCount(*static_cast<int *>(event->data()));
	     break;
	case MediaNodeEvent::MediaPlaying:
	     m_videoRenderWidget->setMovieIsPaused(!(*static_cast<bool *>(event->data())));
	     break;
	default:
        break;
    }
}

void VideoWidget::updateVideo(VideoFrame &frame){
	PhononAutoReleasePool pool;
    m_videoRenderWidget->setVideoFrame(frame);
    MediaNode::updateVideo(frame);
}

}} // namespace Phonon::QT7

QT_END_NAMESPACE

#include "moc_videowidget.cpp"
