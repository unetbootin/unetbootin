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
** Form generated from reading ui file 'itemlisteditor.ui'
**
** Created: Thu Mar 5 20:47:29 2009
**      by: Qt User Interface Compiler version 4.5.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_ITEMLISTEDITOR_H
#define UI_ITEMLISTEDITOR_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QToolButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {

class Ui_ItemListEditor
{
public:
    QVBoxLayout *verticalLayout_2;
    QWidget *widget;
    QVBoxLayout *verticalLayout;
    QListWidget *listWidget;
    QHBoxLayout *buttonsLayout;
    QToolButton *newListItemButton;
    QToolButton *deleteListItemButton;
    QSpacerItem *spacerItem;
    QToolButton *moveListItemUpButton;
    QToolButton *moveListItemDownButton;
    QSpacerItem *horizontalSpacer;
    QPushButton *showPropertiesButton;

    void setupUi(QWidget *qdesigner_internal__ItemListEditor)
    {
        if (qdesigner_internal__ItemListEditor->objectName().isEmpty())
            qdesigner_internal__ItemListEditor->setObjectName(QString::fromUtf8("qdesigner_internal__ItemListEditor"));
        qdesigner_internal__ItemListEditor->resize(550, 360);
        verticalLayout_2 = new QVBoxLayout(qdesigner_internal__ItemListEditor);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        widget = new QWidget(qdesigner_internal__ItemListEditor);
        widget->setObjectName(QString::fromUtf8("widget"));
        verticalLayout = new QVBoxLayout(widget);
        verticalLayout->setMargin(0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        listWidget = new QListWidget(widget);
        listWidget->setObjectName(QString::fromUtf8("listWidget"));
        listWidget->setEnabled(true);

        verticalLayout->addWidget(listWidget);

        buttonsLayout = new QHBoxLayout();
        buttonsLayout->setObjectName(QString::fromUtf8("buttonsLayout"));
        newListItemButton = new QToolButton(widget);
        newListItemButton->setObjectName(QString::fromUtf8("newListItemButton"));

        buttonsLayout->addWidget(newListItemButton);

        deleteListItemButton = new QToolButton(widget);
        deleteListItemButton->setObjectName(QString::fromUtf8("deleteListItemButton"));

        buttonsLayout->addWidget(deleteListItemButton);

        spacerItem = new QSpacerItem(16, 10, QSizePolicy::Expanding, QSizePolicy::Minimum);

        buttonsLayout->addItem(spacerItem);

        moveListItemUpButton = new QToolButton(widget);
        moveListItemUpButton->setObjectName(QString::fromUtf8("moveListItemUpButton"));

        buttonsLayout->addWidget(moveListItemUpButton);

        moveListItemDownButton = new QToolButton(widget);
        moveListItemDownButton->setObjectName(QString::fromUtf8("moveListItemDownButton"));

        buttonsLayout->addWidget(moveListItemDownButton);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        buttonsLayout->addItem(horizontalSpacer);

        showPropertiesButton = new QPushButton(widget);
        showPropertiesButton->setObjectName(QString::fromUtf8("showPropertiesButton"));

        buttonsLayout->addWidget(showPropertiesButton);


        verticalLayout->addLayout(buttonsLayout);


        verticalLayout_2->addWidget(widget);


        retranslateUi(qdesigner_internal__ItemListEditor);

        QMetaObject::connectSlotsByName(qdesigner_internal__ItemListEditor);
    } // setupUi

    void retranslateUi(QWidget *qdesigner_internal__ItemListEditor)
    {
        qdesigner_internal__ItemListEditor->setWindowTitle(QString());
#ifndef QT_NO_TOOLTIP
        listWidget->setToolTip(QApplication::translate("qdesigner_internal::ItemListEditor", "Items List", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        newListItemButton->setToolTip(QApplication::translate("qdesigner_internal::ItemListEditor", "New Item", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        newListItemButton->setText(QApplication::translate("qdesigner_internal::ItemListEditor", "&New", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        deleteListItemButton->setToolTip(QApplication::translate("qdesigner_internal::ItemListEditor", "Delete Item", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        deleteListItemButton->setText(QApplication::translate("qdesigner_internal::ItemListEditor", "&Delete", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        moveListItemUpButton->setToolTip(QApplication::translate("qdesigner_internal::ItemListEditor", "Move Item Up", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        moveListItemUpButton->setText(QApplication::translate("qdesigner_internal::ItemListEditor", "U", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        moveListItemDownButton->setToolTip(QApplication::translate("qdesigner_internal::ItemListEditor", "Move Item Down", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        moveListItemDownButton->setText(QApplication::translate("qdesigner_internal::ItemListEditor", "D", 0, QApplication::UnicodeUTF8));
        showPropertiesButton->setText(QApplication::translate("qdesigner_internal::ItemListEditor", "Properties &>>", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(qdesigner_internal__ItemListEditor);
    } // retranslateUi

};

} // namespace qdesigner_internal

namespace qdesigner_internal {
namespace Ui {
    class ItemListEditor: public Ui_ItemListEditor {};
} // namespace Ui
} // namespace qdesigner_internal

QT_END_NAMESPACE

#endif // UI_ITEMLISTEDITOR_H
