//! [0]
        static void paintLayout(QPainter *painter, QLayoutItem *item)
        {
            QLayout *layout = item->layout();
            if (layout) {
                for (int i = 0; i < layout->count(); ++i)
                    paintLayout(painter, layout->itemAt(i));
            }
            painter->drawRect(layout->geometry());
        }

        void MyWidget::paintEvent(QPaintEvent *)
        {
            QPainter painter(this);
            if (layout())
                paintLayout(&painter, layout());
        }
//! [0]


//! [1]
        QLayoutItem *child;
        while ((child = layout->takeAt(0)) != 0) {
            ...
            delete child;
        }
//! [1]


