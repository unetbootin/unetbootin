/*
 * Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
 *
 * Distributable under the terms of either the Apache License (Version 2.0) or 
 * the GNU Lesser General Public License, as specified in the COPYING file.
 *
 * Changes are Copyright(C) 2007, 2008 by Nokia Corporation and/or its subsidiary(-ies), all rights reserved.
*/
#ifndef _lucene_index_IndexReader_
#define _lucene_index_IndexReader_

#if defined(_LUCENE_PRAGMA_ONCE)
#   pragma once
#endif

#include <QtCore/QString>

#include "CLucene/store/Directory.h"
#include "CLucene/store/FSDirectory.h"
#include "CLucene/store/Lock.h"
#include "CLucene/document/Document.h"
#include "CLucene/index/TermVector.h"
#include "SegmentInfos.h"
#include "Terms.h"


CL_NS_DEF(index)


/** IndexReader is an abstract class, providing an interface for accessing an
 index.  Search of an index is done entirely through this abstract interface,
 so that any subclass which implements it is searchable.

 <p> Concrete subclasses of IndexReader are usually constructed with a call to
 one of the static <code>open()</code> methods, e.g. {@link #open(String)}.

 <p> For efficiency, in this API documents are often referred to via
 <i>document numbers</i>, non-negative integers which each name a unique
 document in the index.  These document numbers are ephemeral--they may change
 as documents are added to and deleted from an index.  Clients should thus not
 rely on a given document having the same number between sessions.
 
 <p> An IndexReader can be opened on a directory for which an IndexWriter is
 opened already, but it cannot be used to delete documents from the index then.
*/
class IndexReader : LUCENE_BASE
{
public:
	//Callback for classes that need to know if IndexReader is closing.
	typedef void (*CloseCallback)(IndexReader*, void*);

	class CloseCallbackCompare:public CL_NS(util)::Compare::_base{
	public:
		bool operator()( CloseCallback t1, CloseCallback t2 ) const{
			return t1 > t2;
		}
		static void doDelete(CloseCallback dummy){
		}
	};
	
	
	enum FieldOption {
		// all fields
		ALL = 1,
		// all indexed fields
		INDEXED = 2,
		// all fields which are not indexed
		UNINDEXED = 4,
		// all fields which are indexed with termvectors enables
		INDEXED_WITH_TERMVECTOR = 8,
		// all fields which are indexed but don't have termvectors enabled
		INDEXED_NO_TERMVECTOR = 16,
		// all fields where termvectors are enabled. Please note that only standard termvector fields are returned
		TERMVECTOR = 32,
		// all field with termvectors wiht positions enabled
		TERMVECTOR_WITH_POSITION = 64,
		// all fields where termvectors with offset position are set
		TERMVECTOR_WITH_OFFSET = 128,
		// all fields where termvectors with offset and position values set
		TERMVECTOR_WITH_POSITION_OFFSET = 256
	};


private:
    bool stale;
    bool hasChanges;
    bool closeDirectory;
    bool directoryOwner;
    
    SegmentInfos* segmentInfos;
    CL_NS(store)::Directory* directory;
	CL_NS(store)::LuceneLock* writeLock;

    typedef CL_NS(util)::CLSet<CloseCallback, void*, CloseCallbackCompare,
		CloseCallbackCompare> CloseCallbackMap;
    CloseCallbackMap closeCallbacks;
	
    /** Internal use. Implements commit */
    virtual void doCommit() = 0;
  
    /**
	* Tries to acquire the WriteLock on this directory.
	* this method is only valid if this IndexReader is directory owner.
	* 
	* @throws IOException If WriteLock cannot be acquired.
	*/
    void aquireWriteLock();
protected:
    /**
    * Constructor used if IndexReader is not owner of its directory. 
    * This is used for IndexReaders that are used within other IndexReaders that take care or locking directories.
    * 
    * @param directory Directory where IndexReader files reside.
    */
    IndexReader(CL_NS(store)::Directory* dir);

    /**
    * Constructor used if IndexReader is owner of its directory.
    * If IndexReader is owner of its directory, it locks its directory in case of write operations.
    * 
    * @param directory Directory where IndexReader files reside.
    * @param segmentInfos Used for write-l
    * @param closeDirectory
    */
	IndexReader(CL_NS(store)::Directory* directory, SegmentInfos* segmentInfos, bool closeDirectory);
	

	/// Implements close. 
	virtual void doClose() = 0;

    /** Implements setNorm in subclass.*/
    virtual void doSetNorm(int32_t doc, const TCHAR* field, uint8_t value) = 0;
      
    /** Implements actual undeleteAll() in subclass. */
    virtual void doUndeleteAll() = 0;


    /** Implements deletion of the document numbered <code>docNum</code>.
	* Applications should call {@link #deleteDocument(int32_t)} or {@link #deleteDocuments(Term*)}.
	*/
    virtual void doDelete(const int32_t docNum) = 0;

public:

	DEFINE_MUTEX(THIS_LOCK)
	
	///Do not access this directly, only public so that MultiReader can access it
	virtual void commit();
	
	
	/** Undeletes all documents currently marked as deleted in this index.*/
	void undeleteAll();

	/**
	* Get a list of unique field names that exist in this index and have the specified
	* field option information.
	* @param fldOption specifies which field option should be available for the returned fields
	* @return Collection of Strings indicating the names of the fields.
	* @see IndexReader.FieldOption
	*/
	virtual void getFieldNames(FieldOption fldOption, CL_NS(util)::StringArrayWithDeletor& retarray) = 0;

	_CL_DEPRECATED( getFieldNames(FieldOption, StringArrayWithDeletor&) ) virtual TCHAR** getFieldNames();
	_CL_DEPRECATED( getFieldNames(FieldOption, StringArrayWithDeletor&) ) virtual TCHAR** getFieldNames(bool indexed);

	/** Returns the byte-encoded normalization factor for the named field of
	* every document.  This is used by the search code to score documents.
	*
	* The number of bytes returned is the size of the IndexReader->maxDoc()
	* MEMORY: The values are cached, so don't delete the returned byte array.
	* @see Field#setBoost(qreal)
	*/
	virtual uint8_t* norms(const TCHAR* field) = 0;
	
	
	/** Reads the byte-encoded normalization factor for the named field of every
	*  document.  This is used by the search code to score documents.
	*
	* @see Field#setBoost(qreal)
	*/
	virtual void norms(const TCHAR* field, uint8_t* bytes) = 0;

    /** Expert: Resets the normalization factor for the named field of the named
    * document.
    *
    * @see #norms(TCHAR*)
    * @see Similarity#decodeNorm(uint8_t)
    */
    void setNorm(int32_t doc, const TCHAR* field, qreal value);
  
    /** Expert: Resets the normalization factor for the named field of the named
    * document.  The norm represents the product of the field's {@link
    * Field#setBoost(qreal) boost} and its {@link Similarity#lengthNorm(TCHAR*,
    * int32_t) length normalization}.  Thus, to preserve the length normalization
    * values when resetting this, one should base the new value upon the old.
    *
    * @see #norms(TCHAR*)
    * @see Similarity#decodeNorm(uint8_t)
    */
    void setNorm(int32_t doc, const TCHAR* field, uint8_t value);

	/// Release the write lock, if needed. 
    virtual ~IndexReader();

	/// Returns an IndexReader reading the index in an FSDirectory in the named path. 
	static IndexReader* open(const QString& path);

	/// Returns an IndexReader reading the index in the given Directory. 
	static IndexReader* open( CL_NS(store)::Directory* directory, bool closeDirectory=false);

	/** 
	* Returns the time the index in the named directory was last modified.
	* Do not use this to check whether the reader is still up-to-date, use
	* {@link #isCurrent()} instead. 
	*/
	static uint64_t lastModified(const QString& directory);

	/** 
	* Returns the time the index in the named directory was last modified. 
	* Do not use this to check whether the reader is still up-to-date, use
	* {@link #isCurrent()} instead. 
	*/
	static uint64_t lastModified(const CL_NS(store)::Directory* directory);

	
	/**
	* Reads version number from segments files. The version number is
	* initialized with a timestamp and then increased by one for each change of
	* the index.
	* 
	* @param directory where the index resides.
	* @return version number.
	* @throws IOException if segments file cannot be read
	*/
	static int64_t getCurrentVersion(CL_NS(store)::Directory* directory);
	
	/**
   * Reads version number from segments files. The version number is
   * initialized with a timestamp and then increased by one for each change of
   * the index.
   * 
   * @param directory where the index resides.
   * @return version number.
   * @throws IOException if segments file cannot be read
   */
	static int64_t getCurrentVersion(const QString& directory);
	
	/**
	* Version number when this IndexReader was opened.
	*/
	int64_t getVersion();
	
	/**
	* Check whether this IndexReader still works on a current version of the index.
	* If this is not the case you will need to re-open the IndexReader to
	* make sure you see the latest changes made to the index.
	* 
	* @throws IOException
	*/
	bool isCurrent();


	/**
	*  Return an array of term frequency vectors for the specified document.
	*  The array contains a vector for each vectorized field in the document.
	*  Each vector contains terms and frequencies for all terms in a given vectorized field.
	*  If no such fields existed, the method returns null. The term vectors that are
	* returned my either be of type TermFreqVector or of type TermPositionsVector if
	* positions or offsets have been stored.
	* 
	* @param docNumber document for which term frequency vectors are returned
	* @return array of term frequency vectors. May be null if no term vectors have been
	*  stored for the specified document.
	* @throws IOException if index cannot be accessed
	* @see org.apache.lucene.document.Field.TermVector
	*/
	virtual bool getTermFreqVectors(int32_t docNumber, Array<TermFreqVector*>& result) =0;
	
	/**
	*  Return a term frequency vector for the specified document and field. The
	*  returned vector contains terms and frequencies for the terms in
	*  the specified field of this document, if the field had the storeTermVector
	*  flag set. If termvectors had been stored with positions or offsets, a 
	*  TermPositionsVector is returned.
	* 
	* @param docNumber document for which the term frequency vector is returned
	* @param field field for which the term frequency vector is returned.
	* @return term frequency vector May be null if field does not exist in the specified
	* document or term vector was not stored.
	* @throws IOException if index cannot be accessed
	* @see org.apache.lucene.document.Field.TermVector
	*/
	virtual TermFreqVector* getTermFreqVector(int32_t docNumber, const TCHAR* field) = 0;
	
	/**
	* Returns <code>true</code> if an index exists at the specified directory.
	* If the directory does not exist or if there is no index in it.
	* @param  directory the directory to check for an index
	* @return <code>true</code> if an index exists; <code>false</code> otherwise
	*/
	static bool indexExists(const QString& directory);

    /**
	* Returns <code>true</code> if an index exists at the specified directory.
	* If the directory does not exist or if there is no index in it.
	* @param  directory the directory to check for an index
	* @return <code>true</code> if an index exists; <code>false</code> otherwise
	* @throws IOException if there is a problem with accessing the index
	*/
	static bool indexExists(const CL_NS(store)::Directory* directory);

	/** Returns the number of documents in this index. */
  	virtual int32_t numDocs() = 0;

	/** Returns one greater than the largest possible document number.
	* This may be used to, e.g., determine how big to allocate an array which
	* will have an element for every document number in an index.
	*/
	virtual int32_t maxDoc() const = 0;

	/** Gets the stored fields of the <code>n</code><sup>th</sup>
   	 * <code>Document</code> in this index. 
   	 * The fields are not cleared before retrieving the document, so the
   	 * object should be new or just cleared.
   	 */ 
	virtual bool document(int32_t n, CL_NS(document)::Document*) =0;
	
	_CL_DEPRECATED( document(i, document) ) CL_NS(document)::Document* document(const int32_t n);

	/** Returns true if document <i>n</i> has been deleted */
  	virtual bool isDeleted(const int32_t n) = 0;

	/** Returns true if any documents have been deleted */
	virtual bool hasDeletions() const = 0;

	/** Returns true if there are norms stored for this field. */
	virtual bool hasNorms(const TCHAR* field);

	/** Returns an enumeration of all the terms in the index.
	* The enumeration is ordered by Term.compareTo().  Each term
	* is greater than all that precede it in the enumeration.
	* @memory Caller must clean up
	*/
	virtual TermEnum* terms() const =0;

	/** Returns an enumeration of all terms after a given term.
	* The enumeration is ordered by Term.compareTo().  Each term
	* is greater than all that precede it in the enumeration.
	* @memory Caller must clean up
	*/
	virtual TermEnum* terms(const Term* t) const = 0;

	/** Returns the number of documents containing the term <code>t</code>. */
	virtual int32_t docFreq(const Term* t) const = 0;

	/* Returns an unpositioned TermPositions enumerator.
	 * @memory Caller must clean up
	 */
	virtual TermPositions* termPositions() const = 0;
	
    /** Returns an enumeration of all the documents which contain
	* <code>term</code>.  For each document, in addition to the document number
	* and frequency of the term in that document, a list of all of the ordinal
	* positions of the term in the document is available.  Thus, this method
	* implements the mapping:
	*
	* <p><ul>
	* Term &nbsp;&nbsp; =&gt; &nbsp;&nbsp; &lt;docNum, freq,
	* &lt;pos<sub>1</sub>, pos<sub>2</sub>, ...
	* pos<sub>freq-1</sub>&gt;
	* &gt;<sup>*</sup>
	* </ul>
	* <p> This positional information faciliates phrase and proximity searching.
	* <p>The enumeration is ordered by document number.  Each document number is
	* greater than all that precede it in the enumeration.
	 * @memory Caller must clean up
	*/
	TermPositions* termPositions(Term* term) const;

	/** Returns an unpositioned {@link TermDocs} enumerator. 
	 * @memory Caller must clean up
	 */
	virtual TermDocs* termDocs() const = 0;

	/** Returns an enumeration of all the documents which contain
	* <code>term</code>. For each document, the document number, the frequency of
	* the term in that document is also provided, for use in search scoring.
	* Thus, this method implements the mapping:
	* <p><ul>Term &nbsp;&nbsp; =&gt; &nbsp;&nbsp; &lt;docNum, freq&gt;<sup>*</sup></ul>
	* <p>The enumeration is ordered by document number.  Each document number
	* is greater than all that precede it in the enumeration.
	 * @memory Caller must clean up
	*/
	TermDocs* termDocs(Term* term) const;

	/** Deletes the document numbered <code>docNum</code>.  Once a document is
	* deleted it will not appear in TermDocs or TermPostitions enumerations.
	* Attempts to read its field with the {@link #document}
	* method will result in an error.  The presence of this document may still be
	* reflected in the {@link #docFreq} statistic, though
	* this will be corrected eventually as the index is further modified.
	*/
	void deleteDocument(const int32_t docNum);

	///@deprecated. Use deleteDocument instead.
	_CL_DEPRECATED( deleteDocument ) void deleteDoc(const int32_t docNum)
    { deleteDocument(docNum); }

	/** Deletes all documents containing <code>term</code>.
	* This is useful if one uses a document field to hold a unique ID string for
	* the document.  Then to delete such a document, one merely constructs a
	* term with the appropriate field and the unique ID string as its text and
	* passes it to this method.
	* See {@link #deleteDocument(int)} for information about when this deletion will 
	* become effective.
	* @return the number of documents deleted
	*/
	int32_t deleteDocuments(Term* term);

	///@deprecated. Use deleteDocuments instead.
	_CL_DEPRECATED( deleteDocuments ) int32_t deleteTerm(Term* term){ return deleteDocuments(term); }

	/** 
	* Closes files associated with this index and also saves any new deletions to disk.
    * No other methods should be called after this has been called.
    */
	void close();

	///Checks if the index in the named directory is currently locked.       
	static bool isLocked(CL_NS(store)::Directory* directory);
	
	///Checks if the index in the named directory is currently locked.       
	static bool isLocked(const QString& directory);


	///Forcibly unlocks the index in the named directory.
	///Caution: this should only be used by failure recovery code,
	///when it is known that no other process nor thread is in fact
	///currently accessing this index.
	static void unlock(CL_NS(store)::Directory* directory);
	static void unlock(const QString& path);

	 /** Returns the directory this index resides in. */
	CL_NS(store)::Directory* getDirectory() { return directory; }

	/** Returns true if the file is a lucene filename (based on extension or filename) */
	static bool isLuceneFile(const QString& filename);

	/**
	* For classes that need to know when the IndexReader closes (such as caches, etc),
	* should pass their callback function to this.
	*/
	void addCloseCallback(CloseCallback callback, void* parameter);

protected:
	class LockWith : public CL_NS(store)::LuceneLockWith<IndexReader*>
    {
	public:
		LockWith(CL_NS(store)::LuceneLock* lock, CL_NS(store)::Directory* dir);

		//Reads the segmentinfo file and depending on the number of segments found
		//it returns a MultiReader or a SegmentReader
		IndexReader* doBody();

    private:
		CL_NS(store)::Directory* directory;
	};
	friend class IndexReader::LockWith;

    class CommitLockWith : public CL_NS(store)::LuceneLockWith<void>
    {
	public:
	    CommitLockWith(CL_NS(store)::LuceneLock* lock, IndexReader* r);
	    void doBody();

    private:
	    IndexReader* reader;
    };
	friend class IndexReader::CommitLockWith;
};

CL_NS_END
#endif


