//! [0]
        QPainterPath path;
        path.addRect(20, 20, 60, 60);

        path.moveTo(0, 0);
        path.cubicTo(99, 0,  50, 50,  99, 99);
        path.cubicTo(0, 99,  50, 50,  0, 0);

        QPainter painter(this);
        painter.fillRect(0, 0, 100, 100, Qt::white);
        painter.setPen(QPen(QColor(79, 106, 25), 1, Qt::SolidLine,
                            Qt::FlatCap, Qt::MiterJoin));
        painter.setBrush(QColor(122, 163, 39));

        painter.drawPath(path);
//! [0]


//! [1]
        QLinearGradient myGradient;
        QPen myPen;

        QPainterPath myPath;
        myPath.cubicto(c1, c2, endPoint);

        QPainter painter(this);
        painter.setBrush(myGradient);
        painter.setPen(myPen);
        painter.drawPath(myPath);
//! [1]


//! [2]
        QLinearGradient myGradient;
        QPen myPen;

        QPointF center, startPoint;

        QPainterPath myPath;
        myPath.moveTo(center);
        myPath.arcTo(boundingRect, startAngle,
                     sweepLength);

        QPainter painter(this);
        painter.setBrush(myGradient);
        painter.setPen(myPen);
        painter.drawPath(myPath);
//! [2]


//! [3]
        QLinearGradient myGradient;
        QPen myPen;
        QRectF myRectangle;

        QPainterPath myPath;
        myPath.addRect(myRectangle);

        QPainter painter(this);
        painter.setBrush(myGradient);
        painter.setPen(myPen);
        painter.drawPath(myPath);
//! [3]


//! [4]
        QLinearGradient myGradient;
        QPen myPen;
        QPolygonF myPolygon;

        QPainterPath myPath;
        myPath.addPolygon(myPolygon);

        QPainter painter(this);
        painter.setBrush(myGradient);
        painter.setPen(myPen);
        painter.drawPath(myPath);
//! [4]


//! [5]
        QLinearGradient myGradient;
        QPen myPen;
        QRectF boundingRectangle;

        QPainterPath myPath;
        myPath.addEllipse(boundingRectangle);

        QPainter painter(this);
        painter.setBrush(myGradient);
        painter.setPen(myPen);
        painter.drawPath(myPath);
//! [5]


//! [6]
        QLinearGradient myGradient;
        QPen myPen;
        QFont myFont;
        QPointF baseline(x, y);

        QPainterPath myPath;
        myPath.addText(baseline, myFont, tr("Qt"));

        QPainter painter(this);
        painter.setBrush(myGradient);
        painter.setPen(myPen);
        painter.drawPath(myPath);
//! [6]


