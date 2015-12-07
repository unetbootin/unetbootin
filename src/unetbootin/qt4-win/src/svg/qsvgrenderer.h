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

#ifndef QSVGRENDERER_H
#define QSVGRENDERER_H

#include <QtGui/qmatrix.h>

#ifndef QT_NO_SVGRENDERER

#include <QtCore/qobject.h>
#include <QtCore/qsize.h>
#include <QtCore/qrect.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Svg)

class QSvgRendererPrivate;
class QPainter;
class QByteArray;

class Q_SVG_EXPORT QSvgRenderer : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QRectF viewBox READ viewBoxF WRITE setViewBox)
    Q_PROPERTY(int framesPerSecond READ framesPerSecond WRITE setFramesPerSecond)
    Q_PROPERTY(int currentFrame READ currentFrame WRITE setCurrentFrame)
public:
    QSvgRenderer(QObject *parent=0);
    QSvgRenderer(const QString &filename, QObject *parent=0);
    QSvgRenderer(const QByteArray &contents, QObject *parent=0);
    ~QSvgRenderer();

    bool isValid() const;

    QSize defaultSize() const;

    QRect viewBox() const;
    QRectF viewBoxF() const;
    void setViewBox(const QRect &viewbox);
    void setViewBox(const QRectF &viewbox);

    bool animated() const;
    int framesPerSecond() const;
    void setFramesPerSecond(int num);
    int currentFrame() const;
    void setCurrentFrame(int);
    int animationDuration() const;//in seconds

    QRectF boundsOnElement(const QString &id) const;
    bool elementExists(const QString &id) const;
    QMatrix matrixForElement(const QString &id) const;

public Q_SLOTS:
    bool load(const QString &filename);
    bool load(const QByteArray &contents);
    void render(QPainter *p);
    void render(QPainter *p, const QRectF &bounds);

    void render(QPainter *p, const QString &elementId,
                const QRectF &bounds=QRectF());

Q_SIGNALS:
    void repaintNeeded();

private:
    Q_DECLARE_PRIVATE(QSvgRenderer)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QT_NO_SVGRENDERER
#endif // QSVGRENDERER_H
