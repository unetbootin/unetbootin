#include <QtGui>
#include <QWebPage>
#include <QWebFrame>

//! [0]
class Thumbnailer : public QObject
{
    Q_OBJECT

public:
    Thumbnailer(const QUrl &url);

signals:
    void finished();

private slots:
    void render();

private:
    QWebPage page;

};
//! [0]

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Thumbnailer thumbnail(QUrl("http://www.trolltech.com"));

    QObject::connect(&thumbnail, SIGNAL(finished()),
        &app, SLOT(quit()));

    return app.exec();
}

//! [1]
Thumbnailer::Thumbnailer(const QUrl &url)
{
    page.mainFrame()->load(url);
    connect(&page, SIGNAL(loadFinished(bool)),
        this, SLOT(render()));
}
//! [1]

//! [2]
void Thumbnailer::render()
{
    page.setViewportSize(page.mainFrame()->contentsSize());
    QImage image(page.viewportSize(), QImage::Format_ARGB32);
    QPainter painter(&image);

    page.mainFrame()->render(&painter);
    painter.end();

    QImage thumbnail = image.scaled(400, 400);
    thumbnail.save("thumbnail.png");

    emit finished();
}
//! [2]
#include "main.moc"
