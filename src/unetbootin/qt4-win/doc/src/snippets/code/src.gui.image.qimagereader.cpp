//! [0]
        QImageReader reader;
        reader.setFormat("png"); // same as reader.setFormat("PNG");
//! [0]


//! [1]
        QImageReader reader("image.png");
        // reader.format() == "png"
//! [1]


//! [2]
        QImage icon(64, 64, QImage::Format_RGB32);
        QImageReader reader("icon_64x64.bmp");
        if (reader.read(&icon)) {
            // Display icon
        }
//! [2]


//! [3]
        QImageReader reader(":/image.png");
        if (reader.supportsOption(QImageIOHandler::Size))
            qDebug() << "Size:" << reader.size();
//! [3]


