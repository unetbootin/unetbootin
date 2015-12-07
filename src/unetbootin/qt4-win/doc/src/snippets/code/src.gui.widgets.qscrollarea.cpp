//! [0]
        QLabel *imageLabel = new QLabel;
        QImage image("happyguy.png");
        imageLabel->setPixmap(QPixmap::fromImage(image));

        scrollArea = new QScrollArea;
        scrollArea->setBackgroundRole(QPalette::Dark);
        scrollArea->setWidget(imageLabel);
//! [0]


