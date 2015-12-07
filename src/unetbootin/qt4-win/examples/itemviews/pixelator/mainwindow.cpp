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

#include "imagemodel.h"
#include "mainwindow.h"
#include "pixeldelegate.h"

//! [0]
MainWindow::MainWindow()
{
//! [0]
    currentPath = QDir::homePath();
    model = new ImageModel(this);

    QWidget *centralWidget = new QWidget;

//! [1]
    view = new QTableView;
    view->setShowGrid(false);
    view->horizontalHeader()->hide();
    view->verticalHeader()->hide();
    view->horizontalHeader()->setMinimumSectionSize(1);
    view->verticalHeader()->setMinimumSectionSize(1);
    view->setModel(model);
//! [1]

//! [2]
    PixelDelegate *delegate = new PixelDelegate(this);
    view->setItemDelegate(delegate);
//! [2]

//! [3]
    QLabel *pixelSizeLabel = new QLabel(tr("Pixel size:"));
    QSpinBox *pixelSizeSpinBox = new QSpinBox;
    pixelSizeSpinBox->setMinimum(4);
    pixelSizeSpinBox->setMaximum(32);
    pixelSizeSpinBox->setValue(12);
//! [3]

    QMenu *fileMenu = new QMenu(tr("&File"), this);
    QAction *openAction = fileMenu->addAction(tr("&Open..."));
    openAction->setShortcut(QKeySequence(tr("Ctrl+O")));

    printAction = fileMenu->addAction(tr("&Print..."));
    printAction->setEnabled(false);
    printAction->setShortcut(QKeySequence(tr("Ctrl+P")));

    QAction *quitAction = fileMenu->addAction(tr("E&xit"));
    quitAction->setShortcut(QKeySequence(tr("Ctrl+Q")));

    QMenu *helpMenu = new QMenu(tr("&Help"), this);
    QAction *aboutAction = helpMenu->addAction(tr("&About"));

    menuBar()->addMenu(fileMenu);
    menuBar()->addSeparator();
    menuBar()->addMenu(helpMenu);

    connect(openAction, SIGNAL(triggered()), this, SLOT(chooseImage()));
    connect(printAction, SIGNAL(triggered()), this, SLOT(printImage()));
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(showAboutBox()));
//! [4]
    connect(pixelSizeSpinBox, SIGNAL(valueChanged(int)),
            delegate, SLOT(setPixelSize(int)));
    connect(pixelSizeSpinBox, SIGNAL(valueChanged(int)),
            this, SLOT(updateView()));
//! [4]

    QHBoxLayout *controlsLayout = new QHBoxLayout;
    controlsLayout->addWidget(pixelSizeLabel);
    controlsLayout->addWidget(pixelSizeSpinBox);
    controlsLayout->addStretch(1);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(view);
    mainLayout->addLayout(controlsLayout);
    centralWidget->setLayout(mainLayout);

    setCentralWidget(centralWidget);

    setWindowTitle(tr("Pixelator"));
    resize(640, 480);
//! [5]
}
//! [5]

void MainWindow::chooseImage()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Choose an image"), currentPath, "*");

    if (!fileName.isEmpty())
        openImage(fileName);
}

void MainWindow::openImage(const QString &fileName)
{
    QImage image;

    if (image.load(fileName)) {
        model->setImage(image);
        if (!fileName.startsWith(":/")) {
            currentPath = fileName;
            setWindowTitle(tr("%1 - Pixelator").arg(currentPath));
        }

        printAction->setEnabled(true);
        updateView();
    }
}

void MainWindow::printImage()
{
#ifndef QT_NO_PRINTER
    if (model->rowCount(QModelIndex())*model->columnCount(QModelIndex())
        > 90000) {
	    QMessageBox::StandardButton answer;
	    answer = QMessageBox::question(this, tr("Large Image Size"),
            tr("The printed image may be very large. Are you sure that "
               "you want to print it?"),
            QMessageBox::Yes | QMessageBox::No);
        if (answer == QMessageBox::No)
            return;
    }

    QPrinter printer(QPrinter::HighResolution);

    QPrintDialog *dlg = new QPrintDialog(&printer, this);
    dlg->setWindowTitle(tr("Print Image"));

    if (dlg->exec() != QDialog::Accepted)
        return;

    QPainter painter;
    painter.begin(&printer);

    int rows = model->rowCount(QModelIndex());
    int columns = model->columnCount(QModelIndex());
    int sourceWidth = (columns+1) * ItemSize;
    int sourceHeight = (rows+1) * ItemSize;

    painter.save();

    double xscale = printer.pageRect().width()/double(sourceWidth);
    double yscale = printer.pageRect().height()/double(sourceHeight);
    double scale = qMin(xscale, yscale);

    painter.translate(printer.paperRect().x() + printer.pageRect().width()/2,
                      printer.paperRect().y() + printer.pageRect().height()/2);
    painter.scale(scale, scale);
    painter.translate(-sourceWidth/2, -sourceHeight/2);

    QStyleOptionViewItem option;
    QModelIndex parent = QModelIndex();

    QProgressDialog progress(tr("Printing..."), tr("Cancel"), 0, rows, this);
    progress.setWindowModality(Qt::ApplicationModal);
    float y = ItemSize/2;

    for (int row = 0; row < rows; ++row) {
        progress.setValue(row);
        qApp->processEvents();
        if (progress.wasCanceled())
            break;

        float x = ItemSize/2;

        for (int column = 0; column < columns; ++column) {
            option.rect = QRect(int(x), int(y), ItemSize, ItemSize);
            view->itemDelegate()->paint(&painter, option,
                                        model->index(row, column, parent));
            x = x + ItemSize;
        }
        y = y + ItemSize;
    }
    progress.setValue(rows);

    painter.restore();
    painter.end();

    if (progress.wasCanceled()) {
        QMessageBox::information(this, tr("Printing canceled"),
            tr("The printing process was canceled."), QMessageBox::Cancel);
    }
#else
    QMessageBox::information(this, tr("Printing canceled"), 
        tr("Printing is not supported on this Qt build"), QMessageBox::Cancel);
#endif
}

void MainWindow::showAboutBox()
{
    QMessageBox::about(this, tr("About the Pixelator example"),
        tr("This example demonstrates how a standard view and a custom\n"
           "delegate can be used to produce a specialized representation\n"
           "of data in a simple custom model."));
}

//! [6]
void MainWindow::updateView()
{
    view->resizeColumnsToContents();
    view->resizeRowsToContents();
}
//! [6]
