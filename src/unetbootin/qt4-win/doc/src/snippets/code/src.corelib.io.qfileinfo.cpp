//! [0]
        #ifdef Q_OS_UNIX

        QFileInfo info1("/home/bob/bin/untabify");
        info1.isSymLink();          // returns true
        info1.absoluteFilePath();   // returns "/home/bob/bin/untabify"
        info1.size();               // returns 56201
        info1.symLinkTarget();      // returns "/opt/pretty++/bin/untabify"

        QFileInfo info2(info1.symLinkTarget());
        info1.isSymLink();          // returns false
        info1.absoluteFilePath();   // returns "/opt/pretty++/bin/untabify"
        info1.size();               // returns 56201

        #endif
//! [0]


//! [1]
        #ifdef Q_OS_WIN

        QFileInfo info1("C:\\Documents and Settings\\Bob\\untabify.lnk");
        info1.isSymLink();          // returns true
        info1.absoluteFilePath();   // returns "C:/Documents and Settings/Bob/untabify.lnk"
        info1.size();               // returns 743
        info1.symLinkTarget();      // returns "C:/Pretty++/untabify"

        QFileInfo info2(info1.symLinkTarget());
        info1.isSymLink();          // returns false
        info1.absoluteFilePath();   // returns "C:/Pretty++/untabify"
        info1.size();               // returns 63942

        #endif
//! [1]


//! [2]
    QString absolute = "/local/bin";
    QString relative = "local/bin";
    QFileInfo absFile(absolute);
    QFileInfo relFile(relative);

    QDir::setCurrent(QDir::rootPath());
    // absFile and relFile now point to the same file

    QDir::setCurrent("/tmp");
    // absFile now points to "/local/bin",
    // while relFile points to "/tmp/local/bin"
//! [2]


//! [3]
        QFileInfo fi("/tmp/archive.tar.gz");
        QString name = fi.fileName();                // name = "archive.tar.gz"
//! [3]


//! [4]
        QFileInfo fi("/Applications/Safari.app");
        QString bundle = fi.bundleName();                // name = "Safari"
//! [4]


//! [5]
        QFileInfo fi("/tmp/archive.tar.gz");
        QString base = fi.baseName();  // base = "archive"
//! [5]


//! [6]
        QFileInfo fi("/tmp/archive.tar.gz");
        QString base = fi.completeBaseName();  // base = "archive.tar"
//! [6]


//! [7]
        QFileInfo fi("/tmp/archive.tar.gz");
        QString ext = fi.completeSuffix();  // ext = "tar.gz"
//! [7]


//! [8]
        QFileInfo fi("/tmp/archive.tar.gz");
        QString ext = fi.suffix();  // ext = "gz"
//! [8]


//! [9]
        QFileInfo info(fileName);
        if (info.isSymLink())
            fileName = info.symLinkTarget();
//! [9]


//! [10]
        QFileInfo fi("/tmp/archive.tar.gz");
        if (fi.permission(QFile::WriteUser | QFile::ReadGroup))
            qWarning("I can change the file; my group can read the file");
        if (fi.permission(QFile::WriteGroup | QFile::WriteOther))
            qWarning("The group or others can change the file");
//! [10]


