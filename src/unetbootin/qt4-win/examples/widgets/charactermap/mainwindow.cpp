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

#include "characterwidget.h"
#include "mainwindow.h"

//! [0]
MainWindow::MainWindow()
{
    QWidget *centralWidget = new QWidget;

    QLabel *fontLabel = new QLabel(tr("Font:"));
    fontCombo = new QFontComboBox;
    QLabel *sizeLabel = new QLabel(tr("Size:"));
    sizeCombo = new QComboBox;
    QLabel *styleLabel = new QLabel(tr("Style:"));
    styleCombo = new QComboBox;
    QLabel *fontMergingLabel = new QLabel(tr("Automatic Font Merging:"));
    fontMerging = new QCheckBox;
    fontMerging->setChecked(true);

    scrollArea = new QScrollArea;
    characterWidget = new CharacterWidget;
    scrollArea->setWidget(characterWidget);
//! [0]

//! [1]
    findStyles(fontCombo->currentFont());
//! [1]
    findSizes(fontCombo->currentFont());

//! [2]
    lineEdit = new QLineEdit;
    QPushButton *clipboardButton = new QPushButton(tr("&To clipboard"));
//! [2]

//! [3]
    clipboard = QApplication::clipboard();
//! [3]

//! [4]
    connect(fontCombo, SIGNAL(currentFontChanged(const QFont &)),
            this, SLOT(findStyles(const QFont &)));
    connect(fontCombo, SIGNAL(currentFontChanged(const QFont &)),
            this, SLOT(findSizes(const QFont &)));
    connect(fontCombo, SIGNAL(currentFontChanged(const QFont &)),
            characterWidget, SLOT(updateFont(const QFont &)));
    connect(sizeCombo, SIGNAL(currentIndexChanged(const QString &)),
            characterWidget, SLOT(updateSize(const QString &)));
    connect(styleCombo, SIGNAL(currentIndexChanged(const QString &)),
            characterWidget, SLOT(updateStyle(const QString &)));
//! [4] //! [5]
    connect(characterWidget, SIGNAL(characterSelected(const QString &)),
            this, SLOT(insertCharacter(const QString &)));
    connect(clipboardButton, SIGNAL(clicked()), this, SLOT(updateClipboard()));
//! [5]
    connect(fontMerging, SIGNAL(toggled(bool)), characterWidget, SLOT(updateFontMerging(bool)));

//! [6]
    QHBoxLayout *controlsLayout = new QHBoxLayout;
    controlsLayout->addWidget(fontLabel);
    controlsLayout->addWidget(fontCombo, 1);
    controlsLayout->addWidget(sizeLabel);
    controlsLayout->addWidget(sizeCombo, 1);
    controlsLayout->addWidget(styleLabel);
    controlsLayout->addWidget(styleCombo, 1);
    controlsLayout->addWidget(fontMergingLabel);
    controlsLayout->addWidget(fontMerging, 1);
    controlsLayout->addStretch(1);

    QHBoxLayout *lineLayout = new QHBoxLayout;
    lineLayout->addWidget(lineEdit, 1);
    lineLayout->addSpacing(12);
    lineLayout->addWidget(clipboardButton);

    QVBoxLayout *centralLayout = new QVBoxLayout;
    centralLayout->addLayout(controlsLayout);
    centralLayout->addWidget(scrollArea, 1);
    centralLayout->addSpacing(4);
    centralLayout->addLayout(lineLayout);
    centralWidget->setLayout(centralLayout);

    setCentralWidget(centralWidget);
    setWindowTitle(tr("Character Map"));
}
//! [6]

//! [7]
void MainWindow::findStyles(const QFont &font)
{
    QFontDatabase fontDatabase;
    QString currentItem = styleCombo->currentText();
    styleCombo->clear();
//! [7]

//! [8]
    QString style;
    foreach (style, fontDatabase.styles(font.family()))
        styleCombo->addItem(style);

    int styleIndex = styleCombo->findText(currentItem);

    if (styleIndex == -1)
        styleCombo->setCurrentIndex(0);
    else
        styleCombo->setCurrentIndex(styleIndex);
}
//! [8]

void MainWindow::findSizes(const QFont &font)
{
    QFontDatabase fontDatabase;
    QString currentSize = sizeCombo->currentText();
    sizeCombo->blockSignals(true);
    sizeCombo->clear();

    int size;
    if(fontDatabase.isSmoothlyScalable(font.family(), fontDatabase.styleString(font))) {
        foreach(size, QFontDatabase::standardSizes()) {
            sizeCombo->addItem(QVariant(size).toString());
            sizeCombo->setEditable(true);
        }

    } else {
        foreach(size, fontDatabase.smoothSizes(font.family(), fontDatabase.styleString(font))) {
            sizeCombo->addItem(QVariant(size).toString());
            sizeCombo->setEditable(false);
        }
    }

    sizeCombo->blockSignals(false);

    int sizeIndex = sizeCombo->findText(currentSize);

    if(sizeIndex == -1)
        sizeCombo->setCurrentIndex(qMax(0, sizeCombo->count() / 3));
    else
        sizeCombo->setCurrentIndex(sizeIndex);
}

//! [9]
void MainWindow::insertCharacter(const QString &character)
{
    lineEdit->insert(character);
}
//! [9]

//! [10]
void MainWindow::updateClipboard()
{
//! [11]
    clipboard->setText(lineEdit->text(), QClipboard::Clipboard);
//! [11]
    clipboard->setText(lineEdit->text(), QClipboard::Selection);
}
//! [10]
