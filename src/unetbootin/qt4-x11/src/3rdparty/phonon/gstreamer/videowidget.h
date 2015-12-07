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

#ifndef Phonon_GSTREAMER_VIDEOWIDGET_H
#define Phonon_GSTREAMER_VIDEOWIDGET_H

#include <phonon/videowidget.h>
#include <phonon/videowidgetinterface.h>

#include "backend.h"
#include "common.h"
#include "medianode.h"
#include "abstractrenderer.h"
#include "videowidget.h"

#include <gst/gst.h>

QT_BEGIN_NAMESPACE

class QString;

namespace Phonon
{
namespace Gstreamer
{

class VideoWidget : public QWidget, public Phonon::VideoWidgetInterface, public MediaNode
{
    Q_OBJECT
    Q_INTERFACES(Phonon::VideoWidgetInterface Phonon::Gstreamer::MediaNode)
public:
    VideoWidget(Backend *backend, QWidget *parent = 0);
    ~VideoWidget();

    void setupVideoBin();
    void paintEvent(QPaintEvent *event);
    void mediaNodeEvent(const MediaNodeEvent *event);
    void setVisible(bool);

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
    void setMovieSize(const QSize &size);
    QSize sizeHint() const;
    QRect scaleToAspect(QRect srcRect, int w, int h) const;
    QRect calculateDrawFrameRect() const;

    GstElement *videoElement()
    {
        Q_ASSERT(m_videoBin);
        return m_videoBin;
    }

    QSize movieSize() const {
        return m_movieSize;
    }

    bool event(QEvent *);

    QWidget *widget() {
        return this;
    }

protected:
    GstElement *m_videoBin;
    QSize m_movieSize;
    AbstractRenderer *m_renderer;

private:
    Phonon::VideoWidget::AspectRatio m_aspectRatio;
    qreal m_brightness, m_hue, m_contrast, m_saturation;
    Phonon::VideoWidget::ScaleMode m_scaleMode;

    GstElement *m_videoBalance;
    GstElement *m_colorspace;
    GstElement *m_videoplug;
};

}
} //namespace Phonon::Gstreamer

QT_END_NAMESPACE

#endif // Phonon_GSTREAMER_VIDEOWIDGET_H
