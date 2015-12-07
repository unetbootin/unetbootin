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
#ifndef SUBINPUTSTREAM_H
#define SUBINPUTSTREAM_H

#include "streambase.h"

namespace jstreams {

template<class T>
class SubInputStream : public StreamBase<T> {
private:
    const int64_t offset;
    StreamBase<T> *input;
public:
    SubInputStream(StreamBase<T> *input, int64_t size=-1);
    int32_t read(const T*& start, int32_t min, int32_t max);
    int64_t reset(int64_t newpos);
    int64_t skip(int64_t ntoskip);
};
template<class T>
SubInputStream<T>::SubInputStream(StreamBase<T> *i, int64_t length)
        : offset(i->getPosition()), input(i) {
    assert(length >= -1);
//    printf("substream offset: %lli\n", offset);
    StreamBase<T>::size = length;
}

template<class T>
int32_t SubInputStream<T>::read(const T*& start, int32_t min, int32_t max) {
    if (StreamBase<T>::size != -1) {
        const int64_t left = StreamBase<T>::size - StreamBase<T>::position;
        if (left == 0) {
            return -1;
        }
        // restrict the amount of data that can be read
        if (max <= 0 || max > left) {
            max = (int32_t)left;
        }
        if (min > max) min = max;
        if (left < min) min = (int32_t)left;
    }
    int32_t nread = input->read(start, min, max);
    if (nread < -1) {
        fprintf(stderr, "substream too short.\n");
        StreamBase<T>::status = Error;
        StreamBase<T>::error = input->getError();
    } else if (nread < min) {
        if (StreamBase<T>::size == -1) {
            StreamBase<T>::status = Eof;
            if (nread > 0) {
                StreamBase<T>::position += nread;
                StreamBase<T>::size = StreamBase<T>::position;
            }
        } else {
//            fprintf(stderr, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! nread %i min %i max %i size %lli\n", nread, min, max, size);
//            fprintf(stderr, "pos %lli parentpos %lli\n", position, input->getPosition());
//            fprintf(stderr, "status: %i error: %s\n", input->getStatus(), input->getError());
            // we expected data but didn't get enough so that's an error
            StreamBase<T>::status = Error;
            StreamBase<T>::error = "Premature end of stream\n";
            nread = -2;
        }
    } else {
        StreamBase<T>::position += nread;
        if (StreamBase<T>::position == StreamBase<T>::size) {
            StreamBase<T>::status = Eof;
        }
    }
    return nread;
}

template<class T>
int64_t SubInputStream<T>::reset(int64_t newpos) {
//    fprintf(stderr, "subreset pos: %lli newpos: %lli offset: %lli\n", position,
//        newpos, offset);
    StreamBase<T>::position = input->reset(newpos + offset);
    if (StreamBase<T>::position < offset) {
        printf("###########\n");
        StreamBase<T>::status = Error;
        StreamBase<T>::error = input->getError();
    } else {
        StreamBase<T>::position -= offset;
        StreamBase<T>::status = input->getStatus();
    }
    return StreamBase<T>::position;
}

template<class T>
int64_t SubInputStream<T>::skip(int64_t ntoskip) {
//    printf("subskip pos: %lli ntoskip: %lli offset: %lli\n", position, ntoskip, offset);
    if (StreamBase<T>::size == StreamBase<T>::position) {
        StreamBase<T>::status = Eof;
        return -1;
    }
    if (StreamBase<T>::size != -1) {
        const int64_t left = StreamBase<T>::size - StreamBase<T>::position;
        // restrict the amount of data that can be skipped
        if (ntoskip > left) {
            ntoskip = left;
        }
    }
    int64_t skipped = input->skip(ntoskip);
    if (input->getStatus() == Error) {
        StreamBase<T>::status = Error;
        StreamBase<T>::error = input->getError();
    } else {
        StreamBase<T>::position += skipped;
        if (StreamBase<T>::position == StreamBase<T>::size) {
            StreamBase<T>::status = Eof;
        }
    }
    return skipped;
}

} //end namespace jstreams

#endif
