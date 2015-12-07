/*
 * Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
 *
 * Distributable under the terms of either the Apache License (Version 2.0) or 
 * the GNU Lesser General Public License, as specified in the COPYING file.
 *
 * Changes are Copyright(C) 2007, 2008 by Nokia Corporation and/or its subsidiary(-ies), all rights reserved.
*/
#ifndef _lucene_index_TermInfosReader_
#define _lucene_index_TermInfosReader_

#if defined(_LUCENE_PRAGMA_ONCE)
#   pragma once
#endif

#include <QtCore/QString>

#include "CLucene/store/Directory.h"
#include "CLucene/util/ThreadLocal.h"
#include "SegmentTermEnum.h"

CL_NS_DEF(index)

class FieldInfos;
class Term;
class TermInfo;
class TermInfos;
class TermInfosWriter;

// PORT STATUS: 365707 (jlucene 1.9)
// This stores a monotonically increasing set of <Term, TermInfo> pairs in a
// Directory. Pairs are accessed either by Term or by ordinal position the set.  
class TermInfosReader : LUCENE_BASE
{
private:
    CL_NS(store)::Directory* directory;
    QString segment;
    FieldInfos* fieldInfos;

    CL_NS(util)::ThreadLocal<SegmentTermEnum*, 
        CL_NS(util)::Deletor::Object<SegmentTermEnum> > enumerators;

    SegmentTermEnum* getEnum();
    SegmentTermEnum* origEnum;
    SegmentTermEnum* indexEnum;
    int64_t _size;

    Term* indexTerms;
    int32_t indexTermsLength;
    TermInfo* indexInfos;
    int64_t* indexPointers;

    DEFINE_MUTEX(THIS_LOCK)

public:
    // Reads the TermInfos file(.tis) and eventually the Term Info Index(.tii)
    TermInfosReader(CL_NS(store)::Directory* dir, const QString& segment,
        FieldInfos* fis);
    ~TermInfosReader();

    //Close the enumeration of TermInfos
    void close();

    //Return the size of the enumeration of TermInfos
    int64_t size() const;

    int32_t getSkipInterval() {
        return origEnum->skipInterval; }

    // Returns an enumeration of terms starting at or after the named term. 
    // If no term is specified, an enumeration of all the Terms 
    // and TermInfos in the set is returned.
    SegmentTermEnum* terms(const Term* term = NULL);

    // Returns the TermInfo for a Term in the set
    // synchronized
    TermInfo* get(const Term* term);

private:
    // Reads the term info index file or .tti file.
    void ensureIndexIsRead();

    // Returns the offset of the greatest index entry which is less than term.
    int32_t getIndexOffset(const Term* term);

    // Reposition the current Term and TermInfo to indexOffset
    void seekEnum(const int32_t indexOffset);  

    // Scans the Enumeration of terms for term and returns the corresponding
    // TermInfo instance if found. The search is started from the current term.
    TermInfo* scanEnum(const Term* term);

    // Scans the enumeration to the requested position and returns the Term
    // located at that position
    Term* scanEnum(const int32_t position);

    // Returns the position of a Term in the set. synchronized 
    int64_t getPosition(const Term* term);

    // Returns the nth term in the set. synchronized
    Term* get(const int32_t position);
};

CL_NS_END

#endif
