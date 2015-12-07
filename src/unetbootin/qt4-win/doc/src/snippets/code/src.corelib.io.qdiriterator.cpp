//! [0]
        QDirIterator it("/etc", QDirIterator::Subdirectories);
        while (it.hasNext()) {
            qDebug() << it.next();

            // /etc/.
            // /etc/..
            // /etc/X11
            // /etc/X11/fs
            // ...
        }
//! [0]


