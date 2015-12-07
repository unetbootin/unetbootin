/*  This file is part of the KDE project.

    Copyright (C) 2    //Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).007 Nokia Corporation and/or its subsidiary(-ies).

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

#ifndef Phonon_GSTREAMER_ABSTRACTRENDERER_H
#define Phonon_GSTREAMER_ABSTRACTRENDERER_H

#include "backend.h"
#include "common.h"
#include "medianode.h"
#include <phonon/videowidget.h>

QT_BEGIN_NAMESPACE

class QString;
namespace Phonon
{
namespace Gstreamer
{

class VideoWidget;

class AbstractRenderer
{
public:
    AbstractRenderer(VideoWidget *video) :
          m_videoWidget(video)
        , m_videoSink(0) { }
    virtual ~AbstractRenderer();
    virtual GstElement *videoSink() {return m_videoSink;}
    virtual void aspectRatioChanged(Phonon::VideoWidget::AspectRatio aspectRatio);
    virtual void scaleModeChanged(Phonon::VideoWidget::ScaleMode scaleMode);
    virtual void handleMediaNodeEvent(const MediaNodeEvent *event) = 0;
    virtual bool eventFilter(QEvent *) = 0;
    virtual void handlePaint(QPaintEvent *) {}
    virtual bool paintsOnWidget() { return true; } // Controls overlays

protected:
    VideoWidget *m_videoWidget;
    GstElement *m_videoSink;
};

}
} //namespace Phonon::Gstreamer

QT_END_NAMESPACE

#endif // Phonon_GSTREAMER_ABSTRACTRENDERER_H
