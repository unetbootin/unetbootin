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
** Form generated from reading ui file 'qtgradientdialog.ui'
**
** Created: Thu Mar 5 20:40:10 2009
**      by: Qt User Interface Compiler version 4.5.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_QTGRADIENTDIALOG_H
#define UI_QTGRADIENTDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHeaderView>
#include <QtGui/QVBoxLayout>
#include "qtgradienteditor.h"

QT_BEGIN_NAMESPACE

class Ui_QtGradientDialog
{
public:
    QVBoxLayout *vboxLayout;
    QtGradientEditor *gradientEditor;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *QtGradientDialog)
    {
        if (QtGradientDialog->objectName().isEmpty())
            QtGradientDialog->setObjectName(QString::fromUtf8("QtGradientDialog"));
        QtGradientDialog->resize(178, 81);
        vboxLayout = new QVBoxLayout(QtGradientDialog);
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        gradientEditor = new QtGradientEditor(QtGradientDialog);
        gradientEditor->setObjectName(QString::fromUtf8("gradientEditor"));
        QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(gradientEditor->sizePolicy().hasHeightForWidth());
        gradientEditor->setSizePolicy(sizePolicy);

        vboxLayout->addWidget(gradientEditor);

        buttonBox = new QDialogButtonBox(QtGradientDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::NoButton|QDialogButtonBox::Ok);

        vboxLayout->addWidget(buttonBox);


        retranslateUi(QtGradientDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), QtGradientDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), QtGradientDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(QtGradientDialog);
    } // setupUi

    void retranslateUi(QDialog *QtGradientDialog)
    {
        QtGradientDialog->setWindowTitle(QApplication::translate("QtGradientDialog", "Edit Gradient", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(QtGradientDialog);
    } // retranslateUi

};

namespace Ui {
    class QtGradientDialog: public Ui_QtGradientDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QTGRADIENTDIALOG_H
