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
#include <QLibraryInfo>
#include <QtXmlPatterns>

#include "mainwindow.h"

MainWindow::MainWindow() : m_fileTree(m_namePool)
{
    setupUi(this);

    /* Setup the font. */
    {
        QFont font("Courier");
        font.setFixedPitch(true);

        fileTree->setFont(font);
        queryEdit->setFont(font);
        output->setFont(font);
    }

    /* We load one which isn't too big. */
    const QString dir(QLibraryInfo::location(QLibraryInfo::ExamplesPath)  + "/xmlpatterns/filetree");
    qDebug() << dir;

    if(QDir(dir).exists())
        loadDirectory(dir);
    else
        fileTree->setPlainText(tr("Use the Open menu entry to select a directory."));

    const QStringList queries(QDir(":/queries/", "*.xq").entryList());
    int len = queries.count();

    for(int i = 0; i < len; ++i)
        queryBox->addItem(queries.at(i));

}

void MainWindow::on_queryBox_currentIndexChanged()
{
    QFile queryFile(":/queries/" + queryBox->currentText());
    queryFile.open(QIODevice::ReadOnly);

    queryEdit->setPlainText(QString::fromLatin1(queryFile.readAll()));
    evaluateResult();
}

void MainWindow::evaluateResult()
{
    if(queryBox->currentText().isEmpty())
        return;

    QXmlQuery query(m_namePool);
    query.bindVariable("fileTree", m_fileNode);
    query.setQuery(QUrl("qrc:/queries/" + queryBox->currentText()));

    QByteArray formatterOutput;
    QBuffer buffer(&formatterOutput);
    buffer.open(QIODevice::WriteOnly);

    QXmlFormatter formatter(query, &buffer);
    query.evaluateTo(&formatter);

    output->setText(QString::fromLatin1(formatterOutput.constData()));
}

void MainWindow::on_actionOpenDirectory_triggered()
{
    loadDirectory(QFileDialog::getExistingDirectory(this));
}

void MainWindow::loadDirectory(const QString &directory)
{
    Q_ASSERT(QDir(directory).exists());

    m_fileNode = m_fileTree.nodeFor(directory);

    QXmlQuery query(m_namePool);
    query.bindVariable("fileTree", m_fileNode);
    query.setQuery(QUrl("qrc:/queries/wholeTree.xq"));

    QByteArray output;
    QBuffer buffer(&output);
    buffer.open(QIODevice::WriteOnly);

    QXmlFormatter formatter(query, &buffer);
    query.evaluateTo(&formatter);

    treeInfo->setText((QString(tr("%1 rendered as XML.")).arg(directory)));
    fileTree->setText(QString::fromLatin1(output.constData()));
    evaluateResult();
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, tr("About File Tree"),
                             tr("<p>The <b>File Tree</b> example shows "
                                "how to use XQuery on top of data of your choice without "
                                "converting it to an XML document.</p>"
                                "<p>In this example a QSimpleXmlNodeModel sub-class makes it possible to "
                                "use the XQuery language's strength of selecting and aggregating on top of "
                                "the file system.</p>"));
}


