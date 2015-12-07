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

#include "bencodeparser.h"
#include "connectionmanager.h"
#include "torrentclient.h"
#include "torrentserver.h"
#include "trackerclient.h"

#include <QtCore>

TrackerClient::TrackerClient(TorrentClient *downloader, QObject *parent)
    : QObject(parent), torrentDownloader(downloader)
{
    length = 0;
    requestInterval = 5 * 60;
    requestIntervalTimer = -1;
    firstTrackerRequest = true;
    lastTrackerRequest = false;
    firstSeeding = true;

    connect(&http, SIGNAL(done(bool)), this, SLOT(httpRequestDone(bool)));
}

void TrackerClient::start(const MetaInfo &info)
{
    metaInfo = info;
    QTimer::singleShot(0, this, SLOT(fetchPeerList()));

    if (metaInfo.fileForm() == MetaInfo::SingleFileForm) {
        length = metaInfo.singleFile().length;
    } else {
        QList<MetaInfoMultiFile> files = metaInfo.multiFiles();
        for (int i = 0; i < files.size(); ++i)
            length += files.at(i).length;
    }
}

void TrackerClient::startSeeding()
{
    firstSeeding = true;
    fetchPeerList();
}

void TrackerClient::stop()
{
    lastTrackerRequest = true;
    http.abort();
    fetchPeerList();
}

void TrackerClient::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == requestIntervalTimer) {
        if (http.state() == QHttp::Unconnected)
            fetchPeerList();
    } else {
        QObject::timerEvent(event);
    }
}

void TrackerClient::fetchPeerList()
{
    // Prepare connection details
    QString fullUrl = metaInfo.announceUrl();
    QUrl url(fullUrl);
    QString passkey = "?";
    if (fullUrl.contains("?passkey")) {
        passkey = metaInfo.announceUrl().mid(fullUrl.indexOf("?passkey"), -1);
        passkey += "&";
    }

    // Percent encode the hash
    QByteArray infoHash = torrentDownloader->infoHash();
    QString encodedSum;
    for (int i = 0; i < infoHash.size(); ++i) {
        encodedSum += '%';
        encodedSum += QString::number(infoHash[i], 16).right(2).rightJustified(2, '0');
    }

    bool seeding = (torrentDownloader->state() == TorrentClient::Seeding);
    QByteArray query;
    query += url.path().toLatin1();
    query += passkey;
    query += "info_hash=" + encodedSum;
    query += "&peer_id=" + ConnectionManager::instance()->clientId();
    query += "&port=" + QByteArray::number(TorrentServer::instance()->serverPort());
    query += "&compact=1";
    query += "&uploaded=" + QByteArray::number(torrentDownloader->uploadedBytes());

    if (!firstSeeding) {
        query += "&downloaded=0";
        query += "&left=0";
    } else {
        query += "&downloaded=" + QByteArray::number(
            torrentDownloader->downloadedBytes());
        int left = qMax<int>(0, metaInfo.totalSize() - torrentDownloader->downloadedBytes());
        query += "&left=" + QByteArray::number(seeding ? 0 : left);
    }

    if (seeding && firstSeeding) {
        query += "&event=completed";
        firstSeeding = false;
    } else if (firstTrackerRequest) {
        firstTrackerRequest = false;
        query += "&event=started";
    } else if(lastTrackerRequest) {
        query += "&event=stopped";
    }

    if (!trackerId.isEmpty())
        query += "&trackerid=" + trackerId;

    http.setHost(url.host(), url.port() == -1 ? 80 : url.port());
    if (!url.userName().isEmpty())
        http.setUser(url.userName(), url.password());
    http.get(query);
}

void TrackerClient::httpRequestDone(bool error)
{
    if (lastTrackerRequest) {
        emit stopped();
        return;
    }

    if (error) {
        emit connectionError(http.error());
        return;
    }

    QByteArray response = http.readAll();
    http.abort();

    BencodeParser parser;
    if (!parser.parse(response)) {
        qWarning("Error parsing bencode response from tracker: %s",
                 qPrintable(parser.errorString()));
        http.abort();
        return;
    }

    QMap<QByteArray, QVariant> dict = parser.dictionary();

    if (dict.contains("failure reason")) {
        // no other items are present
        emit failure(QString::fromUtf8(dict.value("failure reason").toByteArray()));
        return;
    }

    if (dict.contains("warning message")) {
        // continue processing
        emit warning(QString::fromUtf8(dict.value("warning message").toByteArray()));
    }

    if (dict.contains("tracker id")) {
        // store it
        trackerId = dict.value("tracker id").toByteArray();
    }

    if (dict.contains("interval")) {
        // Mandatory item
        if (requestIntervalTimer != -1)
            killTimer(requestIntervalTimer);
        requestIntervalTimer = startTimer(dict.value("interval").toInt() * 1000);
    }

    if (dict.contains("peers")) {
        // store it
        peers.clear();
        QVariant peerEntry = dict.value("peers");
        if (peerEntry.type() == QVariant::List) {
            QList<QVariant> peerTmp = peerEntry.toList();
            for (int i = 0; i < peerTmp.size(); ++i) {
                TorrentPeer tmp;
                QMap<QByteArray, QVariant> peer = qVariantValue<QMap<QByteArray, QVariant> >(peerTmp.at(i));
                tmp.id = QString::fromUtf8(peer.value("peer id").toByteArray());
                tmp.address.setAddress(QString::fromUtf8(peer.value("ip").toByteArray()));
                tmp.port = peer.value("port").toInt();
                peers << tmp;
            }
        } else {
            QByteArray peerTmp = peerEntry.toByteArray();
            for (int i = 0; i < peerTmp.size(); i += 6) {
                TorrentPeer tmp;
                uchar *data = (uchar *)peerTmp.constData() + i;
                tmp.port = (int(data[4]) << 8) + data[5];
                uint ipAddress = 0;
                ipAddress += uint(data[0]) << 24;
                ipAddress += uint(data[1]) << 16;
                ipAddress += uint(data[2]) << 8;
                ipAddress += uint(data[3]);
                tmp.address.setAddress(ipAddress);
                peers << tmp;
            }
        }
        emit peerListUpdated(peers);
    }
}
