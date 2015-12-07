/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
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
#include <QtXmlPatterns>

#include "mainwindow.h"
#include "qobjectxmlmodel.h"

MainWindow::MainWindow()
{
    setupUi(this);

    /* Setup the font. */
    {
        QFont font("Courier");
        font.setFixedPitch(true);

        wholeTree->setFont(font);
        wholeTreeOutput->setFont(font);
        htmlQueryEdit->setFont(font);
    }

    /* We ensure the same name pool is used for our QXmlQuery
     * and node model. */
    QXmlNamePool namePool;

    QObjectXmlModel qObjectModel(this, namePool);

    QXmlQuery query(namePool);

    /* The basic XML view. */
    {
        query.bindVariable("root", qObjectModel.root());
        query.setQuery(QUrl("qrc:/queries/wholeTree.xq"));

        Q_ASSERT(query.isValid());
        QByteArray output;
        QBuffer buffer(&output);
        buffer.open(QIODevice::WriteOnly);

        /* Let's the use the formatter, so it's a bit easier to read. */
        QXmlFormatter serializer(query, &buffer);

        query.evaluateTo(&serializer);
        buffer.close();

        {
            QFile queryFile(":/queries/wholeTree.xq");
            queryFile.open(QIODevice::ReadOnly);
            wholeTree->setPlainText(QString::fromUtf8(queryFile.readAll()));
            wholeTreeOutput->setPlainText(QString::fromUtf8(output.constData()));
        }
    }

    /* The HTML view. */
    {
        query.setQuery(QUrl("qrc:/queries/statisticsInHTML.xq"));
        Q_ASSERT(query.isValid());

        QByteArray output;
        QBuffer buffer(&output);
        buffer.open(QIODevice::WriteOnly);

        /* Let's the use the serializer, so we gain a bit of speed. */
        QXmlSerializer serializer(query, &buffer);

        query.evaluateTo(&serializer);
        buffer.close();

        {
            QFile queryFile(":/queries/statisticsInHTML.xq");
            queryFile.open(QIODevice::ReadOnly);
            htmlQueryEdit->setPlainText(QString::fromUtf8(queryFile.readAll()));
            htmlOutput->setHtml(output);
        }
    }
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, tr("About QObject XML Model"),
                              tr("<p>The <b>QObject XML Model</b> example shows "
                                 "how to use XQuery on top of data of your choice without "
                                 "converting it to an XML document.</p>"
                                 "<p>In this example a QSimpleXmlNodeModel subclass makes it possible to "
                                 "query a QObject tree using XQuery and retrieve the result as pointers to QObjects, or as XML.</p>"
                                 "<p>A possible use case of this could be to write an application that tests a graphical interface "
                                 "against Human Interface Guidelines, or that queries an application's data which is modeled using "
                                 "a QObject tree and dynamic properties."));
}


