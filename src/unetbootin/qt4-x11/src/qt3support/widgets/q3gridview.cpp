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


#include "q3gridview.h"
#include "qpainter.h"

QT_BEGIN_NAMESPACE

using namespace Qt;

/*!
  \class Q3GridView qgridview.h
  \brief The Q3GridView class provides an abstract base for
  fixed-size grids.

  \compat

  A grid view consists of a number of abstract cells organized in
  rows and columns. The cells have a fixed size and are identified
  with a row index and a column index. The top-left cell is in row
  0, column 0. The bottom-right cell is in row numRows()-1, column
  numCols()-1.

  You can define \l{Q3GridView::numRows} {numRows},
  \l{Q3GridView::numCols} {numCols}, \l{Q3GridView::cellWidth}
  {cellWidth} and \l{Q3GridView::cellHeight} {cellHeight}. Reimplement
  the pure virtual function paintCell() to draw the contents of a
  cell.

  With ensureCellVisible(), you can ensure a certain cell is
  visible. With rowAt() and columnAt() you can find a cell based on
  the given x- and y-coordinates.

  If you need to monitor changes to the grid's dimensions (i.e. when
  numRows or numCols is changed), reimplement the dimensionChange()
  change handler.

  Note: the row and column indices are always given in the order,
  row (vertical offset) then column (horizontal offset). This order
  is the opposite of all pixel operations, which are given in the
  order x (horizontal offset), y (vertical offset).

  Q3GridView is a very simple abstract class based on Q3ScrollView. It
  is designed to simplify the task of drawing many cells of the same
  size in a potentially scrollable canvas. If you need rows and
  columns with different sizes, use a Q3Table instead. If you need a
  simple list of items, use a Q3ListBox. If you need to present
  hierachical data use a Q3ListView, and if you need random objects
  at random positions, consider using either a Q3IconView or a
  Q3Canvas.
*/


/*!
  Constructs a grid view.

  The \a parent, \a name and widget flag, \a f, arguments are passed
  to the Q3ScrollView constructor.
*/
Q3GridView::Q3GridView(QWidget *parent, const char *name, Qt::WindowFlags f)
    : Q3ScrollView(parent, name, f | WStaticContents),
      nrows(5), ncols(5), cellw(12), cellh(12)
{
    viewport()->setBackgroundMode(PaletteBase);
    setBackgroundMode(PaletteBackground, PaletteBase);
    viewport()->setFocusProxy(this);
}

/*!
  Destroys the grid view.
*/
Q3GridView::~Q3GridView()
{
}

void Q3GridView::updateGrid()
{
    resizeContents(ncols * cellw, nrows * cellh);
}

/*!
  \property Q3GridView::numRows
  \brief The number of rows in the grid

  \sa numCols
*/
void Q3GridView::setNumRows(int numRows)
{
    int oldnrows = nrows;
    nrows = numRows;
    dimensionChange(oldnrows, ncols);
    updateGrid();
}

/*!
  \property Q3GridView::numCols
  \brief The number of columns in the grid

  \sa numRows
*/
void Q3GridView::setNumCols(int numCols)
{
    int oldncols = ncols;
    ncols = numCols;
    dimensionChange(nrows, oldncols);
    updateGrid();
}

/*!
  \property Q3GridView::cellWidth
  \brief The width of a grid column

  All columns in a grid view have the same width.

  \sa cellHeight
*/
void Q3GridView::setCellWidth(int cellWidth)
{
    cellw = cellWidth;
    updateGrid();
    updateContents();
}

/*!
  \property Q3GridView::cellHeight
  \brief The height of a grid row

  All rows in a grid view have the same height.

  \sa cellWidth
*/
void Q3GridView::setCellHeight(int cellHeight)
{
    cellh = cellHeight;
    updateGrid();
    updateContents();
}

/*!
  Returns the geometry of cell (\a row, \a column) in the content
  coordinate system.

  \sa cellRect()
*/
QRect Q3GridView::cellGeometry(int row, int column)
{
    QRect r;
    if (row >= 0 && row < nrows && column >= 0 && column < ncols)
	r.setRect(cellw * column, cellh * row, cellw, cellh);
    return r;
}

/*!
  Repaints cell (\a row, \a column).

  If \a erase is true, Qt erases the area of the cell before the
  paintCell() call; otherwise no erasing takes place.

  \sa QWidget::repaint()
*/
void Q3GridView::repaintCell(int row, int column, bool erase)
{
    repaintContents(cellGeometry(row, column), erase);
}

/*!
  Updates cell (\a row, \a column).

  \sa QWidget::update()
*/
void Q3GridView::updateCell(int row, int column)
{
    updateContents(cellGeometry(row, column));
}

/*!
  Ensures cell (\a row, \a column) is visible, scrolling the grid
  view if necessary.
*/
void Q3GridView::ensureCellVisible(int row, int column)
{
    QRect r = cellGeometry(row, column);
    ensureVisible(r.x(), r.y(), r.width(), r.height());
}

/*!
  This function fills the \a cw pixels wide and \a ch pixels high
  rectangle starting at position (\a cx, \a cy) with the background
  color using the painter \a p.

  paintEmptyArea() is invoked by drawContents() to erase or fill
  unused areas.
*/

void Q3GridView::paintEmptyArea(QPainter *p, int cx ,int cy, int cw, int ch)
{
    if (gridSize().width() >= contentsWidth() && gridSize().height() >= contentsHeight())
	return;
    // Region of the rect we should draw
    contentsToViewport(cx, cy, cx, cy);
    QRegion reg(QRect(cx, cy, cw, ch));
    // Subtract the table from it
    reg = reg.subtracted(QRect(contentsToViewport(QPoint(0, 0)), gridSize()));

    // And draw the rectangles (transformed as needed)
    QVector<QRect> r = reg.rects();
    const QBrush &brush = backgroundBrush();
    for (int i = 0; i < (int)r.count(); ++i)
	p->fillRect(r[ i ], brush);
}

/*!\reimp
 */
void Q3GridView::drawContents(QPainter *p, int cx, int cy, int cw, int ch)
{
    int colfirst = columnAt(cx);
    int collast = columnAt(cx + cw);
    int rowfirst = rowAt(cy);
    int rowlast = rowAt(cy + ch);

    if (rowfirst == -1 || colfirst == -1) {
	paintEmptyArea(p, cx, cy, cw, ch);
	return;
    }

    if (collast < 0 || collast >= ncols)
	collast = ncols-1;
    if (rowlast < 0 || rowlast >= nrows)
	rowlast = nrows-1;

    // Go through the rows
    for (int r = rowfirst; r <= rowlast; ++r) {
	// get row position and height
	int rowp = r * cellh;

	// Go through the columns in the row r
	// if we know from where to where, go through [colfirst, collast],
	// else go through all of them
	for (int c = colfirst; c <= collast; ++c) {
	    // get position and width of column c
	    int colp = c * cellw;
	    // Translate painter and draw the cell
	    p->translate(colp, rowp);
	    paintCell(p, r, c);
	    p->translate(-colp, -rowp);
	}
    }

    // Paint empty rects
    paintEmptyArea(p, cx, cy, cw, ch);
}

/*!
  \reimp

  (Implemented to get rid of a compiler warning.)
*/
void Q3GridView::drawContents(QPainter *)
{
}

/*!
  \fn void Q3GridView::dimensionChange(int oldNumRows, int oldNumCols)

  This change handler is called whenever any of the grid's
  dimensions change. \a oldNumRows and \a oldNumCols contain the
  old dimensions, numRows() and numCols() contain the new
  dimensions.
*/
void Q3GridView::dimensionChange(int, int) {}



/*!
  \fn int Q3GridView::rowAt(int y) const

  Returns the number of the row at position \a y. \a y must be given
  in content coordinates.

  \sa columnAt()
*/

/*!
  \fn int Q3GridView::columnAt(int x) const

  Returns the number of the column at position \a x. \a x must be
  given in content coordinates.

  \sa rowAt()
*/

/*!
  \fn void Q3GridView::paintCell(QPainter *p, int row, int col)

  This pure virtual function is called to paint the single cell at
  (\a row, \a col) using painter \a p. The painter must be open when
  paintCell() is called and must remain open.

  The coordinate system is \link QPainter::translate() translated
  \endlink so that the origin is at the top-left corner of the cell
  to be painted, i.e. \e cell coordinates. Do not scale or shear
  the coordinate system (or if you do, restore the transformation
  matrix before you return).

  The painter is not clipped by default in order to get maximum
  efficiency. If you want clipping, use

  \snippet doc/src/snippets/code/src_qt3support_widgets_q3gridview.cpp 0
*/

/*!
  \fn  QRect Q3GridView::cellRect() const

  Returns the geometry of a cell in a cell's coordinate system. This
  is a convenience function useful in paintCell(). It is equivalent
  to QRect(0, 0, cellWidth(), cellHeight()).

  \sa cellGeometry()

*/

/*!
  \fn  QSize Q3GridView::gridSize() const

  Returns the size of the grid in pixels.
*/

QT_END_NAMESPACE
