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

#include "mainwindow.h"

static const char * const listEntries[] = {
    QT_TRANSLATE_NOOP("MainWindow", "First"),
    QT_TRANSLATE_NOOP("MainWindow", "Second"),
    QT_TRANSLATE_NOOP("MainWindow", "Third"),
    0
};

MainWindow::MainWindow()
{
    centralWidget = new QWidget;
    setCentralWidget(centralWidget);

    createGroupBox();

    listWidget = new QListWidget;
    for (int i = 0; listEntries[i]; ++i)
        listWidget->addItem(tr(listEntries[i]));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(groupBox);
    mainLayout->addWidget(listWidget);
    centralWidget->setLayout(mainLayout);

    exitAction = new QAction(tr("E&xit"), this);
    connect(exitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->setPalette(QPalette(Qt::red));
    fileMenu->addAction(exitAction);

    setWindowTitle(tr("Language: %1").arg(tr("English")));
    statusBar()->showMessage(tr("Internationalization Example"));

    if (tr("LTR") == "RTL")
        setLayoutDirection(Qt::RightToLeft);
}

void MainWindow::createGroupBox()
{
    groupBox = new QGroupBox(tr("View"));
    perspectiveRadioButton = new QRadioButton(tr("Perspective"));
    isometricRadioButton = new QRadioButton(tr("Isometric"));
    obliqueRadioButton = new QRadioButton(tr("Oblique"));
    perspectiveRadioButton->setChecked(true);

    QVBoxLayout *groupBoxLayout = new QVBoxLayout;
    groupBoxLayout->addWidget(perspectiveRadioButton);
    groupBoxLayout->addWidget(isometricRadioButton);
    groupBoxLayout->addWidget(obliqueRadioButton);
    groupBox->setLayout(groupBoxLayout);
}
