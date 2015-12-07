/*  This file is part of the KDE project
    Copyright (C) 2005-2007 Matthias Kretz <kretz@kde.org>

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
#ifndef Phonon_VIDEOWIDGET_H
#define Phonon_VIDEOWIDGET_H

#include "phonon_export.h"
#include "phonondefs.h"
#include "abstractvideooutput.h"
#include <QtGui/QWidget>

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

class QString;

#ifndef QT_NO_PHONON_VIDEO

namespace Phonon
{
class AbstractVideoOutput;
    class VideoWidgetPrivate;
    /** \class VideoWidget videowidget.h Phonon/VideoWidget
     * \short Widget to display video.
     *
     * This widget shows the video signal.
     *
     * \code
     * MediaObject *media = new MediaObject(parent);
     * VideoWidget *vwidget = new VideoWidget(parent);
     * Phonon::createPath(media, vwidget);
     * \endcode
     *
     * \ingroup PhononVideo
     * \ingroup PhononWidgets
     * \author Matthias Kretz <kretz@kde.org>
     */
    class PHONON_EXPORT VideoWidget : public QWidget, public Phonon::AbstractVideoOutput
    {
        K_DECLARE_PRIVATE(VideoWidget)
        Q_OBJECT
        Q_ENUMS(AspectRatio ScaleMode)
        /**
         * This property holds whether the video is shown using the complete
         * screen.
         *
         * The property differs from QWidget::fullScreen in that it is
         * writeable.
         *
         * By default the widget is not shown in fullScreen.
         *
         * \warning When switching the video to fullscreen using setFullScreen
         * your application loses control over the widget that actually shows
         * the video (which is then shown as a toplevel window while your
         * application still uses this widget). If you only need to capture key
         * events the event forwarding done internally should suffice for your
         * needs. If you need to map mouse coordinates or add widgets (that are
         * not overlays) you should probably handle fullscreen yourself.
         */
        Q_PROPERTY(bool fullScreen READ isFullScreen WRITE setFullScreen)
        /**
         *
         * Defaults to AspectRatioAuto.
         *
         * \see AspectRatio
         */
        Q_PROPERTY(AspectRatio aspectRatio READ aspectRatio WRITE setAspectRatio)

        /**
         * If the size of the widget and the size of the video are not equal.
         * The video will be zoomed to fit the widget. The smaller zoom
         * (AddBarsScaleMode) adds black bars at the left/right or top/bottom to
         * make all of the image visible (default). The bigger zoom (ExpandMode)
         * fills the widget completely, keeping all information in one direction
         * and leaving parts of the image outside of the widget in the other
         * direction.
         */
        Q_PROPERTY(ScaleMode scaleMode READ scaleMode WRITE setScaleMode)

        /**
         * This property holds brightness of the video.
         *
         * Default is 0. Acceptable values are in range of -1, 1.
         */
        Q_PROPERTY(qreal brightness READ brightness WRITE setBrightness)
        /**
         * This property holds the contrast of the video.
         *
         * Default is 0. Acceptable values are in range of -1, 1.
         */
        Q_PROPERTY(qreal contrast READ contrast WRITE setContrast)
        /**
         * This property holds the hue of the video.
         *
         * Default is 0. Acceptable values are in range of -1, 1.
         */
        Q_PROPERTY(qreal hue READ hue WRITE setHue)
        /**
         * This property holds saturation of the video.
         *
         * Default is 0. Acceptable values are in range of -1, 1.
         */
        Q_PROPERTY(qreal saturation READ saturation WRITE setSaturation)

        public:
            /**
             * Defines the width:height to be used for the video.
             */
            enum AspectRatio
            {
                /**
                 * Let the decoder find the aspect ratio automatically from the
                 * media file (this is the default).
                 */
                AspectRatioAuto = 0,
                /**
                 * Fits the video into the widget making the aspect ratio depend
                 * solely on the size of the widget. This way the aspect ratio
                 * is freely resizeable by the user.
                 */
                AspectRatioWidget = 1,
                /**
                 * Make width/height == 4/3, which is the old TV size and
                 * monitor size (1024/768 == 4/3). (4:3)
                 */
                AspectRatio4_3 = 2,
                /**
                 * Make width/height == 16/9, which is the size of most current
                 * media. (16:9)
                 */
                AspectRatio16_9 = 3
//X                 /**
//X                  * Assume that every pixel of the video image needs to be displayed with the same
//X                  * physical width and height. (1:1 image pixels, not imagewidth
//X                  * = imageheight)
//X                  */
//X                 AspectRatioSquare = 4,
            };

            enum ScaleMode {
                FitInView = 0,
                ScaleAndCrop = 1
            };

            /**
             * Constructs a new video widget with a \p parent.
             */
            VideoWidget(QWidget *parent = 0);

            AspectRatio aspectRatio() const;
            ScaleMode scaleMode() const;

            qreal brightness() const;
            qreal contrast() const;
            qreal hue() const;
            qreal saturation() const;

            //TODO: bar colors property
        public Q_SLOTS:
            void setFullScreen(bool fullscreen);

            /**
             * Convenience slot, calling setFullScreen(false)
             */
            void exitFullScreen();

            /**
             * Convenience slot, calling setFullScreen(true)
             */
            void enterFullScreen();

            void setAspectRatio(AspectRatio);
            void setScaleMode(ScaleMode);

            void setBrightness(qreal value);
            void setContrast(qreal value);
            void setHue(qreal value);
            void setSaturation(qreal value);

        protected:
            /**
             * \internal
             *
             * Constructs a new video widget with private data pointer \p d and
             * a \p parent.
             */
            VideoWidget(VideoWidgetPrivate &d, QWidget *parent);

            void mouseMoveEvent(QMouseEvent *);
            bool event(QEvent *);
    };

} //namespace Phonon

#endif //QT_NO_PHONON_VIDEO

QT_END_NAMESPACE
QT_END_HEADER

// vim: sw=4 ts=4 tw=80
#endif // Phonon_VIDEOWIDGET_H
