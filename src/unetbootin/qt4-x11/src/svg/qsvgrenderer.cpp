/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtSVG module of the Qt Toolkit.
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

    static void callRepaintNeeded(QSvgRenderer *const q);

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
    Constructs a new renderer with the given \a parent and loads the SVG data
    from the byte array specified by \a contents.
*/
QSvgRenderer::QSvgRenderer(const QByteArray &contents, QObject *parent)
    : QObject(*new QSvgRendererPrivate, parent)
{
    load(contents);
}

/*!
    \since 4.5

    Constructs a new renderer with the given \a parent and loads the SVG data
    using the stream reader specified by \a contents.
*/
QSvgRenderer::QSvgRenderer(QXmlStreamReader *contents, QObject *parent)
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
 \internal
 \since 4.5

 We can't have template functions, that's loadDocument(), as friends, for this
 code, so we let this function be a friend of QSvgRenderer instead.
 */
void QSvgRendererPrivate::callRepaintNeeded(QSvgRenderer *const q)
{
    q->repaintNeeded();
}

template<typename TInputType>
static bool loadDocument(QSvgRenderer *const q,
                         QSvgRendererPrivate *const d,
                         const TInputType &in)
{
    delete d->render;
    d->render = QSvgTinyDocument::load(in);
    if (d->render && d->render->animated() && d->fps > 0) {
        if (!d->timer)
            d->timer = new QTimer(q);
        else
            d->timer->stop();
        q->connect(d->timer, SIGNAL(timeout()),
                   q, SIGNAL(repaintNeeded()));
        d->timer->start(1000/d->fps);
    } else if (d->timer) {
        d->timer->stop();
    }

    //force first update
    QSvgRendererPrivate::callRepaintNeeded(q);

    return d->render;
}

/*!
    Loads the SVG file specified by \a filename, returning true if the content
    was successfully parsed; otherwise returns false.
*/
bool QSvgRenderer::load(const QString &filename)
{
    Q_D(QSvgRenderer);
    return loadDocument(this, d, filename);
}

/*!
    Loads the specified SVG format \a contents, returning true if the content
    was successfully parsed; otherwise returns false.
*/
bool QSvgRenderer::load(const QByteArray &contents)
{
    Q_D(QSvgRenderer);
    return loadDocument(this, d, contents);
}

/*!
  Loads the specified SVG in \a contents, returning true if the content
  was successfully parsed; otherwise returns false.

  The reader will be used from where it currently is positioned. If \a contents
  is \c null, behavior is undefined.

  \since 4.5
*/
bool QSvgRenderer::load(QXmlStreamReader *contents)
{
    Q_D(QSvgRenderer);
    return loadDocument(this, d, contents);
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

    \sa matrixForElement()
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

    Returns the transformation matrix for the element
    with the given \a id. The matrix is a product of
    the transformation of the element's parents. The transformation of
    the element itself is not included.

    To find the bounding rectangle of the element in logical coordinates,
    you can apply the matrix on the rectangle returned from boundsOnElement().

    \sa boundsOnElement()
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
