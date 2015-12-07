/*
 * Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
 *
 * Distributable under the terms of either the Apache License (Version 2.0) or 
 * the GNU Lesser General Public License, as specified in the COPYING file.
 *
 * Changes are Copyright(C) 2007, 2008 by Nokia Corporation and/or its subsidiary(-ies), all rights reserved.
*/
#ifndef _lucene_index_IndexWriter_
#define _lucene_index_IndexWriter_

#if defined(_LUCENE_PRAGMA_ONCE)
#   pragma once
#endif

#include <QtCore/QString>
#include <QtCore/QStringList>

#include "CLucene/analysis/AnalysisHeader.h"
#include "CLucene/util/VoidList.h"
#include "CLucene/search/Similarity.h"
#include "CLucene/store/Lock.h"
#include "CLucene/store/TransactionalRAMDirectory.h"

#include "SegmentHeader.h"

CL_NS_DEF(index)

/**
An IndexWriter creates and maintains an index.

The third argument to the 
<a href="#IndexWriter(org.apache.lucene.store.Directory, org.apache.lucene.analysis.Analyzer, boolean)"><b>constructor</b></a>
determines whether a new index is created, or whether an existing index is
opened for the addition of new documents.

In either case, documents are added with the <a
href="#addDocument(org.apache.lucene.document.Document)"><b>addDocument</b></a> method.  
When finished adding documents, <a href="#close()"><b>close</b></a> should be called.

<p>If an index will not have more documents added for a while and optimal search
performance is desired, then the <a href="#optimize()"><b>optimize</b></a>
method should be called before the index is closed.

<p>Opening an IndexWriter creates a lock file for the directory in use. Trying to open
another IndexWriter on the same directory will lead to an IOException. The IOException
is also thrown if an IndexReader on the same directory is used to delete documents
from the index.

@see IndexModifier IndexModifier supports the important methods of IndexWriter plus deletion
*/
class IndexWriter : LUCENE_BASE
{
	class LockWith2 : public CL_NS(store)::LuceneLockWith<void>
    {
	public:
		LockWith2(CL_NS(store)::LuceneLock* lock,
                  int64_t lockWaitTimeout,
                  IndexWriter* wr,
                  CL_NS(util)::CLVector<SegmentReader*>* std,
                  bool create);

        ~LockWith2() {}

		void doBody();

    private:
        bool create;
        IndexWriter* writer;
		CL_NS(util)::CLVector<SegmentReader*>* segmentsToDelete;
	};
	friend class LockWith2;

	class LockWithCFS : public CL_NS(store)::LuceneLockWith<void>
    {
	public:
		LockWithCFS(CL_NS(store)::LuceneLock* lock,
                    int64_t lockWaitTimeout,
                    CL_NS(store)::Directory* dir,
                    IndexWriter* wr,
                    const QString& segName,
                    const QStringList& ftd);
		
        ~LockWithCFS() {}

        void doBody();

    private:
		QString segName;        
        IndexWriter* writer;
		CL_NS(store)::Directory* directory;
		QStringList filesToDelete;
	};
    friend class IndexWriter::LockWithCFS;

    // indicates if the writers is open - this way close can be called multiple
    // times
    bool isOpen;

	// how to analyze text
	CL_NS(analysis)::Analyzer* analyzer;

	CL_NS(search)::Similarity* similarity; // how to normalize

	/** Use compound file setting. Normally defaults to true, except when
	* using a RAMDirectory. This minimizes the number of files used.  
	* Setting this to false may improve indexing performance, but
	* may also cause file handle problems.
	*/
	bool useCompoundFile;
	bool closeDir;

    // for temp segs
	CL_NS(store)::TransactionalRAMDirectory* ramDirectory;

	CL_NS(store)::LuceneLock* writeLock;

	void _IndexWriter(const bool create);

	void _finalize();

	// where this index resides
	CL_NS(store)::Directory* directory;		
		
		
	int32_t getSegmentsCounter() { return segmentInfos.counter; }
	int32_t maxFieldLength;
	int32_t mergeFactor;
	int32_t minMergeDocs;
	int32_t maxMergeDocs;
	int32_t termIndexInterval;

	int64_t writeLockTimeout;
	int64_t commitLockTimeout;
public:
	DEFINE_MUTEX(THIS_LOCK)
	
	// Release the write lock, if needed. 
	SegmentInfos segmentInfos;
  
	// Release the write lock, if needed.
	~IndexWriter();

	/**
	*  The Java implementation of Lucene silently truncates any tokenized
	*  field if the number of tokens exceeds a certain threshold.  Although
	*  that threshold is adjustable, it is easy for the client programmer
	*  to be unaware that such a threshold exists, and to become its
	*  unwitting victim.
	*  CLucene implements a less insidious truncation policy.  Up to
	*  DEFAULT_MAX_FIELD_LENGTH tokens, CLucene behaves just as JLucene
	*  does.  If the number of tokens exceeds that threshold without any
	*  indication of a truncation preference by the client programmer,
	*  CLucene raises an exception, prompting the client programmer to
	*  explicitly set a truncation policy by adjusting maxFieldLength.
	*/
	LUCENE_STATIC_CONSTANT(int32_t, DEFAULT_MAX_FIELD_LENGTH = 10000);
	LUCENE_STATIC_CONSTANT(int32_t, FIELD_TRUNC_POLICY__WARN = -1);
	int32_t getMaxFieldLength() const{ return maxFieldLength; }
	void setMaxFieldLength(int32_t val){ maxFieldLength = val; }

	/**
	* Default value is 10. Change using {@link #setMaxBufferedDocs(int)}.
	*/
	LUCENE_STATIC_CONSTANT(int32_t, DEFAULT_MAX_BUFFERED_DOCS = 10);
	/** Determines the minimal number of documents required before the buffered
	* in-memory documents are merging and a new Segment is created.
	* Since Documents are merged in a {@link RAMDirectory},
	* large value gives faster indexing.  At the same time, mergeFactor limits
	* the number of files open in a FSDirectory.
	*
	* <p> The default value is DEFAULT_MAX_BUFFERED_DOCS.*/
	void setMaxBufferedDocs(int32_t val){ minMergeDocs = val; }
	/**
	* @see #setMaxBufferedDocs
	*/
	int32_t getMaxBufferedDocs(){ return minMergeDocs; }
	
	/**
	* Default value for the write lock timeout (1,000).
	*/
	LUCENE_STATIC_CONSTANT(int64_t, WRITE_LOCK_TIMEOUT = 1000);
	/**
	* Sets the maximum time to wait for a write lock (in milliseconds).
	*/
	void setWriteLockTimeout(int64_t writeLockTimeout)
    { this->writeLockTimeout = writeLockTimeout; }
	/**
	* @see #setWriteLockTimeout
	*/
	int64_t getWriteLockTimeout() { return writeLockTimeout; }
	
	/**
	* Default value for the commit lock timeout (10,000).
	*/
	LUCENE_STATIC_CONSTANT(int64_t, COMMIT_LOCK_TIMEOUT = 10000);
	/**
	* Sets the maximum time to wait for a commit lock (in milliseconds).
	*/
	void setCommitLockTimeout(int64_t commitLockTimeout)
    { this->commitLockTimeout = commitLockTimeout; }
	/**
	* @see #setCommitLockTimeout
	*/
	int64_t getCommitLockTimeout() { return commitLockTimeout; }

	static const QLatin1String WRITE_LOCK_NAME; //"write.lock";
	static const QLatin1String COMMIT_LOCK_NAME; //"commit.lock";
	
	/**
	* Default value is 10. Change using {@link #setMergeFactor(int)}.
	*/
	LUCENE_STATIC_CONSTANT(int32_t, DEFAULT_MERGE_FACTOR = 10);
	/* Determines how often segment indices are merged by addDocument().  With
	*  smaller values, less RAM is used while indexing, and searches on
	*  unoptimized indices are faster, but indexing speed is slower.  With larger
	*  values more RAM is used while indexing and searches on unoptimized indices
	*  are slower, but indexing is faster.  Thus larger values (> 10) are best
	*  for batched index creation, and smaller values (< 10) for indices that are
	*  interactively maintained.
	*
	* <p>This must never be less than 2.  The default value is 10.
	*/
	int32_t getMergeFactor() const{ return mergeFactor; }
	void setMergeFactor(int32_t val){ mergeFactor = val; }

	
	/** Expert: The fraction of terms in the "dictionary" which should be stored
	*   in RAM.  Smaller values use more memory, but make searching slightly
	*   faster, while larger values use less memory and make searching slightly
	*   slower.  Searching is typically not dominated by dictionary lookup, so
	*   tweaking this is rarely useful.
	*/
	LUCENE_STATIC_CONSTANT(int32_t, DEFAULT_TERM_INDEX_INTERVAL = 128);
	/** Expert: Set the interval between indexed terms.  Large values cause less
	* memory to be used by IndexReader, but slow random-access to terms.  Small
	* values cause more memory to be used by an IndexReader, and speed
	* random-access to terms.
	*
	* This parameter determines the amount of computation required per query
	* term, regardless of the number of documents that contain that term.  In
	* particular, it is the maximum number of other terms that must be
	* scanned before a term is located and its frequency and position information
	* may be processed.  In a large index with user-entered query terms, query
	* processing time is likely to be dominated not by term lookup but rather
	* by the processing of frequency and positional data.  In a small index
	* or when many uncommon query terms are generated (e.g., by wildcard
	* queries) term lookup may become a dominant cost.
	*
	* In particular, <code>numUniqueTerms/interval</code> terms are read into
	* memory by an IndexReader, and, on average, <code>interval/2</code> terms
	* must be scanned for each random term access.
	*
	* @see #DEFAULT_TERM_INDEX_INTERVAL
	*/
	void setTermIndexInterval(int32_t interval) { termIndexInterval = interval; }
	/** Expert: Return the interval between indexed terms.
	*
	* @see #setTermIndexInterval(int)
	*/
	int32_t getTermIndexInterval() { return termIndexInterval; }
  
	/** Determines the minimal number of documents required before the buffered
	* in-memory documents are merging and a new Segment is created.
	* Since Documents are merged in a {@link RAMDirectory},
	* large value gives faster indexing.  At the same time, mergeFactor limits
	* the number of files open in a FSDirectory.
	*
	* <p> The default value is 10.*/
	int32_t getMinMergeDocs() const{ return minMergeDocs; }
	void setMinMergeDocs(int32_t val){ minMergeDocs = val; }

	/** Determines the largest number of documents ever merged by addDocument().
	* Small values (e.g., less than 10,000) are best for interactive indexing,
	* as this limits the length of pauses while indexing to a few seconds.
	* Larger values are best for batched indexing and speedier searches.
	*
	* <p>The default value is {@link #DEFAULT_MAX_MERGE_DOCS}.
	*/
	LUCENE_STATIC_CONSTANT(int32_t, DEFAULT_MAX_MERGE_DOCS = LUCENE_INT32_MAX_SHOULDBE);
	/**Determines the largest number of documents ever merged by addDocument().
	*  Small values (e.g., less than 10,000) are best for interactive indexing,
	*  as this limits the length of pauses while indexing to a few seconds.
	*  Larger values are best for batched indexing and speedier searches.
	*
	*  <p>The default value is {@link Integer#MAX_VALUE}.
	*/
	int32_t getMaxMergeDocs() const{ return maxMergeDocs; }
	void setMaxMergeDocs(int32_t val){ maxMergeDocs = val; }

	/**
	* Constructs an IndexWriter for the index in <code>path</code>.
	* Text will be analyzed with <code>a</code>.  If <code>create</code>
	* is true, then a new, empty index will be created in
	* <code>path</code>, replacing the index already there, if any.
	*
	* @param path the path to the index directory
	* @param a the analyzer to use
	* @param create <code>true</code> to create the index or overwrite
	*  the existing one; <code>false</code> to append to the existing
	*  index
	* @throws IOException if the directory cannot be read/written to, or
	*  if it does not exist, and <code>create</code> is
	*  <code>false</code>
	*/
	IndexWriter(const QString& path, CL_NS(analysis)::Analyzer* a,
        const bool create, const bool closeDir = true);
	
	
	/**Constructs an IndexWriter for the index in <code>d</code>.  Text will be
	*  analyzed with <code>a</code>.  If <code>create</code> is true, then a new,
	*  empty index will be created in <code>d</code>, replacing the index already
	*  there, if any.
	*/
	IndexWriter(CL_NS(store)::Directory* d, CL_NS(analysis)::Analyzer* a,
        const bool create, const bool closeDir = false);

    // Flushes all changes to an index, closes all associated files, and closes
    // the directory that the index is stored in.
	void close();

	// Returns the number of documents currently in this index. synchronized
	int32_t docCount();


    // Adds a document to this index, using the provided analyzer instead of
    // the value of {@link #getAnalyzer()}.  If the document contains more than
    // {@link #setMaxFieldLength(int)} terms for a given field, the remainder
    // are discarded.
	void addDocument(CL_NS(document)::Document* doc,
        CL_NS(analysis)::Analyzer* analyzer = NULL);
  

	// Merges all segments together into a single segment, optimizing an index
	// for search. synchronized
	void optimize();


	/**Merges all segments from an array of indices into this index.
	*  
	*  <p>This may be used to parallelize batch indexing.  A large document
	*  collection can be broken into sub-collections.  Each sub-collection can be
	*  indexed in parallel, on a different thread, process or machine.  The
	*  complete index can then be created by merging sub-collection indices
	*  with this method.
	*
	*  <p>After this completes, the index is optimized.
	*@synchronized
	*/
	void addIndexes(CL_NS(store)::Directory** dirs);
		
	/** Merges the provided indexes into this index.
	* <p>After this completes, the index is optimized. </p>
	* <p>The provided IndexReaders are not closed.</p>
	*/
	void addIndexes(IndexReader** readers);


	/** Returns the directory this index resides in. */
	CL_NS(store)::Directory* getDirectory() { return directory; }

	/** Get the current setting of whether to use the compound file format.
	*  Note that this just returns the value you set with setUseCompoundFile(boolean)
	*  or the default. You cannot use this to query the status of an existing index.
	*  @see #setUseCompoundFile(boolean)
	*/
	bool getUseCompoundFile() { return useCompoundFile; }

	/** Setting to turn on usage of a compound file. When on, multiple files
	*  for each segment are merged into a single file once the segment creation
	*  is finished. This is done regardless of what directory is in use.
	*/
	void setUseCompoundFile(bool value) { useCompoundFile = value; }


	/** Expert: Set the Similarity implementation used by this IndexWriter.
	*
	* @see Similarity#setDefault(Similarity)
	*/
	void setSimilarity(CL_NS(search)::Similarity* similarity)
    { this->similarity = similarity; }

	/** Expert: Return the Similarity implementation used by this IndexWriter.
	*
	* <p>This defaults to the current value of {@link Similarity#getDefault()}.
	*/
	CL_NS(search)::Similarity* getSimilarity() { return this->similarity; }

	/** Returns the analyzer used by this index. */
	CL_NS(analysis)::Analyzer* getAnalyzer() { return analyzer; }

private:
	/** Merges all RAM-resident segments. */
	void flushRamSegments();

	/** Incremental segment merger. */
	void maybeMergeSegments();

	// Pops segments off of segmentInfos stack down to minSegment, merges them,
	// and pushes the merged index onto the top of the segmentInfos stack.
	void mergeSegments(const uint32_t minSegment);
	
	// Merges the named range of segments, replacing them in the stack with a
	// single segment.
	void mergeSegments(const uint32_t minSegment, const uint32_t end);

    // Some operating systems (e.g. Windows) don't permit a file to be deleted
    // while it is opened for read (e.g. by another process or thread). So we
    // assume that when a delete fails it is because the file is open in another
    // process, and queue the file for subsequent deletion.
	void deleteSegments(CL_NS(util)::CLVector<SegmentReader*>* segments);

	void deleteFiles(const QStringList& files);
	void readDeleteableFiles(QStringList& files);
	void deleteFiles(const QStringList& files, QStringList& deletable);
    void deleteFiles(const QStringList& files, CL_NS(store)::Directory* directory);
	void writeDeleteableFiles(const QStringList& files);

	// synchronized
	QString newSegmentName();
};

CL_NS_END

#endif
