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
