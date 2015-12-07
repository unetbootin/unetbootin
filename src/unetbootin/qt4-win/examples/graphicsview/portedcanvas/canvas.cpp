/****************************************************************************
**
** Copyright (C) 2006-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
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

#include <qdatetime.h>
#include <qmainwindow.h>
#include <qstatusbar.h>
#include <qmessagebox.h>
#include <qmenubar.h>
#include <qapplication.h>
#include <qpainter.h>
#include <qprinter.h>
#include <qlabel.h>
#include <qimage.h>
#include <q3progressdialog.h>
#include <Q3PointArray>
#include <Q3PtrList>
#include <QPixmap>
#include <Q3PopupMenu>
#include <QMouseEvent>
#include <Q3MemArray>
#include "canvas.h"
#include <QStyleOptionGraphicsItem>
#include <qdebug.h>
#include <stdlib.h>

// We use a global variable to save memory - all the brushes and pens in
// the mesh are shared.
static QBrush *tb = 0;
static QPen *tp = 0;

class EdgeItem;
class NodeItem;

class EdgeItem: public QGraphicsLineItem
{
public:
    EdgeItem( NodeItem*, NodeItem* );
    void setFromPoint( int x, int y ) ;
    void setToPoint( int x, int y );
    static int count() { return c; }
private:
    static int c;
};

static const int imageRTTI = 984376;


class ImageItem: public QGraphicsRectItem
{
public:
    ImageItem( QImage img );
    int rtti () const { return imageRTTI; }
protected:
    void paint( QPainter *, const QStyleOptionGraphicsItem *option, QWidget *widget );
private:
    QImage image;
    QPixmap pixmap;
};


ImageItem::ImageItem( QImage img )
    : image(img)
{
    setRect(0, 0, image.width(), image.height());
    setFlag(ItemIsMovable);
#if !defined(Q_WS_QWS)
    pixmap.convertFromImage(image, Qt::OrderedAlphaDither);
#endif
}

void ImageItem::paint( QPainter *p, const QStyleOptionGraphicsItem *option, QWidget * )
{
// On Qt/Embedded, we can paint a QImage as fast as a QPixmap,
// but on other platforms, we need to use a QPixmap.
#if defined(Q_WS_QWS)
    p->drawImage( option->exposedRect, image, option->exposedRect, Qt::OrderedAlphaDither );
#else
    p->drawPixmap( option->exposedRect, pixmap, option->exposedRect );
#endif
}

class NodeItem: public QGraphicsEllipseItem
{
public:
    NodeItem();
    ~NodeItem() {}

    void addInEdge( EdgeItem *edge ) { inList.append( edge ); }
    void addOutEdge( EdgeItem *edge ) { outList.append( edge ); }

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);


    //    QPoint center() { return boundingRect().center(); }
private:
    Q3PtrList<EdgeItem> inList;
    Q3PtrList<EdgeItem> outList;
};


int EdgeItem::c = 0;

EdgeItem::EdgeItem( NodeItem *from, NodeItem *to )
    : QGraphicsLineItem( )
{
    c++;
    setPen( *tp );
    from->addOutEdge( this );
    to->addInEdge( this );
    setLine( QLineF(int(from->x()), int(from->y()), int(to->x()), int(to->y()) ));
    setZValue( 127 );
    setBoundingRegionGranularity(0.05);
}

void EdgeItem::setFromPoint( int x, int y )
{
    setLine(QLineF( x,y, line().p2().x(), line().p2().y() ));
}

void EdgeItem::setToPoint( int x, int y )
{
    setLine(QLineF( line().p1().x(), line().p1().y(), x, y ));
}

QVariant NodeItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionHasChanged) {
        Q3PtrListIterator<EdgeItem> it1( inList );
        EdgeItem *edge;
        while (( edge = it1.current() )) {
            ++it1;
            edge->setToPoint( int(x()), int(y()) );
        }
        Q3PtrListIterator<EdgeItem> it2( outList );
        while (( edge = it2.current() )) {
            ++it2;
            edge->setFromPoint( int(x()), int(y()) );
        }
    }

    return QGraphicsEllipseItem::itemChange(change, value);
}

NodeItem::NodeItem( )
    : QGraphicsEllipseItem( QRectF(-3, -3, 6, 6) )
{
    setPen( *tp );
    setBrush( *tb );
    setZValue( 128 );
    setFlag(ItemIsMovable);
}

FigureEditor::FigureEditor(
	QGraphicsScene& c, QWidget* parent,
	const char* name, Qt::WindowFlags f) :
    QGraphicsView(&c,parent)
{
    setObjectName(name);
    setWindowFlags(f);
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
}

void FigureEditor::clear()
{
    scene()->clear();
}

BouncyLogo::BouncyLogo() :
    xvel(0), yvel(0)
{
    setPixmap(QPixmap(":/trolltech/examples/graphicsview/portedcanvas/qt-trans.xpm"));
}

const int logo_rtti = 1234;

int BouncyLogo::type() const
{
    return logo_rtti;
}

QPainterPath BouncyLogo::shape() const
{
    QPainterPath path;
    path.addRect(boundingRect());
    return path;
}

void BouncyLogo::initPos()
{
    initSpeed();
    int trial=1000;
    do {
	setPos(qrand()%int(scene()->width()),qrand()%int(scene()->height()));
	advance(0);
    } while (trial-- && xvel==0.0 && yvel==0.0);
}

void BouncyLogo::initSpeed()
{
    const double speed = 4.0;
    double d = (double)(qrand()%1024) / 1024.0;
    xvel = d*speed*2-speed;
    yvel = (1-d)*speed*2-speed;
}

void BouncyLogo::advance(int stage)
{
    switch ( stage ) {
      case 0: {
	double vx = xvel;
	double vy = yvel;

	if ( vx == 0.0 && vy == 0.0 ) {
	    // stopped last turn
	    initSpeed();
	    vx = xvel;
	    vy = yvel;
	}

	double nx = x() + vx;
	double ny = y() + vy;

	if ( nx < 0 || nx >= scene()->width() )
	    vx = -vx;
	if ( ny < 0 || ny >= scene()->height() )
	    vy = -vy;

	for (int bounce=0; bounce<4; bounce++) {
	    QList<QGraphicsItem *> l=scene()->collidingItems(this);
            for (QList<QGraphicsItem *>::Iterator it=l.begin(); it!=l.end(); ++it) {
                QGraphicsItem *hit = *it;
                QPainterPath advancedShape = QMatrix().translate(xvel, yvel).map(shape());
                if ( hit->type()==logo_rtti && hit->collidesWithPath(mapToItem(hit, advancedShape)) ) {
		    switch ( bounce ) {
		      case 0:
			vx = -vx;
			break;
		      case 1:
			vy = -vy;
			vx = -vx;
			break;
		      case 2:
			vx = -vx;
			break;
		      case 3:
			// Stop for this turn
			vx = 0;
			vy = 0;
			break;
		    }
		    xvel = vx;
                    yvel = vy;
		    break;
		}
	    }
        }

	if ( x()+vx < 0 || x()+vx >= scene()->width() )
	    vx = 0;
	if ( y()+vy < 0 || y()+vy >= scene()->height() )
	    vy = 0;

	xvel = vx;
        yvel = vy;
      } break;
      case 1:
        moveBy(xvel, yvel);
	break;
    }
}

static uint mainCount = 0;
static QImage *butterflyimg;
static QImage *logoimg;

Main::Main(QGraphicsScene& c, QWidget* parent, const char* name, Qt::WindowFlags f) :
    Q3MainWindow(parent,name,f),
    canvas(c)
{
    editor = new FigureEditor(canvas,this);
    QMenuBar* menu = menuBar();

    Q3PopupMenu* file = new Q3PopupMenu( menu );
    file->insertItem("&Fill canvas", this, SLOT(init()), Qt::CTRL+Qt::Key_F);
    file->insertItem("&Erase canvas", this, SLOT(clear()), Qt::CTRL+Qt::Key_E);
    file->insertItem("&New view", this, SLOT(newView()), Qt::CTRL+Qt::Key_N);
    file->insertSeparator();
    file->insertItem("&Print...", this, SLOT(print()), Qt::CTRL+Qt::Key_P);
    file->insertSeparator();
    file->insertItem("E&xit", qApp, SLOT(quit()), Qt::CTRL+Qt::Key_Q);
    menu->insertItem("&File", file);

    Q3PopupMenu* edit = new Q3PopupMenu( menu );
    edit->insertItem("Add &Circle", this, SLOT(addCircle()), Qt::ALT+Qt::Key_C);
    edit->insertItem("Add &Hexagon", this, SLOT(addHexagon()), Qt::ALT+Qt::Key_H);
    edit->insertItem("Add &Polygon", this, SLOT(addPolygon()), Qt::ALT+Qt::Key_P);
    edit->insertItem("Add Spl&ine", this, SLOT(addSpline()), Qt::ALT+Qt::Key_I);
    edit->insertItem("Add &Text", this, SLOT(addText()), Qt::ALT+Qt::Key_T);
    edit->insertItem("Add &Line", this, SLOT(addLine()), Qt::ALT+Qt::Key_L);
    edit->insertItem("Add &Rectangle", this, SLOT(addRectangle()), Qt::ALT+Qt::Key_R);
    edit->insertItem("Add &Sprite", this, SLOT(addSprite()), Qt::ALT+Qt::Key_S);
    edit->insertItem("Create &Mesh", this, SLOT(addMesh()), Qt::ALT+Qt::Key_M );
    edit->insertItem("Add &Alpha-blended image", this, SLOT(addButterfly()), Qt::ALT+Qt::Key_A);
    menu->insertItem("&Edit", edit);

    Q3PopupMenu* view = new Q3PopupMenu( menu );
    view->insertItem("&Enlarge", this, SLOT(enlarge()), Qt::SHIFT+Qt::CTRL+Qt::Key_Plus);
    view->insertItem("Shr&ink", this, SLOT(shrink()), Qt::SHIFT+Qt::CTRL+Qt::Key_Minus);
    view->insertSeparator();
    view->insertItem("&Rotate clockwise", this, SLOT(rotateClockwise()), Qt::CTRL+Qt::Key_PageDown);
    view->insertItem("Rotate &counterclockwise", this, SLOT(rotateCounterClockwise()), Qt::CTRL+Qt::Key_PageUp);
    view->insertItem("&Zoom in", this, SLOT(zoomIn()), Qt::CTRL+Qt::Key_Plus);
    view->insertItem("Zoom &out", this, SLOT(zoomOut()), Qt::CTRL+Qt::Key_Minus);
    view->insertItem("Translate left", this, SLOT(moveL()), Qt::CTRL+Qt::Key_Left);
    view->insertItem("Translate right", this, SLOT(moveR()), Qt::CTRL+Qt::Key_Right);
    view->insertItem("Translate up", this, SLOT(moveU()), Qt::CTRL+Qt::Key_Up);
    view->insertItem("Translate down", this, SLOT(moveD()), Qt::CTRL+Qt::Key_Down);
    view->insertItem("&Mirror", this, SLOT(mirror()), Qt::CTRL+Qt::Key_Home);
    menu->insertItem("&View", view);

    menu->insertSeparator();

    Q3PopupMenu* help = new Q3PopupMenu( menu );
    help->insertItem("&About", this, SLOT(help()), Qt::Key_F1);
    help->setItemChecked(dbf_id, TRUE);
    menu->insertItem("&Help",help);

    statusBar();

    setCentralWidget(editor);

    printer = 0;

    init();
}

void Main::init()
{
    clear();

    static int r=24;
    qsrand(++r);

    mainCount++;
    butterflyimg = 0;
    logoimg = 0;

    int i;
    for ( i=0; i < int(canvas.width()) / 56; i++) {
	addButterfly();
    }
    for ( i=0; i < int(canvas.width()) / 85; i++) {
	addHexagon();
    }
    for ( i=0; i < int(canvas.width()) / 128; i++) {
	addLogo();
    }
}

Main::~Main()
{
    delete printer;
    if ( !--mainCount ) {
	delete[] butterflyimg;
	butterflyimg = 0;
	delete[] logoimg;
	logoimg = 0;
    }
}

void Main::newView()
{
    // Open a new view... have it delete when closed.
    Main *m = new Main(canvas, 0, 0, Qt::WDestructiveClose);
    m->show();
}

void Main::clear()
{
    editor->clear();
}

void Main::help()
{
    static QMessageBox* about = new QMessageBox( "Qt Canvas Example",
	    "<h3>The QCanvas classes example</h3>"
	    "<ul>"
		"<li> Press ALT-S for some sprites."
		"<li> Press ALT-C for some circles."
		"<li> Press ALT-L for some lines."
		"<li> Drag the objects around."
		"<li> Read the code!"
	    "</ul>", QMessageBox::Information, 1, 0, 0, this, 0, FALSE );
    about->setButtonText( 1, "Dismiss" );
    about->show();
}

void Main::aboutQt()
{
    QMessageBox::aboutQt( this, "Qt Canvas Example" );
}

void Main::enlarge()
{
    canvas.setSceneRect(0, 0, canvas.width()*4/3, canvas.height()*4/3);
}

void Main::shrink()
{
    canvas.setSceneRect(0, 0, qMax(canvas.width()*3/4, qreal(1.0)), qMax(canvas.height()*3/4, qreal(1.0)));
}

void Main::rotateClockwise()
{
    editor->rotate( 22.5 );
}

void Main::rotateCounterClockwise()
{
    editor->rotate( -22.5 );
}

void Main::zoomIn()
{
    editor->scale( 2.0, 2.0 );
}

void Main::zoomOut()
{
    editor->scale( 0.5, 0.5 );
}

void Main::mirror()
{
    editor->scale( -1, 1 );
}

void Main::moveL()
{
    editor->translate( -16, 0 );
}

void Main::moveR()
{
    editor->translate( +16, 0 );
}

void Main::moveU()
{
    editor->translate( 0, -16 );
}

void Main::moveD()
{
    editor->translate( 0, +16 );
}

void Main::print()
{
    if ( !printer ) printer = new QPrinter;
    if ( printer->setup(this) ) {
	QPainter pp(printer);
        canvas.render(&pp);
    }
}


void Main::addSprite()
{
    BouncyLogo* i = new BouncyLogo;
    canvas.addItem(i);
    i->initPos();
    i->setZValue(qrand()%256);
}

QString butterfly_fn;
QString logo_fn;


void Main::addButterfly()
{
    if ( butterfly_fn.isEmpty() )
	return;
    if ( !butterflyimg ) {
	butterflyimg = new QImage[4];
	butterflyimg[0].load( butterfly_fn );
	butterflyimg[1] = butterflyimg[0].smoothScale( int(butterflyimg[0].width()*0.75),
		int(butterflyimg[0].height()*0.75) );
	butterflyimg[2] = butterflyimg[0].smoothScale( int(butterflyimg[0].width()*0.5),
		int(butterflyimg[0].height()*0.5) );
	butterflyimg[3] = butterflyimg[0].smoothScale( int(butterflyimg[0].width()*0.25),
		int(butterflyimg[0].height()*0.25) );
    }
    QAbstractGraphicsShapeItem* i = new ImageItem(butterflyimg[qrand()%4]);
    canvas.addItem(i);
    i->setPos(qrand()%int(canvas.width()-butterflyimg->width()),
	    qrand()%int(canvas.height()-butterflyimg->height()));
    i->setZValue(qrand()%256+250);
}

void Main::addLogo()
{
    if ( logo_fn.isEmpty() )
	return;
    if ( !logoimg ) {
	logoimg = new QImage[4];
	logoimg[0].load( logo_fn );
	logoimg[1] = logoimg[0].smoothScale( int(logoimg[0].width()*0.75),
		int(logoimg[0].height()*0.75) );
	logoimg[2] = logoimg[0].smoothScale( int(logoimg[0].width()*0.5),
		int(logoimg[0].height()*0.5) );
	logoimg[3] = logoimg[0].smoothScale( int(logoimg[0].width()*0.25),
		int(logoimg[0].height()*0.25) );
    }
    QAbstractGraphicsShapeItem* i = new ImageItem(logoimg[qrand()%4]);
    canvas.addItem(i);
    i->setPos(qrand()%int(canvas.width()-logoimg->width()),
	    qrand()%int(canvas.height()-logoimg->width()));
    i->setZValue(qrand()%256+256);
}



void Main::addCircle()
{
    QAbstractGraphicsShapeItem* i = canvas.addEllipse(QRectF(0,0,50,50));
    i->setFlag(QGraphicsItem::ItemIsMovable);
    i->setPen(Qt::NoPen);
    i->setBrush( QColor(qrand()%32*8,qrand()%32*8,qrand()%32*8) );
    i->setPos(qrand()%int(canvas.width()),qrand()%int(canvas.height()));
    i->setZValue(qrand()%256);
}

void Main::addHexagon()
{
    const int size = int(canvas.width() / 25);
    Q3PointArray pa(6);
    pa[0] = QPoint(2*size,0);
    pa[1] = QPoint(size,-size*173/100);
    pa[2] = QPoint(-size,-size*173/100);
    pa[3] = QPoint(-2*size,0);
    pa[4] = QPoint(-size,size*173/100);
    pa[5] = QPoint(size,size*173/100);
    QGraphicsPolygonItem* i = canvas.addPolygon(pa);
    i->setFlag(QGraphicsItem::ItemIsMovable);
    i->setPen(Qt::NoPen);
    i->setBrush( QColor(qrand()%32*8,qrand()%32*8,qrand()%32*8) );
    i->setPos(qrand()%int(canvas.width()),qrand()%int(canvas.height()));
    i->setZValue(qrand()%256);
}

void Main::addPolygon()
{
    const int size = int(canvas.width()/2);
    Q3PointArray pa(6);
    pa[0] = QPoint(0,0);
    pa[1] = QPoint(size,size/5);
    pa[2] = QPoint(size*4/5,size);
    pa[3] = QPoint(size/6,size*5/4);
    pa[4] = QPoint(size*3/4,size*3/4);
    pa[5] = QPoint(size*3/4,size/4);
    QGraphicsPolygonItem* i = canvas.addPolygon(pa);
    i->setFlag(QGraphicsItem::ItemIsMovable);
    i->setPen(Qt::NoPen);
    i->setBrush( QColor(qrand()%32*8,qrand()%32*8,qrand()%32*8) );
    i->setPos(qrand()%int(canvas.width()),qrand()%int(canvas.height()));
    i->setZValue(qrand()%256);
}

void Main::addSpline()
{
    const int size = int(canvas.width()/6);

    Q3PointArray pa(12);
    pa[0] = QPoint(0,0);
    pa[1] = QPoint(size/2,0);
    pa[2] = QPoint(size,size/2);
    pa[3] = QPoint(size,size);
    pa[4] = QPoint(size,size*3/2);
    pa[5] = QPoint(size/2,size*2);
    pa[6] = QPoint(0,size*2);
    pa[7] = QPoint(-size/2,size*2);
    pa[8] = QPoint(size/4,size*3/2);
    pa[9] = QPoint(0,size);
    pa[10]= QPoint(-size/4,size/2);
    pa[11]= QPoint(-size/2,0);

    QPainterPath path;
    path.moveTo(pa[0]);
    for (int i = 1; i < pa.size(); i += 3)
        path.cubicTo(pa[i], pa[(i + 1) % pa.size()], pa[(i + 2) % pa.size()]);

    QGraphicsPathItem* item = canvas.addPath(path);
    item->setFlag(QGraphicsItem::ItemIsMovable);
    item->setPen(Qt::NoPen);
    item->setBrush( QColor(qrand()%32*8,qrand()%32*8,qrand()%32*8) );
    item->setPos(qrand()%int(canvas.width()),qrand()%int(canvas.height()));
    item->setZValue(qrand()%256);
}

void Main::addText()
{
    QGraphicsTextItem* i = canvas.addText("QCanvasText");
    i->setFlag(QGraphicsItem::ItemIsMovable);
    i->setPos(qrand()%int(canvas.width()),qrand()%int(canvas.height()));
    i->setZValue(qrand()%256);
}

void Main::addLine()
{
    QGraphicsLineItem* i = canvas.addLine(QLineF( qrand()%int(canvas.width()), qrand()%int(canvas.height()),
                                                  qrand()%int(canvas.width()), qrand()%int(canvas.height()) ));
    i->setFlag(QGraphicsItem::ItemIsMovable);
    i->setPen( QPen(QColor(qrand()%32*8,qrand()%32*8,qrand()%32*8), 6) );
    i->setZValue(qrand()%256);
}

void Main::addMesh()
{
    int x0 = 0;
    int y0 = 0;

    if ( !tb ) tb = new QBrush( Qt::red );
    if ( !tp ) tp = new QPen( Qt::black );

    int nodecount = 0;

    int w = int(canvas.width());
    int h = int(canvas.height());

    const int dist = 30;
    int rows = h / dist;
    int cols = w / dist;

#ifndef QT_NO_PROGRESSDIALOG
    Q3ProgressDialog progress( "Creating mesh...", "Abort", rows,
			      this, "progress", TRUE );
#endif

    canvas.update();
    
    Q3MemArray<NodeItem*> lastRow(cols);
    for ( int j = 0; j < rows; j++ ) {
	int n = j%2 ? cols-1 : cols;
	NodeItem *prev = 0;
	for ( int i = 0; i < n; i++ ) {
	    NodeItem *el = new NodeItem;
            canvas.addItem(el);
	    nodecount++;
	    int r = qrand();
	    int xrand = r %20;
	    int yrand = (r/20) %20;
	    el->setPos( xrand + x0 + i*dist + (j%2 ? dist/2 : 0 ),
                        yrand + y0 + j*dist );

	    if ( j > 0 ) {
		if ( i < cols-1 )
		    canvas.addItem(new EdgeItem( lastRow[i], el));
		if ( j%2 )
		    canvas.addItem(new EdgeItem( lastRow[i+1], el));
		else if ( i > 0 )
		    canvas.addItem(new EdgeItem( lastRow[i-1], el));
	    }
	    if ( prev ) {
		canvas.addItem(new EdgeItem( prev, el));
	    }
	    if ( i > 0 ) lastRow[i-1] = prev;
	    prev = el;
	}
	lastRow[n-1]=prev;
#ifndef QT_NO_PROGRESSDIALOG
	progress.setProgress( j );
	if ( progress.wasCancelled() )
	    break;
#endif
    }
#ifndef QT_NO_PROGRESSDIALOG
    progress.setProgress( rows );
#endif
    // qDebug( "%d nodes, %d edges", nodecount, EdgeItem::count() );
}

void Main::addRectangle()
{
    QAbstractGraphicsShapeItem *i = canvas.addRect( QRectF(qrand()%int(canvas.width()),
                                                          qrand()%int(canvas.height()),
                                                          canvas.width()/5,
                                                          canvas.width()/5) );
    i->setFlag(QGraphicsItem::ItemIsMovable);
    int z = qrand()%256;
    i->setBrush( QColor(z,z,z) );
    i->setPen( QPen(QColor(qrand()%32*8,qrand()%32*8,qrand()%32*8), 6) );
    i->setZValue(z);
}
