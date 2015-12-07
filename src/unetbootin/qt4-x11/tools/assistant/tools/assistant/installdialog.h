/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt Assistant of the Qt Toolkit.
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
