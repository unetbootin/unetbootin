/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qvfbratedlg.h"
#include <QLayout>
#include <QLabel>
#include <qslider.h>
#include <QPushButton>

QT_BEGIN_NAMESPACE

QVFbRateDialog::QVFbRateDialog(int rate, QWidget *parent)
    : QDialog(parent)
{
    oldRate = rate;

    QVBoxLayout *tl = new QVBoxLayout(this);
    tl->setMargin(5);

    QLabel *label = new QLabel("Target frame rate:", this);
    tl->addWidget(label);

    QHBoxLayout *hl = new QHBoxLayout();
    tl->addItem(hl);
    rateSlider = new QSlider(Qt::Horizontal);
    rateSlider->setMinimum(1);
    rateSlider->setMaximum(100);
    rateSlider->setPageStep(10);
    rateSlider->setValue(rate);
    hl->addWidget(rateSlider);
    connect(rateSlider, SIGNAL(valueChanged(int)), this, SLOT(rateChanged(int)));
    rateLabel = new QLabel(QString("%1fps").arg(rate), this);
    hl->addWidget(rateLabel);

    hl = new QHBoxLayout();
    tl->addItem(hl);
    QPushButton *pb = new QPushButton("OK", this);
    connect(pb, SIGNAL(clicked()), this, SLOT(ok()));
    hl->addWidget(pb);
    pb = new QPushButton("Cancel", this);
    connect(pb, SIGNAL(clicked()), this, SLOT(cancel()));
    hl->addWidget(pb);
}

void QVFbRateDialog::rateChanged(int r)
{
    if (rateSlider->value() != r)
	rateSlider->setValue(r);
    rateLabel->setText(QString("%1fps").arg(r));
    emit updateRate(r);
}

void QVFbRateDialog::cancel()
{
    rateChanged(oldRate);
    reject();
}

void QVFbRateDialog::ok()
{
    oldRate = rateSlider->value();
    accept();
}

QT_END_NAMESPACE
