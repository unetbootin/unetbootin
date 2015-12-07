/*
 * Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
 *
 * Distributable under the terms of either the Apache License (Version 2.0) or 
 * the GNU Lesser General Public License, as specified in the COPYING file.
 *
 * Changes are Copyright(C) 2007, 2008 by Nokia Corporation and/or its subsidiary(-ies), all rights reserved.
*/
#ifndef _lucene_index_SegmentMerger_
#define _lucene_index_SegmentMerger_

#if defined(_LUCENE_PRAGMA_ONCE)
#   pragma once
#endif

#include <QtCore/QString>
#include <QtCore/QStringList>

#include "CLucene/store/Directory.h"
#include "CLucene/store/RAMDirectory.h"
#include "CLucene/util/VoidList.h"
#include "SegmentMergeInfo.h"
#include "SegmentMergeQueue.h"
#include "IndexWriter.h"
#include "FieldInfos.h"
#include "FieldsWriter.h"
#include "TermInfosWriter.h"

CL_NS_DEF(index)

/**
* The SegmentMerger class combines two or more Segments, represented by an IndexReader ({@link #add},
* into a single Segment.  After adding the appropriate readers, call the merge method to combine the 
* segments.
*<P> 
* If the compoundFile flag is set, then the segments will be merged into a compound file.
*   
* 
* @see #merge
* @see #add
*/
class SegmentMerger : LUCENE_BASE
{
	bool useCompoundFile;
	
	CL_NS(store)::RAMIndexOutput* skipBuffer;
	int32_t lastSkipDoc;
	int64_t lastSkipFreqPointer;
	int64_t lastSkipProxPointer;
	
	void resetSkip();
	void bufferSkip(int32_t doc);
	int64_t writeSkip();

	//Directory of the segment
	CL_NS(store)::Directory* directory;     
	//name of the new segment
	QString segment;
	//Set of IndexReaders
	CL_NS(util)::CLVector<IndexReader*,
        CL_NS(util)::Deletor::Object<IndexReader> > readers;
	//Field Infos for t	he FieldInfo instances of all fields
	FieldInfos* fieldInfos;

	//The queue that holds SegmentMergeInfo instances
	SegmentMergeQueue* queue;
	//IndexOutput to the new Frequency File
	CL_NS(store)::IndexOutput* freqOutput;
  	//IndexOutput to the new Prox File
	CL_NS(store)::IndexOutput* proxOutput;
	//Writes Terminfos that have been merged
	TermInfosWriter* termInfosWriter;
	TermInfo termInfo; //(new) minimize consing

    int32_t termIndexInterval;
	int32_t skipInterval;

public:
	/**
	* 
	* @param dir The Directory to merge the other segments into
	* @param name The name of the new segment
	* @param compoundFile true if the new segment should use a compoundFile
	*/
	SegmentMerger( IndexWriter* writer, const QString& name );

	//Destructor
	~SegmentMerger();
	
	/**
	* Add an IndexReader to the collection of readers that are to be merged
	* @param reader
	*/
	void add(IndexReader* reader);
	
	/**
	* 
	* @param i The index of the reader to return
	* @return The ith reader to be merged
	*/
	IndexReader* segmentReader(const int32_t i);
	
	/**
	* Merges the readers specified by the {@link #add} method into the
    * directory passed to the constructor
	* @return The number of documents that were merged
	* @throws IOException
	*/
	int32_t merge();
	/**
	* close all IndexReaders that have been added.
	* Should not be called before merge().
	* @throws IOException
	*/
	void closeReaders();
private:
	void addIndexed(IndexReader* reader, FieldInfos* fieldInfos,
        CL_NS(util)::StringArrayWithDeletor& names, 
		bool storeTermVectors, bool storePositionWithTermVector,
		bool storeOffsetWithTermVector);

	/**
	* Merge the fields of all segments 
	* @return The number of documents in all of the readers
	* @throws IOException
	*/
	int32_t mergeFields();

	/**
	* Merge the TermVectors from each of the segments into the new one.
	* @throws IOException
	*/
  	void mergeVectors();

	/** Merge the terms of all segments */
	void mergeTerms();

	/** Merges all TermInfos into a single segment */
	void mergeTermInfos();

	/** Merge one term found in one or more segments. The array <code>smis</code>
	*  contains segments that are positioned at the same term. <code>N</code>
	*  is the number of cells in the array actually occupied.
	*
	* @param smis array of segments
	* @param n number of cells in the array actually occupied
	*/
	void mergeTermInfo( SegmentMergeInfo** smis);
	    
	/** Process postings from multiple segments all positioned on the
	*  same term. Writes out merged entries into freqOutput and
	*  the proxOutput streams.
	*
	* @param smis array of segments
	* @param n number of cells in the array actually occupied
	* @return number of documents across all segments where this term was found
	*/
	int32_t appendPostings(SegmentMergeInfo** smis);

	//Merges the norms for all fields 
	void mergeNorms();
	
	void createCompoundFile(const QString& filename, QStringList& files);
	friend class IndexWriter; //allow IndexWriter to use createCompoundFile
};

CL_NS_END

#endif
