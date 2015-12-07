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
#ifndef PHONON_VIDEORENDERER_VMR9_H
#define PHONON_VIDEORENDERER_VMR9_H

#include "abstractvideorenderer.h"
#include "compointer.h"

#include <d3d9.h>
#include <vmr9.h>

QT_BEGIN_NAMESPACE

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
            void notifyResize(const QRect&, Phonon::VideoWidget::AspectRatio, Phonon::VideoWidget::ScaleMode);
            QSize videoSize() const;
            QImage snapshot() const;
            Filter getFilter() const;
            void applyMixerSettings(qreal brightness, qreal contrast, qreal m_hue, qreal saturation);
            void setActive(bool);
            bool isHardwareAccelerated() const;

        private:
            ComPointer<IVMRWindowlessControl9> m_windowlessControl;
        };
    }
}

QT_END_NAMESPACE

#endif
