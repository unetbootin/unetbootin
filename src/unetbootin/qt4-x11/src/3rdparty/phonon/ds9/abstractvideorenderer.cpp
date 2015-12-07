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

#include "abstractvideorenderer.h"

#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>

QT_BEGIN_NAMESPACE

#ifndef QT_NO_PHONON_VIDEO

namespace Phonon
{
    namespace DS9
    {

        AbstractVideoRenderer::AbstractVideoRenderer() : 
            m_dstX(0), m_dstY(0), m_dstWidth(0), m_dstHeight(0),
                m_active(false)
        {
        }

        AbstractVideoRenderer::~AbstractVideoRenderer()
        {
        }

        Filter AbstractVideoRenderer::getFilter() const
        {
            return m_filter;
        }

        QSize AbstractVideoRenderer::sizeHint() const
        {
            QSize s = videoSize();
            if (s.isNull()) {
                s = QSize(640, 480).boundedTo( QApplication::desktop()->availableGeometry().size() );
            }
            return s;
        }

        void AbstractVideoRenderer::setActive(bool b)
        {
            m_active = b;
        }

        bool AbstractVideoRenderer::isActive() const
        {
            return m_active;
        }

        void AbstractVideoRenderer::internalNotifyResize(const QSize &size, const QSize &videoSize,
            Phonon::VideoWidget::AspectRatio aspectRatio, Phonon::VideoWidget::ScaleMode scaleMode)
        {
            //update the video rects
            qreal ratio = -1.;
            const int videoWidth = videoSize.width(),
                videoHeight = videoSize.height();

            switch(aspectRatio)
            {
            case Phonon::VideoWidget::AspectRatioAuto:
                {
                    //preserve the aspect ratio of the video
                    ratio = qreal(videoWidth) / qreal(videoHeight);
                }
                break;
            case Phonon::VideoWidget::AspectRatio4_3:
                ratio = qreal(4. / 3.);
                break;
            case Phonon::VideoWidget::AspectRatio16_9:
                ratio = qreal(16. / 9.);
                break;
            case Phonon::VideoWidget::AspectRatioWidget:
            default:
                break;
            }

            const qreal realWidth = size.width(), 
                realHeight = size.height();

            //reinitialization
            m_dstWidth = size.width();
            m_dstHeight = size.height();
            m_dstX = m_dstY = 0;

            if (ratio > 0) {
                if (realWidth / realHeight > ratio && scaleMode == Phonon::VideoWidget::FitInView
                    || realWidth / realHeight < ratio && scaleMode == Phonon::VideoWidget::ScaleAndCrop) {
                        //the height is correct, let's change the width
                        m_dstWidth = qRound(realHeight * ratio);
                        m_dstX = qRound((realWidth - realHeight * ratio) / 2.);
                } else {
                    m_dstHeight = qRound(realWidth / ratio);
                    m_dstY = qRound((realHeight - realWidth / ratio) / 2.);
                }
            }
        }
    }
}

#endif //QT_NO_PHONON_EFFECT

QT_END_NAMESPACE
