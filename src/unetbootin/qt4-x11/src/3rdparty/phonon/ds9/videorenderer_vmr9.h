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

#ifndef PHONON_VIDEORENDERER_VMR9_H
#define PHONON_VIDEORENDERER_VMR9_H

#include "abstractvideorenderer.h"
#include "compointer.h"

QT_BEGIN_NAMESPACE

#ifndef QT_NO_PHONON_VIDEO

namespace Phonon
{
    namespace DS9
    {
        class VideoRendererVMR9 : public AbstractVideoRenderer
        {
        public:
            VideoRendererVMR9(QWidget *target);
            ~VideoRendererVMR9();

            //Implementation from AbstractVideoRenderer
            void repaintCurrentFrame(QWidget *target, const QRect &rect);
            void notifyResize(const QSize&, Phonon::VideoWidget::AspectRatio, Phonon::VideoWidget::ScaleMode);
            QSize videoSize() const;
            QImage snapshot() const;
            void applyMixerSettings(qreal brightness, qreal contrast, qreal m_hue, qreal saturation);
            bool isNative() const;
        private:
            QWidget *m_target;
        };
    }
}

#endif //QT_NO_PHONON_VIDEO

QT_END_NAMESPACE

#endif

