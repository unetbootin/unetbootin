//! [0]
        int ret = QMessageBox::warning(this, tr("My Application"),
                          tr("The document has been modified.\n"
                             "Do you want to save your changes?"),
                          QMessageBox::Save | QMessageBox::Discard
                          | QMessageBox::Cancel,
                          QMessageBox::Save);
//! [0]


//! [1]
        QMessageBox msgBox;
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        switch (msgBox.exec()) {
        case QMessageBox::Yes:
            // yes was clicked
            break;
        case QMessageBox::No:
            // no was clicked
            break;
        default:
            // should never be reached
            break;
        }
//! [1]


//! [2]
        QMessageBox msgBox;
        QPushButton *connectButton = msgBox.addButton(tr("Connect"), QMessageBox::ActionRole);
        QPushButton *abortButton = msgBox.addButton(QMessageBox::Abort);

        msgBox.exec();

        if (msgBox.clickedButton() == connectButton) {
            // connect
        } else if (msgBox.clickedButton() == abortButton) {
            // abort
        }
//! [2]


//! [3]
        QMessageBox messageBox(this);
        QAbstractButton *disconnectButton =
              messageBox.addButton(tr("Disconnect"), QMessageBox::ActionRole);
        ...
        messageBox.exec();
        if (messageBox.clickedButton() == disconnectButton) {
            ...
        }
//! [3]


//! [4]
        #include <QApplication>
        #include <QMessageBox>

        int main(int argc, char *argv[])
        {
            QT_REQUIRE_VERSION(argc, argv, "4.0.2")

            QApplication app(argc, argv);
            ...
            return app.exec();
        }
//! [4]


