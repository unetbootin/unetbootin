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

#ifndef Phonon_QT7_VIDEOFRAME_H
#define Phonon_QT7_VIDEOFRAME_H

#import <QuartzCore/CVOpenGLTexture.h>
#import <AppKit/NSImage.h>
#undef check // avoid name clash;

#include <QtCore>
#include <QtGui>

QT_BEGIN_NAMESPACE

class QGLContext;

namespace Phonon
{
namespace QT7
{
    class QuickTimeVideoPlayer;
	class QNSBitmapImage;

    class VideoFrame
    {
        public:
            VideoFrame();
            VideoFrame(QuickTimeVideoPlayer *videoPlayer);
            VideoFrame(const VideoFrame& frame);
            void operator=(const VideoFrame& frame);
            ~VideoFrame();


            CVOpenGLTextureRef cachedCVTexture() const;
			void *cachedCIImage() const;
            GLuint glTextureRef() const;

			void drawQImage(QPainter *p, const QRect &rect) const;
			void drawCIImage(const CGRect &rect, float opacity = 1.0f) const;
			void drawCIImage(const QRect &rect, float opacity = 1.0f) const;
            void drawCVTexture(const QRect &rect, float opacity = 1.0f) const;
            void drawGLTexture(const QRect &rect, float opacity = 1.0f) const;

            void applyCoreImageFilter(void *filter);
            void setColors(qreal brightness, qreal contrast, qreal hue, qreal saturation);
			bool hasColorAdjustments();
            void setBaseOpacity(qreal opacity);
            void setBackgroundFrame(const VideoFrame &frame);

            bool isEmpty();
            QRect frameRect() const;
            QuickTimeVideoPlayer *videoPlayer();

            void retain() const;
            void release() const;

			static CGRect QRectToCGRect(const QRect & qrect);

        private:
            CVOpenGLTextureRef m_cachedCVTextureRef;
            void *m_cachedCIImage;
			QImage m_cachedQImage;
            NSBitmapImageRep *m_cachedNSBitmap;

            QuickTimeVideoPlayer *m_videoPlayer;
            VideoFrame *m_backgroundFrame;

            qreal m_brightness;
            qreal m_contrast;
            qreal m_hue;
            qreal m_saturation;
            qreal m_opacity;

            void initMembers();
            void copyMembers(const VideoFrame& frame);
            void invalidateImage() const;
    };

}} //namespace Phonon::QT7

QT_END_NAMESPACE
#endif // Phonon_QT7_VIDEOFRAME_H
