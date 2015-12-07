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

#include "iconpreviewarea.h"

//! [0]
IconPreviewArea::IconPreviewArea(QWidget *parent)
    : QWidget(parent)
{
    QGridLayout *mainLayout = new QGridLayout;
    setLayout(mainLayout);

    stateLabels[0] = createHeaderLabel(tr("Off"));
    stateLabels[1] = createHeaderLabel(tr("On"));
    Q_ASSERT(NumStates == 2);

    modeLabels[0] = createHeaderLabel(tr("Normal"));
    modeLabels[1] = createHeaderLabel(tr("Active"));
    modeLabels[2] = createHeaderLabel(tr("Disabled"));
    modeLabels[3] = createHeaderLabel(tr("Selected"));
    Q_ASSERT(NumModes == 4);

    for (int j = 0; j < NumStates; ++j)
        mainLayout->addWidget(stateLabels[j], j + 1, 0);

    for (int i = 0; i < NumModes; ++i) {
        mainLayout->addWidget(modeLabels[i], 0, i + 1);

        for (int j = 0; j < NumStates; ++j) {
            pixmapLabels[i][j] = createPixmapLabel();
            mainLayout->addWidget(pixmapLabels[i][j], j + 1, i + 1);
        }
    }
}
//! [0]

//! [1]
void IconPreviewArea::setIcon(const QIcon &icon)
{
    this->icon = icon;
    updatePixmapLabels();
}
//! [1]

//! [2]
void IconPreviewArea::setSize(const QSize &size)
{
    if (size != this->size) {
        this->size = size;
        updatePixmapLabels();
    }
}
//! [2]

//! [3]
QLabel *IconPreviewArea::createHeaderLabel(const QString &text)
{
    QLabel *label = new QLabel(tr("<b>%1</b>").arg(text));
    label->setAlignment(Qt::AlignCenter);
    return label;
}
//! [3]

//! [4]
QLabel *IconPreviewArea::createPixmapLabel()
{
    QLabel *label = new QLabel;
    label->setEnabled(false);
    label->setAlignment(Qt::AlignCenter);
    label->setFrameShape(QFrame::Box);
    label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    label->setBackgroundRole(QPalette::Base);
    label->setAutoFillBackground(true);
    label->setMinimumSize(132, 132);
    return label;
}
//! [4]

//! [5]
void IconPreviewArea::updatePixmapLabels()
{
    for (int i = 0; i < NumModes; ++i) {
        QIcon::Mode mode;
        if (i == 0) {
            mode = QIcon::Normal;
        } else if (i == 1) {
            mode = QIcon::Active;
        } else if (i == 2) {
            mode = QIcon::Disabled;
        } else {
            mode = QIcon::Selected;
        }

        for (int j = 0; j < NumStates; ++j) {
            QIcon::State state = (j == 0) ? QIcon::Off : QIcon::On;
            QPixmap pixmap = icon.pixmap(size, mode, state);
            pixmapLabels[i][j]->setPixmap(pixmap);
            pixmapLabels[i][j]->setEnabled(!pixmap.isNull());
        }
    }
}
//! [5]
