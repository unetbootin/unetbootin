
//! [0]
int main(int argc, char * argv[])
{
    QPushButton *button1 = new QPushButton;
    QPushButton *button2 = new QPushButton;
    QPushButton *button3 = new QPushButton;

    QSignalMapper *signalMapper = new QSignalMapper(this);
    signalMapper->setMapping(button1, QString("File1"));
    signalMapper->setMapping(button2, QString("File2"));
    signalMapper->setMapping(button3, QString("File3"));

    connect(signalMapper, SIGNAL(mapped(const QString &)),
        this, SLOT(mySlot(const QString&)));
}
//! [0]

//! [1]
void mySlot(const QString &fileName)
{
    QFile file(fileName);
    file.open(QIODevice::ReadOnly);
}
//! [1]