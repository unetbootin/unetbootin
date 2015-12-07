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

#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QBitArray>
#include <QList>
#include <QMutex>
#include <QThread>
#include <QWaitCondition>

#include "metainfo.h"

QT_BEGIN_NAMESPACE
class QByteArray;
class QFile;
class QTimerEvent;
QT_END_NAMESPACE

class FileManager : public QThread
{
    Q_OBJECT

public:
    FileManager(QObject *parent = 0);
    virtual ~FileManager();

    inline void setMetaInfo(const MetaInfo &info) { metaInfo = info; }
    inline void setDestinationFolder(const QString &directory) { destinationPath = directory; }
    
    int read(int pieceIndex, int offset, int length);
    void write(int pieceIndex, int offset, const QByteArray &data);
    void verifyPiece(int pieceIndex);
    inline qint64 totalSize() const { return totalLength; }

    inline int pieceCount() const { return numPieces; }
    int pieceLengthAt(int pieceIndex) const;

    QBitArray completedPieces() const;
    void setCompletedPieces(const QBitArray &pieces);

    QString errorString() const;

public slots:
    void startDataVerification();

signals:
    void dataRead(int id, int pieceIndex, int offset, const QByteArray &data);
    void error();
    void verificationProgress(int percent);
    void verificationDone();
    void pieceVerified(int pieceIndex, bool verified);

protected:
    void run();

private slots:
    bool verifySinglePiece(int pieceIndex);
    void wakeUp();

private:
    bool generateFiles();
    QByteArray readBlock(int pieceIndex, int offset, int length);
    bool writeBlock(int pieceIndex, int offset, const QByteArray &data);
    void verifyFileContents();

    struct WriteRequest {
        int pieceIndex;
        int offset;
        QByteArray data;
    };
    struct ReadRequest {
        int pieceIndex;
        int offset;
        int length;
        int id;
    };

    QString errString;
    QString destinationPath;
    MetaInfo metaInfo;
    QList<QFile *> files;
    QList<QByteArray> sha1s;
    QBitArray verifiedPieces;

    bool newFile;
    int pieceLength;
    qint64 totalLength;
    int numPieces;
    int readId;
    bool startVerification;
    bool quit;
    bool wokeUp;

    QList<WriteRequest> writeRequests;
    QList<ReadRequest> readRequests;
    QList<int> pendingVerificationRequests;
    QList<int> newPendingVerificationRequests;
    QList<qint64> fileSizes;

    mutable QMutex mutex;
    mutable QWaitCondition cond;
};

#endif
