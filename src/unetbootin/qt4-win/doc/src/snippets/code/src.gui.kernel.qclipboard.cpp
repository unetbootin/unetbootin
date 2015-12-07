//! [0]
        QClipboard *clipboard = QApplication::clipboard();
        QString originalText = clipboard->text();
        ...
        clipboard->setText(newText);
//! [0]


//! [1]
        QMimeData *data = new QMimeData;
        data->setImageData(image);
        clipboard->setMimeData(data, mode);
//! [1]


