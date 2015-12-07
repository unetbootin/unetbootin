/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#ifndef QRINGBUFFER_P_H
#define QRINGBUFFER_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of a number of Qt sources files.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//

#include <QtCore/qbytearray.h>
#include <QtCore/qlist.h>

QT_BEGIN_NAMESPACE

class Q_CORE_EXPORT QRingBuffer
{
public:
    inline QRingBuffer(int growth = 4096) : basicBlockSize(growth) {
        buffers << QByteArray();
        clear();
    }

    inline int nextDataBlockSize() const {
        return (tailBuffer == 0 ? tail : buffers.first().size()) - head;
    }

    inline const char *readPointer() const {
        return buffers.isEmpty() ? 0 : (buffers.first().constData() + head);
    }

    inline void free(int bytes) {
        bufferSize -= bytes;
        if (bufferSize < 0)
            bufferSize = 0;

        for (;;) {
            int nextBlockSize = nextDataBlockSize();
            if (bytes < nextBlockSize) {
                head += bytes;
                if (head == tail && tailBuffer == 0)
                    head = tail = 0;
                break;
            }

            bytes -= nextBlockSize;
            if (buffers.count() == 1) {
                if (buffers.at(0).size() != basicBlockSize)
                    buffers[0].resize(basicBlockSize);
                head = tail = 0;
                tailBuffer = 0;
                break;
            }

            buffers.removeAt(0);
            --tailBuffer;
            head = 0;
        }
    }

    inline char *reserve(int bytes) {
        bufferSize += bytes;

        // if there is already enough space, simply return.
        if (tail + bytes <= buffers.at(tailBuffer).size()) {
            char *writePtr = buffers[tailBuffer].data() + tail;
            tail += bytes;
            return writePtr;
        }

        // if our buffer isn't half full yet, simply resize it.
        if (tail < buffers.at(tailBuffer).size() / 2) {
            buffers[tailBuffer].resize(tail + bytes);
            char *writePtr = buffers[tailBuffer].data() + tail;
            tail += bytes;
            return writePtr;
        }

        // shrink this buffer to its current size
        buffers[tailBuffer].resize(tail);

        // create a new QByteArray with the right size
        buffers << QByteArray();
        ++tailBuffer;
        buffers[tailBuffer].resize(qMax(basicBlockSize, bytes));
        tail = bytes;
        return buffers[tailBuffer].data();
    }

    inline void truncate(int pos) {
        if (pos < size())
            chop(size() - pos);
    }

    inline void chop(int bytes) {
        bufferSize -= bytes;
        if (bufferSize < 0)
            bufferSize = 0;

        for (;;) {
            // special case: head and tail are in the same buffer
            if (tailBuffer == 0) {
                tail -= bytes;
                if (tail <= head)
                    tail = head = 0;
                return;
            }

            if (bytes <= tail) {
                tail -= bytes;
                return;
            }

            bytes -= tail;
            buffers.removeAt(tailBuffer);

            --tailBuffer;
            tail = buffers.at(tailBuffer).size();
        }
    }

    inline bool isEmpty() const {
        return tailBuffer == 0 && tail == 0;
    }

    inline int getChar() {
        if (isEmpty())
            return -1;
        char c = *readPointer();
        free(1);
        return int(uchar(c));
    }

    inline void putChar(char c) {
        char *ptr = reserve(1);
        *ptr = c;
    }

    inline void ungetChar(char c) {
        --head;
        if (head < 0) {
            buffers.prepend(QByteArray());
            buffers[0].resize(basicBlockSize);
            head = basicBlockSize - 1;
            ++tailBuffer;
        }
        buffers[0][head] = c;
        ++bufferSize;
    }

    inline int size() const {
        return bufferSize;
    }

    inline void clear() {
        if(!buffers.isEmpty()) {
            QByteArray tmp = buffers[0];
            buffers.clear();
            buffers << tmp;
            if (buffers.at(0).size() != basicBlockSize)
                buffers[0].resize(basicBlockSize);
        }
        head = tail = 0;
        tailBuffer = 0;
        bufferSize = 0;
    }

    inline int indexOf(char c) const {
        int index = 0;
        for (int i = 0; i < buffers.size(); ++i) {
            int start = 0;
            int end = buffers.at(i).size();

            if (i == 0)
                start = head;
            if (i == tailBuffer)
                end = tail;
            const char *ptr = buffers.at(i).data() + start;
            for (int j = start; j < end; ++j) {
                if (*ptr++ == c)
                    return index;
                ++index;
            }
        }
        return -1;
    }

    inline int read(char *data, int maxLength) {
        int bytesToRead = qMin(size(), maxLength);
        int readSoFar = 0;
        while (readSoFar < bytesToRead) {
            const char *ptr = readPointer();
            int bytesToReadFromThisBlock = qMin(bytesToRead - readSoFar, nextDataBlockSize());
            if (data)
                memcpy(data + readSoFar, ptr, bytesToReadFromThisBlock);
            readSoFar += bytesToReadFromThisBlock;
            free(bytesToReadFromThisBlock);
        }
        return readSoFar;
    }

    inline QByteArray read(int maxLength) {
        QByteArray tmp;
        tmp.resize(qMin(maxLength, size()));
        read(tmp.data(), tmp.size());
        return tmp;
    }

    inline QByteArray readAll() {
        return read(size());
    }

    inline QByteArray peek(int maxLength) const {
        int bytesToRead = qMin(size(), maxLength);
        if(maxLength <= 0)
            return QByteArray();
        QByteArray ret;
        ret.resize(bytesToRead);
        int readSoFar = 0;
        for (int i = 0; readSoFar < bytesToRead && i < buffers.size(); ++i) {
            int start = 0;
            int end = buffers.at(i).size();
            if (i == 0)
                start = head;
            if (i == tailBuffer)
                end = tail;
            const int len = qMin(ret.size()-readSoFar, end-start);
            memcpy(ret.data()+readSoFar, buffers.at(i).constData()+start, len);
            readSoFar += len;
        }
        Q_ASSERT(readSoFar == ret.size());
        return ret;
    }

    inline int skip(int length) {
        return read(0, length);
    }

    inline int readLine(char *data, int maxLength) {
        int index = indexOf('\n');
        if (index == -1)
            return read(data, maxLength);
        if (maxLength <= 0)
            return -1;

        int readSoFar = 0;
        while (readSoFar < index + 1 && readSoFar < maxLength - 1) {
            int bytesToRead = qMin((index + 1) - readSoFar, nextDataBlockSize());
            bytesToRead = qMin(bytesToRead, (maxLength - 1) - readSoFar);
            memcpy(data + readSoFar, readPointer(), bytesToRead);
            readSoFar += bytesToRead;
            free(bytesToRead);
        }

        // Terminate it.
        data[readSoFar] = '\0';
        return readSoFar;
    }

    inline bool canReadLine() const {
        return indexOf('\n') != -1;
    }

private:
    QList<QByteArray> buffers;
    int head, tail;
    int tailBuffer;
    int basicBlockSize;
    int bufferSize;
};

QT_END_NAMESPACE

#endif // QRINGBUFFER_P_H
