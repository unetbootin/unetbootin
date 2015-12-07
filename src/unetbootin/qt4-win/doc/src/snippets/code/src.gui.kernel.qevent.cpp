//! [0]
        void MyWidget::wheelEvent(QWheelEvent *event)
        {
            int numDegrees = event->delta() / 8;
            int numSteps = numDegrees / 15;

            if (event->orientation() == Qt::Horizontal) {
                scrollHorizontally(numSteps);
            } else {
                scrollVertically(numSteps);
            }
            event->accept();
        }
//! [0]


