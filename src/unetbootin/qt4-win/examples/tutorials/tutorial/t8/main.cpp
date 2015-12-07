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

#include <QApplication>
#include <QFont>
#include <QGridLayout>
#include <QPushButton>

//! [0]
#include "cannonfield.h"
//! [0]
#include "lcdrange.h"

//! [1]
class MyWidget : public QWidget
{
public:
    MyWidget(QWidget *parent = 0);
};
//! [1]

MyWidget::MyWidget(QWidget *parent)
    : QWidget(parent)
{
    QPushButton *quit = new QPushButton(tr("Quit"));
    quit->setFont(QFont("Times", 18, QFont::Bold));

    connect(quit, SIGNAL(clicked()), qApp, SLOT(quit()));

//! [2]
    LCDRange *angle = new LCDRange;
//! [2] //! [3]
    angle->setRange(5, 70);
//! [3]

//! [4]
    CannonField *cannonField = new CannonField;
//! [4]

//! [5]
    connect(angle, SIGNAL(valueChanged(int)),
            cannonField, SLOT(setAngle(int)));
//! [5] //! [6]
    connect(cannonField, SIGNAL(angleChanged(int)),
            angle, SLOT(setValue(int)));
//! [6]

//! [7]
    QGridLayout *gridLayout = new QGridLayout;
//! [7] //! [8]
    gridLayout->addWidget(quit, 0, 0);
//! [8] //! [9]
    gridLayout->addWidget(angle, 1, 0);
//! [9] //! [10]
    gridLayout->addWidget(cannonField, 1, 1, 2, 1);
//! [10] //! [11]
    gridLayout->setColumnStretch(1, 10);
//! [11]
    setLayout(gridLayout);

//! [12]
    angle->setValue(60);
//! [12] //! [13]
    angle->setFocus();
//! [13]
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MyWidget widget;
    widget.setGeometry(100, 100, 500, 355);
    widget.show();
    return app.exec();
}
