/*  This file is part of the KDE project
    Copyright (C) 2007 Trolltech ASA. <thierry.bastian@trolltech.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2 as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.

*/

#ifndef PHONON_VIDEOWIDGETINTERFACE_H
#define PHONON_VIDEOWIDGETINTERFACE_H

#include "phonon/videowidget.h"

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

namespace Phonon
{
class VideoWidgetInterface
{
    public:
        virtual ~VideoWidgetInterface() {}

        virtual Phonon::VideoWidget::AspectRatio aspectRatio() const = 0;
        virtual void setAspectRatio(Phonon::VideoWidget::AspectRatio) = 0;
        virtual qreal brightness() const = 0;
        virtual void setBrightness(qreal) = 0;
        virtual Phonon::VideoWidget::ScaleMode scaleMode() const = 0;
        virtual void setScaleMode(Phonon::VideoWidget::ScaleMode) = 0;
        virtual qreal contrast() const = 0;
        virtual void setContrast(qreal) = 0;
        virtual qreal hue() const = 0;
        virtual void setHue(qreal) = 0;
        virtual qreal saturation() const = 0;
        virtual void setSaturation(qreal) = 0;
        virtual QWidget *widget() = 0;
//X        virtual int overlayCapabilities() const = 0;
//X        virtual bool createOverlay(QWidget *widget, int type) = 0;
};
}

Q_DECLARE_INTERFACE(Phonon::VideoWidgetInterface, "VideoWidgetInterface3.phonon.kde.org")

QT_END_NAMESPACE
QT_END_HEADER

#endif // PHONON_VIDEOWIDGETINTERFACE_H
