/****************************************************************************
**
** Copyright (C) 2006-2008 Trolltech ASA. All rights reserved.
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
#include "dirmodel.h"
#include "mainwindow.h"

//! [0]
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), completer(0), lineEdit(0)
{
    createMenu();

    QWidget *centralWidget = new QWidget;

    QLabel *modelLabel = new QLabel;
    modelLabel->setText(tr("Model"));

    modelCombo = new QComboBox;
    modelCombo->addItem(tr("QDirModel"));
    modelCombo->addItem(tr("QDirModel that shows full path"));
    modelCombo->addItem(tr("Country list"));
    modelCombo->addItem(tr("Word list"));
    modelCombo->setCurrentIndex(0);

    QLabel *modeLabel = new QLabel;
    modeLabel->setText(tr("Completion Mode"));
    modeCombo = new QComboBox;
    modeCombo->addItem(tr("Inline"));
    modeCombo->addItem(tr("Filtered Popup"));
    modeCombo->addItem(tr("Unfiltered Popup"));
    modeCombo->setCurrentIndex(1);

    QLabel *caseLabel = new QLabel;
    caseLabel->setText(tr("Case Sensitivity"));
    caseCombo = new QComboBox;
    caseCombo->addItem(tr("Case Insensitive"));
    caseCombo->addItem(tr("Case Sensitive"));
    caseCombo->setCurrentIndex(0);
//! [0]

//! [1]
    wrapCheckBox = new QCheckBox;
    wrapCheckBox->setText(tr("Wrap around completions"));
    wrapCheckBox->setChecked(true);
//! [1]

//! [2]
    contentsLabel = new QLabel;
    contentsLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    connect(modelCombo, SIGNAL(activated(int)), this, SLOT(changeModel()));
    connect(modeCombo, SIGNAL(activated(int)), this, SLOT(changeMode(int)));
    connect(caseCombo, SIGNAL(activated(int)), this, SLOT(changeCase(int)));
//! [2]

//! [3]
    lineEdit = new QLineEdit;
    
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(modelLabel, 0, 0); layout->addWidget(modelCombo, 0, 1);
    layout->addWidget(modeLabel, 1, 0);  layout->addWidget(modeCombo, 1, 1);
    layout->addWidget(caseLabel, 2, 0);  layout->addWidget(caseCombo, 2, 1);
    layout->addWidget(wrapCheckBox, 3, 0);
    layout->addWidget(contentsLabel, 4, 0, 1, 2);
    layout->addWidget(lineEdit, 5, 0, 1, 2);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    changeModel();

    setWindowTitle(tr("Completer"));
    lineEdit->setFocus();
}
//! [3]

//! [4]
void MainWindow::createMenu()
{
    QAction *exitAction = new QAction(tr("Exit"), this);
    QAction *aboutAct = new QAction(tr("About"), this);
    QAction *aboutQtAct = new QAction(tr("About Qt"), this);

    connect(exitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    QMenu* fileMenu = menuBar()->addMenu(tr("File"));
    fileMenu->addAction(exitAction);

    QMenu* helpMenu = menuBar()->addMenu(tr("About"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);
}
//! [4]

//! [5]
QAbstractItemModel *MainWindow::modelFromFile(const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly))
        return new QStringListModel(completer);
//! [5]

//! [6]
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
#endif
    QStringList words;

    while (!file.atEnd()) {
        QByteArray line = file.readLine();
        if (!line.isEmpty())
            words << line.trimmed();
    }

#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
//! [6]

//! [7]
    if (!fileName.contains(QLatin1String("countries.txt")))
        return new QStringListModel(words, completer);
//! [7]

    // The last two chars of the countries.txt file indicate the country
    // symbol. We put that in column 2 of a standard item model
//! [8]
    QStandardItemModel *m = new QStandardItemModel(words.count(), 2, completer);
//! [8] //! [9]
    for (int i = 0; i < words.count(); ++i) {
        QModelIndex countryIdx = m->index(i, 0);
        QModelIndex symbolIdx = m->index(i, 1);
        QString country = words[i].mid(0, words[i].length() - 2).trimmed();
        QString symbol = words[i].right(2);
        m->setData(countryIdx, country);
        m->setData(symbolIdx, symbol);
    }

    return m;
}
//! [9]

//! [10]
void MainWindow::changeMode(int index)
{
    QCompleter::CompletionMode mode;
    if (index == 0)
        mode = QCompleter::InlineCompletion;
    else if (index == 1)
        mode = QCompleter::PopupCompletion;
    else
        mode = QCompleter::UnfilteredPopupCompletion;

    completer->setCompletionMode(mode);
}
//! [10]

void MainWindow::changeCase(int cs)
{
    completer->setCaseSensitivity(cs ? Qt::CaseSensitive : Qt::CaseInsensitive);
}

//! [11]
void MainWindow::changeModel()
{
    delete completer;
    completer = new QCompleter(this);

    switch (modelCombo->currentIndex()) {
    default:
    case 0:
        { // Unsorted QDirModel
            QDirModel *dirModel = new QDirModel(completer);
            completer->setModel(dirModel);
            contentsLabel->setText(tr("Enter file path"));
        }
        break;
//! [11] //! [12]
    case 1:
        { // DirModel that shows full paths
            DirModel *dirModel = new DirModel(completer);
            completer->setModel(dirModel);
            contentsLabel->setText(tr("Enter file path"));
        }
        break;
//! [12] //! [13]
    case 2:
        { // Country List
            completer->setModel(modelFromFile(":/resources/countries.txt"));
            QTreeView *treeView = new QTreeView;
            completer->setPopup(treeView);
            treeView->setRootIsDecorated(false);
            treeView->header()->hide();
            treeView->header()->setStretchLastSection(false);
            treeView->header()->setResizeMode(0, QHeaderView::Stretch);
            treeView->header()->setResizeMode(1, QHeaderView::ResizeToContents);
            contentsLabel->setText(tr("Enter name of your country"));
        }
        break;
//! [13] //! [14]
    case 3:
        { // Word list
            completer->setModel(modelFromFile(":/resources/wordlist.txt"));
            completer->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
            contentsLabel->setText(tr("Enter a word"));
        }
        break;
    }

    changeMode(modeCombo->currentIndex());
    changeCase(caseCombo->currentIndex());
    completer->setWrapAround(wrapCheckBox->isChecked());
    lineEdit->setCompleter(completer);
    connect(wrapCheckBox, SIGNAL(clicked(bool)), completer, SLOT(setWrapAround(bool)));
}
//! [14]

//! [15]
void MainWindow::about()
{
    QMessageBox::about(this, tr("About"), tr("This example demonstrates the "
        "different features of the QCompleter class."));
}
//! [15]
