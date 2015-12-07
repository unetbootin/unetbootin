/********************************************************************************
** Form generated from reading ui file 'qtresourceeditordialog.ui'
**
** Created: Thu Mar 5 20:40:10 2009
**      by: Qt User Interface Compiler version 4.5.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_QTRESOURCEEDITORDIALOG_H
#define UI_QTRESOURCEEDITORDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QListWidget>
#include <QtGui/QSpacerItem>
#include <QtGui/QSplitter>
#include <QtGui/QToolButton>
#include <QtGui/QTreeView>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QtResourceEditorDialog
{
public:
    QVBoxLayout *verticalLayout;
    QSplitter *splitter;
    QWidget *qrcLayoutWidget;
    QGridLayout *qrcLayout;
    QListWidget *qrcFileList;
    QToolButton *newQrcButton;
    QToolButton *removeQrcButton;
    QSpacerItem *spacerItem;
    QToolButton *importQrcButton;
    QWidget *resourceLayoutWidget;
    QGridLayout *resourceLayout;
    QTreeView *resourceTreeView;
    QToolButton *newResourceButton;
    QToolButton *addResourceButton;
    QToolButton *removeResourceButton;
    QSpacerItem *horizontalSpacer;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *QtResourceEditorDialog)
    {
        if (QtResourceEditorDialog->objectName().isEmpty())
            QtResourceEditorDialog->setObjectName(QString::fromUtf8("QtResourceEditorDialog"));
        QtResourceEditorDialog->resize(469, 317);
        verticalLayout = new QVBoxLayout(QtResourceEditorDialog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        splitter = new QSplitter(QtResourceEditorDialog);
        splitter->setObjectName(QString::fromUtf8("splitter"));
        splitter->setOrientation(Qt::Horizontal);
        splitter->setChildrenCollapsible(false);
        qrcLayoutWidget = new QWidget(splitter);
        qrcLayoutWidget->setObjectName(QString::fromUtf8("qrcLayoutWidget"));
        qrcLayout = new QGridLayout(qrcLayoutWidget);
        qrcLayout->setObjectName(QString::fromUtf8("qrcLayout"));
        qrcLayout->setContentsMargins(0, 0, 0, 0);
        qrcFileList = new QListWidget(qrcLayoutWidget);
        qrcFileList->setObjectName(QString::fromUtf8("qrcFileList"));
        QSizePolicy sizePolicy(QSizePolicy::Ignored, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(qrcFileList->sizePolicy().hasHeightForWidth());
        qrcFileList->setSizePolicy(sizePolicy);

        qrcLayout->addWidget(qrcFileList, 0, 0, 1, 4);

        newQrcButton = new QToolButton(qrcLayoutWidget);
        newQrcButton->setObjectName(QString::fromUtf8("newQrcButton"));

        qrcLayout->addWidget(newQrcButton, 1, 0, 1, 1);

        removeQrcButton = new QToolButton(qrcLayoutWidget);
        removeQrcButton->setObjectName(QString::fromUtf8("removeQrcButton"));

        qrcLayout->addWidget(removeQrcButton, 1, 2, 1, 1);

        spacerItem = new QSpacerItem(21, 20, QSizePolicy::Ignored, QSizePolicy::Minimum);

        qrcLayout->addItem(spacerItem, 1, 3, 1, 1);

        importQrcButton = new QToolButton(qrcLayoutWidget);
        importQrcButton->setObjectName(QString::fromUtf8("importQrcButton"));

        qrcLayout->addWidget(importQrcButton, 1, 1, 1, 1);

        splitter->addWidget(qrcLayoutWidget);
        resourceLayoutWidget = new QWidget(splitter);
        resourceLayoutWidget->setObjectName(QString::fromUtf8("resourceLayoutWidget"));
        resourceLayout = new QGridLayout(resourceLayoutWidget);
        resourceLayout->setObjectName(QString::fromUtf8("resourceLayout"));
        resourceLayout->setContentsMargins(0, 0, 0, 0);
        resourceTreeView = new QTreeView(resourceLayoutWidget);
        resourceTreeView->setObjectName(QString::fromUtf8("resourceTreeView"));

        resourceLayout->addWidget(resourceTreeView, 0, 0, 1, 4);

        newResourceButton = new QToolButton(resourceLayoutWidget);
        newResourceButton->setObjectName(QString::fromUtf8("newResourceButton"));

        resourceLayout->addWidget(newResourceButton, 1, 0, 1, 1);

        addResourceButton = new QToolButton(resourceLayoutWidget);
        addResourceButton->setObjectName(QString::fromUtf8("addResourceButton"));

        resourceLayout->addWidget(addResourceButton, 1, 1, 1, 1);

        removeResourceButton = new QToolButton(resourceLayoutWidget);
        removeResourceButton->setObjectName(QString::fromUtf8("removeResourceButton"));

        resourceLayout->addWidget(removeResourceButton, 1, 2, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        resourceLayout->addItem(horizontalSpacer, 1, 3, 1, 1);

        splitter->addWidget(resourceLayoutWidget);

        verticalLayout->addWidget(splitter);

        buttonBox = new QDialogButtonBox(QtResourceEditorDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(QtResourceEditorDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), QtResourceEditorDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), QtResourceEditorDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(QtResourceEditorDialog);
    } // setupUi

    void retranslateUi(QDialog *QtResourceEditorDialog)
    {
        QtResourceEditorDialog->setWindowTitle(QApplication::translate("QtResourceEditorDialog", "Dialog", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        newQrcButton->setToolTip(QApplication::translate("QtResourceEditorDialog", "New File", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        newQrcButton->setText(QApplication::translate("QtResourceEditorDialog", "N", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        removeQrcButton->setToolTip(QApplication::translate("QtResourceEditorDialog", "Remove File", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        removeQrcButton->setText(QApplication::translate("QtResourceEditorDialog", "R", 0, QApplication::UnicodeUTF8));
        importQrcButton->setText(QApplication::translate("QtResourceEditorDialog", "I", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        newResourceButton->setToolTip(QApplication::translate("QtResourceEditorDialog", "New Resource", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        newResourceButton->setText(QApplication::translate("QtResourceEditorDialog", "N", 0, QApplication::UnicodeUTF8));
        addResourceButton->setText(QApplication::translate("QtResourceEditorDialog", "A", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        removeResourceButton->setToolTip(QApplication::translate("QtResourceEditorDialog", "Remove Resource or File", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        removeResourceButton->setText(QApplication::translate("QtResourceEditorDialog", "R", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(QtResourceEditorDialog);
    } // retranslateUi

};

namespace Ui {
    class QtResourceEditorDialog: public Ui_QtResourceEditorDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QTRESOURCEEDITORDIALOG_H
