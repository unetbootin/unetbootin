/*
 * Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
 *
 * Distributable under the terms of either the Apache License (Version 2.0) or 
 * the GNU Lesser General Public License, as specified in the COPYING file.
 *
 * Changes are Copyright(C) 2007, 2008 by Nokia Corporation and/or its subsidiary(-ies), all rights reserved.
*/
#ifndef _lucene_index_FieldsReader_
#define _lucene_index_FieldsReader_

#if defined(_LUCENE_PRAGMA_ONCE)
#   pragma once
#endif

#include <QtCore/QString>

#include "CLucene/store/Directory.h"
#include "CLucene/document/Document.h"
#include "CLucene/util/subinputstream.h"
#include "CLucene/store/IndexInput.h"

CL_NS_DEF(index)

class FieldInfos;

class FieldsReader : LUCENE_BASE
{
private:
    const FieldInfos* fieldInfos;
    CL_NS(store)::IndexInput* fieldsStream;
    CL_NS(store)::IndexInput* indexStream;
    int32_t _size;

    class FieldsStreamHolder : public jstreams::StreamBase<char>
    {
        CL_NS(store)::IndexInput* indexInput;
        CL_NS(store)::IndexInputStream* indexInputStream;
        jstreams::SubInputStream<char>* subStream;
    
    public:
        FieldsStreamHolder(CL_NS(store)::IndexInput* indexInput, int32_t subLength);
        ~FieldsStreamHolder();
        int32_t read(const char*& start, int32_t _min, int32_t _max);
        int64_t skip(int64_t ntoskip);
        int64_t reset(int64_t pos);
    };

public:
    FieldsReader(CL_NS(store)::Directory* d, const QString& segment, FieldInfos* fn);
    ~FieldsReader();
    void close();
    int32_t size() const;
    // loads the fields from n'th document into doc. returns true on success.
    bool doc(int32_t n, CL_NS(document)::Document* doc);
};

CL_NS_END

#endif
