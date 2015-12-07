/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#include "CLucene/StdHeader.h"
#include "CLucene/util/Reader.h"
#include "Field.h"
#include "CLucene/util/Misc.h"
#include "CLucene/util/StringIntern.h"
#include "CLucene/util/StringBuffer.h"

CL_NS_USE(util)
CL_NS_DEF(document) 

Field::Field(const TCHAR* Name, const TCHAR* String, bool store, bool index, bool token, const bool storeTermVector)
{
//Func - Constructor
//Pre  - Name != NULL and contains the name of the field
//       String != NULL and contains the value of the field
//       store indicates if the field must be stored
//       index indicates if the field must be indexed
//       token indicates if the field must be tokenized
//Post - The instance has been created

	CND_PRECONDITION(Name != NULL, "Name is NULL");
	CND_PRECONDITION(String != NULL,"String is NULL");
	CND_PRECONDITION(!(!index && storeTermVector),"cannot store a term vector for fields that are not indexed.");

	_name        = CLStringIntern::intern( Name CL_FILELINE);
	_stringValue = stringDuplicate( String );
	_readerValue = NULL;
	_streamValue = NULL;
	boost=1.0f;
	omitNorms = false;

	int cfg = 0;
	if ( store )
		cfg |= STORE_YES;
	if ( index && token )
		cfg |= INDEX_TOKENIZED;
	else if ( index && !token )
		cfg |= INDEX_UNTOKENIZED;

	if ( storeTermVector )
		_CLTHROWA(CL_ERR_IllegalArgument,"Stored term vector is deprecated with using this constructor");

	setConfig(cfg);
}

Field::Field(const TCHAR* Name, Reader* reader, bool store, bool index, bool token, const bool storeTermVector)
{
//Func - Constructor
//Pre  - Name != NULL and contains the name of the field
//       reader != NULL and contains a Reader
//       store indicates if the field must be stored
//       index indicates if the field must be indexed
//       token indicates if the field must be tokenized
//Post - The instance has been created

	CND_PRECONDITION(Name != NULL, "Name is NULL");
	CND_PRECONDITION(reader != NULL, "reader is NULL");

	_name        = CLStringIntern::intern( Name  CL_FILELINE);
	_stringValue = NULL;
	_readerValue = reader;
	_streamValue = NULL;
	boost=1.0f;
	omitNorms = false;

	int cfg = 0;
	if ( store )
		cfg |= STORE_YES;
	if ( index && token )
		cfg |= INDEX_TOKENIZED;
	else if ( index && !token )
		cfg |= INDEX_UNTOKENIZED;

	if ( storeTermVector )
		_CLTHROWA(CL_ERR_IllegalArgument,"Stored term vector is deprecated with using this constructor");

	setConfig(cfg);
}

Field::Field(const TCHAR* Name, Reader* reader, int config)
{
	CND_PRECONDITION(Name != NULL, "Name is NULL");
	CND_PRECONDITION(reader != NULL, "reader is NULL");

	_name        = CLStringIntern::intern( Name  CL_FILELINE);
	_stringValue = NULL;
	_readerValue = reader;
	_streamValue = NULL;
	boost=1.0f;
	omitNorms = false;

	setConfig(config);
}


Field::Field(const TCHAR* Name, const TCHAR* Value, int config)
{
	CND_PRECONDITION(Name != NULL, "Name is NULL");
	CND_PRECONDITION(Value != NULL, "value is NULL");

	_name        = CLStringIntern::intern( Name  CL_FILELINE);
	_stringValue = stringDuplicate( Value );
	_readerValue = NULL;
	_streamValue = NULL;
	boost=1.0f;
	omitNorms = false;

	setConfig(config);
}

Field::Field(const TCHAR* Name, jstreams::StreamBase<char>* Value, int config)
{
	CND_PRECONDITION(Name != NULL, "Name is NULL");
	CND_PRECONDITION(Value != NULL, "value is NULL");

	_name        = CLStringIntern::intern( Name  CL_FILELINE);
	_stringValue = NULL;
	_readerValue = NULL;
	_streamValue = Value;
	boost=1.0f;
	omitNorms = false;

	setConfig(config);
}

Field::~Field(){
//Func - Destructor
//Pre  - true
//Post - Instance has been destroyed

	CLStringIntern::unintern(_name);
	_CLDELETE_CARRAY(_stringValue);
	_CLDELETE(_readerValue);
	_CLVDELETE( _streamValue );
}


/*===============FIELDS=======================*/
const TCHAR* Field::name() 		{ return _name; } ///<returns reference
TCHAR* Field::stringValue()		{ return _stringValue; } ///<returns reference
Reader* Field::readerValue()	{ return _readerValue; } ///<returns reference
jstreams::StreamBase<char>* Field::streamValue()	{ return _streamValue; } ///<returns reference

bool	Field::isStored() 	{ return (config & STORE_YES) != 0; }
bool 	Field::isIndexed() 	{ return (config & INDEX_TOKENIZED)!=0 || (config & INDEX_UNTOKENIZED)!=0; }
bool 	Field::isTokenized() 	{ return (config & INDEX_TOKENIZED) != 0; }
bool 	Field::isCompressed() 	{ return (config & STORE_COMPRESS) != 0; }
bool 	Field::isBinary() 	{ return _streamValue!=NULL; }

bool	Field::isTermVectorStored() { return (config & TERMVECTOR_YES) != 0; }
bool	Field::isStoreOffsetWithTermVector() { return (config & TERMVECTOR_YES) != 0 && (config & TERMVECTOR_WITH_OFFSETS) != 0; }
bool	Field::isStorePositionWithTermVector() { return (config & TERMVECTOR_YES) != 0 && (config & TERMVECTOR_WITH_POSITIONS) != 0; }

bool Field::getOmitNorms() { return omitNorms; }
void Field::setOmitNorms(bool omitNorms) { this->omitNorms=omitNorms; }

void Field::setBoost(qreal boost) { this->boost = boost; }
qreal Field::getBoost() { return boost; }

void Field::setConfig(int x){
	int newConfig=0;

	//set storage settings
	if ( (x & STORE_YES) || (x & STORE_COMPRESS) ){
		newConfig |= STORE_YES;
		if ( x & STORE_COMPRESS )
			newConfig |= STORE_COMPRESS;
	}else
		newConfig |= STORE_NO;

	if ( (x & INDEX_NO)==0 ){
		bool index=false;

		if ( x & INDEX_NONORMS ){
			newConfig |= INDEX_NONORMS;
			index = true;
		}

		if ( x & INDEX_TOKENIZED && x & INDEX_UNTOKENIZED )
			_CLTHROWA(CL_ERR_IllegalArgument,"it doesn't make sense to have an untokenised and tokenised field");
		if ( x & INDEX_TOKENIZED ){
			newConfig |= INDEX_TOKENIZED;
			index = true;
		}
		if ( x & INDEX_UNTOKENIZED ){
			newConfig |= INDEX_UNTOKENIZED;
			index = true;
		}
		if ( !index )
			newConfig |= INDEX_NO;
	}else
		newConfig |= INDEX_NO;

	if ( newConfig & INDEX_NO && newConfig & STORE_NO )
		_CLTHROWA(CL_ERR_IllegalArgument,"it doesn't make sense to have a field that is neither indexed nor stored");

	//set termvector settings
	if ( (x & TERMVECTOR_NO) == 0 ){
		bool termVector=false;
		if ( x & TERMVECTOR_YES ){
			termVector=true;
		}
		if ( x & TERMVECTOR_WITH_OFFSETS ){
			newConfig |= TERMVECTOR_WITH_OFFSETS;
			termVector=true;
		}
		if ( x & TERMVECTOR_WITH_POSITIONS ){
			newConfig |= TERMVECTOR_WITH_POSITIONS;
			termVector=true;
		}
		if ( termVector ){
			if ( newConfig & INDEX_NO )
				_CLTHROWA(CL_ERR_IllegalArgument,"cannot store a term vector for fields that are not indexed.");

			newConfig |= TERMVECTOR_YES;
		}else
			newConfig |= TERMVECTOR_NO;
	}else
		newConfig |= TERMVECTOR_NO;

	config = newConfig;
}

TCHAR* Field::toString() {
    CL_NS(util)::StringBuffer result;
	if (isStored()) {
      result.append( _T("stored") );
    }
    if (isIndexed()) {
      if (result.length() > 0)
        result.append( _T(",") );
      result.append( _T("indexed") );
    }
    if (isTokenized()) {
      if (result.length() > 0)
        result.append( _T(",") );
      result.append( _T("tokenized") );
    }
    if (isTermVectorStored()) {
      if (result.length() > 0)
        result.append( _T(",") );
      result.append( _T("termVector") );
    }
    if (isStoreOffsetWithTermVector()) { 
      if (result.length() > 0) 
        result.appendChar( ',' ); 
      result.append( _T("termVectorOffsets") ); 
    } 
    if (isStorePositionWithTermVector()) { 
      if (result.length() > 0) 
        result.appendChar( ',' ); 
      result.append( _T("termVectorPosition") ); 
    } 
    if (isBinary()) {
      if (result.length() > 0)
        result.appendChar( ',' );
      result.append( _T("binary") );
    }
    if (getOmitNorms()) {
      result.append( _T(",omitNorms") );
    }
    result.appendChar('<');
    result.append(name());
    result.appendChar(':');
    
    if (_stringValue != NULL)
    	result.append(_stringValue);
    else if ( _readerValue != NULL )
    	result.append( _T("Reader") );
    else if ( _streamValue != NULL )
    	result.append( _T("Stream") );
    else
    	result.append( _T("NULL") );
    
    result.appendChar('>');
    return result.toString();
}


Field* Field::Keyword(const TCHAR* Name, const TCHAR* Value) {
	return _CLNEW Field(Name,Value,Field::STORE_YES | Field::INDEX_UNTOKENIZED);
}

Field* Field::UnIndexed(const TCHAR* Name, const TCHAR* Value) {
	return _CLNEW Field(Name,Value,Field::STORE_YES | Field::INDEX_NO);
}

Field* Field::Text(const TCHAR* Name, const TCHAR* Value, const bool storeTermVector) {
	if ( storeTermVector )
		return _CLNEW Field(Name,Value,Field::STORE_YES | Field::INDEX_TOKENIZED | Field::TERMVECTOR_YES);
	else
		return _CLNEW Field(Name,Value,Field::STORE_YES | Field::INDEX_TOKENIZED);
}

Field* Field::UnStored(const TCHAR* Name, const TCHAR* Value, const bool storeTermVector) {
	if ( storeTermVector )
		return _CLNEW Field(Name,Value,Field::STORE_NO | Field::INDEX_TOKENIZED | Field::TERMVECTOR_YES);
	else
		return _CLNEW Field(Name,Value,Field::STORE_NO | Field::INDEX_TOKENIZED);
}

Field* Field::Text(const TCHAR* Name, Reader* Value, const bool storeTermVector) {
	if ( storeTermVector )
		return _CLNEW Field(Name,Value,Field::INDEX_TOKENIZED | Field::TERMVECTOR_YES);
	else
		return _CLNEW Field(Name,Value,Field::INDEX_TOKENIZED);
}

CL_NS_END
