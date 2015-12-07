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

#include "videoframe.h"
#include "quicktimevideoplayer.h"

QT_BEGIN_NAMESPACE

namespace Phonon
{
namespace QT7
{
    VideoFrame::VideoFrame()
    {
        initMembers();
    }

    VideoFrame::VideoFrame(QuickTimeVideoPlayer *videoPlayer, const LinkTimeProxy &timeStamp)
    {
        initMembers();
        m_videoPlayer = videoPlayer;
        m_timeStamp = timeStamp;
    }

    VideoFrame::VideoFrame(const VideoFrame& frame)
    {
        copyMembers(frame);
        retain();
    }

    void VideoFrame::operator=(const VideoFrame& frame)
    {
        if (this == &frame)
            return;
            
        release();
        copyMembers(frame);
        retain();
    }

    void VideoFrame::initMembers()
    {
        m_cvTextureRef = 0;
        m_videoPlayer = 0;
        m_brightness = 0;
        m_contrast = 0;
        m_hue = 0;
        m_saturation = 0;
        m_opacity = 1;
        m_backgroundFrame = 0;

#if 0 // will be awailable in a later version of phonon.
        m_ciFrameRef = 0;

        // Move this stuff to a shared resource pool!
        AGLContext aglContext = sharedOpenGLContext();
        AGLPixelFormat aglPixelFormat = sharedOpenGLPixelFormat();

        GLint swapInterval = 1;
        GLint surfaceOpacity = 1;
        aglSetInteger(aglContext, AGL_SURFACE_OPACITY, &surfaceOpacity);
        aglSetInteger(aglContext, AGL_SWAP_INTERVAL, &swapInterval);

        coreImageContext = objc_createCiContext(aglContext, aglPixelFormat);
#endif
    }

    void VideoFrame::copyMembers(const VideoFrame& frame)
    {
        m_cvTextureRef = frame.m_cvTextureRef;
        m_videoPlayer = frame.m_videoPlayer;
        m_timeStamp = frame.m_timeStamp;
        m_brightness = frame.m_brightness;
        m_contrast = frame.m_contrast;
        m_hue = frame.m_hue;
        m_saturation = frame.m_saturation;
        m_opacity = frame.m_opacity;
        m_backgroundFrame = frame.m_backgroundFrame;

#if 0 // will be awailable in a later version of phonon.
        m_ciFrameRef = frame.m_ciFrameRef;
#endif
    }

    VideoFrame::~VideoFrame()
    {
        release();
#if 0 // will be awailable in a later version of phonon.
        objc_releaseCiContext(coreImageContext);
#endif
    }

    void VideoFrame::setBackgroundFrame(const VideoFrame &frame)
    {
        m_backgroundFrame = new VideoFrame(frame);
    }

    QRect VideoFrame::frameRect() const
    {
        return m_videoPlayer->videoRect();
    }

    CVOpenGLTextureRef VideoFrame::cvTextureRef() const
    {
        if (!m_cvTextureRef && m_videoPlayer){
            m_videoPlayer->setColors(m_brightness, m_contrast, m_hue, m_saturation);
            (const_cast<VideoFrame *>(this))->m_cvTextureRef = m_videoPlayer->createCvTexture(m_timeStamp);
        }
        return m_cvTextureRef;
    }

    GLuint VideoFrame::glTextureRef() const
    {
        return CVOpenGLTextureGetName(cvTextureRef());
    }

#if 0 // will be awailable in a later version of phonon.
    void *VideoFrame::coreImageRef() const
    {
        if (!m_ciFrameRef)
            (const_cast<VideoFrame *>(this))->m_ciFrameRef = objc_ciImageFromCvImageBuffer(cvTextureRef());
        return m_ciFrameRef;
    }

    void VideoFrame::applyCoreImageFilter(void *filter)
    {
        objc_applyCiFilter(coreImageRef(), filter);
    }

    bool VideoFrame::hasCoreImage()
    {
        return m_ciFrameRef != 0;
    }
#endif

    void VideoFrame::setColors(qreal brightness, qreal contrast, qreal hue, qreal saturation)
    {
        if (m_backgroundFrame)
            m_backgroundFrame->setColors(brightness, contrast, hue, saturation);
        if (m_brightness == brightness
            && m_contrast == contrast
            && m_hue == hue 
            && m_saturation == saturation)
            return;
        
        m_brightness = brightness;
        m_contrast = contrast;
        m_hue = hue;
        m_saturation = saturation;
        invalidateTexture();
    }
    
    void VideoFrame::setBaseOpacity(qreal opacity)
    {
        m_opacity = opacity;
    }

    QImage VideoFrame::toQImage()
    {
        if (!m_videoPlayer)
            return QImage();
        if (m_videoPlayer->setCurrentGLContext())
            invalidateTexture();
        CVOpenGLTextureRef texRef = cvTextureRef();
        if (!texRef)
            return QImage();

        GLenum target = CVOpenGLTextureGetTarget(texRef);
        glEnable(target);
        glBindTexture(target, CVOpenGLTextureGetName(texRef));
        QImage image(frameRect().size(), QImage::Format_ARGB32);
        glGetTexImage(target, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, image.bits());
        return image;
    }

    void VideoFrame::drawFrame(const QRect &rect, float opacity, bool origoLowLeft) const
    {
        if (!m_videoPlayer)
            return;            
        if (m_videoPlayer->setCurrentGLContext())
            invalidateTexture();
            
#if 0 // will be awailable in a later version of phonon.
        objc_drawCiImage(m_currentFrame.coreImageRef(), m_drawFrameRect, coreImageContext);
#endif
        drawGLTexture(rect, origoLowLeft, opacity);
    }

    void VideoFrame::drawGLTexture(const QRect &rect, float opacity, bool origoLowLeft) const
    {
        if (m_backgroundFrame)
            m_backgroundFrame->drawGLTexture(rect, opacity, origoLowLeft);
            
        CVOpenGLTextureRef texRef = cvTextureRef();
        if (!texRef)
            return;

        glPushMatrix();
            glDisable(GL_CULL_FACE);            
            GLenum target = CVOpenGLTextureGetTarget(texRef);
            glEnable(target);
            
            opacity *= m_opacity;
            if (opacity < 1){
                glEnable(GL_BLEND);
                glColor4f(1, 1, 1, opacity);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            } else {
                glColor3f(1, 1, 1);
                glDisable(GL_BLEND);
            }
            
            glBindTexture(target, CVOpenGLTextureGetName(texRef));
            glTexParameterf(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameterf(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            GLfloat lowerLeft[2], lowerRight[2], upperRight[2], upperLeft[2];
            
            // Work-around for CVOpenGLTextureGetCleanTexCoords. It 
            // fails to catch movies with origo != (0, 0) 
            if (origoLowLeft){
                CVOpenGLTextureGetCleanTexCoords(texRef, lowerLeft, lowerRight, upperRight, upperLeft);
                // Work-around for CVOpenGLTextureGetCleanTexCoords. It 
                // fails to catch movies with origo != (0, 0) :
                upperLeft[0] += m_videoPlayer->videoRect().x();
                lowerLeft[0] += m_videoPlayer->videoRect().x();
                upperLeft[1] += m_videoPlayer->videoRect().y();
                upperRight[1] += m_videoPlayer->videoRect().y();
            } else {
                CVOpenGLTextureGetCleanTexCoords(texRef, upperLeft, upperRight, lowerRight, lowerLeft);
                // upperLeft[0] += m_videoPlayer->videoRect().x();
                // lowerLeft[0] += m_videoPlayer->videoRect().x();
                // upperLeft[1] += m_videoPlayer->videoRect().y();
                // upperRight[1] += m_videoPlayer->videoRect().y();
            }
                
            // Draw the frame:
            glBegin(GL_QUADS);
                glTexCoord2f(lowerLeft[0], lowerLeft[1]);
                glVertex2i(rect.topLeft().x(), rect.topLeft().y());
                glTexCoord2f(lowerRight[0], lowerRight[1]);
                glVertex2i(rect.topRight().x() + 1, rect.topRight().y());
                glTexCoord2f(upperRight[0], upperRight[1]);
                glVertex2i(rect.bottomRight().x() + 1, rect.bottomRight().y() + 1);
                glTexCoord2f(upperLeft[0], upperLeft[1]);
                glVertex2i(rect.bottomLeft().x(), rect.bottomLeft().y() + 1);
            glEnd();
        glPopMatrix();
    }

    bool VideoFrame::isEmpty()
    {
        return (m_videoPlayer == 0);
    }

    void VideoFrame::invalidateTexture() const
    {
        if (m_cvTextureRef){
            CVOpenGLTextureRelease(m_cvTextureRef);
            (const_cast<VideoFrame *>(this))->m_cvTextureRef = 0;
        }
    }

    void VideoFrame::retain() const
    {    
        if (m_cvTextureRef)
            CVOpenGLTextureRetain(m_cvTextureRef);
        if (m_backgroundFrame)
            m_backgroundFrame->retain();

#if 0 // will be awailable in a later version of phonon.
        if (m_ciFrameRef)
            objc_retainCiImage(m_ciFrameRef);
#endif
    }

    void VideoFrame::release() const
    {
        if (m_cvTextureRef)
            CVOpenGLTextureRelease(m_cvTextureRef);
        if (m_backgroundFrame)
            m_backgroundFrame->release();

        (const_cast<VideoFrame *>(this))->m_backgroundFrame = 0;
        (const_cast<VideoFrame *>(this))->m_cvTextureRef = 0;

#if 0 // will be awailable in a later version of phonon.
        if (m_ciFrameRef)
            objc_releaseCiImage(m_ciFrameRef);
        (const_cast<VideoFrame *>(this))->m_ciFrameRef = 0;
#endif

    }

}} //namespace Phonon::QT7

QT_END_NAMESPACE
