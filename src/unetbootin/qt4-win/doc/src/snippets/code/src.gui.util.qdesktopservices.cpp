//! [0]
    class MyHelpHandler : public QObject
    {
        Q_OBJECT
    public:
        ...
    public slots:
        void showHelp(const QUrl &url);
    };

    QDesktopServices::setUrlHandler("help", helpInstance, "showHelp");
//! [0]


//! [1]
    mailto:user@foo.com?subject=Test&body=Just a test
//! [1]


