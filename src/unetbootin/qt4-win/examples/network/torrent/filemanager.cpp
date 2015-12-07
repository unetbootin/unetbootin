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

#include "filemanager.h"
#include "metainfo.h"

#include <QByteArray>
#include <QDir>
#include <QFile>
#include <QTimer>
#include <QTimerEvent>

extern "C" {
#include "3rdparty/sha1.h"
}

FileManager::FileManager(QObject *parent)
    : QThread(parent)
{
    quit = false;
    totalLength = 0;
    readId = 0;
    startVerification = false;
    wokeUp = false;
    newFile = false;
    numPieces = 0;
    verifiedPieces.fill(false);
}

FileManager::~FileManager()
{
    quit = true;
    cond.wakeOne();
    wait();

    foreach (QFile *file, files) {
        file->close();
        delete file;
    }
}

int FileManager::read(int pieceIndex, int offset, int length)
{
    ReadRequest request;
    request.pieceIndex = pieceIndex;
    request.offset = offset;
    request.length = length;

    QMutexLocker locker(&mutex);
    request.id = readId++;
    readRequests << request;

    if (!wokeUp) {
        wokeUp = true;
        QMetaObject::invokeMethod(this, "wakeUp", Qt::QueuedConnection);
    }

    return request.id;
}

void FileManager::write(int pieceIndex, int offset, const QByteArray &data)
{
    WriteRequest request;
    request.pieceIndex = pieceIndex;
    request.offset = offset;
    request.data = data;

    QMutexLocker locker(&mutex);
    writeRequests << request;

    if (!wokeUp) {
        wokeUp = true;
        QMetaObject::invokeMethod(this, "wakeUp", Qt::QueuedConnection);
    }
}

void FileManager::verifyPiece(int pieceIndex)
{
    QMutexLocker locker(&mutex);
    pendingVerificationRequests << pieceIndex;
    startVerification = true;

    if (!wokeUp) {
        wokeUp = true;
        QMetaObject::invokeMethod(this, "wakeUp", Qt::QueuedConnection);
    }
}

int FileManager::pieceLengthAt(int pieceIndex) const
{
    QMutexLocker locker(&mutex);
    return (sha1s.size() == pieceIndex + 1)
        ? (totalLength % pieceLength) : pieceLength;
}

QBitArray FileManager::completedPieces() const
{
    QMutexLocker locker(&mutex);
    return verifiedPieces;
}

void FileManager::setCompletedPieces(const QBitArray &pieces)
{
    QMutexLocker locker(&mutex);
    verifiedPieces = pieces;
}

QString FileManager::errorString() const
{
    return errString;
}

void FileManager::run()
{
    if (!generateFiles())
        return;

    do {
        {
            // Go to sleep if there's nothing to do.
            QMutexLocker locker(&mutex);
            if (!quit && readRequests.isEmpty() && writeRequests.isEmpty() && !startVerification)
                cond.wait(&mutex);
        }

        // Read pending read requests
        mutex.lock();
        QList<ReadRequest> newReadRequests = readRequests;
        readRequests.clear();
        mutex.unlock();
        while (!newReadRequests.isEmpty()) {
            ReadRequest request = newReadRequests.takeFirst();
            QByteArray block = readBlock(request.pieceIndex, request.offset, request.length);
            emit dataRead(request.id, request.pieceIndex, request.offset, block);
        }

        // Write pending write requests
        mutex.lock();
        QList<WriteRequest> newWriteRequests = writeRequests;
        writeRequests.clear();
        while (!quit && !newWriteRequests.isEmpty()) {
            WriteRequest request = newWriteRequests.takeFirst();
            writeBlock(request.pieceIndex, request.offset, request.data);
        }

        // Process pending verification requests
        if (startVerification) {
            newPendingVerificationRequests = pendingVerificationRequests;
            pendingVerificationRequests.clear();
            verifyFileContents();
            startVerification = false;
        }
        mutex.unlock();
        newPendingVerificationRequests.clear();

    } while (!quit);

    // Write pending write requests
    mutex.lock();
    QList<WriteRequest> newWriteRequests = writeRequests;
    writeRequests.clear();
    mutex.unlock();
    while (!newWriteRequests.isEmpty()) {
        WriteRequest request = newWriteRequests.takeFirst();
        writeBlock(request.pieceIndex, request.offset, request.data);
    }
}

void FileManager::startDataVerification()
{
    QMutexLocker locker(&mutex);
    startVerification = true;
    cond.wakeOne();
}

bool FileManager::generateFiles()
{
    numPieces = -1;

    // Set up the thread local data
    if (metaInfo.fileForm() == MetaInfo::SingleFileForm) {
        QMutexLocker locker(&mutex);
        MetaInfoSingleFile singleFile = metaInfo.singleFile();

        QString prefix;
        if (!destinationPath.isEmpty()) {
            prefix = destinationPath;
            if (!prefix.endsWith("/"))
                prefix += "/";
            QDir dir;
            if (!dir.mkpath(prefix)) {
                errString = tr("Failed to create directory %1").arg(prefix);
                emit error();
                return false;
            }
        }
        QFile *file = new QFile(prefix + singleFile.name);
        if (!file->open(QFile::ReadWrite)) {
            errString = tr("Failed to open/create file %1: %2")
                        .arg(file->fileName()).arg(file->errorString());
            emit error();
            return false;
        }

        if (file->size() != singleFile.length) {
            newFile = true;
            if (!file->resize(singleFile.length)) {
                errString = tr("Failed to resize file %1: %2")
                            .arg(file->fileName()).arg(file->errorString());
                emit error();
                return false;
            }
        }
        fileSizes << file->size();
        files << file;
        file->close();

        pieceLength = singleFile.pieceLength;
        totalLength = singleFile.length;
        sha1s = singleFile.sha1Sums;
    } else {
        QMutexLocker locker(&mutex);
        QDir dir;
        QString prefix;

        if (!destinationPath.isEmpty()) {
            prefix = destinationPath;
            if (!prefix.endsWith("/"))
                prefix += "/";
        }
        if (!metaInfo.name().isEmpty()) {
            prefix += metaInfo.name();
            if (!prefix.endsWith("/"))
                prefix += "/";
        }
        if (!dir.mkpath(prefix)) {
            errString = tr("Failed to create directory %1").arg(prefix);
            emit error();
            return false;
        }

        foreach (const MetaInfoMultiFile &entry, metaInfo.multiFiles()) {
            QString filePath = QFileInfo(prefix + entry.path).path();
            if (!QFile::exists(filePath)) {
                if (!dir.mkpath(filePath)) {
                    errString = tr("Failed to create directory %1").arg(filePath);
                    emit error();
                    return false;
                }
            }

            QFile *file = new QFile(prefix + entry.path);
            if (!file->open(QFile::ReadWrite)) {
                errString = tr("Failed to open/create file %1: %2")
                            .arg(file->fileName()).arg(file->errorString());
                emit error();
                return false;
            }

            if (file->size() != entry.length) {
                newFile = true;
                if (!file->resize(entry.length)) {
                    errString = tr("Failed to resize file %1: %2")
                                .arg(file->fileName()).arg(file->errorString());
                    emit error();
                    return false;
                }
            }
            fileSizes << file->size();
            files << file;
            file->close();

            totalLength += entry.length;
        }

        sha1s = metaInfo.sha1Sums();
        pieceLength = metaInfo.pieceLength();
    }
    numPieces = sha1s.size();
    return true;
}

QByteArray FileManager::readBlock(int pieceIndex, int offset, int length)
{
    QByteArray block;
    qint64 startReadIndex = (quint64(pieceIndex) * pieceLength) + offset;
    qint64 currentIndex = 0;

    for (int i = 0; !quit && i < files.size() && length > 0; ++i) {
        QFile *file = files[i];
        qint64 currentFileSize = fileSizes.at(i);
        if ((currentIndex + currentFileSize) > startReadIndex) {
            if (!file->isOpen()) {
                if (!file->open(QFile::ReadWrite)) {
                    errString = tr("Failed to read from file %1: %2")
                        .arg(file->fileName()).arg(file->errorString());
                    emit error();
                    break;
                }
            }

            file->seek(startReadIndex - currentIndex);
            QByteArray chunk = file->read(qMin<qint64>(length, currentFileSize - file->pos()));
            file->close();

            block += chunk;
            length -= chunk.size();
            startReadIndex += chunk.size();
            if (length < 0) {
                errString = tr("Failed to read from file %1 (read %3 bytes): %2")
                            .arg(file->fileName()).arg(file->errorString()).arg(length);
                emit error();
                break;
            }
        }
        currentIndex += currentFileSize;
    }
    return block;
}

bool FileManager::writeBlock(int pieceIndex, int offset, const QByteArray &data)
{
    qint64 startWriteIndex = (qint64(pieceIndex) * pieceLength) + offset;
    qint64 currentIndex = 0;
    int bytesToWrite = data.size();
    int written = 0;

    for (int i = 0; !quit && i < files.size(); ++i) {
        QFile *file = files[i];
        qint64 currentFileSize = fileSizes.at(i);

        if ((currentIndex + currentFileSize) > startWriteIndex) {
            if (!file->isOpen()) {
                if (!file->open(QFile::ReadWrite)) {
                    errString = tr("Failed to write to file %1: %2")
                        .arg(file->fileName()).arg(file->errorString());
                    emit error();
                    break;
                }
            }

            file->seek(startWriteIndex - currentIndex);
            qint64 bytesWritten = file->write(data.constData() + written,
                                              qMin<qint64>(bytesToWrite, currentFileSize - file->pos()));
            file->close();

            if (bytesWritten <= 0) {
                errString = tr("Failed to write to file %1: %2")
                            .arg(file->fileName()).arg(file->errorString());
                emit error();
                return false;
            }

            written += bytesWritten;
            startWriteIndex += bytesWritten;
            bytesToWrite -= bytesWritten;
            if (bytesToWrite == 0)
                break;
        }
        currentIndex += currentFileSize;
    }
    return true;
}

void FileManager::verifyFileContents()
{
    // Verify all pieces the first time
    if (newPendingVerificationRequests.isEmpty()) {
        if (verifiedPieces.count(true) == 0) {
            verifiedPieces.resize(sha1s.size());

            int oldPercent = 0;
            if (!newFile) {
                int numPieces = sha1s.size();

                for (int index = 0; index < numPieces; ++index) {
                    verifySinglePiece(index);

                    int percent = ((index + 1) * 100) / numPieces;
                    if (oldPercent != percent) {
                        emit verificationProgress(percent);
                        oldPercent = percent;
                    }
                }
            }
        }
        emit verificationDone();
        return;
    }

    // Verify all pending pieces
    foreach (int index, newPendingVerificationRequests)
        emit pieceVerified(index, verifySinglePiece(index));
}

bool FileManager::verifySinglePiece(int pieceIndex)
{
    QByteArray block = readBlock(pieceIndex, 0, pieceLength);

    SHA1Context sha;
    SHA1Reset(&sha);
    SHA1Input(&sha, (const unsigned char *)block.constData(), block.size());
    SHA1Result(&sha);

    QByteArray sha1Sum(20, ' ');
    unsigned char *digest = (unsigned char *)sha.Message_Digest;
    for (int i = 0; i < 5; ++i) {
#if Q_BYTE_ORDER == Q_BIG_ENDIAN
        sha1Sum[i * 4 + 3] = digest[i * 4 + 3];
        sha1Sum[i * 4 + 2] = digest[i * 4 + 2];
        sha1Sum[i * 4 + 1] = digest[i * 4 + 1];
        sha1Sum[i * 4 + 0] = digest[i * 4 + 0];
#else
        sha1Sum[i * 4 + 0] = digest[i * 4 + 3];
        sha1Sum[i * 4 + 1] = digest[i * 4 + 2];
        sha1Sum[i * 4 + 2] = digest[i * 4 + 1];
        sha1Sum[i * 4 + 3] = digest[i * 4 + 0];
#endif
    }

    if (sha1Sum != sha1s.at(pieceIndex))
        return false;
    verifiedPieces.setBit(pieceIndex);
    return true;
}

void FileManager::wakeUp()
{
    QMutexLocker locker(&mutex);
    wokeUp = false;
    cond.wakeOne();
}
