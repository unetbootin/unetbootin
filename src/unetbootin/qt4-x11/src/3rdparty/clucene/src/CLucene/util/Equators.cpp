/*
 * Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
 *
 * Distributable under the terms of either the Apache License (Version 2.0) or 
 * the GNU Lesser General Public License, as specified in the COPYING file.
 *
 * Changes are Copyright(C) 2007, 2008 by Nokia Corporation and/or its subsidiary(-ies), all rights reserved.
*/
#include "CLucene/StdHeader.h"
#include "Equators.h"

CL_NS_DEF(util)

bool Equals::Int32::operator()(const int32_t val1, const int32_t val2) const
{
    return (val1)==(val2);
}

bool Equals::Char::operator()(const char* val1, const char* val2) const
{
    if ( val1 == val2 )
        return true;
    return (strcmp(val1, val2) == 0);
}

#ifdef _UCS2
bool Equals::WChar::operator()(const wchar_t* val1, const wchar_t* val2) const
{
    if (val1 == val2)
        return true;
    return (_tcscmp(val1, val2) == 0);
}
#endif

bool Equals::Qstring::operator()(const QString& val1, const QString& val2) const
{
    return (val1 == val2);
}

///////////////////////////////////////////////////////////////////////////////
// Comparors
///////////////////////////////////////////////////////////////////////////////

int32_t Compare::Int32::getValue() const
{
    return value;
}

Compare::Int32::Int32(int32_t val)
{
    value = val;
}

Compare::Int32::Int32()
{
    value = 0;
}

int32_t Compare::Int32::compareTo(void* o)
{
    try {
        Int32* other = (Int32*)o;
        if (value == other->value)
            return 0;
        // Returns just -1 or 1 on inequality; doing math might overflow.
        return value > other->value ? 1 : -1;
    } catch(...) {
        _CLTHROWA(CL_ERR_Runtime, "Couldnt compare types");
    }  
}

bool Compare::Int32::operator()(int32_t t1, int32_t t2) const
{
    return t1 > t2 ? true : false;
}

size_t Compare::Int32::operator()(int32_t t) const
{
    return t;
}

qreal Compare::Float::getValue() const
{
    return value;
}

Compare::Float::Float(qreal val)
{
    value = val;
}

int32_t Compare::Float::compareTo(void* o)
{
    try {
        Float* other = (Float*)o;
        if (value == other->value)
            return 0;
        // Returns just -1 or 1 on inequality; doing math might overflow.
        return value > other->value ? 1 : -1;
    } catch(...) {
        _CLTHROWA(CL_ERR_Runtime,"Couldnt compare types");
    }  
}

bool Compare::Char::operator()(const char* val1, const char* val2) const
{
    if ( val1 == val2)
        return false;
    return (strcmp(val1, val2) < 0);
}

size_t Compare::Char::operator()(const char* val1) const
{
    return CL_NS(util)::Misc::ahashCode(val1);
}

#ifdef _UCS2
bool Compare::WChar::operator()(const wchar_t* val1, const wchar_t* val2) const
{
    if ( val1==val2)
        return false;
    return (_tcscmp(val1, val2) < 0);
}

size_t Compare::WChar::operator()(const wchar_t* val1) const
{
    return CL_NS(util)::Misc::whashCode(val1);
}
#endif

const TCHAR* Compare::TChar::getValue() const
{
    return s;
}

Compare::TChar::TChar()
{
    s = NULL;
}

Compare::TChar::TChar(const TCHAR* str)
{
    this->s = str;
}

int32_t Compare::TChar::compareTo(void* o)
{
    try {
        TChar* os = (TChar*)o;
        return _tcscmp(s, os->s);
    } catch(...) {
        _CLTHROWA(CL_ERR_Runtime,"Couldnt compare types");
    }  

}

bool Compare::TChar::operator()(const TCHAR* val1, const TCHAR* val2) const
{
    if (val1 == val2)
        return false;

    return (_tcscmp(val1, val2) < 0);
}

size_t Compare::TChar::operator()(const TCHAR* val1) const
{
    return CL_NS(util)::Misc::thashCode(val1);
}

bool Compare::Qstring::operator()(const QString& val1, const QString& val2) const
{
    return (val1 < val2);
}

size_t Compare::Qstring::operator ()(const QString& val1) const
{
    return CL_NS(util)::Misc::qhashCode(val1);
}

CL_NS_END
