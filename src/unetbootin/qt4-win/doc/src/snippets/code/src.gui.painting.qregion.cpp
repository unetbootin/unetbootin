//! [0]
        void MyWidget::paintEvent(QPaintEvent *)
        {
            QRegion r1(QRect(100, 100, 200, 80),    // r1: elliptic region
                       QRegion::Ellipse);
            QRegion r2(QRect(100, 120, 90, 30));    // r2: rectangular region
            QRegion r3 = r1.intersected(r2);        // r3: intersection

            QPainter painter(this);
	    painter.setClipRegion(r3);
            ...                                     // paint clipped graphics
        }
//! [0]


