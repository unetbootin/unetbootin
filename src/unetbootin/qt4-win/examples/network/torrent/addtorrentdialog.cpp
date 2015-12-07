/****************************************************************************
**
** Copyright (C) 2004-2008 Trolltech ASA. All rights reserved.
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

#include "addtorrentdialog.h"
#include "metainfo.h"

#include <QFile>
#include <QFileDialog>
#include <QLineEdit>
#include <QMetaObject>

static QString stringNumber(qint64 number)
{
    QString tmp;
    if (number > (1024 * 1024 * 1024))
        tmp.sprintf("%.2fGB", number / (1024.0 * 1024.0 * 1024.0));
    else if (number > (1024 * 1024))
        tmp.sprintf("%.2fMB", number / (1024.0 * 1024.0));
    else if (number > (1024))
        tmp.sprintf("%.2fKB", number / (1024.0));
    else
        tmp.sprintf("%d bytes", int(number));
    return tmp;
}

AddTorrentDialog::AddTorrentDialog(QWidget *parent)
  : QDialog(parent, Qt::Sheet)
{
    ui.setupUi(this);

    connect(ui.browseTorrents, SIGNAL(clicked()),
            this, SLOT(selectTorrent()));
    connect(ui.browseDestination, SIGNAL(clicked()),
            this, SLOT(selectDestination()));
    connect(ui.torrentFile, SIGNAL(textChanged(const QString &)),
            this, SLOT(setTorrent(const QString &)));

    ui.destinationFolder->setText(destinationDirectory = QDir::current().path());
    ui.torrentFile->setFocus();
}

void AddTorrentDialog::selectTorrent()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Choose a torrent file"),
                                                    lastDirectory,
                                                    tr("Torrents (*.torrent);; All files (*.*)"));
    if (fileName.isEmpty())
        return;
    lastDirectory = QFileInfo(fileName).absolutePath();
    setTorrent(fileName);
}

void AddTorrentDialog::selectDestination()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Choose a destination directory"),
                                                    lastDestinationDirectory);
    if (dir.isEmpty())
        return;
    lastDestinationDirectory = destinationDirectory = dir;
    ui.destinationFolder->setText(destinationDirectory);
    enableOkButton();
}

void AddTorrentDialog::enableOkButton()
{
    ui.okButton->setEnabled(!ui.destinationFolder->text().isEmpty()
                            && !ui.torrentFile->text().isEmpty());
}

void AddTorrentDialog::setTorrent(const QString &torrentFile)
{
    if (torrentFile.isEmpty()) {
        enableOkButton();
        return;
    }

    ui.torrentFile->setText(torrentFile);
    if (!torrentFile.isEmpty())
        lastDirectory = QFileInfo(torrentFile).absolutePath();

    if (lastDestinationDirectory.isEmpty())
        lastDestinationDirectory = lastDirectory;
    
    MetaInfo metaInfo;
    QFile torrent(torrentFile);
    if (!torrent.open(QFile::ReadOnly) || !metaInfo.parse(torrent.readAll())) {
        enableOkButton();
        return;
    }
    
    ui.torrentFile->setText(torrentFile);
    ui.announceUrl->setText(metaInfo.announceUrl());
    if (metaInfo.comment().isEmpty())
        ui.commentLabel->setText("<unknown>");
    else
        ui.commentLabel->setText(metaInfo.comment());
    if (metaInfo.createdBy().isEmpty())
        ui.creatorLabel->setText("<unknown>");
    else
        ui.creatorLabel->setText(metaInfo.createdBy());
    ui.sizeLabel->setText(stringNumber(metaInfo.totalSize()));
    if (metaInfo.fileForm() == MetaInfo::SingleFileForm) {
        ui.torrentContents->setHtml(metaInfo.singleFile().name);
    } else {
        QString html;
        foreach (MetaInfoMultiFile file, metaInfo.multiFiles()) {
            QString name = metaInfo.name();
            if (!name.isEmpty()) {
                html += name;
                if (!name.endsWith('/'))
                    html += '/';
            }
            html += file.path + "<br>";
        }
        ui.torrentContents->setHtml(html);
    }

    QFileInfo info(torrentFile);
    ui.destinationFolder->setText(info.absolutePath());

    enableOkButton();
}

QString AddTorrentDialog::torrentFileName() const
{
    return ui.torrentFile->text();
}

QString AddTorrentDialog::destinationFolder() const
{
    return ui.destinationFolder->text();
}
