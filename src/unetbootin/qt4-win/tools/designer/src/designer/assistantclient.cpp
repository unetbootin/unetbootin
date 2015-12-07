/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt Designer of the Qt Toolkit.
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

#include "assistantclient.h"

#include <QtCore/QString>
#include <QtCore/QProcess>
#include <QtCore/QDir>
#include <QtCore/QLibraryInfo>
#include <QtCore/QDebug>
#include <QtCore/QFileInfo>
#include <QtCore/QObject>
#include <QtCore/QTextStream>

QT_BEGIN_NAMESPACE

enum { debugAssistantClient = 0 };

AssistantClient::AssistantClient() :
    m_process(0)
{
}

AssistantClient::~AssistantClient()
{
    if (isRunning()) {
        m_process->terminate();
        m_process->waitForFinished();
    }
    delete m_process;
}

bool AssistantClient::showPage(const QString &path, QString *errorMessage)
{
    QString cmd = QLatin1String("SetSource ");
    cmd += path;
    return sendCommand(cmd, errorMessage);
}

bool AssistantClient::activateIdentifier(const QString &identifier, QString *errorMessage)
{
    QString cmd = QLatin1String("ActivateIdentifier ");
    cmd += identifier;
    return sendCommand(cmd, errorMessage);
}

bool AssistantClient::activateKeyword(const QString &keyword, QString *errorMessage)
{
    QString cmd = QLatin1String("ActivateKeyword ");
    cmd += keyword;
    return sendCommand(cmd, errorMessage);
}

bool AssistantClient::sendCommand(const QString &cmd, QString *errorMessage)
{
    if (debugAssistantClient)
        qDebug() << "sendCommand " << cmd;
    if (!ensureRunning(errorMessage))
        return false;
    if (!m_process->isWritable() || m_process->bytesToWrite() > 0) {
        *errorMessage = QObject::tr("Unable to send request: Assistant is not responding.");
        return false;
    }
    QTextStream str(m_process);
    str << cmd << QLatin1Char('\0') << endl;
    return true;
}

bool AssistantClient::isRunning() const
{
    return m_process && m_process->state() != QProcess::NotRunning;
}

QString AssistantClient::binary()
{
    QString app = QLibraryInfo::location(QLibraryInfo::BinariesPath) + QDir::separator();
#if !defined(Q_OS_MAC)
    app += QLatin1String("assistant");
#else
    app += QLatin1String("Assistant.app/Contents/MacOS/Assistant");    
#endif

#if defined(Q_OS_WIN)
    app += QLatin1String(".exe");
#endif

    return app;
}

bool AssistantClient::ensureRunning(QString *errorMessage)
{
    if (isRunning())
        return true;

    if (!m_process)
        m_process = new QProcess;

    const QString app = binary();
    if (!QFileInfo(app).isFile()) {
        *errorMessage = QObject::tr("The binary '%1' does not exist.").arg(app);
        return false;
    }
    if (debugAssistantClient)
        qDebug() << "Running " << app;
    // run
    QStringList args(QLatin1String("-enableRemoteControl"));
    m_process->start(app, args);
    if (!m_process->waitForStarted()) {
        *errorMessage = QObject::tr("Unable to launch assistant (%1)").arg(app);
        return false;
    }
    return true;
}

QString AssistantClient::documentUrl(const QString &prefix, int qtVersion)
{
    if (qtVersion == 0)
        qtVersion = QT_VERSION;
    QString rc;
    QTextStream(&rc) << QLatin1String("qthelp://com.trolltech.") << prefix << QLatin1Char('.')
                     << (qtVersion >> 16) << ((qtVersion >> 8) & 0xFF) << (qtVersion & 0xFF)
                     << QLatin1String("/qdoc/");
    return rc;
}

QString AssistantClient::designerManualUrl(int qtVersion)
{
    return documentUrl(QLatin1String("designer"), qtVersion);
}

QString AssistantClient::qtReferenceManualUrl(int qtVersion)
{
    return documentUrl(QLatin1String("qt"), qtVersion);
}

QT_END_NAMESPACE
