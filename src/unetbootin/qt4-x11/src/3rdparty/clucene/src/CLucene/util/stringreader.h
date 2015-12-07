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
#ifndef STRINGREADER_H
#define STRINGREADER_H

/**
 * Author: Jos van den Oever
 *         Ben van Klinken
 **/


#include "streambase.h"

namespace jstreams {

template <class T>
class StringReader : public StreamBase<T> {
private:
    int64_t markpt;
    T* data;
    bool dataowner;
    StringReader(const StringReader<T>&);
    void operator=(const StringReader<T>&);
public:
    StringReader(const T* value, int32_t length = -1, bool copy = true);
    ~StringReader();
    int32_t read(const T*& start, int32_t min, int32_t max);
    int64_t skip(int64_t ntoskip);
    int64_t reset(int64_t pos);
};

typedef StringReader<char> StringInputStream;

template <class T>
StringReader<T>::StringReader(const T* value, int32_t length, bool copy)
        : markpt(0), dataowner(copy) {
    if (length < 0) {
        length = 0;
        while (value[length] != '\0') {
            length++;
        }
    }
    StreamBase<T>::size = length;
    if (copy) {
        data = new T[length+1];
        size_t s = (size_t)(length*sizeof(T));
        memcpy(data, value, s);
        data[length] = 0;
    } else {
        // casting away const is ok, because we don't write anyway
        data = (T*)value;
    }
}
template <class T>
StringReader<T>::~StringReader() {
    if (dataowner) {
        delete [] data;
    }
}
template <class T>
int32_t
StringReader<T>::read(const T*& start, int32_t min, int32_t max) {
    int64_t left = StreamBase<T>::size - StreamBase<T>::position;
    if (left == 0) {
        StreamBase<T>::status = Eof;
        return -1;
    }
    if (min < 0) min = 0;
    int32_t nread = (int32_t)((max > left || max < 1) ?left :max);
    start = data + StreamBase<T>::position;
    StreamBase<T>::position += nread;
    if (StreamBase<T>::position == StreamBase<T>::size) {
        StreamBase<T>::status = Eof;
    }
    return nread;
}
template <class T>
int64_t
StringReader<T>::skip(int64_t ntoskip) {
    const T* start;
    return read(start, ntoskip, ntoskip);
}
template <class T>
int64_t
StringReader<T>::reset(int64_t newpos) {
    if (newpos < 0) {
        StreamBase<T>::status = Ok;
        StreamBase<T>::position = 0;
    } else if (newpos < StreamBase<T>::size) {
        StreamBase<T>::status = Ok;
        StreamBase<T>::position = newpos;
    } else {
        StreamBase<T>::position = StreamBase<T>::size;
        StreamBase<T>::status = Eof;
    }
    return StreamBase<T>::position;
}

} // end namespace jstreams

#endif
