/****************************************************************************
**
** Copyright (C) 2004-2008 Trolltech ASA. All rights reserved.
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

#include "mainwindow.h"
#include "xmlwriter.h"

MainWindow::MainWindow()
{
    QMenu *fileMenu = new QMenu(tr("&File"));

    QAction *saveAction = fileMenu->addAction(tr("&Save..."));
    saveAction->setShortcut(tr("Ctrl+S"));

    QAction *quitAction = fileMenu->addAction(tr("E&xit"));
    quitAction->setShortcut(tr("Ctrl+Q"));

    menuBar()->addMenu(fileMenu);
    editor = new QTextEdit();

    QTextCursor cursor(editor->textCursor());
    cursor.movePosition(QTextCursor::Start); 

    QTextFrame *mainFrame = cursor.currentFrame();
    
    QTextCharFormat plainCharFormat;
    QTextCharFormat boldCharFormat;
    boldCharFormat.setFontWeight(QFont::Bold);
/*  main frame
//! [0]
    QTextFrame *mainFrame = cursor.currentFrame();
    cursor.insertText(...);
//! [0]
*/
    cursor.insertText("Text documents are represented by the "
                      "QTextDocument class, rather than by QString objects. "
                      "Each QTextDocument object contains information about "
                      "the document's internal representation, its structure, "
                      "and keeps track of modifications to provide undo/redo "
                      "facilities. This approach allows features such as the "
                      "layout management to be delegated to specialized "
                      "classes, but also provides a focus for the framework.",
                      plainCharFormat);

//! [1]
    QTextFrameFormat frameFormat;
    frameFormat.setMargin(32);
    frameFormat.setPadding(8);
    frameFormat.setBorder(4);
//! [1]
    cursor.insertFrame(frameFormat);

/*  insert frame
//! [2]
    cursor.insertFrame(frameFormat);
    cursor.insertText(...);
//! [2]
*/
    cursor.insertText("Documents are either converted from external sources "
                      "or created from scratch using Qt. The creation process "
                      "can done by an editor widget, such as QTextEdit, or by "
                      "explicit calls to the Scribe API.", boldCharFormat);

    cursor = mainFrame->lastCursorPosition();
/*  last cursor
//! [3]
    cursor = mainFrame->lastCursorPosition();
    cursor.insertText(...);
//! [3]
*/
    cursor.insertText("There are two complementary ways to visualize the "
                      "contents of a document: as a linear buffer that is "
                      "used by editors to modify the contents, and as an "
                      "object hierarchy containing structural information "
                      "that is useful to layout engines. In the hierarchical "
                      "model, the objects generally correspond to visual "
                      "elements such as frames, tables, and lists. At a lower "
                      "level, these elements describe properties such as the "
                      "style of text used and its alignment. The linear "
                      "representation of the document is used for editing and "
                      "manipulation of the document's contents.",
                      plainCharFormat);


    connect(saveAction, SIGNAL(triggered()), this, SLOT(saveFile()));
    connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));

    setCentralWidget(editor);
    setWindowTitle(tr("Text Document Frames"));
}

void MainWindow::saveFile()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save document as:"), "", tr("XML (*.xml)"));

    if (!fileName.isEmpty()) {
        if (writeXml(fileName))
            setWindowTitle(fileName);
        else
            QMessageBox::warning(this, tr("Warning"),
                tr("Failed to save the document."), QMessageBox::Cancel,
                QMessageBox::NoButton);
    }
}
bool MainWindow::writeXml(const QString &fileName)
{
    XmlWriter documentWriter(editor->document());

    QDomDocument *domDocument = documentWriter.toXml();
    QFile file(fileName);

    if (file.open(QFile::WriteOnly)) {
        QTextStream textStream(&file);
        textStream.setCodec(QTextCodec::codecForName("UTF-8"));
        
        textStream << domDocument->toString(1).toUtf8();
        file.close();
        return true;
    }
    else
        return false;
}
