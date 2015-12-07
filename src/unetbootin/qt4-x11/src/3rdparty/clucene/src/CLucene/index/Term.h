/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
*
* Changes are Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
------------------------------------------------------------------------------*/
#ifndef _lucene_index_Term_
#define _lucene_index_Term_

#if defined(_LUCENE_PRAGMA_ONCE)
# pragma once
#endif

#include "CLucene/util/Misc.h"
#include "CLucene/util/StringIntern.h"

CL_NS_DEF(index)

/*
A Term represents a word from text.  This is the unit of search.  It is
composed of two elements, the text of the word, as a string, and the name of
the field that the text occured in, an interned string.

Note that terms may represent more than words from text fields, but also
things like dates, email addresses, urls, etc.  

IMPORTANT NOTE:
Term inherits from the template class LUCENE_REFBASE which tries to do
some garbage collection by counting the references an instance has. As a result
of this construction you MUST use _CLDECDELETE(obj) when you want to delete an 
of Term!

ABOUT intrn 

intrn indicates if field and text are interned or not. Interning of Strings
is the process of converting duplicated strings to shared ones. 

*/
class Term : LUCENE_REFBASE
{
private:
    const TCHAR* _field;
    bool internF; // Indicates if Term Field is interned(and therefore must be uninternd).
    size_t cachedHashCode;
    size_t textLen; // a cache of text len, this allows for a preliminary comparison of text lengths

#ifdef LUCENE_TERM_TEXT_LENGTH
    TCHAR _text[LUCENE_TERM_TEXT_LENGTH + 1];
#else
    TCHAR* _text;
    size_t textLenBuf; //a cache of text len, this allows for a preliminary comparison of text lengths
#endif

    void init();
public:

    //uses the specified fieldTerm's field. this saves on intern'ing time.
    Term(const Term* fieldTerm, const TCHAR* txt);

    ///Constructs a blank term
    Term();

    // TODO: need to be private, a few other things need to be changed first...
    Term(const TCHAR* fld, const TCHAR* txt, bool internField);

    /**
    * Constructor. Constructs a Term with the given field and text. Field and
    * text are not copied Field and text are deleted in destructor only if
    * intern is false. 
    */
    Term(const TCHAR* fld, const TCHAR* txt);

    ///Destructor.
    ~Term();

    ///Returns the field of this term, an interned string. The field indicates
    ///the part of a document which this term came from. 
    const TCHAR* field() const; ///<returns reference

    ///Returns the text of this term.  In the case of words, this is simply the
    ///text of the word.  In the case of dates and other types, this is an
    ///encoding of the object as a string.
    const TCHAR* text() const; ///<returns reference

    ///Resets the field and text of a Term.
    inline void set(const TCHAR* fld, const TCHAR* txt)
    {
        set(fld, txt, true);
    }

    /**
    * Optimized set of Term by reusing same field as this Term
    * - avoids field.intern() overhead
    * @param text The text of the new term
    * (field is implicitly same as this Term instance)
    */
    void set(const Term* term, const TCHAR* txt);

    void set(const TCHAR* fld, const TCHAR* txt, bool internField);

    /** Compares two terms, returning a negative integer if this
    term belongs before the argument, zero if this term is equal to the
    argument, and a positive integer if this term belongs after the argument.

    The ordering of terms is first by field, then by text.*/
    int32_t compareTo(const Term* other) const;

    bool equals(const Term* other) const;

    size_t textLength() const { return textLen; }

    ///Forms the contents of Field and term in some kind of tuple notation
    ///<field:text>
    TCHAR* toString() const;

    size_t hashCode();

    class Equals:public CL_NS_STD(binary_function)<const Term*,const Term*,bool>
    {
    public:
        bool operator()( const Term* val1, const Term* val2 ) const
        {
            return val1->equals(val2);
        }
    };

    class Compare:LUCENE_BASE, public CL_NS(util)::Compare::_base //<Term*>
    {
    public:
        bool operator()(Term* t1, Term* t2) const
        {
            return (t1->compareTo(t2) < 0);
        }

        size_t operator()(Term* t) const
        {
            return t->hashCode();
        }
    };
};

CL_NS_END

#endif
