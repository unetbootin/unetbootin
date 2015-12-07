/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtCore module of the Qt Toolkit.
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
