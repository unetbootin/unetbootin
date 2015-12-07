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
** Form generated from reading ui file 'treewidgeteditor.ui'
**
** Created: Thu Mar 5 20:47:29 2009
**      by: Qt User Interface Compiler version 4.5.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_TREEWIDGETEDITOR_H
#define UI_TREEWIDGETEDITOR_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QTabWidget>
#include <QtGui/QToolButton>
#include <QtGui/QTreeWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {

class Ui_TreeWidgetEditor
{
public:
    QVBoxLayout *verticalLayout;
    QTabWidget *tabWidget;
    QWidget *itemsTab;
    QVBoxLayout *verticalLayout_3;
    QWidget *widget;
    QVBoxLayout *verticalLayout_2;
    QTreeWidget *treeWidget;
    QHBoxLayout *buttonsLayout;
    QToolButton *newItemButton;
    QToolButton *newSubItemButton;
    QToolButton *deleteItemButton;
    QSpacerItem *spacerItem;
    QToolButton *moveItemLeftButton;
    QToolButton *moveItemRightButton;
    QToolButton *moveItemUpButton;
    QToolButton *moveItemDownButton;
    QSpacerItem *horizontalSpacer;
    QPushButton *showPropertiesButton;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *qdesigner_internal__TreeWidgetEditor)
    {
        if (qdesigner_internal__TreeWidgetEditor->objectName().isEmpty())
            qdesigner_internal__TreeWidgetEditor->setObjectName(QString::fromUtf8("qdesigner_internal__TreeWidgetEditor"));
        qdesigner_internal__TreeWidgetEditor->resize(700, 360);
        verticalLayout = new QVBoxLayout(qdesigner_internal__TreeWidgetEditor);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        tabWidget = new QTabWidget(qdesigner_internal__TreeWidgetEditor);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        itemsTab = new QWidget();
        itemsTab->setObjectName(QString::fromUtf8("itemsTab"));
        verticalLayout_3 = new QVBoxLayout(itemsTab);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(9, 9, 9, -1);
        widget = new QWidget(itemsTab);
        widget->setObjectName(QString::fromUtf8("widget"));
        verticalLayout_2 = new QVBoxLayout(widget);
        verticalLayout_2->setMargin(0);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        treeWidget = new QTreeWidget(widget);
        treeWidget->setObjectName(QString::fromUtf8("treeWidget"));
        treeWidget->setFocusPolicy(Qt::WheelFocus);

        verticalLayout_2->addWidget(treeWidget);

        buttonsLayout = new QHBoxLayout();
        buttonsLayout->setObjectName(QString::fromUtf8("buttonsLayout"));
        newItemButton = new QToolButton(widget);
        newItemButton->setObjectName(QString::fromUtf8("newItemButton"));

        buttonsLayout->addWidget(newItemButton);

        newSubItemButton = new QToolButton(widget);
        newSubItemButton->setObjectName(QString::fromUtf8("newSubItemButton"));

        buttonsLayout->addWidget(newSubItemButton);

        deleteItemButton = new QToolButton(widget);
        deleteItemButton->setObjectName(QString::fromUtf8("deleteItemButton"));

        buttonsLayout->addWidget(deleteItemButton);

        spacerItem = new QSpacerItem(28, 23, QSizePolicy::Expanding, QSizePolicy::Minimum);

        buttonsLayout->addItem(spacerItem);

        moveItemLeftButton = new QToolButton(widget);
        moveItemLeftButton->setObjectName(QString::fromUtf8("moveItemLeftButton"));

        buttonsLayout->addWidget(moveItemLeftButton);

        moveItemRightButton = new QToolButton(widget);
        moveItemRightButton->setObjectName(QString::fromUtf8("moveItemRightButton"));

        buttonsLayout->addWidget(moveItemRightButton);

        moveItemUpButton = new QToolButton(widget);
        moveItemUpButton->setObjectName(QString::fromUtf8("moveItemUpButton"));

        buttonsLayout->addWidget(moveItemUpButton);

        moveItemDownButton = new QToolButton(widget);
        moveItemDownButton->setObjectName(QString::fromUtf8("moveItemDownButton"));

        buttonsLayout->addWidget(moveItemDownButton);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        buttonsLayout->addItem(horizontalSpacer);

        showPropertiesButton = new QPushButton(widget);
        showPropertiesButton->setObjectName(QString::fromUtf8("showPropertiesButton"));

        buttonsLayout->addWidget(showPropertiesButton);


        verticalLayout_2->addLayout(buttonsLayout);


        verticalLayout_3->addWidget(widget);

        tabWidget->addTab(itemsTab, QString());

        verticalLayout->addWidget(tabWidget);

        buttonBox = new QDialogButtonBox(qdesigner_internal__TreeWidgetEditor);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(qdesigner_internal__TreeWidgetEditor);
        QObject::connect(buttonBox, SIGNAL(accepted()), qdesigner_internal__TreeWidgetEditor, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), qdesigner_internal__TreeWidgetEditor, SLOT(reject()));

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(qdesigner_internal__TreeWidgetEditor);
    } // setupUi

    void retranslateUi(QDialog *qdesigner_internal__TreeWidgetEditor)
    {
        qdesigner_internal__TreeWidgetEditor->setWindowTitle(QApplication::translate("qdesigner_internal::TreeWidgetEditor", "Edit Tree Widget", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem = treeWidget->headerItem();
        ___qtreewidgetitem->setText(0, QApplication::translate("qdesigner_internal::TreeWidgetEditor", "1", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        treeWidget->setToolTip(QApplication::translate("qdesigner_internal::TreeWidgetEditor", "Tree Items", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        newItemButton->setToolTip(QApplication::translate("qdesigner_internal::TreeWidgetEditor", "New Item", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        newItemButton->setText(QApplication::translate("qdesigner_internal::TreeWidgetEditor", "&New", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        newSubItemButton->setToolTip(QApplication::translate("qdesigner_internal::TreeWidgetEditor", "New Subitem", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        newSubItemButton->setText(QApplication::translate("qdesigner_internal::TreeWidgetEditor", "New &Subitem", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        deleteItemButton->setToolTip(QApplication::translate("qdesigner_internal::TreeWidgetEditor", "Delete Item", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        deleteItemButton->setText(QApplication::translate("qdesigner_internal::TreeWidgetEditor", "&Delete", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        moveItemLeftButton->setToolTip(QApplication::translate("qdesigner_internal::TreeWidgetEditor", "Move Item Left (before Parent Item)", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        moveItemLeftButton->setText(QApplication::translate("qdesigner_internal::TreeWidgetEditor", "L", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        moveItemRightButton->setToolTip(QApplication::translate("qdesigner_internal::TreeWidgetEditor", "Move Item Right (as a First Subitem of the Next Sibling Item)", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        moveItemRightButton->setText(QApplication::translate("qdesigner_internal::TreeWidgetEditor", "R", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        moveItemUpButton->setToolTip(QApplication::translate("qdesigner_internal::TreeWidgetEditor", "Move Item Up", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        moveItemUpButton->setText(QApplication::translate("qdesigner_internal::TreeWidgetEditor", "U", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        moveItemDownButton->setToolTip(QApplication::translate("qdesigner_internal::TreeWidgetEditor", "Move Item Down", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        moveItemDownButton->setText(QApplication::translate("qdesigner_internal::TreeWidgetEditor", "D", 0, QApplication::UnicodeUTF8));
        showPropertiesButton->setText(QApplication::translate("qdesigner_internal::TreeWidgetEditor", "Properties &>>", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(itemsTab), QApplication::translate("qdesigner_internal::TreeWidgetEditor", "&Items", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(qdesigner_internal__TreeWidgetEditor);
    } // retranslateUi

};

} // namespace qdesigner_internal

namespace qdesigner_internal {
namespace Ui {
    class TreeWidgetEditor: public Ui_TreeWidgetEditor {};
} // namespace Ui
} // namespace qdesigner_internal

QT_END_NAMESPACE

#endif // UI_TREEWIDGETEDITOR_H
