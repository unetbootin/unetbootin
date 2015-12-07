//! [0]
        QPainter painter(this);
        QPen pen(Qt::green, 3, Qt::DashDotLine, Qt::RoundCap, Qt::RoundJoin);
        painter.setPen(pen);
//! [0]


//! [1]
        QPainter painter(this);
        QPen pen();  // creates a default pen

        pen.setStyle(Qt::DashDotLine);
        pen.setWidth(3);
        pen.setBrush(Qt::green);
        pen.setCapStyle(Qt::RoundCap);
        pen.setJoinStyle(Qt::RoundJoin);

        painter.setPen(pen);
//! [1]


//! [2]
    QPen pen;
    QVector<qreal> dashes;
    qreal space = 4;

    dashes << 1 << space << 3 << space << 9 << space
               << 27 << space << 9;

    pen.setDashPattern(dashes);
//! [2]


//! [3]
    QPen pen;
    QVector<qreal> dashes;
    qreal space = 4;
    dashes << 1 << space << 3 << space << 9 << space
               << 27 << space << 9;
    pen.setDashPattern(dashes);
//! [3]


