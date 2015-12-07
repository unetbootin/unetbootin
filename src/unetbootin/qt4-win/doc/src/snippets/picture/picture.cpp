/****************************************************************************
**
** Copyright (C) 2005-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the documentation of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License versions 2.0 or 3.0 as published by the Free Software
** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file.  Alternatively you may (at
** your option) use any later version of the GNU General Public
** License if such license has been publicly approved by Trolltech ASA
** (or its successors, if any) and the KDE Free Qt Foundation. In
** addition, as a special exception, Trolltech gives you certain
** additional rights. These rights are described in the Trolltech GPL
** Exception version 1.2, which can be found at
** http://www.trolltech.com/products/qt/gplexception/ and in the file
** GPL_EXCEPTION.txt in this package.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/. If
** you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** In addition, as a special exception, Trolltech, as the sole
** copyright holder for Qt Designer, grants users of the Qt/Eclipse
** Integration plug-in the right for the Qt/Eclipse Integration to
** link to functionality provided by Qt Designer and its related
** libraries.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not expressly
** granted herein.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include <QtGui>

void myProcessing(const QString &)
{
}

int main()
{
    QWidget myWidget;
    {
        // RECORD
//! [0]
        QPicture picture;
        QPainter painter;
        painter.begin(&picture);           // paint in picture
        painter.drawEllipse(10,20, 80,70); // draw an ellipse
        painter.end();                     // painting done
        picture.save("drawing.pic");       // save picture
//! [0]
    }

    {
        // REPLAY
//! [1]
        QPicture picture;
        picture.load("drawing.pic");           // load picture
        QPainter painter;
        painter.begin(&myWidget);              // paint in myWidget
        painter.drawPicture(0, 0, picture);    // draw the picture at (0,0)
        painter.end();                         // painting done
//! [1]
    }

    QPicture myPicture;
    {
        // FORMATS
//! [2]
        QStringList list = QPicture::inputFormatList();
        foreach (QString string, list)
            myProcessing(string);
//! [2]
    }

    {
        // OUTPUT
//! [3]
        QStringList list = QPicture::outputFormatList();
        foreach (QString string, list)
            myProcessing(string);
//! [3]
    }

    {
        // PIC READ
//! [4]
        QPictureIO iio;
        QPixmap  pixmap;
        iio.setFileName("vegeburger.pic");
        if (iio.read()) {        // OK
            QPicture picture = iio.picture();
            QPainter painter(&pixmap);
            painter.drawPicture(0, 0, picture);
        }
//! [4]
    }

    {
        QPixmap pixmap;
        // PIC WRITE
//! [5]
        QPictureIO iio;
        QPicture   picture;
        QPainter painter(&picture);
        painter.drawPixmap(0, 0, pixmap);
        iio.setPicture(picture);
        iio.setFileName("vegeburger.pic");
        iio.setFormat("PIC");
        if (iio.write())
            return true; // returned true if written successfully
//! [5]
    }

}

// SVG READ
//! [6]
void readSVG(QPictureIO *picture)
{
    // read the picture using the picture->ioDevice()
}
//! [6]

// SVG WRITE
//! [7]
void writeSVG(QPictureIO *picture)
{
    // write the picture using the picture->ioDevice()
}
//! [7]

// USE SVG
void foo() {

//! [8]
    // add the SVG picture handler
    // ...
//! [8]
    QPictureIO::defineIOHandler("SVG", 0, 0, readSVG, writeSVG);
    // ...

}
