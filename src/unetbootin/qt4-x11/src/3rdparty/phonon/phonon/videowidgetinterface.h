/*  This file is part of the KDE project
    Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies). <thierry.bastian@trolltech.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), Trolltech ASA 
    (or its successors, if any) and the KDE Free Qt Foundation, which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public 
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef PHONON_VIDEOWIDGETINTERFACE_H
#define PHONON_VIDEOWIDGETINTERFACE_H

#include "videowidget.h"

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

#ifndef QT_NO_PHONON_VIDEO

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

#endif //QT_NO_PHONON_VIDEO

QT_END_NAMESPACE
QT_END_HEADER

#endif // PHONON_VIDEOWIDGETINTERFACE_H
