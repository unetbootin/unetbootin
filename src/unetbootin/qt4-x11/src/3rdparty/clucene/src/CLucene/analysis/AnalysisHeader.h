/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#ifndef _lucene_analysis_AnalysisHeader_
#define _lucene_analysis_AnalysisHeader_

#if defined(_LUCENE_PRAGMA_ONCE)
# pragma once
#endif

#include "CLucene/util/Reader.h"

CL_NS_DEF(analysis)
  

/** A Token is an occurence of a term from the text of a field.  It consists of
* a term's text, the start and end offset of the term in the text of the field,
* and a type string.
* 
* The start and end offsets permit applications to re-associate a token with
* its source text, e.g., to display highlighted query terms in a document
* browser, or to show matching text fragments in a KWIC (KeyWord In Context)
* display, etc.
* 
* The type is an interned string, assigned by a lexical analyzer
* (a.k.a. tokenizer), naming the lexical or syntactic class that the token
* belongs to.  For example an end of sentence marker token might be implemented
* with type "eos".  The default token type is "word".  
*/
class Token:LUCENE_BASE{
private:
	int32_t _startOffset;				// start in source text
	int32_t _endOffset;				  // end in source text
	const TCHAR* _type;				  // lexical type
	int32_t positionIncrement;
	size_t bufferTextLen;

public:
	#ifndef LUCENE_TOKEN_WORD_LENGTH
	TCHAR* _termText;				  // the text of the term
	#else
	TCHAR _termText[LUCENE_TOKEN_WORD_LENGTH+1];				  // the text of the term
	#endif
	int32_t _termTextLen;
	static const TCHAR* defaultType;

	Token();
	~Token();
	// Constructs a Token with the given text, start and end offsets, & type. 
	Token(const TCHAR* text, const int32_t start, const int32_t end, const TCHAR* typ=defaultType);
	void set(const TCHAR* text, const int32_t start, const int32_t end, const TCHAR* typ=defaultType);
	
	size_t bufferLength(){ return bufferTextLen; }
	void growBuffer(size_t size);
	
	/* Set the position increment.  This determines the position of this
	* token relative to the previous Token in a TokenStream, used in
	* phrase searching.
	*
	* The default value is 1.
	*
	* Some common uses for this are:
	*
	* - Set it to zero to put multiple terms in the same position.  This is
	* useful if, e.g., a word has multiple stems.  Searches for phrases
	* including either stem will match.  In this case, all but the first stem's
	* increment should be set to zero: the increment of the first instance
	* should be one.  Repeating a token with an increment of zero can also be
	* used to boost the scores of matches on that token.
	*
	* - Set it to values greater than one to inhibit exact phrase matches.
	* If, for example, one does not want phrases to match across removed stop
	* words, then one could build a stop word filter that removes stop words and
	* also sets the increment to the number of stop words removed before each
	* non-stop word.  Then exact phrase queries will only match when the terms
	* occur with no intervening stop words.
	*/
	void setPositionIncrement(int32_t posIncr);
	int32_t getPositionIncrement() const;
	const TCHAR* termText() const;
	size_t termTextLength();
	void resetTermTextLen();
	void setText(const TCHAR* txt);

	/**
	* Returns this Token's starting offset, the position of the first character
	* corresponding to this token in the source text.
	* 
	* Note that the difference between endOffset() and startOffset() may not be
	* equal to termText.length(), as the term text may have been altered by a
	* stemmer or some other filter.
	*/
	int32_t startOffset() const { return _startOffset; }
	void setStartOffset(int32_t val){ _startOffset =val; }

	/**
	* Returns this Token's ending offset, one greater than the position of the
	* last character corresponding to this token in the source text.
	*/
	int32_t endOffset() const { return _endOffset; }
	void setEndOffset(int32_t val){ _endOffset =val; }

	// Returns this Token's lexical type.  Defaults to "word". 
	const TCHAR* type() const { return _type; } ///<returns reference
	void setType(const TCHAR* val) { _type = val; } ///<returns reference

	TCHAR* toString() const;

	///Compares the Token for their order
	class OrderCompare:LUCENE_BASE, public CL_NS(util)::Compare::_base //<Token*>
	{
	public:
		bool operator()( Token* t1, Token* t2 ) const;
	};
};

/** 
* A TokenStream enumerates the sequence of tokens, either from
* fields of a document or from query text.
* <p>
* This is an abstract class.  Concrete subclasses are:
* <ul>
* <li>{@link Tokenizer}, a TokenStream
* whose input is a Reader; and
* <li>{@link TokenFilter}, a TokenStream
* whose input is another TokenStream.
* </ul>
*/
class TokenStream:LUCENE_BASE {
public:
	/** Sets token to the next token in the stream, returns false at the EOS. */
	virtual bool next(Token* token) = 0;

    /** Releases resources associated with this stream. */
	virtual void close() = 0;

	virtual ~TokenStream(){
	}

	/* This is for backwards compatibility only. You should pass the token you want to fill
	 * to next(), this will save a lot of object construction and destructions.
	 *  @deprecated. use next(token). Kept only to avoid breaking existing code.
	 */
	_CL_DEPRECATED(next(Token)) Token* next();
};


/** An Analyzer builds TokenStreams, which analyze text.  It thus represents a
 *  policy for extracting index terms from text.
 *  <p>
 *  Typical implementations first build a Tokenizer, which breaks the stream of
 *  characters from the Reader into raw Tokens.  One or more TokenFilters may
 *  then be applied to the output of the Tokenizer.
 *  <p>
 *  WARNING: You must override one of the methods defined by this class in your
 *  subclass or the Analyzer will enter an infinite loop.
 */
class Analyzer:LUCENE_BASE{
public:
	/** Creates a TokenStream which tokenizes all the text in the provided
	Reader.  Default implementation forwards to tokenStream(Reader) for 
	compatibility with older version.  Override to allow Analyzer to choose 
	strategy based on document and/or field.  Must be able to handle null
	field name for backward compatibility. */
	virtual TokenStream* tokenStream(const TCHAR* fieldName, CL_NS(util)::Reader* reader)=0;
	  
	virtual ~Analyzer(){
    }

	/**
	* Invoked before indexing a Field instance if
	* terms have already been added to that field.  This allows custom
	* analyzers to place an automatic position increment gap between
	* Field instances using the same field name.  The default value
	* position increment gap is 0.  With a 0 position increment gap and
	* the typical default token position increment of 1, all terms in a field,
	* including across Field instances, are in successive positions, allowing
	* exact PhraseQuery matches, for instance, across Field instance boundaries.
	*
	* @param fieldName Field name being indexed.
	* @return position increment gap, added to the next token emitted from {@link #tokenStream(TCHAR*, Reader*)}
	*/
	virtual int32_t getPositionIncrementGap(const TCHAR* fieldName);
};


/** A Tokenizer is a TokenStream whose input is a Reader.
<p>
This is an abstract class.
*/
class Tokenizer:public TokenStream {
protected:
    /** The text source for this Tokenizer. */
	CL_NS(util)::Reader* input;

public:
    /** Construct a tokenizer with null input. */
	Tokenizer();
    /** Construct a token stream processing the given input. */
    Tokenizer(CL_NS(util)::Reader* _input);

    // ** By default, closes the input Reader. */
	virtual void close();
	virtual ~Tokenizer();
};

/** A TokenFilter is a TokenStream whose input is another token stream.
<p>
This is an abstract class.
*/
class TokenFilter:public TokenStream {
protected:
    /** The source of tokens for this filter. */
	TokenStream* input;
    /** If true then input will be deleted in the destructor */
	bool deleteTokenStream;

    /** Construct a token stream filtering the given input. 
     *
     * @param in The TokenStream to filter from
     * @param deleteTS If true, input will be deleted in the destructor
    */
	TokenFilter(TokenStream* in, bool deleteTS=false);
	virtual ~TokenFilter();
public:
    /** Close the input TokenStream. */
	void close();
};

CL_NS_END
#endif
