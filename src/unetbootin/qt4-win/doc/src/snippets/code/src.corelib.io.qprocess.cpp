//! [0]
        QProcess builder;
        builder.setProcessChannelMode(QProcess::MergedChannels);
        builder.start("make", QStringList() << "-j2");

        if (!builder.waitForFinished())
            qDebug() << "Make failed:" << builder.errorString();
        else
            qDebug() << "Make output:" << builder.readAll();
//! [0]


//! [1]
        QProcess more;
        more.start("more");
        more.write("Text to display");
        more.closeWriteChannel();
        // QProcess will emit readyRead() once "more" starts printing
//! [1]


//! [2]
        command1 | command2
//! [2]


//! [3]
        QProcess process1;
        QProcess process2;

        process1.setStandardOutputProcess(process2);

        process1.start("command1");
        process2.start("command2");
//! [3]


//! [4]
        class SandboxProcess : public QProcess
        {
            ...
         protected:
             void setupChildProcess();
            ...
        };

        void SandboxProcess::setupChildProcess()
        {
            // Drop all privileges in the child process, and enter
            // a chroot jail.
        #if defined Q_OS_UNIX
            ::setgroups(0, 0);
            ::chroot("/etc/safe");
            ::chdir("/");
            ::setgid(safeGid);
            ::setuid(safeUid);
            ::umask(0);
        #endif
        }

//! [4]


//! [5]
        QProcess process;
        process.start("del /s *.txt");
        // same as process.start("del", QStringList() << "/s" << "*.txt");
        ...
//! [5]


//! [6]
        QProcess process;
        process.start("dir \"My Documents\"");
//! [6]


//! [7]
        QProcess process;
        process.start("dir \"\"\"My Documents\"\"\"");
//! [7]


//! [8]
        QStringList environment = QProcess::systemEnvironment();
        // environment = {"PATH=/usr/bin:/usr/local/bin",
                          "USER=greg", "HOME=/home/greg"}
//! [8]


