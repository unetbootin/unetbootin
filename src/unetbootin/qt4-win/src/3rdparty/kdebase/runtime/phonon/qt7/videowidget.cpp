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

#include "videowidget.h"
#include "backendheader.h"
#include "quicktimevideoplayer.h"
#include "medianode.h"
#include "medianodeevent.h"
#include "mediaobject.h"

QT_BEGIN_NAMESPACE

namespace Phonon
{
namespace QT7
{

QGLWidget *PhononSharedQGLWidget()
{
    static QGLWidget *sharedWidget = 0;
    if (!sharedWidget)
        sharedWidget = new QGLWidget();
    return sharedWidget;    
}

class VideoRenderWidgetOpenGL : public QGLWidget
{    
public:
    VideoFrame m_currentFrame;
    QRect m_drawFrameRect;

    VideoRenderWidgetOpenGL::VideoRenderWidgetOpenGL(QWidget *parent, const QGLFormat &format)
        : QGLWidget(format, parent, PhononSharedQGLWidget())
    {
        setAutoFillBackground(false);        
    }
    
    void VideoRenderWidgetOpenGL::initializeGL()
    {
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    }
    
    void VideoRenderWidgetOpenGL::resizeGL(int w, int h)
    {
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glViewport(0, 0, GLsizei(w), GLsizei(h));
        gluOrtho2D(0, GLsizei(w), 0, GLsizei(h));    
        updateGL();
    }

    void VideoRenderWidgetOpenGL::paintGL()
    {
        glClear(GL_COLOR_BUFFER_BIT);
        m_currentFrame.drawFrame(m_drawFrameRect);
    }

    void VideoRenderWidgetOpenGL::setVideoFrame(VideoFrame &frame)
    {
        m_currentFrame = frame;
        if (isVisible()){
            makeCurrent();
            paintGL();
            swapBuffers();
        }
    }
    
    void VideoRenderWidgetOpenGL::setDrawFrameRect(const QRect &rect)
    {
        m_drawFrameRect = rect;
    }
};

/////////////////////////////////////////////////////////////////////////////////////////

class VideoRenderWidget : public QWidget
{    
public:
    enum RenderSystem {RS_GLWidget, RS_PaintEvent} m_renderSystem;
    
    VideoFrame m_currentFrame;
    QRect m_movieFrameRect;
    QRect m_drawFrameRect;
    Phonon::VideoWidget::ScaleMode m_scaleMode;
    Phonon::VideoWidget::AspectRatio m_aspect;
    VideoRenderWidgetOpenGL *m_glWidget;
    
    qreal m_brightness;
    qreal m_contrast;
    qreal m_hue;
    qreal m_saturation;
    qreal m_opacity;

    VideoRenderWidget::VideoRenderWidget() : QWidget(0),
        m_scaleMode(Phonon::VideoWidget::FitInView), m_aspect(Phonon::VideoWidget::AspectRatioAuto)
    {
        m_brightness = 0;
        m_contrast = 0;
        m_hue = 0;
        m_saturation = 0;
        m_opacity = 1;

        QGLFormat format = QGLFormat::defaultFormat();
        format.setSwapInterval(1); // Vertical sync (avoid tearing)
        m_glWidget = new VideoRenderWidgetOpenGL(this, format);
        m_glWidget->setVisible(true);
        m_renderSystem = RS_GLWidget;
        setAutoFillBackground(false);        
        updateDrawFrameRect();
    }
    
    bool inGraphicsView() const
    {
        return parentWidget() && parentWidget()->testAttribute(Qt::WA_DontShowOnScreen);
    }
    
    void setRenderSystem(RenderSystem renderSystem){
        if (m_renderSystem == renderSystem)
            return;
            
        m_renderSystem = renderSystem;
        
        switch (m_renderSystem){
            case RS_GLWidget:
                m_glWidget->setVisible(true);
            	m_glWidget->setVideoFrame(m_currentFrame);
                break;
            case RS_PaintEvent:
                m_glWidget->setVisible(false);
                break;
        }
    }
    
    QSize VideoRenderWidget::sizeHint() const
    {
        return m_movieFrameRect.size();
    }

    void VideoRenderWidget::resizeEvent(QResizeEvent */*event*/)
    {
        updateDrawFrameRect();
        m_glWidget->resize(size());
    }     

    bool VideoRenderWidget::event(QEvent *event)
    {
        switch (event->type()){
            // Try to detect if one of this objects
            // anchestors might have changed:
            case QEvent::Resize:
            case QEvent::Move:
            case QEvent::ParentChange:
                if (inGraphicsView())
                    setRenderSystem(RS_PaintEvent);
                else
                    setRenderSystem(RS_GLWidget);
                break;
            case QEvent::Paint:
                if (m_renderSystem == RS_PaintEvent){
                    QPainter p(this);
                    if (p.paintEngine()->type() == QPaintEngine::OpenGL){
                        float opacity = parentWidget() ? parentWidget()->windowOpacity() : 1;
                        m_currentFrame.drawFrame(m_drawFrameRect, opacity, false);
                    } else
                        p.drawImage(m_drawFrameRect, m_currentFrame.toQImage());
                }
                break;
            default:
                break;
        }

        return QWidget::event(event);
    }

    void VideoRenderWidget::setVideoFrame(VideoFrame &frame)
    {
        m_currentFrame = frame;
        m_currentFrame.setColors(m_brightness, m_contrast, m_hue, m_saturation);

        if (m_renderSystem == RS_GLWidget)          
            m_glWidget->setVideoFrame(m_currentFrame);
        else
            update();
    }
    
    void VideoRenderWidget::updateVideoFrame()
    {
        setVideoFrame(m_currentFrame);
    }
    
    void VideoRenderWidget::setMovieRect(const QRect &mrect)
    {
        if (mrect == m_movieFrameRect)
            return;
        m_movieFrameRect = mrect;
        updateDrawFrameRect();
        updateGeometry();
        qApp->processEvents();        
    }
    
    void VideoRenderWidget::setScaleMode(Phonon::VideoWidget::ScaleMode scaleMode)
    {
        m_scaleMode = scaleMode;
        updateDrawFrameRect();
        updateVideoFrame();
        repaint();
    }

    void VideoRenderWidget::setAspectRatio(Phonon::VideoWidget::AspectRatio aspect)
    {
        m_aspect = aspect;
        updateDrawFrameRect();
        updateVideoFrame();
        repaint();
    }

    void VideoRenderWidget::updateDrawFrameRect()
    {
        if (!m_movieFrameRect.isValid())
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
        m_glWidget->setDrawFrameRect(m_drawFrameRect);
    }
    
    QRect VideoRenderWidget::scaleToAspect(QRect srcRect, int w, int h)
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
    default:
        break;
    }
}

void VideoWidget::updateVideo(VideoFrame &frame){
    m_videoRenderWidget->setVideoFrame(frame);
    MediaNode::updateVideo(frame);
}

}} // namespace Phonon::QT7

QT_END_NAMESPACE

#include "moc_videowidget.cpp"
