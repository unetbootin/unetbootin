/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt3Support module of the Qt Toolkit.
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

#ifndef Q3GRIDVIEW_H
#define Q3GRIDVIEW_H

#include <Qt3Support/q3scrollview.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3SupportLight)

class Q3GridViewPrivate;

class Q_COMPAT_EXPORT Q3GridView : public Q3ScrollView
{
    Q_OBJECT
    Q_PROPERTY(int numRows READ numRows WRITE setNumRows)
    Q_PROPERTY(int numCols READ numCols WRITE setNumCols)
    Q_PROPERTY(int cellWidth READ cellWidth WRITE setCellWidth)
    Q_PROPERTY(int cellHeight READ cellHeight WRITE setCellHeight)
public:

    Q3GridView(QWidget *parent=0, const char *name=0, Qt::WindowFlags f=0);
   ~Q3GridView();

    int numRows() const;
    virtual void setNumRows(int);
    int numCols() const;
    virtual void setNumCols(int);

    int cellWidth() const;
    virtual void setCellWidth(int);
    int cellHeight() const;
    virtual void setCellHeight(int);

    QRect cellRect() const;
    QRect cellGeometry(int row, int column);
    QSize gridSize() const;

    int rowAt(int y) const;
    int columnAt(int x) const;

    void repaintCell(int row, int column, bool erase=true);
    void updateCell(int row, int column);
    void ensureCellVisible(int row, int column);

protected:
    virtual void paintCell(QPainter *, int row, int col) = 0;
    virtual void paintEmptyArea(QPainter *p, int cx, int cy, int cw, int ch);

    void drawContents(QPainter *p, int cx, int cy, int cw, int ch);

    virtual void dimensionChange(int, int);

private:
    void drawContents(QPainter*);
    void updateGrid();

    int nrows;
    int ncols;
    int cellw;
    int cellh;
    Q3GridViewPrivate* d;

    Q_DISABLE_COPY(Q3GridView)
};

inline int Q3GridView::cellWidth() const
{ return cellw; }

inline int Q3GridView::cellHeight() const
{ return cellh; }

inline int Q3GridView::rowAt(int y) const
{ return y / cellh; }

inline int Q3GridView::columnAt(int x) const
{ return x / cellw; }

inline int Q3GridView::numRows() const
{ return nrows; }

inline int Q3GridView::numCols() const
{return ncols; }

inline QRect Q3GridView::cellRect() const
{ return QRect(0, 0, cellw, cellh); }

inline QSize Q3GridView::gridSize() const
{ return QSize(ncols * cellw, nrows * cellh); }

QT_END_NAMESPACE

QT_END_HEADER

#endif // Q3GRIDVIEW_H
