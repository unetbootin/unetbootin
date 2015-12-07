/*
 * Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
 *
 * Distributable under the terms of either the Apache License (Version 2.0) or 
 * the GNU Lesser General Public License, as specified in the COPYING file.
 *
 * Changes are Copyright(C) 2007, 2008 by Nokia Corporation and/or its subsidiary(-ies), all rights reserved.
*/
#include "CLucene/StdHeader.h"
#include "FieldInfos.h"

#include "CLucene/store/Directory.h"
#include "CLucene/document/Document.h"
#include "CLucene/document/Field.h"
#include "CLucene/util/VoidMap.h"
#include "CLucene/util/Misc.h"
#include "CLucene/util/StringIntern.h"

CL_NS_USE(store)
CL_NS_USE(document)
CL_NS_USE(util)
CL_NS_DEF(index)

FieldInfo::FieldInfo(const TCHAR* _fieldName, bool _isIndexed,
    int32_t _fieldNumber, bool _storeTermVector, bool _storeOffsetWithTermVector,
    bool _storePositionWithTermVector, bool _omitNorms)
    : name(CLStringIntern::intern(_fieldName CL_FILELINE))
    , isIndexed(_isIndexed)
    , number(_fieldNumber)
    , storeTermVector(_storeTermVector)
    , storeOffsetWithTermVector(_storeOffsetWithTermVector)
    , storePositionWithTermVector(_storeTermVector)
    , omitNorms(_omitNorms)
{
}

FieldInfo::~FieldInfo()
{
	CL_NS(util)::CLStringIntern::unintern(name);
}

// #pragma mark -- FieldInfos

FieldInfos::FieldInfos()
    : byName(false, false)
    , byNumber(true)
{
}

FieldInfos::~FieldInfos()
{
	byName.clear();
	byNumber.clear();
}

FieldInfos::FieldInfos(Directory* d, const QString& name)
    : byName(false, false)
    , byNumber(true) 
{
	IndexInput* input = d->openInput(name);
	try {	
		read(input);
	} _CLFINALLY (
	    input->close();
	    _CLDELETE(input);
	);
}

void FieldInfos::add(const Document* doc)
{
    DocumentFieldEnumeration* fields  = doc->fields();
    Field* field;
    while (fields->hasMoreElements()) {
        field = fields->nextElement();
        add(field->name(), field->isIndexed(), field->isTermVectorStored());
    }
    _CLDELETE(fields);
}

void FieldInfos::add(const TCHAR* name, bool isIndexed, bool storeTermVector,
    bool storePositionWithTermVector, bool storeOffsetWithTermVector, bool omitNorms)
{
	FieldInfo* fi = fieldInfo(name);
	if (fi == NULL) {
		addInternal(name, isIndexed, storeTermVector, 
			storePositionWithTermVector, 
			storeOffsetWithTermVector, omitNorms);
	} else {
		if (fi->isIndexed != isIndexed) {
            // once indexed, always index
			fi->isIndexed = true;
		}

		if (fi->storeTermVector != storeTermVector) {
            // once vector, always vector
			fi->storeTermVector = true;
		}

		if (fi->storePositionWithTermVector != storePositionWithTermVector) {
            // once vector, always vector
	        fi->storePositionWithTermVector = true;
	    }

	    if (fi->storeOffsetWithTermVector != storeOffsetWithTermVector) {
            // once vector, always vector
	        fi->storeOffsetWithTermVector = true;
        }

	    if (fi->omitNorms != omitNorms) {
            // once norms are stored, always store
	        fi->omitNorms = false;
	    }
	}
}

void FieldInfos::add(const TCHAR** names, bool isIndexed, bool storeTermVectors,
    bool storePositionWithTermVector, bool storeOffsetWithTermVector, bool omitNorms)
{
    int32_t i=0;
    while (names[i] != NULL) {
        add(names[i], isIndexed, storeTermVectors, storePositionWithTermVector, 
            storeOffsetWithTermVector, omitNorms);
        ++i;
    }
}

int32_t FieldInfos::fieldNumber(const TCHAR* fieldName) const
{
	FieldInfo* fi = fieldInfo(fieldName);
	return (fi != NULL) ? fi->number : -1;
}

FieldInfo* FieldInfos::fieldInfo(const TCHAR* fieldName) const
{
	return byName.get(fieldName);
}

const TCHAR* FieldInfos::fieldName(const int32_t fieldNumber) const
{
	FieldInfo* fi = fieldInfo(fieldNumber);
	return (fi == NULL) ? LUCENE_BLANK_STRING : fi->name;
}

FieldInfo* FieldInfos::fieldInfo(const int32_t fieldNumber) const
{
	if (fieldNumber < 0 || (size_t)fieldNumber >= byNumber.size())
        return NULL;
    return byNumber[fieldNumber];
}

int32_t FieldInfos::size() const
{
	return byNumber.size();
}

void FieldInfos::write(Directory* d, const QString& name) const
{
	IndexOutput* output = d->createOutput(name);
	try {
		write(output);
	} _CLFINALLY (
	    output->close();
	    _CLDELETE(output);
	);
}

void FieldInfos::write(IndexOutput* output) const
{
	output->writeVInt(size());
	FieldInfo* fi;
	uint8_t bits;
	for (int32_t i = 0; i < size(); ++i) {
		fi = fieldInfo(i);
		bits = 0x0;
 		if (fi->isIndexed)
            bits |= IS_INDEXED;

 		if (fi->storeTermVector)
            bits |= STORE_TERMVECTOR;

 		if (fi->storePositionWithTermVector)
            bits |= STORE_POSITIONS_WITH_TERMVECTOR;

 		if (fi->storeOffsetWithTermVector)
            bits |= STORE_OFFSET_WITH_TERMVECTOR;

 		if (fi->omitNorms)
            bits |= OMIT_NORMS;

	    output->writeString(fi->name, _tcslen(fi->name));
	    output->writeByte(bits);
	}
}

void FieldInfos::read(IndexInput* input)
{
	int32_t size = input->readVInt();
	for (int32_t i = 0; i < size; ++i) {
        // we could read name into a string buffer, but we can't be sure what
        // the maximum field length will be.
	    TCHAR* name = input->readString();
		uint8_t bits = input->readByte();
   		bool isIndexed = (bits & IS_INDEXED) != 0;
   		bool storeTermVector = (bits & STORE_TERMVECTOR) != 0;
   		bool storePositionsWithTermVector =
            (bits & STORE_POSITIONS_WITH_TERMVECTOR) != 0;
   		bool storeOffsetWithTermVector = (bits & STORE_OFFSET_WITH_TERMVECTOR) != 0;
   		bool omitNorms = (bits & OMIT_NORMS) != 0;
   
   		addInternal(name, isIndexed, storeTermVector,
            storePositionsWithTermVector, storeOffsetWithTermVector, omitNorms);
   		_CLDELETE_CARRAY(name);
	}
}

void FieldInfos::addInternal(const TCHAR* name, bool isIndexed,
    bool storeTermVector, bool storePositionWithTermVector,
    bool storeOffsetWithTermVector, bool omitNorms)
{
	FieldInfo* fi = _CLNEW FieldInfo(name, isIndexed, byNumber.size(),
        storeTermVector, storePositionWithTermVector, storeOffsetWithTermVector,
        omitNorms);
	byNumber.push_back(fi);
	byName.put(fi->name, fi);
}

bool FieldInfos::hasVectors() const
{
	for (int32_t i = 0; i < size(); i++) {
	   if (fieldInfo(i)->storeTermVector)
	      return true;
	}
	return false;
}

CL_NS_END
