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
#include "treemodelcompleter.h"
#include "mainwindow.h"

//! [0]
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), completer(0), lineEdit(0)
{
    createMenu();

    completer = new TreeModelCompleter(this);
    completer->setModel(modelFromFile(":/resources/treemodel.txt"));
    completer->setSeparator(QLatin1String("."));
    QObject::connect(completer, SIGNAL(highlighted(const QModelIndex&)),
                     this, SLOT(highlight(const QModelIndex&)));

    QWidget *centralWidget = new QWidget;

    QLabel *modelLabel = new QLabel;
    modelLabel->setText(tr("Tree Model<br>(Double click items to edit)"));

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
    QLabel *separatorLabel = new QLabel;
    separatorLabel->setText(tr("Tree Separator"));

    QLineEdit *separatorLineEdit = new QLineEdit;
    separatorLineEdit->setText(completer->separator());
    connect(separatorLineEdit, SIGNAL(textChanged(const QString&)),
            completer, SLOT(setSeparator(const QString&)));

    QCheckBox *wrapCheckBox = new QCheckBox;
    wrapCheckBox->setText(tr("Wrap around completions"));
    wrapCheckBox->setChecked(completer->wrapAround());
    connect(wrapCheckBox, SIGNAL(clicked(bool)), completer, SLOT(setWrapAround(bool)));

    contentsLabel = new QLabel;
    contentsLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(separatorLineEdit, SIGNAL(textChanged(const QString&)),
            this, SLOT(updateContentsLabel(const QString&)));

    treeView = new QTreeView;
    treeView->setModel(completer->model());
    treeView->header()->hide();
    treeView->expandAll();
//! [1]

//! [2]
    connect(modeCombo, SIGNAL(activated(int)), this, SLOT(changeMode(int)));
    connect(caseCombo, SIGNAL(activated(int)), this, SLOT(changeCase(int)));

    lineEdit = new QLineEdit;
    lineEdit->setCompleter(completer);
//! [2]

//! [3]
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(modelLabel, 0, 0); layout->addWidget(treeView, 0, 1);
    layout->addWidget(modeLabel, 1, 0);  layout->addWidget(modeCombo, 1, 1);
    layout->addWidget(caseLabel, 2, 0);  layout->addWidget(caseCombo, 2, 1);
    layout->addWidget(separatorLabel, 3, 0); layout->addWidget(separatorLineEdit, 3, 1);
    layout->addWidget(wrapCheckBox, 4, 0);
    layout->addWidget(contentsLabel, 5, 0, 1, 2);
    layout->addWidget(lineEdit, 6, 0, 1, 2);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    changeCase(caseCombo->currentIndex());
    changeMode(modeCombo->currentIndex());

    setWindowTitle(tr("Tree Model Completer"));
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
//! [5]

QAbstractItemModel *MainWindow::modelFromFile(const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly))
        return new QStringListModel(completer);

#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
#endif
    QStringList words;

    QStandardItemModel *model = new QStandardItemModel(completer);
    QVector<QStandardItem *> parents(10);
    parents[0] = model->invisibleRootItem();

    while (!file.atEnd()) {
        QString line = file.readLine();
        QString trimmedLine = line.trimmed();
        if (line.isEmpty() || trimmedLine.isEmpty())
            continue;

        QRegExp re("^\\s+");
        int nonws = re.indexIn(line);
        int level = 0;
        if (nonws == -1) {
            level = 0;
        } else {
            if (line.startsWith("\t")) {
                level = re.cap(0).length();
            } else {
                level = re.cap(0).length()/4;
            }
        }

        if (level+1 >= parents.size())
            parents.resize(parents.size()*2);

        QStandardItem *item = new QStandardItem;
        item->setText(trimmedLine);
        parents[level]->appendRow(item);
        parents[level+1] = item;
    }

#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    return model;
}

void MainWindow::highlight(const QModelIndex &index)
{
    QAbstractItemModel *completionModel = completer->completionModel();
    QAbstractProxyModel *proxy = qobject_cast<QAbstractProxyModel *>(completionModel);
    if (!proxy)
        return;
    QModelIndex sourceIndex = proxy->mapToSource(index);
    treeView->selectionModel()->select(sourceIndex, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
    treeView->scrollTo(index);
}

//! [6]
void MainWindow::about()
{
    QMessageBox::about(this, tr("About"), tr("This example demonstrates how "
        "to use a QCompleter with a custom tree model."));
}
//! [6]

//! [7]
void MainWindow::changeCase(int cs)
{
    completer->setCaseSensitivity(cs ? Qt::CaseSensitive : Qt::CaseInsensitive);
}
//! [7]

void MainWindow::updateContentsLabel(const QString& sep)
{
    contentsLabel->setText(QString(tr("Type path from model above with items at each level separated by a '%1'")).arg(sep));
}

