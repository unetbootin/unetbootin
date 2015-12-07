/*  This file is part of the KDE project
    Copyright (C) 2006-2007 Matthias Kretz <kretz@kde.org>

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

#ifndef VIDEOWIDGET_P_H
#define VIDEOWIDGET_P_H

#include "videowidget.h"
#include "abstractvideooutput_p.h"
#include <QtGui/QBoxLayout>
#include <QtCore/QEvent>
#include <QtCore/QCoreApplication>
#include <QtGui/QPalette>
#include <QtGui/QKeyEvent>
#include <QtCore/QTimer>

QT_BEGIN_NAMESPACE

#ifndef QT_NO_PHONON_VIDEO

namespace Phonon
{

class VideoWidgetPrivate : public Phonon::AbstractVideoOutputPrivate
{
    Q_DECLARE_PUBLIC(VideoWidget)
    public:
        virtual QObject *qObject() { return q_func(); }
    protected:
        virtual bool aboutToDeleteBackendObject();
        virtual void createBackendObject();
        void setupBackendObject();

        VideoWidgetPrivate(VideoWidget *parent)
            : layout(parent),
              aspectRatio(VideoWidget::AspectRatioAuto),
              scaleMode(VideoWidget::FitInView),
              brightness(0),
              contrast(0),
              hue(0),
              saturation(0)
        {
            layout.setMargin(0);
        }

        QHBoxLayout layout;
        VideoWidget::AspectRatio aspectRatio;
        VideoWidget::ScaleMode scaleMode;
        Qt::WindowFlags changeFlags;

        qreal brightness;
        qreal contrast;
        qreal hue;
        qreal saturation;

    private:
        void init();
};

} // namespace Phonon

#endif //QT_NO_PHONON_VIDEO

QT_END_NAMESPACE
#endif // VIDEOWIDGET_P_H
// vim: sw=4 ts=4 tw=80
