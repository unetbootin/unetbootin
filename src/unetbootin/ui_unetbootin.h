/********************************************************************************
** Form generated from reading ui file 'unetbootin.ui'
**
** Created: Sun Jun 22 20:47:46 2008
**      by: Qt User Interface Compiler version 4.4.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_UNETBOOTIN_H
#define UI_UNETBOOTIN_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QTextBrowser>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_unetbootinui
{
public:
    QWidget *firstlayer;
    QRadioButton *radioDistro;
    QRadioButton *radioFloppy;
    QRadioButton *radioManual;
    QComboBox *distroselect;
    QPushButton *okbutton;
    QPushButton *cancelbutton;
    QLabel *labeldrive;
    QComboBox *driveselect;
    QLabel *labeltype;
    QComboBox *typeselect;
    QComboBox *dverselect;
    QTextBrowser *intromessage;
    QWidget *optionslayer;
    QPushButton *FloppyFileSelector;
    QComboBox *diskimagetypeselect;
    QLineEdit *FloppyPath;
    QLineEdit *KernelPath;
    QPushButton *KernelFileSelector;
    QLabel *labelkernel;
    QLabel *labelinitrd;
    QPushButton *InitrdFileSelector;
    QLineEdit *InitrdPath;
    QPushButton *CfgFileSelector;
    QLineEdit *OptionEnter;
    QLabel *labeloption;
    QCheckBox *showalldrivescheckbox;
    QCheckBox *formatdrivecheckbox;
    QWidget *secondlayer;
    QWidget *progresslayer;
    QProgressBar *tprogress;
    QLabel *pdesc5;
    QLabel *pdesc4;
    QLabel *pdesc3;
    QLabel *pdesc2;
    QLabel *pdesc1;
    QLabel *sdesc5;
    QLabel *sdesc4;
    QLabel *sdesc3;
    QLabel *sdesc2;
    QLabel *sdesc1;
    QWidget *rebootlayer;
    QPushButton *fexitbutton;
    QPushButton *frebootbutton;
    QTextBrowser *rebootmsgtext;

    void setupUi(QWidget *unetbootinui)
    {
    if (unetbootinui->objectName().isEmpty())
        unetbootinui->setObjectName(QString::fromUtf8("unetbootinui"));
    unetbootinui->resize(524, 360);
    unetbootinui->setMinimumSize(QSize(524, 360));
    unetbootinui->setMaximumSize(QSize(524, 360));
    firstlayer = new QWidget(unetbootinui);
    firstlayer->setObjectName(QString::fromUtf8("firstlayer"));
    firstlayer->setEnabled(false);
    firstlayer->setGeometry(QRect(0, 0, 524, 360));
    firstlayer->setAutoFillBackground(true);
    radioDistro = new QRadioButton(firstlayer);
    radioDistro->setObjectName(QString::fromUtf8("radioDistro"));
    radioDistro->setGeometry(QRect(10, 20, 111, 25));
    radioDistro->setChecked(true);
    radioFloppy = new QRadioButton(firstlayer);
    radioFloppy->setObjectName(QString::fromUtf8("radioFloppy"));
    radioFloppy->setGeometry(QRect(10, 180, 81, 25));
    radioManual = new QRadioButton(firstlayer);
    radioManual->setObjectName(QString::fromUtf8("radioManual"));
    radioManual->setGeometry(QRect(10, 220, 81, 25));
    distroselect = new QComboBox(firstlayer);
    distroselect->setObjectName(QString::fromUtf8("distroselect"));
    distroselect->setGeometry(QRect(110, 20, 200, 25));
    okbutton = new QPushButton(firstlayer);
    okbutton->setObjectName(QString::fromUtf8("okbutton"));
    okbutton->setGeometry(QRect(343, 325, 80, 25));
    cancelbutton = new QPushButton(firstlayer);
    cancelbutton->setObjectName(QString::fromUtf8("cancelbutton"));
    cancelbutton->setGeometry(QRect(433, 325, 80, 25));
    labeldrive = new QLabel(firstlayer);
    labeldrive->setObjectName(QString::fromUtf8("labeldrive"));
    labeldrive->setGeometry(QRect(150, 325, 41, 25));
    driveselect = new QComboBox(firstlayer);
    driveselect->setObjectName(QString::fromUtf8("driveselect"));
    driveselect->setGeometry(QRect(190, 325, 111, 25));
    labeltype = new QLabel(firstlayer);
    labeltype->setObjectName(QString::fromUtf8("labeltype"));
    labeltype->setGeometry(QRect(10, 325, 32, 25));
    typeselect = new QComboBox(firstlayer);
    typeselect->setObjectName(QString::fromUtf8("typeselect"));
    typeselect->setGeometry(QRect(50, 325, 91, 25));
    dverselect = new QComboBox(firstlayer);
    dverselect->setObjectName(QString::fromUtf8("dverselect"));
    dverselect->setGeometry(QRect(313, 20, 200, 25));
    intromessage = new QTextBrowser(firstlayer);
    intromessage->setObjectName(QString::fromUtf8("intromessage"));
    intromessage->setGeometry(QRect(9, 60, 503, 101));
    intromessage->setStyleSheet(QString::fromUtf8("background-color: transparent;"));
    intromessage->setFrameShape(QFrame::NoFrame);
    intromessage->setFrameShadow(QFrame::Plain);
    intromessage->setOpenExternalLinks(true);
    optionslayer = new QWidget(firstlayer);
    optionslayer->setObjectName(QString::fromUtf8("optionslayer"));
    optionslayer->setEnabled(false);
    optionslayer->setGeometry(QRect(100, 170, 521, 121));
    FloppyFileSelector = new QPushButton(optionslayer);
    FloppyFileSelector->setObjectName(QString::fromUtf8("FloppyFileSelector"));
    FloppyFileSelector->setGeometry(QRect(388, 10, 25, 25));
    FloppyFileSelector->setAcceptDrops(false);
    diskimagetypeselect = new QComboBox(optionslayer);
    diskimagetypeselect->setObjectName(QString::fromUtf8("diskimagetypeselect"));
    diskimagetypeselect->setGeometry(QRect(0, 10, 51, 25));
    FloppyPath = new QLineEdit(optionslayer);
    FloppyPath->setObjectName(QString::fromUtf8("FloppyPath"));
    FloppyPath->setGeometry(QRect(60, 10, 321, 25));
    KernelPath = new QLineEdit(optionslayer);
    KernelPath->setObjectName(QString::fromUtf8("KernelPath"));
    KernelPath->setGeometry(QRect(60, 50, 111, 25));
    KernelFileSelector = new QPushButton(optionslayer);
    KernelFileSelector->setObjectName(QString::fromUtf8("KernelFileSelector"));
    KernelFileSelector->setGeometry(QRect(180, 50, 25, 25));
    KernelFileSelector->setAcceptDrops(false);
    labelkernel = new QLabel(optionslayer);
    labelkernel->setObjectName(QString::fromUtf8("labelkernel"));
    labelkernel->setGeometry(QRect(0, 50, 51, 25));
    labelinitrd = new QLabel(optionslayer);
    labelinitrd->setObjectName(QString::fromUtf8("labelinitrd"));
    labelinitrd->setGeometry(QRect(230, 50, 41, 25));
    InitrdFileSelector = new QPushButton(optionslayer);
    InitrdFileSelector->setObjectName(QString::fromUtf8("InitrdFileSelector"));
    InitrdFileSelector->setGeometry(QRect(388, 50, 25, 25));
    InitrdFileSelector->setAcceptDrops(false);
    InitrdPath = new QLineEdit(optionslayer);
    InitrdPath->setObjectName(QString::fromUtf8("InitrdPath"));
    InitrdPath->setGeometry(QRect(270, 50, 111, 25));
    CfgFileSelector = new QPushButton(optionslayer);
    CfgFileSelector->setObjectName(QString::fromUtf8("CfgFileSelector"));
    CfgFileSelector->setGeometry(QRect(388, 90, 25, 25));
    CfgFileSelector->setAcceptDrops(false);
    OptionEnter = new QLineEdit(optionslayer);
    OptionEnter->setObjectName(QString::fromUtf8("OptionEnter"));
    OptionEnter->setGeometry(QRect(60, 90, 321, 25));
    labeloption = new QLabel(optionslayer);
    labeloption->setObjectName(QString::fromUtf8("labeloption"));
    labeloption->setGeometry(QRect(0, 90, 61, 25));
    showalldrivescheckbox = new QCheckBox(firstlayer);
    showalldrivescheckbox->setObjectName(QString::fromUtf8("showalldrivescheckbox"));
    showalldrivescheckbox->setGeometry(QRect(10, 295, 251, 25));
    formatdrivecheckbox = new QCheckBox(firstlayer);
    formatdrivecheckbox->setObjectName(QString::fromUtf8("formatdrivecheckbox"));
    formatdrivecheckbox->setGeometry(QRect(260, 295, 251, 25));
    secondlayer = new QWidget(unetbootinui);
    secondlayer->setObjectName(QString::fromUtf8("secondlayer"));
    secondlayer->setEnabled(false);
    secondlayer->setGeometry(QRect(0, 0, 524, 360));
    progresslayer = new QWidget(secondlayer);
    progresslayer->setObjectName(QString::fromUtf8("progresslayer"));
    progresslayer->setEnabled(false);
    progresslayer->setGeometry(QRect(0, 150, 524, 210));
    tprogress = new QProgressBar(progresslayer);
    tprogress->setObjectName(QString::fromUtf8("tprogress"));
    tprogress->setGeometry(QRect(10, 170, 501, 23));
    tprogress->setValue(24);
    pdesc5 = new QLabel(progresslayer);
    pdesc5->setObjectName(QString::fromUtf8("pdesc5"));
    pdesc5->setGeometry(QRect(10, 10, 501, 20));
    pdesc5->setOpenExternalLinks(true);
    pdesc4 = new QLabel(progresslayer);
    pdesc4->setObjectName(QString::fromUtf8("pdesc4"));
    pdesc4->setGeometry(QRect(10, 40, 501, 20));
    pdesc4->setOpenExternalLinks(true);
    pdesc3 = new QLabel(progresslayer);
    pdesc3->setObjectName(QString::fromUtf8("pdesc3"));
    pdesc3->setGeometry(QRect(10, 70, 501, 20));
    pdesc3->setOpenExternalLinks(true);
    pdesc2 = new QLabel(progresslayer);
    pdesc2->setObjectName(QString::fromUtf8("pdesc2"));
    pdesc2->setGeometry(QRect(10, 100, 501, 20));
    pdesc2->setOpenExternalLinks(true);
    pdesc1 = new QLabel(progresslayer);
    pdesc1->setObjectName(QString::fromUtf8("pdesc1"));
    pdesc1->setGeometry(QRect(10, 130, 501, 20));
    pdesc1->setOpenExternalLinks(true);
    sdesc5 = new QLabel(secondlayer);
    sdesc5->setObjectName(QString::fromUtf8("sdesc5"));
    sdesc5->setGeometry(QRect(10, 130, 501, 20));
    sdesc4 = new QLabel(secondlayer);
    sdesc4->setObjectName(QString::fromUtf8("sdesc4"));
    sdesc4->setGeometry(QRect(10, 100, 501, 20));
    sdesc3 = new QLabel(secondlayer);
    sdesc3->setObjectName(QString::fromUtf8("sdesc3"));
    sdesc3->setGeometry(QRect(10, 70, 501, 20));
    sdesc2 = new QLabel(secondlayer);
    sdesc2->setObjectName(QString::fromUtf8("sdesc2"));
    sdesc2->setGeometry(QRect(10, 40, 501, 20));
    sdesc1 = new QLabel(secondlayer);
    sdesc1->setObjectName(QString::fromUtf8("sdesc1"));
    sdesc1->setGeometry(QRect(10, 10, 501, 20));
    rebootlayer = new QWidget(secondlayer);
    rebootlayer->setObjectName(QString::fromUtf8("rebootlayer"));
    rebootlayer->setEnabled(false);
    rebootlayer->setGeometry(QRect(0, 150, 524, 210));
    fexitbutton = new QPushButton(rebootlayer);
    fexitbutton->setObjectName(QString::fromUtf8("fexitbutton"));
    fexitbutton->setGeometry(QRect(420, 170, 100, 29));
    frebootbutton = new QPushButton(rebootlayer);
    frebootbutton->setObjectName(QString::fromUtf8("frebootbutton"));
    frebootbutton->setGeometry(QRect(310, 170, 100, 29));
    rebootmsgtext = new QTextBrowser(rebootlayer);
    rebootmsgtext->setObjectName(QString::fromUtf8("rebootmsgtext"));
    rebootmsgtext->setGeometry(QRect(20, 10, 501, 151));
    rebootmsgtext->setStyleSheet(QString::fromUtf8("background-color: transparent;"));
    rebootmsgtext->setFrameShape(QFrame::NoFrame);
    rebootmsgtext->setFrameShadow(QFrame::Plain);
    rebootmsgtext->setLineWidth(0);
    rebootmsgtext->setOpenExternalLinks(true);
    secondlayer->raise();
    firstlayer->raise();

    retranslateUi(unetbootinui);

    distroselect->setCurrentIndex(-1);
    dverselect->setCurrentIndex(-1);


    QMetaObject::connectSlotsByName(unetbootinui);
    } // setupUi

    void retranslateUi(QWidget *unetbootinui)
    {
    unetbootinui->setWindowTitle(QApplication::translate("unetbootinui", "UNetbootin", 0, QApplication::UnicodeUTF8));

#ifndef QT_NO_TOOLTIP
    radioDistro->setToolTip(QApplication::translate("unetbootinui", "Select from a list of supported distributions", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP

    radioDistro->setText(QApplication::translate("unetbootinui", "Distribution", 0, QApplication::UnicodeUTF8));

#ifndef QT_NO_TOOLTIP
    radioFloppy->setToolTip(QApplication::translate("unetbootinui", "Specify a disk image file to load", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP

    radioFloppy->setText(QApplication::translate("unetbootinui", "Diskimage", 0, QApplication::UnicodeUTF8));

#ifndef QT_NO_TOOLTIP
    radioManual->setToolTip(QApplication::translate("unetbootinui", "Manually specify a kernel and initrd to load", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP

    radioManual->setText(QApplication::translate("unetbootinui", "Custom", 0, QApplication::UnicodeUTF8));

#ifndef QT_NO_TOOLTIP
    distroselect->setToolTip(QApplication::translate("unetbootinui", "Select from a list of supported distributions", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP

    okbutton->setText(QApplication::translate("unetbootinui", "OK", 0, QApplication::UnicodeUTF8));
    cancelbutton->setText(QApplication::translate("unetbootinui", "Cancel", 0, QApplication::UnicodeUTF8));

#ifndef QT_NO_TOOLTIP
    labeldrive->setToolTip(QApplication::translate("unetbootinui", "Select the target drive to install to", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP

    labeldrive->setText(QApplication::translate("unetbootinui", "Drive:", 0, QApplication::UnicodeUTF8));

#ifndef QT_NO_TOOLTIP
    driveselect->setToolTip(QApplication::translate("unetbootinui", "Select the target drive to install to", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP


#ifndef QT_NO_TOOLTIP
    labeltype->setToolTip(QApplication::translate("unetbootinui", "Select the installation target type", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP

    labeltype->setText(QApplication::translate("unetbootinui", "Type:", 0, QApplication::UnicodeUTF8));
    typeselect->insertItems(0, QStringList()
     << QApplication::translate("unetbootinui", "Hard Disk", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("unetbootinui", "USB Drive", 0, QApplication::UnicodeUTF8)
    );

#ifndef QT_NO_TOOLTIP
    typeselect->setToolTip(QApplication::translate("unetbootinui", "Select the installation target type", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP


#ifndef QT_NO_TOOLTIP
    dverselect->setToolTip(QApplication::translate("unetbootinui", "Select the distribution version", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP


#ifndef QT_NO_TOOLTIP
    FloppyFileSelector->setToolTip(QApplication::translate("unetbootinui", "Select disk image file", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP

    FloppyFileSelector->setText(QApplication::translate("unetbootinui", "...", 0, QApplication::UnicodeUTF8));
    diskimagetypeselect->insertItems(0, QStringList()
     << QApplication::translate("unetbootinui", "ISO", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("unetbootinui", "Floppy", 0, QApplication::UnicodeUTF8)
    );

#ifndef QT_NO_TOOLTIP
    diskimagetypeselect->setToolTip(QApplication::translate("unetbootinui", "Select the disk image type", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP


#ifndef QT_NO_TOOLTIP
    FloppyPath->setToolTip(QApplication::translate("unetbootinui", "Specify a floppy/hard disk image, or CD image (ISO) file to load", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP


#ifndef QT_NO_TOOLTIP
    KernelPath->setToolTip(QApplication::translate("unetbootinui", "Specify a kernel file to load", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP


#ifndef QT_NO_TOOLTIP
    KernelFileSelector->setToolTip(QApplication::translate("unetbootinui", "Select kernel file", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP

    KernelFileSelector->setText(QApplication::translate("unetbootinui", "...", 0, QApplication::UnicodeUTF8));

#ifndef QT_NO_TOOLTIP
    labelkernel->setToolTip(QApplication::translate("unetbootinui", "Specify a kernel file to load", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP

    labelkernel->setText(QApplication::translate("unetbootinui", "Kernel:", 0, QApplication::UnicodeUTF8));

#ifndef QT_NO_TOOLTIP
    labelinitrd->setToolTip(QApplication::translate("unetbootinui", "Specify an initrd file to load", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP

    labelinitrd->setText(QApplication::translate("unetbootinui", "Initrd:", 0, QApplication::UnicodeUTF8));

#ifndef QT_NO_TOOLTIP
    InitrdFileSelector->setToolTip(QApplication::translate("unetbootinui", "Select initrd file", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP

    InitrdFileSelector->setText(QApplication::translate("unetbootinui", "...", 0, QApplication::UnicodeUTF8));

#ifndef QT_NO_TOOLTIP
    InitrdPath->setToolTip(QApplication::translate("unetbootinui", "Specify an initrd file to load", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP


#ifndef QT_NO_TOOLTIP
    CfgFileSelector->setToolTip(QApplication::translate("unetbootinui", "Select syslinux.cfg or isolinux.cfg file", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP

    CfgFileSelector->setText(QApplication::translate("unetbootinui", "...", 0, QApplication::UnicodeUTF8));

#ifndef QT_NO_TOOLTIP
    OptionEnter->setToolTip(QApplication::translate("unetbootinui", "Specify parameters and options to pass to the kernel", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP


#ifndef QT_NO_TOOLTIP
    labeloption->setToolTip(QApplication::translate("unetbootinui", "Specify parameters and options to pass to the kernel", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP

    labeloption->setText(QApplication::translate("unetbootinui", "Options:", 0, QApplication::UnicodeUTF8));

#ifndef QT_NO_TOOLTIP
    showalldrivescheckbox->setToolTip(QApplication::translate("unetbootinui", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:10pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">This <span style=\" font-weight:600;\">potentially dangerous</span> option will show all drives in the drop-down box titled \"Drives\". Enable this only if you know what you're doing; if you use this option, the installation will likely fail, or can lead to system boot issues.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP

    showalldrivescheckbox->setText(QApplication::translate("unetbootinui", "Show All Drives (Use with Care)", 0, QApplication::UnicodeUTF8));

#ifndef QT_NO_TOOLTIP
    formatdrivecheckbox->setToolTip(QApplication::translate("unetbootinui", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:10pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">This <span style=\" font-weight:600;\">potentially dangerous</span> option will format the selected drive, wiping out all data on it. It shouldn't be necessary if you're using a standard USB drive.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP

    formatdrivecheckbox->setText(QApplication::translate("unetbootinui", "Format Drive (Wipes Out Data)", 0, QApplication::UnicodeUTF8));
    pdesc5->setText(QString());
    pdesc4->setText(QString());
    pdesc3->setText(QString());
    pdesc2->setText(QString());
    pdesc1->setText(QString());
    sdesc5->setText(QString());
    sdesc4->setText(QApplication::translate("unetbootinui", "4. Installation Complete, Reboot", 0, QApplication::UnicodeUTF8));
    sdesc3->setText(QApplication::translate("unetbootinui", "3. Installing Bootloader", 0, QApplication::UnicodeUTF8));
    sdesc2->setText(QApplication::translate("unetbootinui", "2. Extracting and Copying Files", 0, QApplication::UnicodeUTF8));
    sdesc1->setText(QApplication::translate("unetbootinui", "1. Downloading Files", 0, QApplication::UnicodeUTF8));
    fexitbutton->setText(QApplication::translate("unetbootinui", "Exit", 0, QApplication::UnicodeUTF8));
    frebootbutton->setText(QApplication::translate("unetbootinui", "Reboot Now", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(unetbootinui);
    } // retranslateUi

};

namespace Ui {
    class unetbootinui: public Ui_unetbootinui {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_UNETBOOTIN_H
