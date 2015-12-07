/**
 * Copyright 2003-2006 The Apache Software Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef FILEINPUTSTREAM_H
#define FILEINPUTSTREAM_H

#include "bufferedstream.h"

namespace jstreams {

class FileInputStream : public BufferedInputStream<char> {
private:
    FILE *file;
    std::string filepath;

public:
    static const int32_t defaultBufferSize;
    FileInputStream(const char *filepath, int32_t buffersize=defaultBufferSize);
    ~FileInputStream();
    int32_t fillBuffer(char* start, int32_t space);
};

} // end namespace jstreams

#endif

