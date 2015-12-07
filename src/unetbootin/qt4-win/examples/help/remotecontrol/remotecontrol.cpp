/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
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

#include <QtCore/QDir>
#include <QtCore/QProcess>
#include <QtCore/QTextStream>
#include <QtCore/QLibraryInfo>

#include <QtGui/QMessageBox>

#include "remotecontrol.h"

RemoteControl::RemoteControl(QWidget *parent, Qt::WFlags flags)
        : QMainWindow(parent, flags)
{
    ui.setupUi(this);
    connect(ui.indexLineEdit, SIGNAL(returnPressed()),
        this, SLOT(on_indexButton_clicked()));
    connect(ui.identifierLineEdit, SIGNAL(returnPressed()),
        this, SLOT(on_identifierButton_clicked()));
    connect(ui.urlLineEdit, SIGNAL(returnPressed()),
        this, SLOT(on_urlButton_clicked()));

    QString rc;
    QTextStream(&rc) << QLatin1String("qthelp://com.trolltech.qt.")
                     << (QT_VERSION >> 16) << ((QT_VERSION >> 8) & 0xFF)
                     << (QT_VERSION & 0xFF)
                     << QLatin1String("/qdoc/index.html");

    ui.startUrlLineEdit->setText(rc);

    process = new QProcess(this);
    connect(process, SIGNAL(finished(int, QProcess::ExitStatus)),
        this, SLOT(helpViewerClosed()));
}

RemoteControl::~RemoteControl()
{
    if (process->state() == QProcess::Running) {
        process->terminate();
        process->waitForFinished(3000);
    }
}

void RemoteControl::on_actionQuit_triggered()
{
    close();
}

void RemoteControl::on_launchButton_clicked()
{
    if (process->state() == QProcess::Running)
        return;

    QString app = QLibraryInfo::location(QLibraryInfo::BinariesPath) + QDir::separator();
#if !defined(Q_OS_MAC)
    app += QLatin1String("assistant");
#else
    app += QLatin1String("Assistant.app/Contents/MacOS/Assistant");
#endif

    ui.contentsCheckBox->setChecked(true);
    ui.indexCheckBox->setChecked(true);
    ui.bookmarksCheckBox->setChecked(true);

    QStringList args;
    args << QLatin1String("-enableRemoteControl");
    process->start(app, args);
    if (!process->waitForStarted()) {
        QMessageBox::critical(this, tr("Remote Control"),
            tr("Could not start Qt Assistant from %1.").arg(app));
        return;
    }

    if (!ui.startUrlLineEdit->text().isEmpty())
        sendCommand(QLatin1String("SetSource ")
            + ui.startUrlLineEdit->text());
        
    ui.launchButton->setEnabled(false);
    ui.startUrlLineEdit->setEnabled(false);
    ui.actionGroupBox->setEnabled(true);
}

void RemoteControl::sendCommand(const QString &cmd)
{
    if (process->state() != QProcess::Running)
        return;
    QTextStream str(process);
    str << cmd << QLatin1Char('\0') << endl;
}

void RemoteControl::on_indexButton_clicked()
{
    sendCommand(QLatin1String("ActivateKeyword ")
        + ui.indexLineEdit->text());
}

void RemoteControl::on_identifierButton_clicked()
{
    sendCommand(QLatin1String("ActivateIdentifier ")
        + ui.identifierLineEdit->text());
}

void RemoteControl::on_urlButton_clicked()
{
    sendCommand(QLatin1String("SetSource ")
        + ui.urlLineEdit->text());
}

void RemoteControl::on_syncContentsButton_clicked()
{
    sendCommand(QLatin1String("SyncContents"));
}

void RemoteControl::on_contentsCheckBox_toggled(bool checked)
{
    sendCommand(checked ?
        QLatin1String("Show Contents") : QLatin1String("Hide Contents"));
}

void RemoteControl::on_indexCheckBox_toggled(bool checked)
{
    sendCommand(checked ?
        QLatin1String("Show Index") : QLatin1String("Hide Index"));
}

void RemoteControl::on_bookmarksCheckBox_toggled(bool checked)
{
    sendCommand(checked ?
        QLatin1String("Show Bookmarks") : QLatin1String("Hide Bookmarks"));
}

void RemoteControl::helpViewerClosed()
{
    ui.launchButton->setEnabled(true);
    ui.startUrlLineEdit->setEnabled(true);
    ui.actionGroupBox->setEnabled(false);
}
