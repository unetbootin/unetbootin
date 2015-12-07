/*
*********************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt Linguist of the Qt Toolkit.
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
** Form generated from reading ui file 'statistics.ui'
**
** Created: Thu Mar 5 20:53:50 2009
**      by: Qt User Interface Compiler version 4.5.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_STATISTICS_H
#define UI_STATISTICS_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>

QT_BEGIN_NAMESPACE

class Ui_Statistics
{
public:
    QGridLayout *gridLayout;
    QHBoxLayout *hboxLayout;
    QSpacerItem *spacer4_2;
    QPushButton *closeBtn;
    QSpacerItem *spacer4;
    QFrame *frame4;
    QGridLayout *gridLayout1;
    QLabel *textLabel4;
    QLabel *textLabel5;
    QLabel *untrWords;
    QLabel *trWords;
    QLabel *textLabel1;
    QLabel *trChars;
    QLabel *untrChars;
    QLabel *textLabel3;
    QLabel *textLabel6;
    QLabel *trCharsSpc;
    QLabel *untrCharsSpc;

    void setupUi(QDialog *linguist_stats)
    {
        if (linguist_stats->objectName().isEmpty())
            linguist_stats->setObjectName(QString::fromUtf8("linguist_stats"));
        linguist_stats->resize(336, 169);
        gridLayout = new QGridLayout(linguist_stats);
        gridLayout->setSpacing(6);
        gridLayout->setMargin(11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        hboxLayout = new QHBoxLayout();
        hboxLayout->setSpacing(6);
        hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
        spacer4_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout->addItem(spacer4_2);

        closeBtn = new QPushButton(linguist_stats);
        closeBtn->setObjectName(QString::fromUtf8("closeBtn"));

        hboxLayout->addWidget(closeBtn);

        spacer4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout->addItem(spacer4);


        gridLayout->addLayout(hboxLayout, 1, 0, 1, 1);

        frame4 = new QFrame(linguist_stats);
        frame4->setObjectName(QString::fromUtf8("frame4"));
        frame4->setFrameShape(QFrame::StyledPanel);
        frame4->setFrameShadow(QFrame::Raised);
        gridLayout1 = new QGridLayout(frame4);
        gridLayout1->setSpacing(6);
        gridLayout1->setMargin(11);
        gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
        textLabel4 = new QLabel(frame4);
        textLabel4->setObjectName(QString::fromUtf8("textLabel4"));

        gridLayout1->addWidget(textLabel4, 0, 2, 1, 1);

        textLabel5 = new QLabel(frame4);
        textLabel5->setObjectName(QString::fromUtf8("textLabel5"));

        gridLayout1->addWidget(textLabel5, 0, 1, 1, 1);

        untrWords = new QLabel(frame4);
        untrWords->setObjectName(QString::fromUtf8("untrWords"));

        gridLayout1->addWidget(untrWords, 1, 1, 1, 1);

        trWords = new QLabel(frame4);
        trWords->setObjectName(QString::fromUtf8("trWords"));

        gridLayout1->addWidget(trWords, 1, 2, 1, 1);

        textLabel1 = new QLabel(frame4);
        textLabel1->setObjectName(QString::fromUtf8("textLabel1"));

        gridLayout1->addWidget(textLabel1, 1, 0, 1, 1);

        trChars = new QLabel(frame4);
        trChars->setObjectName(QString::fromUtf8("trChars"));

        gridLayout1->addWidget(trChars, 2, 2, 1, 1);

        untrChars = new QLabel(frame4);
        untrChars->setObjectName(QString::fromUtf8("untrChars"));

        gridLayout1->addWidget(untrChars, 2, 1, 1, 1);

        textLabel3 = new QLabel(frame4);
        textLabel3->setObjectName(QString::fromUtf8("textLabel3"));

        gridLayout1->addWidget(textLabel3, 2, 0, 1, 1);

        textLabel6 = new QLabel(frame4);
        textLabel6->setObjectName(QString::fromUtf8("textLabel6"));

        gridLayout1->addWidget(textLabel6, 3, 0, 1, 1);

        trCharsSpc = new QLabel(frame4);
        trCharsSpc->setObjectName(QString::fromUtf8("trCharsSpc"));

        gridLayout1->addWidget(trCharsSpc, 3, 2, 1, 1);

        untrCharsSpc = new QLabel(frame4);
        untrCharsSpc->setObjectName(QString::fromUtf8("untrCharsSpc"));

        gridLayout1->addWidget(untrCharsSpc, 3, 1, 1, 1);


        gridLayout->addWidget(frame4, 0, 0, 1, 1);


        retranslateUi(linguist_stats);
        QObject::connect(closeBtn, SIGNAL(clicked()), linguist_stats, SLOT(reject()));

        QMetaObject::connectSlotsByName(linguist_stats);
    } // setupUi

    void retranslateUi(QDialog *linguist_stats)
    {
        linguist_stats->setWindowTitle(QApplication::translate("Statistics", "Statistics", 0, QApplication::UnicodeUTF8));
        closeBtn->setText(QApplication::translate("Statistics", "Close", 0, QApplication::UnicodeUTF8));
        textLabel4->setText(QApplication::translate("Statistics", "Translation", 0, QApplication::UnicodeUTF8));
        textLabel5->setText(QApplication::translate("Statistics", "Source", 0, QApplication::UnicodeUTF8));
        untrWords->setText(QApplication::translate("Statistics", "0", 0, QApplication::UnicodeUTF8));
        trWords->setText(QApplication::translate("Statistics", "0", 0, QApplication::UnicodeUTF8));
        textLabel1->setText(QApplication::translate("Statistics", "Words:", 0, QApplication::UnicodeUTF8));
        trChars->setText(QApplication::translate("Statistics", "0", 0, QApplication::UnicodeUTF8));
        untrChars->setText(QApplication::translate("Statistics", "0", 0, QApplication::UnicodeUTF8));
        textLabel3->setText(QApplication::translate("Statistics", "Characters:", 0, QApplication::UnicodeUTF8));
        textLabel6->setText(QApplication::translate("Statistics", "Characters (with spaces):", 0, QApplication::UnicodeUTF8));
        trCharsSpc->setText(QApplication::translate("Statistics", "0", 0, QApplication::UnicodeUTF8));
        untrCharsSpc->setText(QApplication::translate("Statistics", "0", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(linguist_stats);
    } // retranslateUi

};

namespace Ui {
    class Statistics: public Ui_Statistics {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_STATISTICS_H
