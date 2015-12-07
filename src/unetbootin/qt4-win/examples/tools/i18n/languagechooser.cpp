/****************************************************************************
**
** Copyright (C) 2005-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License versions 2.0 or 3.0 as published by the Free Software
** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file.  Alternatively you may (at
** your option) use any later version of the GNU General Public
** License if such license has been publicly approved by Trolltech ASA
** (or its successors, if any) and the KDE Free Qt Foundation. In
** addition, as a special exception, Trolltech gives you certain
** additional rights. These rights are described in the Trolltech GPL
** Exception version 1.2, which can be found at
** http://www.trolltech.com/products/qt/gplexception/ and in the file
** GPL_EXCEPTION.txt in this package.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/. If
** you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** In addition, as a special exception, Trolltech, as the sole
** copyright holder for Qt Designer, grants users of the Qt/Eclipse
** Integration plug-in the right for the Qt/Eclipse Integration to
** link to functionality provided by Qt Designer and its related
** libraries.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not expressly
** granted herein.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include <QtGui>

#include "languagechooser.h"
#include "mainwindow.h"

#ifdef Q_WS_MAC
QT_BEGIN_NAMESPACE
extern void qt_mac_set_menubar_merge(bool merge);
QT_END_NAMESPACE
#endif

LanguageChooser::LanguageChooser(QWidget *parent)
    : QDialog(parent, Qt::WindowStaysOnTopHint)
{
    groupBox = new QGroupBox("Languages");

    QGridLayout *groupBoxLayout = new QGridLayout;

    QStringList qmFiles = findQmFiles();
    for (int i = 0; i < qmFiles.size(); ++i) {
        QCheckBox *checkBox = new QCheckBox(languageName(qmFiles[i]));
        qmFileForCheckBoxMap.insert(checkBox, qmFiles[i]);
        connect(checkBox, SIGNAL(toggled(bool)), this, SLOT(checkBoxToggled()));
        groupBoxLayout->addWidget(checkBox, i / 2, i % 2);
    }
    groupBox->setLayout(groupBoxLayout);

    buttonBox = new QDialogButtonBox;

    showAllButton = buttonBox->addButton("Show All",
                                         QDialogButtonBox::ActionRole);
    hideAllButton = buttonBox->addButton("Hide All",
                                         QDialogButtonBox::ActionRole);

    connect(showAllButton, SIGNAL(clicked()), this, SLOT(showAll()));
    connect(hideAllButton, SIGNAL(clicked()), this, SLOT(hideAll()));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(groupBox);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);

#ifdef Q_WS_MAC
    qt_mac_set_menubar_merge(false);
#endif

    setWindowTitle("I18N");
}

bool LanguageChooser::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::Close) {
        MainWindow *window = qobject_cast<MainWindow *>(object);
        if (window) {
            QCheckBox *checkBox = mainWindowForCheckBoxMap.key(window);
            if (checkBox)
                checkBox->setChecked(false);
        }
    }
    return QWidget::eventFilter(object, event);
}

void LanguageChooser::closeEvent(QCloseEvent * /* event */)
{
    qApp->quit();
}

void LanguageChooser::checkBoxToggled()
{
    QCheckBox *checkBox = qobject_cast<QCheckBox *>(sender());
    MainWindow *window = mainWindowForCheckBoxMap[checkBox];
    if (!window) {
        QTranslator translator;
        translator.load(qmFileForCheckBoxMap[checkBox]);
        qApp->installTranslator(&translator);

        window = new MainWindow;
        window->setPalette(colorForLanguage(checkBox->text()));

        window->installEventFilter(this);
        mainWindowForCheckBoxMap.insert(checkBox, window);
    }
    window->setVisible(checkBox->isChecked());
}

void LanguageChooser::showAll()
{
    foreach (QCheckBox *checkBox, qmFileForCheckBoxMap.keys())
        checkBox->setChecked(true);
}

void LanguageChooser::hideAll()
{
    foreach (QCheckBox *checkBox, qmFileForCheckBoxMap.keys())
        checkBox->setChecked(false);
}

QStringList LanguageChooser::findQmFiles()
{
    QDir dir(":/translations");
    QStringList fileNames = dir.entryList(QStringList("*.qm"), QDir::Files,
                                          QDir::Name);
    QMutableStringListIterator i(fileNames);
    while (i.hasNext()) {
        i.next();
        i.setValue(dir.filePath(i.value()));
    }
    return fileNames;
}

QString LanguageChooser::languageName(const QString &qmFile)
{
    QTranslator translator;
    translator.load(qmFile);

    return translator.translate("MainWindow", "English");
}

QColor LanguageChooser::colorForLanguage(const QString &language)
{
    uint hashValue = qHash(language);
    int red = 156 + (hashValue & 0x3F);
    int green = 156 + ((hashValue >> 6) & 0x3F);
    int blue = 156 + ((hashValue >> 12) & 0x3F);
    return QColor(red, green, blue);
}
