/*
*********************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt Designer of the Qt Toolkit.
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
** Form generated from reading ui file 'newformwidget.ui'
**
** Created: Thu Mar 5 20:40:11 2009
**      by: Qt User Interface Compiler version 4.5.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_NEWFORMWIDGET_H
#define UI_NEWFORMWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QSpacerItem>
#include <QtGui/QTreeWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {

class Ui_NewFormWidget
{
public:
    QHBoxLayout *hboxLayout;
    QTreeWidget *treeWidget;
    QVBoxLayout *verticalLayout;
    QLabel *lblPreview;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QGroupBox *embeddedGroup;
    QGridLayout *gridLayout;
    QComboBox *profileComboBox;
    QComboBox *sizeComboBox;
    QLabel *label;
    QLabel *label_2;
    QSpacerItem *horizontalSpacer_2;

    void setupUi(QWidget *qdesigner_internal__NewFormWidget)
    {
        if (qdesigner_internal__NewFormWidget->objectName().isEmpty())
            qdesigner_internal__NewFormWidget->setObjectName(QString::fromUtf8("qdesigner_internal__NewFormWidget"));
        qdesigner_internal__NewFormWidget->resize(480, 194);
        hboxLayout = new QHBoxLayout(qdesigner_internal__NewFormWidget);
#ifndef Q_OS_MAC
        hboxLayout->setSpacing(6);
#endif
        hboxLayout->setMargin(1);
        hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
        treeWidget = new QTreeWidget(qdesigner_internal__NewFormWidget);
        treeWidget->setObjectName(QString::fromUtf8("treeWidget"));
        treeWidget->setMinimumSize(QSize(200, 0));
        treeWidget->setIconSize(QSize(128, 128));
        treeWidget->setRootIsDecorated(false);
        treeWidget->setColumnCount(1);

        hboxLayout->addWidget(treeWidget);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        lblPreview = new QLabel(qdesigner_internal__NewFormWidget);
        lblPreview->setObjectName(QString::fromUtf8("lblPreview"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(lblPreview->sizePolicy().hasHeightForWidth());
        lblPreview->setSizePolicy(sizePolicy);
        lblPreview->setLineWidth(1);
        lblPreview->setAlignment(Qt::AlignCenter);
        lblPreview->setMargin(5);

        verticalLayout->addWidget(lblPreview);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(7, 0, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        embeddedGroup = new QGroupBox(qdesigner_internal__NewFormWidget);
        embeddedGroup->setObjectName(QString::fromUtf8("embeddedGroup"));
        gridLayout = new QGridLayout(embeddedGroup);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        profileComboBox = new QComboBox(embeddedGroup);
        profileComboBox->setObjectName(QString::fromUtf8("profileComboBox"));

        gridLayout->addWidget(profileComboBox, 0, 1, 1, 1);

        sizeComboBox = new QComboBox(embeddedGroup);
        sizeComboBox->setObjectName(QString::fromUtf8("sizeComboBox"));

        gridLayout->addWidget(sizeComboBox, 1, 1, 1, 1);

        label = new QLabel(embeddedGroup);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        label_2 = new QLabel(embeddedGroup);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 1, 0, 1, 1);


        horizontalLayout->addWidget(embeddedGroup);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);


        verticalLayout->addLayout(horizontalLayout);


        hboxLayout->addLayout(verticalLayout);


        retranslateUi(qdesigner_internal__NewFormWidget);

        QMetaObject::connectSlotsByName(qdesigner_internal__NewFormWidget);
    } // setupUi

    void retranslateUi(QWidget *qdesigner_internal__NewFormWidget)
    {
        QTreeWidgetItem *___qtreewidgetitem = treeWidget->headerItem();
        ___qtreewidgetitem->setText(0, QApplication::translate("qdesigner_internal::NewFormWidget", "0", 0, QApplication::UnicodeUTF8));
        lblPreview->setText(QApplication::translate("qdesigner_internal::NewFormWidget", "Choose a template for a preview", 0, QApplication::UnicodeUTF8));
        embeddedGroup->setTitle(QApplication::translate("qdesigner_internal::NewFormWidget", "Embedded Design", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("qdesigner_internal::NewFormWidget", "Device:", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("qdesigner_internal::NewFormWidget", "Screen Size:", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(qdesigner_internal__NewFormWidget);
    } // retranslateUi

};

} // namespace qdesigner_internal

namespace qdesigner_internal {
namespace Ui {
    class NewFormWidget: public Ui_NewFormWidget {};
} // namespace Ui
} // namespace qdesigner_internal

QT_END_NAMESPACE

#endif // UI_NEWFORMWIDGET_H
