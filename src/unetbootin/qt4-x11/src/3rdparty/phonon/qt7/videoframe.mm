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

#include "videoframe.h"
#include "quicktimevideoplayer.h"
#import <QuartzCore/CIFilter.h>
#import <QuartzCore/CIContext.h>

QT_BEGIN_NAMESPACE

namespace Phonon
{
namespace QT7
{

    VideoFrame::VideoFrame()
    {
        initMembers();
    }

    VideoFrame::VideoFrame(QuickTimeVideoPlayer *videoPlayer)
    {
        initMembers();
        m_videoPlayer = videoPlayer;
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
        m_cachedCVTextureRef = 0;
		m_cachedCIImage = 0;
        m_cachedNSBitmap = 0;
        m_videoPlayer = 0;
        m_brightness = 0;
        m_contrast = 0;
        m_hue = 0;
        m_saturation = 0;
        m_opacity = 1;
        m_backgroundFrame = 0;
    }

    void VideoFrame::copyMembers(const VideoFrame& frame)
    {
        m_cachedCVTextureRef = frame.m_cachedCVTextureRef;
		m_cachedCIImage = frame.m_cachedCIImage;
		m_cachedQImage = frame.m_cachedQImage;
        m_cachedNSBitmap = frame.m_cachedNSBitmap;
        m_videoPlayer = frame.m_videoPlayer;
        m_brightness = frame.m_brightness;
        m_contrast = frame.m_contrast;
        m_hue = frame.m_hue;
        m_saturation = frame.m_saturation;
        m_opacity = frame.m_opacity;
        m_backgroundFrame = frame.m_backgroundFrame;
    }

    VideoFrame::~VideoFrame()
    {
        release();
    }

    QuickTimeVideoPlayer *VideoFrame::videoPlayer()
    {
        return m_videoPlayer;
    }

    void VideoFrame::setBackgroundFrame(const VideoFrame &frame)
    {
        m_backgroundFrame = new VideoFrame(frame);
    }

    QRect VideoFrame::frameRect() const
    {
        return m_videoPlayer->videoRect();
    }

    CVOpenGLTextureRef VideoFrame::cachedCVTexture() const
    {
        if (!m_cachedCVTextureRef && m_videoPlayer){
            m_videoPlayer->setColors(m_brightness, m_contrast, m_hue, m_saturation);
            (const_cast<VideoFrame *>(this))->m_cachedCVTextureRef = m_videoPlayer->currentFrameAsCVTexture();
        }
        return m_cachedCVTextureRef;
    }

    void *VideoFrame::cachedCIImage() const
    {
        if (!m_cachedCIImage && m_videoPlayer){
            m_videoPlayer->setColors(m_brightness, m_contrast, m_hue, m_saturation);
            (const_cast<VideoFrame *>(this))->m_cachedCIImage = m_videoPlayer->currentFrameAsCIImage();
        }
        return m_cachedCIImage;
    }

    GLuint VideoFrame::glTextureRef() const
    {
        return CVOpenGLTextureGetName(cachedCVTexture());
    }

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

        invalidateImage();
    }

	CGRect VideoFrame::QRectToCGRect(const QRect & qrect)
	{
        CGRect cgrect;
        cgrect.origin.x = qrect.x();
        cgrect.origin.y = qrect.y() + qrect.height();
        cgrect.size.width = qrect.width();
        cgrect.size.height = -qrect.height();
		return cgrect;
	}

	bool VideoFrame::hasColorAdjustments()
	{
		return (m_brightness || m_contrast || m_saturation || m_hue);
	}

    void VideoFrame::setBaseOpacity(qreal opacity)
    {
        m_opacity = opacity;
    }

    void VideoFrame::drawQImage(QPainter *p, const QRect &rect) const
	{
        if (!m_videoPlayer)
            return;
#ifdef QUICKTIME_C_API_AVAILABLE
        if (m_cachedQImage.isNull()){
            m_videoPlayer->setColors(m_brightness, m_contrast, m_hue, m_saturation);
            (const_cast<VideoFrame *>(this))->m_cachedQImage = m_videoPlayer->currentFrameAsQImage();
        }
#else
        // Since cocoa-64 doesn't give us OpenGL textures directly, the process of converting
        // CIImges into QImages takes time. We could still call m_videoPlayer->currentFrameAsQImage(),
        // but because of bitmap memory management issues, and the fact that we need to swap red and
        // blue, we can optimize the process a bit here since we are going to draw immidiatly:
        CIImage *img = (CIImage*)cachedCIImage();
        if (!img)
            return;

        if (!m_cachedNSBitmap){
            (const_cast<VideoFrame *>(this))->m_cachedNSBitmap =
                [[NSBitmapImageRep alloc] initWithCIImage:img];
            CGRect bounds = [img extent];
            int w = bounds.size.width;
            int h = bounds.size.height;
            (const_cast<VideoFrame *>(this))->m_cachedQImage =
                QImage([m_cachedNSBitmap bitmapData], w, h, QImage::Format_ARGB32);
			// Swap red and blue (same as QImage::rgbSwapped, but without copy)
            for (int i=0; i<h; ++i) {
                uint *p = (uint*) m_cachedQImage.scanLine(i);
                uint *end = p + w;
                while (p < end) {
                    *p = ((*p << 16) & 0xff0000) | ((*p >> 16) & 0xff) | (*p & 0xff00ff00);
                    p++;
                }
            }
        }
#endif
        p->drawImage(rect, m_cachedQImage);
	}

    void VideoFrame::drawCIImage(const QRect &rect, float opacity) const
	{
		drawCIImage(QRectToCGRect(rect), opacity);
	}

    void VideoFrame::drawCIImage(const CGRect &rect, float opacity) const
	{
		Q_UNUSED(opacity);
		CIImage *img = (CIImage *) cachedCIImage();
		if (!img)
			return;

	    CIContext* ciContext = [[NSGraphicsContext currentContext] CIContext];
	    [ciContext drawImage:img inRect:rect fromRect:[img extent]];
	}

    void VideoFrame::drawCVTexture(const QRect &rect, float opacity) const
    {
        if (!m_videoPlayer)
            return;
        if (m_backgroundFrame)
            m_backgroundFrame->drawCVTexture(rect, opacity);

        CVOpenGLTextureRef texRef = cachedCVTexture();
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
            CVOpenGLTextureGetCleanTexCoords(texRef, lowerLeft, lowerRight, upperRight, upperLeft);

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

    void VideoFrame::drawGLTexture(const QRect &rect, float opacity) const
    {
        if (!m_videoPlayer)
            return;
        if (m_backgroundFrame)
            m_backgroundFrame->drawGLTexture(rect, opacity);

        GLuint texture = m_videoPlayer->currentFrameAsGLTexture();
        if (!texture)
            return;

        glPushMatrix();
            glDisable(GL_CULL_FACE);
            glEnable(GL_TEXTURE_RECTANGLE_EXT);

            opacity *= m_opacity;
            if (opacity < 1){
                glEnable(GL_BLEND);
                glColor4f(1, 1, 1, opacity);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            } else {
                glColor3f(1, 1, 1);
                glDisable(GL_BLEND);
            }

            glBindTexture(GL_TEXTURE_RECTANGLE_EXT, texture);
            glTexParameterf(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameterf(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            QRect videoRect = m_videoPlayer->videoRect();
            GLfloat lowerLeft[2], lowerRight[2], upperRight[2], upperLeft[2];
            lowerLeft[0] = 0;
            lowerLeft[1] = videoRect.height();
            lowerRight[0] = videoRect.width();
            lowerRight[1] = videoRect.height();
            upperRight[0] = videoRect.width();
            upperRight[1] = 0;
            upperLeft[0] = 0;
            upperLeft[1] = 0;

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


        // FOR NOW. FREE THE TEXTURE:
        glDeleteTextures(1, &texture);
    }

    bool VideoFrame::isEmpty()
    {
        return (m_videoPlayer == 0);
    }

    void VideoFrame::invalidateImage() const
    {
        if (m_cachedCVTextureRef){
            CVOpenGLTextureRelease(m_cachedCVTextureRef);
            (const_cast<VideoFrame *>(this))->m_cachedCVTextureRef = 0;
        }
        if (m_cachedCIImage){
			[(CIImage *) m_cachedCIImage release];
            (const_cast<VideoFrame *>(this))->m_cachedCIImage = 0;
        }
        if (m_cachedNSBitmap){
            [m_cachedNSBitmap release];
            (const_cast<VideoFrame *>(this))->m_cachedNSBitmap = 0;
        }
        (const_cast<VideoFrame *>(this))-> m_cachedQImage = QImage();
    }

    void VideoFrame::retain() const
    {
        if (m_cachedCVTextureRef)
            CVOpenGLTextureRetain(m_cachedCVTextureRef);
		if (m_cachedCIImage)
			[(CIImage *) m_cachedCIImage retain];
        if (m_backgroundFrame)
            m_backgroundFrame->retain();
        if (m_cachedNSBitmap)
            [m_cachedNSBitmap retain];
    }

    void VideoFrame::release() const
    {
        if (m_cachedCVTextureRef)
            CVOpenGLTextureRelease(m_cachedCVTextureRef);
		if (m_cachedCIImage)
			[(CIImage *) m_cachedCIImage release];
        if (m_backgroundFrame)
            m_backgroundFrame->release();
        if (m_cachedNSBitmap)
            [m_cachedNSBitmap release];

        (const_cast<VideoFrame *>(this))->m_backgroundFrame = 0;
        (const_cast<VideoFrame *>(this))->m_cachedCVTextureRef = 0;
        (const_cast<VideoFrame *>(this))->m_cachedCIImage = 0;
        (const_cast<VideoFrame *>(this))->m_cachedNSBitmap = 0;
    }

}} //namespace Phonon::QT7

QT_END_NAMESPACE
