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

#ifndef Phonon_GSTREAMER_X11RENDERER_H
#define Phonon_GSTREAMER_X11RENDERER_H

#include "videowidget.h"
#include "common.h"

#include <QtGui/QWidget>

#include <gst/gst.h>

QT_BEGIN_NAMESPACE

#ifndef Q_WS_QWS

class QString;

namespace Phonon
{
namespace Gstreamer
{

class OverlayWidget;
class X11Renderer : public AbstractRenderer
{
public:
    X11Renderer(VideoWidget *videoWidget);
    ~X11Renderer();
    void handlePaint(QPaintEvent *event);
    void aspectRatioChanged(Phonon::VideoWidget::AspectRatio aspectRatio);
    void scaleModeChanged(Phonon::VideoWidget::ScaleMode scaleMode);
    void handleMediaNodeEvent(const MediaNodeEvent *event);
    bool eventFilter(QEvent *);
    bool paintsOnWidget() { return false; }
    bool overlaySet() const { return m_overlaySet; }
    void setOverlay();
    void windowExposed();
    GstElement *createVideoSink();
private:
    OverlayWidget *m_renderWidget;
    bool m_overlaySet;
};

}
} //namespace Phonon::Gstreamer

#endif // Q_WS_QWS

QT_END_NAMESPACE

#endif // Phonon_GSTREAMER_X11RENDERER_H
