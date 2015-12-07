/********************************************************************************
** Form generated from reading ui file 'preferencesdialog.ui'
**
** Created: Thu Mar 5 20:32:51 2009
**      by: Qt User Interface Compiler version 4.5.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_PREFERENCESDIALOG_H
#define UI_PREFERENCESDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QStackedWidget>
#include <QtGui/QTabWidget>
#include <QtGui/QTreeWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PreferencesDialogClass
{
public:
    QVBoxLayout *vboxLayout;
    QTabWidget *tabWidget;
    QWidget *fontsTab;
    QGridLayout *gridLayout;
    QHBoxLayout *hboxLayout;
    QLabel *fontLabel;
    QComboBox *comboBox;
    QStackedWidget *stackedWidget_2;
    QWidget *page_4;
    QWidget *filtersTab;
    QGridLayout *gridLayout1;
    QLabel *label;
    QLabel *label_2;
    QListWidget *filterWidget;
    QTreeWidget *attributeWidget;
    QPushButton *filterAddButton;
    QPushButton *filterRemoveButton;
    QWidget *docsTab;
    QVBoxLayout *vboxLayout1;
    QLabel *label_3;
    QHBoxLayout *hboxLayout1;
    QListWidget *registeredDocsListWidget;
    QVBoxLayout *vboxLayout2;
    QPushButton *docAddButton;
    QPushButton *docRemoveButton;
    QSpacerItem *spacerItem;
    QWidget *optionsTab;
    QVBoxLayout *verticalLayout_3;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout_2;
    QLineEdit *homePageLineEdit;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer;
    QHBoxLayout *horizontalLayout;
    QPushButton *currentPageButton;
    QPushButton *restoreDefaultHomePageButton;
    QSpacerItem *verticalSpacer_2;
    QHBoxLayout *hboxLayout2;
    QSpacerItem *spacerItem1;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *PreferencesDialogClass)
    {
        if (PreferencesDialogClass->objectName().isEmpty())
            PreferencesDialogClass->setObjectName(QString::fromUtf8("PreferencesDialogClass"));
        PreferencesDialogClass->resize(359, 266);
        vboxLayout = new QVBoxLayout(PreferencesDialogClass);
        vboxLayout->setSpacing(6);
        vboxLayout->setMargin(11);
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        tabWidget = new QTabWidget(PreferencesDialogClass);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        fontsTab = new QWidget();
        fontsTab->setObjectName(QString::fromUtf8("fontsTab"));
        gridLayout = new QGridLayout(fontsTab);
        gridLayout->setSpacing(6);
        gridLayout->setMargin(11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        hboxLayout = new QHBoxLayout();
        hboxLayout->setSpacing(6);
        hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
        fontLabel = new QLabel(fontsTab);
        fontLabel->setObjectName(QString::fromUtf8("fontLabel"));
        QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(fontLabel->sizePolicy().hasHeightForWidth());
        fontLabel->setSizePolicy(sizePolicy);

        hboxLayout->addWidget(fontLabel);

        comboBox = new QComboBox(fontsTab);
        comboBox->setObjectName(QString::fromUtf8("comboBox"));

        hboxLayout->addWidget(comboBox);


        gridLayout->addLayout(hboxLayout, 0, 0, 1, 1);

        stackedWidget_2 = new QStackedWidget(fontsTab);
        stackedWidget_2->setObjectName(QString::fromUtf8("stackedWidget_2"));
        page_4 = new QWidget();
        page_4->setObjectName(QString::fromUtf8("page_4"));
        stackedWidget_2->addWidget(page_4);

        gridLayout->addWidget(stackedWidget_2, 1, 0, 1, 1);

        tabWidget->addTab(fontsTab, QString());
        filtersTab = new QWidget();
        filtersTab->setObjectName(QString::fromUtf8("filtersTab"));
        gridLayout1 = new QGridLayout(filtersTab);
        gridLayout1->setSpacing(6);
        gridLayout1->setMargin(11);
        gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
        label = new QLabel(filtersTab);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout1->addWidget(label, 0, 0, 1, 2);

        label_2 = new QLabel(filtersTab);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setFrameShape(QFrame::NoFrame);

        gridLayout1->addWidget(label_2, 0, 2, 1, 1);

        filterWidget = new QListWidget(filtersTab);
        filterWidget->setObjectName(QString::fromUtf8("filterWidget"));

        gridLayout1->addWidget(filterWidget, 1, 0, 1, 2);

        attributeWidget = new QTreeWidget(filtersTab);
        attributeWidget->setObjectName(QString::fromUtf8("attributeWidget"));

        gridLayout1->addWidget(attributeWidget, 1, 2, 2, 1);

        filterAddButton = new QPushButton(filtersTab);
        filterAddButton->setObjectName(QString::fromUtf8("filterAddButton"));

        gridLayout1->addWidget(filterAddButton, 2, 0, 1, 1);

        filterRemoveButton = new QPushButton(filtersTab);
        filterRemoveButton->setObjectName(QString::fromUtf8("filterRemoveButton"));

        gridLayout1->addWidget(filterRemoveButton, 2, 1, 1, 1);

        tabWidget->addTab(filtersTab, QString());
        docsTab = new QWidget();
        docsTab->setObjectName(QString::fromUtf8("docsTab"));
        vboxLayout1 = new QVBoxLayout(docsTab);
        vboxLayout1->setSpacing(6);
        vboxLayout1->setMargin(11);
        vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));
        label_3 = new QLabel(docsTab);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        vboxLayout1->addWidget(label_3);

        hboxLayout1 = new QHBoxLayout();
        hboxLayout1->setSpacing(6);
        hboxLayout1->setMargin(0);
        hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
        registeredDocsListWidget = new QListWidget(docsTab);
        registeredDocsListWidget->setObjectName(QString::fromUtf8("registeredDocsListWidget"));
        registeredDocsListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);

        hboxLayout1->addWidget(registeredDocsListWidget);

        vboxLayout2 = new QVBoxLayout();
        vboxLayout2->setSpacing(6);
        vboxLayout2->setMargin(0);
        vboxLayout2->setObjectName(QString::fromUtf8("vboxLayout2"));
        docAddButton = new QPushButton(docsTab);
        docAddButton->setObjectName(QString::fromUtf8("docAddButton"));

        vboxLayout2->addWidget(docAddButton);

        docRemoveButton = new QPushButton(docsTab);
        docRemoveButton->setObjectName(QString::fromUtf8("docRemoveButton"));

        vboxLayout2->addWidget(docRemoveButton);

        spacerItem = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        vboxLayout2->addItem(spacerItem);


        hboxLayout1->addLayout(vboxLayout2);


        vboxLayout1->addLayout(hboxLayout1);

        tabWidget->addTab(docsTab, QString());
        optionsTab = new QWidget();
        optionsTab->setObjectName(QString::fromUtf8("optionsTab"));
        verticalLayout_3 = new QVBoxLayout(optionsTab);
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setMargin(11);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        groupBox = new QGroupBox(optionsTab);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        verticalLayout_2 = new QVBoxLayout(groupBox);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setMargin(11);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        homePageLineEdit = new QLineEdit(groupBox);
        homePageLineEdit->setObjectName(QString::fromUtf8("homePageLineEdit"));

        verticalLayout_2->addWidget(homePageLineEdit);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        currentPageButton = new QPushButton(groupBox);
        currentPageButton->setObjectName(QString::fromUtf8("currentPageButton"));

        horizontalLayout->addWidget(currentPageButton);

        restoreDefaultHomePageButton = new QPushButton(groupBox);
        restoreDefaultHomePageButton->setObjectName(QString::fromUtf8("restoreDefaultHomePageButton"));

        horizontalLayout->addWidget(restoreDefaultHomePageButton);


        horizontalLayout_2->addLayout(horizontalLayout);


        verticalLayout_2->addLayout(horizontalLayout_2);


        verticalLayout_3->addWidget(groupBox);

        verticalSpacer_2 = new QSpacerItem(20, 72, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_3->addItem(verticalSpacer_2);

        tabWidget->addTab(optionsTab, QString());

        vboxLayout->addWidget(tabWidget);

        hboxLayout2 = new QHBoxLayout();
        hboxLayout2->setSpacing(6);
        hboxLayout2->setMargin(0);
        hboxLayout2->setObjectName(QString::fromUtf8("hboxLayout2"));
        spacerItem1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout2->addItem(spacerItem1);

        buttonBox = new QDialogButtonBox(PreferencesDialogClass);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        hboxLayout2->addWidget(buttonBox);


        vboxLayout->addLayout(hboxLayout2);


        retranslateUi(PreferencesDialogClass);
        QObject::connect(comboBox, SIGNAL(currentIndexChanged(int)), stackedWidget_2, SLOT(setCurrentIndex(int)));

        tabWidget->setCurrentIndex(0);
        stackedWidget_2->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(PreferencesDialogClass);
    } // setupUi

    void retranslateUi(QDialog *PreferencesDialogClass)
    {
        PreferencesDialogClass->setWindowTitle(QApplication::translate("PreferencesDialogClass", "Preferences", 0, QApplication::UnicodeUTF8));
        fontLabel->setText(QApplication::translate("PreferencesDialogClass", "Font settings:", 0, QApplication::UnicodeUTF8));
        comboBox->clear();
        comboBox->insertItems(0, QStringList()
         << QApplication::translate("PreferencesDialogClass", "Browser", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("PreferencesDialogClass", "Application", 0, QApplication::UnicodeUTF8)
        );
        tabWidget->setTabText(tabWidget->indexOf(fontsTab), QApplication::translate("PreferencesDialogClass", "Fonts", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("PreferencesDialogClass", "Filter:", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("PreferencesDialogClass", "Attributes:", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem = attributeWidget->headerItem();
        ___qtreewidgetitem->setText(0, QApplication::translate("PreferencesDialogClass", "1", 0, QApplication::UnicodeUTF8));
        filterAddButton->setText(QApplication::translate("PreferencesDialogClass", "Add", 0, QApplication::UnicodeUTF8));
        filterRemoveButton->setText(QApplication::translate("PreferencesDialogClass", "Remove", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(filtersTab), QApplication::translate("PreferencesDialogClass", "Filters", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("PreferencesDialogClass", "Registered Documentation:", 0, QApplication::UnicodeUTF8));
        docAddButton->setText(QApplication::translate("PreferencesDialogClass", "Add...", 0, QApplication::UnicodeUTF8));
        docRemoveButton->setText(QApplication::translate("PreferencesDialogClass", "Remove", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(docsTab), QApplication::translate("PreferencesDialogClass", "Documentation", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("PreferencesDialogClass", "Homepage", 0, QApplication::UnicodeUTF8));
        currentPageButton->setText(QApplication::translate("PreferencesDialogClass", "Current Page", 0, QApplication::UnicodeUTF8));
        restoreDefaultHomePageButton->setText(QApplication::translate("PreferencesDialogClass", "Restore to default", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(optionsTab), QApplication::translate("PreferencesDialogClass", "Options", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(PreferencesDialogClass);
    } // retranslateUi

};

namespace Ui {
    class PreferencesDialogClass: public Ui_PreferencesDialogClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PREFERENCESDIALOG_H
