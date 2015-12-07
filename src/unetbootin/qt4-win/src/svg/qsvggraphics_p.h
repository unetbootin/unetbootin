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

#ifndef QSVGGRAPHICS_P_H
#define QSVGGRAPHICS_P_H

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

#include "qsvgnode_p.h"

#ifndef QT_NO_SVG

#include "QtGui/qpainterpath.h"
#include "QtGui/qimage.h"
#include "QtGui/qtextlayout.h"
#include "QtGui/qtextoption.h"
#include "QtCore/qstack.h"

QT_BEGIN_NAMESPACE

class QTextCharFormat;

class QSvgAnimation : public QSvgNode
{
public:
    virtual void draw(QPainter *p);
    virtual Type type() const;
};

class QSvgArc : public QSvgNode
{
public:
    QSvgArc(QSvgNode *parent, const QPainterPath &path);
    virtual void draw(QPainter *p);
    virtual Type type() const;
    virtual QRectF bounds() const;
private:
    QPainterPath cubic;
    QRectF m_cachedBounds;
};

class QSvgCircle : public QSvgNode
{
public:
    QSvgCircle(QSvgNode *parent, const QRectF &rect);
    virtual void draw(QPainter *p);
    virtual Type type() const;
    virtual QRectF bounds() const;
private:
    QRectF m_bounds;
};

class QSvgEllipse : public QSvgNode
{
public:
    QSvgEllipse(QSvgNode *parent, const QRectF &rect);
    virtual void draw(QPainter *p);
    virtual Type type() const;
    virtual QRectF bounds() const;
private:
    QRectF m_bounds;
};

class QSvgImage : public QSvgNode
{
public:
    QSvgImage(QSvgNode *parent, const QImage &image,
              const QRect &bounds);
    virtual void draw(QPainter *p);
    virtual Type type() const;
    virtual QRectF bounds() const;
private:
    QImage m_image;
    QRect  m_bounds;
};

class QSvgLine : public QSvgNode
{
public:
    QSvgLine(QSvgNode *parent, const QLineF &line);
    virtual void draw(QPainter *p);
    virtual Type type() const;
    virtual QRectF bounds() const;
private:
    QLineF m_bounds;
};

class QSvgPath : public QSvgNode
{
public:
    QSvgPath(QSvgNode *parent, const QPainterPath &qpath);
    virtual void draw(QPainter *p);
    virtual Type type() const;
    virtual QRectF bounds() const;

    QPainterPath *qpath() {
        return &m_path;
    }
private:
    QPainterPath m_path;
    QRectF m_cachedBounds;
};

class QSvgPolygon : public QSvgNode
{
public:
    QSvgPolygon(QSvgNode *parent, const QPolygonF &poly);
    virtual void draw(QPainter *p);
    virtual Type type() const;
    virtual QRectF bounds() const;
private:
    QPolygonF m_poly;
};

class QSvgPolyline : public QSvgNode
{
public:
    QSvgPolyline(QSvgNode *parent, const QPolygonF &poly);
    virtual void draw(QPainter *p);
    virtual Type type() const;
    virtual QRectF bounds() const;
private:
    QPolygonF m_poly;
};

class QSvgRect : public QSvgNode
{
public:
    QSvgRect(QSvgNode *paren, const QRectF &rect, int rx=0, int ry=0);
    virtual Type type() const;
    virtual void draw(QPainter *p);
    virtual QRectF bounds() const;
private:
    QRectF m_rect;
    int m_rx, m_ry;
};

class  QSvgText : public QSvgNode
{
public:
    QSvgText(QSvgNode *parent, const QPointF &coord);
    ~QSvgText();
    virtual void draw(QPainter *p);
    virtual Type type() const;
    void insertText(const QString &text);
    void insertFormat(const QTextCharFormat &format);
    void popFormat();
    void setTextAlignment(const Qt::Alignment &alignment);
    const QTextCharFormat &topFormat() const;
    //virtual QRectF bounds() const;
private:
    QPointF m_coord;

    QString m_text;
    QStack<QTextCharFormat> m_formats;
    Qt::Alignment           m_textAlignment;
    QList<QTextLayout::FormatRange> m_formatRanges;

};

class  QSvgTextArea : public QSvgNode
{
public:
    virtual void draw(QPainter *p);
    virtual Type type() const;
};

class QSvgUse : public QSvgNode
{
public:
    QSvgUse(const QPointF &start, QSvgNode *parent, QSvgNode *link);
    virtual void draw(QPainter *p);
    virtual Type type() const;
    virtual QRectF bounds() const;
    virtual QRectF transformedBounds(const QMatrix &mat) const;

private:
    QSvgNode *m_link;
    QPointF   m_start;
    mutable QRectF    m_bounds;
};

class QSvgVideo : public QSvgNode
{
public:
    virtual void draw(QPainter *p);
    virtual Type type() const;
};

QT_END_NAMESPACE

#endif // QT_NO_SVG
#endif // QSVGGRAPHICS_P_H
