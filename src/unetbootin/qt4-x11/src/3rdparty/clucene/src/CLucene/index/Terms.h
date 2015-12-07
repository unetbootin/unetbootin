/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#ifndef _lucene_index_Terms_
#define _lucene_index_Terms_

#if defined(_LUCENE_PRAGMA_ONCE)
# pragma once
#endif

#include "Term.h"
CL_NS_DEF(index)

class TermEnum; //predefine
class TermPositions;

/** TermDocs provides an interface for enumerating &lt;document, frequency&gt;
 pairs for a term.  <p> The document portion names each document containing
 the term.  Documents are indicated by number.  The frequency portion gives
 the number of times the term occurred in each document.  <p> The pairs are
 ordered by document number.

 @see IndexReader#termDocs()
 */
class TermDocs: LUCENE_BASE {
public:
	virtual ~TermDocs(){
	}

	// Sets this to the data for a term.
	// The enumeration is reset to the start of the data for this term.
	virtual void seek(Term* term)=0;

	/** Sets this to the data for the current term in a {@link TermEnum}.
	* This may be optimized in some implementations.
	*/
	virtual void seek(TermEnum* termEnum)=0;

	// Returns the current document number.  <p> This is invalid until {@link
	//	#next()} is called for the first time.
	virtual int32_t doc() const=0;

	// Returns the frequency of the term within the current document.  <p> This
	//	is invalid until {@link #next()} is called for the first time.
	virtual int32_t freq() const=0;

	// Moves to the next pair in the enumeration.  <p> Returns true iff there is
	//	such a next pair in the enumeration.
	virtual bool next() =0;

	// Attempts to read multiple entries from the enumeration, up to length of
	// <i>docs</i>.  Document numbers are stored in <i>docs</i>, and term
	// frequencies are stored in <i>freqs</i>.  The <i>freqs</i> array must be as
	// int64_t as the <i>docs</i> array.
	//
	// <p>Returns the number of entries read.  Zero is only returned when the
	// stream has been exhausted.
	virtual int32_t read(int32_t* docs, int32_t* freqs, int32_t length)=0;

	// Skips entries to the first beyond the current whose document number is
	// greater than or equal to <i>target</i>. <p>Returns true iff there is such
	// an entry.  <p>Behaves as if written: <pre>
	//   bool skipTo(int32_t target) {
	//     do {
	//       if (!next())
	// 	     return false;
	//     } while (target > doc());
	//     return true;
	//   }
	// </pre>
	// Some implementations are considerably more efficient than that.
	virtual bool skipTo(const int32_t target)=0;

	// Frees associated resources.
	virtual void close() = 0;

	
	/** Solve the diamond inheritence problem by providing a reinterpret function.
    *	No dynamic casting is required and no RTTI data is needed to do this
    */
	virtual TermPositions* __asTermPositions()=0;
};


// Abstract class for enumerating terms.
//
//<p>Term enumerations are always ordered by Term.compareTo().  Each term in
//the enumeration is greater than all that precede it.  
class TermEnum: LUCENE_BASE {
public:
	// Increments the enumeration to the next element.  True if one exists.
	virtual bool next()=0;

	// Returns a pointer to the current Term in the enumeration.
	virtual Term* term()=0;
	
	// Returns the current Term in the enumeration.
	virtual Term* term(bool pointer){
		Term* ret = term();
		if ( !pointer )
			ret->__cl_decref();
		return ret;
	}

	// Returns the docFreq of the current Term in the enumeration.
	virtual int32_t docFreq() const=0;

	// Closes the enumeration to further activity, freeing resources.
	virtual void close() =0;

	virtual ~TermEnum(){
	}
	
	// Term Vector support
	/** Skips terms to the first beyond the current whose value is
	* greater or equal to <i>target</i>. <p>Returns true iff there is such
	* an entry.  <p>Behaves as if written: <pre>
	*   public boolean skipTo(Term target) {
	*     do {
	*       if (!next())
	* 	     return false;
	*     } while (target > term());
	*     return true;
	*   }
	* </pre>
	* Some implementations are considerably more efficient than that.
	*/
	virtual bool skipTo(Term* target){
		do {
			if (!next())
				return false;
		} while (target->compareTo(term(false)) > 0);
		return true;
	}

	/**
	* Because we need to know how to cast the object, we need the objects name.
	*/
	virtual const char* getObjectName() = 0;
};



/**
 * TermPositions provides an interface for enumerating the &lt;document,
 * frequency, &lt;position&gt;* &gt; tuples for a term.  <p> The document and
 * frequency are the same as for a TermDocs.  The positions portion lists the ordinal
 * positions of each occurrence of a term in a document.
 *
 * @see IndexReader#termPositions()
 */
class TermPositions: public virtual TermDocs {
public:
	// Returns next position in the current document.  It is an error to call
	//	this more than {@link #freq()} times
	//	without calling {@link #next()}<p> This is
	//	invalid until {@link #next()} is called for
	//	the first time.
	virtual int32_t nextPosition() = 0;

	virtual ~TermPositions(){
	}

	/** Solve the diamond inheritence problem by providing a reinterpret function.
	  *	No dynamic casting is required and no RTTI data is needed to do this
	  */
	virtual TermDocs* __asTermDocs()=0;
	virtual TermPositions* __asTermPositions()=0;
};
CL_NS_END
#endif
