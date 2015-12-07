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
Window::Window()
{
    originalRenderArea = new RenderArea;

    shapeComboBox = new QComboBox;
    shapeComboBox->addItem(tr("Clock"));
    shapeComboBox->addItem(tr("House"));
    shapeComboBox->addItem(tr("Text"));
    shapeComboBox->addItem(tr("Truck"));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(originalRenderArea, 0, 0);
    layout->addWidget(shapeComboBox, 1, 0);
//! [0]

//! [1]
    for (int i = 0; i < NumTransformedAreas; ++i) {
        transformedRenderAreas[i] = new RenderArea;

        operationComboBoxes[i] = new QComboBox;
        operationComboBoxes[i]->addItem(tr("No transformation"));
        operationComboBoxes[i]->addItem(tr("Rotate by 60\xB0"));
        operationComboBoxes[i]->addItem(tr("Scale to 75%"));
        operationComboBoxes[i]->addItem(tr("Translate by (50, 50)"));

        connect(operationComboBoxes[i], SIGNAL(activated(int)),
                this, SLOT(operationChanged()));

        layout->addWidget(transformedRenderAreas[i], 0, i + 1);
        layout->addWidget(operationComboBoxes[i], 1, i + 1);
    }
//! [1]

//! [2]
    setLayout(layout);
    setupShapes();
    shapeSelected(0);

    setWindowTitle(tr("Transformations"));
}
//! [2]

//! [3]
void Window::setupShapes()
{
    QPainterPath truck;
//! [3]
    truck.setFillRule(Qt::WindingFill);
    truck.moveTo(0.0, 87.0);
    truck.lineTo(0.0, 60.0);
    truck.lineTo(10.0, 60.0);
    truck.lineTo(35.0, 35.0);
    truck.lineTo(100.0, 35.0);
    truck.lineTo(100.0, 87.0);
    truck.lineTo(0.0, 87.0);
    truck.moveTo(17.0, 60.0);
    truck.lineTo(55.0, 60.0);
    truck.lineTo(55.0, 40.0);
    truck.lineTo(37.0, 40.0);
    truck.lineTo(17.0, 60.0);
    truck.addEllipse(17.0, 75.0, 25.0, 25.0);
    truck.addEllipse(63.0, 75.0, 25.0, 25.0);

//! [4]
    QPainterPath clock;
//! [4]
    clock.addEllipse(-50.0, -50.0, 100.0, 100.0);
    clock.addEllipse(-48.0, -48.0, 96.0, 96.0);
    clock.moveTo(0.0, 0.0);
    clock.lineTo(-2.0, -2.0);
    clock.lineTo(0.0, -42.0);
    clock.lineTo(2.0, -2.0);
    clock.lineTo(0.0, 0.0);
    clock.moveTo(0.0, 0.0);
    clock.lineTo(2.732, -0.732);
    clock.lineTo(24.495, 14.142);
    clock.lineTo(0.732, 2.732);
    clock.lineTo(0.0, 0.0);

//! [5]
    QPainterPath house;
//! [5]
    house.moveTo(-45.0, -20.0);
    house.lineTo(0.0, -45.0);
    house.lineTo(45.0, -20.0);
    house.lineTo(45.0, 45.0);
    house.lineTo(-45.0, 45.0);
    house.lineTo(-45.0, -20.0);
    house.addRect(15.0, 5.0, 20.0, 35.0);
    house.addRect(-35.0, -15.0, 25.0, 25.0);

//! [6]
    QPainterPath text;
//! [6]
    QFont font;
    font.setPixelSize(50);
    QRect fontBoundingRect = QFontMetrics(font).boundingRect(tr("Qt"));
    text.addText(-QPointF(fontBoundingRect.center()), font, tr("Qt"));

//! [7]
    shapes.append(clock);
    shapes.append(house);
    shapes.append(text);
    shapes.append(truck);

    connect(shapeComboBox, SIGNAL(activated(int)),
            this, SLOT(shapeSelected(int)));
}
//! [7]

//! [8]
void Window::operationChanged()
{
    static const Operation operationTable[] = {
        NoTransformation, Rotate, Scale, Translate
    };

    QList<Operation> operations;
    for (int i = 0; i < NumTransformedAreas; ++i) {
        int index = operationComboBoxes[i]->currentIndex();
        operations.append(operationTable[index]);
        transformedRenderAreas[i]->setOperations(operations);
    }
}
//! [8]

//! [9]
void Window::shapeSelected(int index)
{
    QPainterPath shape = shapes[index];
    originalRenderArea->setShape(shape);
    for (int i = 0; i < NumTransformedAreas; ++i)
        transformedRenderAreas[i]->setShape(shape);
}
//! [9]
