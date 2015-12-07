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

#ifndef PEERWIRECLIENT_H
#define PEERWIRECLIENT_H

#include <QBitArray>
#include <QList>
#include <QTcpSocket>

QT_BEGIN_NAMESPACE
class QHostAddress;
class QTimerEvent;
QT_END_NAMESPACE
class TorrentPeer;

struct TorrentBlock
{
    inline TorrentBlock(int p, int o, int l)
        : pieceIndex(p), offset(o), length(l)
    {
    }
    inline bool operator==(const TorrentBlock &other) const
    {
        return pieceIndex == other.pieceIndex
                && offset == other.offset
                && length == other.length;
    }
    
    int pieceIndex;
    int offset;
    int length;
};

class PeerWireClient : public QTcpSocket
{
    Q_OBJECT

public:
    enum PeerWireStateFlag {
        ChokingPeer = 0x1,
        InterestedInPeer = 0x2,
        ChokedByPeer = 0x4,
        PeerIsInterested = 0x8
    };
    Q_DECLARE_FLAGS(PeerWireState, PeerWireStateFlag)

    PeerWireClient(const QByteArray &peerId, QObject *parent = 0);
    void initialize(const QByteArray &infoHash, int pieceCount);

    void setPeer(TorrentPeer *peer);
    TorrentPeer *peer() const;

    // State
    inline PeerWireState peerWireState() const { return pwState; }
    QBitArray availablePieces() const;
    QList<TorrentBlock> incomingBlocks() const;

    // Protocol
    void chokePeer();
    void unchokePeer();
    void sendInterested();
    void sendKeepAlive();
    void sendNotInterested();
    void sendPieceNotification(int piece);
    void sendPieceList(const QBitArray &bitField);
    void requestBlock(int piece, int offset, int length);
    void cancelRequest(int piece, int offset, int length);
    void sendBlock(int piece, int offset, const QByteArray &data);

    // Rate control
    qint64 writeToSocket(qint64 bytes);
    qint64 readFromSocket(qint64 bytes);
    qint64 downloadSpeed() const;
    qint64 uploadSpeed() const;

    bool canTransferMore() const;
    qint64 bytesAvailable() const { return incomingBuffer.size() + QTcpSocket::bytesAvailable(); }
    qint64 socketBytesAvailable() const { return socket.bytesAvailable(); }
    qint64 socketBytesToWrite() const { return socket.bytesToWrite(); }

    void setReadBufferSize(int size);

signals:
    void infoHashReceived(const QByteArray &infoHash);
    void readyToTransfer();

    void choked();
    void unchoked();
    void interested();
    void notInterested();

    void piecesAvailable(const QBitArray &pieces);
    void blockRequested(int pieceIndex, int begin, int length);
    void blockReceived(int pieceIndex, int begin, const QByteArray &data);

    void bytesReceived(qint64 size);

protected slots:
    void connectToHostImplementation(const QString &hostName,
                                     quint16 port, OpenMode openMode = ReadWrite);
    void diconnectFromHostImplementation();

protected:
    void timerEvent(QTimerEvent *event);

    qint64 readData(char *data, qint64 maxlen);
    qint64 readLineData(char *data, qint64 maxlen);
    qint64 writeData(const char *data, qint64 len);

private slots:
    void sendHandShake();
    void processIncomingData();
    void socketStateChanged(QAbstractSocket::SocketState state);

private:
    // Data waiting to be read/written
    QByteArray incomingBuffer;
    QByteArray outgoingBuffer;

    struct BlockInfo {
        int pieceIndex;
        int offset;
        int length;
        QByteArray block;
    };
    QList<BlockInfo> pendingBlocks;
    int pendingBlockSizes;
    QList<TorrentBlock> incoming;

    enum PacketType {
        ChokePacket = 0,
        UnchokePacket = 1,
        InterestedPacket = 2,
        NotInterestedPacket = 3,
        HavePacket = 4,
        BitFieldPacket = 5,
        RequestPacket = 6,
        PiecePacket = 7,
        CancelPacket = 8
    };

    // State
    PeerWireState pwState;
    bool receivedHandShake;
    bool gotPeerId;
    bool sentHandShake;
    int nextPacketLength;

    // Upload/download speed records
    qint64 uploadSpeedData[8];
    qint64 downloadSpeedData[8];
    int transferSpeedTimer;

    // Timeout handling
    int timeoutTimer;
    int pendingRequestTimer;
    bool invalidateTimeout;
    int keepAliveTimer;

    // Checksum, peer ID and set of available pieces
    QByteArray infoHash;
    QByteArray peerIdString;
    QBitArray peerPieces;
    TorrentPeer *torrentPeer;

    QTcpSocket socket;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(PeerWireClient::PeerWireState)

#endif
