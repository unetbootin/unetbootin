/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#ifndef _lucene_document_Field_
#define _lucene_document_Field_

#if defined(_LUCENE_PRAGMA_ONCE)
# pragma once
#endif

#include "CLucene/util/Reader.h"
#include "CLucene/util/streambase.h"

CL_NS_DEF(document)
/**
A field is a section of a Document.  Each field has two parts, a name and a
value.  Values may be free text, provided as a String or as a Reader, or they
may be atomic keywords, which are not further processed.  Such keywords may
be used to represent dates, urls, etc.  Fields are optionally stored in the
index, so that they may be returned with hits on the document.

PORTING: CLucene doesn't directly support compressed fields. However, it is easy 
to reproduce this functionality by using the GZip streams in the contrib package.
Also note that binary fields are not read immediately in CLucene, a substream
is pointed directly to the field's data, in affect creating a lazy load ability.
This means that large fields are best saved in binary format (even if they are
text), so that they can be loaded lazily.
*/
class Field :LUCENE_BASE{
private:
	const TCHAR* _name;
	TCHAR* _stringValue;
	CL_NS(util)::Reader* _readerValue;
    jstreams::StreamBase<char>* _streamValue;

	int config;
	qreal boost;
	bool omitNorms;
public:
	enum Store{ 
		/** Store the original field value in the index. This is useful for short texts
		* like a document's title which should be displayed with the results. The
		* value is stored in its original form, i.e. no analyzer is used before it is
		* stored.
		*/
		STORE_YES=1, 
		/** Do not store the field value in the index. */
		STORE_NO=2,

		/** Store the original field value in the index in a compressed form. This is
	     * useful for long documents and for binary valued fields.
	     * NOTE: CLucene does not directly support compressed fields, to store a
	     * compressed field. 
	     * //TODO: need better documentation on how to add a compressed field
	     * //because actually we still need to write a GZipOutputStream...
	     */
	    STORE_COMPRESS=4
	};

	enum Index{ 
		/** Do not index the field value. This field can thus not be searched,
		* but one can still access its contents provided it is
		* {@link Field::Store stored}. */
		INDEX_NO=16, 
		/** Index the field's value so it can be searched. An Analyzer will be used
		* to tokenize and possibly further normalize the text before its
		* terms will be stored in the index. This is useful for common text.
		*/
		INDEX_TOKENIZED=32, 
		/** Index the field's value without using an Analyzer, so it can be searched.
		* As no analyzer is used the value will be stored as a single term. This is
		* useful for unique Ids like product numbers.
		*/
		INDEX_UNTOKENIZED=64, 
		/** Index the field's value without an Analyzer, and disable
		* the storing of norms.  No norms means that index-time boosting
		* and field length normalization will be disabled.  The benefit is
		* less memory usage as norms take up one byte per indexed field
		* for every document in the index.
		*/
		INDEX_NONORMS=128
	};

	enum TermVector{
		/** Do not store term vectors. */
		TERMVECTOR_NO=256, 
		/** Store the term vectors of each document. A term vector is a list
		* of the document's terms and their number of occurences in that document. */
		TERMVECTOR_YES=512,    
		/**
		* Store the term vector + token position information
		* 
		* @see #YES
		*/ 
		TERMVECTOR_WITH_POSITIONS=1024,
		/**
		* Store the term vector + Token offset information
		* 
		* @see #YES
		*/ 
		TERMVECTOR_WITH_OFFSETS=2048
	};

	_CL_DEPRECATED( another overload ) Field(const TCHAR* name, const TCHAR* value, bool store, bool index, bool token, const bool storeTermVector=false);
	_CL_DEPRECATED( another overload ) Field(const TCHAR* name, CL_NS(util)::Reader* reader, bool store, bool index, bool token, const bool storeTermVector=false);

	Field(const TCHAR* name, const TCHAR* value, int configs);
	Field(const TCHAR* name, CL_NS(util)::Reader* reader, int configs);
	Field(const TCHAR* name, jstreams::StreamBase<char>* stream, int configs);
    ~Field();

	/** Constructs a String-valued Field that is not tokenized, but is indexed
	* and stored.  Useful for non-text fields, e.g. date or url. 
	* @deprecated Use new Field(name,value,Field::STORE_YES | Field::INDEX_UNTOKENIZED)
	*/
	_CL_DEPRECATED( new Field(*) ) static Field* Keyword(const TCHAR* name, const TCHAR* value);

	/** Constructs a String-valued Field that is not tokenized nor indexed,
	*	but is stored in the index, for return with hits. 
	* @deprecated Use new Field(name,value,Field::STORE_YES | Field::INDEX_NO)
	*/
	_CL_DEPRECATED( new Field(*) ) static Field* UnIndexed(const TCHAR* name, const TCHAR* value);

	/** Constructs a String-valued Field that is tokenized and indexed,
	*	and is stored in the index, for return with hits.  Useful for short text
	*	fields, like "title" or "subject". 
	* @deprecated Use new Field(name,value,Field::STORE_YES | Field::INDEX_TOKENIZED)
	*/
	_CL_DEPRECATED( new Field(*) ) static Field* Text(const TCHAR* name, const TCHAR* value, const bool storeTermVector=false);

	/** Constructs a String-valued Field that is tokenized and indexed,
	*	but that is not stored in the index.
	* @deprecated Use new Field(name,value,Field::STORE_NO | Field::INDEX_TOKENIZED)
	*/
	_CL_DEPRECATED( new Field(*) ) static Field* UnStored(const TCHAR* name, const TCHAR* value, const bool storeTermVector=false);

	/** Constructs a Reader-valued Field that is tokenized and indexed, but is
	*	*not* stored in the index verbatim.  Useful for longer text fields, like
	*	"body".
	* @deprecated Use new Field(name,value, Field::INDEX_TOKENIZED)
	*/
	_CL_DEPRECATED( new Field(*) ) static Field* Text(const TCHAR* name, CL_NS(util)::Reader* value, const bool storeTermVector=false);

	/**  The name of the field (e.g., "date", "subject", "title", "body", etc.)
	*	as an interned string. */
	const TCHAR* name(); ///<returns reference

	/** The value of the field as a String, or null.  If null, the Reader value
	* or binary value is used.  Exactly one of stringValue(), readerValue() and
	* streamValue() must be set. */
	TCHAR* stringValue(); ///<returns reference

	/** The value of the field as a reader, or null.  If null, the String value
	* or stream value is used.  Exactly one of stringValue(), readerValue() and
	* streamValue() must be set. */
	CL_NS(util)::Reader* readerValue();

	/** The value of the field as a String, or null.  If null, the String value
	* or Reader value is used.  Exactly one of stringValue(), readerValue() and
	* streamValue() must be set. */
	jstreams::StreamBase<char>* streamValue();

	//  True iff the value of the field is to be stored in the index for return
	//	with search hits.  It is an error for this to be true if a field is
	//	Reader-valued. 
	bool isStored();

	//  True iff the value of the field is to be indexed, so that it may be
	//	searched on. 
	bool isIndexed();

	// True iff the value of the field should be tokenized as text prior to
	//	indexing.  Un-tokenized fields are indexed as a single word and may not be
	//	Reader-valued.
	bool isTokenized();
	
	/** True if the value of the field is stored and compressed within the index 
	* NOTE: CLucene does not actually support compressed fields, Instead, a reader
	* will be returned with a pointer to a SubIndexInputStream. A GZipInputStream
	* and a UTF8 reader must be used to actually read the content. This flag
	* will only be set if the index was created by another lucene implementation.
	*/
	bool isCompressed();

	//Set configs using XOR. This resets all the settings
	//For example, to use term vectors with positions and offsets do:
	//object->setConfig(TERMVECTOR_WITH_POSITIONS | TERMVECTOR_WITH_OFFSETS);
	void setConfig(int termVector);

	/** True iff the term or terms used to index this field are stored as a term
	*  vector, available from {@link IndexReader#getTermFreqVector(int32_t,TCHAR*)}.
	*  These methods do not provide access to the original content of the field,
	*  only to terms used to index it. If the original content must be
	*  preserved, use the <code>stored</code> attribute instead.
	*
	* @see IndexReader#getTermFreqVector(int32_t, String)
	*/
	bool isTermVectorStored();

	/**
	* True iff terms are stored as term vector together with their offsets 
	* (start and end positon in source text).
	*/
	bool isStoreOffsetWithTermVector();
	  
	/**
	* True iff terms are stored as term vector together with their token positions.
	*/
	bool isStorePositionWithTermVector();

	/** Returns the boost factor for hits for this field.
	*
	* <p>The default value is 1.0.
	*
	* <p>Note: this value is not stored directly with the document in the index.
	* Documents returned from {@link IndexReader#document(int)} and
	* {@link Hits#doc(int)} may thus not have the same value present as when
	* this field was indexed.
	*
	* @see #setBoost(float)
	*/
	qreal getBoost();
      
	/** Sets the boost factor hits on this field.  This value will be
	* multiplied into the score of all hits on this this field of this
	* document.
	*
	* <p>The boost is multiplied by {@link Document#getBoost()} of the document
	* containing this field.  If a document has multiple fields with the same
	* name, all such values are multiplied together.  This product is then
	* multipled by the value {@link Similarity#lengthNorm(String,int)}, and
	* rounded by {@link Similarity#encodeNorm(float)} before it is stored in the
	* index.  One should attempt to ensure that this product does not overflow
	* the range of that encoding.
	*
	* @see Document#setBoost(float)
	* @see Similarity#lengthNorm(String, int)
	* @see Similarity#encodeNorm(float)
	*/
	void setBoost(qreal value);

	/** True iff the value of the filed is stored as binary */
	bool isBinary();
	
	/** True if norms are omitted for this indexed field */
	bool getOmitNorms();

	/** Expert:
	*
	* If set, omit normalization factors associated with this indexed field.
	* This effectively disables indexing boosts and length normalization for this field.
	*/
	void setOmitNorms(bool omitNorms);

	// Prints a Field for human consumption.
	TCHAR* toString();
};
CL_NS_END
#endif
