/****************************************************************************
**
** Copyright (C) 2005-2008 Trolltech ASA. All rights reserved.
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

#include "window.h"

//! [0]
Window::Window(QWidget *parent)
    : QDialog(parent)
{
    browseButton = createButton(tr("&Browse..."), SLOT(browse()));
    findButton = createButton(tr("&Find"), SLOT(find()));

    fileComboBox = createComboBox(tr("*"));
    textComboBox = createComboBox();
    directoryComboBox = createComboBox(QDir::currentPath());

    fileLabel = new QLabel(tr("Named:"));
    textLabel = new QLabel(tr("Containing text:"));
    directoryLabel = new QLabel(tr("In directory:"));
    filesFoundLabel = new QLabel;

    createFilesTable();
//! [0]

//! [1]
    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(findButton);

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(fileLabel, 0, 0);
    mainLayout->addWidget(fileComboBox, 0, 1, 1, 2);
    mainLayout->addWidget(textLabel, 1, 0);
    mainLayout->addWidget(textComboBox, 1, 1, 1, 2);
    mainLayout->addWidget(directoryLabel, 2, 0);
    mainLayout->addWidget(directoryComboBox, 2, 1);
    mainLayout->addWidget(browseButton, 2, 2);
    mainLayout->addWidget(filesTable, 3, 0, 1, 3);
    mainLayout->addWidget(filesFoundLabel, 4, 0);
    mainLayout->addLayout(buttonsLayout, 5, 0, 1, 3);
    setLayout(mainLayout);

    setWindowTitle(tr("Find Files"));
    resize(700, 300);
}
//! [1]

//! [2]
void Window::browse()
{
    QString directory = QFileDialog::getExistingDirectory(this,
                               tr("Find Files"), QDir::currentPath());
    if (!directory.isEmpty()) {
        directoryComboBox->addItem(directory);
        directoryComboBox->setCurrentIndex(directoryComboBox->currentIndex() + 1);
    }
}
//! [2]

//! [3]
void Window::find()
{
    filesTable->setRowCount(0);

    QString fileName = fileComboBox->currentText();
    QString text = textComboBox->currentText();
    QString path = directoryComboBox->currentText();
//! [3]

//! [4]
    QDir directory = QDir(path);
    QStringList files;
    if (fileName.isEmpty())
        fileName = "*";
    files = directory.entryList(QStringList(fileName),
                                QDir::Files | QDir::NoSymLinks);

    if (!text.isEmpty())
        files = findFiles(directory, files, text);
    showFiles(directory, files);
}
//! [4]

//! [5]
QStringList Window::findFiles(const QDir &directory, const QStringList &files,
                              const QString &text)
{
    QProgressDialog progressDialog(this);
    progressDialog.setCancelButtonText(tr("&Cancel"));
    progressDialog.setRange(0, files.size());
    progressDialog.setWindowTitle(tr("Find Files"));

//! [5] //! [6]
    QStringList foundFiles;

    for (int i = 0; i < files.size(); ++i) {
        progressDialog.setValue(i);
        progressDialog.setLabelText(tr("Searching file number %1 of %2...")
                                    .arg(i).arg(files.size()));
        qApp->processEvents();
//! [6]

        if (progressDialog.wasCanceled())
            break;

//! [7]
        QFile file(directory.absoluteFilePath(files[i]));

        if (file.open(QIODevice::ReadOnly)) {
            QString line;
            QTextStream in(&file);
            while (!in.atEnd()) {
                if (progressDialog.wasCanceled())
                    break;
                line = in.readLine();
                if (line.contains(text)) {
                    foundFiles << files[i];
                    break;
                }
            }
        }
    }
    return foundFiles;
}
//! [7]

//! [8]
void Window::showFiles(const QDir &directory, const QStringList &files)
{
    for (int i = 0; i < files.size(); ++i) {
        QFile file(directory.absoluteFilePath(files[i]));
        qint64 size = QFileInfo(file).size();

        QTableWidgetItem *fileNameItem = new QTableWidgetItem(files[i]);
        fileNameItem->setFlags(Qt::ItemIsEnabled);
        QTableWidgetItem *sizeItem = new QTableWidgetItem(tr("%1 KB")
                                             .arg(int((size + 1023) / 1024)));
        sizeItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        sizeItem->setFlags(Qt::ItemIsEnabled);

        int row = filesTable->rowCount();
        filesTable->insertRow(row);
        filesTable->setItem(row, 0, fileNameItem);
        filesTable->setItem(row, 1, sizeItem);
    }
    filesFoundLabel->setText(tr("%1 file(s) found").arg(files.size()));
}
//! [8]

//! [9]
QPushButton *Window::createButton(const QString &text, const char *member)
{
    QPushButton *button = new QPushButton(text);
    connect(button, SIGNAL(clicked()), this, member);
    return button;
}
//! [9]

//! [10]
QComboBox *Window::createComboBox(const QString &text)
{
    QComboBox *comboBox = new QComboBox;
    comboBox->setEditable(true);
    comboBox->addItem(text);
    comboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    return comboBox;
}
//! [10]

//! [11]
void Window::createFilesTable()
{
    filesTable = new QTableWidget(0, 2);
    QStringList labels;
    labels << tr("File Name") << tr("Size");
    filesTable->setHorizontalHeaderLabels(labels);
    filesTable->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
    filesTable->verticalHeader()->hide();
    filesTable->setShowGrid(false);
}
//! [11]
