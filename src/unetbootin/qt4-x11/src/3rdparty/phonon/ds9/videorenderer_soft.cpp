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


#include "videorenderer_soft.h"

#ifndef QT_NO_PHONON_VIDEO

#include "qmeminputpin.h"
#include "qbasefilter.h"

#include <QtGui/QPainter>
#include <QtGui/QPaintEngine>
#include <QtGui/QApplication>
#include <QtCore/QTime>

#define _USE_MATH_DEFINES //for pi
#include <QtCore/qmath.h> //for sin and cos
/* M_PI is a #define that may or may not be handled in <cmath> */
#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288419717
#endif

#include <dvdmedia.h> //for VIDEOINFOHEADER2

//this will make a display every second of how many frames were pocessed and actually displayed
//#define FPS_COUNTER

#ifdef Q_OS_WINCE
#define QT_NO_OPENGL
#endif

#ifndef QT_NO_OPENGL
#include <gl/gl.h>
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

#define CLIP_SHIFT_RIGHT_8(c) ((c) < 0 ? 0 : (c) > 0xffff ? 0xff : (c) >> 8)
#define CLIP_SHIFT_LEFT_8(c) ((c) < 0 ? 0 : (c) > 0xffff ? 0xff0000 : ( ((c) << 8) & 0xff0000) )
#define CLIP_NO_SHIFT(c) ((c) < 0 ? 0 : (c) > 0xffff ? 0xff00 : ((c) & 0xff00) )
#define CLIPPED_PIXEL(base, r, g, b) (0xff000000u | CLIP_SHIFT_LEFT_8(base+r) | CLIP_NO_SHIFT(base+g) | CLIP_SHIFT_RIGHT_8(base+b))
#define CLIPPED_PIXEL2(r, g, b) (0xff000000u | CLIP_SHIFT_LEFT_8(r) | CLIP_NO_SHIFT(g) | CLIP_SHIFT_RIGHT_8(b))

QT_BEGIN_NAMESPACE

namespace Phonon
{
    namespace DS9
    {
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

            QSize videoSize() const;

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
                m_sampleBuffer = ComPointer<IMediaSample>();
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

            void freeMediaSample()
            {
                QMutexLocker locker(&m_mutex);
                m_sampleBuffer = ComPointer<IMediaSample>();
            }

            void beginFlush()
            {
                freeMediaSample();
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
                m_hue = hue * M_PI;
                m_saturation = saturation + 1.;
            }
            
            QImage currentImage() const
            {
                return m_currentImage;
            }

            void setCurrentImage(const QImage &image)
            {
                QMutexLocker locker(&m_mutex);
                m_currentImage = image;
            }

            //the following function is called from the GUI thread
            void repaintCurrentFrame(QPainter &painter, const QRect &r);


        protected:
            static void convertYV12toRGB(const uchar *data, const QSize &s, QImage &dest,
                                         qreal brightness, qreal contrast, qreal hue, qreal saturation);
            static void convertYUY2toRGB(const uchar *data, const QSize &s, QImage &dest,
                                         qreal brightness, qreal contrast, qreal hue, qreal saturation);
            static void normalizeRGB(const uchar *data, const QSize &s, QImage &destImage);

        private:
            QPin *const m_inputPin;
            ComPointer<IMediaSample> m_sampleBuffer;
            QImage m_currentImage;


            VideoRendererSoft *m_renderer;
            mutable QMutex m_mutex;
            REFERENCE_TIME m_start;
            HANDLE m_renderEvent, m_receiveCanWait;         // Signals sample to render
            QSize m_size;
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
            bool checkGLPrograms();

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


              STDMETHODIMP GetAllocatorRequirements(ALLOCATOR_PROPERTIES *prop)
              {
                  if (!prop) {
                      return E_POINTER;
                  }

                  //we need 2 buffers
                  prop->cBuffers = 2;
                  return S_OK;
              }


              STDMETHODIMP NotifyAllocator(IMemAllocator *alloc, BOOL readonly)
              {
                  if (!alloc) {
                      return E_POINTER;
                  }
                  ALLOCATOR_PROPERTIES prop;
                  HRESULT hr = alloc->GetProperties(&prop);
                  if (SUCCEEDED(hr) && prop.cBuffers == 1) {
                      //we ask to get 2 buffers so that we don't block the flow 
                      //when we addref the mediasample
                      prop.cBuffers = 2;
                      ALLOCATOR_PROPERTIES dummy;
                      alloc->SetProperties(&prop, &dummy);
                  }

                  return QMemInputPin::NotifyAllocator(alloc, readonly);
              }



        private:
            VideoRendererSoftFilter * const m_renderer;

        };

        VideoRendererSoftFilter::VideoRendererSoftFilter(VideoRendererSoft *renderer) :
        QBaseFilter(CLSID_NULL), m_inputPin(new VideoRendererSoftPin(this)),
            m_renderer(renderer), m_start(0)
#ifndef QT_NO_OPENGL
            ,m_usingOpenGL(false), m_checkedPrograms(false), m_textureUploaded(false)
#endif
        {
            m_renderEvent    = ::CreateEvent(0, 0, 0, 0);
            m_receiveCanWait = ::CreateEvent(0, 0, 0, 0);
            //simply initialize the array with default values
            applyMixerSettings(0., 0., 0., 0.);
#ifndef QT_NO_OPENGL
#endif
        }

        VideoRendererSoftFilter::~VideoRendererSoftFilter()
        {
            ::CloseHandle(m_renderEvent);
            ::CloseHandle(m_receiveCanWait);
            //this frees up resources
            freeResources();
        }

        QSize VideoRendererSoftFilter::videoSize() const
        {
            QSize ret;
            const AM_MEDIA_TYPE &mt = m_inputPin->connectedType();
            if (mt.pbFormat && mt.pbFormat) {
                if (mt.formattype == FORMAT_VideoInfo) {
                    const VIDEOINFOHEADER *header = reinterpret_cast<VIDEOINFOHEADER*>(mt.pbFormat);
                    const int h = qAbs(header->bmiHeader.biHeight),
                        w = qAbs(header->bmiHeader.biWidth);
                    ret = QSize(w, h);
                } else if (mt.formattype == FORMAT_VideoInfo2) {
                    const VIDEOINFOHEADER2 *header = reinterpret_cast<VIDEOINFOHEADER2*>(mt.pbFormat);
                    const int h = qAbs(header->bmiHeader.biHeight),
                        w = qAbs(header->bmiHeader.biWidth);
                    ret = QSize(w, h);
                }
            }
            return ret;
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

            m_size = videoSize();
            if (!m_size.isValid()) {
                return VFW_E_INVALIDMEDIATYPE;
            }

#ifdef FPS_COUNTER
            nbFramesProcessed++;
#endif

            REFERENCE_TIME start = 0, stop = 0;
            HRESULT hr = sample->GetTime(&start, &stop);

            ComPointer<IReferenceClock> clock;
            GetSyncSource(clock.pparam());

            const bool playing = SUCCEEDED(hr) && state() == State_Running && clock;

            if (playing) {
                REFERENCE_TIME current;
                clock->GetTime(&current);

                DWORD_PTR advise;

                //let's synchronize here
                clock->AdviseTime(m_start, start,
                    reinterpret_cast<HEVENT>(m_renderEvent), &advise);

                HANDLE handles[] = {m_receiveCanWait, m_renderEvent};
                if (::WaitForMultipleObjects(2, handles, false, INFINITE) == WAIT_OBJECT_0) {
                    if (state() != State_Stopped && !m_inputPin->isFlushing()) {
                        ::ResetEvent(m_receiveCanWait);
                    }
                }
            }


            //the let's lock the sample to be used in the GUI thread
            {
                QMutexLocker locker(&m_mutex);
                sample->AddRef();
                m_sampleBuffer = ComPointer<IMediaSample>(sample);
            }

            //image is updated: we should update the widget
            //we should never call directly members of target due to thread-safety
            QApplication::postEvent(m_renderer, new QEvent(QEvent::UpdateRequest));

            if (!playing) {
                //useless to test the return value of WaitForSingleObject: timeout can't happen
                ::WaitForSingleObject(m_receiveCanWait, INFINITE); 
                if (state() != State_Stopped && !m_inputPin->isFlushing()) {
                    ::ResetEvent(m_receiveCanWait);
                }
            }

            //everything should be ok
            return S_OK;
        }

#ifndef QT_NO_OPENGL
        bool VideoRendererSoftFilter::checkGLPrograms()
        {
            if (!m_checkedPrograms) {
                m_checkedPrograms = true;

                glProgramStringARB = (_glProgramStringARB) wglGetProcAddress("glProgramStringARB");
                glBindProgramARB = (_glBindProgramARB) wglGetProcAddress("glBindProgramARB");
                glDeleteProgramsARB = (_glDeleteProgramsARB) wglGetProcAddress("glDeleteProgramsARB");
                glGenProgramsARB = (_glGenProgramsARB) wglGetProcAddress("glGenProgramsARB");
                glProgramLocalParameter4fARB = (_glProgramLocalParameter4fARB) wglGetProcAddress("glProgramLocalParameter4fARB");
                glActiveTexture = (_glActiveTexture) wglGetProcAddress("glActiveTexture");

                //we check only once if the widget is drawn using opengl
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
            return m_usingOpenGL;
        }

        void VideoRendererSoftFilter::updateTexture()
        {
            if (!m_sampleBuffer) {
                return; //the texture is already up2date or their is no data yet
            }

            uchar *data = 0;
            m_sampleBuffer->GetPointer(&data);

            if (m_inputPin->connectedType().subtype == MEDIASUBTYPE_YV12) {
                int w[3] = { m_size.width(), m_size.width()/2, m_size.width()/2 };
                int h[3] = { m_size.height(), m_size.height()/2, m_size.height()/2 };
                int offs[3] = { 0, m_size.width()*m_size.height(), m_size.width()*m_size.height()*5/4 };

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
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_size.width() / 2, m_size.height(), 0,
                    GL_RGBA, GL_UNSIGNED_BYTE, data);

                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            }
            m_sampleBuffer = ComPointer<IMediaSample>();
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
            if (painter.paintEngine() && painter.paintEngine()->type() == QPaintEngine::OpenGL && checkGLPrograms()) {
                //for now we only support YUV (both YV12 and YUY2)
                updateTexture();

                if (!m_textureUploaded) {
                    //we simply fill the whole video with content
                    //the callee has already set the brush
                    painter.drawRect(r);
                    return;
                }

                //let's draw the texture

                //Let's pass the other arguments
                const Program prog = (m_inputPin->connectedType().subtype == MEDIASUBTYPE_YV12) ? YV12toRGB : YUY2toRGB;
                glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, m_program[prog]);
                //loading the parameters
                glProgramLocalParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 0, m_brightness / 256., m_contrast, qCos(m_hue), qSin(m_hue));
                glProgramLocalParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 1, m_saturation, painter.opacity() /*alpha */, 0. /*dummy*/, 0. /*dummy*/);

                glEnable(GL_FRAGMENT_PROGRAM_ARB);

                const float v_array[] = { r.left(), r.top(), r.right()+1, r.top(), r.right()+1, r.bottom()+1, r.left(), r.bottom()+1 };

                float tx_array[12] = {0., 0., 0., 1.,
                    0., 0., 1., 1.,
                    0., 0., 1., 0.};

                if (prog == YUY2toRGB) {
                    const float w = m_size.width() / 2,
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
            } else
#endif
              if (m_sampleBuffer) {
                //we need to get the sample data
                uchar *data = 0;
                m_sampleBuffer->GetPointer(&data);


                //let's update the current image
                if (m_inputPin->connectedType().subtype == MEDIASUBTYPE_YV12) {
                    convertYV12toRGB(data, m_size, m_currentImage,
                        m_brightness, m_contrast, m_hue, m_saturation);
                } else if (m_inputPin->connectedType().subtype == MEDIASUBTYPE_YUY2) {
                    convertYUY2toRGB(data, m_size, m_currentImage,
                        m_brightness, m_contrast, m_hue, m_saturation);
                } else if (m_inputPin->connectedType().subtype == MEDIASUBTYPE_RGB32) {
                    normalizeRGB(data, m_size, m_currentImage);
                }
                m_sampleBuffer = ComPointer<IMediaSample>();
            }

            if (m_currentImage.isNull()) {
                //we simply fill the whole video with content
                //the callee has alrtead set the brush
                painter.drawRect(r);
            } else {
                painter.drawImage(0, 0, m_currentImage);
            }
        }


        void VideoRendererSoftFilter::normalizeRGB(const uchar *data, const QSize &s, QImage &destImage)
        {
            const int w = s.width(),
                      h = s.height();
            if (destImage.size() != s) {
                destImage = QImage(w, h, QImage::Format_ARGB32_Premultiplied);
            }
            if (destImage.isNull()) {
                return; //the system can't allocate the memory for the image drawing
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
        void VideoRendererSoftFilter::convertYV12toRGB(const uchar *data, const QSize &s, QImage &destImage,
            qreal brightness, qreal contrast, qreal hue, qreal saturation)
        {
            const int w = s.width(),
                      h = s.height();

            //let's cache some computation
            const int cosHx256 = qRound(qCos(hue) * contrast * saturation * 256),
                      sinHx256 = qRound(qSin(hue) * contrast * saturation * 256);

            int Yvalue[256];
            for(int i = 0;i<256;++i) {
                Yvalue[i] = qRound(((i - 16) * contrast + brightness) * 298 + 128);
            }


            if (destImage.size() != s) {
                destImage = QImage(w, h, QImage::Format_ARGB32_Premultiplied);
            }

            if (destImage.isNull()) {
                return; //the system can't allocate the memory for the image drawing
            }

            QRgb *dest = reinterpret_cast<QRgb*>(destImage.bits());
            const uchar *dataY = data,
                *dataV = data + (w*h),
                *dataU = dataV + (w*h)/4;

            uint *line1 = dest,
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
                    *line1++ = CLIPPED_PIXEL(y11, compRed, compGreen, compBlue);

                    //1st line, 2nd pixel
                    *line1++ = CLIPPED_PIXEL(y12, compRed, compGreen, compBlue);

                    //2nd line 1st pixel
                    *line2++ = CLIPPED_PIXEL(y21, compRed, compGreen, compBlue);

                    //2nd line 2nd pixel
                    *line2++ = CLIPPED_PIXEL(y22, compRed, compGreen, compBlue);

                } //for

                //end of the line
                dataY += w;
                line1 = line2;
                line2 += w;

            } //for

        }

        //we render data interpreted as YUY2 into m_renderbuffer
        void VideoRendererSoftFilter::convertYUY2toRGB(const uchar *data, const QSize &s, QImage &destImage,
                                         qreal brightness, qreal contrast, qreal hue, qreal saturation)
        {
            const int w = s.width(),
                      h = s.height();

            //let's cache some computation
            int Yvalue[256];
            for(int i = 0;i<256;++i) {
                Yvalue[i] = qRound(((i - 16) * contrast + brightness) * 298 + 128);
            }

            const int cosHx256 = qRound(qCos(hue) * contrast * saturation * 256),
                      sinHx256 = qRound(qSin(hue) * contrast * saturation * 256);

            if (destImage.size() != s) {
                //this will only allocate memory when needed
                destImage = QImage(w, h, QImage::Format_ARGB32_Premultiplied);
            }
            if (destImage.isNull()) {
                return; //the system can't allocate the memory for the image drawing
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
                *dest++ = CLIPPED_PIXEL(y1, compRed, compGreen, compBlue);

                //second pixel
                *dest++ = CLIPPED_PIXEL(y2, compRed, compGreen, compBlue);
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


        bool VideoRendererSoft::isNative() const
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
                const QVector<QRect> reg = (QRegion(rect) - m_videoRect).rects();
                for (int i = 0; i < reg.count(); ++i) {
                    painter.drawRect(reg.at(i));
                }
            }

            painter.setRenderHint(QPainter::SmoothPixmapTransform);
            painter.setTransform(m_transform, true);
            QSize vsize = videoSize();
            m_renderer->repaintCurrentFrame(painter, QRect(0,0, vsize.width(), vsize.height()));
        }

        void VideoRendererSoft::notifyResize(const QSize &size,
            Phonon::VideoWidget::AspectRatio aspectRatio, Phonon::VideoWidget::ScaleMode scaleMode)
        {
            const QSize vsize = videoSize();
            internalNotifyResize(size, vsize, aspectRatio, scaleMode);

            m_transform.reset();

            if (vsize.isValid() && size.isValid()) {
                m_transform.translate(m_dstX, m_dstY);
                const qreal sx = qreal(m_dstWidth) / qreal(vsize.width()),
                    sy = qreal(m_dstHeight) / qreal(vsize.height());
                m_transform.scale(sx, sy);
                m_videoRect = m_transform.mapRect( QRect(0,0, vsize.width(), vsize.height()));
            }
        }

        QSize VideoRendererSoft::videoSize() const
        {
            if (m_renderer->pins().first()->connected()) {
                return m_renderer->videoSize();
            } else {
                return m_renderer->currentImage().size();
            }
        }

        void VideoRendererSoft::applyMixerSettings(qreal brightness, qreal contrast, qreal hue, qreal saturation)
        {
            m_renderer->applyMixerSettings(brightness, contrast, hue, saturation);
        }

        QImage VideoRendererSoft::snapshot() const
        {
            return m_renderer->currentImage(); //not accurate (especially when using opengl...)
        }

        void VideoRendererSoft::setSnapshot(const QImage &image)
        {
            m_renderer->setCurrentImage(image);
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

#endif //QT_NO_PHONON_VIDEO
