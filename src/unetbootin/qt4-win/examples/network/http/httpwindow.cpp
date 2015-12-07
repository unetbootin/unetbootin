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

#include <QtGui>
#include <QtNetwork>

#include "httpwindow.h"
#include "ui_authenticationdialog.h"

HttpWindow::HttpWindow(QWidget *parent)
    : QDialog(parent)
{
#ifndef QT_NO_OPENSSL
    urlLineEdit = new QLineEdit("https://");
#else
    urlLineEdit = new QLineEdit("http://");
#endif

    urlLabel = new QLabel(tr("&URL:"));
    urlLabel->setBuddy(urlLineEdit);
    statusLabel = new QLabel(tr("Please enter the URL of a file you want to "
                                "download."));

    downloadButton = new QPushButton(tr("Download"));
    downloadButton->setDefault(true);
    quitButton = new QPushButton(tr("Quit"));
    quitButton->setAutoDefault(false);

    buttonBox = new QDialogButtonBox;
    buttonBox->addButton(downloadButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(quitButton, QDialogButtonBox::RejectRole);

    progressDialog = new QProgressDialog(this);

    http = new QHttp(this);

    connect(urlLineEdit, SIGNAL(textChanged(const QString &)),
            this, SLOT(enableDownloadButton()));
    connect(http, SIGNAL(requestFinished(int, bool)),
            this, SLOT(httpRequestFinished(int, bool)));
    connect(http, SIGNAL(dataReadProgress(int, int)),
            this, SLOT(updateDataReadProgress(int, int)));
    connect(http, SIGNAL(responseHeaderReceived(const QHttpResponseHeader &)),
            this, SLOT(readResponseHeader(const QHttpResponseHeader &)));
    connect(http, SIGNAL(authenticationRequired(const QString &, quint16, QAuthenticator *)),
            this, SLOT(slotAuthenticationRequired(const QString &, quint16, QAuthenticator *)));
#ifndef QT_NO_OPENSSL
    connect(http, SIGNAL(sslErrors(const QList<QSslError> &)),
            this, SLOT(sslErrors(const QList<QSslError> &)));
#endif
    connect(progressDialog, SIGNAL(canceled()), this, SLOT(cancelDownload()));
    connect(downloadButton, SIGNAL(clicked()), this, SLOT(downloadFile()));
    connect(quitButton, SIGNAL(clicked()), this, SLOT(close()));

    QHBoxLayout *topLayout = new QHBoxLayout;
    topLayout->addWidget(urlLabel);
    topLayout->addWidget(urlLineEdit);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(statusLabel);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);

    setWindowTitle(tr("HTTP"));
    urlLineEdit->setFocus();
}

void HttpWindow::downloadFile()
{
    QUrl url(urlLineEdit->text());
    QFileInfo fileInfo(url.path());
    QString fileName = fileInfo.fileName();
    if (fileName.isEmpty())
        fileName = "index.html";

    if (QFile::exists(fileName)) {
        if (QMessageBox::question(this, tr("HTTP"), 
                                  tr("There already exists a file called %1 in "
                                     "the current directory. Overwrite?").arg(fileName),
                                  QMessageBox::Ok|QMessageBox::Cancel, QMessageBox::Cancel)
            == QMessageBox::Cancel)
            return;
        QFile::remove(fileName);
    }

    file = new QFile(fileName);
    if (!file->open(QIODevice::WriteOnly)) {
        QMessageBox::information(this, tr("HTTP"),
                                 tr("Unable to save the file %1: %2.")
                                 .arg(fileName).arg(file->errorString()));
        delete file;
        file = 0;
        return;
    }

    QHttp::ConnectionMode mode = url.scheme().toLower() == "https" ? QHttp::ConnectionModeHttps : QHttp::ConnectionModeHttp;
    http->setHost(url.host(), mode, url.port() == -1 ? 0 : url.port());
    
    if (!url.userName().isEmpty())
        http->setUser(url.userName(), url.password());

    httpRequestAborted = false;
    QByteArray path = QUrl::toPercentEncoding(url.path(), "!$&'()*+,;=:@/");
    if (path.isEmpty())
        path = "/";
    httpGetId = http->get(path, file);

    progressDialog->setWindowTitle(tr("HTTP"));
    progressDialog->setLabelText(tr("Downloading %1.").arg(fileName));
    downloadButton->setEnabled(false);
}

void HttpWindow::cancelDownload()
{
    statusLabel->setText(tr("Download canceled."));
    httpRequestAborted = true;
    http->abort();
    downloadButton->setEnabled(true);
}

void HttpWindow::httpRequestFinished(int requestId, bool error)
{
    if (requestId != httpGetId)
        return;
    if (httpRequestAborted) {
        if (file) {
            file->close();
            file->remove();
            delete file;
            file = 0;
        }

        progressDialog->hide();
        return;
    }

    if (requestId != httpGetId)
        return;

    progressDialog->hide();
    file->close();

    if (error) {
        file->remove();
        QMessageBox::information(this, tr("HTTP"),
                                 tr("Download failed: %1.")
                                 .arg(http->errorString()));
    } else {
        QString fileName = QFileInfo(QUrl(urlLineEdit->text()).path()).fileName();
        statusLabel->setText(tr("Downloaded %1 to current directory.").arg(fileName));
    }

    downloadButton->setEnabled(true);
    delete file;
    file = 0;
}

void HttpWindow::readResponseHeader(const QHttpResponseHeader &responseHeader)
{
    switch (responseHeader.statusCode()) {
    case 200:                   // Ok
    case 301:                   // Moved Permanently
    case 302:                   // Found
    case 303:                   // See Other
    case 307:                   // Temporary Redirect
        // these are not error conditions
        break;

    default:
        QMessageBox::information(this, tr("HTTP"),
                                 tr("Download failed: %1.")
                                 .arg(responseHeader.reasonPhrase()));
        httpRequestAborted = true;
        progressDialog->hide();
        http->abort();
    }
}

void HttpWindow::updateDataReadProgress(int bytesRead, int totalBytes)
{
    if (httpRequestAborted)
        return;

    progressDialog->setMaximum(totalBytes);
    progressDialog->setValue(bytesRead);
}

void HttpWindow::enableDownloadButton()
{
    downloadButton->setEnabled(!urlLineEdit->text().isEmpty());
}

void HttpWindow::slotAuthenticationRequired(const QString &hostName, quint16, QAuthenticator *authenticator)
{
    QDialog dlg;
    Ui::Dialog ui;
    ui.setupUi(&dlg);
    dlg.adjustSize();
    ui.siteDescription->setText(tr("%1 at %2").arg(authenticator->realm()).arg(hostName));
    
    if (dlg.exec() == QDialog::Accepted) {
        authenticator->setUser(ui.userEdit->text());
        authenticator->setPassword(ui.passwordEdit->text());
    }
}

#ifndef QT_NO_OPENSSL
void HttpWindow::sslErrors(const QList<QSslError> &errors)
{
    QString errorString;
    foreach (const QSslError &error, errors) {
        if (!errorString.isEmpty())
            errorString += ", ";
        errorString += error.errorString();
    }
    
    if (QMessageBox::warning(this, tr("HTTP Example"),
                             tr("One or more SSL errors has occurred: %1").arg(errorString),
                             QMessageBox::Ignore | QMessageBox::Abort) == QMessageBox::Ignore) {
        http->ignoreSslErrors();
    }
}
#endif
