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

#ifndef PHONON_VIDEOWIDGET_H
#define PHONON_VIDEOWIDGET_H

#include <QtGui/QWidget>
#include <phonon/videowidgetinterface.h>
#include "backendnode.h"

QT_BEGIN_NAMESPACE

#ifndef QT_NO_PHONON_VIDEO

namespace Phonon
{
    namespace DS9
    {
        class VideoWindow;
        class AbstractVideoRenderer;

        class VideoWidget : public BackendNode, public Phonon::VideoWidgetInterface
        {
            enum RendererType
            {
                Native = 0,
                NonNative = 1
            };

            Q_OBJECT
                Q_INTERFACES(Phonon::VideoWidgetInterface)
        public:
            VideoWidget(QWidget *parent = 0);
            ~VideoWidget();

            Phonon::VideoWidget::AspectRatio aspectRatio() const;
            void setAspectRatio(Phonon::VideoWidget::AspectRatio aspectRatio);
            Phonon::VideoWidget::ScaleMode scaleMode() const;
            void setScaleMode(Phonon::VideoWidget::ScaleMode);
            qreal brightness() const;
            void setBrightness(qreal);
            qreal contrast() const;
            void setContrast(qreal);
            qreal hue() const;
            void setHue(qreal);
            qreal saturation() const;
            void setSaturation(qreal);

            void setCurrentGraph(int index);

            QWidget *widget();

            void notifyVideoLoaded();
            AbstractVideoRenderer *switchRendering(AbstractVideoRenderer *current);
            void performSoftRendering(const QImage &currentImage);

            //apply contrast/brightness/hue/saturation
            void applyMixerSettings() const;
            void updateVideoSize() const;

        protected:
            virtual void connected(BackendNode *, const InputPin& inpin);

        private:
            AbstractVideoRenderer *getRenderer(int graphIndex, RendererType type, bool autoCreate = false);

            Phonon::VideoWidget::AspectRatio m_aspectRatio;
            Phonon::VideoWidget::ScaleMode m_scaleMode;

            VideoWindow *m_widget;
            qreal m_brightness, m_contrast, m_hue, m_saturation;
            AbstractVideoRenderer* m_renderers[4];
            mutable bool m_noNativeRendererSupported;
        };
    }
}

QT_END_NAMESPACE

#endif // PHONON_VIDEOWIDGET_H

#endif //QT_NO_PHONON_VIDEO
