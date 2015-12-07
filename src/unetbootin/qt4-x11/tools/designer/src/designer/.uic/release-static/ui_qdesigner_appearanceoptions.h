/********************************************************************************
** Form generated from reading ui file 'qdesigner_appearanceoptions.ui'
**
** Created: Thu Mar 5 20:52:48 2009
**      by: Qt User Interface Compiler version 4.5.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_QDESIGNER_APPEARANCEOPTIONS_H
#define UI_QDESIGNER_APPEARANCEOPTIONS_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include "fontpanel.h"

QT_BEGIN_NAMESPACE

class Ui_AppearanceOptionsWidget
{
public:
    QVBoxLayout *verticalLayout;
    QGroupBox *m_uiModeGroupBox;
    QVBoxLayout *vboxLayout;
    QComboBox *m_uiModeCombo;
    FontPanel *m_fontPanel;
    QSpacerItem *verticalSpacer;

    void setupUi(QWidget *AppearanceOptionsWidget)
    {
        if (AppearanceOptionsWidget->objectName().isEmpty())
            AppearanceOptionsWidget->setObjectName(QString::fromUtf8("AppearanceOptionsWidget"));
        AppearanceOptionsWidget->resize(325, 360);
        verticalLayout = new QVBoxLayout(AppearanceOptionsWidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        m_uiModeGroupBox = new QGroupBox(AppearanceOptionsWidget);
        m_uiModeGroupBox->setObjectName(QString::fromUtf8("m_uiModeGroupBox"));
        vboxLayout = new QVBoxLayout(m_uiModeGroupBox);
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        m_uiModeCombo = new QComboBox(m_uiModeGroupBox);
        m_uiModeCombo->setObjectName(QString::fromUtf8("m_uiModeCombo"));

        vboxLayout->addWidget(m_uiModeCombo);


        verticalLayout->addWidget(m_uiModeGroupBox);

        m_fontPanel = new FontPanel(AppearanceOptionsWidget);
        m_fontPanel->setObjectName(QString::fromUtf8("m_fontPanel"));

        verticalLayout->addWidget(m_fontPanel);

        verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        retranslateUi(AppearanceOptionsWidget);

        QMetaObject::connectSlotsByName(AppearanceOptionsWidget);
    } // setupUi

    void retranslateUi(QWidget *AppearanceOptionsWidget)
    {
        AppearanceOptionsWidget->setWindowTitle(QApplication::translate("AppearanceOptionsWidget", "Form", 0, QApplication::UnicodeUTF8));
        m_uiModeGroupBox->setTitle(QApplication::translate("AppearanceOptionsWidget", "User Interface Mode", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(AppearanceOptionsWidget);
    } // retranslateUi

};

namespace Ui {
    class AppearanceOptionsWidget: public Ui_AppearanceOptionsWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QDESIGNER_APPEARANCEOPTIONS_H
