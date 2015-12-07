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

#include "q3canvas.h"
#include "qapplication.h"
#include "qbitmap.h"
#include "qdesktopwidget.h"
#include "qimage.h"
#include "q3ptrdict.h"
#include "qpainter.h"
#include "q3polygonscanner.h"
#include "qtimer.h"
#include "q3tl.h"

#include <stdlib.h>

QT_BEGIN_NAMESPACE

using namespace Qt;

class Q3CanvasData {
public:
    Q3CanvasData() :
	itemDict(1013), animDict(503)
    {
    }

    Q3PtrList<Q3CanvasView> viewList;
    Q3PtrDict<void> itemDict;
    Q3PtrDict<void> animDict;
};

class Q3CanvasViewData {
public:
    Q3CanvasViewData() {}
#ifndef QT_NO_TRANSFORMATIONS
    QMatrix xform;
    QMatrix ixform;
#endif
    QRegion eraseRegion;
};

// clusterizer

class Q3CanvasClusterizer {
public:
    Q3CanvasClusterizer(int maxclusters);
    ~Q3CanvasClusterizer();

    void add(int x, int y); // 1x1 rectangle (point)
    void add(int x, int y, int w, int h);
    void add(const QRect& rect);

    void clear();
    int clusters() const { return count; }
    const QRect& operator[](int i) const;

private:
    QRect* cluster;
    int count;
    const int maxcl;
};

static
void include(QRect& r, const QRect& rect)
{
    if (rect.left()<r.left()) {
	    r.setLeft(rect.left());
    }
    if (rect.right()>r.right()) {
	    r.setRight(rect.right());
    }
    if (rect.top()<r.top()) {
	    r.setTop(rect.top());
    }
    if (rect.bottom()>r.bottom()) {
	    r.setBottom(rect.bottom());
    }
}

/*
A Q3CanvasClusterizer groups rectangles (QRects) into non-overlapping rectangles
by a merging heuristic.
*/
Q3CanvasClusterizer::Q3CanvasClusterizer(int maxclusters) :
    cluster(new QRect[maxclusters]),
    count(0),
    maxcl(maxclusters)
{ }

Q3CanvasClusterizer::~Q3CanvasClusterizer()
{
    delete [] cluster;
}

void Q3CanvasClusterizer::clear()
{
    count=0;
}

void Q3CanvasClusterizer::add(int x, int y)
{
    add(QRect(x,y,1,1));
}

void Q3CanvasClusterizer::add(int x, int y, int w, int h)
{
    add(QRect(x,y,w,h));
}

void Q3CanvasClusterizer::add(const QRect& rect)
{
    QRect biggerrect(rect.x()-1,rect.y()-1,rect.width()+2,rect.height()+2);

    //Q_ASSERT(rect.width()>0 && rect.height()>0);

    int cursor;

    for (cursor=0; cursor<count; cursor++) {
	if (cluster[cursor].contains(rect)) {
	    // Wholly contained already.
	    return;
	}
    }

    int lowestcost=9999999;
    int cheapest=-1;
    cursor = 0;
    while(cursor<count) {
	if (cluster[cursor].intersects(biggerrect)) {
	    QRect larger=cluster[cursor];
	    include(larger,rect);
	    int cost = larger.width()*larger.height() -
		       cluster[cursor].width()*cluster[cursor].height();

	    if (cost < lowestcost) {
		bool bad=false;
		for (int c=0; c<count && !bad; c++) {
		    bad=cluster[c].intersects(larger) && c!=cursor;
		}
		if (!bad) {
		    cheapest=cursor;
		    lowestcost=cost;
		}
	    }
	}
	cursor++;
    }

    if (cheapest>=0) {
	include(cluster[cheapest],rect);
	return;
    }

    if (count < maxcl) {
	cluster[count++]=rect;
	return;
    }

    // Do cheapest of:
    //     add to closest cluster
    //     do cheapest cluster merge, add to new cluster

    lowestcost=9999999;
    cheapest=-1;
    cursor=0;
    while(cursor<count) {
	QRect larger=cluster[cursor];
	include(larger,rect);
	int cost=larger.width()*larger.height()
		- cluster[cursor].width()*cluster[cursor].height();
	if (cost < lowestcost) {
	    bool bad=false;
	    for (int c=0; c<count && !bad; c++) {
		bad=cluster[c].intersects(larger) && c!=cursor;
	    }
	    if (!bad) {
		cheapest=cursor;
		lowestcost=cost;
	    }
	}
	cursor++;
    }

    // ###
    // could make an heuristic guess as to whether we need to bother
    // looking for a cheap merge.

    int cheapestmerge1 = -1;
    int cheapestmerge2 = -1;

    int merge1 = 0;
    while(merge1 < count) {
	int merge2=0;
	while(merge2 < count) {
	    if(merge1!=merge2) {
		QRect larger=cluster[merge1];
		include(larger,cluster[merge2]);
		int cost=larger.width()*larger.height()
		    - cluster[merge1].width()*cluster[merge1].height()
		    - cluster[merge2].width()*cluster[merge2].height();
		if (cost < lowestcost) {
		    bool bad=false;
		    for (int c=0; c<count && !bad; c++) {
			bad=cluster[c].intersects(larger) && c!=cursor;
		    }
		    if (!bad) {
			cheapestmerge1=merge1;
			cheapestmerge2=merge2;
			lowestcost=cost;
		    }
		}
	    }
	    merge2++;
	}
	merge1++;
    }

    if (cheapestmerge1>=0) {
	include(cluster[cheapestmerge1],cluster[cheapestmerge2]);
	cluster[cheapestmerge2]=cluster[count--];
    } else {
	// if (!cheapest) debugRectangles(rect);
	include(cluster[cheapest],rect);
    }

    // NB: clusters do not intersect (or intersection will
    //     overwrite). This is a result of the above algorithm,
    //     given the assumption that (x,y) are ordered topleft
    //     to bottomright.

    // ###
    //
    // add explicit x/y ordering to that comment, move it to the top
    // and rephrase it as pre-/post-conditions.
}

const QRect& Q3CanvasClusterizer::operator[](int i) const
{
    return cluster[i];
}

// end of clusterizer

// there's no more device coordinate clipping done, so introduce these
// clip setting compat functions

static void qt_setclipregion(QPainter *p, const QRegion &r)
{
    QMatrix matrix = p->worldMatrix();
    p->setWorldMatrix(QMatrix());
    p->setClipRegion(r);
    p->setWorldMatrix(matrix);
}

static void qt_setcliprect(QPainter *p, const QRect &r)
{
    qt_setclipregion(p, QRegion(r));
}


class Q_COMPAT_EXPORT Q3CanvasItemPtr {
public:
    Q3CanvasItemPtr() : ptr(0) { }
    Q3CanvasItemPtr(Q3CanvasItem* p) : ptr(p) { }

    bool operator<=(const Q3CanvasItemPtr& that) const
    {
	// Order same-z objects by identity.
	if (that.ptr->z()==ptr->z())
	    return that.ptr <= ptr;
	return that.ptr->z() <= ptr->z();
    }
    bool operator<(const Q3CanvasItemPtr& that) const
    {
	// Order same-z objects by identity.
	if (that.ptr->z()==ptr->z())
	    return that.ptr < ptr;
	return that.ptr->z() < ptr->z();
    }
    bool operator>(const Q3CanvasItemPtr& that) const
    {
	// Order same-z objects by identity.
	if (that.ptr->z()==ptr->z())
	    return that.ptr > ptr;
	return that.ptr->z() > ptr->z();
    }
    bool operator==(const Q3CanvasItemPtr& that) const
    {
	    return that.ptr == ptr;
    }
    operator Q3CanvasItem*() const { return ptr; }

private:
    Q3CanvasItem* ptr;
};


/*!
    \class Q3CanvasItemList
    \compat
    \brief The Q3CanvasItemList class is a list of Q3CanvasItems.

    Q3CanvasItemList is a Q3ValueList of pointers to \l{Q3CanvasItem}s.
    This class is used by some methods in Q3Canvas that need to return
    a list of canvas items.

    The \l Q3ValueList documentation describes how to use this list.

    \sa QtCanvas, {Porting to Graphics View}
*/

/*!
  \internal
*/
void Q3CanvasItemList::sort()
{
    qHeapSort(*((Q3ValueList<Q3CanvasItemPtr>*)this));
}

/*!
  \internal
*/
void Q3CanvasItemList::drawUnique(QPainter& painter)
{
    Q3CanvasItem* prev=0;
    for (Iterator it=fromLast(); it!=end(); --it) {
	Q3CanvasItem *g=*it;
	if (g!=prev) {
	    g->draw(painter);
	    prev=g;
	}
    }
}

/*!
    Returns the concatenation of this list and list \a l.
*/
Q3CanvasItemList Q3CanvasItemList::operator+(const Q3CanvasItemList &l) const
{
    Q3CanvasItemList l2(*this);
    for(const_iterator it = l.begin(); it != l.end(); ++it)
       l2.append(*it);
    return l2;
}

class Q3CanvasChunk {
public:
    Q3CanvasChunk() : changed(true) { }
    // Other code assumes lists are not deleted. Assignment is also
    // done on ChunkRecs. So don't add that sort of thing here.

    void sort()
    {
	list.sort();
    }

    const Q3CanvasItemList* listPtr() const
    {
	return &list;
    }

    void add(Q3CanvasItem* item)
    {
	list.prepend(item);
	changed = true;
    }

    void remove(Q3CanvasItem* item)
    {
	list.remove(item);
	changed = true;
    }

    void change()
    {
	changed = true;
    }

    bool hasChanged() const
    {
	return changed;
    }

    bool takeChange()
    {
	bool y = changed;
	changed = false;
	return y;
    }

private:
    Q3CanvasItemList list;
    bool changed;
};


static int gcd(int a, int b)
{
    int r;
    while ((r = a%b)) {
	a=b;
	b=r;
    }
    return b;
}

static int scm(int a, int b)
{
    int g = gcd(a,b);
    return a/g*b;
}



/*!
    \class Q3Canvas
    \compat
    \brief The Q3Canvas class provides a 2D area that can contain Q3CanvasItem objects.

    The Q3Canvas class manages its 2D graphic area and all the canvas
    items the area contains. The canvas has no visual appearance of
    its own. Instead, it is displayed on screen using a Q3CanvasView.
    Multiple Q3CanvasView widgets may be associated with a canvas to
    provide multiple views of the same canvas.

    The canvas is optimized for large numbers of items, particularly
    where only a small percentage of the items change at any
    one time. If the entire display changes very frequently, you should
    consider using your own custom Q3ScrollView subclass.

    Qt provides a rich
    set of canvas item classes, e.g. Q3CanvasEllipse, Q3CanvasLine,
    Q3CanvasPolygon, Q3CanvasPolygonalItem, Q3CanvasRectangle, Q3CanvasSpline,
    Q3CanvasSprite and Q3CanvasText. You can subclass to create your own
    canvas items; Q3CanvasPolygonalItem is the most common base class used
    for this purpose.

    Items appear on the canvas after their \link Q3CanvasItem::show()
    show()\endlink function has been called (or \link
    Q3CanvasItem::setVisible() setVisible(true)\endlink), and \e after
    update() has been called. The canvas only shows items that are
    \link Q3CanvasItem::setVisible() visible\endlink, and then only if
    \l update() is called. (By default the canvas is white and so are
    canvas items, so if nothing appears try changing colors.)

    If you created the canvas without passing a width and height to
    the constructor you must also call resize().

    Although a canvas may appear to be similar to a widget with child
    widgets, there are several notable differences:

    \list
    \i Canvas items are usually much faster to manipulate and redraw than
    child widgets, with the speed advantage becoming especially great when
    there are \e many canvas items and non-rectangular items. In most
    situations canvas items are also a lot more memory efficient than child
    widgets.

    \i It's easy to detect overlapping items (collision detection).

    \i The canvas can be larger than a widget. A million-by-million canvas
    is perfectly possible. At such a size a widget might be very
    inefficient, and some window systems might not support it at all,
    whereas Q3Canvas scales well. Even with a billion pixels and a million
    items, finding a particular canvas item, detecting collisions, etc.,
    is still fast (though the memory consumption may be prohibitive
    at such extremes).

    \i Two or more Q3CanvasView objects can view the same canvas.

    \i An arbitrary transformation matrix can be set on each Q3CanvasView
    which makes it easy to zoom, rotate or shear the viewed canvas.

    \i Widgets provide a lot more functionality, such as input (QKeyEvent,
    QMouseEvent etc.) and layout management (QGridLayout etc.).

    \endlist

    A canvas consists of a background, a number of canvas items organized by
    x, y and z coordinates, and a foreground. A canvas item's z coordinate
    can be treated as a layer number -- canvas items with a higher z
    coordinate appear in front of canvas items with a lower z coordinate.

    The background is white by default, but can be set to a different color
    using setBackgroundColor(), or to a repeated pixmap using
    setBackgroundPixmap() or to a mosaic of smaller pixmaps using
    setTiles(). Individual tiles can be set with setTile(). There
    are corresponding get functions, e.g. backgroundColor() and
    backgroundPixmap().

    Note that Q3Canvas does not inherit from QWidget, even though it has some
    functions which provide the same functionality as those in QWidget. One
    of these is setBackgroundPixmap(); some others are resize(), size(),
    width() and height(). \l Q3CanvasView is the widget used to display a
    canvas on the screen.

    Canvas items are added to a canvas by constructing them and passing the
    canvas to the canvas item's constructor. An item can be moved to a
    different canvas using Q3CanvasItem::setCanvas().

    Canvas items are movable (and in the case of Q3CanvasSprites, animated)
    objects that inherit Q3CanvasItem. Each canvas item has a position on the
    canvas (x, y coordinates) and a height (z coordinate), all of which are
    held as floating-point numbers. Moving canvas items also have x and y
    velocities. It's possible for a canvas item to be outside the canvas
    (for example Q3CanvasItem::x() is greater than width()). When a canvas
    item is off the canvas, onCanvas() returns false and the canvas
    disregards the item. (Canvas items off the canvas do not slow down any
    of the common operations on the canvas.)

    Canvas items can be moved with Q3CanvasItem::move(). The advance()
    function moves all Q3CanvasItem::animated() canvas items and
    setAdvancePeriod() makes Q3Canvas move them automatically on a periodic
    basis. In the context of the Q3Canvas classes, to `animate' a canvas item
    is to set it in motion, i.e. using Q3CanvasItem::setVelocity(). Animation
    of a canvas item itself, i.e. items which change over time, is enabled
    by calling Q3CanvasSprite::setFrameAnimation(), or more generally by
    subclassing and reimplementing Q3CanvasItem::advance(). To detect collisions
    use one of the Q3CanvasItem::collisions() functions.

    The changed parts of the canvas are redrawn (if they are visible in a
    canvas view) whenever update() is called. You can either call update()
    manually after having changed the contents of the canvas, or force
    periodic updates using setUpdatePeriod(). If you have moving objects on
    the canvas, you must call advance() every time the objects should
    move one step further. Periodic calls to advance() can be forced using
    setAdvancePeriod(). The advance() function will call
    Q3CanvasItem::advance() on every item that is \link
    Q3CanvasItem::animated() animated\endlink and trigger an update of the
    affected areas afterwards. (A canvas item that is `animated' is simply
    a canvas item that is in motion.)

    Q3Canvas organizes its canvas items into \e chunks; these are areas on
    the canvas that are used to speed up most operations. Many operations
    start by eliminating most chunks (i.e. those which haven't changed)
    and then process only the canvas items that are in the few interesting
    (i.e. changed) chunks. A valid chunk, validChunk(), is one which is on
    the canvas.

    The chunk size is a key factor to Q3Canvas's speed: if there are too many
    chunks, the speed benefit of grouping canvas items into chunks is
    reduced. If the chunks are too large, it takes too long to process each
    one. The Q3Canvas constructor tries to pick a suitable size, but you
    can call retune() to change it at any time. The chunkSize() function
    returns the current chunk size. The canvas items always make sure
    they're in the right chunks; all you need to make sure of is that
    the canvas uses the right chunk size. A good rule of thumb is that
    the size should be a bit smaller than the average canvas item
    size. If you have moving objects, the chunk size should be a bit
    smaller than the average size of the moving items.

    The foreground is normally nothing, but if you reimplement
    drawForeground(), you can draw things in front of all the canvas
    items.

    Areas can be set as changed with setChanged() and set unchanged with
    setUnchanged(). The entire canvas can be set as changed with
    setAllChanged(). A list of all the items on the canvas is returned by
    allItems().

    An area can be copied (painted) to a QPainter with drawArea().

    If the canvas is resized it emits the resized() signal.

    The examples/canvas application and the 2D graphics page of the
    examples/demo application demonstrate many of Q3Canvas's facilities.

    \sa Q3CanvasView Q3CanvasItem, QtCanvas, {Porting to Graphics View}
*/
void Q3Canvas::init(int w, int h, int chunksze, int mxclusters)
{
    d = new Q3CanvasData;
    awidth=w;
    aheight=h;
    chunksize=chunksze;
    maxclusters=mxclusters;
    chwidth=(w+chunksize-1)/chunksize;
    chheight=(h+chunksize-1)/chunksize;
    chunks=new Q3CanvasChunk[chwidth*chheight];
    update_timer = 0;
    bgcolor = white;
    grid = 0;
    htiles = 0;
    vtiles = 0;
    dblbuf = false;
    debug_redraw_areas = false;
}

/*!
    Create a Q3Canvas with no size. \a parent and \a name are passed to
    the QObject superclass.

    \warning You \e must call resize() at some time after creation to
    be able to use the canvas.
*/
Q3Canvas::Q3Canvas(QObject* parent, const char* name)
    : QObject(parent, name)
{
    init(0,0);
}

/*!
    Constructs a Q3Canvas that is \a w pixels wide and \a h pixels high.
*/
Q3Canvas::Q3Canvas(int w, int h)
{
    init(w,h);
}

/*!
    Constructs a Q3Canvas which will be composed of \a h tiles
    horizontally and \a v tiles vertically. Each tile will be an image
    \a tilewidth by \a tileheight pixels taken from pixmap \a p.

    The pixmap \a p is a list of tiles, arranged left to right, (and
    in the case of pixmaps that have multiple rows of tiles, top to
    bottom), with tile 0 in the top-left corner, tile 1 next to the
    right, and so on, e.g.

    \table
    \row \i 0 \i 1 \i 2 \i 3
    \row \i 4 \i 5 \i 6 \i 7
    \endtable

    The Q3Canvas is initially sized to show exactly the given number of
    tiles horizontally and vertically. If it is resized to be larger,
    the entire matrix of tiles will be repeated as often as necessary
    to cover the area. If it is smaller, tiles to the right and bottom
    will not be visible.

    \sa setTiles()
*/
Q3Canvas::Q3Canvas(QPixmap p,
	int h, int v, int tilewidth, int tileheight)
{
    init(h*tilewidth, v*tileheight, scm(tilewidth,tileheight));
    setTiles(p, h, v, tilewidth, tileheight);
}

void qt_unview(Q3Canvas* c)
{
    for (Q3CanvasView* view=c->d->viewList.first(); view != 0; view=c->d->viewList.next()) {
	view->viewing = 0;
    }
}

/*!
    Destroys the canvas and all the canvas's canvas items.
*/
Q3Canvas::~Q3Canvas()
{
    qt_unview(this);
    Q3CanvasItemList all = allItems();
    for (Q3CanvasItemList::Iterator it=all.begin(); it!=all.end(); ++it)
	delete *it;
    delete [] chunks;
    delete [] grid;
    delete d;
}

/*!
\internal
Returns the chunk at a chunk position \a i, \a j.
*/
Q3CanvasChunk& Q3Canvas::chunk(int i, int j) const
{
    return chunks[i+chwidth*j];
}

/*!
\internal
Returns the chunk at a pixel position \a x, \a y.
*/
Q3CanvasChunk& Q3Canvas::chunkContaining(int x, int y) const
{
    return chunk(x/chunksize,y/chunksize);
}

/*!
    Returns a list of all the items in the canvas.
*/
Q3CanvasItemList Q3Canvas::allItems()
{
    Q3CanvasItemList list;
    for (Q3PtrDictIterator<void> it=d->itemDict; it.currentKey(); ++it) {
	list.prepend((Q3CanvasItem*)it.currentKey());
    }
    return list;
}


/*!
    Changes the size of the canvas to have a width of \a w and a
    height of \a h. This is a slow operation.
*/
void Q3Canvas::resize(int w, int h)
{
    if (awidth==w && aheight==h)
	return;

    Q3CanvasItem* item;
    Q3PtrList<Q3CanvasItem> hidden;
    for (Q3PtrDictIterator<void> it=d->itemDict; it.currentKey(); ++it) {
	if (((Q3CanvasItem*)it.currentKey())->isVisible()) {
	    ((Q3CanvasItem*)it.currentKey())->hide();
	    hidden.append(((Q3CanvasItem*)it.currentKey()));
	}
    }

    int nchwidth=(w+chunksize-1)/chunksize;
    int nchheight=(h+chunksize-1)/chunksize;

    Q3CanvasChunk* newchunks = new Q3CanvasChunk[nchwidth*nchheight];

    // Commit the new values.
    //
    awidth=w;
    aheight=h;
    chwidth=nchwidth;
    chheight=nchheight;
    delete [] chunks;
    chunks=newchunks;

    for (item=hidden.first(); item != 0; item=hidden.next()) {
	item->show();
    }

    setAllChanged();

    emit resized();
}

/*!
    \fn void Q3Canvas::resized()

    This signal is emitted whenever the canvas is resized. Each
    Q3CanvasView connects to this signal to keep the scrollview's size
    correct.
*/

/*!
    Change the efficiency tuning parameters to \a mxclusters clusters,
    each of size \a chunksze. This is a slow operation if there are
    many objects on the canvas.

    The canvas is divided into chunks which are rectangular areas \a
    chunksze wide by \a chunksze high. Use a chunk size which is about
    the average size of the canvas items. If you choose a chunk size
    which is too small it will increase the amount of calculation
    required when drawing since each change will affect many chunks.
    If you choose a chunk size which is too large the amount of
    drawing required will increase because for each change, a lot of
    drawing will be required since there will be many (unchanged)
    canvas items which are in the same chunk as the changed canvas
    items.

    Internally, a canvas uses a low-resolution "chunk matrix" to keep
    track of all the items in the canvas. A 64x64 chunk matrix is the
    default for a 1024x1024 pixel canvas, where each chunk collects
    canvas items in a 16x16 pixel square. This default is also
    affected by setTiles(). You can tune this default using this
    function. For example if you have a very large canvas and want to
    trade off speed for memory then you might set the chunk size to 32
    or 64.

    The \a mxclusters argument is the number of rectangular groups of
    chunks that will be separately drawn. If the canvas has a large
    number of small, dispersed items, this should be about that
    number. Our testing suggests that a large number of clusters is
    almost always best.

*/
void Q3Canvas::retune(int chunksze, int mxclusters)
{
    maxclusters=mxclusters;

    if (chunksize!=chunksze) {
	Q3PtrList<Q3CanvasItem> hidden;
	for (Q3PtrDictIterator<void> it=d->itemDict; it.currentKey(); ++it) {
	    if (((Q3CanvasItem*)it.currentKey())->isVisible()) {
		((Q3CanvasItem*)it.currentKey())->hide();
		hidden.append(((Q3CanvasItem*)it.currentKey()));
	    }
	}

	chunksize=chunksze;

	int nchwidth=(awidth+chunksize-1)/chunksize;
	int nchheight=(aheight+chunksize-1)/chunksize;

	Q3CanvasChunk* newchunks = new Q3CanvasChunk[nchwidth*nchheight];

	// Commit the new values.
	//
	chwidth=nchwidth;
	chheight=nchheight;
	delete [] chunks;
	chunks=newchunks;

	for (Q3CanvasItem* item=hidden.first(); item != 0; item=hidden.next()) {
	    item->show();
	}
    }
}

/*!
    \fn int Q3Canvas::width() const

    Returns the width of the canvas, in pixels.
*/

/*!
    \fn int Q3Canvas::height() const

    Returns the height of the canvas, in pixels.
*/

/*!
    \fn QSize Q3Canvas::size() const

    Returns the size of the canvas, in pixels.
*/

/*!
    \fn QRect Q3Canvas::rect() const

    Returns a rectangle the size of the canvas.
*/


/*!
    \fn bool Q3Canvas::onCanvas(int x, int y) const

    Returns true if the pixel position (\a x, \a y) is on the canvas;
    otherwise returns false.

    \sa validChunk()
*/

/*!
    \fn bool Q3Canvas::onCanvas(const QPoint& p) const
    \overload

    Returns true if the pixel position \a p is on the canvas;
    otherwise returns false.

    \sa validChunk()
*/

/*!
    \fn bool Q3Canvas::validChunk(int x, int y) const

    Returns true if the chunk position (\a x, \a y) is on the canvas;
    otherwise returns false.

    \sa onCanvas()
*/

/*!
  \fn bool Q3Canvas::validChunk(const QPoint& p) const
  \overload

  Returns true if the chunk position \a p is on the canvas; otherwise
  returns false.

  \sa onCanvas()
*/

/*!
    \fn int Q3Canvas::chunkSize() const

    Returns the chunk size of the canvas.

    \sa retune()
*/

/*!
\fn bool Q3Canvas::sameChunk(int x1, int y1, int x2, int y2) const
\internal
Tells if the points (\a x1, \a y1) and (\a x2, \a y2) are within the same chunk.
*/

/*!
\internal
This method adds an the item \a item to the list of Q3CanvasItem objects
in the Q3Canvas. The Q3CanvasItem class calls this.
*/
void Q3Canvas::addItem(Q3CanvasItem* item)
{
    d->itemDict.insert((void*)item,(void*)1);
}

/*!
\internal
This method adds the item \a item to the list of Q3CanvasItem objects
to be moved. The Q3CanvasItem class calls this.
*/
void Q3Canvas::addAnimation(Q3CanvasItem* item)
{
    d->animDict.insert((void*)item,(void*)1);
}

/*!
\internal
This method adds the item \a item  to the list of Q3CanvasItem objects
which are no longer to be moved. The Q3CanvasItem class calls this.
*/
void Q3Canvas::removeAnimation(Q3CanvasItem* item)
{
    d->animDict.remove((void*)item);
}

/*!
\internal
This method removes the item \a item from the list of Q3CanvasItem objects
in this Q3Canvas. The Q3CanvasItem class calls this.
*/
void Q3Canvas::removeItem(Q3CanvasItem* item)
{
    d->itemDict.remove((void*)item);
}

/*!
\internal
This method adds the view \a view to the list of Q3CanvasView objects
viewing this Q3Canvas. The Q3CanvasView class calls this.
*/
void Q3Canvas::addView(Q3CanvasView* view)
{
    d->viewList.append(view);
    if (htiles>1 || vtiles>1 || pm.isNull())
	view->viewport()->setBackgroundColor(backgroundColor());
}

/*!
\internal
This method removes the view \a view from the list of Q3CanvasView objects
viewing this Q3Canvas. The Q3CanvasView class calls this.
*/
void Q3Canvas::removeView(Q3CanvasView* view)
{
    d->viewList.removeRef(view);
}

/*!
    Sets the canvas to call advance() every \a ms milliseconds. Any
    previous setting by setAdvancePeriod() or setUpdatePeriod() is
    overridden.

    If \a ms is less than 0 advancing will be stopped.
*/
void Q3Canvas::setAdvancePeriod(int ms)
{
    if (ms<0) {
	if (update_timer)
	    update_timer->stop();
    } else {
	if (update_timer)
	    delete update_timer;
	update_timer = new QTimer(this);
	connect(update_timer,SIGNAL(timeout()),this,SLOT(advance()));
	update_timer->start(ms);
    }
}

/*!
    Sets the canvas to call update() every \a ms milliseconds. Any
    previous setting by setAdvancePeriod() or setUpdatePeriod() is
    overridden.

    If \a ms is less than 0 automatic updating will be stopped.
*/
void Q3Canvas::setUpdatePeriod(int ms)
{
    if (ms<0) {
	if (update_timer)
	    update_timer->stop();
    } else {
	if (update_timer)
	    delete update_timer;
	update_timer = new QTimer(this);
	connect(update_timer,SIGNAL(timeout()),this,SLOT(update()));
	update_timer->start(ms);
    }
}

/*!
    Moves all Q3CanvasItem::animated() canvas items on the canvas and
    refreshes all changes to all views of the canvas. (An `animated'
    item is an item that is in motion; see setVelocity().)

    The advance takes place in two phases. In phase 0, the
    Q3CanvasItem::advance() function of each Q3CanvasItem::animated()
    canvas item is called with paramater 0. Then all these canvas
    items are called again, with parameter 1. In phase 0, the canvas
    items should not change position, merely examine other items on
    the canvas for which special processing is required, such as
    collisions between items. In phase 1, all canvas items should
    change positions, ignoring any other items on the canvas. This
    two-phase approach allows for considerations of "fairness",
    although no Q3CanvasItem subclasses supplied with Qt do anything
    interesting in phase 0.

    The canvas can be configured to call this function periodically
    with setAdvancePeriod().

    \sa update()
*/
void Q3Canvas::advance()
{
    Q3PtrDictIterator<void> it=d->animDict;
    while (it.current()) {
	Q3CanvasItem* i = (Q3CanvasItem*)(void*)it.currentKey();
	++it;
	if (i)
	    i->advance(0);
    }
    // we expect the dict contains the exact same items as in the
    // first pass.
    it.toFirst();
    while (it.current()) {
	Q3CanvasItem* i = (Q3CanvasItem*)(void*)it.currentKey();
	++it;
	if (i)
	    i->advance(1);
    }
    update();
}

// Don't call this unless you know what you're doing.
// p is in the content's co-ordinate example.
/*!
  \internal
*/
void Q3Canvas::drawViewArea(Q3CanvasView* view, QPainter* p, const QRect& vr, bool)
{
    QPoint tl = view->contentsToViewport(QPoint(0,0));

#ifndef QT_NO_TRANSFORMATIONS
    QMatrix wm = view->worldMatrix();
    QMatrix iwm = wm.invert();
    // ivr = covers all chunks in vr
    QRect ivr = iwm.map(vr);
    QMatrix twm;
    twm.translate(tl.x(),tl.y());
#else
    QRect ivr = vr;
#endif

    QRect all(0,0,width(),height());

    if (!all.contains(ivr)) {
	// Need to clip with edge of canvas.

#ifndef QT_NO_TRANSFORMATIONS
	// For translation-only transformation, it is safe to include the right
	// and bottom edges, but otherwise, these must be excluded since they
	// are not precisely defined (different bresenham paths).
	Q3PointArray a;
	if (wm.m12()==0.0 && wm.m21()==0.0 && wm.m11() == 1.0 && wm.m22() == 1.0)
	    a = Q3PointArray(QRect(all.x(),all.y(),all.width()+1,all.height()+1));
	else
	    a = Q3PointArray(all);

	a = (wm*twm).map(a);
#else
	Q3PointArray a(QRect(all.x(),all.y(),all.width()+1,all.height()+1));
#endif
	if (view->viewport()->backgroundMode() == NoBackground) {
	    QRect cvr = vr; cvr.moveBy(tl.x(),tl.y());
	    qt_setclipregion(p, QRegion(cvr)-QRegion(a));
	    p->fillRect(vr,view->viewport()->palette()
                        .brush(QPalette::Active,QPalette::Window));
	}
	qt_setclipregion(p, a);
    }

    QRect r = vr; r.moveBy(tl.x(),tl.y()); // move to untransformed co-ords
    if (!all.contains(ivr)) {
        QRegion inside = p->clipRegion() & r;
        //QRegion outside = p->clipRegion() - r;
        //p->setClipRegion(outside);
        //p->fillRect(outside.boundingRect(),red);
        qt_setclipregion(p, inside);
    } else {
        qt_setcliprect(p, r);
    }
#ifndef QT_NO_TRANSFORMATIONS
    p->setWorldMatrix(wm*twm);
#else
#endif
    drawCanvasArea(ivr,p,false);
}

/*!
    Repaints changed areas in all views of the canvas.

    \sa advance()
*/
void Q3Canvas::update()
{
    // ##### fix QT_NO_TRANSFORMATIONS
#ifndef QT_NO_TRANSFORMATIONS
    Q3PtrList<QRect> doneareas;
    doneareas.setAutoDelete(true);
#endif

    Q3PtrListIterator<Q3CanvasView> it(d->viewList);
    Q3CanvasView* view;
    while((view=it.current()) != 0) {
	++it;
#ifndef QT_NO_TRANSFORMATIONS
	QMatrix wm = view->worldMatrix();
#endif
	QRect area(view->contentsX(),view->contentsY(),
		   view->visibleWidth(),view->visibleHeight());
	if (area.width()>0 && area.height()>0) {
#ifndef QT_NO_TRANSFORMATIONS
            // r = Visible area of the canvas where there are changes
            QRect r = changeBounds(view->inverseWorldMatrix().map(area));
            if (!r.isEmpty()) {
                QRect tr = wm.map(r);
                tr.moveBy(-view->contentsX(), -view->contentsY());
                view->viewport()->update(tr);
                doneareas.append(new QRect(r));
            }
#endif
	}
    }

#ifndef QT_NO_TRANSFORMATIONS
    for (QRect* r=doneareas.first(); r != 0; r=doneareas.next())
	setUnchanged(*r);
#endif
}


/*!
    Marks the whole canvas as changed.
    All views of the canvas will be entirely redrawn when
    update() is called next.
*/
void Q3Canvas::setAllChanged()
{
    setChanged(QRect(0,0,width(),height()));
}

/*!
    Marks \a area as changed. This \a area will be redrawn in all
    views that are showing it when update() is called next.
*/
void Q3Canvas::setChanged(const QRect& area)
{
    QRect thearea = area.intersected(QRect(0, 0, width(), height()));

    int mx = (thearea.x()+thearea.width()+chunksize)/chunksize;
    int my = (thearea.y()+thearea.height()+chunksize)/chunksize;
    if (mx>chwidth)
	mx=chwidth;
    if (my>chheight)
	my=chheight;

    int x=thearea.x()/chunksize;
    while(x<mx) {
	int y = thearea.y()/chunksize;
	while(y<my) {
	    chunk(x,y).change();
	    y++;
	}
	x++;
    }
}

/*!
    Marks \a area as \e unchanged. The area will \e not be redrawn in
    the views for the next update(), unless it is marked or changed
    again before the next call to update().
*/
void Q3Canvas::setUnchanged(const QRect& area)
{
    QRect thearea = area.intersected(QRect(0, 0, width(), height()));

    int mx = (thearea.x()+thearea.width()+chunksize)/chunksize;
    int my = (thearea.y()+thearea.height()+chunksize)/chunksize;
    if (mx>chwidth)
	mx=chwidth;
    if (my>chheight)
	my=chheight;

    int x=thearea.x()/chunksize;
    while(x<mx) {
	int y = thearea.y()/chunksize;
	while(y<my) {
	    chunk(x,y).takeChange();
	    y++;
	}
	x++;
    }
}


/*!
  \internal
*/
QRect Q3Canvas::changeBounds(const QRect& inarea)
{
    QRect area = inarea.intersected(QRect(0, 0, width(), height()));

    int mx = (area.x()+area.width()+chunksize)/chunksize;
    int my = (area.y()+area.height()+chunksize)/chunksize;
    if (mx > chwidth)
	mx=chwidth;
    if (my > chheight)
	my=chheight;

    QRect result;

    int x=area.x()/chunksize;
    while(x<mx) {
	int y=area.y()/chunksize;
	while(y<my) {
	    Q3CanvasChunk& ch=chunk(x,y);
	    if (ch.hasChanged())
		result |= QRect(x,y,1,1);
	    y++;
	}
	x++;
    }

    if (!result.isEmpty()) {
	result.rLeft() *= chunksize;
	result.rTop() *= chunksize;
	result.rRight() *= chunksize;
	result.rBottom() *= chunksize;
	result.rRight() += chunksize;
	result.rBottom() += chunksize;
    }

    return result;
}

void Q3Canvas::ensureOffScrSize(int osw, int osh)
{
    if (osw > offscr.width() || osh > offscr.height())
	offscr.resize(QMAX(osw,offscr.width()),
		      QMAX(osh,offscr.height()));
    else if (offscr.width() == 0 || offscr.height() == 0)
	offscr.resize(QMAX(offscr.width(), 1),
		       QMAX(offscr.height(), 1));
}

/*!
    Paints all canvas items that are in the area \a clip to \a
    painter, using double-buffering if \a dbuf is true.

    e.g. to print the canvas to a printer:
    \snippet doc/src/snippets/code/src_qt3support_canvas_q3canvas.cpp 0
*/
void Q3Canvas::drawArea(const QRect& clip, QPainter* painter, bool dbuf)
{
    if (painter)
	drawCanvasArea(clip, painter, dbuf);
}

QT_BEGIN_INCLUDE_NAMESPACE
#include <qdebug.h>
QT_END_INCLUDE_NAMESPACE

/*!
  \internal
*/
void Q3Canvas::drawCanvasArea(const QRect& inarea, QPainter* p, bool /*double_buffer*/)
{
    QRect area=inarea.intersected(QRect(0,0,width(),height()));

    if (!p) return; // Nothing to do.

    int lx=area.x()/chunksize;
    int ly=area.y()/chunksize;
    int mx=area.right()/chunksize;
    int my=area.bottom()/chunksize;
    if (mx>=chwidth)
	mx=chwidth-1;
    if (my>=chheight)
	my=chheight-1;

    Q3CanvasItemList allvisible;

    // Stores the region within area that need to be drawn. It is relative
    // to area.topLeft()  (so as to keep within bounds of 16-bit XRegions)
    QRegion rgn;

    for (int x=lx; x<=mx; x++) {
	for (int y=ly; y<=my; y++) {
	    // Only reset change if all views updating, and
	    // wholy within area. (conservative:  ignore entire boundary)
	    //
	    // Disable this to help debugging.
	    //
	    if (!p) {
		if (chunk(x,y).takeChange()) {
		    // ### should at least make bands
		    rgn |= QRegion(x*chunksize-area.x(),y*chunksize-area.y(),
                                   chunksize,chunksize);
		    allvisible += *chunk(x,y).listPtr();
		}
	    } else {
		allvisible += *chunk(x,y).listPtr();
	    }
	}
    }
    allvisible.sort();

    drawBackground(*p,area);
    allvisible.drawUnique(*p);
    drawForeground(*p,area);
}

/*!
\internal
This method to informs the Q3Canvas that a given chunk is
`dirty' and needs to be redrawn in the next Update.

(\a x,\a y) is a chunk location.

The sprite classes call this. Any new derived class of Q3CanvasItem
must do so too. SetChangedChunkContaining can be used instead.
*/
void Q3Canvas::setChangedChunk(int x, int y)
{
    if (validChunk(x,y)) {
	Q3CanvasChunk& ch=chunk(x,y);
	ch.change();
    }
}

/*!
\internal
This method to informs the Q3Canvas that the chunk containing a given
pixel is `dirty' and needs to be redrawn in the next Update.

(\a x,\a y) is a pixel location.

The item classes call this. Any new derived class of Q3CanvasItem must
do so too. SetChangedChunk can be used instead.
*/
void Q3Canvas::setChangedChunkContaining(int x, int y)
{
    if (x>=0 && x<width() && y>=0 && y<height()) {
	Q3CanvasChunk& chunk=chunkContaining(x,y);
	chunk.change();
    }
}

/*!
\internal
This method adds the Q3CanvasItem \a g to the list of those which need to be
drawn if the given chunk at location (\a x, \a y) is redrawn. Like
SetChangedChunk and SetChangedChunkContaining, this method marks the
chunk as `dirty'.
*/
void Q3Canvas::addItemToChunk(Q3CanvasItem* g, int x, int y)
{
    if (validChunk(x,y)) {
	chunk(x,y).add(g);
    }
}

/*!
\internal
This method removes the Q3CanvasItem \a g from the list of those which need to
be drawn if the given chunk at location (\a x, \a y) is redrawn. Like
SetChangedChunk and SetChangedChunkContaining, this method marks the chunk
as `dirty'.
*/
void Q3Canvas::removeItemFromChunk(Q3CanvasItem* g, int x, int y)
{
    if (validChunk(x,y)) {
	chunk(x,y).remove(g);
    }
}


/*!
\internal
This method adds the Q3CanvasItem \a g to the list of those which need to be
drawn if the chunk containing the given pixel (\a x, \a y) is redrawn. Like
SetChangedChunk and SetChangedChunkContaining, this method marks the
chunk as `dirty'.
*/
void Q3Canvas::addItemToChunkContaining(Q3CanvasItem* g, int x, int y)
{
    if (x>=0 && x<width() && y>=0 && y<height()) {
	chunkContaining(x,y).add(g);
    }
}

/*!
\internal
This method removes the Q3CanvasItem \a g from the list of those which need to
be drawn if the chunk containing the given pixel (\a x, \a y) is redrawn.
Like SetChangedChunk and SetChangedChunkContaining, this method
marks the chunk as `dirty'.
*/
void Q3Canvas::removeItemFromChunkContaining(Q3CanvasItem* g, int x, int y)
{
    if (x>=0 && x<width() && y>=0 && y<height()) {
	chunkContaining(x,y).remove(g);
    }
}

/*!
    Returns the color set by setBackgroundColor(). By default, this is
    white.

    This function is not a reimplementation of
    QWidget::backgroundColor() (Q3Canvas is not a subclass of QWidget),
    but all Q3CanvasViews that are viewing the canvas will set their
    backgrounds to this color.

    \sa setBackgroundColor(), backgroundPixmap()
*/
QColor Q3Canvas::backgroundColor() const
{
    return bgcolor;
}

/*!
    Sets the solid background to be the color \a c.

    \sa backgroundColor(), setBackgroundPixmap(), setTiles()
*/
void Q3Canvas::setBackgroundColor(const QColor& c)
{
    if (bgcolor != c) {
	bgcolor = c;
	Q3CanvasView* view=d->viewList.first();
	while (view != 0) {
	    /* XXX this doesn't look right. Shouldn't this
	       be more like setBackgroundPixmap? : Ian */
	    view->viewport()->setEraseColor(bgcolor);
	    view=d->viewList.next();
	}
	setAllChanged();
    }
}

/*!
    Returns the pixmap set by setBackgroundPixmap(). By default,
    this is a null pixmap.

    \sa setBackgroundPixmap(), backgroundColor()
*/
QPixmap Q3Canvas::backgroundPixmap() const
{
    return pm;
}

/*!
    Sets the solid background to be the pixmap \a p repeated as
    necessary to cover the entire canvas.

    \sa backgroundPixmap(), setBackgroundColor(), setTiles()
*/
void Q3Canvas::setBackgroundPixmap(const QPixmap& p)
{
    setTiles(p, 1, 1, p.width(), p.height());
    Q3CanvasView* view = d->viewList.first();
    while (view != 0) {
	view->updateContents();
	view = d->viewList.next();
    }
}

/*!
    This virtual function is called for all updates of the canvas. It
    renders any background graphics using the painter \a painter, in
    the area \a clip. If the canvas has a background pixmap or a tiled
    background, that graphic is used, otherwise the canvas is cleared
    using the background color.

    If the graphics for an area change, you must explicitly call
    setChanged(const QRect&) for the result to be visible when
    update() is next called.

    \sa setBackgroundColor(), setBackgroundPixmap(), setTiles()
*/
void Q3Canvas::drawBackground(QPainter& painter, const QRect& clip)
{
    if (pm.isNull()) {
	painter.fillRect(clip,bgcolor);
    } else if (!grid) {
	for (int x=clip.x()/pm.width();
	    x<(clip.x()+clip.width()+pm.width()-1)/pm.width(); x++)
	{
	    for (int y=clip.y()/pm.height();
		y<(clip.y()+clip.height()+pm.height()-1)/pm.height(); y++)
	    {
		painter.drawPixmap(x*pm.width(), y*pm.height(),pm);
	    }
	}
    } else {
	const int x1 = clip.left()/tilew;
	int x2 = clip.right()/tilew;
	const int y1 = clip.top()/tileh;
	int y2 = clip.bottom()/tileh;

	const int roww = pm.width()/tilew;

	for (int j=y1; j<=y2; j++) {
	    int jj = j%tilesVertically();
	    for (int i=x1; i<=x2; i++) {
		int t = tile(i%tilesHorizontally(), jj);
		int tx = t % roww;
		int ty = t / roww;
		painter.drawPixmap(i*tilew, j*tileh, pm,
				tx*tilew, ty*tileh, tilew, tileh);
	    }
	}
    }
}

/*!
    This virtual function is called for all updates of the canvas. It
    renders any foreground graphics using the painter \a painter, in
    the area \a clip.

    If the graphics for an area change, you must explicitly call
    setChanged(const QRect&) for the result to be visible when
    update() is next called.

    The default is to draw nothing.
*/
void Q3Canvas::drawForeground(QPainter& painter, const QRect& clip)
{
    if (debug_redraw_areas) {
	painter.setPen(red);
	painter.setBrush(NoBrush);
	painter.drawRect(clip);
    }
}

/*!
    If \a y is true (the default) double-buffering is switched on;
    otherwise double-buffering is switched off.

    Turning off double-buffering causes the redrawn areas to flicker a
    little and also gives a (usually small) performance improvement.
*/
void Q3Canvas::setDoubleBuffering(bool y)
{
    dblbuf = y;
}


/*!
    Sets the Q3Canvas to be composed of \a h tiles horizontally and \a
    v tiles vertically. Each tile will be an image \a tilewidth by \a
    tileheight pixels from pixmap \a p.

    The pixmap \a p is a list of tiles, arranged left to right, (and
    in the case of pixmaps that have multiple rows of tiles, top to
    bottom), with tile 0 in the top-left corner, tile 1 next to the
    right, and so on, e.g.

    \table
    \row \i 0 \i 1 \i 2 \i 3
    \row \i 4 \i 5 \i 6 \i 7
    \endtable

    If the canvas is larger than the matrix of tiles, the entire
    matrix is repeated as necessary to cover the whole canvas. If it
    is smaller, tiles to the right and bottom are not visible.

    The width and height of \a p must be a multiple of \a tilewidth
    and \a tileheight. If they are not the function will do nothing.

    If you want to unset any tiling set, then just pass in a null
    pixmap and 0 for \a h, \a v, \a tilewidth, and
    \a tileheight.
*/
void Q3Canvas::setTiles(QPixmap p,
			int h, int v, int tilewidth, int tileheight)
{
    if (!p.isNull() && (!tilewidth || !tileheight ||
	 p.width() % tilewidth != 0 || p.height() % tileheight != 0))
    	return;

    htiles = h;
    vtiles = v;
    delete[] grid;
    pm = p;
    if (h && v && !p.isNull()) {
	grid = new ushort[h*v];
	memset(grid, 0, h*v*sizeof(ushort));
	tilew = tilewidth;
	tileh = tileheight;
    } else {
	grid = 0;
    }
    if (h + v > 10) {
	int s = scm(tilewidth,tileheight);
	retune(s < 128 ? s : QMAX(tilewidth,tileheight));
    }
    setAllChanged();
}

/*!
    \fn int Q3Canvas::tile(int x, int y) const

    Returns the tile at position (\a x, \a y). Initially, all tiles
    are 0.

    The parameters must be within range, i.e.
	0 \< \a x \< tilesHorizontally() and
	0 \< \a y \< tilesVertically().

    \sa setTile()
*/

/*!
    \fn int Q3Canvas::tilesHorizontally() const

    Returns the number of tiles horizontally.
*/

/*!
    \fn int Q3Canvas::tilesVertically() const

    Returns the number of tiles vertically.
*/

/*!
    \fn int Q3Canvas::tileWidth() const

    Returns the width of each tile.
*/

/*!
    \fn int Q3Canvas::tileHeight() const

    Returns the height of each tile.
*/


/*!
    Sets the tile at (\a x, \a y) to use tile number \a tilenum, which
    is an index into the tile pixmaps. The canvas will update
    appropriately when update() is next called.

    The images are taken from the pixmap set by setTiles() and are
    arranged left to right, (and in the case of pixmaps that have
    multiple rows of tiles, top to bottom), with tile 0 in the
    top-left corner, tile 1 next to the right, and so on, e.g.

    \table
    \row \i 0 \i 1 \i 2 \i 3
    \row \i 4 \i 5 \i 6 \i 7
    \endtable

    \sa tile() setTiles()
*/
void Q3Canvas::setTile(int x, int y, int tilenum)
{
    ushort& t = grid[x+y*htiles];
    if (t != tilenum) {
	t = tilenum;
	if (tilew == tileh && tilew == chunksize)
	    setChangedChunk(x, y);	    // common case
	else
	    setChanged(QRect(x*tilew,y*tileh,tilew,tileh));
    }
}


// lesser-used data in canvas item, plus room for extension.
// Be careful adding to this - check all usages.
class Q3CanvasItemExtra {
    Q3CanvasItemExtra() : vx(0.0), vy(0.0) { }
    double vx,vy;
    friend class Q3CanvasItem;
};


/*!
    \class Q3CanvasItem qcanvas.h
    \compat
    \brief The Q3CanvasItem class provides an abstract graphic object on a Q3Canvas.

    A variety of Q3CanvasItem subclasses provide immediately usable
    behaviour. This class is a pure abstract superclass providing the
    behaviour that is shared among all the concrete canvas item classes.
    Q3CanvasItem is not intended for direct subclassing. It is much easier
    to subclass one of its subclasses, e.g. Q3CanvasPolygonalItem (the
    commonest base class), Q3CanvasRectangle, Q3CanvasSprite, Q3CanvasEllipse
    or Q3CanvasText.

    Canvas items are added to a canvas by constructing them and passing the
    canvas to the canvas item's constructor. An item can be moved to a
    different canvas using setCanvas().

    Items appear on the canvas after their \link show() show()\endlink
    function has been called (or \link setVisible()
    setVisible(true)\endlink), and \e after update() has been called. The
    canvas only shows items that are \link setVisible() visible\endlink,
    and then only if \l update() is called. If you created the canvas
    without passing a width and height to the constructor you'll also need
    to call \link Q3Canvas::resize() resize()\endlink. Since the canvas
    background defaults to white and canvas items default to white,
    you may need to change colors to see your items.

    A Q3CanvasItem object can be moved in the x(), y() and z() dimensions
    using functions such as move(), moveBy(), setX(), setY() and setZ(). A
    canvas item can be set in motion, `animated', using setAnimated() and
    given a velocity in the x and y directions with setXVelocity() and
    setYVelocity() -- the same effect can be achieved by calling
    setVelocity(). Use the collidesWith() function to see if the canvas item
    will collide on the \e next advance(1) and use collisions() to see what
    collisions have occurred.

    Use Q3CanvasSprite or your own subclass of Q3CanvasSprite to create canvas
    items which are animated, i.e. which change over time.

    The size of a canvas item is given by boundingRect(). Use
    boundingRectAdvanced() to see what the size of the canvas item will be
    \e after the next advance(1) call.

    The rtti() function is used for identifying subclasses of Q3CanvasItem.
    The canvas() function returns a pointer to the canvas which contains the
    canvas item.

    Q3CanvasItem provides the show() and isVisible() functions like those in
    QWidget.

    Q3CanvasItem also provides the setEnabled(), setActive() and
    setSelected() functions; these functions set the relevant boolean and
    cause a repaint but the boolean values they set are not used in
    Q3CanvasItem itself. You can make use of these booleans in your subclasses.

    By default, canvas items have no velocity, no size, and are not in
    motion. The subclasses provided in Qt do not change these defaults
    except where noted.

    \sa QtCanvas, {Porting to Graphics View}
*/

/*!
    \enum Q3CanvasItem::RttiValues

    This enum is used to name the different types of canvas item.

    \value Rtti_Item Canvas item abstract base class
    \value Rtti_Ellipse
    \value Rtti_Line
    \value Rtti_Polygon
    \value Rtti_PolygonalItem
    \value Rtti_Rectangle
    \value Rtti_Spline
    \value Rtti_Sprite
    \value Rtti_Text

*/

/*!
    \fn void Q3CanvasItem::update()

    Call this function to repaint the canvas's changed chunks.
*/

/*!
    Constructs a Q3CanvasItem on canvas \a canvas.

    \sa setCanvas()
*/
Q3CanvasItem::Q3CanvasItem(Q3Canvas* canvas) :
    cnv(canvas),
    myx(0),myy(0),myz(0)
{
    ani=0;
    vis=0;
    val=0;
    sel=0;
    ena=0;
    act=0;

    ext = 0;
    if (cnv) cnv->addItem(this);
}

/*!
    Destroys the Q3CanvasItem and removes it from its canvas.
*/
Q3CanvasItem::~Q3CanvasItem()
{
    if (cnv) {
	cnv->removeItem(this);
	cnv->removeAnimation(this);
    }
    delete ext;
}

Q3CanvasItemExtra& Q3CanvasItem::extra()
{
    if (!ext)
	ext = new Q3CanvasItemExtra;
    return *ext;
}

/*!
    \fn double Q3CanvasItem::x() const

    Returns the horizontal position of the canvas item. Note that
    subclasses often have an origin other than the top-left corner.
*/

/*!
    \fn double Q3CanvasItem::y() const

    Returns the vertical position of the canvas item. Note that
    subclasses often have an origin other than the top-left corner.
*/

/*!
    \fn double Q3CanvasItem::z() const

    Returns the z index of the canvas item, which is used for visual
    order: higher-z items obscure (are in front of) lower-z items.
*/

/*!
    \fn void Q3CanvasItem::setX(double x)

    Moves the canvas item so that its x-position is \a x.

    \sa x(), move()
*/

/*!
    \fn void Q3CanvasItem::setY(double y)

    Moves the canvas item so that its y-position is \a y.

    \sa y(), move()
*/

/*!
    \fn void Q3CanvasItem::setZ(double z)

    Sets the z index of the canvas item to \a z. Higher-z items
    obscure (are in front of) lower-z items.

    \sa z(), move()
*/


/*!
    Moves the canvas item relative to its current position by (\a dx,
    \a dy).
*/
void Q3CanvasItem::moveBy(double dx, double dy)
{
    if (dx || dy) {
	removeFromChunks();
	myx += dx;
	myy += dy;
	addToChunks();
    }
}


/*!
    Moves the canvas item to the absolute position (\a x, \a y).
*/
void Q3CanvasItem::move(double x, double y)
{
    moveBy(x-myx, y-myy);
}


/*!
    Returns true if the canvas item is in motion; otherwise returns
    false.

    \sa setVelocity(), setAnimated()
*/
bool Q3CanvasItem::animated() const
{
    return (bool)ani;
}

/*!
    Sets the canvas item to be in motion if \a y is true, or not if \a
    y is false. The speed and direction of the motion is set with
    setVelocity(), or with setXVelocity() and setYVelocity().

    \sa advance(), Q3Canvas::advance()
*/
void Q3CanvasItem::setAnimated(bool y)
{
    if (y != (bool)ani) {
	ani = (uint)y;
	if (y) {
	    cnv->addAnimation(this);
	} else {
	    cnv->removeAnimation(this);
	}
    }
}

/*!
    \fn void Q3CanvasItem::setXVelocity(double vx)

    Sets the horizontal component of the canvas item's velocity to \a vx.

    \sa setYVelocity() setVelocity()
*/

/*!
    \fn void Q3CanvasItem::setYVelocity(double vy)

    Sets the vertical component of the canvas item's velocity to \a vy.

    \sa setXVelocity() setVelocity()
*/

/*!
    Sets the canvas item to be in motion, moving by \a vx and \a vy
    pixels in the horizontal and vertical directions respectively.

    \sa advance() setXVelocity() setYVelocity()
*/
void Q3CanvasItem::setVelocity(double vx, double vy)
{
    if (ext || vx!=0.0 || vy!=0.0) {
	if (!ani)
	    setAnimated(true);
	extra().vx = vx;
	extra().vy = vy;
    }
}

/*!
    Returns the horizontal velocity component of the canvas item.
*/
double Q3CanvasItem::xVelocity() const
{
    return ext ? ext->vx : 0;
}

/*!
    Returns the vertical velocity component of the canvas item.
*/
double Q3CanvasItem::yVelocity() const
{
    return ext ? ext->vy : 0;
}

/*!
    The default implementation moves the canvas item, if it is
    animated(), by the preset velocity if \a phase is 1, and does
    nothing if \a phase is 0.

    Note that if you reimplement this function, the reimplementation
    must not change the canvas in any way, for example it must not add
    or remove items.

    \sa Q3Canvas::advance() setVelocity()
*/
void Q3CanvasItem::advance(int phase)
{
    if (ext && phase==1)
	moveBy(ext->vx,ext->vy);
}

/*!
    \fn void Q3CanvasItem::draw(QPainter& painter)

    This abstract virtual function draws the canvas item using \a painter.
*/

/*!
    Sets the Q3Canvas upon which the canvas item is to be drawn to \a c.

    \sa canvas()
*/
void Q3CanvasItem::setCanvas(Q3Canvas* c)
{
    bool v=isVisible();
    setVisible(false);
    if (cnv) {
	if (ext)
	    cnv->removeAnimation(this);
	cnv->removeItem(this);
    }
    cnv=c;
    if (cnv) {
	cnv->addItem(this);
	if (ext)
	    cnv->addAnimation(this);
    }
    setVisible(v);
}

/*!
    \fn Q3Canvas* Q3CanvasItem::canvas() const

    Returns the canvas containing the canvas item.
*/

/*! Shorthand for setVisible(true). */
void Q3CanvasItem::show()
{
    setVisible(true);
}

/*! Shorthand for setVisible(false). */
void Q3CanvasItem::hide()
{
    setVisible(false);
}

/*!
    Makes the canvas item visible if \a yes is true, or invisible if
    \a yes is false. The change takes effect when Q3Canvas::update() is
    next called.
*/
void Q3CanvasItem::setVisible(bool yes)
{
    if ((bool)vis!=yes) {
	if (yes) {
	    vis=(uint)yes;
	    addToChunks();
	} else {
	    removeFromChunks();
	    vis=(uint)yes;
	}
    }
}
/*!
    \obsolete
    \fn bool Q3CanvasItem::visible() const
    Use isVisible() instead.
*/

/*!
    \fn bool Q3CanvasItem::isVisible() const

    Returns true if the canvas item is visible; otherwise returns
    false.

    Note that in this context true does \e not mean that the canvas
    item is currently in a view, merely that if a view is showing the
    area where the canvas item is positioned, and the item is not
    obscured by items with higher z values, and the view is not
    obscured by overlaying windows, it would be visible.

    \sa setVisible(), z()
*/

/*!
    \obsolete
    \fn bool Q3CanvasItem::selected() const
    Use isSelected() instead.
*/

/*!
    \fn bool Q3CanvasItem::isSelected() const

    Returns true if the canvas item is selected; otherwise returns false.
*/

/*!
    Sets the selected flag of the item to \a yes. If this changes the
    item's selected state the item will be redrawn when
    Q3Canvas::update() is next called.

    The Q3Canvas, Q3CanvasItem and the Qt-supplied Q3CanvasItem
    subclasses do not make use of this value. The setSelected()
    function is supplied because many applications need it, but it is
    up to you how you use the isSelected() value.
*/
void Q3CanvasItem::setSelected(bool yes)
{
    if ((bool)sel!=yes) {
	sel=(uint)yes;
	changeChunks();
    }
}

/*!
    \obsolete
    \fn bool Q3CanvasItem::enabled() const
    Use isEnabled() instead.
*/

/*!
    \fn bool Q3CanvasItem::isEnabled() const

    Returns true if the Q3CanvasItem is enabled; otherwise returns false.
*/

/*!
    Sets the enabled flag of the item to \a yes. If this changes the
    item's enabled state the item will be redrawn when
    Q3Canvas::update() is next called.

    The Q3Canvas, Q3CanvasItem and the Qt-supplied Q3CanvasItem
    subclasses do not make use of this value. The setEnabled()
    function is supplied because many applications need it, but it is
    up to you how you use the isEnabled() value.
*/
void Q3CanvasItem::setEnabled(bool yes)
{
    if (ena!=(uint)yes) {
	ena=(uint)yes;
	changeChunks();
    }
}

/*!
    \obsolete
    \fn bool Q3CanvasItem::active() const
    Use isActive() instead.
*/

/*!
    \fn bool Q3CanvasItem::isActive() const

    Returns true if the Q3CanvasItem is active; otherwise returns false.
*/

/*!
    Sets the active flag of the item to \a yes. If this changes the
    item's active state the item will be redrawn when
    Q3Canvas::update() is next called.

    The Q3Canvas, Q3CanvasItem and the Qt-supplied Q3CanvasItem
    subclasses do not make use of this value. The setActive() function
    is supplied because many applications need it, but it is up to you
    how you use the isActive() value.
*/
void Q3CanvasItem::setActive(bool yes)
{
    if (act!=(uint)yes) {
	act=(uint)yes;
	changeChunks();
    }
}

bool qt_testCollision(const Q3CanvasSprite* s1, const Q3CanvasSprite* s2)
{
    const QImage* s2image = s2->imageAdvanced()->collision_mask;
    QRect s2area = s2->boundingRectAdvanced();

    QRect cyourarea(s2area.x(),s2area.y(),
	    s2area.width(),s2area.height());

    QImage* s1image=s1->imageAdvanced()->collision_mask;

    QRect s1area = s1->boundingRectAdvanced();

    QRect ourarea = s1area.intersected(cyourarea);

    if (ourarea.isEmpty())
	return false;

    int x2=ourarea.x()-cyourarea.x();
    int y2=ourarea.y()-cyourarea.y();
    int x1=ourarea.x()-s1area.x();
    int y1=ourarea.y()-s1area.y();
    int w=ourarea.width();
    int h=ourarea.height();

    if (!s2image) {
	if (!s1image)
	    return w>0 && h>0;
	// swap everything around
	int t;
	t=x1; x1=x2; x2=t;
	t=y1; x1=y2; y2=t;
	s2image = s1image;
	s1image = 0;
    }

    // s2image != 0

    // A non-linear search may be more efficient.
    // Perhaps spiralling out from the center, or a simpler
    // vertical expansion from the centreline.

    // We assume that sprite masks don't have
    // different bit orders.
    //
    // Q_ASSERT(s1image->bitOrder()==s2image->bitOrder());

    if (s1image) {
	if (s1image->bitOrder() == QImage::LittleEndian) {
	    for (int j=0; j<h; j++) {
		uchar* ml = s1image->scanLine(y1+j);
		const uchar* yl = s2image->scanLine(y2+j);
		for (int i=0; i<w; i++) {
		    if (*(yl + ((x2+i) >> 3)) & (1 << ((x2+i) & 7))
		    && *(ml + ((x1+i) >> 3)) & (1 << ((x1+i) & 7)))
		    {
			return true;
		    }
		}
	    }
	} else {
	    for (int j=0; j<h; j++) {
		uchar* ml = s1image->scanLine(y1+j);
		const uchar* yl = s2image->scanLine(y2+j);
		for (int i=0; i<w; i++) {
		    if (*(yl + ((x2+i) >> 3)) & (1 << (7-((x2+i) & 7)))
		    && *(ml + ((x1+i) >> 3)) & (1 << (7-((x1+i) & 7))))
		    {
			return true;
		    }
		}
	    }
	}
    } else {
	if (s2image->bitOrder() == QImage::LittleEndian) {
	    for (int j=0; j<h; j++) {
		const uchar* yl = s2image->scanLine(y2+j);
		for (int i=0; i<w; i++) {
		    if (*(yl + ((x2+i) >> 3)) & (1 << ((x2+i) & 7)))
		    {
			return true;
		    }
		}
	    }
	} else {
	    for (int j=0; j<h; j++) {
		const uchar* yl = s2image->scanLine(y2+j);
		for (int i=0; i<w; i++) {
		    if (*(yl + ((x2+i) >> 3)) & (1 << (7-((x2+i) & 7))))
		    {
			return true;
		    }
		}
	    }
	}
    }

    return false;
}

static bool collision_double_dispatch(const Q3CanvasSprite* s1,
				       const Q3CanvasPolygonalItem* p1,
				       const Q3CanvasRectangle* r1,
				       const Q3CanvasEllipse* e1,
				       const Q3CanvasText* t1,
				       const Q3CanvasSprite* s2,
				       const Q3CanvasPolygonalItem* p2,
				       const Q3CanvasRectangle* r2,
				       const Q3CanvasEllipse* e2,
				       const Q3CanvasText* t2)
{
    const Q3CanvasItem* i1 = s1 ?
			    (const Q3CanvasItem*)s1 : p1 ?
			    (const Q3CanvasItem*)p1 : r1 ?
			    (const Q3CanvasItem*)r1 : e1 ?
			    (const Q3CanvasItem*)e1 : (const Q3CanvasItem*)t1;
    const Q3CanvasItem* i2 = s2 ?
			    (const Q3CanvasItem*)s2 : p2 ?
			    (const Q3CanvasItem*)p2 : r2 ?
			    (const Q3CanvasItem*)r2 : e2 ?
			    (const Q3CanvasItem*)e2 : (const Q3CanvasItem*)t2;

    if (s1 && s2) {
	// a
	return qt_testCollision(s1,s2);
    } else if ((r1 || t1 || s1) && (r2 || t2 || s2)) {
	// b
	QRect rc1 = i1->boundingRectAdvanced();
	QRect rc2 = i2->boundingRectAdvanced();
	return rc1.intersects(rc2);
    } else if (e1 && e2
		&& e1->angleLength()>=360*16 && e2->angleLength()>=360*16
		&& e1->width()==e1->height()
		&& e2->width()==e2->height()) {
	// c
	double xd = (e1->x()+e1->xVelocity())-(e2->x()+e1->xVelocity());
	double yd = (e1->y()+e1->yVelocity())-(e2->y()+e1->yVelocity());
	double rd = (e1->width()+e2->width())/2;
	return xd*xd+yd*yd <= rd*rd;
    } else if (p1 && (p2 || s2 || t2)) {
	// d
	Q3PointArray pa1 = p1->areaPointsAdvanced();
	Q3PointArray pa2 = p2 ? p2->areaPointsAdvanced()
			  : Q3PointArray(i2->boundingRectAdvanced());
	bool col= !(QRegion(pa1) & QRegion(pa2,true)).isEmpty();

	return col;
    } else {
	return collision_double_dispatch(s2,p2,r2,e2,t2,
					 s1,p1,r1,e1,t1);
    }
}

/*!
    \fn bool Q3CanvasItem::collidesWith(const Q3CanvasItem* other) const

    Returns true if the canvas item will collide with the \a other
    item \e after they have moved by their current velocities;
    otherwise returns false.

    \sa collisions()
*/


/*!
    \class Q3CanvasSprite qcanvas.h
    \compat
    \brief The Q3CanvasSprite class provides an animated canvas item on a Q3Canvas.

    A canvas sprite is an object which can contain any number of images
    (referred to as frames), only one of which is current, i.e.
    displayed, at any one time. The images can be passed in the
    constructor or set or changed later with setSequence(). If you
    subclass Q3CanvasSprite you can change the frame that is displayed
    periodically, e.g. whenever Q3CanvasItem::advance(1) is called to
    create the effect of animation.

    The current frame can be set with setFrame() or with move(). The
    number of frames available is given by frameCount(). The bounding
    rectangle of the current frame is returned by boundingRect().

    The current frame's image can be retrieved with image(); use
    imageAdvanced() to retrieve the image for the frame that will be
    shown after advance(1) is called. Use the image() overload passing
    it an integer index to retrieve a particular image from the list of
    frames.

    Use width() and height() to retrieve the dimensions of the current
    frame.

    Use leftEdge() and rightEdge() to retrieve the current frame's
    left-hand and right-hand x-coordinates respectively. Use
    bottomEdge() and topEdge() to retrieve the current frame's bottom
    and top y-coordinates respectively. These functions have an overload
    which will accept an integer frame number to retrieve the
    coordinates of a particular frame.

    Q3CanvasSprite draws very quickly, at the expense of memory.

    The current frame's image can be drawn on a painter with draw().

    Like any other canvas item, canvas sprites can be moved with
    move() which sets the x and y coordinates and the frame number, as
    well as with Q3CanvasItem::move() and Q3CanvasItem::moveBy(), or by
    setting coordinates with Q3CanvasItem::setX(), Q3CanvasItem::setY()
    and Q3CanvasItem::setZ().

    \sa QtCanvas, {Porting to Graphics View}
*/


/*!
  \reimp
*/
bool Q3CanvasSprite::collidesWith(const Q3CanvasItem* i) const
{
    return i->collidesWith(this,0,0,0,0);
}

/*!
    Returns true if the canvas item collides with any of the given
    items; otherwise returns false. The parameters, \a s, \a p, \a r,
    \a e and \a t, are all the same object, this is just a type
    resolution trick.
*/
bool Q3CanvasSprite::collidesWith(const Q3CanvasSprite* s,
				  const Q3CanvasPolygonalItem* p,
				  const Q3CanvasRectangle* r,
				  const Q3CanvasEllipse* e,
				  const Q3CanvasText* t) const
{
    return collision_double_dispatch(s,p,r,e,t,this,0,0,0,0);
}

/*!
  \reimp
*/
bool Q3CanvasPolygonalItem::collidesWith(const Q3CanvasItem* i) const
{
    return i->collidesWith(0,this,0,0,0);
}

bool Q3CanvasPolygonalItem::collidesWith( const Q3CanvasSprite* s,
				 const Q3CanvasPolygonalItem* p,
				 const Q3CanvasRectangle* r,
				 const Q3CanvasEllipse* e,
				 const Q3CanvasText* t) const
{
    return collision_double_dispatch(s,p,r,e,t,0,this,0,0,0);
}

/*!
  \reimp
*/
bool Q3CanvasRectangle::collidesWith(const Q3CanvasItem* i) const
{
    return i->collidesWith(0,this,this,0,0);
}

bool Q3CanvasRectangle::collidesWith( const Q3CanvasSprite* s,
				 const Q3CanvasPolygonalItem* p,
				 const Q3CanvasRectangle* r,
				 const Q3CanvasEllipse* e,
				 const Q3CanvasText* t) const
{
    return collision_double_dispatch(s,p,r,e,t,0,this,this,0,0);
}


/*!
  \reimp
*/
bool Q3CanvasEllipse::collidesWith(const Q3CanvasItem* i) const
{
    return i->collidesWith(0,this,0,this,0);
}

bool Q3CanvasEllipse::collidesWith( const Q3CanvasSprite* s,
				 const Q3CanvasPolygonalItem* p,
				 const Q3CanvasRectangle* r,
				 const Q3CanvasEllipse* e,
				 const Q3CanvasText* t) const
{
    return collision_double_dispatch(s,p,r,e,t,0,this,0,this,0);
}

/*!
  \reimp
*/
bool Q3CanvasText::collidesWith(const Q3CanvasItem* i) const
{
    return i->collidesWith(0,0,0,0,this);
}

bool Q3CanvasText::collidesWith( const Q3CanvasSprite* s,
				 const Q3CanvasPolygonalItem* p,
				 const Q3CanvasRectangle* r,
				 const Q3CanvasEllipse* e,
				 const Q3CanvasText* t) const
{
    return collision_double_dispatch(s,p,r,e,t,0,0,0,0,this);
}

/*!
    Returns the list of canvas items that this canvas item has
    collided with.

    A collision is generally defined as occurring when the pixels of
    one item draw on the pixels of another item, but not all
    subclasses are so precise. Also, since pixel-wise collision
    detection can be slow, this function works in either exact or
    inexact mode, according to the \a exact parameter.

    If \a exact is true, the canvas items returned have been
    accurately tested for collision with the canvas item.

    If \a exact is false, the canvas items returned are \e near the
    canvas item. You can test the canvas items returned using
    collidesWith() if any are interesting collision candidates. By
    using this approach, you can ignore some canvas items for which
    collisions are not relevant.

    The returned list is a list of Q3CanvasItems, but often you will
    need to cast the items to their subclass types. The safe way to do
    this is to use rtti() before casting. This provides some of the
    functionality of the standard C++ dynamic cast operation even on
    compilers where dynamic casts are not available.

    Note that a canvas item may be `on' a canvas, e.g. it was created
    with the canvas as parameter, even though its coordinates place it
    beyond the edge of the canvas's area. Collision detection only
    works for canvas items which are wholly or partly within the
    canvas's area.

    Note that if items have a velocity (see \l setVelocity()), then
    collision testing is done based on where the item \e will be when
    it moves, not its current location. For example, a "ball" item
    doesn't need to actually embed into a "wall" item before a
    collision is detected. For items without velocity, plain
    intersection is used.
*/
Q3CanvasItemList Q3CanvasItem::collisions(bool exact) const
{
    return canvas()->collisions(chunks(),this,exact);
}

/*!
    Returns a list of canvas items that collide with the point \a p.
    The list is ordered by z coordinates, from highest z coordinate
    (front-most item) to lowest z coordinate (rear-most item).
*/
Q3CanvasItemList Q3Canvas::collisions(const QPoint& p) const
{
    return collisions(QRect(p,QSize(1,1)));
}

/*!
    \overload

    Returns a list of items which collide with the rectangle \a r. The
    list is ordered by z coordinates, from highest z coordinate
    (front-most item) to lowest z coordinate (rear-most item).
*/
Q3CanvasItemList Q3Canvas::collisions(const QRect& r) const
{
    Q3CanvasRectangle i(r,(Q3Canvas*)this);
    i.setPen(NoPen);
    i.show(); // doesn't actually show, since we destroy it
    Q3CanvasItemList l = i.collisions(true);
    l.sort();
    return l;
}

/*!
    \overload

    Returns a list of canvas items which intersect with the chunks
    listed in \a chunklist, excluding \a item. If \a exact is true,
    only those which actually \link Q3CanvasItem::collidesWith()
    collide with\endlink \a item are returned; otherwise canvas items
    are included just for being in the chunks.

    This is a utility function mainly used to implement the simpler
    Q3CanvasItem::collisions() function.
*/
Q3CanvasItemList Q3Canvas::collisions(const Q3PointArray& chunklist,
	    const Q3CanvasItem* item, bool exact) const
{
    Q3PtrDict<void> seen;
    Q3CanvasItemList result;
    for (int i=0; i<(int)chunklist.count(); i++) {
	int x = chunklist[i].x();
	int y = chunklist[i].y();
	if (validChunk(x,y)) {
	    const Q3CanvasItemList* l = chunk(x,y).listPtr();
	    for (Q3CanvasItemList::ConstIterator it=l->begin(); it!=l->end(); ++it) {
		Q3CanvasItem *g=*it;
		if (g != item) {
		    if (!seen.find(g)) {
			seen.replace(g,(void*)1);
			if (!exact || item->collidesWith(g))
			    result.append(g);
		    }
		}
	    }
	}
    }
    return result;
}

/*!
  \internal
  Adds the item to all the chunks it covers.
*/
void Q3CanvasItem::addToChunks()
{
    if (isVisible() && canvas()) {
	Q3PointArray pa = chunks();
	for (int i=0; i<(int)pa.count(); i++)
	    canvas()->addItemToChunk(this,pa[i].x(),pa[i].y());
	val=(uint)true;
    }
}

/*!
  \internal
  Removes the item from all the chunks it covers.
*/
void Q3CanvasItem::removeFromChunks()
{
    if (isVisible() && canvas()) {
	Q3PointArray pa = chunks();
	for (int i=0; i<(int)pa.count(); i++)
	    canvas()->removeItemFromChunk(this,pa[i].x(),pa[i].y());
    }
}

/*!
  \internal
  Sets all the chunks covered by the item to be refreshed with Q3Canvas::update()
  is next called.
*/
void Q3CanvasItem::changeChunks()
{
    if (isVisible() && canvas()) {
	if (!val)
	    addToChunks();
	Q3PointArray pa = chunks();
	for (int i=0; i<(int)pa.count(); i++)
	    canvas()->setChangedChunk(pa[i].x(),pa[i].y());
    }
}

/*!
    \fn QRect Q3CanvasItem::boundingRect() const

    Returns the bounding rectangle in pixels that the canvas item covers.

    \sa boundingRectAdvanced()
*/

/*!
    Returns the bounding rectangle of pixels that the canvas item \e
    will cover after advance(1) is called.

    \sa boundingRect()
*/
QRect Q3CanvasItem::boundingRectAdvanced() const
{
    int dx = int(x()+xVelocity())-int(x());
    int dy = int(y()+yVelocity())-int(y());
    QRect r = boundingRect();
    r.moveBy(dx,dy);
    return r;
}

/*!
    \class Q3CanvasPixmap qcanvas.h
    \compat
    \brief The Q3CanvasPixmap class provides pixmaps for Q3CanvasSprites.

    If you want to show a single pixmap on a Q3Canvas use a
    Q3CanvasSprite with just one pixmap.

    When pixmaps are inserted into a Q3CanvasPixmapArray they are held
    as Q3CanvasPixmaps. \l{Q3CanvasSprite}s are used to show pixmaps on
    \l{Q3Canvas}es and hold their pixmaps in a Q3CanvasPixmapArray. If
    you retrieve a frame (pixmap) from a Q3CanvasSprite it will be
    returned as a Q3CanvasPixmap.

    The pixmap is a QPixmap and can only be set in the constructor.
    There are three different constructors, one taking a QPixmap, one
    a QImage and one a file name that refers to a file in any
    supported file format (see QImageReader).

    Q3CanvasPixmap can have a hotspot which is defined in terms of an (x,
    y) offset. When you create a Q3CanvasPixmap from a PNG file or from
    a QImage that has a QImage::offset(), the offset() is initialized
    appropriately, otherwise the constructor leaves it at (0, 0). You
    can set it later using setOffset(). When the Q3CanvasPixmap is used
    in a Q3CanvasSprite, the offset position is the point at
    Q3CanvasItem::x() and Q3CanvasItem::y(), not the top-left corner of
    the pixmap.

    Note that for Q3CanvasPixmap objects created by a Q3CanvasSprite, the
    position of each Q3CanvasPixmap object is set so that the hotspot
    stays in the same position.

    \sa Q3CanvasPixmapArray Q3CanvasItem Q3CanvasSprite, QtCanvas, {Porting to Graphics View}
*/

#ifndef QT_NO_IMAGEIO

/*!
    Constructs a Q3CanvasPixmap that uses the image stored in \a
    datafilename.
*/
Q3CanvasPixmap::Q3CanvasPixmap(const QString& datafilename)
{
    QImage image(datafilename);
    init(image);
}

#endif

/*!
    Constructs a Q3CanvasPixmap from the image \a image.
*/
Q3CanvasPixmap::Q3CanvasPixmap(const QImage& image)
{
    init(image);
}
/*!
    Constructs a Q3CanvasPixmap from the pixmap \a pm using the offset
    \a offset.
*/
Q3CanvasPixmap::Q3CanvasPixmap(const QPixmap& pm, const QPoint& offset)
{
    init(pm,offset.x(),offset.y());
}

void Q3CanvasPixmap::init(const QImage& image)
{
    convertFromImage(image);
    hotx = image.offset().x();
    hoty = image.offset().y();
#ifndef QT_NO_IMAGE_DITHER_TO_1
    if(image.hasAlphaBuffer()) {
	QImage i = image.createAlphaMask();
	collision_mask = new QImage(i);
    } else
#endif
	collision_mask = 0;
}

void Q3CanvasPixmap::init(const QPixmap& pixmap, int hx, int hy)
{
    (QPixmap&)*this = pixmap;
    hotx = hx;
    hoty = hy;
    if(pixmap.hasAlphaChannel())  {
	QImage i = mask().convertToImage();
	collision_mask = new QImage(i);
    } else
	collision_mask = 0;
}

/*!
    Destroys the pixmap.
*/
Q3CanvasPixmap::~Q3CanvasPixmap()
{
    delete collision_mask;
}

/*!
    \fn int Q3CanvasPixmap::offsetX() const

    Returns the x-offset of the pixmap's hotspot.

    \sa setOffset()
*/

/*!
    \fn int Q3CanvasPixmap::offsetY() const

    Returns the y-offset of the pixmap's hotspot.

    \sa setOffset()
*/

/*!
    \fn void Q3CanvasPixmap::setOffset(int x, int y)

    Sets the offset of the pixmap's hotspot to (\a x, \a y).

    \warning Do not call this function if any Q3CanvasSprites are
    currently showing this pixmap.
*/

/*!
    \class Q3CanvasPixmapArray qcanvas.h
    \compat
    \brief The Q3CanvasPixmapArray class provides an array of Q3CanvasPixmaps.

    This class is used by Q3CanvasSprite to hold an array of pixmaps.
    It is used to implement animated sprites, i.e. images that change
    over time, with each pixmap in the array holding one frame.

    Depending on the constructor you use you can load multiple pixmaps
    into the array either from a directory (specifying a wildcard
    pattern for the files), or from a list of QPixmaps. You can also
    read in a set of pixmaps after construction using readPixmaps().

    Individual pixmaps can be set with setImage() and retrieved with
    image(). The number of pixmaps in the array is returned by
    count().

    Q3CanvasSprite uses an image's mask for collision detection. You
    can change this by reading in a separate set of image masks using
    readCollisionMasks().

    \sa QtCanvas, {Porting to Graphics View}
*/

/*!
    Constructs an invalid array (i.e. isValid() will return false).
    You must call readPixmaps() before being able to use this
    Q3CanvasPixmapArray.
*/
Q3CanvasPixmapArray::Q3CanvasPixmapArray()
: framecount(0), img(0)
{
}

#ifndef QT_NO_IMAGEIO
/*!
    Constructs a Q3CanvasPixmapArray from files.

    The \a fc parameter sets the number of frames to be loaded for
    this image.

    If \a fc is not 0, \a datafilenamepattern should contain "%1",
    e.g. "foo%1.png". The actual filenames are formed by replacing the
    %1 with four-digit integers from 0 to (fc - 1), e.g. foo0000.png,
    foo0001.png, foo0002.png, etc.

    If \a fc is 0, \a datafilenamepattern is asssumed to be a
    filename, and the image contained in this file will be loaded as
    the first (and only) frame.

    If \a datafilenamepattern does not exist, is not readable, isn't
    an image, or some other error occurs, the array ends up empty and
    isValid() returns false.
*/

Q3CanvasPixmapArray::Q3CanvasPixmapArray(const QString& datafilenamepattern,
					int fc)
: framecount(0), img(0)
{
    readPixmaps(datafilenamepattern,fc);
}
#endif

/*!
  \obsolete
  Use Q3CanvasPixmapArray::Q3CanvasPixmapArray(Q3ValueList<QPixmap>, Q3PointArray)
  instead.

  Constructs a Q3CanvasPixmapArray from the list of QPixmaps \a
  list. The \a hotspots list has to be of the same size as \a list.
*/
Q3CanvasPixmapArray::Q3CanvasPixmapArray(Q3PtrList<QPixmap> list, Q3PtrList<QPoint> hotspots) :
    framecount(list.count()),
    img(new Q3CanvasPixmap*[list.count()])
{
    if (list.count() != hotspots.count()) {
	qWarning("Q3CanvasPixmapArray: lists have different lengths");
	reset();
	img = 0;
    } else {
	list.first();
	hotspots.first();
	for (int i=0; i<framecount; i++) {
	    img[i]=new Q3CanvasPixmap(*list.current(), *hotspots.current());
	    list.next();
	    hotspots.next();
	}
    }
}

/*!
    Constructs a Q3CanvasPixmapArray from the list of QPixmaps in the
    \a list. Each pixmap will get a hotspot according to the \a
    hotspots array. If no hotspots are specified, each one is set to
    be at position (0, 0).

    If an error occurs, isValid() will return false.
*/
Q3CanvasPixmapArray::Q3CanvasPixmapArray(Q3ValueList<QPixmap> list, Q3PointArray hotspots) :
    framecount((int)list.size()),
    img(new Q3CanvasPixmap*[list.size()])
{
    bool have_hotspots = (hotspots.size() != 0);
    if (have_hotspots && list.count() != hotspots.count()) {
	qWarning("Q3CanvasPixmapArray: lists have different lengths");
	reset();
	img = 0;
    } else {
	Q3ValueList<QPixmap>::iterator it;
	it = list.begin();
	for (int i=0; i<framecount; i++) {
	    QPoint hs = have_hotspots ? hotspots[i] : QPoint(0, 0);
	    img[i]=new Q3CanvasPixmap(*it, hs);
	    ++it;
	}
    }
}

/*!
    Destroys the pixmap array and all the pixmaps it contains.
*/
Q3CanvasPixmapArray::~Q3CanvasPixmapArray()
{
    reset();
}

void Q3CanvasPixmapArray::reset()
{
    for (int i=0; i<framecount; i++)
	delete img[i];
    delete [] img;
    img = 0;
    framecount = 0;
}

#ifndef QT_NO_IMAGEIO
/*!
    Reads one or more pixmaps into the pixmap array.

    If \a fc is not 0, \a filenamepattern should contain "%1", e.g.
    "foo%1.png". The actual filenames are formed by replacing the %1
    with four-digit integers from 0 to (fc - 1), e.g. foo0000.png,
    foo0001.png, foo0002.png, etc.

    If \a fc is 0, \a filenamepattern is asssumed to be a filename,
    and the image contained in this file will be loaded as the first
    (and only) frame.

    If \a filenamepattern does not exist, is not readable, isn't an
    image, or some other error occurs, this function will return
    false, and isValid() will return false; otherwise this function
    will return true.

    \sa isValid()
*/
bool Q3CanvasPixmapArray::readPixmaps(const QString& filenamepattern,
				      int fc)
{
    return readPixmaps(filenamepattern,fc,false);
}

/*!
    Reads new collision masks for the array.

    By default, Q3CanvasSprite uses the image mask of a sprite to
    detect collisions. Use this function to set your own collision
    image masks.

    If count() is 1 \a filename must specify a real filename to read
    the mask from. If count() is greater than 1, the \a filename must
    contain a "%1" that will get replaced by the number of the mask to
    be loaded, just like Q3CanvasPixmapArray::readPixmaps().

    All collision masks must be 1-bit images or this function call
    will fail.

    If the file isn't readable, contains the wrong number of images,
    or there is some other error, this function will return false, and
    the array will be flagged as invalid; otherwise this function
    returns true.

    \sa isValid()
*/
bool Q3CanvasPixmapArray::readCollisionMasks(const QString& filename)
{
    return readPixmaps(filename,framecount,true);
}


bool Q3CanvasPixmapArray::readPixmaps(const QString& datafilenamepattern,
				      int fc, bool maskonly)
{
    if (!maskonly) {
	reset();
	framecount = fc;
	if (!framecount)
	    framecount=1;
	img = new Q3CanvasPixmap*[framecount];
    }
    if (!img)
        return false;

    bool ok = true;
    bool arg = fc > 1;
    if (!arg)
	framecount=1;
    for (int i=0; i<framecount; i++) {
	QString r;
	r.sprintf("%04d",i);
	if (maskonly) {
            if (!img[i]->collision_mask)
                img[i]->collision_mask = new QImage();
	    img[i]->collision_mask->load(
		arg ? datafilenamepattern.arg(r) : datafilenamepattern);
	    ok = ok
	       && !img[i]->collision_mask->isNull()
	       && img[i]->collision_mask->depth()==1;
	} else {
	    img[i]=new Q3CanvasPixmap(
		arg ? datafilenamepattern.arg(r) : datafilenamepattern);
	    ok = ok && !img[i]->isNull();
	}
    }
    if (!ok) {
	reset();
    }
    return ok;
}
#endif

/*!
  \obsolete

  Use isValid() instead.

  This returns false if the array is valid, and true if it is not.
*/
bool Q3CanvasPixmapArray::operator!()
{
    return img==0;
}

/*!
    Returns true if the pixmap array is valid; otherwise returns
    false.
*/
bool Q3CanvasPixmapArray::isValid() const
{
    return (img != 0);
}

/*!
    \fn Q3CanvasPixmap* Q3CanvasPixmapArray::image(int i) const

    Returns pixmap \a i in the array, if \a i is non-negative and less
    than than count(), and returns an unspecified value otherwise.
*/

// ### wouldn't it be better to put empty Q3CanvasPixmaps in there instead of
// initializing the additional elements in the array to 0? Lars
/*!
    Replaces the pixmap at index \a i with pixmap \a p.

    The array takes ownership of \a p and will delete \a p when the
    array itself is deleted.

    If \a i is beyond the end of the array the array is extended to at
    least i+1 elements, with elements count() to i-1 being initialized
    to 0.
*/
void Q3CanvasPixmapArray::setImage(int i, Q3CanvasPixmap* p)
{
    if (i >= framecount) {
	Q3CanvasPixmap** newimg = new Q3CanvasPixmap*[i+1];
	memcpy(newimg, img, sizeof(Q3CanvasPixmap *)*framecount);
	memset(newimg + framecount, 0, sizeof(Q3CanvasPixmap *)*(i+1 - framecount));
	framecount = i+1;
	delete [] img;
	img = newimg;
    }
    delete img[i]; img[i]=p;
}

/*!
    \fn uint Q3CanvasPixmapArray::count() const

    Returns the number of pixmaps in the array.
*/

/*!
    Returns the x-coordinate of the current left edge of the sprite.
    (This may change as the sprite animates since different frames may
    have different left edges.)

    \sa rightEdge() bottomEdge() topEdge()
*/
int Q3CanvasSprite::leftEdge() const
{
    return int(x()) - image()->hotx;
}

/*!
    \overload

    Returns what the x-coordinate of the left edge of the sprite would
    be if the sprite (actually its hotspot) were moved to x-position
    \a nx.

    \sa rightEdge() bottomEdge() topEdge()
*/
int Q3CanvasSprite::leftEdge(int nx) const
{
    return nx - image()->hotx;
}

/*!
    Returns the y-coordinate of the top edge of the sprite. (This may
    change as the sprite animates since different frames may have
    different top edges.)

    \sa leftEdge() rightEdge() bottomEdge()
*/
int Q3CanvasSprite::topEdge() const
{
    return int(y()) - image()->hoty;
}

/*!
    \overload

    Returns what the y-coordinate of the top edge of the sprite would
    be if the sprite (actually its hotspot) were moved to y-position
    \a ny.

    \sa leftEdge() rightEdge() bottomEdge()
*/
int Q3CanvasSprite::topEdge(int ny) const
{
    return ny - image()->hoty;
}

/*!
    Returns the x-coordinate of the current right edge of the sprite.
    (This may change as the sprite animates since different frames may
    have different right edges.)

    \sa leftEdge() bottomEdge() topEdge()
*/
int Q3CanvasSprite::rightEdge() const
{
    return leftEdge() + image()->width()-1;
}

/*!
    \overload

    Returns what the x-coordinate of the right edge of the sprite
    would be if the sprite (actually its hotspot) were moved to
    x-position \a nx.

    \sa leftEdge() bottomEdge() topEdge()
*/
int Q3CanvasSprite::rightEdge(int nx) const
{
    return leftEdge(nx) + image()->width()-1;
}

/*!
    Returns the y-coordinate of the current bottom edge of the sprite.
    (This may change as the sprite animates since different frames may
    have different bottom edges.)

    \sa leftEdge() rightEdge() topEdge()
*/
int Q3CanvasSprite::bottomEdge() const
{
    return topEdge() + image()->height()-1;
}

/*!
    \overload

    Returns what the y-coordinate of the top edge of the sprite would
    be if the sprite (actually its hotspot) were moved to y-position
    \a ny.

    \sa leftEdge() rightEdge() topEdge()
*/
int Q3CanvasSprite::bottomEdge(int ny) const
{
    return topEdge(ny) + image()->height()-1;
}

/*!
    \fn Q3CanvasPixmap* Q3CanvasSprite::image() const

    Returns the current frame's image.

    \sa frame(), setFrame()
*/

/*!
    \fn Q3CanvasPixmap* Q3CanvasSprite::image(int f) const
    \overload

    Returns the image for frame \a f. Does not do any bounds checking on \a f.
*/

/*!
    Returns the image the sprite \e will have after advance(1) is
    called. By default this is the same as image().
*/
Q3CanvasPixmap* Q3CanvasSprite::imageAdvanced() const
{
    return image();
}

/*!
    Returns the bounding rectangle for the image in the sprite's
    current frame. This assumes that the images are tightly cropped
    (i.e. do not have transparent pixels all along a side).
*/
QRect Q3CanvasSprite::boundingRect() const
{
    return QRect(leftEdge(), topEdge(), width(), height());
}


/*!
  \internal
  Returns the chunks covered by the item.
*/
Q3PointArray Q3CanvasItem::chunks() const
{
    Q3PointArray r;
    int n=0;
    QRect br = boundingRect();
    if (isVisible() && canvas()) {
	int chunksize=canvas()->chunkSize();
	br &= QRect(0,0,canvas()->width(),canvas()->height());
	if (br.isValid()) {
	    r.resize((br.width()/chunksize+2)*(br.height()/chunksize+2));
	    for (int j=br.top()/chunksize; j<=br.bottom()/chunksize; j++) {
		for (int i=br.left()/chunksize; i<=br.right()/chunksize; i++) {
		    r[n++] = QPoint(i,j);
		}
	    }
	}
    }
    r.resize(n);
    return r;
}


/*!
  \internal
  Add the sprite to the chunks in its Q3Canvas which it overlaps.
*/
void Q3CanvasSprite::addToChunks()
{
    if (isVisible() && canvas()) {
	int chunksize=canvas()->chunkSize();
	for (int j=topEdge()/chunksize; j<=bottomEdge()/chunksize; j++) {
	    for (int i=leftEdge()/chunksize; i<=rightEdge()/chunksize; i++) {
		canvas()->addItemToChunk(this,i,j);
	    }
	}
    }
}

/*!
  \internal
  Remove the sprite from the chunks in its Q3Canvas which it overlaps.

  \sa addToChunks()
*/
void Q3CanvasSprite::removeFromChunks()
{
    if (isVisible() && canvas()) {
	int chunksize=canvas()->chunkSize();
	for (int j=topEdge()/chunksize; j<=bottomEdge()/chunksize; j++) {
	    for (int i=leftEdge()/chunksize; i<=rightEdge()/chunksize; i++) {
		canvas()->removeItemFromChunk(this,i,j);
	    }
	}
    }
}

/*!
    The width of the sprite for the current frame's image.

    \sa frame()
*/
//### mark: Why don't we have width(int) and height(int) to be
//consistent with leftEdge() and leftEdge(int)?
int Q3CanvasSprite::width() const
{
    return image()->width();
}

/*!
    The height of the sprite for the current frame's image.

    \sa frame()
*/
int Q3CanvasSprite::height() const
{
    return image()->height();
}


/*!
    Draws the current frame's image at the sprite's current position
    on painter \a painter.
*/
void Q3CanvasSprite::draw(QPainter& painter)
{
    painter.drawPixmap(leftEdge(),topEdge(),*image());
}

/*!
    \class Q3CanvasView qcanvas.h
    \compat
    \brief The Q3CanvasView class provides an on-screen view of a Q3Canvas.

    A Q3CanvasView is widget which provides a view of a Q3Canvas.

    If you want users to be able to interact with a canvas view,
    subclass Q3CanvasView. You might then reimplement
    Q3ScrollView::contentsMousePressEvent(). For example:

    \snippet doc/src/snippets/code/src_qt3support_canvas_q3canvas.cpp 1

    The canvas view shows canvas canvas(); this can be changed using
    setCanvas().

    A transformation matrix can be used to transform the view of the
    canvas in various ways, for example, zooming in or out or rotating.
    For example:

    \snippet doc/src/snippets/code/src_qt3support_canvas_q3canvas.cpp 2

    Use setWorldMatrix() to set the canvas view's world matrix: you must
    ensure that the world matrix is invertible. The current world matrix
    is retrievable with worldMatrix(), and its inversion is retrievable
    with inverseWorldMatrix().

    Example:

    The following code finds the part of the canvas that is visible in
    this view, i.e. the bounding rectangle of the view in canvas coordinates.

    \snippet doc/src/snippets/code/src_qt3support_canvas_q3canvas.cpp 3

    \sa QMatrix QPainter::setWorldMatrix(), QtCanvas, {Porting to Graphics View}
*/

/*!
    Constructs a Q3CanvasView with parent \a parent, and name \a name,
    using the widget flags \a f. The canvas view is not associated
    with a canvas, so you must to call setCanvas() to view a
    canvas.
*/
Q3CanvasView::Q3CanvasView(QWidget* parent, const char* name, Qt::WindowFlags f)
    : Q3ScrollView(parent,name,f|WResizeNoErase|WStaticContents)
{
    d = new Q3CanvasViewData;
    viewing = 0;
    setCanvas(0);
}

/*!
    \overload

    Constructs a Q3CanvasView which views canvas \a canvas, with parent
    \a parent, and name \a name, using the widget flags \a f.
*/
Q3CanvasView::Q3CanvasView(Q3Canvas* canvas, QWidget* parent, const char* name, Qt::WindowFlags f)
    : Q3ScrollView(parent,name,f|WResizeNoErase|WStaticContents)
{
    d = new Q3CanvasViewData;
    viewing = 0;
    setCanvas(canvas);
}

/*!
    Destroys the canvas view. The associated canvas is \e not deleted.
*/
Q3CanvasView::~Q3CanvasView()
{
    delete d;
    d = 0;
    setCanvas(0);
}

/*!
    \fn Q3Canvas* Q3CanvasView::canvas() const

    Returns a pointer to the canvas which the Q3CanvasView is currently
    showing.
*/


/*!
    Sets the canvas that the Q3CanvasView is showing to the canvas \a
    canvas.
*/
void Q3CanvasView::setCanvas(Q3Canvas* canvas)
{
    if (viewing == canvas)
        return;

    if (viewing) {
	disconnect(viewing);
	viewing->removeView(this);
    }
    viewing=canvas;
    if (viewing) {
	connect(viewing,SIGNAL(resized()), this, SLOT(updateContentsSize()));
	viewing->addView(this);
	viewing->setAllChanged();
    }
    if (d) // called by d'tor
        updateContentsSize();
    update();
}

#ifndef QT_NO_TRANSFORMATIONS
/*!
    Returns a reference to the canvas view's current transformation matrix.

    \sa setWorldMatrix() inverseWorldMatrix()
*/
const QMatrix &Q3CanvasView::worldMatrix() const
{
    return d->xform;
}

/*!
    Returns a reference to the inverse of the canvas view's current
    transformation matrix.

    \sa setWorldMatrix() worldMatrix()
*/
const QMatrix &Q3CanvasView::inverseWorldMatrix() const
{
    return d->ixform;
}

/*!
    Sets the transformation matrix of the Q3CanvasView to \a wm. The
    matrix must be invertible (i.e. if you create a world matrix that
    zooms out by 2 times, then the inverse of this matrix is one that
    will zoom in by 2 times).

    When you use this, you should note that the performance of the
    Q3CanvasView will decrease considerably.

    Returns false if \a wm is not invertable; otherwise returns true.

    \sa worldMatrix() inverseWorldMatrix() QMatrix::isInvertible()
*/
bool Q3CanvasView::setWorldMatrix(const QMatrix & wm)
{
    bool ok = wm.isInvertible();
    if (ok) {
	d->xform = wm;
	d->ixform = wm.invert();
	updateContentsSize();
	viewport()->update();
    }
    return ok;
}
#endif

void Q3CanvasView::updateContentsSize()
{
    if (viewing) {
	QRect br;
#ifndef QT_NO_TRANSFORMATIONS
	br = d->xform.map(QRect(0,0,viewing->width(),viewing->height()));
#else
	br = QRect(0,0,viewing->width(),viewing->height());
#endif

	if (br.width() < contentsWidth()) {
	    QRect r(contentsToViewport(QPoint(br.width(),0)),
		    QSize(contentsWidth()-br.width(),contentsHeight()));
            d->eraseRegion = r;
	}
	if (br.height() < contentsHeight()) {
	    QRect r(contentsToViewport(QPoint(0,br.height())),
		    QSize(contentsWidth(),contentsHeight()-br.height()));
            d->eraseRegion |= r;
	}

	resizeContents(br.width(),br.height());
    } else {
        d->eraseRegion = rect();
	resizeContents(1,1);
    }
}

/*!
    Repaints part of the Q3Canvas that the canvas view is showing
    starting at \a cx by \a cy, with a width of \a cw and a height of \a
    ch using the painter \a p.
*/
void Q3CanvasView::drawContents(QPainter *p, int cx, int cy, int cw, int ch)
{
    QRect r(cx,cy,cw,ch);
    if (!d->eraseRegion.isEmpty()) {
        const QVector<QRect> rects = d->eraseRegion.rects();
        for (int i = 0; i < rects.size(); ++i)
            p->eraseRect(rects.at(i));

        d->eraseRegion = QRegion();
    }

    if (viewing) {
        viewing->drawViewArea(this,p,r,false);
    } else {
	p->eraseRect(r);
    }
}

/*!
  \reimp
  \internal

  (Implemented to get rid of a compiler warning.)
*/
void Q3CanvasView::drawContents(QPainter *)
{
}

/*!
    Suggests a size sufficient to view the entire canvas.
*/
QSize Q3CanvasView::sizeHint() const
{
    if (!canvas())
	return Q3ScrollView::sizeHint();
    // should maybe take transformations into account
    return (canvas()->size() + 2 * QSize(frameWidth(), frameWidth()))
	   .boundedTo(3 * QApplication::desktop()->size() / 4);
}

/*!
    \class Q3CanvasPolygonalItem qcanvas.h
    \compat
    \brief The Q3CanvasPolygonalItem class provides a polygonal canvas item
    on a Q3Canvas.

    The mostly rectangular classes, such as Q3CanvasSprite and
    Q3CanvasText, use the object's bounding rectangle for movement,
    repainting and collision calculations. For most other items, the
    bounding rectangle can be far too large -- a diagonal line being
    the worst case, and there are many other cases which are also bad.
    Q3CanvasPolygonalItem provides polygon-based bounding rectangle
    handling, etc., which is much faster for non-rectangular items.

    Derived classes should try to define as small an area as possible
    to maximize efficiency, but the polygon must \e definitely be
    contained completely within the polygonal area. Calculating the
    exact requirements is usually difficult, but if you allow a small
    overestimate it can be easy and quick, while still getting almost
    all of Q3CanvasPolygonalItem's speed.

    Note that all subclasses \e must call hide() in their destructor
    since hide() needs to be able to access areaPoints().

    Normally, Q3CanvasPolygonalItem uses the odd-even algorithm for
    determining whether an object intersects this object. You can
    change this to the winding algorithm using setWinding().

    The bounding rectangle is available using boundingRect(). The
    points bounding the polygonal item are retrieved with
    areaPoints(). Use areaPointsAdvanced() to retrieve the bounding
    points the polygonal item \e will have after
    Q3CanvasItem::advance(1) has been called.

    If the shape of the polygonal item is about to change while the
    item is visible, call invalidate() before updating with a
    different result from \l areaPoints().

    By default, Q3CanvasPolygonalItem objects have a black pen and no
    brush (the default QPen and QBrush constructors). You can change
    this with setPen() and setBrush(), but note that some
    Q3CanvasPolygonalItem subclasses only use the brush, ignoring the
    pen setting.

    The polygonal item can be drawn on a painter with draw().
    Subclasses must reimplement drawShape() to draw themselves.

    Like any other canvas item polygonal items can be moved with
    Q3CanvasItem::move() and Q3CanvasItem::moveBy(), or by setting coordinates
    with Q3CanvasItem::setX(), Q3CanvasItem::setY() and Q3CanvasItem::setZ().

    \sa QtCanvas, {Porting to Graphics View}
*/


/*
  Since most polygonal items don't have a pen, the default is
  NoPen and a black brush.
*/
static const QPen& defaultPolygonPen()
{
    static QPen* dp=0;
    if (!dp)
	dp = new QPen;
    return *dp;
}

static const QBrush& defaultPolygonBrush()
{
    static QBrush* db=0;
    if (!db)
	db = new QBrush;
    return *db;
}

/*!
    Constructs a Q3CanvasPolygonalItem on the canvas \a canvas.
*/
Q3CanvasPolygonalItem::Q3CanvasPolygonalItem(Q3Canvas* canvas) :
    Q3CanvasItem(canvas),
    br(defaultPolygonBrush()),
    pn(defaultPolygonPen())
{
    wind=0;
}

/*!
    Note that all subclasses \e must call hide() in their destructor
    since hide() needs to be able to access areaPoints().
*/
Q3CanvasPolygonalItem::~Q3CanvasPolygonalItem()
{
}

/*!
    Returns true if the polygonal item uses the winding algorithm to
    determine the "inside" of the polygon. Returns false if it uses
    the odd-even algorithm.

    The default is to use the odd-even algorithm.

    \sa setWinding()
*/
bool Q3CanvasPolygonalItem::winding() const
{
    return wind;
}

/*!
    If \a enable is true, the polygonal item will use the winding
    algorithm to determine the "inside" of the polygon; otherwise the
    odd-even algorithm will be used.

    The default is to use the odd-even algorithm.

    \sa winding()
*/
void Q3CanvasPolygonalItem::setWinding(bool enable)
{
    wind = enable;
}

/*!
    Invalidates all information about the area covered by the canvas
    item. The item will be updated automatically on the next call that
    changes the item's status, for example, move() or update(). Call
    this function if you are going to change the shape of the item (as
    returned by areaPoints()) while the item is visible.
*/
void Q3CanvasPolygonalItem::invalidate()
{
    val = (uint)false;
    removeFromChunks();
}

/*!
    \fn Q3CanvasPolygonalItem::isValid() const

    Returns true if the polygonal item's area information has not been
    invalidated; otherwise returns false.

    \sa invalidate()
*/

/*!
    Returns the points the polygonal item \e will have after
    Q3CanvasItem::advance(1) is called, i.e. what the points are when
    advanced by the current xVelocity() and yVelocity().
*/
Q3PointArray Q3CanvasPolygonalItem::areaPointsAdvanced() const
{
    int dx = int(x()+xVelocity())-int(x());
    int dy = int(y()+yVelocity())-int(y());
    Q3PointArray r = areaPoints();
    r.detach(); // Explicit sharing is stupid.
    if (dx || dy)
	r.translate(dx,dy);
    return r;
}

//#define QCANVAS_POLYGONS_DEBUG
#ifdef QCANVAS_POLYGONS_DEBUG
static QWidget* dbg_wid=0;
static QPainter* dbg_ptr=0;
#endif

class QPolygonalProcessor {
public:
    QPolygonalProcessor(Q3Canvas* c, const Q3PointArray& pa) :
	canvas(c)
    {
	QRect pixelbounds = pa.boundingRect();
	int cs = canvas->chunkSize();
    QRect canvasbounds = pixelbounds.intersected(canvas->rect());
    bounds.setLeft(canvasbounds.left()/cs);
    bounds.setRight(canvasbounds.right()/cs);
    bounds.setTop(canvasbounds.top()/cs);
    bounds.setBottom(canvasbounds.bottom()/cs);
    bitmap = QImage(bounds.width() + 1, bounds.height(), 1, 2, QImage::LittleEndian);
    pnt = 0;
	bitmap.fill(0);
#ifdef QCANVAS_POLYGONS_DEBUG
	dbg_start();
#endif
    }

    inline void add(int x, int y)
    {
	if (pnt >= (int)result.size()) {
	    result.resize(pnt*2+10);
	}
	result[pnt++] = QPoint(x+bounds.x(),y+bounds.y());
#ifdef QCANVAS_POLYGONS_DEBUG
	if (dbg_ptr) {
	    int cs = canvas->chunkSize();
	    QRect r(x*cs+bounds.x()*cs,y*cs+bounds.y()*cs,cs-1,cs-1);
	    dbg_ptr->setPen(Qt::blue);
	    dbg_ptr->drawRect(r);
	}
#endif
    }

    inline void addBits(int x1, int x2, uchar newbits, int xo, int yo)
    {
	for (int i=x1; i<=x2; i++)
	    if (newbits & (1<<i))
		add(xo+i,yo);
    }

#ifdef QCANVAS_POLYGONS_DEBUG
    void dbg_start()
    {
	if (!dbg_wid) {
	    dbg_wid = new QWidget;
	    dbg_wid->resize(800,600);
	    dbg_wid->show();
	    dbg_ptr = new QPainter(dbg_wid);
	    dbg_ptr->setBrush(Qt::NoBrush);
	}
	dbg_ptr->fillRect(dbg_wid->rect(),Qt::white);
    }
#endif

    void doSpans(int n, QPoint* pt, int* w)
    {
	int cs = canvas->chunkSize();
	for (int j=0; j<n; j++) {
	    int y = pt[j].y()/cs-bounds.y();
        if (y >= bitmap.height() || y < 0) continue;
	    uchar* l = bitmap.scanLine(y);
	    int x = pt[j].x();
	    int x1 = x/cs-bounds.x();
        if (x1 > bounds.width()) continue;
        x1  = QMAX(0,x1);
	    int x2 = (x+w[j])/cs-bounds.x();
        if (x2 < 0) continue;
        x2 = QMIN(bounds.width(), x2);
	    int x1q = x1/8;
	    int x1r = x1%8;
	    int x2q = x2/8;
	    int x2r = x2%8;
#ifdef QCANVAS_POLYGONS_DEBUG
	    if (dbg_ptr) dbg_ptr->setPen(Qt::yellow);
#endif
	    if (x1q == x2q) {
		uchar newbits = (~l[x1q]) & (((2<<(x2r-x1r))-1)<<x1r);
		if (newbits) {
#ifdef QCANVAS_POLYGONS_DEBUG
		    if (dbg_ptr) dbg_ptr->setPen(Qt::darkGreen);
#endif
		    addBits(x1r,x2r,newbits,x1q*8,y);
		    l[x1q] |= newbits;
		}
	    } else {
#ifdef QCANVAS_POLYGONS_DEBUG
		if (dbg_ptr) dbg_ptr->setPen(Qt::blue);
#endif
		uchar newbits1 = (~l[x1q]) & (0xff<<x1r);
		if (newbits1) {
#ifdef QCANVAS_POLYGONS_DEBUG
		    if (dbg_ptr) dbg_ptr->setPen(Qt::green);
#endif
		    addBits(x1r,7,newbits1,x1q*8,y);
		    l[x1q] |= newbits1;
		}
		for (int i=x1q+1; i<x2q; i++) {
		    if (l[i] != 0xff) {
			addBits(0,7,~l[i],i*8,y);
			l[i]=0xff;
		    }
		}
		uchar newbits2 = (~l[x2q]) & (0xff>>(7-x2r));
		if (newbits2) {
#ifdef QCANVAS_POLYGONS_DEBUG
		    if (dbg_ptr) dbg_ptr->setPen(Qt::red);
#endif
		    addBits(0,x2r,newbits2,x2q*8,y);
		    l[x2q] |= newbits2;
		}
	    }
#ifdef QCANVAS_POLYGONS_DEBUG
	    if (dbg_ptr) {
		dbg_ptr->drawLine(pt[j],pt[j]+QPoint(w[j],0));
	    }
#endif
	}
	result.resize(pnt);
    }

    int pnt;
    Q3PointArray result;
    Q3Canvas* canvas;
    QRect bounds;
    QImage bitmap;
};


Q3PointArray Q3CanvasPolygonalItem::chunks() const
{
    Q3PointArray pa = areaPoints();

    if (!pa.size()) {
	pa.detach(); // Explicit sharing is stupid.
	return pa;
    }

    QPolygonalProcessor processor(canvas(),pa);

    scanPolygon(pa, wind, processor);

    return processor.result;
}
/*!
    Simply calls Q3CanvasItem::chunks().
*/
Q3PointArray Q3CanvasRectangle::chunks() const
{
    // No need to do a polygon scan!
    return Q3CanvasItem::chunks();
}

/*!
    Returns the bounding rectangle of the polygonal item, based on
    areaPoints().
*/
QRect Q3CanvasPolygonalItem::boundingRect() const
{
    return areaPoints().boundingRect();
}

/*!
    Reimplemented from Q3CanvasItem, this draws the polygonal item by
    setting the pen and brush for the item on the painter \a p and
    calling drawShape().
*/
void Q3CanvasPolygonalItem::draw(QPainter & p)
{
    p.setPen(pn);
    p.setBrush(br);
    drawShape(p);
}

/*!
    \fn void Q3CanvasPolygonalItem::drawShape(QPainter & p)

    Subclasses must reimplement this function to draw their shape. The
    pen and brush of \a p are already set to pen() and brush() prior
    to calling this function.

    \sa draw()
*/

/*!
    \fn QPen Q3CanvasPolygonalItem::pen() const

    Returns the QPen used to draw the outline of the item, if any.

    \sa setPen()
*/

/*!
    \fn QBrush Q3CanvasPolygonalItem::brush() const

    Returns the QBrush used to fill the item, if filled.

    \sa setBrush()
*/

/*!
    Sets the QPen used when drawing the item to the pen \a p.
    Note that many Q3CanvasPolygonalItems do not use the pen value.

    \sa setBrush(), pen(), drawShape()
*/
void Q3CanvasPolygonalItem::setPen(QPen p)
{
    if (pn != p) {
	removeFromChunks();
	pn = p;
	addToChunks();
    }
}

/*!
    Sets the QBrush used when drawing the polygonal item to the brush \a b.

    \sa setPen(), brush(), drawShape()
*/
void Q3CanvasPolygonalItem::setBrush(QBrush b)
{
    if (br != b) {
	br = b;
	changeChunks();
    }
}


/*!
    \class Q3CanvasPolygon qcanvas.h
    \compat
    \brief The Q3CanvasPolygon class provides a polygon on a Q3Canvas.

    Paints a polygon with a QBrush. The polygon's points can be set in
    the constructor or set or changed later using setPoints(). Use
    points() to retrieve the points, or areaPoints() to retrieve the
    points relative to the canvas's origin.

    The polygon can be drawn on a painter with drawShape().

    Like any other canvas item polygons can be moved with
    Q3CanvasItem::move() and Q3CanvasItem::moveBy(), or by setting
    coordinates with Q3CanvasItem::setX(), Q3CanvasItem::setY() and
    Q3CanvasItem::setZ().

    Note: Q3CanvasPolygon does not use the pen.

    \sa QtCanvas, {Porting to Graphics View}
*/

/*!
    Constructs a point-less polygon on the canvas \a canvas. You
    should call setPoints() before using it further.
*/
Q3CanvasPolygon::Q3CanvasPolygon(Q3Canvas* canvas) :
    Q3CanvasPolygonalItem(canvas)
{
}

/*!
    Destroys the polygon.
*/
Q3CanvasPolygon::~Q3CanvasPolygon()
{
    hide();
}

/*!
    Draws the polygon using the painter \a p.

    Note that Q3CanvasPolygon does not support an outline (the pen is
    always NoPen).
*/
void Q3CanvasPolygon::drawShape(QPainter & p)
{
    // ### why can't we draw outlines? We could use drawPolyline for it. Lars
    // ### see other message. Warwick

    p.setPen(NoPen); // since QRegion(Q3PointArray) excludes outline :-()-:
    p.drawPolygon(poly);
}

/*!
    Sets the points of the polygon to be \a pa. These points will have
    their x and y coordinates automatically translated by x(), y() as
    the polygon is moved.
*/
void Q3CanvasPolygon::setPoints(Q3PointArray pa)
{
    removeFromChunks();
    poly = pa;
    poly.detach(); // Explicit sharing is stupid.
    poly.translate((int)x(),(int)y());
    addToChunks();
}

/*!
  \reimp
*/
void Q3CanvasPolygon::moveBy(double dx, double dy)
{
    // Note: does NOT call Q3CanvasPolygonalItem::moveBy(), since that
    // only does half this work.
    //
    int idx = int(x()+dx)-int(x());
    int idy = int(y()+dy)-int(y());
    if (idx || idy) {
	removeFromChunks();
	poly.translate(idx,idy);
    }
    myx+=dx;
    myy+=dy;
    if (idx || idy) {
	addToChunks();
    }
}

/*!
    \class Q3CanvasSpline qcanvas.h
    \compat
    \brief The Q3CanvasSpline class provides multi-bezier splines on a Q3Canvas.

    A Q3CanvasSpline is a sequence of 4-point bezier curves joined
    together to make a curved shape.

    You set the control points of the spline with setControlPoints().

    If the bezier is closed(), then the first control point will be
    re-used as the last control point. Therefore, a closed bezier must
    have a multiple of 3 control points and an open bezier must have
    one extra point.

    The beziers are not necessarily joined "smoothly". To ensure this,
    set control points appropriately (general reference texts about
    beziers will explain this in detail).

    Like any other canvas item splines can be moved with
    Q3CanvasItem::move() and Q3CanvasItem::moveBy(), or by setting
    coordinates with Q3CanvasItem::setX(), Q3CanvasItem::setY() and
    Q3CanvasItem::setZ().

    \sa QtCanvas, {Porting to Graphics View}
*/

/*!
    Create a spline with no control points on the canvas \a canvas.

    \sa setControlPoints()
*/
Q3CanvasSpline::Q3CanvasSpline(Q3Canvas* canvas) :
    Q3CanvasPolygon(canvas),
    cl(true)
{
}

/*!
    Destroy the spline.
*/
Q3CanvasSpline::~Q3CanvasSpline()
{
}

/*!
    Set the spline control points to \a ctrl.

    If \a close is true, then the first point in \a ctrl will be
    re-used as the last point, and the number of control points must
    be a multiple of 3. If \a close is false, one additional control
    point is required, and the number of control points must be one of
    (4, 7, 10, 13, ...).

    If the number of control points doesn't meet the above conditions,
    the number of points will be truncated to the largest number of
    points that do meet the requirement.
*/
void Q3CanvasSpline::setControlPoints(Q3PointArray ctrl, bool close)
{
    if ((int)ctrl.count() % 3 != (close ? 0 : 1)) {
	qWarning("Q3CanvasSpline::setControlPoints(): Number of points doesn't fit.");
	int numCurves = (ctrl.count() - (close ? 0 : 1))/ 3;
	ctrl.resize(numCurves*3 + (close ? 0 : 1));
    }

    cl = close;
    bez = ctrl;
    recalcPoly();
}

/*!
    Returns the current set of control points.

    \sa setControlPoints(), closed()
*/
Q3PointArray Q3CanvasSpline::controlPoints() const
{
    return bez;
}

/*!
    Returns true if the control points are a closed set; otherwise
    returns false.
*/
bool Q3CanvasSpline::closed() const
{
    return cl;
}

void Q3CanvasSpline::recalcPoly()
{
    Q3PtrList<Q3PointArray> segs;
    segs.setAutoDelete(true);
    int n=0;
    for (int i=0; i<(int)bez.count()-1; i+=3) {
	Q3PointArray ctrl(4);
	ctrl[0] = bez[i+0];
	ctrl[1] = bez[i+1];
	ctrl[2] = bez[i+2];
	if (cl)
	    ctrl[3] = bez[(i+3)%(int)bez.count()];
	else
	    ctrl[3] = bez[i+3];
	Q3PointArray *seg = new Q3PointArray(ctrl.cubicBezier());
	n += seg->count()-1;
	segs.append(seg);
    }
    Q3PointArray p(n+1);
    n=0;
    for (Q3PointArray* seg = segs.first(); seg; seg = segs.next()) {
	for (int i=0; i<(int)seg->count()-1; i++)
	    p[n++] = seg->point(i);
	if (n == (int)p.count()-1)
	    p[n] = seg->point(seg->count()-1);
    }
    Q3CanvasPolygon::setPoints(p);
}

/*!
    \fn Q3PointArray Q3CanvasPolygonalItem::areaPoints() const

    This function must be reimplemented by subclasses. It \e must
    return the points bounding (i.e. outside and not touching) the
    shape or drawing errors will occur.
*/

/*!
    \fn Q3PointArray Q3CanvasPolygon::points() const

    Returns the vertices of the polygon, not translated by the position.

    \sa setPoints(), areaPoints()
*/
Q3PointArray Q3CanvasPolygon::points() const
{
    Q3PointArray pa = areaPoints();
    pa.translate(int(-x()),int(-y()));
    return pa;
}

/*!
    Returns the vertices of the polygon translated by the polygon's
    current x(), y() position, i.e. relative to the canvas's origin.

    \sa setPoints(), points()
*/
Q3PointArray Q3CanvasPolygon::areaPoints() const
{
    return poly.copy();
}

/*!
    \class Q3CanvasLine qcanvas.h
    \compat
    \brief The Q3CanvasLine class provides a line on a Q3Canvas.

    The line inherits functionality from Q3CanvasPolygonalItem, for
    example the setPen() function. The start and end points of the
    line are set with setPoints().

    Like any other canvas item lines can be moved with
    Q3CanvasItem::move() and Q3CanvasItem::moveBy(), or by setting
    coordinates with Q3CanvasItem::setX(), Q3CanvasItem::setY() and
    Q3CanvasItem::setZ().

    \sa QtCanvas, {Porting to Graphics View}
*/

/*!
    Constructs a line from (0,0) to (0,0) on \a canvas.

    \sa setPoints()
*/
Q3CanvasLine::Q3CanvasLine(Q3Canvas* canvas) :
    Q3CanvasPolygonalItem(canvas)
{
    x1 = y1 = x2 = y2 = 0;
}

/*!
    Destroys the line.
*/
Q3CanvasLine::~Q3CanvasLine()
{
    hide();
}

/*!
  \reimp
*/
void Q3CanvasLine::setPen(QPen p)
{
    Q3CanvasPolygonalItem::setPen(p);
}

/*!
    \fn QPoint Q3CanvasLine::startPoint () const

    Returns the start point of the line.

    \sa setPoints(), endPoint()
*/

/*!
    \fn QPoint Q3CanvasLine::endPoint () const

    Returns the end point of the line.

    \sa setPoints(), startPoint()
*/

/*!
    Sets the line's start point to (\a xa, \a ya) and its end point to
    (\a xb, \a yb).
*/
void Q3CanvasLine::setPoints(int xa, int ya, int xb, int yb)
{
    if (x1 != xa || x2 != xb || y1 != ya || y2 != yb) {
	removeFromChunks();
	x1 = xa;
	y1 = ya;
	x2 = xb;
	y2 = yb;
	addToChunks();
    }
}

/*!
  \reimp
*/
void Q3CanvasLine::drawShape(QPainter &p)
{
    p.drawLine((int)(x()+x1), (int)(y()+y1), (int)(x()+x2), (int)(y()+y2));
}

/*!
    \reimp

    Note that the area defined by the line is somewhat thicker than
    the line that is actually drawn.
*/
Q3PointArray Q3CanvasLine::areaPoints() const
{
    Q3PointArray p(4);
    int xi = int(x());
    int yi = int(y());
    int pw = pen().width();
    int dx = QABS(x1-x2);
    int dy = QABS(y1-y2);
    pw = pw*4/3+2; // approx pw*sqrt(2)
    int px = x1<x2 ? -pw : pw ;
    int py = y1<y2 ? -pw : pw ;
    if (dx && dy && (dx > dy ? (dx*2/dy <= 2) : (dy*2/dx <= 2))) {
	// steep
	if (px == py) {
	    p[0] = QPoint(x1+xi   ,y1+yi+py);
	    p[1] = QPoint(x2+xi-px,y2+yi  );
	    p[2] = QPoint(x2+xi   ,y2+yi-py);
	    p[3] = QPoint(x1+xi+px,y1+yi  );
	} else {
	    p[0] = QPoint(x1+xi+px,y1+yi  );
	    p[1] = QPoint(x2+xi   ,y2+yi-py);
	    p[2] = QPoint(x2+xi-px,y2+yi  );
	    p[3] = QPoint(x1+xi   ,y1+yi+py);
	}
    } else if (dx > dy) {
	// horizontal
	p[0] = QPoint(x1+xi+px,y1+yi+py);
	p[1] = QPoint(x2+xi-px,y2+yi+py);
	p[2] = QPoint(x2+xi-px,y2+yi-py);
	p[3] = QPoint(x1+xi+px,y1+yi-py);
    } else {
	// vertical
	p[0] = QPoint(x1+xi+px,y1+yi+py);
	p[1] = QPoint(x2+xi+px,y2+yi-py);
	p[2] = QPoint(x2+xi-px,y2+yi-py);
	p[3] = QPoint(x1+xi-px,y1+yi+py);
    }
    return p;
}

/*!
    \reimp

*/

void Q3CanvasLine::moveBy(double dx, double dy)
{
    Q3CanvasPolygonalItem::moveBy(dx, dy);
}

/*!
    \class Q3CanvasRectangle qcanvas.h
    \compat
    \brief The Q3CanvasRectangle class provides a rectangle on a Q3Canvas.

    This item paints a single rectangle which may have any pen() and
    brush(), but may not be tilted/rotated. For rotated rectangles,
    use Q3CanvasPolygon.

    The rectangle's size and initial position can be set in the
    constructor. The size can be set or changed later using setSize().
    Use height() and width() to retrieve the rectangle's dimensions.

    The rectangle can be drawn on a painter with drawShape().

    Like any other canvas item rectangles can be moved with
    Q3CanvasItem::move() and Q3CanvasItem::moveBy(), or by setting
    coordinates with Q3CanvasItem::setX(), Q3CanvasItem::setY() and
    Q3CanvasItem::setZ().

    \sa QtCanvas, {Porting to Graphics View}
*/

/*!
    Constructs a rectangle at position (0,0) with both width and
    height set to 32 pixels on \a canvas.
*/
Q3CanvasRectangle::Q3CanvasRectangle(Q3Canvas* canvas) :
    Q3CanvasPolygonalItem(canvas),
    w(32), h(32)
{
}

/*!
    Constructs a rectangle positioned and sized by \a r on \a canvas.
*/
Q3CanvasRectangle::Q3CanvasRectangle(const QRect& r, Q3Canvas* canvas) :
    Q3CanvasPolygonalItem(canvas),
    w(r.width()), h(r.height())
{
    move(r.x(),r.y());
}

/*!
    Constructs a rectangle at position (\a x, \a y) and size \a width
    by \a height, on \a canvas.
*/
Q3CanvasRectangle::Q3CanvasRectangle(int x, int y, int width, int height,
	Q3Canvas* canvas) :
    Q3CanvasPolygonalItem(canvas),
    w(width), h(height)
{
    move(x,y);
}

/*!
    Destroys the rectangle.
*/
Q3CanvasRectangle::~Q3CanvasRectangle()
{
    hide();
}


/*!
    Returns the width of the rectangle.
*/
int Q3CanvasRectangle::width() const
{
    return w;
}

/*!
    Returns the height of the rectangle.
*/
int Q3CanvasRectangle::height() const
{
    return h;
}

/*!
    Sets the \a width and \a height of the rectangle.
*/
void Q3CanvasRectangle::setSize(int width, int height)
{
    if (w != width || h != height) {
	removeFromChunks();
	w = width;
	h = height;
	addToChunks();
    }
}

/*!
    \fn QSize Q3CanvasRectangle::size() const

    Returns the width() and height() of the rectangle.

    \sa rect(), setSize()
*/

/*!
    \fn QRect Q3CanvasRectangle::rect() const

    Returns the integer-converted x(), y() position and size() of the
    rectangle as a QRect.
*/

/*!
  \reimp
*/
Q3PointArray Q3CanvasRectangle::areaPoints() const
{
    Q3PointArray pa(4);
    int pw = (pen().width()+1)/2;
    if (pw < 1) pw = 1;
    if (pen() == NoPen) pw = 0;
    pa[0] = QPoint((int)x()-pw,(int)y()-pw);
    pa[1] = pa[0] + QPoint(w+pw*2,0);
    pa[2] = pa[1] + QPoint(0,h+pw*2);
    pa[3] = pa[0] + QPoint(0,h+pw*2);
    return pa;
}

/*!
    Draws the rectangle on painter \a p.
*/
void Q3CanvasRectangle::drawShape(QPainter & p)
{
    p.drawRect((int)x(), (int)y(), w, h);
}


/*!
    \class Q3CanvasEllipse qcanvas.h
    \compat
    \brief The Q3CanvasEllipse class provides an ellipse or ellipse segment on a Q3Canvas.

    A canvas item that paints an ellipse or ellipse segment with a QBrush.
    The ellipse's height, width, start angle and angle length can be set
    at construction time. The size can be changed at runtime with
    setSize(), and the angles can be changed (if you're displaying an
    ellipse segment rather than a whole ellipse) with setAngles().

    Note that angles are specified in 16ths of a degree.

    \target anglediagram
    \img qcanvasellipse.png Ellipse

    If a start angle and length angle are set then an ellipse segment
    will be drawn. The start angle is the angle that goes from zero in a
    counter-clockwise direction (shown in green in the diagram). The
    length angle is the angle from the start angle in a
    counter-clockwise direction (shown in blue in the diagram). The blue
    segment is the segment of the ellipse that would be drawn. If no
    start angle and length angle are specified the entire ellipse is
    drawn.

    The ellipse can be drawn on a painter with drawShape().

    Like any other canvas item ellipses can be moved with move() and
    moveBy(), or by setting coordinates with setX(), setY() and setZ().

    Note: Q3CanvasEllipse does not use the pen.

    \sa QtCanvas, {Porting to Graphics View}
*/

/*!
    Constructs a 32x32 ellipse, centered at (0, 0) on \a canvas.
*/
Q3CanvasEllipse::Q3CanvasEllipse(Q3Canvas* canvas) :
    Q3CanvasPolygonalItem(canvas),
    w(32), h(32),
    a1(0), a2(360*16)
{
}

/*!
    Constructs a \a width by \a height pixel ellipse, centered at
    (0, 0) on \a canvas.
*/
Q3CanvasEllipse::Q3CanvasEllipse(int width, int height, Q3Canvas* canvas) :
    Q3CanvasPolygonalItem(canvas),
    w(width),h(height),
    a1(0),a2(360*16)
{
}

// ### add a constructor taking degrees in float. 1/16 degrees is stupid. Lars
// ### it's how QPainter does it, so Q3Canvas does too for consistency. If it's
// ###  a good idea, it should be added to QPainter, not just to Q3Canvas. Warwick
/*!
    Constructs a \a width by \a height pixel ellipse, centered at
    (0, 0) on \a canvas. Only a segment of the ellipse is drawn,
    starting at angle \a startangle, and extending for angle \a angle
    (the angle length).

    Note that angles are specified in sixteenths of a degree.
*/
Q3CanvasEllipse::Q3CanvasEllipse(int width, int height,
    int startangle, int angle, Q3Canvas* canvas) :
    Q3CanvasPolygonalItem(canvas),
    w(width),h(height),
    a1(startangle),a2(angle)
{
}

/*!
    Destroys the ellipse.
*/
Q3CanvasEllipse::~Q3CanvasEllipse()
{
    hide();
}

/*!
    Returns the width of the ellipse.
*/
int Q3CanvasEllipse::width() const
{
    return w;
}

/*!
    Returns the height of the ellipse.
*/
int Q3CanvasEllipse::height() const
{
    return h;
}

/*!
    Sets the \a width and \a height of the ellipse.
*/
void Q3CanvasEllipse::setSize(int width, int height)
{
    if (w != width || h != height) {
	removeFromChunks();
	w = width;
	h = height;
	addToChunks();
    }
}

/*!
    \fn int Q3CanvasEllipse::angleStart() const

    Returns the start angle in 16ths of a degree. Initially
    this will be 0.

    \sa setAngles(), angleLength()
*/

/*!
    \fn int Q3CanvasEllipse::angleLength() const

    Returns the length angle (the extent of the ellipse segment) in
    16ths of a degree. Initially this will be 360 * 16 (a complete
    ellipse).

    \sa setAngles(), angleStart()
*/

/*!
    Sets the angles for the ellipse. The start angle is \a start and
    the extent of the segment is \a length (the angle length) from the
    \a start. The angles are specified in 16ths of a degree. By
    default the ellipse will start at 0 and have an angle length of
    360 * 16 (a complete ellipse).

    \sa angleStart(), angleLength()
*/
void Q3CanvasEllipse::setAngles(int start, int length)
{
    if (a1 != start || a2 != length) {
	removeFromChunks();
	a1 = start;
	a2 = length;
	addToChunks();
    }
}

/*!
  \reimp
*/
Q3PointArray Q3CanvasEllipse::areaPoints() const
{
    Q3PointArray r;
    // makeArc at 0,0, then translate so that fixed point math doesn't overflow
    r.makeArc(int(x()-w/2.0+0.5)-1, int(y()-h/2.0+0.5)-1, w+3, h+3, a1, a2);
    r.resize(r.size()+1);
    r.setPoint(r.size()-1,int(x()),int(y()));
    return r;
}

// ### support outlines! Lars
// ### QRegion doesn't, so we cannot (try it). Warwick
/*!
    Draws the ellipse, centered at x(), y() using the painter \a p.

    Note that Q3CanvasEllipse does not support an outline (the pen is
    always NoPen).
*/
void Q3CanvasEllipse::drawShape(QPainter & p)
{
    p.setPen(NoPen); // since QRegion(Q3PointArray) excludes outline :-()-:
    if (!a1 && a2 == 360*16) {
	p.drawEllipse(int(x()-w/2.0+0.5), int(y()-h/2.0+0.5), w, h);
    } else {
	p.drawPie(int(x()-w/2.0+0.5), int(y()-h/2.0+0.5), w, h, a1, a2);
    }
}


/*!
    \class Q3CanvasText
    \compat
    \brief The Q3CanvasText class provides a text object on a Q3Canvas.

    A canvas text item has text with font, color and alignment
    attributes. The text and font can be set in the constructor or set
    or changed later with setText() and setFont(). The color is set
    with setColor() and the alignment with setTextFlags(). The text
    item's bounding rectangle is retrieved with boundingRect().

    The text can be drawn on a painter with draw().

    Like any other canvas item text items can be moved with
    Q3CanvasItem::move() and Q3CanvasItem::moveBy(), or by setting
    coordinates with Q3CanvasItem::setX(), Q3CanvasItem::setY() and
    Q3CanvasItem::setZ().

    \sa QtCanvas, {Porting to Graphics View}
*/

/*!
    Constructs a Q3CanvasText with the text "\<text\>", on \a canvas.
*/
Q3CanvasText::Q3CanvasText(Q3Canvas* canvas) :
    Q3CanvasItem(canvas),
    txt(QLatin1String("<text>")), flags(0)
{
    setRect();
}

// ### add textflags to the constructor? Lars
/*!
    Constructs a Q3CanvasText with the text \a t, on canvas \a canvas.
*/
Q3CanvasText::Q3CanvasText(const QString& t, Q3Canvas* canvas) :
    Q3CanvasItem(canvas),
    txt(t), flags(0)
{
    setRect();
}

// ### see above
/*!
    Constructs a Q3CanvasText with the text \a t and font \a f, on the
    canvas \a canvas.
*/
Q3CanvasText::Q3CanvasText(const QString& t, QFont f, Q3Canvas* canvas) :
    Q3CanvasItem(canvas),
    txt(t), flags(0),
    fnt(f)
{
    setRect();
}

/*!
    Destroys the canvas text item.
*/
Q3CanvasText::~Q3CanvasText()
{
    removeFromChunks();
}

/*!
    Returns the bounding rectangle of the text.
*/
QRect Q3CanvasText::boundingRect() const { return brect; }

void Q3CanvasText::setRect()
{
    brect = QFontMetrics(fnt).boundingRect(int(x()), int(y()), 0, 0, flags, txt);
}

/*!
    \fn int Q3CanvasText::textFlags() const

    Returns the currently set alignment flags.

    \sa setTextFlags() Qt::AlignmentFlag
*/


/*!
    Sets the alignment flags to \a f. These are a bitwise OR of the
    flags available to QPainter::drawText() -- see the
    \l{Qt::AlignmentFlag}s.

    \sa setFont() setColor()
*/
void Q3CanvasText::setTextFlags(int f)
{
    if (flags != f) {
	removeFromChunks();
	flags = f;
	setRect();
	addToChunks();
    }
}

/*!
    Returns the text item's text.

    \sa setText()
*/
QString Q3CanvasText::text() const
{
    return txt;
}


/*!
    Sets the text item's text to \a t. The text may contain newlines.

    \sa text(), setFont(), setColor() setTextFlags()
*/
void Q3CanvasText::setText(const QString& t)
{
    if (txt != t) {
	removeFromChunks();
	txt = t;
	setRect();
	addToChunks();
    }
}

/*!
    Returns the font in which the text is drawn.

    \sa setFont()
*/
QFont Q3CanvasText::font() const
{
    return fnt;
}

/*!
    Sets the font in which the text is drawn to font \a f.

    \sa font()
*/
void Q3CanvasText::setFont(const QFont& f)
{
    if (f != fnt) {
	removeFromChunks();
	fnt = f;
	setRect();
	addToChunks();
    }
}

/*!
    Returns the color of the text.

    \sa setColor()
*/
QColor Q3CanvasText::color() const
{
    return col;
}

/*!
    Sets the color of the text to the color \a c.

    \sa color(), setFont()
*/
void Q3CanvasText::setColor(const QColor& c)
{
    col=c;
    changeChunks();
}


/*!
  \reimp
*/
void Q3CanvasText::moveBy(double dx, double dy)
{
    int idx = int(x()+dx)-int(x());
    int idy = int(y()+dy)-int(y());
    if (idx || idy) {
	removeFromChunks();
    }
    myx+=dx;
    myy+=dy;
    if (idx || idy) {
	brect.moveBy(idx,idy);
	addToChunks();
    }
}

/*!
    Draws the text using the painter \a painter.
*/
void Q3CanvasText::draw(QPainter& painter)
{
    painter.setFont(fnt);
    painter.setPen(col);
    painter.drawText(painter.fontMetrics().boundingRect(int(x()), int(y()), 0, 0, flags, txt), flags, txt);
}

/*!
  \reimp
*/
void Q3CanvasText::changeChunks()
{
    if (isVisible() && canvas()) {
	int chunksize=canvas()->chunkSize();
	for (int j=brect.top()/chunksize; j<=brect.bottom()/chunksize; j++) {
	    for (int i=brect.left()/chunksize; i<=brect.right()/chunksize; i++) {
		canvas()->setChangedChunk(i,j);
	    }
	}
    }
}

/*!
    Adds the text item to the appropriate chunks.
*/
void Q3CanvasText::addToChunks()
{
    if (isVisible() && canvas()) {
	int chunksize=canvas()->chunkSize();
	for (int j=brect.top()/chunksize; j<=brect.bottom()/chunksize; j++) {
	    for (int i=brect.left()/chunksize; i<=brect.right()/chunksize; i++) {
		canvas()->addItemToChunk(this,i,j);
	    }
	}
    }
}

/*!
    Removes the text item from the appropriate chunks.
*/
void Q3CanvasText::removeFromChunks()
{
    if (isVisible() && canvas()) {
	int chunksize=canvas()->chunkSize();
	for (int j=brect.top()/chunksize; j<=brect.bottom()/chunksize; j++) {
	    for (int i=brect.left()/chunksize; i<=brect.right()/chunksize; i++) {
		canvas()->removeItemFromChunk(this,i,j);
	    }
	}
    }
}


/*!
    Returns 0 (Q3CanvasItem::Rtti_Item).

    Make your derived classes return their own values for rtti(), so
    that you can distinguish between objects returned by
    Q3Canvas::at(). You should use values greater than 1000 to allow
    for extensions to this class.

    Overuse of this functionality can damage its extensibility. For
    example, once you have identified a base class of a Q3CanvasItem
    found by Q3Canvas::at(), cast it to that type and call meaningful
    methods rather than acting upon the object based on its rtti
    value.

    For example:

    \snippet doc/src/snippets/code/src_qt3support_canvas_q3canvas.cpp 4
*/
int Q3CanvasItem::rtti() const { return RTTI; }
int Q3CanvasItem::RTTI = Rtti_Item;

/*!
    Returns 1 (Q3CanvasItem::Rtti_Sprite).

    \sa Q3CanvasItem::rtti()
*/
int Q3CanvasSprite::rtti() const { return RTTI; }
int Q3CanvasSprite::RTTI = Rtti_Sprite;

/*!
    Returns 2 (Q3CanvasItem::Rtti_PolygonalItem).

    \sa Q3CanvasItem::rtti()
*/
int Q3CanvasPolygonalItem::rtti() const { return RTTI; }
int Q3CanvasPolygonalItem::RTTI = Rtti_PolygonalItem;

/*!
    Returns 3 (Q3CanvasItem::Rtti_Text).

    \sa Q3CanvasItem::rtti()
*/
int Q3CanvasText::rtti() const { return RTTI; }
int Q3CanvasText::RTTI = Rtti_Text;

/*!
    Returns 4 (Q3CanvasItem::Rtti_Polygon).

    \sa Q3CanvasItem::rtti()
*/
int Q3CanvasPolygon::rtti() const { return RTTI; }
int Q3CanvasPolygon::RTTI = Rtti_Polygon;

/*!
    Returns 5 (Q3CanvasItem::Rtti_Rectangle).

    \sa Q3CanvasItem::rtti()
*/
int Q3CanvasRectangle::rtti() const { return RTTI; }
int Q3CanvasRectangle::RTTI = Rtti_Rectangle;

/*!
    Returns 6 (Q3CanvasItem::Rtti_Ellipse).

    \sa Q3CanvasItem::rtti()
*/
int Q3CanvasEllipse::rtti() const { return RTTI; }
int Q3CanvasEllipse::RTTI = Rtti_Ellipse;

/*!
    Returns 7 (Q3CanvasItem::Rtti_Line).

    \sa Q3CanvasItem::rtti()
*/
int Q3CanvasLine::rtti() const { return RTTI; }
int Q3CanvasLine::RTTI = Rtti_Line;

/*!
    Returns 8 (Q3CanvasItem::Rtti_Spline).

    \sa Q3CanvasItem::rtti()
*/
int Q3CanvasSpline::rtti() const { return RTTI; }
int Q3CanvasSpline::RTTI = Rtti_Spline;

/*!
    Constructs a Q3CanvasSprite which uses images from the
    Q3CanvasPixmapArray \a a.

    The sprite in initially positioned at (0, 0) on \a canvas, using
    frame 0.
*/
Q3CanvasSprite::Q3CanvasSprite(Q3CanvasPixmapArray* a, Q3Canvas* canvas) :
    Q3CanvasItem(canvas),
    frm(0),
    anim_val(0),
    anim_state(0),
    anim_type(0),
    images(a)
{
}


/*!
    Set the array of images used for displaying the sprite to the
    Q3CanvasPixmapArray \a a.

    If the current frame() is larger than the number of images in \a
    a, the current frame will be reset to 0.
*/
void Q3CanvasSprite::setSequence(Q3CanvasPixmapArray* a)
{
    bool isvisible = isVisible();
    if (isvisible && images)
	hide();
    images = a;
    if (frm >= (int)images->count())
	frm = 0;
    if (isvisible)
	show();
}

/*!
\internal

Marks any chunks the sprite touches as changed.
*/
void Q3CanvasSprite::changeChunks()
{
    if (isVisible() && canvas()) {
	int chunksize=canvas()->chunkSize();
	for (int j=topEdge()/chunksize; j<=bottomEdge()/chunksize; j++) {
	    for (int i=leftEdge()/chunksize; i<=rightEdge()/chunksize; i++) {
		canvas()->setChangedChunk(i,j);
	    }
	}
    }
}

/*!
    Destroys the sprite and removes it from the canvas. Does \e not
    delete the images.
*/
Q3CanvasSprite::~Q3CanvasSprite()
{
    removeFromChunks();
}

/*!
    Sets the animation frame used for displaying the sprite to \a f,
    an index into the Q3CanvasSprite's Q3CanvasPixmapArray. The call
    will be ignored if \a f is larger than frameCount() or smaller
    than 0.

    \sa frame() move()
*/
void Q3CanvasSprite::setFrame(int f)
{
    move(x(),y(),f);
}

/*!
    \enum Q3CanvasSprite::FrameAnimationType

    This enum is used to identify the different types of frame
    animation offered by Q3CanvasSprite.

    \value Cycle at each advance the frame number will be incremented by
    1 (modulo the frame count).
    \value Oscillate at each advance the frame number will be
    incremented by 1 up to the frame count then decremented to by 1 to
    0, repeating this sequence forever.
*/

/*!
    Sets the animation characteristics for the sprite.

    For \a type == \c Cycle, the frames will increase by \a step
    at each advance, modulo the frameCount().

    For \a type == \c Oscillate, the frames will increase by \a step
    at each advance, up to the frameCount(), then decrease by \a step
    back to 0, repeating forever.

    The \a state parameter is for internal use.
*/
void Q3CanvasSprite::setFrameAnimation(FrameAnimationType type, int step, int state)
{
    anim_val = step;
    anim_type = type;
    anim_state = state;
    setAnimated(true);
}

/*!
    Extends the default Q3CanvasItem implementation to provide the
    functionality of setFrameAnimation().

    The \a phase is 0 or 1: see Q3CanvasItem::advance() for details.

    \sa Q3CanvasItem::advance() setVelocity()
*/
void Q3CanvasSprite::advance(int phase)
{
    if (phase==1) {
	int nf = frame();
	if (anim_type == Oscillate) {
	    if (anim_state)
		nf += anim_val;
	    else
		nf -= anim_val;
	    if (nf < 0) {
		nf = abs(anim_val);
		anim_state = !anim_state;
	    } else if (nf >= frameCount()) {
		nf = frameCount()-1-abs(anim_val);
		anim_state = !anim_state;
	    }
	} else {
	    nf = (nf + anim_val + frameCount()) % frameCount();
	}
	move(x()+xVelocity(),y()+yVelocity(),nf);
    }
}


/*!
    \fn int Q3CanvasSprite::frame() const

    Returns the index of the current animation frame in the
    Q3CanvasSprite's Q3CanvasPixmapArray.

    \sa setFrame(), move()
*/

/*!
    \fn int Q3CanvasSprite::frameCount() const

    Returns the number of frames in the Q3CanvasSprite's
    Q3CanvasPixmapArray.
*/


/*!
    Moves the sprite to (\a x, \a y).
*/
void Q3CanvasSprite::move(double x, double y) { Q3CanvasItem::move(x,y); }

/*!
    \fn void Q3CanvasSprite::move(double nx, double ny, int nf)

    Moves the sprite to (\a nx, \a ny) and sets the current
    frame to \a nf. \a nf will be ignored if it is larger than
    frameCount() or smaller than 0.
*/
void Q3CanvasSprite::move(double nx, double ny, int nf)
{
    if (isVisible() && canvas()) {
	hide();
	Q3CanvasItem::move(nx,ny);
	if (nf >= 0 && nf < frameCount())
	    frm=nf;
	show();
    } else {
	Q3CanvasItem::move(nx,ny);
	if (nf >= 0 && nf < frameCount())
	    frm=nf;
    }
}

class Q3CanvasPolygonScanner : public Q3PolygonScanner {
    QPolygonalProcessor& processor;
public:
    Q3CanvasPolygonScanner(QPolygonalProcessor& p) :
	processor(p)
    {
    }
    void processSpans(int n, QPoint* point, int* width)
    {
	processor.doSpans(n,point,width);
    }
};

void Q3CanvasPolygonalItem::scanPolygon(const Q3PointArray& pa, int winding, QPolygonalProcessor& process) const
{
    Q3CanvasPolygonScanner scanner(process);
    scanner.scan(pa,winding);
}

QT_END_NAMESPACE
