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

#include "peerwireclient.h"
#include "ratecontroller.h"

#include <QtCore>

Q_GLOBAL_STATIC(RateController, rateController)

RateController *RateController::instance()
{
    return rateController();
}

void RateController::addSocket(PeerWireClient *socket)
{
    connect(socket, SIGNAL(readyToTransfer()), this, SLOT(scheduleTransfer()));
    socket->setReadBufferSize(downLimit * 4);
    sockets << socket;
    scheduleTransfer();
}

void RateController::removeSocket(PeerWireClient *socket)
{
    disconnect(socket, SIGNAL(readyToTransfer()), this, SLOT(scheduleTransfer()));
    socket->setReadBufferSize(0);
    sockets.remove(socket);
}

void RateController::setDownloadLimit(int bytesPerSecond)
{
    downLimit = bytesPerSecond;
    foreach (PeerWireClient *socket, sockets)
        socket->setReadBufferSize(downLimit * 4);
}

void RateController::scheduleTransfer()
{
    if (transferScheduled)
        return;
    transferScheduled = true;
    QTimer::singleShot(50, this, SLOT(transfer()));
}

void RateController::transfer()
{
    transferScheduled = false;
    if (sockets.isEmpty())
        return;

    int msecs = 1000;
    if (!stopWatch.isNull())
        msecs = qMin(msecs, stopWatch.elapsed());

    qint64 bytesToWrite = (upLimit * msecs) / 1000;
    qint64 bytesToRead = (downLimit * msecs) / 1000;
    if (bytesToWrite == 0 && bytesToRead == 0) {
        scheduleTransfer();
        return;
    }

    QSet<PeerWireClient *> pendingSockets;
    foreach (PeerWireClient *client, sockets) {
        if (client->canTransferMore())
            pendingSockets << client;
    }
    if (pendingSockets.isEmpty())
        return;

    stopWatch.start();

    bool canTransferMore;
    do {
        canTransferMore = false;
        qint64 writeChunk = qMax<qint64>(1, bytesToWrite / pendingSockets.size());
        qint64 readChunk = qMax<qint64>(1, bytesToRead / pendingSockets.size());

        QSetIterator<PeerWireClient *> it(pendingSockets);
        while (it.hasNext() && (bytesToWrite > 0 || bytesToRead > 0)) {
            PeerWireClient *socket = it.next();
            if (socket->state() != QAbstractSocket::ConnectedState) {
                pendingSockets.remove(socket);
                continue;
            }

            bool dataTransferred = false;
            qint64 available = qMin<qint64>(socket->socketBytesAvailable(), readChunk);
            if (available > 0) {
                qint64 readBytes = socket->readFromSocket(qMin<qint64>(available, bytesToRead));
                if (readBytes > 0) {
                    bytesToRead -= readBytes;
                    dataTransferred = true;
                }
            }

            if (upLimit * 2 > socket->bytesToWrite()) {
                qint64 chunkSize = qMin<qint64>(writeChunk, bytesToWrite);
                qint64 toWrite = qMin(upLimit * 2 - socket->bytesToWrite(), chunkSize);
                if (toWrite > 0) {
                    qint64 writtenBytes = socket->writeToSocket(toWrite);
                    if (writtenBytes > 0) {
                        bytesToWrite -= writtenBytes;
                        dataTransferred = true;
                    }
                }
            }

            if (dataTransferred && socket->canTransferMore())
                canTransferMore = true;
            else
                pendingSockets.remove(socket);
        }
    } while (canTransferMore && (bytesToWrite > 0 || bytesToRead > 0) && !pendingSockets.isEmpty());

    if (canTransferMore || bytesToWrite == 0 || bytesToRead == 0)
        scheduleTransfer();
}
