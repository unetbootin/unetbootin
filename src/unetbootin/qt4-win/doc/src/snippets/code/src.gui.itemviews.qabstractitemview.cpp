//! [0]
        void MyView::resizeEvent(QResizeEvent *event) {
            horizontalScrollBar()->setRange(0, realWidth - width());
            ...
        }
//! [0]


