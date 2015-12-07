/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt Assistant of the Qt Toolkit.
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

#ifndef INSTALLDIALOG_H
#define INSTALLDIALOG_H

#include <QtCore/QQueue>
#include <QtGui/QDialog>
#include <QtNetwork/QHttpResponseHeader>
#include "ui_installdialog.h"

QT_BEGIN_NAMESPACE

class QHttp;
class QBuffer;
class QFile;
class QHelpEngineCore;

class InstallDialog : public QDialog
{
    Q_OBJECT

public:
    InstallDialog(QHelpEngineCore *helpEngine, QWidget *parent = 0,
        const QString &host = QString(), int port = -1);
    ~InstallDialog();

    QStringList installedDocumentations() const;

private slots:
    void init();
    void cancelDownload();
    void install();
    void httpRequestFinished(int requestId, bool error);
    void readResponseHeader(const QHttpResponseHeader &responseHeader);
    void updateDataReadProgress(int bytesRead, int totalBytes);
    void updateInstallButton();
    void browseDirectories();

private:
    void downloadNextFile();
    void updateDocItemList();
    void installFile(const QString &fileName);

    Ui::InstallDialog m_ui;
    QHelpEngineCore *m_helpEngine;
    QHttp *m_http;
    QBuffer *m_buffer;
    QFile *m_file;
    bool m_httpAborted;
    int m_docInfoId;
    int m_docId;
    QQueue<QListWidgetItem*> m_itemsToInstall;
    QString m_currentCheckSum;
    QString m_windowTitle;
    QStringList m_installedDocumentations;
    QString m_host;
    int m_port;
};

QT_END_NAMESPACE

#endif // INSTALLDIALOG_H
