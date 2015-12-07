/*
 * Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
 *
 * Distributable under the terms of either the Apache License (Version 2.0) or 
 * the GNU Lesser General Public License, as specified in the COPYING file.
 *
 * Changes are Copyright(C) 2007, 2008 by Nokia Corporation and/or its subsidiary(-ies), all rights reserved.
*/
#ifndef _lucene_index_termvector_h
#define _lucene_index_termvector_h

#if defined(_LUCENE_PRAGMA_ONCE)
#   pragma once
#endif

#include <QtCore/QString>

#include "CLucene/store/Directory.h"
#include "CLucene/store/IndexOutput.h"
#include "FieldInfos.h"

CL_NS_DEF(index)

struct TermVectorOffsetInfo;
class TermPositionVector; 

// Provides access to stored term vector of a document field.
class TermFreqVector : LUCENE_BASE
{
public:
	virtual ~TermFreqVector() {}

	// @return The field this vector is associated with.
	virtual const TCHAR* getField() = 0;

	// @return The number of terms in the term vector.
	virtual int32_t size() = 0;

	// @return An Array of term texts in ascending order.
	virtual const TCHAR** getTerms() = 0;


	/* Array of term frequencies. Locations of the array correspond one to one
	 * to the terms in the array obtained from <code>getTerms</code>
	 * method. Each location in the array contains the number of times this
	 * term occurs in the document or the document field.
	 *
	 * The size of the returned array is size()
	 * @memory Returning a pointer to internal data. Do not delete.
	*/
	virtual const Array<int32_t>* getTermFrequencies() = 0;


	/* Return an index in the term numbers array returned from
	 * <code>getTerms</code> at which the term with the specified
	 * <code>term</code> appears. If this term does not appear in the array,
	 * return -1.
	*/
	virtual int32_t indexOf(const TCHAR* term) = 0;


	/* Just like <code>indexOf(int32_t)</code> but searches for a number of terms
	 * at the same time. Returns an array that has the same size as the number
	 * of terms searched for, each slot containing the result of searching for
	 * that term number.
	 *
	 * @param terms array containing terms to look for
	 * @param start index in the array where the list of terms starts
	 * @param len the number of terms in the list
	*/
	virtual void indexesOf(const TCHAR** terms, const int32_t start,
        const int32_t len, Array<int32_t>& ret) = 0;

	// Solve the diamond inheritence problem by providing a reinterpret function.
    // No dynamic casting is required and no RTTI data is needed to do this
	virtual TermPositionVector* __asTermPositionVector() = 0;
};


/**
* Writer works by opening a document and then opening the fields within the document and then
* writing out the vectors for each field.
* 
* Rough usage:
*
<CODE>
for each document
{
writer.openDocument();
for each field on the document
{
writer.openField(field);
for all of the terms
{
writer.addTerm(...)
}
writer.closeField
}
writer.closeDocument()    
}
</CODE>
*/
class TermVectorsWriter : LUCENE_BASE
{
private:
	class TVField : LUCENE_BASE
    {
	public:
		int32_t number;
		int64_t tvfPointer;
		int32_t length;   // number of distinct term positions
		bool storePositions;
		bool storeOffsets;
	 
		TVField(int32_t number, bool storePos, bool storeOff)
            : tvfPointer(0)
            , length(0)
        {
			this->number = number;
			this->storePositions = storePos;
			this->storeOffsets = storeOff;
		}
        ~TVField() {}
	};

	class TVTerm : LUCENE_BASE
    {
		const TCHAR* termText;
		int32_t termTextLen; //textlen cache
		
	public:
		TVTerm();
        ~TVTerm();
		
		int32_t freq;
		Array<int32_t>* positions;
		Array<TermVectorOffsetInfo>* offsets;

		const TCHAR* getTermText() const;
		size_t getTermTextLen();
		void setTermText(const TCHAR* val);
	};

	CL_NS(store)::IndexOutput* tvx, *tvd, *tvf;
	CL_NS(util)::CLVector<TVField*,CL_NS(util)::Deletor::Object<TVField> > fields;
	CL_NS(util)::CLVector<TVTerm*,CL_NS(util)::Deletor::Object<TVTerm> > terms;
	FieldInfos* fieldInfos;

	TVField* currentField;
	int64_t currentDocPointer;

	void addTermInternal(const TCHAR* termText, const int32_t freq, 
		Array<int32_t>* positions, Array<TermVectorOffsetInfo>* offsets);

	void writeField();
	void writeDoc();
  
	void openField(int32_t fieldNumber, bool storePositionWithTermVector, 
        bool storeOffsetWithTermVector);

public:
	LUCENE_STATIC_CONSTANT(int32_t, FORMAT_VERSION = 2);

	// The size in bytes that the FORMAT_VERSION will take up at the beginning
    // of each file 
	LUCENE_STATIC_CONSTANT(int32_t, FORMAT_SIZE = 4);

	LUCENE_STATIC_CONSTANT(uint8_t, STORE_POSITIONS_WITH_TERMVECTOR = 0x1);
	LUCENE_STATIC_CONSTANT(uint8_t, STORE_OFFSET_WITH_TERMVECTOR = 0x2);
	
	static const QLatin1String LUCENE_TVX_EXTENSION;
	static const QLatin1String LUCENE_TVD_EXTENSION;
	static const QLatin1String LUCENE_TVF_EXTENSION;

	TermVectorsWriter(CL_NS(store)::Directory* directory, const QString& segment,
						   FieldInfos* fieldInfos);

	~TermVectorsWriter();
	void openDocument();
	void closeDocument();

	/** Close all streams. */
	void close();
	bool isDocumentOpen() const;

	/** Start processing a field. This can be followed by a number of calls to
	*  addTerm, and a final call to closeField to indicate the end of
	*  processing of this field. If a field was previously open, it is
	*  closed automatically.
	*/
	void openField(const TCHAR* field);

	/** Finished processing current field. This should be followed by a call to
	*  openField before future calls to addTerm.
	*/
	void closeField();

	/** Return true if a field is currently open. */
	bool isFieldOpen() const;

	/**
	* Add a complete document specified by all its term vectors. If document has no
	* term vectors, add value for tvx.
	* 
	* @param vectors
	* @throws IOException
	*/
	void addAllDocVectors(Array<TermFreqVector*>& vectors);

	/** Add term to the field's term vector. Field must already be open.
	*  Terms should be added in
	*  increasing order of terms, one call per unique termNum. ProxPointer
	*  is a pointer into the TermPosition file (prx). Freq is the number of
	*  times this term appears in this field, in this document.
	* @throws IllegalStateException if document or field is not open
	*/
	void addTerm(const TCHAR* termText, int32_t freq,
		Array<int32_t>* positions = NULL, Array<TermVectorOffsetInfo>* offsets = NULL);
};

class SegmentTermVector : public virtual TermFreqVector
{
private:
	const TCHAR* field;
	TCHAR** terms;
	int32_t termsLen; //cache
	Array<int32_t>* termFreqs;

	int32_t binarySearch(TCHAR** a, const int32_t arraylen, const TCHAR* key) const;
public:
	//note: termFreqs must be the same length as terms
	SegmentTermVector(const TCHAR* field, TCHAR** terms, Array<int32_t>* termFreqs);
	virtual ~SegmentTermVector();

	/**
	* 
	* @return The number of the field this vector is associated with
	*/
	const TCHAR* getField();
	TCHAR* toString() const;
	int32_t size();
	const TCHAR** getTerms();
	const Array<int32_t>* getTermFrequencies();
	int32_t indexOf(const TCHAR* termText);
	void indexesOf(const TCHAR** termNumbers, const int32_t start, const int32_t len, Array<int32_t>& ret);

	virtual TermPositionVector* __asTermPositionVector();
};

class TermVectorsReader : LUCENE_BASE
{
private:
    FieldInfos* fieldInfos;
    
    CL_NS(store)::IndexInput* tvx;
    CL_NS(store)::IndexInput* tvd;
    CL_NS(store)::IndexInput* tvf;
    int64_t _size;
    
    int32_t tvdFormat;
    int32_t tvfFormat;
    
    
    int32_t checkValidFormat(CL_NS(store)::IndexInput* in);
    
	void readTermVectors(const TCHAR** fields, const int64_t* tvfPointers,
        const int32_t len, Array<TermFreqVector*>& _return);

    /**
    * 
    * @param field The field to read in
    * @param tvfPointer The pointer within the tvf file where we should start reading
    * @return The TermVector located at that position
    * @throws IOException
    */
    SegmentTermVector* readTermVector(const TCHAR* field, const int64_t tvfPointer);

    int64_t size();
  
  
	DEFINE_MUTEX(THIS_LOCK)
	TermVectorsReader(const TermVectorsReader& copy);
public:
	TermVectorsReader(CL_NS(store)::Directory* d, const QString& segment,
        FieldInfos* fieldInfos);
	~TermVectorsReader();

	void close();
	TermVectorsReader* clone() const;

	/**
	* Retrieve the term vector for the given document and field
	* @param docNum The document number to retrieve the vector for
	* @param field The field within the document to retrieve
	* @return The TermFreqVector for the document and field or null if there is no termVector for this field.
	* @throws IOException if there is an error reading the term vector files
	*/ 
	TermFreqVector* get(const int32_t docNum, const TCHAR* field);


	/**
	* Return all term vectors stored for this document or null if the could not be read in.
	* 
	* @param docNum The document number to retrieve the vector for
	* @return All term frequency vectors
	* @throws IOException if there is an error reading the term vector files 
	*/
	bool get(int32_t docNum, Array<TermFreqVector*>& result);
};


struct TermVectorOffsetInfo
{
    int startOffset;
    int endOffset;

public:
	static Array<TermVectorOffsetInfo> EMPTY_OFFSET_INFO;
    TermVectorOffsetInfo();
    ~TermVectorOffsetInfo();
    TermVectorOffsetInfo(int32_t startOffset, int32_t endOffset);
    int32_t getEndOffset() const;
    void setEndOffset(int32_t endOffset);
    int32_t getStartOffset() const;
    void setStartOffset(int32_t startOffset);
    bool equals(TermVectorOffsetInfo* o);
    size_t hashCode() const;
};


/* Extends <code>TermFreqVector</code> to provide additional information about
 * positions in which each of the terms is found. A TermPositionVector not
 * necessarily contains both positions and offsets, but at least one of these
 * arrays exists.
*/
class TermPositionVector : public virtual TermFreqVector
{
public:

    /** Returns an array of positions in which the term is found.
     *  Terms are identified by the index at which its number appears in the
     *  term String array obtained from the <code>indexOf</code> method.
     *  May return null if positions have not been stored.
     */
    virtual Array<int32_t>* getTermPositions(int32_t index) = 0;
  
    /**
     * Returns an array of TermVectorOffsetInfo in which the term is found.
     * May return null if offsets have not been stored.
     * 
     * @see org.apache.lucene.analysis.Token
     * 
     * @param index The position in the array to get the offsets from
     * @return An array of TermVectorOffsetInfo objects or the empty list
     */ 
     virtual Array<TermVectorOffsetInfo>* getOffsets(int32_t index) = 0;
     
     virtual ~TermPositionVector(){
	 }
};


class SegmentTermPositionVector: public SegmentTermVector, public TermPositionVector
{
protected:
	Array< Array<int32_t> >* positions;
	Array< Array<TermVectorOffsetInfo> >* offsets;
	static Array<int32_t> EMPTY_TERM_POS;
public:
	SegmentTermPositionVector(const TCHAR* field, TCHAR** terms,
        Array<int32_t>* termFreqs, Array< Array<int32_t> >* positions,
        Array< Array<TermVectorOffsetInfo> >* offsets);
	~SegmentTermPositionVector();

	/**
	* Returns an array of TermVectorOffsetInfo in which the term is found.
	*
	* @param index The position in the array to get the offsets from
	* @return An array of TermVectorOffsetInfo objects or the empty list
	* @see org.apache.lucene.analysis.Token
	*/
	Array<TermVectorOffsetInfo>* getOffsets(int32_t index);

	/**
	* Returns an array of positions in which the term is found.
	* Terms are identified by the index at which its number appears in the
	* term String array obtained from the <code>indexOf</code> method.
	*/
	Array<int32_t>* getTermPositions(int32_t index);

	const TCHAR* getField() {
        return SegmentTermVector::getField(); }
	
    TCHAR* toString() const {
        return SegmentTermVector::toString(); }
	
    int32_t size() {
        return SegmentTermVector::size(); }
	
    const TCHAR** getTerms() {
        return SegmentTermVector::getTerms(); }
	
    const Array<int32_t>* getTermFrequencies() {
        return SegmentTermVector::getTermFrequencies(); }
	
    int32_t indexOf(const TCHAR* termText) {
        return SegmentTermVector::indexOf(termText); }
	
    void indexesOf(const TCHAR** termNumbers, const int32_t start,
        const int32_t len, Array<int32_t>& ret) {
        SegmentTermVector::indexesOf(termNumbers, start, len, ret); }

	virtual TermPositionVector* __asTermPositionVector();
};

CL_NS_END

#endif
