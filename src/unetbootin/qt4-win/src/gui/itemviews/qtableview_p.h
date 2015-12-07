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

#ifndef QTABLEVIEW_P_H
#define QTABLEVIEW_P_H

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

#include "private/qabstractitemview_p.h"

#ifndef QT_NO_TABLEVIEW

QT_BEGIN_NAMESPACE

class QTableViewPrivate : public QAbstractItemViewPrivate
{
    Q_DECLARE_PUBLIC(QTableView)
public:
    QTableViewPrivate()
        : showGrid(true), gridStyle(Qt::SolidLine),
          rowSectionAnchor(-1), columnSectionAnchor(-1),
          columnResizeTimerID(0), rowResizeTimerID(0),
          horizontalHeader(0), verticalHeader(0),
          sortingEnabled(false), geometryRecursionBlock(false)
 {
    wrapItemText = true;
#ifndef QT_NO_DRAGANDDROP
    overwrite = true;
#endif
 }
    void init();
    void trimHiddenSelections(QItemSelectionRange *range) const;

    inline bool isHidden(int row, int col) const {
        return verticalHeader->isSectionHidden(row)
            || horizontalHeader->isSectionHidden(col);
    }
    inline int visualRow(int logicalRow) const {
        return verticalHeader->visualIndex(logicalRow);
    }
    inline int visualColumn(int logicalCol) const {
        return horizontalHeader->visualIndex(logicalCol);
    }
    inline int logicalRow(int visualRow) const {
        return verticalHeader->logicalIndex(visualRow);
    }
    inline int logicalColumn(int visualCol) const {
        return horizontalHeader->logicalIndex(visualCol);
    }

    int sectionSpanEndLogical(const QHeaderView *header, int logical, int span) const;
    int sectionSpanSize(const QHeaderView *header, int logical, int span) const;
    bool spanContainsSection(const QHeaderView *header, int logical, int spanLogical, int span) const;
    bool spansIntersectColumn(int column) const;
    bool spansIntersectRow(int row) const;
    bool spansIntersectColumns(const QList<int> &columns) const;
    bool spansIntersectRows(const QList<int> &rows) const;
    QBitArray drawAndClipSpans(const QRect &area, QPainter *painter,
                               const QStyleOptionViewItemV4 &option);
    void drawCell(QPainter *painter, const QStyleOptionViewItemV4 &option, const QModelIndex &index);

    bool showGrid;
    Qt::PenStyle gridStyle;
    int rowSectionAnchor;
    int columnSectionAnchor;
    int columnResizeTimerID;
    int rowResizeTimerID;
    QList<int> columnsToUpdate;
    QList<int> rowsToUpdate;
    QHeaderView *horizontalHeader;
    QHeaderView *verticalHeader;
    QWidget *cornerWidget;
    bool sortingEnabled;
    bool geometryRecursionBlock;

    struct Span
    {
        int m_top;
        int m_left;
        int m_bottom;
        int m_right;
        Span()
            : m_top(-1), m_left(-1), m_bottom(-1), m_right(-1) { }
        Span(int row, int column, int rowCount, int columnCount)
            : m_top(row), m_left(column), m_bottom(row+rowCount-1), m_right(column+columnCount-1) { }
        bool operator<(const Span &other) const
        {
            if (m_top < other.m_top)
                return true;
            if (m_top == other.m_top) {
                if (m_bottom < other.m_bottom)
                    return true;
                if (m_bottom == other.m_bottom) {
                    if (m_left < other.m_left)
                        return true;
                    if (m_left == other.m_left) {
                        if (m_right < other.m_right)
                            return true;
                    }
                }
            }
            return false;
        }
        inline int top() const { return m_top; }
        inline int left() const { return m_left; }
        inline int bottom() const { return m_bottom; }
        inline int right() const { return m_right; }
        inline int height() const { return m_bottom - m_top + 1; }
        inline int width() const { return m_right - m_left + 1; }
    };
    QList<Span> spans;

    void setSpan(int row, int column, int rowSpan, int columnSpan);
    Span span(int row, int column) const;
    inline int rowSpan(int row, int column) const {
        return span(row, column).height();
    }
    inline int columnSpan(int row, int column) const {
        return span(row, column).width();
    }
    inline bool hasSpans() const {
        return !spans.isEmpty();
    }
    inline bool spanContainsRow(int row, int spanRow, int span) const {
        return spanContainsSection(verticalHeader, row, spanRow, span);
    }
    inline bool spanContainsColumn(int column, int spanColumn, int span) const {
        return spanContainsSection(horizontalHeader, column, spanColumn, span);
    }
    inline bool isInSpan(int row, int column, const Span &span) const {
        return spanContainsRow(row, span.top(), span.height())
            && spanContainsColumn(column, span.left(), span.width());
    }
    inline int rowSpanHeight(int row, int span) const {
        return sectionSpanSize(verticalHeader, row, span);
    }
    inline int columnSpanWidth(int column, int span) const {
        return sectionSpanSize(horizontalHeader, column, span);
    }
    inline int rowSpanEndLogical(int row, int span) const {
        return sectionSpanEndLogical(verticalHeader, row, span);
    }
    inline int columnSpanEndLogical(int column, int span) const {
        return sectionSpanEndLogical(horizontalHeader, column, span);
    }

    QRect visualSpanRect(const Span &span) const;

    void _q_selectRow(int row);
    void _q_selectColumn(int column);

    void selectRow(int row, bool anchor);
    void selectColumn(int column, bool anchor);
};

QT_END_NAMESPACE

#endif // QT_NO_TABLEVIEW

#endif // QTABLEVIEW_P_H
