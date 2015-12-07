/*
*********************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt Assistant of the Qt Toolkit.
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
** Form generated from reading ui file 'topicchooser.ui'
**
** Created: Thu Mar 5 20:35:47 2009
**      by: Qt User Interface Compiler version 4.5.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_TOPICCHOOSER_H
#define UI_TOPICCHOOSER_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_TopicChooser
{
public:
    QVBoxLayout *vboxLayout;
    QLabel *label;
    QListWidget *listbox;
    QHBoxLayout *hboxLayout;
    QSpacerItem *spacerItem;
    QPushButton *buttonDisplay;
    QPushButton *buttonCancel;

    void setupUi(QDialog *TopicChooser)
    {
        if (TopicChooser->objectName().isEmpty())
            TopicChooser->setObjectName(QString::fromUtf8("TopicChooser"));
        TopicChooser->resize(391, 223);
        TopicChooser->setSizeGripEnabled(true);
        vboxLayout = new QVBoxLayout(TopicChooser);
        vboxLayout->setSpacing(6);
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        vboxLayout->setContentsMargins(11, 11, 11, 11);
        label = new QLabel(TopicChooser);
        label->setObjectName(QString::fromUtf8("label"));

        vboxLayout->addWidget(label);

        listbox = new QListWidget(TopicChooser);
        listbox->setObjectName(QString::fromUtf8("listbox"));

        vboxLayout->addWidget(listbox);

        hboxLayout = new QHBoxLayout();
        hboxLayout->setSpacing(6);
        hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
        hboxLayout->setContentsMargins(0, 0, 0, 0);
        spacerItem = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout->addItem(spacerItem);

        buttonDisplay = new QPushButton(TopicChooser);
        buttonDisplay->setObjectName(QString::fromUtf8("buttonDisplay"));
        buttonDisplay->setAutoDefault(true);
        buttonDisplay->setDefault(true);

        hboxLayout->addWidget(buttonDisplay);

        buttonCancel = new QPushButton(TopicChooser);
        buttonCancel->setObjectName(QString::fromUtf8("buttonCancel"));
        buttonCancel->setAutoDefault(true);

        hboxLayout->addWidget(buttonCancel);


        vboxLayout->addLayout(hboxLayout);

#ifndef QT_NO_SHORTCUT
        label->setBuddy(listbox);
#endif // QT_NO_SHORTCUT

        retranslateUi(TopicChooser);

        QMetaObject::connectSlotsByName(TopicChooser);
    } // setupUi

    void retranslateUi(QDialog *TopicChooser)
    {
        TopicChooser->setWindowTitle(QApplication::translate("TopicChooser", "Choose Topic", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_WHATSTHIS
        TopicChooser->setWhatsThis(QApplication::translate("TopicChooser", "Select a topic from the list and click the <b>Display</b>-button to open the online help.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        label->setText(QApplication::translate("TopicChooser", "&Topics", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_WHATSTHIS
        listbox->setWhatsThis(QApplication::translate("TopicChooser", "Displays a list of available help topics for the keyword.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
#ifndef QT_NO_WHATSTHIS
        buttonDisplay->setWhatsThis(QApplication::translate("TopicChooser", "Open the topic selected in the list.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        buttonDisplay->setText(QApplication::translate("TopicChooser", "&Display", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_WHATSTHIS
        buttonCancel->setWhatsThis(QApplication::translate("TopicChooser", "Close the Dialog.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        buttonCancel->setText(QApplication::translate("TopicChooser", "&Close", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(TopicChooser);
    } // retranslateUi

};

namespace Ui {
    class TopicChooser: public Ui_TopicChooser {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TOPICCHOOSER_H
