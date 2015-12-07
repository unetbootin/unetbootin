/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <qglobal.h>

#include <QDebug>

#include "qpainter.h"
#include "qpixmap.h"
#include "qpixmapfilter_p.h"
#include "qvarlengtharray.h"

#include "private/qapplication_p.h"
#include "private/qgraphicssystem_p.h"
#include "private/qpaintengineex_p.h"
#include "private/qpaintengine_raster_p.h"

QT_BEGIN_NAMESPACE

class QPixmapFilterPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QPixmapFilter)
public:
    QPixmapFilter::FilterType type;
};

/*!
    \class QPixmapFilter
    \since 4.5
    \ingroup multimedia

    \brief The QPixmapFilter class provides the basic functionality for
    pixmap filter classes. Pixmap filter can be for example colorize or blur.

    QPixmapFilter is the base class for every pixmap filter. QPixmapFilter is
    an abstract class and cannot itself be instantiated. It provides a standard
    interface for filter processing.

    \internal
*/

/*!
    \enum QPixmapFilter::FilterType

    \internal

    This enum describes the types of filter that can be applied to pixmaps.

    \value ConvolutionFilter  A filter that is used to calculate the convolution
                              of the image with a kernel. See
                              QPixmapConvolutionFilter for more information.
    \value ColorizeFilter     A filter that is used to change the overall color
                              of an image. See QPixmapColorizeFilter for more
                              information.
    \value DropShadowFilter   A filter that is used to add a drop shadow to an
                              image. See QPixmapDropShadowFilter for more
                              information.

    \value UserFilter   The first filter type that can be used for
                        application-specific purposes.
*/


/*!
    Constructs a default QPixmapFilter with the given \a type.

    This constructor should be used when subclassing QPixmapFilter to
    create custom user filters.

    \internal
*/
QPixmapFilter::QPixmapFilter(FilterType type, QObject *parent)
    : QObject(*new QPixmapFilterPrivate, parent)
{
    d_func()->type = type;
}



/*!
   \internal
*/
QPixmapFilter::QPixmapFilter(QPixmapFilterPrivate&d, QPixmapFilter::FilterType type, QObject *parent)
    : QObject(d, parent)
{
    d_func()->type = type;
}


/*!
    Destroys the pixmap filter.

    \internal
*/
QPixmapFilter::~QPixmapFilter()
{
}

/*!
    Returns the type of the filter. All standard pixmap filter classes
    are associated with a unique value.

    \internal
*/
QPixmapFilter::FilterType QPixmapFilter::type() const
{
    Q_D(const QPixmapFilter);
    return d->type;
}

/*!
    Returns the bounding rectangle that is affected by the pixmap
    filter if the filter is applied to the specified \a rect.

    \internal
*/
QRectF QPixmapFilter::boundingRectFor(const QRectF &rect) const
{
    return rect;
}

/*!
    \fn void QPixmapFilter::draw(QPainter *painter, const QPointF &p, const QPixmap &src, const QRectF& srcRect) const

    Uses \a painter to draw filtered result of \a src at the point
    specified by \a p. If \a srcRect is specified the it will
    be used as a source rectangle to only draw a part of the source.

    draw() will affect the area which boundingRectFor() returns.

    \internal
*/

/*!
    \class QPixmapConvolutionFilter
    \since 4.5
    \ingroup multimedia

    \brief The QPixmapConvolutionFilter class provides convolution
    filtering for pixmaps.

    QPixmapConvolutionFilter implements a convolution pixmap filter,
    which is applied when \l{QPixmapFilter::}{draw()} is called. A
    convolution filter lets you distort an image by setting the values
    of a matrix of qreal values called its
    \l{setConvolutionKernel()}{kernel}. The matrix's values are
    usually between -1.0 and 1.0.

    \omit
    In convolution filtering, the pixel value is calculated from the
    neighboring pixels based on the weighting convolution kernel.
    This needs explaining to be useful.
    \endomit

    Example:
    \snippet doc/src/snippets/code/src_gui_image_qpixmapfilter.cpp 1

    \sa {Pixmap Filters Example}, QPixmapColorizeFilter, QPixmapDropShadowFilter


    \internal
*/

class QPixmapConvolutionFilterPrivate : public QPixmapFilterPrivate
{
public:
    QPixmapConvolutionFilterPrivate(): convolutionKernel(0), kernelWidth(0), kernelHeight(0), convoluteAlpha(false) {}
    ~QPixmapConvolutionFilterPrivate() {
        delete[] convolutionKernel;
    }

    qreal *convolutionKernel;
    int kernelWidth;
    int kernelHeight;
    bool convoluteAlpha;
};


/*!
    Constructs a pixmap convolution filter.

    By default there is no convolution kernel.

    \internal
*/
QPixmapConvolutionFilter::QPixmapConvolutionFilter(QObject *parent)
    : QPixmapFilter(*new QPixmapConvolutionFilterPrivate, ConvolutionFilter, parent)
{
    Q_D(QPixmapConvolutionFilter);
    d->convoluteAlpha = true;
}

/*!
    Destructor of pixmap convolution filter.

    \internal
*/
QPixmapConvolutionFilter::~QPixmapConvolutionFilter()
{
}

/*!
     Sets convolution kernel with the given number of \a rows and \a columns.
     Values from \a kernel are copied to internal data structure.

     To preserve the intensity of the pixmap, the sum of all the
     values in the convolution kernel should add up to 1.0. A sum
     greater than 1.0 produces a lighter result and a sum less than 1.0
     produces a darker and transparent result.

    \internal
*/
void QPixmapConvolutionFilter::setConvolutionKernel(const qreal *kernel, int rows, int columns)
{
    Q_D(QPixmapConvolutionFilter);
    delete [] d->convolutionKernel;
    d->convolutionKernel = new qreal[rows * columns];
    memcpy(d->convolutionKernel, kernel, sizeof(qreal) * rows * columns);
    d->kernelWidth = columns;
    d->kernelHeight = rows;
}

/*!
    Gets the convolution kernel data.

    \internal
*/
const qreal *QPixmapConvolutionFilter::convolutionKernel() const
{
    Q_D(const QPixmapConvolutionFilter);
    return d->convolutionKernel;
}

/*!
    Gets the number of rows in the convolution kernel.

    \internal
*/
int QPixmapConvolutionFilter::rows() const
{
    Q_D(const QPixmapConvolutionFilter);
    return d->kernelHeight;
}

/*!
    Gets the number of columns in the convolution kernel.

    \internal
*/
int QPixmapConvolutionFilter::columns() const
{
    Q_D(const QPixmapConvolutionFilter);
    return d->kernelWidth;
}


/*!
    \reimp

    \internal
*/
QRectF QPixmapConvolutionFilter::boundingRectFor(const QRectF &rect) const
{
    Q_D(const QPixmapConvolutionFilter);
    return rect.adjusted(-d->kernelWidth / 2, -d->kernelHeight / 2, (d->kernelWidth - 1) / 2, (d->kernelHeight - 1) / 2);
}

// Convolutes the image
static void convolute(
        QImage *destImage,
        const QPointF &pos,
        const QImage &srcImage,
        const QRectF &srcRect,
        QPainter::CompositionMode mode,
        qreal *kernel,
        int kernelWidth,
        int kernelHeight )
{
    const QImage processImage = (srcImage.format() != QImage::Format_ARGB32_Premultiplied ) ?               srcImage.convertToFormat(QImage::Format_ARGB32_Premultiplied) : srcImage;
    // TODO: support also other formats directly without copying

    int *fixedKernel = new int[kernelWidth*kernelHeight];
    for(int i = 0; i < kernelWidth*kernelHeight; i++)
    {
        fixedKernel[i] = (int)(65536 * kernel[i]);
    }
    QRectF trect = srcRect.isNull() ? processImage.rect() : srcRect;
    trect.moveTo(pos);
    QRectF bounded = trect.adjusted(-kernelWidth / 2, -kernelHeight / 2, (kernelWidth - 1) / 2, (kernelHeight - 1) / 2);
    QRect rect = bounded.toAlignedRect();
    QRect targetRect = rect.intersected(destImage->rect());

    QRectF srect = srcRect.isNull() ? processImage.rect() : srcRect;
    QRectF sbounded = srect.adjusted(-kernelWidth / 2, -kernelHeight / 2, (kernelWidth - 1) / 2, (kernelHeight - 1) / 2);
    QPoint srcStartPoint = sbounded.toAlignedRect().topLeft()+(targetRect.topLeft()-rect.topLeft());

    const uint *sourceStart = (uint*)processImage.scanLine(0);
    uint *outputStart = (uint*)destImage->scanLine(0);

    int yk = srcStartPoint.y();
    for (int y = targetRect.top(); y <= targetRect.bottom(); y++) {
        uint* output = outputStart + (destImage->bytesPerLine()/sizeof(uint))*y+targetRect.left();
        int xk = srcStartPoint.x();
        for(int x = targetRect.left(); x <= targetRect.right(); x++) {
            int r = 0;
            int g = 0;
            int b = 0;
            int a = 0;

            // some out of bounds pre-checking to avoid inner-loop ifs
            int kernely = -kernelHeight/2;
            int starty = 0;
            int endy = kernelHeight;
            if(yk+kernely+endy >= srcImage.height())
                endy = kernelHeight-((yk+kernely+endy)-srcImage.height())-1;
            if(yk+kernely < 0)
                starty = -(yk+kernely);

            int kernelx = -kernelWidth/2;
            int startx = 0;
            int endx = kernelWidth;
            if(xk+kernelx+endx >= srcImage.width())
                endx = kernelWidth-((xk+kernelx+endx)-srcImage.width())-1;
            if(xk+kernelx < 0)
                startx = -(xk+kernelx);

            for (int ys = starty; ys < endy; ys ++) {
                const uint *pix = sourceStart + (processImage.bytesPerLine()/sizeof(uint))*(yk+kernely+ys) + ((xk+kernelx+startx));
                const uint *endPix = pix+endx-startx;
                int kernelPos = ys*kernelWidth+startx;
                while (pix < endPix) {
                    int factor = fixedKernel[kernelPos++];
                    a += (((*pix) & 0xff000000)>>24) * factor;
                    r += (((*pix) & 0x00ff0000)>>16) * factor;
                    g += (((*pix) & 0x0000ff00)>>8 ) * factor;
                    b += (((*pix) & 0x000000ff)    ) * factor;
                    pix++;
                }
            }

            r = qBound((int)0, r >> 16, (int)255);
            g = qBound((int)0, g >> 16, (int)255);
            b = qBound((int)0, b >> 16, (int)255);
            a = qBound((int)0, a >> 16, (int)255);
            // composition mode checking could be moved outside of loop
            if(mode == QPainter::CompositionMode_Source) {
                uint color = (a<<24)+(r<<16)+(g<<8)+b;
                *output++ = color;
            } else {
                uint current = *output;
                uchar ca = (current&0xff000000)>>24;
                uchar cr = (current&0x00ff0000)>>16;
                uchar cg = (current&0x0000ff00)>>8;
                uchar cb = (current&0x000000ff);
                uint color =
                        (((ca*(255-a) >> 8)+a) << 24)+
                        (((cr*(255-a) >> 8)+r) << 16)+
                        (((cg*(255-a) >> 8)+g) << 8)+
                        (((cb*(255-a) >> 8)+b));
                *output++ = color;;
            }
            xk++;
        }
        yk++;
    }
}

/*!
    \reimp

    \internal
*/
void QPixmapConvolutionFilter::draw(QPainter *painter, const QPointF &p, const QPixmap &src, const QRectF& srcRect) const
{
    Q_D(const QPixmapConvolutionFilter);
    if (!painter->isActive())
        return;

    if(d->kernelWidth<=0 || d->kernelHeight <= 0)
        return;

    QPixmapFilter *filter = painter->paintEngine() && painter->paintEngine()->isExtended() ?
        static_cast<QPaintEngineEx *>(painter->paintEngine())->createPixmapFilter(type()) : 0;
    QPixmapConvolutionFilter *convolutionFilter = static_cast<QPixmapConvolutionFilter*>(filter);
    if (convolutionFilter) {
        convolutionFilter->setConvolutionKernel(d->convolutionKernel, d->kernelWidth, d->kernelHeight);
        convolutionFilter->d_func()->convoluteAlpha = d->convoluteAlpha;
        convolutionFilter->draw(painter, p, src, srcRect);
        delete convolutionFilter;
        return;
    }

    // falling back to raster implementation

    QImage *target = 0;
    if (painter->paintEngine()->paintDevice()->devType() == QInternal::Image) {
        target = static_cast<QImage *>(painter->paintEngine()->paintDevice());

        QTransform mat = painter->combinedTransform();

        if (mat.type() > QTransform::TxTranslate) {
            // Disabled because of transformation...
            target = 0;
        } else {
            QRasterPaintEngine *pe = static_cast<QRasterPaintEngine *>(painter->paintEngine());
            if (pe->clipType() == QRasterPaintEngine::ComplexClip)
                // disabled because of complex clipping...
                target = 0;
            else {
                QRectF clip = pe->clipBoundingRect();
                QRectF rect = boundingRectFor(srcRect.isEmpty() ? src.rect() : srcRect);
                QTransform x = painter->deviceTransform();
                if (!clip.contains(rect.translated(x.dx() + p.x(), x.dy() + p.y()))) {
                    target = 0;
                }

            }
        }
    }

    if (target) {
        QTransform x = painter->deviceTransform();
        QPointF offset(x.dx(), x.dy());

        convolute(target, p+offset, src.toImage(), srcRect, QPainter::CompositionMode_SourceOver, d->convolutionKernel, d->kernelWidth, d->kernelHeight);
    } else {
        QRect srect = srcRect.isNull() ? src.rect() : srcRect.toRect();
        QRect rect = boundingRectFor(srect).toRect();
        QImage result = QImage(rect.size(), QImage::Format_ARGB32_Premultiplied);
        QPoint offset = srect.topLeft() - rect.topLeft();
        convolute(&result,
                  offset,
                  src.toImage(),
                  srect,
                  QPainter::CompositionMode_Source,
                  d->convolutionKernel,
                  d->kernelWidth,
                  d->kernelHeight);
        painter->drawImage(p - offset, result);
    }
}

// grayscales the image to dest (could be same). If rect isn't defined
// destination image size is used to determine the dimension of grayscaling
// process.
static void grayscale(const QImage &image, QImage &dest, const QRect& rect = QRect())
{
    QRect destRect = rect;
    QRect srcRect = rect;
    if (rect.isNull()) {
        srcRect = dest.rect();
        destRect = dest.rect();
    }
    if (image != dest) {
        destRect.moveTo(QPoint(0, 0));
    }

    unsigned int *data = (unsigned int *)image.bits();
    unsigned int *outData = (unsigned int *)dest.bits();

    if (dest.size() == image.size() && image.rect() == srcRect) {
        // a bit faster loop for grayscaling everything
        int pixels = dest.width() * dest.height();
        for (int i = 0; i < pixels; ++i) {
            int val = qGray(data[i]);
            outData[i] = qRgba(val, val, val, qAlpha(data[i]));
        }
    } else {
        int yd = destRect.top();
        for (int y = srcRect.top(); y <= srcRect.bottom() && y < image.height(); y++) {
            data = (unsigned int*)image.scanLine(y);
            outData = (unsigned int*)dest.scanLine(yd++);
            int xd = destRect.left();
            for (int x = srcRect.left(); x <= srcRect.right() && x < image.width(); x++) {
                int val = qGray(data[x]);
                outData[xd++] = qRgba(val, val, val, qAlpha(data[x]));
            }
        }
    }
}

/*!
    \class QPixmapColorizeFilter
    \since 4.5
    \ingroup multimedia

    \brief The QPixmapColorizeFilter class provides colorizing
    filtering for pixmaps.

    A colorize filter gives the pixmap a tint of its color(). The
    filter first grayscales the pixmap and then converts those to
    colorized values using QPainter::CompositionMode_Screen with the
    chosen color. The alpha-channel is not changed.

    Example:
    \snippet doc/src/snippets/code/src_gui_image_qpixmapfilter.cpp 0

    \sa QPainter::CompositionMode

    \internal
*/
class QPixmapColorizeFilterPrivate : public QPixmapFilterPrivate
{
    Q_DECLARE_PUBLIC(QPixmapColorizeFilter)
public:
    QColor color;
};

/*!
    Constructs an pixmap colorize filter.

    Default color value for colorizing is QColor(0, 0, 192).

    \internal
*/
QPixmapColorizeFilter::QPixmapColorizeFilter(QObject *parent)
    : QPixmapFilter(*new QPixmapColorizeFilterPrivate, ColorizeFilter, parent)
{
    d_func()->color = QColor(0, 0, 192);
}

/*!
    Gets the color of the colorize filter.

    \internal
*/
QColor QPixmapColorizeFilter::color() const
{
    Q_D(const QPixmapColorizeFilter);
    return d->color;
}

/*!
    Sets the color of the colorize filter to the \a color specified.

    \internal
*/
void QPixmapColorizeFilter::setColor(const QColor &color)
{
    Q_D(QPixmapColorizeFilter);
    d->color = color;
}

/*!
    \reimp

    \internal
*/
void QPixmapColorizeFilter::draw(QPainter *painter, const QPointF &dest, const QPixmap &src, const QRectF &srcRect) const
{
    Q_D(const QPixmapColorizeFilter);
    QPixmapFilter *filter = painter->paintEngine() && painter->paintEngine()->isExtended() ?
        static_cast<QPaintEngineEx *>(painter->paintEngine())->createPixmapFilter(type()) : 0;
    QPixmapColorizeFilter *colorizeFilter = static_cast<QPixmapColorizeFilter*>(filter);
    if (colorizeFilter) {
        colorizeFilter->setColor(d->color);
        colorizeFilter->draw(painter, dest, src, srcRect);
        delete colorizeFilter;
        return;
    }

    // falling back to raster implementation

    QImage srcImage;
    QImage destImage;

    if (srcRect.isNull()) {
        srcImage = src.toImage();
        srcImage = srcImage.convertToFormat(srcImage.hasAlphaChannel() ? QImage::Format_ARGB32_Premultiplied : QImage::Format_RGB32);
        destImage = QImage(srcImage.size(), srcImage.format());
    } else {
        QRect rect = srcRect.toAlignedRect().intersected(src.rect());

        srcImage = src.copy(rect).toImage();
        srcImage = srcImage.convertToFormat(srcImage.hasAlphaChannel() ? QImage::Format_ARGB32_Premultiplied : QImage::Format_RGB32);
        destImage = QImage(rect.size(), srcImage.format());
    }

    // do colorizing
    QPainter destPainter(&destImage);
    grayscale(srcImage, destImage, srcImage.rect());
    destPainter.setCompositionMode(QPainter::CompositionMode_Screen);
    destPainter.fillRect(srcImage.rect(), d->color);
    destPainter.end();

    if (srcImage.hasAlphaChannel())
        destImage.setAlphaChannel(srcImage.alphaChannel());

    painter->drawImage(dest, destImage);
}

class QPixmapDropShadowFilterPrivate : public QPixmapFilterPrivate
{
public:
    QPixmapDropShadowFilterPrivate()
            : offset(8, 8),
            radius(1),
            color(63, 63, 63, 255) {
    }

    QPointF offset;
    qreal radius;
    QColor color;

    QPixmapConvolutionFilter *convolution;
};

/*!
    \class QPixmapDropShadowFilter
    \since 4.5
    \ingroup multimedia

    \brief The QPixmapDropShadowFilter class is a convenience class
    for drawing pixmaps with drop shadows.

    The drop shadow is produced by taking a copy of the source pixmap
    and applying a color to the copy using a
    QPainter::CompositionMode_DestinationIn operation. This produces a
    homogeneously-colored pixmap which is then drawn using a
    QPixmapConvolutionFilter at an offset. The original pixmap is
    drawn on top.

    The QPixmapDropShadowFilter class provides some customization
    options to specify how the drop shadow should appear. The color of
    the drop shadow can be modified using the setColor() function, the
    drop shadow offset can be modified using the setOffset() function,
    and the blur radius of the drop shadow can be changed through the
    setBlurRadius() function.

    By default, the drop shadow is a dark gray shadow, blurred with a
    radius of 1 at an offset of 8 pixels towards the lower right.

    Example:
    \snippet doc/src/snippets/code/src_gui_image_qpixmapfilter.cpp 2

    \sa QPixmapColorizeFilter, QPixmapConvolutionFilter

    \internal
 */

/*!
    Constructs drop shadow filter.

    \internal
*/
QPixmapDropShadowFilter::QPixmapDropShadowFilter(QObject *parent)
    : QPixmapFilter(*new QPixmapDropShadowFilterPrivate, DropShadowFilter, parent)
{
    Q_D(QPixmapDropShadowFilter);
    d->convolution = new QPixmapConvolutionFilter;

    setBlurRadius(1);
}

/*!
    Destroys drop shadow filter.

    \internal
*/
QPixmapDropShadowFilter::~QPixmapDropShadowFilter()
{
    Q_D(QPixmapDropShadowFilter);
    delete d->convolution;
}

/*!
    Returns the radius in pixels of the blur on the drop shadow.

    A smaller radius results in a sharper shadow.

    \sa color(), offset()

    \internal
*/
qreal QPixmapDropShadowFilter::blurRadius() const
{
    Q_D(const QPixmapDropShadowFilter);
    return d->radius;
}

/*!
    Sets the radius in pixels of the blur on the drop shadow to the \a radius specified.

    Using a smaller radius results in a sharper shadow.

    \sa setColor(), setOffset()

    \internal
*/
void QPixmapDropShadowFilter::setBlurRadius(qreal radius)
{
    Q_D(QPixmapDropShadowFilter);

    d->radius = radius;

    int dim = 2 * qRound(radius) + 1;
    QVarLengthArray<qreal> arr(dim * dim);
    qreal f = 1 / qreal(dim * dim);
    for (int i = 0; i < dim * dim; ++i)
        arr[i] = f;
    d->convolution->setConvolutionKernel(arr.data(), dim, dim);
}

/*!
    Returns the color of the drop shadow.

    \sa blurRadius(), offset()

    \internal
*/
QColor QPixmapDropShadowFilter::color() const
{
    Q_D(const QPixmapDropShadowFilter);
    return d->color;
}

/*!
    Sets the color of the drop shadow to the \a color specified.

    \sa setBlurRadius(), setOffset()

    \internal
*/
void QPixmapDropShadowFilter::setColor(const QColor &color)
{
    Q_D(QPixmapDropShadowFilter);
    d->color = color;
}

/*!
    Returns the shadow offset in pixels.

    \sa blurRadius(), color()

    \internal
*/
QPointF QPixmapDropShadowFilter::offset() const
{
    Q_D(const QPixmapDropShadowFilter);
    return d->offset;
}

/*!
    Sets the shadow offset in pixels to the \a offset specified.

    \sa setBlurRadius(), setColor()

    \internal
*/
void QPixmapDropShadowFilter::setOffset(const QPointF &offset)
{
    Q_D(QPixmapDropShadowFilter);
    d->offset = offset;
}

/*!
    \fn void QPixmapDropShadowFilter::setOffset(qreal dx, qreal dy)
    \overload

    Sets the shadow offset in pixels to be the displacement specified by the
    horizontal \a dx and vertical \a dy coordinates.

    \sa setBlurRadius(), setColor()

    \internal
*/

/*!
    \reimp

    \internal
 */
QRectF QPixmapDropShadowFilter::boundingRectFor(const QRectF &rect) const
{
    Q_D(const QPixmapDropShadowFilter);

    qreal x1 = qMin(rect.left(), rect.left() + d->offset.x() - d->radius);
    qreal y1 = qMin(rect.top(), rect.top() + d->offset.y() - d->radius);
    qreal x2 = qMax(rect.right(), rect.right() + d->offset.x() + d->radius);
    qreal y2 = qMax(rect.bottom(), rect.bottom() + d->offset.y() + d->radius);

    return QRectF(x1, y1, x2 - x1, y2 - y1);
}

/*!
    \reimp

    \internal
 */
void QPixmapDropShadowFilter::draw(QPainter *p,
                                   const QPointF &pos,
                                   const QPixmap &px,
                                   const QRectF &src) const
{
    Q_D(const QPixmapDropShadowFilter);

    QPixmap tmp = src.isNull() ? px : px.copy(src.toRect());
    QPainter tmpPainter(&tmp);

    // blacken the image...
    tmpPainter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    tmpPainter.fillRect(0, 0, tmp.width(), tmp.height(), d->color);
    tmpPainter.end();

    // draw the blurred drop shadow...
    d->convolution->draw(p, pos + d->offset, tmp);

    // Draw the actual pixmap...
    p->drawPixmap(pos, px, src);
}
QT_END_NAMESPACE
