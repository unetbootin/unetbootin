/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Jos van den Oever
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2006 Jos van den Oever <jos@vandenoever.info>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#ifndef BUFFEREDSTREAM_H
#define BUFFEREDSTREAM_H

#include "streambase.h"
#include "inputstreambuffer.h"
#include <cassert>

namespace jstreams {

template <class T>
class BufferedInputStream : public StreamBase<T> {
private:
    bool finishedWritingToBuffer;
    InputStreamBuffer<T> buffer;

    void writeToBuffer(int32_t minsize);
    int32_t read_(const T*& start, int32_t min, int32_t max);
protected:
    /**
     * This function must be implemented by the subclasses.
     * It should write a maximum of @p space characters at the buffer
     * position pointed to by @p start. If no more data is avaiable due to
     * end of file, -1 should be returned. If an error occurs, the status
     * should be set to Error, an error message should be set and the function
     * must return -1.
     **/
    virtual int32_t fillBuffer(T* start, int32_t space) = 0;
    // this function might be useful if you want to reuse a bufferedstream
    void resetBuffer() {printf("implement 'resetBuffer'\n");}
    BufferedInputStream<T>();
public:
    int32_t read(const T*& start, int32_t min, int32_t max);
    int64_t reset(int64_t);
    virtual int64_t skip(int64_t ntoskip);
};

template <class T>
BufferedInputStream<T>::BufferedInputStream() {
    finishedWritingToBuffer = false;
}

template <class T>
void
BufferedInputStream<T>::writeToBuffer(int32_t ntoread) {
    int32_t missing = ntoread - buffer.avail;
    int32_t nwritten = 0;
    while (missing > 0 && nwritten >= 0) {
        int32_t space;
        space = buffer.makeSpace(missing);
        T* start = buffer.readPos + buffer.avail;
        nwritten = fillBuffer(start, space);
        assert(StreamBase<T>::status != Eof);
        if (nwritten > 0) {
            buffer.avail += nwritten;
            missing = ntoread - buffer.avail;
        }
    }
    if (nwritten < 0) {
        finishedWritingToBuffer = true;
    }
}
template <class T>
int32_t
BufferedInputStream<T>::read(const T*& start, int32_t min, int32_t max) {
    if (StreamBase<T>::status == Error) return -2;
    if (StreamBase<T>::status == Eof) return -1;

    // do we need to read data into the buffer?
    if (!finishedWritingToBuffer && min > buffer.avail) {
        // do we have enough space in the buffer?
        writeToBuffer(min);
        if (StreamBase<T>::status == Error) return -2;
    }

    int32_t nread = buffer.read(start, max);

    BufferedInputStream<T>::position += nread;
    if (BufferedInputStream<T>::position > BufferedInputStream<T>::size
        && BufferedInputStream<T>::size > 0) {
        // error: we read more than was specified in size
        // this is an error because all dependent code might have been labouring
        // under a misapprehension
        BufferedInputStream<T>::status = Error;
        BufferedInputStream<T>::error = "Stream is longer than specified.";
        nread = -2;
    } else if (BufferedInputStream<T>::status == Ok && buffer.avail == 0
            && finishedWritingToBuffer) {
        BufferedInputStream<T>::status = Eof;
        if (BufferedInputStream<T>::size == -1) {
            BufferedInputStream<T>::size = BufferedInputStream<T>::position;
        }
        // save one call to read() by already returning -1 if no data is there
        if (nread == 0) nread = -1;
    }
    return nread;
}
template <class T>
int64_t
BufferedInputStream<T>::reset(int64_t newpos) {
    if (StreamBase<T>::status == Error) return -2;
    // check to see if we have this position
    int64_t d = BufferedInputStream<T>::position - newpos;
    if (buffer.readPos - d >= buffer.start && -d < buffer.avail) {
        BufferedInputStream<T>::position -= d;
        buffer.avail += (int32_t)d;
        buffer.readPos -= d;
        StreamBase<T>::status = Ok;
    }
    return StreamBase<T>::position;
}
template <class T>
int64_t
BufferedInputStream<T>::skip(int64_t ntoskip) {
    const T *begin;
    int32_t nread;
    int64_t skipped = 0;
    while (ntoskip) {
        int32_t step = (int32_t)((ntoskip > buffer.size) ?buffer.size :ntoskip);
        nread = read(begin, 1, step);
        if (nread <= 0) {
            return skipped;
        }
        ntoskip -= nread;
        skipped += nread;
    }
    return skipped;
}
}

#endif
