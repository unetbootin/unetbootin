/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
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

#include <stdio.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <QtCore/QDebug>
#include <QtCore/QProcess>
#include <QtDBus/QtDBus>

#include "ping-common.h"
#include "complexping.h"

void Ping::start(const QString &name, const QString &oldValue, const QString &newValue)
{
    Q_UNUSED(oldValue);

    if (name != SERVICE_NAME || newValue.isEmpty())
        return;

    // open stdin for reading
    qstdin.open(stdin, QIODevice::ReadOnly);

    // find our remote
    iface = new QDBusInterface(SERVICE_NAME, "/", "com.trolltech.QtDBus.ComplexPong.Pong",
                               QDBusConnection::sessionBus(), this);
    if (!iface->isValid()) {
        fprintf(stderr, "%s\n",
                qPrintable(QDBusConnection::sessionBus().lastError().message()));
        QCoreApplication::instance()->quit();
    }

    connect(iface, SIGNAL(aboutToQuit()), QCoreApplication::instance(), SLOT(quit()));

    while (true) {
        printf("Ask your question: ");

        QString line = QString::fromLocal8Bit(qstdin.readLine()).trimmed();
        if (line.isEmpty()) {
            iface->call("quit");
            return;
        } else if (line == "value") {
            QVariant reply = iface->property("value");
            if (!reply.isNull())
                printf("value = %s\n", qPrintable(reply.toString()));
        } else if (line.startsWith("value=")) {
            iface->setProperty("value", line.mid(6));            
        } else {
            QDBusReply<QDBusVariant> reply = iface->call("query", line);
            if (reply.isValid())
                printf("Reply was: %s\n", qPrintable(reply.value().variant().toString()));
        }

        if (iface->lastError().isValid())
            fprintf(stderr, "Call failed: %s\n", qPrintable(iface->lastError().message()));
    }
}    

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    if (!QDBusConnection::sessionBus().isConnected()) {
        fprintf(stderr, "Cannot connect to the D-BUS session bus.\n"
                "To start it, run:\n"
                "\teval `dbus-launch --auto-syntax`\n");
        return 1;
    }

    Ping ping;
    ping.connect(QDBusConnection::sessionBus().interface(),
                 SIGNAL(serviceOwnerChanged(QString,QString,QString)),
                 SLOT(start(QString,QString,QString)));

    QProcess pong;
    pong.start("./complexpong");

    app.exec();
}
