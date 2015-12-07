//! [0]
        int leading = fontMetrics.leading();
        qreal height = 0;
        qreal widthUsed = 0;
        textLayout.beginLayout();
        while (1) {
            QTextLine line = textLayout.createLine();
            if (!line.isValid())
                break;

            line.setLineWidth(lineWidth);
            height += leading;
            line.setPosition(QPointF(0, height));
            height += line.height();
            widthUsed = qMax(widthUsed, line.naturalTextWidth());
        }
        textLayout.endLayout();
//! [0]


//! [1]
        QPainter painter(this);
        textLayout.draw(&painter, QPoint(0, 0));
//! [1]


