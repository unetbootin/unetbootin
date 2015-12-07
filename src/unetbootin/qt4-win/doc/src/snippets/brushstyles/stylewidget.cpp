/****************************************************************************
**
** Copyright (C) 2005-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the documentation of the Qt Toolkit.
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

#include "stylewidget.h"

StyleWidget::StyleWidget(QWidget *parent)
        : QWidget(parent)
{
    solid = new RenderArea(new QBrush(Qt::SolidPattern));
    dense1 = new RenderArea(new QBrush(Qt::Dense1Pattern));
    dense2 = new RenderArea(new QBrush(Qt::Dense2Pattern));
    dense3 = new RenderArea(new QBrush(Qt::Dense3Pattern));
    dense4 = new RenderArea(new QBrush(Qt::Dense4Pattern));
    dense5 = new RenderArea(new QBrush(Qt::Dense5Pattern));
    dense6 = new RenderArea(new QBrush(Qt::Dense6Pattern));
    dense7 = new RenderArea(new QBrush(Qt::Dense7Pattern));
    no = new RenderArea(new QBrush(Qt::NoBrush));
    hor = new RenderArea(new QBrush(Qt::HorPattern));
    ver = new RenderArea(new QBrush(Qt::VerPattern));
    cross = new RenderArea(new QBrush(Qt::CrossPattern));
    bdiag = new RenderArea(new QBrush(Qt::BDiagPattern));
    fdiag = new RenderArea(new QBrush(Qt::FDiagPattern));
    diagCross = new RenderArea(new QBrush(Qt::DiagCrossPattern));
    linear = new RenderArea(new QBrush(QLinearGradient()));
    radial = new RenderArea(new QBrush(QRadialGradient()));
    conical = new RenderArea(new QBrush(QConicalGradient()));
    texture = new RenderArea(new QBrush(QPixmap("qt-logo.png")));

    solidLabel = new QLabel("Qt::SolidPattern");
    dense1Label = new QLabel("Qt::Dense1Pattern");
    dense2Label = new QLabel("Qt::Dense2Pattern");
    dense3Label = new QLabel("Qt::Dense3Pattern");
    dense4Label = new QLabel("Qt::Dense4Pattern");
    dense5Label = new QLabel("Qt::Dense5Pattern");
    dense6Label = new QLabel("Qt::Dense6Pattern");
    dense7Label = new QLabel("Qt::Dense7Pattern");
    noLabel = new QLabel("Qt::NoPattern");
    horLabel = new QLabel("Qt::HorPattern");
    verLabel = new QLabel("Qt::VerPattern");
    crossLabel = new QLabel("Qt::CrossPattern");
    bdiagLabel = new QLabel("Qt::BDiagPattern");
    fdiagLabel = new QLabel("Qt::FDiagPattern");
    diagCrossLabel = new QLabel("Qt::DiagCrossPattern");
    linearLabel = new QLabel("Qt::LinearGradientPattern");
    radialLabel = new QLabel("Qt::RadialGradientPattern");
    conicalLabel = new QLabel("Qt::ConicalGradientPattern");
    textureLabel = new QLabel("Qt::TexturePattern");

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(solid, 0, 0);
    layout->addWidget(dense1, 0, 1);
    layout->addWidget(dense2, 0, 2);
    layout->addWidget(solidLabel, 1, 0);
    layout->addWidget(dense1Label, 1, 1);
    layout->addWidget(dense2Label, 1, 2);

    layout->addWidget(dense3, 2, 0 );
    layout->addWidget(dense4, 2, 1);
    layout->addWidget(dense5, 2, 2);
    layout->addWidget(dense3Label, 3, 0);
    layout->addWidget(dense4Label, 3, 1);
    layout->addWidget(dense5Label, 3, 2);

    layout->addWidget(dense6, 4, 0);
    layout->addWidget(dense7, 4, 1);
    layout->addWidget(no, 4, 2);
    layout->addWidget(dense6Label, 5, 0);
    layout->addWidget(dense7Label, 5, 1);
    layout->addWidget(noLabel, 5, 2);

    layout->addWidget(hor, 6, 0);
    layout->addWidget(ver, 6, 1);
    layout->addWidget(cross, 6, 2);
    layout->addWidget(horLabel, 7, 0);
    layout->addWidget(verLabel, 7, 1);
    layout->addWidget(crossLabel, 7, 2);

    layout->addWidget(bdiag, 8, 0);
    layout->addWidget(fdiag, 8, 1);
    layout->addWidget(diagCross, 8, 2);
    layout->addWidget(bdiagLabel, 9, 0);
    layout->addWidget(fdiagLabel, 9, 1);
    layout->addWidget(diagCrossLabel, 9, 2);

    layout->addWidget(linear, 10, 0);
    layout->addWidget(radial, 10, 1);
    layout->addWidget(conical, 10, 2);
    layout->addWidget(linearLabel, 11, 0);
    layout->addWidget(radialLabel, 11, 1);
    layout->addWidget(conicalLabel, 11, 2);

    layout->addWidget(texture, 12, 0, 1, 3);
    layout->addWidget(textureLabel, 13, 0, 1, 3);

    setLayout(layout);

    QPalette newPalette = palette();
    newPalette.setColor(QPalette::Window, Qt::white);
    setPalette(newPalette);

    setWindowTitle(tr("Brush Styles"));
    resize(430, 605);
}


