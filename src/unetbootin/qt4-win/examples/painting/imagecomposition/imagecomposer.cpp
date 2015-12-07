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

#include "imagecomposer.h"

//! [0]
static const QSize resultSize(200, 200);
//! [0]

//! [1]
ImageComposer::ImageComposer()
{
    sourceButton = new QToolButton;
    sourceButton->setIconSize(resultSize);

    operatorComboBox = new QComboBox;
    addOp(QPainter::CompositionMode_SourceOver, tr("SourceOver"));
    addOp(QPainter::CompositionMode_DestinationOver, tr("DestinationOver"));
    addOp(QPainter::CompositionMode_Clear, tr("Clear"));
    addOp(QPainter::CompositionMode_Source, tr("Source"));
    addOp(QPainter::CompositionMode_Destination, tr("Destination"));
    addOp(QPainter::CompositionMode_SourceIn, tr("SourceIn"));
    addOp(QPainter::CompositionMode_DestinationIn, tr("DestinationIn"));
    addOp(QPainter::CompositionMode_SourceOut, tr("SourceOut"));
    addOp(QPainter::CompositionMode_DestinationOut, tr("DestinationOut"));
    addOp(QPainter::CompositionMode_SourceAtop, tr("SourceAtop"));
    addOp(QPainter::CompositionMode_DestinationAtop, tr("DestinationAtop"));
    addOp(QPainter::CompositionMode_Xor, tr("Xor"));
//! [1]

//! [2]
    destinationButton = new QToolButton;
    destinationButton->setIconSize(resultSize);

    equalLabel = new QLabel(tr("="));

    resultLabel = new QLabel;
    resultLabel->setMinimumWidth(resultSize.width());
//! [2]

//! [3]
    connect(sourceButton, SIGNAL(clicked()), this, SLOT(chooseSource()));
    connect(operatorComboBox, SIGNAL(activated(int)),
            this, SLOT(recalculateResult()));
    connect(destinationButton, SIGNAL(clicked()),
            this, SLOT(chooseDestination()));
//! [3]

//! [4]
    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(sourceButton, 0, 0, 3, 1);
    mainLayout->addWidget(operatorComboBox, 1, 1);
    mainLayout->addWidget(destinationButton, 0, 2, 3, 1);
    mainLayout->addWidget(equalLabel, 1, 3);
    mainLayout->addWidget(resultLabel, 0, 4, 3, 1);
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(mainLayout);
//! [4]

//! [5]
    resultImage = QImage(resultSize, QImage::Format_ARGB32_Premultiplied);

    loadImage(":/images/butterfly.png", &sourceImage, sourceButton);
    loadImage(":/images/checker.png", &destinationImage, destinationButton);

    setWindowTitle(tr("Image Composition"));
}
//! [5]

//! [6]
void ImageComposer::chooseSource()
{
    chooseImage(tr("Choose Source Image"), &sourceImage, sourceButton);
}
//! [6]

//! [7]
void ImageComposer::chooseDestination()
{
    chooseImage(tr("Choose Destination Image"), &destinationImage,
                destinationButton);
}
//! [7]

//! [8]
void ImageComposer::recalculateResult()
{
    QPainter::CompositionMode mode = currentMode();

    QPainter painter(&resultImage);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.fillRect(resultImage.rect(), Qt::transparent);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.drawImage(0, 0, destinationImage);
    painter.setCompositionMode(mode);
    painter.drawImage(0, 0, sourceImage);
    painter.setCompositionMode(QPainter::CompositionMode_DestinationOver);
    painter.fillRect(resultImage.rect(), Qt::white);
    painter.end();

    resultLabel->setPixmap(QPixmap::fromImage(resultImage));
}
//! [8]

//! [9]
void ImageComposer::addOp(QPainter::CompositionMode mode, const QString &name)
{
    operatorComboBox->addItem(name, mode);
}
//! [9]

//! [10]
void ImageComposer::chooseImage(const QString &title, QImage *image,
                                QToolButton *button)
{
    QString fileName = QFileDialog::getOpenFileName(this, title);
    if (!fileName.isEmpty())
        loadImage(fileName, image, button);
}
//! [10]

//! [11]
void ImageComposer::loadImage(const QString &fileName, QImage *image,
                              QToolButton *button)
{
    image->load(fileName);

    QImage fixedImage(resultSize, QImage::Format_ARGB32_Premultiplied);
    QPainter painter(&fixedImage);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.fillRect(fixedImage.rect(), Qt::transparent);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.drawImage(imagePos(*image), *image);
    painter.end();
    button->setIcon(QPixmap::fromImage(fixedImage));

    *image = fixedImage;

    recalculateResult();
}
//! [11]

//! [12]
QPainter::CompositionMode ImageComposer::currentMode() const
{
    return (QPainter::CompositionMode)
           operatorComboBox->itemData(operatorComboBox->currentIndex()).toInt();
}
//! [12]

//! [13]
QPoint ImageComposer::imagePos(const QImage &image) const
{
    return QPoint((resultSize.width() - image.width()) / 2,
                  (resultSize.height() - image.height()) / 2);
}
//! [13]
