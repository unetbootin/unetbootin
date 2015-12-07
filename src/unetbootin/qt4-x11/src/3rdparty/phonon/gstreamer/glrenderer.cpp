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

#include <QtGui/QPainter>
#include <QtGui/QResizeEvent>

#ifndef QT_NO_OPENGL

#include "common.h"
#include "message.h"
#include "mediaobject.h"
#include "qwidgetvideosink.h"
#include "glrenderer.h"
#include "qrgb.h"

#if !defined(QT_OPENGL_ES)

#include <gst/gst.h>

// support old OpenGL installations (1.2)
// assume that if TEXTURE0 isn't defined, none are
#ifndef GL_TEXTURE0
# define GL_TEXTURE0    0x84C0
# define GL_TEXTURE1    0x84C1
# define GL_TEXTURE2    0x84C2
#endif

QT_BEGIN_NAMESPACE

static void frameRendered()
{
    static QString displayFps = qgetenv("PHONON_GST_FPS");
    if (displayFps.isEmpty())
        return;

    static int frames = 0;
    static QTime lastTime = QTime::currentTime();
    QTime time = QTime::currentTime();

    int delta = lastTime.msecsTo(time);
    if (delta > 2000) {
        printf("FPS: %f\n", 1000.0 * frames / qreal(delta));
        lastTime = time;
        frames = 0;
    }

    ++frames;
}

namespace Phonon
{
namespace Gstreamer
{

GLRenderer::GLRenderer(VideoWidget* videoWidget) :
    AbstractRenderer(videoWidget)
    , m_glWindow(0)
{
    videoWidget->backend()->logMessage("Creating OpenGL renderer");
    QGLFormat format = QGLFormat::defaultFormat();
    format.setSwapInterval(1);    // Enable vertical sync on draw to avoid tearing
    m_glWindow = new GLRenderWidgetImplementation(videoWidget, format);

    if ((m_videoSink = m_glWindow->createVideoSink())) {    //if ((m_videoSink = m_glWindow->createVideoSink())) {
        gst_object_ref (GST_OBJECT (m_videoSink)); //Take ownership
        gst_object_sink (GST_OBJECT (m_videoSink));

        QWidgetVideoSinkBase*  sink = reinterpret_cast<QWidgetVideoSinkBase*>(m_videoSink);
        // Let the videosink know which widget to direct frame updates to
        sink->renderWidget = videoWidget;
    }
}

GLRenderer::~GLRenderer()
{
    if (m_videoSink) {
        gst_object_unref (GST_OBJECT (m_videoSink));
        m_videoSink = 0;
    }
}


bool GLRenderer::eventFilter(QEvent * event)
{
    if (event->type() == QEvent::User) {
        NewFrameEvent *frameEvent= static_cast <NewFrameEvent *>(event);
        m_glWindow->setNextFrame(frameEvent->frame, frameEvent->width, frameEvent->height);
        return true;
    }
    else if (event->type() == QEvent::Resize) {
        m_glWindow->setGeometry(m_videoWidget->geometry());
        return true;
    }
    return false;
}

void GLRenderer::handleMediaNodeEvent(const MediaNodeEvent *event)
{
    switch (event->type()) {
    case MediaNodeEvent::SourceChanged:
    {
        Q_ASSERT(m_glWindow);
        m_glWindow->clearFrame();
        break;
    }
    default:
        break;
    }
}

GstElement* GLRenderWidgetImplementation::createVideoSink()
{
    if (hasYUVSupport())
        return GST_ELEMENT(g_object_new(get_type_YUV(), NULL));
    return 0;
}

void GLRenderWidgetImplementation::setNextFrame(const QByteArray &array, int w, int h)
{
    if (m_videoWidget->root()->state() == Phonon::LoadingState)
        return;

    m_frame = QImage();

    if (hasYUVSupport())
        updateTexture(array, w, h);
    else
        m_frame = QImage((uchar *)array.constData(), w, h, QImage::Format_RGB32);

    m_array = array;
    m_width = w;
    m_height = h;

    update();
}

void GLRenderWidgetImplementation::clearFrame()
{
    m_frame = QImage();
    m_array = QByteArray();
    update();
}

bool GLRenderWidgetImplementation::hasYUVSupport() const
{
    return m_yuvSupport;
}

static QImage convertFromYUV(const QByteArray &array, int w, int h)
{
    QImage result(w, h, QImage::Format_RGB32);

    // TODO: bilinearly interpolate the U and V channels for better result

    for (int y = 0; y < h; ++y) {
        uint *sp = (uint *)result.scanLine(y);

        const uchar *yp = (const uchar *)(array.constData() + y * w);
        const uchar *up = (const uchar *)(array.constData() + w * h + (y/2)*(w/2));
        const uchar *vp = (const uchar *)(array.constData() + w * h * 5/4 + (y/2)*(w/2));

        for (int x = 0; x < w; ++x) {
            const int sy = *yp;
            const int su = *up;
            const int sv = *vp;

            const int R = int(1.164 * (sy - 16) + 1.596 * (sv - 128));
            const int G = int(1.164 * (sy - 16) - 0.813 * (sv - 128) - 0.391 * (su - 128));
            const int B = int(1.164 * (sy - 16)                      + 2.018 * (su - 128));

            *sp = qRgb(qBound(0, R, 255),
                       qBound(0, G, 255),
                       qBound(0, B, 255));

            ++yp;
            ++sp;
            if (x & 1) {
                ++up;
                ++vp;
            }
        }
    }
    return result;
}

const QImage &GLRenderWidgetImplementation::currentFrame() const
{
    if (m_frame.isNull() && !m_array.isNull())
        m_frame = convertFromYUV(m_array, m_width, m_height);

    return m_frame;
}

#ifndef GL_FRAGMENT_PROGRAM_ARB
#define GL_FRAGMENT_PROGRAM_ARB           0x8804
#define GL_PROGRAM_FORMAT_ASCII_ARB       0x8875
#endif

// arbfp1 fragment program for converting yuv to rgb
const char *const yuvToRgb =
    "!!ARBfp1.0"
    "PARAM c[3] = { { 0.5, 0.0625 },"
    "{ 1.164, 0, 1.596, 2.0179999 },"
    "{ 1.164, -0.391, -0.81300002 } };"
    "TEMP R0;"
    "TEMP R1;"
    "TEX R0.x, fragment.texcoord[0], texture[2], 2D;"
    "ADD R1.z, R0.x, -c[0].x;"
    "TEX R1.x, fragment.texcoord[0], texture[0], 2D;"
    "TEX R0.x, fragment.texcoord[0], texture[1], 2D;"
    "ADD R1.x, R1, -c[0].y;"
    "ADD R1.y, R0.x, -c[0].x;"
    "DP3 result.color.x, R1, c[1];"
    "DP3 result.color.y, R1, c[2];"
    "DP3 result.color.z, R1, c[1].xwyw;"
    "END";

GLRenderWidgetImplementation::GLRenderWidgetImplementation(VideoWidget*videoWidget, const QGLFormat &format) :
        QGLWidget(format, videoWidget)
        , m_program(0)
        , m_yuvSupport(false)
        , m_videoWidget(videoWidget)
{
    makeCurrent();
    glGenTextures(3, m_texture);

    glProgramStringARB = (_glProgramStringARB) context()->getProcAddress(QLatin1String("glProgramStringARB"));
    glBindProgramARB = (_glBindProgramARB) context()->getProcAddress(QLatin1String("glBindProgramARB"));
    glDeleteProgramsARB = (_glDeleteProgramsARB) context()->getProcAddress(QLatin1String("glDeleteProgramsARB"));
    glGenProgramsARB = (_glGenProgramsARB) context()->getProcAddress(QLatin1String("glGenProgramsARB"));
    glActiveTexture = (_glActiveTexture) context()->getProcAddress(QLatin1String("glActiveTexture"));

    m_hasPrograms = glProgramStringARB && glBindProgramARB && glDeleteProgramsARB && glGenProgramsARB && glActiveTexture;

    if (m_hasPrograms) {
        glGenProgramsARB(1, &m_program);
        glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, m_program);

        const GLbyte *gl_src = reinterpret_cast<const GLbyte *>(yuvToRgb);
        glProgramStringARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB,
                           int(strlen(yuvToRgb)), gl_src);

        if (glGetError() != GL_NO_ERROR) {
            glDeleteProgramsARB(1, &m_program);
            m_hasPrograms = false;
        } else {
            m_yuvSupport = true;
        }
    }

    QPalette palette;
    palette.setColor(QPalette::Background, Qt::black);
    setPalette(palette);
    setAutoFillBackground(true);
    // Videowidget allways have this property to allow hiding the mouse cursor
    setMouseTracking(true);
}

void GLRenderWidgetImplementation::updateTexture(const QByteArray &array, int width, int height)
{
    m_width = width;
    m_height = height;

    makeCurrent();

    int w[3] = { width, width/2, width/2 };
    int h[3] = { height, height/2, height/2 };
    int offs[3] = { 0, width*height, width*height*5/4 };

    for (int i = 0; i < 3; ++i) {
        glBindTexture(GL_TEXTURE_2D, m_texture[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, w[i], h[i], 0,
                     GL_LUMINANCE, GL_UNSIGNED_BYTE, array.data() + offs[i]);

        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    }
}

void GLRenderWidgetImplementation::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    m_drawFrameRect = m_videoWidget->calculateDrawFrameRect();
    if (m_yuvSupport && frameIsSet()) {
        glEnable(GL_FRAGMENT_PROGRAM_ARB);
        glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, m_program);
        const float tx_array[] = { 0, 0, 1, 0, 1, 1, 0, 1};
        const QRectF r = drawFrameRect();

        const float v_array[] = { r.left(), r.top(), r.right(), r.top(), r.right(), r.bottom(), r.left(), r.bottom() };

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_texture[0]);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_texture[1]);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, m_texture[2]);
        glActiveTexture(GL_TEXTURE0);

        glVertexPointer(2, GL_FLOAT, 0, v_array);
        glTexCoordPointer(2, GL_FLOAT, 0, tx_array);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glDrawArrays(GL_QUADS, 0, 4);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);

        glDisable(GL_FRAGMENT_PROGRAM_ARB);
    } else {
        painter.setRenderHint(QPainter::SmoothPixmapTransform);
        painter.drawImage(drawFrameRect(), currentFrame());
    }

    frameRendered();
}
}
} //namespace Phonon::Gstreamer

QT_END_NAMESPACE

#endif // QT_OPENGL_ES
#endif // QT_NO_OPENGL
