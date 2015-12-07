/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#ifndef QGRAPHICSGRIDLAYOUT_H
#define QGRAPHICSGRIDLAYOUT_H

#include <QtGui/qgraphicsitem.h>
#include <QtGui/qgraphicslayout.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

#if !defined(QT_NO_GRAPHICSVIEW) || (QT_EDITION & QT_MODULE_GRAPHICSVIEW) != QT_MODULE_GRAPHICSVIEW

class QGraphicsGridLayoutPrivate;

class Q_GUI_EXPORT QGraphicsGridLayout : public QGraphicsLayout
{
public:
    QGraphicsGridLayout(QGraphicsLayoutItem *parent = 0);
    virtual ~QGraphicsGridLayout();

    void addItem(QGraphicsLayoutItem *item, int row, int column, int rowSpan, int columnSpan,
                   Qt::Alignment alignment = 0);
    inline void addItem(QGraphicsLayoutItem *item, int row, int column, Qt::Alignment alignment = 0);

    void setHorizontalSpacing(qreal spacing);
    qreal horizontalSpacing() const;
    void setVerticalSpacing(qreal spacing);
    qreal verticalSpacing() const;
    void setSpacing(qreal spacing);

    void setRowSpacing(int row, qreal spacing);
    qreal rowSpacing(int row) const;
    void setColumnSpacing(int column, qreal spacing);
    qreal columnSpacing(int column) const;

    void setRowStretchFactor(int row, int stretch);
    int rowStretchFactor(int row) const;
    void setColumnStretchFactor(int column, int stretch);
    int columnStretchFactor(int column) const;

    void setRowMinimumHeight(int row, qreal height);
    qreal rowMinimumHeight(int row) const;
    void setRowPreferredHeight(int row, qreal height);
    qreal rowPreferredHeight(int row) const;
    void setRowMaximumHeight(int row, qreal height);
    qreal rowMaximumHeight(int row) const;
    void setRowFixedHeight(int row, qreal height);

    void setColumnMinimumWidth(int column, qreal width);
    qreal columnMinimumWidth(int column) const;
    void setColumnPreferredWidth(int column, qreal width);
    qreal columnPreferredWidth(int column) const;
    void setColumnMaximumWidth(int column, qreal width);
    qreal columnMaximumWidth(int column) const;
    void setColumnFixedWidth(int column, qreal width);

    void setRowAlignment(int row, Qt::Alignment alignment);
    Qt::Alignment rowAlignment(int row) const;
    void setColumnAlignment(int column, Qt::Alignment alignment);
    Qt::Alignment columnAlignment(int column) const;

    void setAlignment(QGraphicsLayoutItem *item, Qt::Alignment alignment);
    Qt::Alignment alignment(QGraphicsLayoutItem *item) const;

    int rowCount() const;
    int columnCount() const;

    QGraphicsLayoutItem *itemAt(int row, int column) const;
    
    // inherited from QGraphicsLayout
    int count() const;
    QGraphicsLayoutItem *itemAt(int index) const;
    void removeAt(int index);

    void invalidate();

    // inherited from QGraphicsLayoutItem
    void setGeometry(const QRectF &rect);
    QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint = QSizeF()) const;

    // ####
    //QRect cellRect(int row, int column, int rowSpan = 1, int columnSpan = 1) const;
    //QSizePolicy::ControlTypes controlTypes(LayoutSide side) const;

private:
    Q_DISABLE_COPY(QGraphicsGridLayout)
    Q_DECLARE_PRIVATE(QGraphicsGridLayout)
};

inline void QGraphicsGridLayout::addItem(QGraphicsLayoutItem *aitem, int arow, int acolumn, Qt::Alignment aalignment)
{ addItem(aitem, arow, acolumn, 1, 1, aalignment); }

#endif

QT_END_NAMESPACE

QT_END_HEADER

#endif

