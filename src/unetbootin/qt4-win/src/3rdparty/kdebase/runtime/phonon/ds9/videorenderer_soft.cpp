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

#include "videorenderer_soft.h"

#include "qmeminputpin.h"
#include "qbasefilter.h"

#include <QtGui/QPainter>
#include <QtGui/QPaintEngine>
#include <QtGui/QApplication>
#include <QtCore/QTime>

#include <cmath> //for sin and cos

#include <dvdmedia.h> //for VIDEOINFOHEADER2

//this will make a display every second of how many frames were pocessed and actually displayed
//#define FPS_COUNTER

#ifndef QT_NO_OPENGL
#include <QtOpenGL/QGLWidget>
#ifndef GL_FRAGMENT_PROGRAM_ARB
#define GL_FRAGMENT_PROGRAM_ARB           0x8804
#define GL_PROGRAM_FORMAT_ASCII_ARB       0x8875
#endif

// support old OpenGL installations (1.2)
// assume that if TEXTURE0 isn't defined, none are
#ifndef GL_TEXTURE0
# define GL_TEXTURE0    0x84C0
# define GL_TEXTURE1    0x84C1
# define GL_TEXTURE2    0x84C2
#endif

// arbfp1 fragment program for converting yuv (YV12) to rgb
static const char yv12ToRgb[] =
"!!ARBfp1.0"
"PARAM c[5] = { program.local[0..1],"
"                { 1.164, 0, 1.596, 0.5 },"
"                { 0.0625, 1.164, -0.391, -0.81300002 },"
"                { 1.164, 2.0179999, 0 } };"
"TEMP R0;"
"TEX R0.x, fragment.texcoord[0], texture[1], 2D;"
"ADD R0.y, R0.x, -c[2].w;"
"TEX R0.x, fragment.texcoord[0], texture[2], 2D;"
"ADD R0.x, R0, -c[2].w;"
"MUL R0.z, R0.y, c[0].w;"
"MAD R0.z, R0.x, c[0], R0;"
"MUL R0.w, R0.x, c[0];"
"MUL R0.z, R0, c[0].y;"
"TEX R0.x, fragment.texcoord[0], texture[0], 2D;"
"MAD R0.y, R0, c[0].z, R0.w;"
"ADD R0.x, R0, -c[3];"
"MUL R0.y, R0, c[0];"
"MUL R0.z, R0, c[1].x;"
"MAD R0.x, R0, c[0].y, c[0];"
"MUL R0.y, R0, c[1].x;"
"DP3 result.color.x, R0, c[2];"
"DP3 result.color.y, R0, c[3].yzww;"
"DP3 result.color.z, R0, c[4];"
"MOV result.color.w, c[1].y;"
"END";

static const char yuy2ToRgb[] =
    "!!ARBfp1.0"
"PARAM c[5] = { program.local[0..1],"
"                { 0.5, 2, 1, 0.0625 },"
"                { 1.164, 0, 1.596, 2.0179999 },"
"                { 1.164, -0.391, -0.81300002 } };"
"TEMP R0;"
"TEMP R1;"
"TEMP R2;"
"FLR R1.z, fragment.texcoord[0].x;"
"ADD R0.x, R1.z, c[2];"
"ADD R1.z, fragment.texcoord[0].x, -R1;"
"MUL R1.x, fragment.texcoord[0].z, R0;"
"MOV R1.y, fragment.texcoord[0];"
"TEX R0, R1, texture[0], 2D;"
"ADD R1.y, R0.z, -R0.x;"
"MUL R2.x, R1.z, R1.y;"
"MAD R0.x, R2, c[2].y, R0;"
"MOV R1.y, fragment.texcoord[0];"
"ADD R1.x, fragment.texcoord[0].z, R1;"
"TEX R1.xyw, R1, texture[0], 2D;"
"ADD R2.x, R1, -R0.z;"
"MAD R1.x, R1.z, c[2].y, -c[2].z;"
"MAD R0.z, R1.x, R2.x, R0;"
"ADD R1.xy, R1.ywzw, -R0.ywzw;"
"ADD R0.z, R0, -R0.x;"
"SGE R1.w, R1.z, c[2].x;"
"MAD R0.x, R1.w, R0.z, R0;"
"MAD R0.yz, R1.z, R1.xxyw, R0.xyww;"
"ADD R0.xyz, R0, -c[2].wxxw;"
"MUL R0.w, R0.y, c[0];"
"MAD R0.w, R0.z, c[0].z, R0;"
"MUL R0.z, R0, c[0].w;"
"MAD R0.y, R0, c[0].z, R0.z;"
"MUL R0.w, R0, c[0].y;"
"MUL R0.y, R0, c[0];"
"MUL R0.z, R0.w, c[1].x;"
"MAD R0.x, R0, c[0].y, c[0];"
"MUL R0.y, R0, c[1].x;"
"DP3 result.color.x, R0, c[3];"
"DP3 result.color.y, R0, c[4];"
"DP3 result.color.z, R0, c[3].xwyw;"
"MOV result.color.w, c[1].y;"
"END";

#endif //QT_NO_OPENGL

QT_BEGIN_NAMESPACE

namespace Phonon
{
    namespace DS9
    {

        static inline uchar clip(int c) 
        {
            return c <= 0 ? 0 : c >= 255 ? 255 : c;
        }

        static const QVector<AM_MEDIA_TYPE> videoMediaTypes()
        {
            AM_MEDIA_TYPE mt;
            qMemSet(&mt, 0, sizeof(AM_MEDIA_TYPE));
            mt.majortype = MEDIATYPE_Video;

            //we accept any video format
            mt.formattype = GUID_NULL;
            mt.cbFormat = 0;
            mt.pbFormat = 0;

            QVector<AM_MEDIA_TYPE> ret;

            //we support YUV (YV12 and YUY2) and RGB32
            mt.subtype = MEDIASUBTYPE_YV12;
            ret << mt;
            mt.subtype = MEDIASUBTYPE_YUY2;
            ret << mt;
            mt.subtype = MEDIASUBTYPE_RGB32;
            ret << mt;

            return ret;
        }

        class VideoRendererSoftFilter : public QBaseFilter
        {
        public:
            VideoRendererSoftFilter(VideoRendererSoft *renderer);

            ~VideoRendererSoftFilter();

#ifndef QT_NO_OPENGL
            void freeGLResources()
            {
                if (m_usingOpenGL) {
                    //let's reinitialize those values
                    m_usingOpenGL = false;
                    //to be sure we recreate it
                    if (m_textureUploaded) {
                        glDeleteTextures(3, m_texture);
                        m_textureUploaded = false;
                    }
                }
                m_checkedPrograms = false;
            }
#endif // QT_NO_OPENGL

            void freeResources()
            {
                QMutexLocker locker(&m_mutex);
                m_frontBuffer.clear();
                m_backBuffer.clear();
#ifndef QT_NO_OPENGL
                freeGLResources();
#endif // QT_NO_OPENGL
                m_textureUploaded = false;
            }

            void endOfStream()
            {
                //received from the input pin
                ::SetEvent(m_receiveCanWait); //unblocks the flow

                //we send the message to the graph
                ComPointer<IMediaEventSink> sink(graph(), IID_IMediaEventSink);
                if (sink) {
                    sink->Notify(EC_COMPLETE, S_OK, 
                        reinterpret_cast<LONG_PTR>(static_cast<IBaseFilter*>(this)));
                }
            }

            void beginFlush()
            {
                m_backBuffer.sample = ComPointer<IMediaSample>();
                m_frontBuffer.sample = ComPointer<IMediaSample>();
                ::SetEvent(m_receiveCanWait); //unblocks the flow
            }

            void endFlush()
            {
                if (m_inputPin->connected() == 0) {
                    ::SetEvent(m_receiveCanWait); //unblock the flow in receive
                } else {
                    ::ResetEvent(m_receiveCanWait); //block the flow again
                }
            }

            STDMETHODIMP Stop()
            {
                HRESULT hr = QBaseFilter::Stop();
                beginFlush();
                return hr;
            }

            STDMETHODIMP Pause()
            {
                HRESULT hr = QBaseFilter::Pause();
                if (m_inputPin->connected() == 0) {
                    ::SetEvent(m_receiveCanWait); //unblock the flow in receive
                } else {
                    ::ResetEvent(m_receiveCanWait); //this will block
                }
                return hr;
            }

            STDMETHODIMP Run(REFERENCE_TIME start)
            {
                HRESULT hr = QBaseFilter::Run(start);
                m_start = start;

                if (m_inputPin->connected() == 0) {
                    endOfStream();
                } else {
                    ::SetEvent(m_receiveCanWait); //unblocks the flow (this event will block then again)
                }

#ifdef FPS_COUNTER
                fpsTime.restart();
                nbFramesProcessed = 0;
                nbFramesDisplayed = 0;
#endif

                return hr;
            }

            HRESULT processSample(IMediaSample *sample);

            void applyMixerSettings(qreal brightness, qreal contrast, qreal hue, qreal saturation)
            {
                //let's normalize the values
                m_brightness = brightness * 128;
                m_contrast = contrast + 1.;
                m_hue = hue * 180.;
                m_saturation = saturation + 1.;
            }

            //the following function is called from the GUI thread
            void repaintCurrentFrame(QPainter &painter, const QRect &r);

        protected:
            static void convertYV12toRGB(const uchar *data, int w, int h, QImage &dest,
                                         qreal brightness, qreal contrast, qreal hue, qreal saturation);
            static void convertYUY2toRGB(const uchar *data, int w, int h, QImage &dest,
                                         qreal brightness, qreal contrast, qreal hue, qreal saturation);
            static void normalizeRGB(const uchar *data, int w, int h, QImage &destImage);

        private:
            QPin *const m_inputPin;
            struct VideoFrameBuffer
            {
                void clear()
                {
                    image = QImage();
                    sample = ComPointer<IMediaSample>();
                }

                QImage image; //in case we render to an image
                ComPointer<IMediaSample> sample;

            } m_frontBuffer, m_backBuffer;


            VideoRendererSoft *m_renderer;
            mutable QMutex m_mutex;
            REFERENCE_TIME m_start;
            HANDLE m_renderEvent, m_receiveCanWait;         // Signals sample to render
            int m_width, m_height;
            bool m_textureUploaded;

            //mixer settings
            qreal m_brightness,
                  m_contrast,
                  m_hue,
                  m_saturation;

#ifdef FPS_COUNTER
           QTime fpsTime;
           int nbFramesProcessed;
           int nbFramesDisplayed;
#endif

#ifndef QT_NO_OPENGL
            enum Program
            {
                YV12toRGB = 0,
                YUY2toRGB = 1,
                ProgramCount = 2
            };

            void updateTexture();
            void checkGLPrograms();

            // ARB_fragment_program
            typedef void (APIENTRY *_glProgramStringARB) (GLenum, GLenum, GLsizei, const GLvoid *);
            typedef void (APIENTRY *_glBindProgramARB) (GLenum, GLuint);
            typedef void (APIENTRY *_glDeleteProgramsARB) (GLsizei, const GLuint *);
            typedef void (APIENTRY *_glGenProgramsARB) (GLsizei, GLuint *);
            typedef void (APIENTRY *_glProgramLocalParameter4fARB) (GLenum, GLuint, GLfloat, GLfloat, GLfloat, GLfloat);
            typedef void (APIENTRY *_glActiveTexture) (GLenum);

            _glProgramStringARB glProgramStringARB;
            _glBindProgramARB glBindProgramARB;
            _glDeleteProgramsARB glDeleteProgramsARB;
            _glGenProgramsARB glGenProgramsARB;
            _glProgramLocalParameter4fARB glProgramLocalParameter4fARB;
            _glActiveTexture glActiveTexture;

            bool m_checkedPrograms;
            bool m_usingOpenGL;
            GLuint m_program[2];
            GLuint m_texture[3];
#endif
        };

        class VideoRendererSoftPin : public QMemInputPin
        {
        public:
            VideoRendererSoftPin(VideoRendererSoftFilter *parent) : 
              QMemInputPin(parent, videoMediaTypes(), false /*no transformation of the samples*/),
                  m_renderer(parent)
              {
              }

              STDMETHODIMP EndOfStream()
              {
                  m_renderer->endOfStream();
                  return QMemInputPin::EndOfStream();
              }

              STDMETHODIMP ReceiveCanBlock()
              {
                  //yes, it can block
                  return S_OK;
              }

              STDMETHODIMP BeginFlush()
              {
                  m_renderer->beginFlush();
                  return QMemInputPin::BeginFlush();
              }

              STDMETHODIMP EndFlush()
              {
                  m_renderer->endFlush();
                  return QMemInputPin::EndFlush();
              }

        private:
            VideoRendererSoftFilter * const m_renderer;

        };

        VideoRendererSoftFilter::VideoRendererSoftFilter(VideoRendererSoft *renderer) : 
        QBaseFilter(CLSID_NULL), m_inputPin(new VideoRendererSoftPin(this)), m_renderer(renderer), m_start(0), 
            m_width(0), m_height(0)
#ifndef QT_NO_OPENGL
            ,m_usingOpenGL(false), m_checkedPrograms(false), m_textureUploaded(false)
#endif
        {
            m_renderEvent    = ::CreateEvent(0, 0, 0, 0);
            m_receiveCanWait = ::CreateEvent(0, 0, 0, 0);
            //simply initialize the array with default values
            applyMixerSettings(0., 0., 0., 0.);
        }

        VideoRendererSoftFilter::~VideoRendererSoftFilter()
        {
            ::CloseHandle(m_renderEvent);
            ::CloseHandle(m_receiveCanWait);
            //this frees up resources
            freeResources();
        }

        HRESULT VideoRendererSoftFilter::processSample(IMediaSample *sample)
        {
#ifdef FPS_COUNTER
            if (fpsTime.elapsed() > 1000) {
                qDebug("FPS_COUNTER: processed=%d, displayed=%d (%d)", nbFramesProcessed, nbFramesDisplayed, fpsTime.elapsed());
                nbFramesProcessed = 0;
                nbFramesDisplayed = 0;
                fpsTime.restart();

            }
#endif

            AM_MEDIA_TYPE *type = 0;
            if (sample->GetMediaType(&type) == S_OK) {
                //let's update the media type of the input pin
                m_inputPin->setConnectedType(*type);
            }


            const AM_MEDIA_TYPE &mt = m_inputPin->connectedType();

            if (mt.pbFormat == 0 || mt.cbFormat == 0) {
                return VFW_E_INVALIDMEDIATYPE;
            }

            if (mt.formattype == FORMAT_VideoInfo) {
                const VIDEOINFOHEADER *header = reinterpret_cast<VIDEOINFOHEADER*>(mt.pbFormat);
                m_height = header->bmiHeader.biHeight;
                m_width = header->bmiHeader.biWidth;
            } else  if (mt.formattype == FORMAT_VideoInfo2) {
                const VIDEOINFOHEADER2 *header = reinterpret_cast<VIDEOINFOHEADER2*>(mt.pbFormat);
                m_height = header->bmiHeader.biHeight;
                m_width = header->bmiHeader.biWidth;
            } else {
                return VFW_E_INVALIDMEDIATYPE;
            }

            uchar *data = 0;
            sample->GetPointer(&data);
#ifndef QT_NO_OPENGL
            if (m_usingOpenGL) {
                sample->AddRef();
                m_backBuffer.sample = ComPointer<IMediaSample>(sample);
            } else
#endif
            if (mt.subtype == MEDIASUBTYPE_YV12) {
                convertYV12toRGB(data, m_width, m_height, m_backBuffer.image, 
                                 m_brightness, m_contrast, m_hue, m_saturation);

            } else if (mt.subtype == MEDIASUBTYPE_YUY2) {
                convertYUY2toRGB(data, m_width, m_height, m_backBuffer.image,
                                    m_brightness, m_contrast, m_hue, m_saturation);
            } else if (mt.subtype == MEDIASUBTYPE_RGB32) {
                normalizeRGB(data, m_width, m_height, m_backBuffer.image);
            }

#ifdef FPS_COUNTER
            nbFramesProcessed++;
#endif

            REFERENCE_TIME start = 0, stop = 0;
            HRESULT hr = sample->GetTime(&start, &stop);

            ComPointer<IReferenceClock> clock;
            GetSyncSource(&clock);
            if (SUCCEEDED(hr) && state() == State_Running && clock) {
                REFERENCE_TIME current;
                clock->GetTime(&current);

                DWORD_PTR advise;

                //let's synchronize here
                clock->AdviseTime(m_start, start,
                    reinterpret_cast<HEVENT>(m_renderEvent), &advise);
            }

            HANDLE handles[] = {m_receiveCanWait, m_renderEvent};
            if (::WaitForMultipleObjects(2, handles, false, INFINITE) == WAIT_OBJECT_0) {
                if (state() != State_Stopped && !m_inputPin->isFlushing()) {
                    ::ResetEvent(m_receiveCanWait);
                }
            }

            //the rendering is finished
            {
                QMutexLocker locker(&m_mutex);
                //we simply swap them to avoid memory allocation and deallocation
                qSwap(m_frontBuffer, m_backBuffer);
                //be sure to free the sample
                m_backBuffer.sample = ComPointer<IMediaSample>();

            }

            //image is updated: we should update the widget
            //we should never call directly members of target due to thread-safety
            QApplication::postEvent(m_renderer, new QEvent(QEvent::UpdateRequest));

            //everything should be ok
            return S_OK;
        }

#ifndef QT_NO_OPENGL
        void VideoRendererSoftFilter::checkGLPrograms()
        {
            if (!m_checkedPrograms) {
                m_checkedPrograms = true;
                //we check only once if the widget is drawn using opengl
                glProgramStringARB = (_glProgramStringARB) QGLContext::currentContext()->getProcAddress(QLatin1String("glProgramStringARB"));
                glBindProgramARB = (_glBindProgramARB) QGLContext::currentContext()->getProcAddress(QLatin1String("glBindProgramARB"));
                glDeleteProgramsARB = (_glDeleteProgramsARB) QGLContext::currentContext()->getProcAddress(QLatin1String("glDeleteProgramsARB"));
                glGenProgramsARB = (_glGenProgramsARB) QGLContext::currentContext()->getProcAddress(QLatin1String("glGenProgramsARB"));
                glProgramLocalParameter4fARB = (_glProgramLocalParameter4fARB) QGLContext::currentContext()->getProcAddress(QLatin1String("glProgramLocalParameter4fARB"));
                glActiveTexture = (_glActiveTexture) QGLContext::currentContext()->getProcAddress(QLatin1String("glActiveTexture"));

                if (glProgramStringARB && glBindProgramARB && glDeleteProgramsARB && 
                    glGenProgramsARB && glActiveTexture && glProgramLocalParameter4fARB) {
                    glGenProgramsARB(2, m_program);

                    const char *code[] = {yv12ToRgb, yuy2ToRgb};

                    bool error = false;
                    for(int i = 0; i < ProgramCount && !error;  ++i) {

                        glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, m_program[i]);

                        const GLbyte *gl_src = reinterpret_cast<const GLbyte *>(code[i]);
                        glProgramStringARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB,
                            strlen(code[i]), gl_src);

                        if (glGetError() != GL_NO_ERROR) {
                            error = true;
                        }
                    }

                    if (error) {
                        glDeleteProgramsARB(2, m_program);
                    } else {
                        //everything went fine we store the context here (we support YV12 and YUY2)
                        m_usingOpenGL = m_inputPin->connectedType().subtype == MEDIASUBTYPE_YV12
                            || m_inputPin->connectedType().subtype == MEDIASUBTYPE_YUY2;
                        //those "textures" will be used as byte streams
                        //to pass Y, U and V data to the graphics card
                        glGenTextures(3, m_texture);
                    }
                }
            }
        }

        void VideoRendererSoftFilter::updateTexture()
        {
            if (!m_frontBuffer.sample) {
                return; //the texture is already up2date or their is no data yet
            }

            uchar *data = 0;
            m_frontBuffer.sample->GetPointer(&data);

            if (m_inputPin->connectedType().subtype == MEDIASUBTYPE_YV12) {
                int w[3] = { m_width, m_width/2, m_width/2 };
                int h[3] = { m_height, m_height/2, m_height/2 };
                int offs[3] = { 0, m_width*m_height, m_width*m_height*5/4 };

                for (int i = 0; i < 3; ++i) {
                    glBindTexture(GL_TEXTURE_2D, m_texture[i]);
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, w[i], h[i], 0,
                        GL_LUMINANCE, GL_UNSIGNED_BYTE, data + offs[i]);

                    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                }
            } else { //m_inputPin->connectedType().subtype == MEDIASUBTYPE_YUY2
                //we upload 1 texture
                glBindTexture(GL_TEXTURE_2D, m_texture[0]);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width / 2, m_height, 0,
                    GL_RGBA, GL_UNSIGNED_BYTE, data);

                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            }
            m_frontBuffer.sample = ComPointer<IMediaSample>();
            m_textureUploaded = true;
        }
#endif

        void VideoRendererSoftFilter::repaintCurrentFrame(QPainter &painter, const QRect &r)
        {
            QMutexLocker locker(&m_mutex);

#ifdef FPS_COUNTER
            nbFramesDisplayed++;
#endif


#ifndef QT_NO_OPENGL
            if (painter.paintEngine()->type() == QPaintEngine::OpenGL) {
                //for now we only support YUV (both YV12 and YUY2)
                checkGLPrograms();

                if (m_usingOpenGL) {
                    updateTexture();

                    if (!m_textureUploaded) {
                        //we simply fill the whole video with content
                        //the callee has alrtead set the brush
                        QRect videorect(0,0,0,0);
                        videorect.setSize(m_renderer->videoSize());
                        painter.drawRect(videorect);
                        return;
                    }

                    //let's draw the texture

                    //Let's pass the other arguments
                    const Program prog = (m_inputPin->connectedType().subtype == MEDIASUBTYPE_YV12) ? YV12toRGB : YUY2toRGB;
                    glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, m_program[prog]);
                    //loading the parameters
                    glProgramLocalParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 0, m_brightness / 256., m_contrast, cos(m_hue), sin(m_hue));
                    glProgramLocalParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 1, m_saturation, painter.opacity() /*alpha */, 0. /*dummy*/, 0. /*dummy*/);

                    glEnable(GL_FRAGMENT_PROGRAM_ARB);


                    const float v_array[] = { r.left(), r.top(), r.right()+1, r.top(), r.right()+1, r.bottom()+1, r.left(), r.bottom()+1 };

                    float tx_array[12] = {0., 0., 0., 1.,
                                          0., 0., 1., 1.,
                                          0., 0., 1., 0.};

                    if (prog == YUY2toRGB) {
                        const float w = m_width / 2,
                            iw = 1. / w;

                        tx_array[3] = w;
                        tx_array[6] = w;

                        for (int i = 0; i < 4; ++i) {
                            tx_array[3*i + 2] = iw;
                        }
                    }

                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, m_texture[0]);

                    if (prog == YV12toRGB) {
                        glActiveTexture(GL_TEXTURE1);
                        glBindTexture(GL_TEXTURE_2D, m_texture[2]);
                        glActiveTexture(GL_TEXTURE2);
                        glBindTexture(GL_TEXTURE_2D, m_texture[1]);
                        glActiveTexture(GL_TEXTURE0);
                    }


                    glVertexPointer(2, GL_FLOAT, 0, v_array);
                    glTexCoordPointer(3, GL_FLOAT, 0, tx_array);
                    glEnableClientState(GL_VERTEX_ARRAY);
                    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                    glDrawArrays(GL_QUADS, 0, 4);
                    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
                    glDisableClientState(GL_VERTEX_ARRAY);

                    glDisable(GL_FRAGMENT_PROGRAM_ARB);
                    return;
                }

            } else if (m_usingOpenGL) {
                //we just switched to software again
                m_usingOpenGL = false;
                if (m_frontBuffer.sample) {
                    uchar *data = 0;
                    m_frontBuffer.sample->GetPointer(&data);

                    //let's update the front buffer
                    if (m_inputPin->connectedType().subtype == MEDIASUBTYPE_YUY2) {
                        convertYUY2toRGB(data, m_width, m_height, m_frontBuffer.image,
                            m_brightness, m_contrast, m_hue, m_saturation);
                    }else { //it is YV12
                        convertYV12toRGB(data, m_width, m_height, m_frontBuffer.image,
                            m_brightness, m_contrast, m_hue, m_saturation);

                    }
                }
                freeGLResources();

            }
#endif
            painter.drawImage(0, 0, m_frontBuffer.image);
        }


        void VideoRendererSoftFilter::normalizeRGB(const uchar *data, int w, int h, QImage &destImage)
        {
            if (destImage.size() != QSize(w, h)) {
                destImage = QImage(w, h, QImage::Format_RGB32);
            }
            const QRgb *rgb = reinterpret_cast<const QRgb*>(data);

            //this sets the alpha channel to 0xff and flip the image vertically
            for (int y = h - 1; y >= 0; --y) {
                QRgb *dest = reinterpret_cast<QRgb*>(destImage.scanLine(y));
                for(int i = w; i > 0; --i, ++rgb, ++dest) {
                    *dest = *rgb | (0xff << 24); //we force the alpha channel to 0xff
                }
            }
        }


        //we render data interpreted as YV12 into m_renderbuffer
        void VideoRendererSoftFilter::convertYV12toRGB(const uchar *data, int w, int h, QImage &destImage,
            qreal brightness, qreal contrast, qreal hue, qreal saturation)
        {
            //let's cache some computation
            const int cosHx256 = qRound(cos(hue) * contrast * saturation * 256),
                      sinHx256 = qRound(sin(hue) * contrast * saturation * 256);

            int Yvalue[256];
            for(int i = 0;i<256;++i) {
                Yvalue[i] = qRound(((i - 16) * contrast + brightness) * 298 + 128);
            }


            if (destImage.size() != QSize(w, h)) {
                destImage = QImage(w, h, QImage::Format_ARGB32);
            }

            QRgb *dest = reinterpret_cast<QRgb*>(destImage.bits());
            const uchar *dataY = data,
                *dataV = data + (w*h),
                *dataU = dataV + (w*h)/4;

            QRgb *line1 = dest,
                *line2 = dest + w;

            for(int l = (h >> 1); l > 0; --l) {
                //we treat 2 lines by 2 lines

                for(int x = (w >> 1); x > 0; --x) {

                    const int u = *dataU++ - 128,
                        v = *dataV++ - 128;
                    const int d = (u * cosHx256 + v * sinHx256) >> 8,
                        e = (v * cosHx256 + u * sinHx256) >> 8;

                    const int compRed = 409 * e,
                        compGreen = -100 * d - 208 * e,
                        compBlue = 516 * d;

                    const int y21 = Yvalue[ dataY[w] ],
                              y11 = Yvalue[ *dataY++ ],
                              y22 = Yvalue[ dataY[w] ],
                              y12 = Yvalue[ *dataY++ ];

                    //1st line 1st pixel
                    *line1++ = qRgb(clip(( y11 + compRed) >> 8), 
                        clip(( y11 + compGreen) >> 8), 
                        clip(( y11 + compBlue) >> 8));

                    //1st line, 2nd pixel
                    *line1++ = qRgb( clip(( y12 + compRed) >> 8),
                        clip(( y12 + compGreen) >> 8),
                        clip(( y12 + compBlue) >> 8));

                    //2nd line 1st pixel
                    *line2++ = qRgb(clip(( y21 + compRed) >> 8),
                        clip(( y21 + compGreen) >> 8),
                        clip(( y21 + compBlue) >> 8));

                    //2nd line 2nd pixel
                    *line2++ = qRgb(clip(( y22 + compRed) >> 8),
                        clip(( y22 + compGreen) >> 8),
                        clip(( y22 + compBlue) >> 8));
                } //for

                //end of the line
                dataY += w;
                line1 = line2;
                line2 += w;

            } //for
            
        }

        //we render data interpreted as YUY2 into m_renderbuffer
        void VideoRendererSoftFilter::convertYUY2toRGB(const uchar *data, int w, int h, QImage &destImage,
                                         qreal brightness, qreal contrast, qreal hue, qreal saturation)
        {
            //let's cache some computation
            int Yvalue[256];
            for(int i = 0;i<256;++i) {
                Yvalue[i] = qRound(((i - 16) * contrast + brightness) * 298 + 128);
            }

            const int cosHx256 = qRound(cos(hue) * contrast * saturation * 256),
                      sinHx256 = qRound(sin(hue) * contrast * saturation * 256);

            if (destImage.size() != QSize(w, h)) {
                //this will only allocate memory when needed
                destImage = QImage(w, h, QImage::Format_ARGB32);
            }
            QRgb *dest = reinterpret_cast<QRgb*>(destImage.bits());

            //the number of iterations is width * height / 2 because we treat 2 pixels at each iterations
            for (int c = w * h / 2; c > 0 ; --c) {

                //the idea of that algorithm comes from 
                //http://msdn2.microsoft.com/en-us/library/ms867704.aspx#yuvformats_identifying_yuv_formats_in_directshow

                //we treat 2 pixels by 2 pixels (we start reading 2 pixels info ie. "YUYV"
                const int y1 = Yvalue[*data++],
                    u = *data++ - 128,
                    y2 = Yvalue[*data++],
                    v = *data++ - 128;

                const int d = (u * cosHx256 + v * sinHx256) >> 8,
                    e = (v * cosHx256 + u * sinHx256) >> 8;

                const int compRed = 409 * e,
                    compGreen = -100 * d - 208 * e,
                    compBlue = 516 * d;

                //first pixel
                *dest++ = qRgb(clip(( y1 + compRed) >> 8),
                    clip(( y1 + compGreen) >> 8),
                    clip(( y1 + compBlue) >> 8));

                //second pixel
                *dest++ = qRgb(clip(( y2 + compRed) >> 8),
                    clip(( y2 + compGreen) >> 8),
                    clip(( y2 + compBlue) >> 8));
            }
        }


        VideoRendererSoft::VideoRendererSoft(QWidget *target) : 
        m_renderer(new VideoRendererSoftFilter(this)), m_target(target)
        {
            m_filter = Filter(m_renderer);
        }

        VideoRendererSoft::~VideoRendererSoft()
        {
        }


        bool VideoRendererSoft::isHardwareAccelerated() const
        {
            return false;
        }


        void VideoRendererSoft::repaintCurrentFrame(QWidget *target, const QRect &rect)
        {
            QPainter painter(target);

            QColor backColor = target->palette().color(target->backgroundRole());
            painter.setBrush(backColor);
            painter.setPen(Qt::NoPen);
            if (!m_videoRect.contains(rect)) {
                //we repaint the borders only when needed
                QRegion reg = QRegion(rect) - m_videoRect;
                foreach(QRect r, reg.rects()) {
                    painter.drawRect(r);
                }
            }

            painter.setRenderHint(QPainter::SmoothPixmapTransform);
            painter.setTransform(m_transform, true);
            m_renderer->repaintCurrentFrame(painter, m_videoRect);
        }

        void VideoRendererSoft::notifyResize(const QRect &rect,
            Phonon::VideoWidget::AspectRatio aspectRatio, Phonon::VideoWidget::ScaleMode scaleMode)
        {
            const QSize vsize = videoSize();
            internalNotifyResize(rect.size(), vsize, aspectRatio, scaleMode);

            m_transform.reset();

            if (vsize.isValid() && rect.isValid()) {
                m_transform.translate(m_dstX, m_dstY);
                const qreal sx = qreal(m_dstWidth) / qreal(vsize.width()),
                    sy = qreal(m_dstHeight) / qreal(vsize.height());
                m_transform.scale(sx, sy);
                m_videoRect = m_transform.mapRect( QRect(0,0, vsize.width(), vsize.height()));
            }
        }

        void VideoRendererSoft::notifyVideoLoaded()
        {
            m_renderer->freeResources();
        }


        QSize VideoRendererSoft::videoSize() const
        {
            QSize ret(0, 0);
            if (m_renderer->pins().first()->connected()) {
                const AM_MEDIA_TYPE &mt = m_renderer->pins().first()->connectedType();
                if (mt.pbFormat && mt.pbFormat) {
                    if (mt.formattype == FORMAT_VideoInfo) {
                        const VIDEOINFOHEADER *header = reinterpret_cast<VIDEOINFOHEADER*>(mt.pbFormat);
                        const int h = header->bmiHeader.biHeight,
                            w = header->bmiHeader.biWidth;
                        ret = QSize(w, h);
                    } else {
                        const VIDEOINFOHEADER2 *header = reinterpret_cast<VIDEOINFOHEADER2*>(mt.pbFormat);
                        const int h = header->bmiHeader.biHeight,
                            w = header->bmiHeader.biWidth;
                        ret = QSize(w, h);
                    }
                }
            }

            return ret;
        }

        void VideoRendererSoft::applyMixerSettings(qreal brightness, qreal contrast, qreal hue, qreal saturation)
        {
            m_renderer->applyMixerSettings(brightness, contrast, hue, saturation);
        }

        void VideoRendererSoft::setActive(bool)
        {
            //nothing to do here (it won't ask to repaint
        }

        bool VideoRendererSoft::event(QEvent *e)
        {
            if (e->type() == QEvent::UpdateRequest) {
                m_target->update(m_videoRect);
                return true;
            }
            return QObject::event(e);
        }


    }
}

QT_END_NAMESPACE
