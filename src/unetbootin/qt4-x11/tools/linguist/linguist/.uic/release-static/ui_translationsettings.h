/********************************************************************************
** Form generated from reading ui file 'translationsettings.ui'
**
** Created: Thu Mar 5 20:53:50 2009
**      by: Qt User Interface Compiler version 4.5.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_TRANSLATIONSETTINGS_H
#define UI_TRANSLATIONSETTINGS_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_TranslationSettingsDialog
{
public:
    QVBoxLayout *verticalLayout;
    QGroupBox *srcGroupBox;
    QGridLayout *_2;
    QComboBox *srcCbLanguageList;
    QLabel *srcLblLanguage;
    QComboBox *srcCbCountryList;
    QLabel *srcLblCountry;
    QGroupBox *tgtGroupBox;
    QGridLayout *gridLayout;
    QComboBox *tgtCbLanguageList;
    QLabel *tgtLblLanguage;
    QComboBox *tgtCbCountryList;
    QLabel *tgtLblCountry;
    QSpacerItem *verticalSpacer;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *translationSettingsDialog)
    {
        if (translationSettingsDialog->objectName().isEmpty())
            translationSettingsDialog->setObjectName(QString::fromUtf8("translationSettingsDialog"));
        translationSettingsDialog->resize(416, 263);
        verticalLayout = new QVBoxLayout(translationSettingsDialog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        srcGroupBox = new QGroupBox(translationSettingsDialog);
        srcGroupBox->setObjectName(QString::fromUtf8("srcGroupBox"));
        _2 = new QGridLayout(srcGroupBox);
#ifndef Q_OS_MAC
        _2->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        _2->setMargin(9);
#endif
        _2->setObjectName(QString::fromUtf8("_2"));
        srcCbLanguageList = new QComboBox(srcGroupBox);
        srcCbLanguageList->setObjectName(QString::fromUtf8("srcCbLanguageList"));

        _2->addWidget(srcCbLanguageList, 0, 1, 1, 1);

        srcLblLanguage = new QLabel(srcGroupBox);
        srcLblLanguage->setObjectName(QString::fromUtf8("srcLblLanguage"));

        _2->addWidget(srcLblLanguage, 0, 0, 1, 1);

        srcCbCountryList = new QComboBox(srcGroupBox);
        srcCbCountryList->setObjectName(QString::fromUtf8("srcCbCountryList"));

        _2->addWidget(srcCbCountryList, 1, 1, 1, 1);

        srcLblCountry = new QLabel(srcGroupBox);
        srcLblCountry->setObjectName(QString::fromUtf8("srcLblCountry"));

        _2->addWidget(srcLblCountry, 1, 0, 1, 1);


        verticalLayout->addWidget(srcGroupBox);

        tgtGroupBox = new QGroupBox(translationSettingsDialog);
        tgtGroupBox->setObjectName(QString::fromUtf8("tgtGroupBox"));
        gridLayout = new QGridLayout(tgtGroupBox);
#ifndef Q_OS_MAC
        gridLayout->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        gridLayout->setMargin(9);
#endif
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        tgtCbLanguageList = new QComboBox(tgtGroupBox);
        tgtCbLanguageList->setObjectName(QString::fromUtf8("tgtCbLanguageList"));

        gridLayout->addWidget(tgtCbLanguageList, 0, 1, 1, 1);

        tgtLblLanguage = new QLabel(tgtGroupBox);
        tgtLblLanguage->setObjectName(QString::fromUtf8("tgtLblLanguage"));

        gridLayout->addWidget(tgtLblLanguage, 0, 0, 1, 1);

        tgtCbCountryList = new QComboBox(tgtGroupBox);
        tgtCbCountryList->setObjectName(QString::fromUtf8("tgtCbCountryList"));

        gridLayout->addWidget(tgtCbCountryList, 1, 1, 1, 1);

        tgtLblCountry = new QLabel(tgtGroupBox);
        tgtLblCountry->setObjectName(QString::fromUtf8("tgtLblCountry"));

        gridLayout->addWidget(tgtLblCountry, 1, 0, 1, 1);


        verticalLayout->addWidget(tgtGroupBox);

        verticalSpacer = new QSpacerItem(20, 100, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        buttonBox = new QDialogButtonBox(translationSettingsDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);

#ifndef QT_NO_SHORTCUT
        srcLblLanguage->setBuddy(tgtCbLanguageList);
        tgtLblLanguage->setBuddy(tgtCbLanguageList);
#endif // QT_NO_SHORTCUT

        retranslateUi(translationSettingsDialog);
        QObject::connect(buttonBox, SIGNAL(rejected()), translationSettingsDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(translationSettingsDialog);
    } // setupUi

    void retranslateUi(QDialog *translationSettingsDialog)
    {
        translationSettingsDialog->setWindowTitle(QString());
        srcGroupBox->setTitle(QApplication::translate("TranslationSettingsDialog", "Source language", 0, QApplication::UnicodeUTF8));
        srcLblLanguage->setText(QApplication::translate("TranslationSettingsDialog", "Language", 0, QApplication::UnicodeUTF8));
        srcLblCountry->setText(QApplication::translate("TranslationSettingsDialog", "Country/Region", 0, QApplication::UnicodeUTF8));
        tgtGroupBox->setTitle(QApplication::translate("TranslationSettingsDialog", "Target language", 0, QApplication::UnicodeUTF8));
        tgtLblLanguage->setText(QApplication::translate("TranslationSettingsDialog", "Language", 0, QApplication::UnicodeUTF8));
        tgtLblCountry->setText(QApplication::translate("TranslationSettingsDialog", "Country/Region", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(translationSettingsDialog);
    } // retranslateUi

};

namespace Ui {
    class TranslationSettingsDialog: public Ui_TranslationSettingsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TRANSLATIONSETTINGS_H
