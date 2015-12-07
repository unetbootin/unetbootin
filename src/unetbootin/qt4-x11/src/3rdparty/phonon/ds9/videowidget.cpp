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

#include "videowidget.h"

#include <QtGui/QPainter>
#include <QtGui/QPaintEvent>
#include <QtCore/QTimer>
#include <QtCore/QSettings>

#include "mediaobject.h"

#include "videorenderer_vmr9.h"
#include "videorenderer_soft.h"

QT_BEGIN_NAMESPACE

#ifndef QT_NO_PHONON_VIDEO

namespace Phonon
{
    namespace DS9
    {
        //class used internally to return the widget where the video is shown on
        class VideoWindow : public QWidget
        {
        public:
            explicit VideoWindow(QWidget *parent, VideoWidget *vw)
                : QWidget(parent), m_node(vw), m_currentRenderer(0)
            {
                //default background color
                setPalette(QPalette(Qt::black));                 
                setAttribute(Qt::WA_OpaquePaintEvent, true);
                setAttribute(Qt::WA_NoSystemBackground, true);
                setAttribute(Qt::WA_PaintOnScreen, true);
                setAutoFillBackground(false);
            }

            QPaintEngine* paintEngine() const
            {
                return 0;
            }

            bool isEmbedded() const
            {
#if QT_VERSION >= 0x040400
                return window()->testAttribute(Qt::WA_DontShowOnScreen);
#else
                return false;
#endif
            }

            bool needsSoftRendering() const
            {
                QPaintDevice *dev = QPainter::redirected(this, 0);
                return (dev && dev != this);
            }

            void resizeEvent(QResizeEvent *e)
            {
                m_node->updateVideoSize();
                QWidget::resizeEvent(e);
            }

            AbstractVideoRenderer *currentRenderer() const
            {
                return m_currentRenderer;
            }

            void setCurrentRenderer(AbstractVideoRenderer *renderer)
            {
                m_currentRenderer = renderer;
                update();
            }

            QSize sizeHint() const
            {
                return m_currentRenderer->sizeHint().expandedTo(QWidget::sizeHint());
            }

            void changeEvent(QEvent *e)
            {
                checkCurrentRenderingMode();
                QWidget::changeEvent(e);
            }

            void setVisible(bool visible)
            {
                checkCurrentRenderingMode();
                QWidget::setVisible(visible);
            }

            void paintEvent(QPaintEvent *e)
            {
                checkCurrentRenderingMode();
                m_currentRenderer->repaintCurrentFrame(this, e->rect());
            }

            //this code manages the activation/deactivation of the screensaver
            /*bool event(QEvent *e)
            {
                if (e->type() == QEvent::Resize) {
                    //we disable the screensaver if the video is in fullscreen mode
                    disableScreenSaver(window()->windowState() & Qt::WindowFullScreen);
                }
                return QWidget::event(e);
            }*/

        private:
            //for fullscreen mode
            void disableScreenSaver(bool b)
            {
                const QLatin1String screenSaverActive("ScreenSaveActive");
                QSettings settings( QLatin1String("HKEY_CURRENT_USER\\Control Panel\\Desktop"), QSettings::NativeFormat);
                if (b) {
                    if (m_restoreScreenSaverActive.isNull()) {
                        //we store the value to be able to restore it later
                        m_restoreScreenSaverActive = settings.value(screenSaverActive);
                        settings.setValue(screenSaverActive, QString::number(!b));
                    }
                } else if (!m_restoreScreenSaverActive.isNull()) {
                    //we restore the previous value
                    settings.setValue(screenSaverActive, m_restoreScreenSaverActive);
                }
            }

            void checkCurrentRenderingMode()
            {
                if (!m_currentRenderer)
                    return;

                if (m_currentRenderer->isNative()) {
                    if (isEmbedded()) {
                        //we need to switch to software renderer
                        m_currentRenderer = m_node->switchRendering(m_currentRenderer);
                        setAttribute(Qt::WA_PaintOnScreen, false);
                    } else if (needsSoftRendering()) {
                        m_node->performSoftRendering(m_currentRenderer->snapshot());
                    }
                } else if (!isEmbedded()) {
                    m_currentRenderer = m_node->switchRendering(m_currentRenderer);
                    setAttribute(Qt::WA_PaintOnScreen, true);
                }
            }

            VideoWidget *m_node;
            AbstractVideoRenderer *m_currentRenderer;
            QVariant m_restoreScreenSaverActive;
        };

        VideoWidget::VideoWidget(QWidget *parent)
            : BackendNode(parent), m_aspectRatio(Phonon::VideoWidget::AspectRatioAuto),
              m_scaleMode(Phonon::VideoWidget::FitInView),
              m_brightness(0.), m_contrast(0.), m_hue(0.), m_saturation(0.), m_noNativeRendererSupported(false)
              
        {
            //initialisation of the widget
            m_widget = new VideoWindow(parent, this);

            //initialization of the renderers
            qMemSet(m_renderers, 0, sizeof(m_renderers));

            for(int i = 0; i< FILTER_COUNT ;++i) {
                //This might return a non native (ie Qt) renderer in case native is not supported
                AbstractVideoRenderer *renderer = getRenderer(i, Native, true);
                m_filters[i] = renderer->getFilter();
            }

            //by default, we take the first VideoWindow object
            setCurrentGraph(0);
        }

        VideoWidget::~VideoWidget()
        {
            for (int i = 0; i < 4; ++i) {
                delete m_renderers[i];
            }
        }

        void VideoWidget::notifyVideoLoaded()
        {
            updateVideoSize();
            m_widget->updateGeometry();
        }

        AbstractVideoRenderer *VideoWidget::switchRendering(AbstractVideoRenderer *current)
        {
            const bool toNative = !current->isNative();
            if (toNative && m_noNativeRendererSupported)
                return current; //no switch here

            //firt we delete the renderer
            //initialization of the widgets
            for(int i = 0; i < FILTER_COUNT; ++i) {
                Filter oldFilter = m_filters[i];

                //Let's create a software renderer
                AbstractVideoRenderer *renderer = getRenderer(i, toNative ? Native : NonNative, true);

                if (m_mediaObject) {
                    m_mediaObject->switchFilters(i, oldFilter, renderer->getFilter());
                }

                m_filters[i] = renderer->getFilter();
            }

            return getRenderer(mediaObject()->currentGraph()->index(), toNative ? Native: NonNative);
        }

        void VideoWidget::performSoftRendering(const QImage &currentImage)
        {
            const int graphIndex = mediaObject()->currentGraph()->index();
            VideoRendererSoft *r = static_cast<VideoRendererSoft*>(getRenderer(graphIndex, NonNative, true /*autocreation*/));
            r->setSnapshot(currentImage);
            r->notifyResize(m_widget->size(), m_aspectRatio, m_scaleMode);
            r->repaintCurrentFrame(m_widget, m_widget->rect());

        }

        void VideoWidget::setCurrentGraph(int index)
        {
            for(int i = 0; i < 2; ++i) {
                if (AbstractVideoRenderer *renderer = getRenderer(i, Native))
                    renderer->setActive(index == i);
            }

            //be sure to update all the things that needs an update
            applyMixerSettings();
            updateVideoSize();

            AbstractVideoRenderer *r = m_widget->currentRenderer();

            //we determine dynamically if it is native or non native
            r = getRenderer(index, !r || r->isNative() ? Native : NonNative);
			if (!r)
				r = getRenderer(index, NonNative);
            m_widget->setCurrentRenderer(r);
        }


        Phonon::VideoWidget::AspectRatio VideoWidget::aspectRatio() const
        {
            return m_aspectRatio;
        }

        void VideoWidget::setAspectRatio(Phonon::VideoWidget::AspectRatio aspectRatio)
        {
            m_aspectRatio = aspectRatio;
            updateVideoSize();
        }

        Phonon::VideoWidget::ScaleMode VideoWidget::scaleMode() const
        {
            return m_scaleMode;
        }


        QWidget *VideoWidget::widget()
        {
            return m_widget;
        }


        void VideoWidget::setScaleMode(Phonon::VideoWidget::ScaleMode scaleMode)
        {
            m_scaleMode = scaleMode;
            updateVideoSize();
        }

        void VideoWidget::setBrightness(qreal b)
        {
            m_brightness = b;
            applyMixerSettings();
        }

        void VideoWidget::setContrast(qreal c)
        {
            m_contrast = c;
            applyMixerSettings();
        }

        void VideoWidget::setHue(qreal h)
        {
            m_hue = h;
            applyMixerSettings();
        }

        void VideoWidget::setSaturation(qreal s)
        {
            m_saturation = s;
            applyMixerSettings();
        }

        qreal VideoWidget::brightness() const
        {
            return m_brightness;
        }


        qreal VideoWidget::contrast() const
        {
            return m_contrast;
        }

        qreal VideoWidget::hue() const
        {
            return m_hue;
        }

        qreal VideoWidget::saturation() const
        {
            return m_saturation;
        }


        AbstractVideoRenderer *VideoWidget::getRenderer(int graphIndex, RendererType type, bool autoCreate)
        {
            int index = graphIndex * 2 + type;
            if (m_renderers[index] == 0 && autoCreate) {
                AbstractVideoRenderer *renderer = 0;
				if (type == Native) {
                    renderer = new VideoRendererVMR9(m_widget);
                    if (renderer->getFilter() == 0) {
                        //instanciating the renderer might fail with error VFW_E_DDRAW_CAPS_NOT_SUITABLE (0x80040273)
                        m_noNativeRendererSupported = true;
                        delete renderer;
                        renderer = 0;
                    }
                }

                if (renderer == 0) {
                    type = NonNative;
                    index = graphIndex * 2 + type;
                    if (m_renderers[index] == 0)
                        renderer = new VideoRendererSoft(m_widget); //this always succeeds
                    else
                        renderer = m_renderers[index];
                }

                m_renderers[index] = renderer;

                //be sure to update all the things that needs an update
                applyMixerSettings();
                updateVideoSize();

            }
            return m_renderers[index];
        }

        //this must be called whe nthe node is actually connected
        void  VideoWidget::applyMixerSettings() const
        {
            for (int i = 0; i < 4; ++i) {
                if (AbstractVideoRenderer *renderer = m_renderers[i])
                    renderer->applyMixerSettings(m_brightness, m_contrast, m_hue, m_saturation);
            }
        }

        void VideoWidget::connected(BackendNode *, const InputPin&)
        {
            //in case of a connection, we simply reapply the mixer settings
            applyMixerSettings();
            updateVideoSize();
        }

        void VideoWidget::updateVideoSize() const
        {
            for (int i = 0; i < 4; ++i) {
                if (AbstractVideoRenderer *renderer = m_renderers[i])
                    renderer->notifyResize(m_widget->size(), m_aspectRatio, m_scaleMode);
            }
        }



    }
}

#endif //QT_NO_PHONON_VIDEO

QT_END_NAMESPACE

#include "moc_videowidget.cpp"
