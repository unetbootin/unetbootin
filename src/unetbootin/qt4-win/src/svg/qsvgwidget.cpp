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

#include "qsvgwidget.h"

#ifndef QT_NO_SVGWIDGET

#include "qsvgrenderer.h"

#include "qpainter.h"
#include "private/qwidget_p.h"

QT_BEGIN_NAMESPACE

/*!
    \class QSvgWidget
    \ingroup multimedia

    \brief The QSvgWidget class provides a widget that is used to display the contents of
    Scalable Vector Graphics (SVG) files.
    \since 4.1

    This class enables developers to display SVG drawings alongside standard widgets, and
    is used in much the same way as QLabel is used for displaying text and bitmap images.

    Since QSvgWidget is a subclass of QWidget, SVG drawings are rendered using the properties
    of the display. More control can be exercised over the rendering process with the
    QSvgRenderer class, as this can be used to paint onto other paint devices, such as QImage
    and QGLWidget. The renderer used by the widget can be obtained with the renderer()
    function.

    Each QSvgWidget can be constructed with the file name of a SVG file, or they can be
    constructed without a specific file to render and one can be supplied later. The load()
    functions provide two different ways to load an SVG file: they accept either the file name
    of an SVG file or a QByteArray containing the serialized XML representation of an SVG file.

    By default, the widget provides a size hint to reflect the size of the drawing that it
    displays. If no data has been loaded, the widget provides the default QWidget size hint.
    Subclass this class and reimplement sizeHint() if you need to customize this behavior.

    \sa QSvgRenderer, {QtSvg Module}, QPicture
*/

class QSvgWidgetPrivate : public QWidgetPrivate
{
    Q_DECLARE_PUBLIC(QSvgWidget)
public:
    QSvgWidgetPrivate()
        : QWidgetPrivate()
    {
        Q_Q(QSvgWidget);
        renderer = new QSvgRenderer(q);
    }
    QSvgWidgetPrivate(const QString &file)
        : QWidgetPrivate()
    {
        Q_Q(QSvgWidget);
        renderer = new QSvgRenderer(file, q);
    }
    QSvgRenderer *renderer;
};

/*!
    Constructs a new SVG display widget with the given \a parent.
*/
QSvgWidget::QSvgWidget(QWidget *parent)
    : QWidget(*new QSvgWidgetPrivate, parent, 0)
{
    QObject::connect(d_func()->renderer, SIGNAL(repaintNeeded()),
                     this, SLOT(update()));
}

/*!
    Constructs a new SVG display widget with the given \a parent and loads the contents
    of the specified \a file.
*/
QSvgWidget::QSvgWidget(const QString &file, QWidget *parent)
    : QWidget(*new QSvgWidgetPrivate(file), parent, 0)
{
    QObject::connect(d_func()->renderer, SIGNAL(repaintNeeded()),
                     this, SLOT(update()));
}

/*!
    Destroys the widget.
*/
QSvgWidget::~QSvgWidget()
{

}

/*!
    Returns the renderer used to display the contents of the widget.
*/
QSvgRenderer * QSvgWidget::renderer() const
{
    Q_D(const QSvgWidget);
    return d->renderer;
}


/*!
    \reimp
*/
QSize QSvgWidget::sizeHint() const
{
    Q_D(const QSvgWidget);
    if (d->renderer->isValid())
        return d->renderer->defaultSize();
    else
        return QSize(128, 64);
}


/*!
    \reimp
*/
void QSvgWidget::paintEvent(QPaintEvent *)
{
    Q_D(QSvgWidget);
    QPainter p(this);
    d->renderer->render(&p);
}

/*!
    Loads the contents of the specified SVG \a file and updates the widget.
*/
void QSvgWidget::load(const QString &file)
{
    Q_D(const QSvgWidget);
    d->renderer->load(file);
}

/*!
    Loads the specified SVG format \a contents and updates the widget.
*/
void QSvgWidget::load(const QByteArray &contents)
{
    Q_D(const QSvgWidget);
    d->renderer->load(contents);
}

QT_END_NAMESPACE

#endif // QT_NO_SVGWIDGET
