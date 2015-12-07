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
#ifndef PHONON_ABSTRACTVIDEORENDERER_H
#define PHONON_ABSTRACTVIDEORENDERER_H

#include "backendnode.h"

#include <phonon/videowidget.h>

#include <QtGui/QImage>

QT_BEGIN_NAMESPACE

namespace Phonon
{
    namespace DS9
    {
        //this is the interface used by the videorenderer from the VideoWidget class
        class AbstractVideoRenderer
        {
        public:
            virtual ~AbstractVideoRenderer();

            virtual void repaintCurrentFrame(QWidget *target, const QRect &rect) = 0;
            virtual void notifyResize(const QRect&, Phonon::VideoWidget::AspectRatio, Phonon::VideoWidget::ScaleMode) = 0;
            virtual void applyMixerSettings(qreal brightness, qreal contrast, qreal m_hue, qreal saturation) = 0;
            virtual void setActive(bool) = 0;
            virtual bool isHardwareAccelerated() const = 0;
            virtual void notifyVideoLoaded() {}

            Filter getFilter() const;
            QSize sizeHint() const;

        protected:
            virtual QSize videoSize() const = 0;

            AbstractVideoRenderer();
            void internalNotifyResize(const QSize &size, const QSize &videoSize, 
                Phonon::VideoWidget::AspectRatio aspectRatio, Phonon::VideoWidget::ScaleMode scaleMode);


            Filter m_filter;
            int m_dstX, m_dstY, m_dstWidth, m_dstHeight;

        };
    }
}

QT_END_NAMESPACE

#endif
