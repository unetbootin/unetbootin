#include <QtGui>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

//! [0]
    QPrinter printer;
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName("/foobar/nonwritable.pdf");
    QPainter painter;
    if (! painter.begin(&printer)) { // failed to open file
        qWarning("failed to open file, is it writable?");
        return 1;
    }
    painter.drawText(10, 10, "Test");
    if (! printer.newPage()) {
        qWarning("failed in flushing page to disk, disk full?");
        return 1;
    }
    painter.drawText(10, 10, "Test 2");
    painter.end();
//! [0]
    return 0;
}
