/****************************************************************************
**
** Copyright (C) 2004-2008 Trolltech ASA. All rights reserved.
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

#include "mainwindow.h"
#include "previewform.h"

MainWindow::MainWindow()
{
    textEdit = new QTextEdit;
    textEdit->setLineWrapMode(QTextEdit::NoWrap);
    setCentralWidget(textEdit);

    findCodecs();

    previewForm = new PreviewForm(this);
    previewForm->setCodecList(codecs);

    createActions();
    createMenus();

    setWindowTitle(tr("Codecs"));
    resize(500, 400);
}

void MainWindow::open()
{
    QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (!file.open(QFile::ReadOnly)) {
            QMessageBox::warning(this, tr("Codecs"),
                                 tr("Cannot read file %1:\n%2")
                                 .arg(fileName)
                                 .arg(file.errorString()));
            return;
        }

        QByteArray data = file.readAll();

        previewForm->setEncodedData(data);
        if (previewForm->exec())
            textEdit->setPlainText(previewForm->decodedString());
    }
}

void MainWindow::save()
{
    QString fileName = QFileDialog::getSaveFileName(this);
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (!file.open(QFile::WriteOnly | QFile::Text)) {
            QMessageBox::warning(this, tr("Codecs"),
                                 tr("Cannot write file %1:\n%2")
                                 .arg(fileName)
                                 .arg(file.errorString()));
            return;
        }

        QAction *action = qobject_cast<QAction *>(sender());
        QByteArray codecName = action->data().toByteArray();

        QTextStream out(&file);
        out.setCodec(codecName);
        out << textEdit->toPlainText();
    }
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About Codecs"),
            tr("The <b>Codecs</b> example demonstrates how to read and write "
               "files using various encodings."));
}

void MainWindow::aboutToShowSaveAsMenu()
{
    QString currentText = textEdit->toPlainText();

    foreach (QAction *action, saveAsActs) {
        QByteArray codecName = action->data().toByteArray();
        QTextCodec *codec = QTextCodec::codecForName(codecName);
        action->setVisible(codec && codec->canEncode(currentText));
    }
}

void MainWindow::findCodecs()
{
    QMap<QString, QTextCodec *> codecMap;
    QRegExp iso8859RegExp("ISO[- ]8859-([0-9]+).*");

    foreach (int mib, QTextCodec::availableMibs()) {
        QTextCodec *codec = QTextCodec::codecForMib(mib);

        QString sortKey = codec->name().toUpper();
        int rank;

        if (sortKey.startsWith("UTF-8")) {
            rank = 1;
        } else if (sortKey.startsWith("UTF-16")) {
            rank = 2;
        } else if (iso8859RegExp.exactMatch(sortKey)) {
            if (iso8859RegExp.cap(1).size() == 1)
                rank = 3;
            else
                rank = 4;
        } else {
            rank = 5;
        }
        sortKey.prepend(QChar('0' + rank));

        codecMap.insert(sortKey, codec);
    }
    codecs = codecMap.values();
}

void MainWindow::createActions()
{
    openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcut(tr("Ctrl+O"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    foreach (QTextCodec *codec, codecs) {
        QString text = tr("%1...").arg(QString(codec->name()));

        QAction *action = new QAction(text, this);
	action->setData(codec->name());
        connect(action, SIGNAL(triggered()), this, SLOT(save()));
        saveAsActs.append(action);
    }

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcut(tr("Ctrl+Q"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    aboutAct = new QAction(tr("&About"), this);
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void MainWindow::createMenus()
{
    saveAsMenu = new QMenu(tr("&Save As"), this);
    foreach (QAction *action, saveAsActs)
        saveAsMenu->addAction(action);
    connect(saveAsMenu, SIGNAL(aboutToShow()),
            this, SLOT(aboutToShowSaveAsMenu()));

    fileMenu = new QMenu(tr("&File"), this);
    fileMenu->addAction(openAct);
    fileMenu->addMenu(saveAsMenu);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    helpMenu = new QMenu(tr("&Help"), this);
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);

    menuBar()->addMenu(fileMenu);
    menuBar()->addSeparator();
    menuBar()->addMenu(helpMenu);
}
