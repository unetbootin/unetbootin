/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
*
* Changes are Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
------------------------------------------------------------------------------*/

#include "CLucene/StdHeader.h"
#include "Term.h"
#include "CLucene/util/StringIntern.h"

CL_NS_USE(util)
CL_NS_DEF(index)

Term::Term()
{
    init();
}

Term::Term(const TCHAR* fld, const TCHAR* txt, bool internField)
{
    init();
    set(fld, txt, internField);
}

Term::Term(const Term* fieldTerm, const TCHAR* txt)
{
    init();
    set(fieldTerm, txt);
}

Term::Term(const TCHAR* fld, const TCHAR* txt)
{
    init();
    set(fld, txt);
}

Term::~Term()
{
    if (internF)
        CLStringIntern::unintern(_field);
    _field = NULL;

#ifndef LUCENE_TERM_TEXT_LENGTH
    if (_text != LUCENE_BLANK_STRING)
        _CLDELETE_CARRAY(_text);
#endif
}

const TCHAR* Term::field() const
{
    return _field;
}

const TCHAR* Term::text() const
{
    return _text;
}

void Term::set(const Term* term, const TCHAR* txt)
{
    set(term->field(), txt, false);
}

void Term::set(const TCHAR* fld, const TCHAR* txt,bool internField)
{
    CND_PRECONDITION(fld != NULL, "fld contains NULL");
    CND_PRECONDITION(txt != NULL, "txt contains NULL");

    //save field for unintern later
    const TCHAR* oldField = _field;
    cachedHashCode = 0;
    textLen = _tcslen(txt);

    //Delete text if it is the owner
#ifdef LUCENE_TERM_TEXT_LENGTH
    if (textLen > LUCENE_TERM_TEXT_LENGTH)
        textLen = LUCENE_TERM_TEXT_LENGTH;

    _tcsncpy(_text,txt,textLen+1);
    _text[textLen]=0;
#else
    //if the term text buffer is bigger than what we have
    if (_text && textLen > textLenBuf) {
        if (_text != LUCENE_BLANK_STRING) {
            _CLDELETE_ARRAY(_text);
        } else {
            _text = NULL;
        }
        textLenBuf = 0;
    }

    if (_text == LUCENE_BLANK_STRING) {
        _text = LUCENE_BLANK_STRING;
    } else if (_text == NULL) {
        if (txt[0] == 0) {
            //if the string is blank and we aren't re-using the buffer...
            _text = LUCENE_BLANK_STRING;
        } else {
            //duplicate the text
            _text  = stringDuplicate(txt);
            textLenBuf = textLen;
        }
    } else {
        //re-use the buffer
        _tcscpy(_text,txt);
    }
#endif

    //Set Term Field
    if (internField) {
        _field = CLStringIntern::intern(fld  CL_FILELINE);
    } else {
        _field = fld;
    }

    //unintern old field after interning new one, 
    if (internF)
        CLStringIntern::unintern(oldField);
    internF = internField;

    CND_PRECONDITION(_tcscmp(fld, _field) == 0, "field not equal");
}

bool Term::equals(const Term* other) const
{
    if (cachedHashCode != 0 && other->cachedHashCode != 0
        && other->cachedHashCode != cachedHashCode)
        return false;

    if (_field == other->_field) {
        if (textLen == other->textLen)
            return (_tcscmp(_text, other->_text) == 0);
        return false;
    }

    return false;
}

size_t Term::hashCode()
{
    if (cachedHashCode == 0)
        cachedHashCode = Misc::thashCode(_field) + Misc::thashCode(_text, textLen);

    return cachedHashCode;
}

int32_t Term::compareTo(const Term* other) const
{
    //Check ret to see if text needs to be compared
    if (_field == other->_field)
        return _tcscmp(_text, other->_text);

    return _tcscmp(_field, other->_field);
}

TCHAR* Term::toString() const
{
    return CL_NS(util)::Misc::join(_field, _T(":"), _text);
}

void Term::init()
{
    textLen = 0;
    internF = false;
    cachedHashCode = 0;
    _field = LUCENE_BLANK_STRING;    

#ifdef LUCENE_TERM_TEXT_LENGTH
    _text[0] = 0;
#else
    _text = LUCENE_BLANK_STRING;
    textLenBuf = 0;
#endif
}

CL_NS_END
