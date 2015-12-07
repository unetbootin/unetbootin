//! [0]
        QPainter painter(this);

        painter.setBrush(Qt::cyan);
        painter.setPen(Qt::darkCyan);
        painter.drawRect(0, 0, 100,100);

        painter.setBrush(Qt::NoBrush);
        painter.setPen(Qt::darkGreen);
        painter.drawRect(40, 40, 100, 100);
//! [0]


