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

#include "slidersgroup.h"

//! [0]
SlidersGroup::SlidersGroup(Qt::Orientation orientation, const QString &title,
                           QWidget *parent)
    : QGroupBox(title, parent)
{
    slider = new QSlider(orientation);
    slider->setFocusPolicy(Qt::StrongFocus);
    slider->setTickPosition(QSlider::TicksBothSides);
    slider->setTickInterval(10);
    slider->setSingleStep(1);

    scrollBar = new QScrollBar(orientation);
    scrollBar->setFocusPolicy(Qt::StrongFocus);

    dial = new QDial;
    dial->setFocusPolicy(Qt::StrongFocus);

    connect(slider, SIGNAL(valueChanged(int)), scrollBar, SLOT(setValue(int)));
    connect(scrollBar, SIGNAL(valueChanged(int)), dial, SLOT(setValue(int)));
    connect(dial, SIGNAL(valueChanged(int)), slider, SLOT(setValue(int)));
//! [0] //! [1]
    connect(dial, SIGNAL(valueChanged(int)), this, SIGNAL(valueChanged(int)));
//! [1] //! [2]

//! [2] //! [3]
    QBoxLayout::Direction direction;
//! [3] //! [4]

    if (orientation == Qt::Horizontal)
        direction = QBoxLayout::TopToBottom;
    else
        direction = QBoxLayout::LeftToRight;

    QBoxLayout *slidersLayout = new QBoxLayout(direction);
    slidersLayout->addWidget(slider);
    slidersLayout->addWidget(scrollBar);
    slidersLayout->addWidget(dial);
    setLayout(slidersLayout);
}
//! [4]

//! [5]
void SlidersGroup::setValue(int value)
//! [5] //! [6]
{
    slider->setValue(value);
}
//! [6]

//! [7]
void SlidersGroup::setMinimum(int value)
//! [7] //! [8]
{
    slider->setMinimum(value);
    scrollBar->setMinimum(value);
    dial->setMinimum(value);
}
//! [8]

//! [9]
void SlidersGroup::setMaximum(int value)
//! [9] //! [10]
{
    slider->setMaximum(value);
    scrollBar->setMaximum(value);
    dial->setMaximum(value);
}
//! [10]

//! [11]
void SlidersGroup::invertAppearance(bool invert)
//! [11] //! [12]
{
    slider->setInvertedAppearance(invert);
    scrollBar->setInvertedAppearance(invert);
    dial->setInvertedAppearance(invert);
}
//! [12]

//! [13]
void SlidersGroup::invertKeyBindings(bool invert)
//! [13] //! [14]
{
    slider->setInvertedControls(invert);
    scrollBar->setInvertedControls(invert);
    dial->setInvertedControls(invert);
}
//! [14]
