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

#ifndef Phonon_GSTREAMER_WIDGETRENDERER_H
#define Phonon_GSTREAMER_WIDGETRENDERER_H

#include "videowidget.h"
#include "common.h"

#ifndef QT_NO_OPENGL
#include <QtOpenGL/QGLFormat>
#include <QtOpenGL/QGLWidget>
#endif

QT_BEGIN_NAMESPACE

class QString;

namespace Phonon
{
namespace Gstreamer
{

class WidgetRenderer : public AbstractRenderer
{
public:
    WidgetRenderer(VideoWidget *videoWidget);
    bool eventFilter(QEvent * event);
    void handlePaint(QPaintEvent *paintEvent);
    void handleMediaNodeEvent(const MediaNodeEvent *event);
    const QImage& currentFrame() const;
    QRect drawFrameRect() const { return m_drawFrameRect; }
    void setNextFrame(const QByteArray &array, int width, int height);
    bool frameIsSet() { return !m_array.isNull(); }
    void clearFrame();
private:
    mutable QImage m_frame;
    QByteArray m_array;
    int m_width;
    int m_height;
    QRect m_drawFrameRect;
};

}
} //namespace Phonon::Gstreamer

QT_END_NAMESPACE

#endif // Phonon_GSTREAMER_WIDGETRENDERER_H
