/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#include "CLucene/StdHeader.h"
#include "FieldCache.h"
#include "FieldCacheImpl.h"

CL_NS_DEF(search)

FieldCache* FieldCache::DEFAULT = _CLNEW FieldCacheImpl();
int32_t FieldCache::STRING_INDEX = -1;

FieldCacheAuto::FieldCacheAuto(int32_t len, int32_t type){
	contentType = type;
	contentLen = len;
	ownContents = false;
	
	intArray=NULL;
	floatArray=NULL;
	stringIndex=NULL;
	stringArray=NULL;
	comparableArray=NULL;
	sortComparator=NULL;
	scoreDocComparator=NULL;
}
FieldCacheAuto::~FieldCacheAuto(){
	if ( contentType == FieldCacheAuto::INT_ARRAY ){
		_CLDELETE_ARRAY(intArray);
    }else if ( contentType == FieldCacheAuto::FLOAT_ARRAY ){
		_CLDELETE_ARRAY(floatArray);
	}else if ( contentType == FieldCacheAuto::STRING_INDEX ){
		_CLDELETE(stringIndex);
    }else if ( contentType == FieldCacheAuto::STRING_ARRAY ){
		if ( ownContents ){
			for ( int32_t i=0;i<contentLen;i++ )
				_CLDELETE_CARRAY(stringArray[i]);
		}
		_CLDELETE_ARRAY(stringArray);
	}else if ( contentType == FieldCacheAuto::COMPARABLE_ARRAY ){
		if ( ownContents ){
			for ( int32_t i=0;i<contentLen;i++ )
				_CLDELETE(comparableArray[i]);
		}
		_CLDELETE_ARRAY(comparableArray);
	}else if ( contentType == FieldCacheAuto::SORT_COMPARATOR ){
		_CLDELETE(sortComparator);
	}else if ( contentType == FieldCacheAuto::SCOREDOC_COMPARATOR ){
		_CLDELETE(scoreDocComparator);
	}
}

CL_NS_END
