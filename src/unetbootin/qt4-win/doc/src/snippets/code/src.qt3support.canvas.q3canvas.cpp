//! [0]
    QPrinter pr;
    if (pr.setup()) {
	QPainter p(&pr);
	canvas.drawArea(canvas.rect(), &p);
    }
//! [0]


//! [1]
    void MyCanvasView::contentsMousePressEvent(QMouseEvent* e)
    {
        Q3CanvasItemList l = canvas()->collisions(e->pos());
        for (Q3CanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it) {
            if ((*it)->rtti() == Q3CanvasRectangle::RTTI)
                qDebug("A Q3CanvasRectangle lies somewhere at this point");
        }
    }
//! [1]


//! [2]
    QMatrix wm;
    wm.scale(2, 2);   // Zooms in by 2 times
    wm.rotate(90);    // Rotates 90 degrees counter clockwise
                      // around the origin.
    wm.translate(0, -canvas->height());
                      // moves the canvas down so what was visible
                      // before is still visible.
    myCanvasView->setWorldMatrix(wm);
//! [2]


//! [3]
    QRect rc = QRect(myCanvasView->contentsX(), myCanvasView->contentsY(),
                     myCanvasView->visibleWidth(), myCanvasView->visibleHeight());
    QRect canvasRect = myCanvasView->inverseWorldMatrix().mapRect(rc);
//! [3]


//! [4]
	Q3CanvasItem* item;
	// Find an item, e.g. with Q3CanvasItem::collisions().
	...
	if (item->rtti() == MySprite::RTTI) {
	    MySprite* s = (MySprite*)item;
	    if (s->isDamagable()) s->loseHitPoints(1000);
	    if (s->isHot()) myself->loseHitPoints(1000);
	    ...
	}
//! [4]


