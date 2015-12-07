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
#include "jstreamsconfig.h"
#include "fileinputstream.h"
#include <cerrno>
#include <cstring>
namespace jstreams {

const int32_t FileInputStream::defaultBufferSize = 1048576;
FileInputStream::FileInputStream(const char *filepath, int32_t buffersize) {
    // try to open the file for reading
    file = fopen(filepath, "rb");
    this->filepath = filepath;
    if (file == 0) {
        // handle error
        error = "Could not read file '";
        error += filepath;
        error += "': ";
        error += strerror(errno);
        status = Error;
        return;
    }
    // determine file size. if the stream is not seekable, the size will be -1
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // if the file has size 0, make sure that it's really empty
    // this is useful for filesystems like /proc that report files as size 0
    // for files that do contain content
    if (size == 0) {
        char dummy[1];
        size_t n = fread(dummy, 1, 1, file);
        if (n == 1) {
            size = -1;
            fseek(file, 0, SEEK_SET);
        } else {
            fclose(file);
            file = 0;
            return;
        }
    }

    // allocate memory in the buffer
    int32_t bufsize = (size <= buffersize) ?size+1 :buffersize;
    mark(bufsize);
}
FileInputStream::~FileInputStream() {
    if (file) {
        if (fclose(file)) {
            // handle error
            error = "Could not close file '" + filepath + "'.";
        }
    }
}
int32_t
FileInputStream::fillBuffer(char* start, int32_t space) {
    if (file == 0) return -1;
    // read into the buffer
    int32_t nwritten = fread(start, 1, space, file);
    // check the file stream status
    if (ferror(file)) {
        error = "Could not read from file '" + filepath + "'.";
        fclose(file);
        file = 0;
        status = Error;
        return -1;
    }
    if (feof(file)) {
        fclose(file);
        file = 0;
    }
    return nwritten;
}
}
