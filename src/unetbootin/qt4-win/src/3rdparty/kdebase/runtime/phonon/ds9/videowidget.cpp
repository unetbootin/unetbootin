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

#include "videowidget.h"

#include <QtGui/QPaintEvent>
#include <QtCore/QTimer>
#include <QtCore/QSettings>

#include "mediaobject.h"

#ifndef Q_OS_WINCE
#include "videorenderer_vmr9.h"
#endif

#include "videorenderer_soft.h"

QT_BEGIN_NAMESPACE

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

            void setVisible(bool visible)
            {
                if (isEmbedded() && m_currentRenderer->isHardwareAccelerated()) {
                    //then we should switch the current renderer
                    m_currentRenderer = m_node->ensureSoftwareRendering(m_currentRenderer);
                }
                QWidget::setVisible(visible);
            }

            bool isEmbedded() const
            {
#if QT_VERSION >= 0x040400
                return window()->testAttribute(Qt::WA_DontShowOnScreen);
#else
                return false;
#endif
            }

            void resizeEvent(QResizeEvent *e)
            {
                updateVideoSize();
                QWidget::resizeEvent(e);
            }

            void notifyVideoLoaded()
            {
                m_currentRenderer->notifyVideoLoaded();
                updateGeometry();
                updateVideoSize();
                update();
            }

            void updateVideoSize()
            {
                m_currentRenderer->notifyResize(rect(), m_node->aspectRatio(), m_node->scaleMode());
                update();
            }

            void setCurrentRenderer(AbstractVideoRenderer *renderer)
            {
                m_currentRenderer = renderer;
                updateVideoSize();
                update();
            }

            QSize sizeHint() const
            {
                return m_currentRenderer->sizeHint().expandedTo(QWidget::sizeHint());
            }

            void paintEvent(QPaintEvent *e)
            {
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

            VideoWidget *m_node;
            AbstractVideoRenderer *m_currentRenderer;
            QVariant m_restoreScreenSaverActive;


        };

        VideoWidget::VideoWidget(QWidget *parent)
            : BackendNode(parent), m_brightness(0.), m_contrast(0.), m_hue(0.), m_saturation(0.),
            m_aspectRatio(Phonon::VideoWidget::AspectRatioAuto), m_scaleMode(Phonon::VideoWidget::FitInView)
        {
            //initialisation of the widget
            m_widget = new VideoWindow(parent, this);

            //initialization of the widgets
            for(QVector<Filter>::iterator it = m_filters.begin(); it != m_filters.end(); ++it) {
                AbstractVideoRenderer *renderer = getDefaultRenderer();                
                m_renderers.append(renderer);
                *it = renderer->getFilter();
            }

            //by default, we take the first VideoWindow object
            setCurrentGraph(0);
        }

        VideoWidget::~VideoWidget()
        {
            qDeleteAll(m_renderers);
        }

        AbstractVideoRenderer *VideoWidget::getDefaultRenderer() const
        {
#ifdef Q_OS_WINCE
			return new VideoRendererSoft(m_widget);
#else
            //First we try the VMR9
            AbstractVideoRenderer *ret = new VideoRendererVMR9(m_widget);
            if (ret->getFilter() == 0) {
                delete ret;
                //if it fails, we create the software renderer
                ret = new VideoRendererSoft(m_widget); 
            }
            return ret;
#endif
        }

        void VideoWidget::notifyVideoLoaded()
        {
            m_widget->notifyVideoLoaded();
        }


        AbstractVideoRenderer *VideoWidget::ensureSoftwareRendering(AbstractVideoRenderer *renderer)
        {
            const int index = m_renderers.indexOf(renderer);

            //we can safely delete the renderer classes
            qDeleteAll(m_renderers);
            m_renderers.clear();

            //firt we delete the renderer
            //initialization of the widgets
            for(int i = 0; i < m_filters.count(); ++i) {
                Filter oldFilter = m_filters.at(i);

                //Let's create a software renderer
                AbstractVideoRenderer *renderer = new VideoRendererSoft(m_widget);
                m_renderers.append(renderer);

                if (m_mediaObject) {
                    m_mediaObject->switchFilters(i, oldFilter, renderer->getFilter());
                }

                m_filters[i] = renderer->getFilter();
            }

            //we return the new renderer
            return m_renderers.at(index);
        }



        void VideoWidget::setCurrentGraph(int index)
        {
            for(int i = 0; i < m_renderers.count(); ++i) {
                m_renderers.at(i)->setActive(index == i);
            }
            m_widget->setCurrentRenderer(m_renderers.at(index));
        }


        Phonon::VideoWidget::AspectRatio VideoWidget::aspectRatio() const
        {
            return m_aspectRatio;
        }

        void VideoWidget::setAspectRatio(Phonon::VideoWidget::AspectRatio aspectRatio)
        {
            m_aspectRatio = aspectRatio;
            m_widget->updateVideoSize();
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
            m_widget->updateVideoSize();
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


        //this must be called whe nthe node is actually connected
        void  VideoWidget::applyMixerSettings() const
        {
            foreach(AbstractVideoRenderer *renderer, m_renderers) {
                renderer->applyMixerSettings(m_brightness, m_contrast, m_hue, m_saturation);
            }
        }

        void VideoWidget::connected(BackendNode *, const InputPin&)
        {
            //in case of a connection, we simply reapply the mixer settings
            applyMixerSettings();
            m_widget->updateVideoSize();
        }

    }
}

QT_END_NAMESPACE

#include "moc_videowidget.cpp"
