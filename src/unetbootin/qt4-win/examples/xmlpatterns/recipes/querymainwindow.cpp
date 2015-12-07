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

#include "querymainwindow.h"

QueryMainWindow::QueryMainWindow() : ui_defaultQueries(0)
{
    setupUi(this);

    ui_defaultQueries = qFindChild<QComboBox*>(this, "defaultQueries");

    QMetaObject::connectSlotsByName(this);

    connect(ui_defaultQueries, SIGNAL(currentIndexChanged(int)), SLOT(displayQuery(int)));

    loadInputFile();

    const QStringList queries(QDir(":/files/", "*.xq").entryList());
    int len = queries.count();

    for(int i = 0; i < len; ++i)
        ui_defaultQueries->addItem(queries.at(i));
}

void QueryMainWindow::displayQuery(int index)
{
    QFile queryFile(QString(":files/") + ui_defaultQueries->itemText(index));
    queryFile.open(QIODevice::ReadOnly);

    const QString query(QString::fromLatin1(queryFile.readAll()));

    qFindChild<QTextEdit*>(this, "queryTextEdit")->setPlainText(query);

    evaluate(query);
}

void QueryMainWindow::loadInputFile()
{
    QFile forView;
    forView.setFileName(":/files/cookbook.xml");
    if (!forView.open(QIODevice::ReadOnly)) {
        QMessageBox::information(this,
            tr("Unable to open file"), forView.errorString());
        return;
    }

    QTextStream in(&forView);
    QString inputDocument = in.readAll();
    qFindChild<QTextEdit*>(this, "inputTextEdit")->setPlainText(inputDocument);
}

void QueryMainWindow::evaluate(const QString &str)
{
    /* This function takes the input string and displays the
     * appropriate output using QXmlQuery.
     */
    QXmlQuery query;

    QFile sourceDocument;
    sourceDocument.setFileName(":/files/cookbook.xml");
    sourceDocument.open(QIODevice::ReadOnly);
    query.bindVariable("inputDocument", &sourceDocument);

    query.setQuery(str);

    if(!query.isValid())
        return;

    QByteArray outArray;
    QBuffer buffer(&outArray);
    buffer.open(QIODevice::ReadWrite);
    
    QXmlFormatter formatter(query, &buffer);

    if(!query.evaluateTo(&formatter))
        return;
 
    buffer.close();
    qFindChild<QTextEdit*>(this, "outputTextEdit")->setPlainText(QString::fromUtf8(outArray.constData()));
    
}
