/*
*********************************************************************
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
*********************************************************************
*/

/********************************************************************************
** Form generated from reading ui file 'qtgradientviewdialog.ui'
**
** Created: Thu Mar 5 20:40:10 2009
**      by: Qt User Interface Compiler version 4.5.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_QTGRADIENTVIEWDIALOG_H
#define UI_QTGRADIENTVIEWDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHeaderView>
#include <QtGui/QVBoxLayout>
#include "qtgradientview.h"

QT_BEGIN_NAMESPACE

class Ui_QtGradientViewDialog
{
public:
    QVBoxLayout *vboxLayout;
    QtGradientView *gradientView;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *QtGradientViewDialog)
    {
        if (QtGradientViewDialog->objectName().isEmpty())
            QtGradientViewDialog->setObjectName(QString::fromUtf8("QtGradientViewDialog"));
        QtGradientViewDialog->resize(178, 72);
        vboxLayout = new QVBoxLayout(QtGradientViewDialog);
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        gradientView = new QtGradientView(QtGradientViewDialog);
        gradientView->setObjectName(QString::fromUtf8("gradientView"));
        QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(gradientView->sizePolicy().hasHeightForWidth());
        gradientView->setSizePolicy(sizePolicy);

        vboxLayout->addWidget(gradientView);

        buttonBox = new QDialogButtonBox(QtGradientViewDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        vboxLayout->addWidget(buttonBox);


        retranslateUi(QtGradientViewDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), QtGradientViewDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), QtGradientViewDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(QtGradientViewDialog);
    } // setupUi

    void retranslateUi(QDialog *QtGradientViewDialog)
    {
        QtGradientViewDialog->setWindowTitle(QApplication::translate("QtGradientViewDialog", "Select Gradient", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(QtGradientViewDialog);
    } // retranslateUi

};

namespace Ui {
    class QtGradientViewDialog: public Ui_QtGradientViewDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QTGRADIENTVIEWDIALOG_H
