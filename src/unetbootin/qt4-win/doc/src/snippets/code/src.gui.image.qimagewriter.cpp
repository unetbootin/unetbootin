//! [0]
        QImageWriter writer;
        writer.setFormat("png"); // same as writer.setFormat("PNG");
//! [0]


//! [1]
        QImage image("some/image.jpeg");
        QImageWriter writer("images/outimage.png", "png");
        writer.setText("Author", "John Smith");
        writer.write(image);
//! [1]


//! [2]
        QImageWriter writer(fileName);
        if (writer.supportsOption(QImageIOHandler::Description))
            writer.setText("Author", "John Smith");
//! [2]


