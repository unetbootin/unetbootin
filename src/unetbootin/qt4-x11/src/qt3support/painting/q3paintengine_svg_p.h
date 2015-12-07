/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt3Support module of the Qt Toolkit.
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

#ifndef Q3PAINTENGINE_SVG_P_H
#define Q3PAINTENGINE_SVG_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "QtXml/qdom.h"
#include "QtGui/qpaintengine.h"
#include "private/qpicture_p.h" // for QPaintCommands

QT_BEGIN_NAMESPACE

class Q3SVGPaintEnginePrivate;

class Q3SVGPaintEngine : public QPaintEngine
{
    Q_DECLARE_PRIVATE(Q3SVGPaintEngine)

public:
    Q3SVGPaintEngine();
    ~Q3SVGPaintEngine();

    bool begin(QPaintDevice *pdev);
    bool end();

    void updateState(const QPaintEngineState &state);

    void updatePen(const QPen &pen);
    void updateBrush(const QBrush &brush, const QPointF &origin);
    void updateFont(const QFont &font);
    void updateBackground(Qt::BGMode bgmode, const QBrush &bgBrush);
    void updateMatrix(const QMatrix &matrix);
    void updateClipRegion(const QRegion &region, Qt::ClipOperation op);
    void updateClipPath(const QPainterPath &path, Qt::ClipOperation op);
    void updateRenderHints(QPainter::RenderHints hints);

    void drawEllipse(const QRect &r);
    void drawLine(const QLineF &line);
    void drawLines(const QLineF *lines, int lineCount);
    void drawRect(const QRectF &r);
    void drawPoint(const QPointF &p);
    void drawPoints(const QPointF *points, int pointCount);
    void drawPath(const QPainterPath &path);
    void drawPolygon(const QPointF *points, int pointCount, PolygonDrawMode mode);
    void drawPolygon(const QPoint *points, int pointCount, PolygonDrawMode mode);

    void drawPixmap(const QRectF &r, const QPixmap &pm, const QRectF &sr);
    void drawTiledPixmap(const QRectF &r, const QPixmap &pixmap, const QPointF &s);
    void drawTextItem(const QPointF &p, const QTextItem &ti);
    void drawImage(const QRectF &r, const QImage &pm, const QRectF &sr,
                   Qt::ImageConversionFlags flags = Qt::AutoColor);

#if defined Q_WS_WIN // ### not liking this!!
    HDC handle() const { return 0; }
#else
    Qt::HANDLE handle() const {return 0; }
#endif
    Type type() const { return SVG; }
    bool play(QPainter *p);

    QString toString() const;

    bool load(QIODevice *dev);
    bool save(QIODevice *dev);
    bool save(const QString &fileName);

    QRect boundingRect() const;
    void setBoundingRect(const QRect &r);

protected:
    Q3SVGPaintEngine(Q3SVGPaintEnginePrivate &dptr);

private:
    Q_DISABLE_COPY(Q3SVGPaintEngine)
};

QT_END_NAMESPACE

#endif // Q3PAINTENGINE_SVG_P_H
