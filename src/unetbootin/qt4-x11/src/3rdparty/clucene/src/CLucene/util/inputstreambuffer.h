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
#ifndef INPUTSTREAMBUFFER_H
#define INPUTSTREAMBUFFER_H

#include <cstdlib>

namespace jstreams {

template <class T>
class InputStreamBuffer {
private:
public:
    T* start;
    int32_t size;
    T* readPos;
    int32_t avail;

    InputStreamBuffer();
    ~InputStreamBuffer();
    void setSize(int32_t size);
    int32_t read(const T*& start, int32_t max=0);

    /**
     * This function prepares the buffer for a new write.
     * returns the number of available places.
     **/
     int32_t makeSpace(int32_t needed);
};

template <class T>
InputStreamBuffer<T>::InputStreamBuffer() {
    readPos = start = 0;
    size = avail = 0;
}
template <class T>
InputStreamBuffer<T>::~InputStreamBuffer() {
    free(start);
}
template <class T>
void
InputStreamBuffer<T>::setSize(int32_t size) {
    // store pointer information
    int32_t offset = (int32_t)(readPos - start);

	// allocate memory in the buffer
    if ( start == 0 )
		start = (T*)malloc(size*sizeof(T));
	else
		start = (T*)realloc(start, size*sizeof(T));
    this->size = size;

    // restore pointer information
    readPos = start + offset;
}
template <class T>
int32_t
InputStreamBuffer<T>::makeSpace(int32_t needed) {
    // determine how much space is available for writing
    int32_t space = size - ((int32_t)(readPos - start)) - avail;
    if (space >= needed) {
        // there's enough space
        return space;
    }

    if (avail) {
        if (readPos != start) {
//            printf("moving\n");
            // move data to the start of the buffer
            memmove(start, readPos, avail*sizeof(T));
            space += (int32_t)(readPos - start);
            readPos = start;
        }
    } else {
        // we may start writing at the start of the buffer
        readPos = start;
        space = size;
    }
    if (space >= needed) {
        // there's enough space now
        return space;
    }

    // still not enough space, we have to allocate more
//    printf("resize %i %i %i %i %i\n", avail, needed, space, size + needed - space, size);
    setSize(size + needed - space);
    return needed;
}
template <class T>
int32_t
InputStreamBuffer<T>::read(const T*& start, int32_t max) {
    start = readPos;
    if (max <= 0 || max > avail) {
        max = avail;
    }
    readPos += max;
    avail -= max;
    return max;
}

} // end namespace jstreams

#endif
