/*
 * Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
 *
 * Distributable under the terms of either the Apache License (Version 2.0) or 
 * the GNU Lesser General Public License, as specified in the COPYING file.
 *
 * Changes are Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
*/
#ifndef _lucene_index_SegmentInfos_
#define _lucene_index_SegmentInfos_

#if defined(_LUCENE_PRAGMA_ONCE)
#   pragma once
#endif

#include <QtCore/QString>
#include <QtCore/QVector>

#include "CLucene/store/Directory.h"

CL_NS_DEF(index)

class SegmentInfo : LUCENE_BASE
{
public:
    SegmentInfo(const QString& Name, const int32_t DocCount,
        CL_NS(store)::Directory* Dir);
    ~SegmentInfo();

    ///Gets the Directory where the segment resides
    CL_NS(store)::Directory* getDir() const { return dir; }

    //Unique name in directory dir
    QString name;
    
    //Number of docs in the segment
    const int32_t docCount;

private:
    //Directory where the segment resides
    CL_NS(store)::Directory* dir;		
};

typedef QVector<SegmentInfo*> segmentInfosType;

//SegmentInfos manages a list of SegmentInfo instances
//Each SegmentInfo contains information about a segment in a directory.
//
//The active segments in the index are stored in the segment info file. 
//An index only has a single file in this format, and it is named "segments". 
//This lists each segment by name, and also contains the size of each segment.
//The format of the file segments is defined as follows:
//
//                                        SegCount
//Segments --> SegCount, <SegName, SegSize>
//
//SegCount, SegSize --> UInt32
//
//SegName --> String
//
//SegName is the name of the segment, and is used as the file name prefix 
//for all of the files that compose the segment's index.
//
//SegSize is the number of documents contained in the segment index. 
//
//Note:
//At http://jakarta.apache.org/lucene/docs/fileformats.html the definition
//of all file formats can be found. Note that java lucene currently 
//defines Segments as follows:
//
//Segments --> Format, Version, SegCount, <SegName, SegSize>SegCount
//        
//Format, SegCount, SegSize --> UInt32        
//      
//Format and Version have not been implemented yet
class SegmentInfos : LUCENE_BASE
{
public:
    SegmentInfos(bool deleteMembers = true);
    ~SegmentInfos();

    //delete and clears objects 'from' from to 'to'
    void clearto(size_t to);

    //count of segment infos
    int32_t size() const;

    //add a segment info
    void add(SegmentInfo* info);

    //Returns a reference to the i-th SegmentInfo in the list.
    SegmentInfo* info(int32_t i) const;

    // version number when this SegmentInfos was generated.
    int64_t getVersion() const { return version; }

    static int64_t readCurrentVersion(CL_NS(store)::Directory* directory);

    //Reads segments file that resides in directory
    void read(CL_NS(store)::Directory* directory);

    //Writes a new segments file based upon the SegmentInfo instances it manages
    void write(CL_NS(store)::Directory* directory);

private:
    // The file format version, a negative number.
    // Works since counter, the old 1st entry, is always >= 0
    LUCENE_STATIC_CONSTANT(int32_t, FORMAT = -1);

    // counts how often the index has been changed by adding or deleting docs.
    // starting with the current time in milliseconds forces to create unique
    // version numbers.
    int64_t version;

    segmentInfosType infos;

    // used to name new segments
    int32_t counter;

    // allow IndexWriter to use counter
    friend class IndexWriter; 

    bool deleteMembers;
};

CL_NS_END

#endif
