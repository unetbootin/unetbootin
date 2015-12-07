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

#include "dialog.h"

//! [Dialog constructor part1]
Dialog::Dialog()
{
    desktopGeometry = QApplication::desktop()->availableGeometry(0);

    setWindowTitle(tr("SIP Dialog Example"));
    QScrollArea *scrollArea = new QScrollArea(this);
    QGroupBox *groupBox = new QGroupBox(scrollArea);
    groupBox->setTitle(tr("SIP Dialog Example"));
    QGridLayout *gridLayout = new QGridLayout(groupBox);
    groupBox->setLayout(gridLayout);
//! [Dialog constructor part1]

//! [Dialog constructor part2]
    QLineEdit* lineEdit = new QLineEdit(groupBox);
    lineEdit->setText(tr("Open and close the SIP"));
    lineEdit->setMinimumWidth(220);

    QLabel* label = new QLabel(groupBox);
    label->setText(tr("This dialog resizes if the SIP is opened"));
    label->setMinimumWidth(220);

    QPushButton* button = new QPushButton(groupBox);
    button->setText(tr("Close Dialog"));
    button->setMinimumWidth(220);
//! [Dialog constructor part2]

//! [Dialog constructor part3]
    if (desktopGeometry.height() < 400)
        gridLayout->setVerticalSpacing(80);
    else
        gridLayout->setVerticalSpacing(150);

    gridLayout->addWidget(label);
    gridLayout->addWidget(lineEdit);
    gridLayout->addWidget(button);
//! [Dialog constructor part3]

//! [Dialog constructor part4]
    scrollArea->setWidget(groupBox);
    QHBoxLayout* layout = new QHBoxLayout();
    layout->addWidget(scrollArea);
    setLayout(layout);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//! [Dialog constructor part4]

//! [Dialog constructor part5]
    connect(button, SIGNAL(pressed()), 
        qApp, SLOT(closeAllWindows()));
    connect(QApplication::desktop(), SIGNAL(workAreaResized(int)), 
        this, SLOT(desktopResized(int)));
}
//! [Dialog constructor part5]

//! [desktopResized() function]
void Dialog::desktopResized(int screen)
{
    if (screen != 0)
        return;
    reactToSIP();
}
//! [desktopResized() function]

//! [reactToSIP() function]
void Dialog::reactToSIP()
{
    QRect availableGeometry = QApplication::desktop()->availableGeometry(0);

    if (desktopGeometry.width() == availableGeometry.width()) {
        if (desktopGeometry.height() > availableGeometry.height()) {
            setWindowState(windowState() & ~Qt::WindowMaximized);
            setGeometry(availableGeometry);
        } else {
            setWindowState(windowState() | Qt::WindowMaximized);
        }
    }
    desktopGeometry = availableGeometry;
}
//! [reactToSIP() function]
