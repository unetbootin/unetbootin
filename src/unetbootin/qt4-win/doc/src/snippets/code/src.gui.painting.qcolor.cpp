//! [0]
    // Specify semi-transparent red
    painter.setBrush(QColor(255, 0, 0, 127));
    painter.drawRect(0, 0, width()/2, height());

    // Specify semi-transparent blue
    painter.setBrush(QColor(0, 0, 255, 127));
    painter.drawRect(0, 0, width(), height()/2);
//! [0]


