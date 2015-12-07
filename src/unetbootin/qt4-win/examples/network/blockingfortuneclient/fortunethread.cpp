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

#include <QtNetwork>

#include "fortunethread.h"

FortuneThread::FortuneThread(QObject *parent)
    : QThread(parent), quit(false)
{
}

//! [0]
FortuneThread::~FortuneThread()
{
    quit = true;
    cond.wakeOne();
    wait();
}
//! [0]

//! [1] //! [2]
void FortuneThread::requestNewFortune(const QString &hostName, quint16 port)
{
//! [1]
    QMutexLocker locker(&mutex);
    this->hostName = hostName;
    this->port = port;
//! [3]
    if (!isRunning())
        start();
    else
        cond.wakeOne();
}
//! [2] //! [3]

//! [4]
void FortuneThread::run()
{
    mutex.lock();
//! [4] //! [5]
    QString serverName = hostName;
    quint16 serverPort = port;
    mutex.unlock();
//! [5]

//! [6]
    while (!quit) {
//! [7]
        const int Timeout = 5 * 1000;

        QTcpSocket socket;
        socket.connectToHost(serverName, serverPort);
//! [6] //! [8]

        if (!socket.waitForConnected(Timeout)) {
            emit error(socket.error(), socket.errorString());
            return;
        }
//! [8] //! [9]

        while (socket.bytesAvailable() < (int)sizeof(quint16)) {
            if (!socket.waitForReadyRead(Timeout)) {
                emit error(socket.error(), socket.errorString());
                return;
            }
//! [9] //! [10]
        }
//! [10] //! [11]

        quint16 blockSize;
        QDataStream in(&socket);
        in.setVersion(QDataStream::Qt_4_0);
        in >> blockSize;
//! [11] //! [12]

        while (socket.bytesAvailable() < blockSize) {
            if (!socket.waitForReadyRead(Timeout)) {
                emit error(socket.error(), socket.errorString());
                return;
            }
//! [12] //! [13]
        }
//! [13] //! [14]

        QMutexLocker locker(&mutex);

        QString fortune;
        in >> fortune;
        emit newFortune(fortune);
//! [7] //! [14] //! [15]

        cond.wait(&mutex);
        serverName = hostName;
        serverPort = port;
    }
//! [15]
}
