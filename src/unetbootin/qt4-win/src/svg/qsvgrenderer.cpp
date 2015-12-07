/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtSVG module of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License versions 2.0 or 3.0 as published by the Free Software
** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file.  Alternatively you may (at
** your option) use any later version of the GNU General Public
** License if such license has been publicly approved by Trolltech ASA
** (or its successors, if any) and the KDE Free Qt Foundation. In
** addition, as a special exception, Trolltech gives you certain
** additional rights. These rights are described in the Trolltech GPL
** Exception version 1.2, which can be found at
** http://www.trolltech.com/products/qt/gplexception/ and in the file
** GPL_EXCEPTION.txt in this package.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/. If
** you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** In addition, as a special exception, Trolltech, as the sole
** copyright holder for Qt Designer, grants users of the Qt/Eclipse
** Integration plug-in the right for the Qt/Eclipse Integration to
** link to functionality provided by Qt Designer and its related
** libraries.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not expressly
** granted herein.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "qsvgrenderer.h"

#ifndef QT_NO_SVGRENDERER

#include "qsvgtinydocument_p.h"

#include "qbytearray.h"
#include "qtimer.h"
#include "qdebug.h"
#include "private/qobject_p.h"


QT_BEGIN_NAMESPACE

/*!
    \class QSvgRenderer
    \ingroup multimedia

    \brief The QSvgRenderer class is used to draw the contents of SVG files onto paint devices.
    \since 4.1
    \reentrant

    Using QSvgRenderer, Scalable Vector Graphics (SVG) can be rendered onto any QPaintDevice
    subclass, including QWidget, QImage, and QGLWidget.

    QSvgRenderer provides an API that supports basic features of SVG rendering, such as loading
    and rendering of static drawings, and more interactive features like animation. Since the
    rendering is performed using QPainter, SVG drawings can be rendered on any subclass of
    QPaintDevice.

    SVG drawings are either loaded when an QSvgRenderer is constructed, or loaded later
    using the load() functions. Data is either supplied directly as serialized XML, or
    indirectly using a file name. If a valid file has been loaded, either when the renderer
    is constructed or at some later time, isValid() returns true; otherwise it returns false.
    QSvgRenderer provides the render() slot to render the current document, or the current
    frame of an animated document, using a given painter.

    The defaultSize() function provides information about the amount of space that is required
    to render the currently loaded SVG file. This is useful for paint devices, such as QWidget,
    that often need to supply a size hint to their parent layout.
    The default size of a drawing may differ from its visible area, found using the \l viewBox
    property.

    Animated SVG drawings are supported, and can be controlled with a simple collection of
    functions and properties:

    \list
    \o The animated() function indicates whether a drawing contains animation information.
    \omit
    \o The animationDuration() function provides the duration in milliseconds of the
       animation, without taking any looping into account.
    \o The \l currentFrame property contains the current frame of the animation.
    \endomit
    \o The \l framesPerSecond property contains the rate at which the animation plays.
    \endlist

    Finally, the QSvgRenderer class provides the repaintNeeded() signal which is emitted
    whenever the rendering of the document needs to be updated.

    \sa QSvgWidget, {QtSvg Module}, {SVG Viewer Example}, QPicture
*/

class QSvgRendererPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QSvgRenderer)
public:
    explicit QSvgRendererPrivate()
        : QObjectPrivate(),
          render(0), timer(0),
          fps(30)
    {}
    ~QSvgRendererPrivate()
    {
        delete render;
    }
    QSvgTinyDocument *render;
    QTimer *timer;
    int fps;
};

/*!
    Constructs a new renderer with the given \a parent.
*/
QSvgRenderer::QSvgRenderer(QObject *parent)
    : QObject(*(new QSvgRendererPrivate), parent)
{
}

/*!
    Constructs a new renderer with the given \a parent and loads the contents of the
    SVG file with the specified \a filename.
*/
QSvgRenderer::QSvgRenderer(const QString &filename, QObject *parent)
    : QObject(*new QSvgRendererPrivate, parent)
{
    load(filename);
}

/*!
    Constructs a new renderer with the given \a parent and loads the specified SVG format
    \a contents.
*/
QSvgRenderer::QSvgRenderer(const QByteArray &contents, QObject *parent)
    : QObject(*new QSvgRendererPrivate, parent)
{
    load(contents);
}

/*!
    Destroys the renderer.
*/
QSvgRenderer::~QSvgRenderer()
{

}

/*!
    Returns true if there is a valid current document; otherwise returns false.
*/
bool QSvgRenderer::isValid() const
{
    Q_D(const QSvgRenderer);
    return d->render;
}

/*!
    Returns the default size of the document contents.
*/
QSize QSvgRenderer::defaultSize() const
{
    Q_D(const QSvgRenderer);
    if (d->render)
        return d->render->size();
    else
        return QSize();
}

/*!
    Returns viewBoxF().toRect().

    \sa viewBoxF()
*/
QRect QSvgRenderer::viewBox() const
{
    Q_D(const QSvgRenderer);
    if (d->render)
        return d->render->viewBox().toRect();
    else
        return QRect();
}

/*!
    \property QSvgRenderer::viewBox
    \brief the rectangle specifying the visible area of the document in logical coordinates
    \since 4.2
*/
void QSvgRenderer::setViewBox(const QRect &viewbox)
{
    Q_D(QSvgRenderer);
    if (d->render)
        d->render->setViewBox(viewbox);
}

/*!
    Returns true if the current document contains animated elements; otherwise
    returns false.

    \sa framesPerSecond()
*/
bool QSvgRenderer::animated() const
{
    Q_D(const QSvgRenderer);
    if (d->render)
        return d->render->animated();
    else
        return false;
}

/*!
    \property QSvgRenderer::framesPerSecond
    \brief the number of frames per second to be shown

    The number of frames per second is 0 if the current document is not animated.

    \sa animated()
*/
int QSvgRenderer::framesPerSecond() const
{
    Q_D(const QSvgRenderer);
    return d->fps;
}

void QSvgRenderer::setFramesPerSecond(int num)
{
    Q_D(QSvgRenderer);
    if (num < 0) {
        qWarning("QSvgRenderer::setFramesPerSecond: Cannot set negative value %d", num);
        return;
    }
    d->fps = num;
}

/*!
  \property QSvgRenderer::currentFrame
  \brief the current frame of the document's animation, or 0 if the document is not animated
  \internal

  \sa animationDuration(), framesPerSecond, animated()
*/

/*!
  \internal
*/
int QSvgRenderer::currentFrame() const
{
    Q_D(const QSvgRenderer);
    return d->render->currentFrame();
}

/*!
  \internal
*/
void QSvgRenderer::setCurrentFrame(int frame)
{
    Q_D(QSvgRenderer);
    d->render->setCurrentFrame(frame);
}

/*!
    \internal

    Returns the number of frames in the animation, or 0 if the current document is not
    animated.

    \sa animated(), framesPerSecond
*/
int QSvgRenderer::animationDuration() const
{
    Q_D(const QSvgRenderer);
    return d->render->animationDuration();
}

/*!
    Loads the SVG file specified by \a filename, returning true if the content
    was successfully parsed; otherwise returns false.
*/
bool QSvgRenderer::load(const QString &filename)
{
    Q_D(QSvgRenderer);
    delete d->render;
    d->render = QSvgTinyDocument::load(filename);
    if (d->render && d->render->animated() && d->fps > 0) {
        if (!d->timer)
            d->timer = new QTimer(this);
        else
            d->timer->stop();
        connect(d->timer, SIGNAL(timeout()),
                this, SIGNAL(repaintNeeded()));
        d->timer->start(1000/d->fps);
    } else if (d->timer) {
        d->timer->stop();
    }

    //force first update
    emit repaintNeeded();

    return d->render;
}

/*!
    Loads the specified SVG format \a contents, returning true if the content
    was successfully parsed; otherwise returns false.
*/
bool QSvgRenderer::load(const QByteArray &contents)
{
    Q_D(QSvgRenderer);
    delete d->render;
    d->render = QSvgTinyDocument::load(contents);
    if (d->render && d->render->animated() && d->fps > 0) {
        if (!d->timer)
            d->timer = new QTimer(this);
        else
            d->timer->stop();
        connect(d->timer, SIGNAL(timeout()),
                this, SIGNAL(repaintNeeded()));
        d->timer->start(1000/d->fps);
    } else if (d->timer) {
        d->timer->stop();
    }

    //force first update
    emit repaintNeeded();

    return d->render;
}

/*!
    Renders the current document, or the current frame of an animated
    document, using the given \a painter.
*/
void QSvgRenderer::render(QPainter *painter)
{
    Q_D(QSvgRenderer);
    if (d->render) {
        d->render->draw(painter);
    }
}

/*!
    \fn void QSvgRenderer::repaintNeeded()

    This signal is emitted whenever the rendering of the document
    needs to be updated, usually for the purposes of animation.
*/

/*!
    Renders the given element with \a elementId using the given \a painter
    on the specified \a bounds. If the bounding rectangle is not specified
    the SVG element is mapped to the whole paint device.
*/
void QSvgRenderer::render(QPainter *painter, const QString &elementId,
                          const QRectF &bounds)
{
    Q_D(QSvgRenderer);
    if (d->render) {
        d->render->draw(painter, elementId, bounds);
    }
}

/*!
    Renders the current document, or the current frame of an animated
    document, using the given \a painter on the specified \a bounds within
    the painter.  If the bounding rectangle is not specified
    the SVG file is mapped to the whole paint device.
*/
void QSvgRenderer::render(QPainter *painter, const QRectF &bounds)
{
    Q_D(QSvgRenderer);
    if (d->render) {
        d->render->draw(painter, bounds);
    }
}

QRectF QSvgRenderer::viewBoxF() const
{
    Q_D(const QSvgRenderer);
    if (d->render)
        return d->render->viewBox();
    else
        return QRect();
}

void QSvgRenderer::setViewBox(const QRectF &viewbox)
{
    Q_D(QSvgRenderer);
    if (d->render)
        d->render->setViewBox(viewbox);
}

/*!
    \since 4.2

    Returns bounding rectangle of the item with the given \a id.
    The transformation matrix of parent elements is not affecting
    the bounds of the element.
*/
QRectF QSvgRenderer::boundsOnElement(const QString &id) const
{
    Q_D(const QSvgRenderer);
    QRectF bounds;
    if (d->render)
        bounds = d->render->boundsOnElement(id);
    return bounds;
}


/*!
    \since 4.2

    Returns true if the element with the given \a id exists
    in the currently parsed SVG file and is a renderable
    element.

    Note: this method returns true only for elements that
    can be rendered. Which implies that elements that are considered
    part of the fill/stroke style properties, e.g. radialGradients
    even tough marked with "id" attributes will not be found by this
    method.
*/
bool QSvgRenderer::elementExists(const QString &id) const
{
    Q_D(const QSvgRenderer);
    bool exists = false;
    if (d->render)
        exists = d->render->elementExists(id);
    return exists;
}

/*!
    \since 4.2

    Returns the transformation matrix setup for the element
    with the given \a id. That includes the transformation on
    the element itself.
*/
QMatrix QSvgRenderer::matrixForElement(const QString &id) const
{
    Q_D(const QSvgRenderer);
    QMatrix mat;
    if (d->render)
        mat = d->render->matrixForElement(id);
    return mat;
}

QT_END_NAMESPACE

#include "moc_qsvgrenderer.cpp"

#endif // QT_NO_SVGRENDERER
