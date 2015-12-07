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

#ifndef Q3CANVAS_H
#define Q3CANVAS_H

#include <Qt3Support/q3scrollview.h>
#include <QtGui/qpixmap.h>
#include <Qt3Support/q3ptrlist.h>
#include <QtGui/qbrush.h>
#include <QtGui/qpen.h>
#include <Qt3Support/q3valuelist.h>
#include <Qt3Support/q3pointarray.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3Support)

class Q3CanvasSprite;
class Q3CanvasPolygonalItem;
class Q3CanvasRectangle;
class Q3CanvasPolygon;
class Q3CanvasEllipse;
class Q3CanvasText;
class Q3CanvasLine;
class Q3CanvasChunk;
class Q3Canvas;
class Q3CanvasItem;
class Q3CanvasView;
class Q3CanvasPixmap;

class Q_COMPAT_EXPORT Q3CanvasItemList : public Q3ValueList<Q3CanvasItem*> {
public:
    void sort();
    void drawUnique(QPainter& painter);
    Q3CanvasItemList operator+(const Q3CanvasItemList &l) const;
};


class Q3CanvasItemExtra;

class Q_COMPAT_EXPORT Q3CanvasItem
{
public:
    Q3CanvasItem(Q3Canvas* canvas);
    virtual ~Q3CanvasItem();

    double x() const
	{ return myx; }
    double y() const
	{ return myy; }
    double z() const
	{ return myz; } // (depth)

    virtual void moveBy(double dx, double dy);
    void move(double x, double y);
    void setX(double a) { move(a,y()); }
    void setY(double a) { move(x(),a); }
    void setZ(double a) { myz=a; changeChunks(); }

    bool animated() const;
    virtual void setAnimated(bool y);
    virtual void setVelocity(double vx, double vy);
    void setXVelocity(double vx) { setVelocity(vx,yVelocity()); }
    void setYVelocity(double vy) { setVelocity(xVelocity(),vy); }
    double xVelocity() const;
    double yVelocity() const;
    virtual void advance(int stage);

    virtual bool collidesWith(const Q3CanvasItem*) const=0;

    Q3CanvasItemList collisions(bool exact /* NO DEFAULT */) const;

    virtual void setCanvas(Q3Canvas*);

    virtual void draw(QPainter&)=0;

    void show();
    void hide();

    virtual void setVisible(bool yes);
    bool isVisible() const
	{ return (bool)vis; }
    virtual void setSelected(bool yes);
    bool isSelected() const
	{ return (bool)sel; }
    virtual void setEnabled(bool yes);
    bool isEnabled() const
	{ return (bool)ena; }
    virtual void setActive(bool yes);
    bool isActive() const
	{ return (bool)act; }
    bool visible() const
	{ return (bool)vis; }
    bool selected() const
	{ return (bool)sel; }
    bool enabled() const
	{ return (bool)ena; }
    bool active() const
	{ return (bool)act; }

    enum RttiValues {
	Rtti_Item = 0,
	Rtti_Sprite = 1,
	Rtti_PolygonalItem = 2,
	Rtti_Text = 3,
	Rtti_Polygon = 4,
	Rtti_Rectangle = 5,
	Rtti_Ellipse = 6,
	Rtti_Line = 7,
	Rtti_Spline = 8
    };

    virtual int rtti() const;
    static int RTTI;

    virtual QRect boundingRect() const=0;
    virtual QRect boundingRectAdvanced() const;

    Q3Canvas* canvas() const
	{ return cnv; }

protected:
    void update() { changeChunks(); }

private:
    // For friendly subclasses...

    friend class Q3CanvasPolygonalItem;
    friend class Q3CanvasSprite;
    friend class Q3CanvasRectangle;
    friend class Q3CanvasPolygon;
    friend class Q3CanvasEllipse;
    friend class Q3CanvasText;
    friend class Q3CanvasLine;

    virtual Q3PointArray chunks() const;
    virtual void addToChunks();
    virtual void removeFromChunks();
    virtual void changeChunks();
    virtual bool collidesWith(const Q3CanvasSprite*,
			       const Q3CanvasPolygonalItem*,
			       const Q3CanvasRectangle*,
			       const Q3CanvasEllipse*,
			       const Q3CanvasText*) const = 0;
    // End of friend stuff

    Q3Canvas* cnv;
    static Q3Canvas* current_canvas;
    double myx,myy,myz;
    Q3CanvasItemExtra *ext;
    Q3CanvasItemExtra& extra();
    uint ani:1;
    uint vis:1;
    uint val:1;
    uint sel:1;
    uint ena:1;
    uint act:1;
};


class Q3CanvasData;

class Q_COMPAT_EXPORT Q3Canvas : public QObject
{
    Q_OBJECT
public:
    Q3Canvas(QObject* parent = 0, const char* name = 0);
    Q3Canvas(int w, int h);
    Q3Canvas(QPixmap p, int h, int v, int tilewidth, int tileheight);

    virtual ~Q3Canvas();

    virtual void setTiles(QPixmap tiles, int h, int v,
			   int tilewidth, int tileheight);
    virtual void setBackgroundPixmap(const QPixmap& p);
    QPixmap backgroundPixmap() const;

    virtual void setBackgroundColor(const QColor& c);
    QColor backgroundColor() const;

    virtual void setTile(int x, int y, int tilenum);
    int tile(int x, int y) const
	{ return grid[x+y*htiles]; }

    int tilesHorizontally() const
	{ return htiles; }
    int tilesVertically() const
	{ return vtiles; }

    int tileWidth() const
	{ return tilew; }
    int tileHeight() const
	{ return tileh; }

    virtual void resize(int width, int height);
    int width() const
	{ return awidth; }
    int height() const
	{ return aheight; }
    QSize size() const
	{ return QSize(awidth,aheight); }
    QRect rect() const
	{ return QRect(0, 0, awidth, aheight); }
    bool onCanvas(int x, int y) const
	{ return x>=0 && y>=0 && x<awidth && y<aheight; }
    bool onCanvas(const QPoint& p) const
	{ return onCanvas(p.x(),p.y()); }
    bool validChunk(int x, int y) const
	{ return x>=0 && y>=0 && x<chwidth && y<chheight; }
    bool validChunk(const QPoint& p) const
	{ return validChunk(p.x(),p.y()); }

    int chunkSize() const
	{ return chunksize; }
    virtual void retune(int chunksize, int maxclusters=100);

    bool sameChunk(int x1, int y1, int x2, int y2) const
	{ return x1/chunksize==x2/chunksize && y1/chunksize==y2/chunksize; }
    virtual void setChangedChunk(int i, int j);
    virtual void setChangedChunkContaining(int x, int y);
    virtual void setAllChanged();
    virtual void setChanged(const QRect& area);
    virtual void setUnchanged(const QRect& area);

    // These call setChangedChunk.
    void addItemToChunk(Q3CanvasItem*, int i, int j);
    void removeItemFromChunk(Q3CanvasItem*, int i, int j);
    void addItemToChunkContaining(Q3CanvasItem*, int x, int y);
    void removeItemFromChunkContaining(Q3CanvasItem*, int x, int y);

    Q3CanvasItemList allItems();
    Q3CanvasItemList collisions(const QPoint&) const;
    Q3CanvasItemList collisions(const QRect&) const;
    Q3CanvasItemList collisions(const Q3PointArray& pa, const Q3CanvasItem* item,
				bool exact) const;

    void drawArea(const QRect&, QPainter* p, bool double_buffer=false);

    // These are for Q3CanvasView to call
    virtual void addView(Q3CanvasView*);
    virtual void removeView(Q3CanvasView*);
    void drawCanvasArea(const QRect&, QPainter* p=0, bool double_buffer=true);
    void drawViewArea(Q3CanvasView* view, QPainter* p, const QRect& r, bool dbuf);

    // These are for Q3CanvasItem to call
    virtual void addItem(Q3CanvasItem*);
    virtual void addAnimation(Q3CanvasItem*);
    virtual void removeItem(Q3CanvasItem*);
    virtual void removeAnimation(Q3CanvasItem*);

    virtual void setAdvancePeriod(int ms);
    virtual void setUpdatePeriod(int ms);

    virtual void setDoubleBuffering(bool y);

Q_SIGNALS:
    void resized();

public Q_SLOTS:
    virtual void advance();
    virtual void update();

protected:
    virtual void drawBackground(QPainter&, const QRect& area);
    virtual void drawForeground(QPainter&, const QRect& area);

private:
    void init(int w, int h, int chunksze=16, int maxclust=100);

    Q3CanvasChunk& chunk(int i, int j) const;
    Q3CanvasChunk& chunkContaining(int x, int y) const;

    QRect changeBounds(const QRect& inarea);

    void ensureOffScrSize(int osw, int osh);
    QPixmap offscr;
    int awidth,aheight;
    int chunksize;
    int maxclusters;
    int chwidth,chheight;
    Q3CanvasChunk* chunks;

    Q3CanvasData* d;

    void initTiles(QPixmap p, int h, int v, int tilewidth, int tileheight);
    ushort *grid;
    ushort htiles;
    ushort vtiles;
    ushort tilew;
    ushort tileh;
    bool oneone;
    QPixmap pm;
    QTimer* update_timer;
    QColor bgcolor;
    bool debug_redraw_areas;
    bool dblbuf;

    friend void qt_unview(Q3Canvas* c);

    Q_DISABLE_COPY(Q3Canvas)
};

class Q3CanvasViewData;

class Q_COMPAT_EXPORT Q3CanvasView : public Q3ScrollView
{
    Q_OBJECT
public:

    Q3CanvasView(QWidget* parent=0, const char* name=0, Qt::WindowFlags f=0);
    Q3CanvasView(Q3Canvas* viewing, QWidget* parent=0, const char* name=0, Qt::WindowFlags f=0);
    ~Q3CanvasView();

    Q3Canvas* canvas() const
	{ return viewing; }
    void setCanvas(Q3Canvas* v);

    const QMatrix &worldMatrix() const;
    const QMatrix &inverseWorldMatrix() const;
    bool setWorldMatrix(const QMatrix &);

protected:
    void drawContents(QPainter *p, int cx, int cy, int cw, int ch);
    QSize sizeHint() const;

private:
    friend class Q3Canvas;
    void drawContents(QPainter*);
    Q3Canvas* viewing;
    Q3CanvasViewData* d;
    friend void qt_unview(Q3Canvas* c);

private Q_SLOTS:
    void updateContentsSize();

private:
    Q_DISABLE_COPY(Q3CanvasView)
};


class Q_COMPAT_EXPORT Q3CanvasPixmap : public QPixmap
{
public:
#ifndef QT_NO_IMAGEIO
    Q3CanvasPixmap(const QString& datafilename);
#endif
    Q3CanvasPixmap(const QImage& image);
    Q3CanvasPixmap(const QPixmap&, const QPoint& hotspot);
    ~Q3CanvasPixmap();

    int offsetX() const
	{ return hotx; }
    int offsetY() const
	{ return hoty; }
    void setOffset(int x, int y) { hotx = x; hoty = y; }

private:
    Q_DISABLE_COPY(Q3CanvasPixmap)

    void init(const QImage&);
    void init(const QPixmap& pixmap, int hx, int hy);

    friend class Q3CanvasSprite;
    friend class Q3CanvasPixmapArray;
    friend bool qt_testCollision(const Q3CanvasSprite* s1, const Q3CanvasSprite* s2);

    int hotx,hoty;

    QImage* collision_mask;
};


class Q_COMPAT_EXPORT Q3CanvasPixmapArray
{
public:
    Q3CanvasPixmapArray();
#ifndef QT_NO_IMAGEIO
    Q3CanvasPixmapArray(const QString& datafilenamepattern, int framecount=0);
#endif
    // this form is deprecated
    Q3CanvasPixmapArray(Q3PtrList<QPixmap>, Q3PtrList<QPoint> hotspots);

    Q3CanvasPixmapArray(Q3ValueList<QPixmap>, Q3PointArray hotspots = Q3PointArray());
    ~Q3CanvasPixmapArray();

#ifndef QT_NO_IMAGEIO
    bool readPixmaps(const QString& datafilenamepattern, int framecount=0);
    bool readCollisionMasks(const QString& filenamepattern);
#endif

    // deprecated
    bool operator!(); // Failure check.
    bool isValid() const;

    Q3CanvasPixmap* image(int i) const
	{ return img ? img[i] : 0; }
    void setImage(int i, Q3CanvasPixmap* p);
    uint count() const
	{ return (uint)framecount; }

private:
    Q_DISABLE_COPY(Q3CanvasPixmapArray)

#ifndef QT_NO_IMAGEIO
    bool readPixmaps(const QString& datafilenamepattern, int framecount, bool maskonly);
#endif

    void reset();
    int framecount;
    Q3CanvasPixmap** img;
};


class Q_COMPAT_EXPORT Q3CanvasSprite : public Q3CanvasItem
{
public:
    Q3CanvasSprite(Q3CanvasPixmapArray* array, Q3Canvas* canvas);

    void setSequence(Q3CanvasPixmapArray* seq);

    virtual ~Q3CanvasSprite();

    void move(double x, double y);
    virtual void move(double x, double y, int frame);
    void setFrame(int);
    enum FrameAnimationType { Cycle, Oscillate };
    virtual void setFrameAnimation(FrameAnimationType=Cycle, int step=1, int state=0);
    int frame() const
	{ return frm; }
    int frameCount() const
	{ return images->count(); }

    int rtti() const;
    static int RTTI;

    bool collidesWith(const Q3CanvasItem*) const;

    QRect boundingRect() const;

    // is there a reason for these to be protected? Lars
//protected:

    int width() const;
    int height() const;

    int leftEdge() const;
    int topEdge() const;
    int rightEdge() const;
    int bottomEdge() const;

    int leftEdge(int nx) const;
    int topEdge(int ny) const;
    int rightEdge(int nx) const;
    int bottomEdge(int ny) const;
    Q3CanvasPixmap* image() const
	{ return images->image(frm); }
    virtual Q3CanvasPixmap* imageAdvanced() const;
    Q3CanvasPixmap* image(int f) const
	{ return images->image(f); }
    virtual void advance(int stage);

public:
    void draw(QPainter& painter);

private:
    Q_DISABLE_COPY(Q3CanvasSprite)

    void addToChunks();
    void removeFromChunks();
    void changeChunks();

    int frm;
    ushort anim_val;
    uint anim_state:2;
    uint anim_type:14;
    bool collidesWith(const Q3CanvasSprite*,
		       const Q3CanvasPolygonalItem*,
		       const Q3CanvasRectangle*,
		       const Q3CanvasEllipse*,
		       const Q3CanvasText*) const;

    friend bool qt_testCollision(const Q3CanvasSprite* s1,
				  const Q3CanvasSprite* s2);

    Q3CanvasPixmapArray* images;
};

class QPolygonalProcessor;

class Q_COMPAT_EXPORT Q3CanvasPolygonalItem : public Q3CanvasItem
{
public:
    Q3CanvasPolygonalItem(Q3Canvas* canvas);
    virtual ~Q3CanvasPolygonalItem();

    bool collidesWith(const Q3CanvasItem*) const;

    virtual void setPen(QPen p);
    virtual void setBrush(QBrush b);

    QPen pen() const
	{ return pn; }
    QBrush brush() const
	{ return br; }

    virtual Q3PointArray areaPoints() const=0;
    virtual Q3PointArray areaPointsAdvanced() const;
    QRect boundingRect() const;

    int rtti() const;
    static int RTTI;

protected:
    void draw(QPainter &);
    virtual void drawShape(QPainter &) = 0;

    bool winding() const;
    void setWinding(bool);

    void invalidate();
    bool isValid() const
	{ return (bool)val; }

private:
    void scanPolygon(const Q3PointArray& pa, int winding,
		      QPolygonalProcessor& process) const;
    Q3PointArray chunks() const;

    bool collidesWith(const Q3CanvasSprite*,
		       const Q3CanvasPolygonalItem*,
		       const Q3CanvasRectangle*,
		       const Q3CanvasEllipse*,
		       const Q3CanvasText*) const;

    QBrush br;
    QPen pn;
    uint wind:1;
};


class Q_COMPAT_EXPORT Q3CanvasRectangle : public Q3CanvasPolygonalItem
{
public:
    Q3CanvasRectangle(Q3Canvas* canvas);
    Q3CanvasRectangle(const QRect&, Q3Canvas* canvas);
    Q3CanvasRectangle(int x, int y, int width, int height, Q3Canvas* canvas);

    ~Q3CanvasRectangle();

    int width() const;
    int height() const;
    void setSize(int w, int h);
    QSize size() const
	{ return QSize(w,h); }
    Q3PointArray areaPoints() const;
    QRect rect() const
	{ return QRect(int(x()),int(y()),w,h); }

    bool collidesWith(const Q3CanvasItem*) const;

    int rtti() const;
    static int RTTI;

protected:
    void drawShape(QPainter &);
    Q3PointArray chunks() const;

private:
    bool collidesWith(  const Q3CanvasSprite*,
			 const Q3CanvasPolygonalItem*,
			 const Q3CanvasRectangle*,
			 const Q3CanvasEllipse*,
			 const Q3CanvasText*) const;

    int w, h;
};


class Q_COMPAT_EXPORT Q3CanvasPolygon : public Q3CanvasPolygonalItem
{
public:
    Q3CanvasPolygon(Q3Canvas* canvas);
    ~Q3CanvasPolygon();
    void setPoints(Q3PointArray);
    Q3PointArray points() const;
    void moveBy(double dx, double dy);

    Q3PointArray areaPoints() const;

    int rtti() const;
    static int RTTI;

protected:
    void drawShape(QPainter &);
    Q3PointArray poly;
};


class Q_COMPAT_EXPORT Q3CanvasSpline : public Q3CanvasPolygon
{
public:
    Q3CanvasSpline(Q3Canvas* canvas);
    ~Q3CanvasSpline();

    void setControlPoints(Q3PointArray, bool closed=true);
    Q3PointArray controlPoints() const;
    bool closed() const;

    int rtti() const;
    static int RTTI;

private:
    void recalcPoly();
    Q3PointArray bez;
    bool cl;
};


class Q_COMPAT_EXPORT Q3CanvasLine : public Q3CanvasPolygonalItem
{
public:
    Q3CanvasLine(Q3Canvas* canvas);
    ~Q3CanvasLine();
    void setPoints(int x1, int y1, int x2, int y2);

    QPoint startPoint() const
	{ return QPoint(x1,y1); }
    QPoint endPoint() const
	{ return QPoint(x2,y2); }

    int rtti() const;
    static int RTTI;

    void setPen(QPen p);
    void moveBy(double dx, double dy);

protected:
    void drawShape(QPainter &);
    Q3PointArray areaPoints() const;

private:
    int x1,y1,x2,y2;
};


class Q_COMPAT_EXPORT Q3CanvasEllipse : public Q3CanvasPolygonalItem
{

public:
    Q3CanvasEllipse(Q3Canvas* canvas);
    Q3CanvasEllipse(int width, int height, Q3Canvas* canvas);
    Q3CanvasEllipse(int width, int height, int startangle, int angle,
		    Q3Canvas* canvas);

    ~Q3CanvasEllipse();

    int width() const;
    int height() const;
    void setSize(int w, int h);
    void setAngles(int start, int length);
    int angleStart() const
	{ return a1; }
    int angleLength() const
	{ return a2; }
    Q3PointArray areaPoints() const;

    bool collidesWith(const Q3CanvasItem*) const;

    int rtti() const;
    static int RTTI;

protected:
    void drawShape(QPainter &);

private:
    bool collidesWith(const Q3CanvasSprite*,
		       const Q3CanvasPolygonalItem*,
		       const Q3CanvasRectangle*,
		       const Q3CanvasEllipse*,
		       const Q3CanvasText*) const;
    int w, h;
    int a1, a2;
};


class Q3CanvasTextExtra;

class Q_COMPAT_EXPORT Q3CanvasText : public Q3CanvasItem
{
public:
    Q3CanvasText(Q3Canvas* canvas);
    Q3CanvasText(const QString&, Q3Canvas* canvas);
    Q3CanvasText(const QString&, QFont, Q3Canvas* canvas);

    virtual ~Q3CanvasText();

    void setText(const QString&);
    void setFont(const QFont&);
    void setColor(const QColor&);
    QString text() const;
    QFont font() const;
    QColor color() const;

    void moveBy(double dx, double dy);

    int textFlags() const
	{ return flags; }
    void setTextFlags(int);

    QRect boundingRect() const;

    bool collidesWith(const Q3CanvasItem*) const;

    int rtti() const;
    static int RTTI;

protected:
    virtual void draw(QPainter&);

private:
    Q_DISABLE_COPY(Q3CanvasText)

    void addToChunks();
    void removeFromChunks();
    void changeChunks();

    void setRect();
    QRect brect;
    QString txt;
    int flags;
    QFont fnt;
    QColor col;
    Q3CanvasTextExtra* extra;

    bool collidesWith(const Q3CanvasSprite*,
                      const Q3CanvasPolygonalItem*,
                      const Q3CanvasRectangle*,
                      const Q3CanvasEllipse*,
                      const Q3CanvasText*) const;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // Q3CANVAS_H
