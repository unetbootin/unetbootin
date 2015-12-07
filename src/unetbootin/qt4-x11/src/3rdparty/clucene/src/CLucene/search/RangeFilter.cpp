/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#include "CLucene/StdHeader.h"
#include "RangeFilter.h"

CL_NS_DEF(search)
CL_NS_USE(index)
CL_NS_USE(util)
CL_NS_USE(document)


RangeFilter::RangeFilter( const TCHAR* fieldName, const TCHAR* lowerTerm, const TCHAR* upperTerm, bool includeLower, bool includeUpper )
{
	this->field = STRDUP_TtoT(fieldName);
	if ( lowerTerm != NULL )
		this->lowerValue = STRDUP_TtoT(lowerTerm);
	else
		this->lowerValue = NULL;
	if ( upperTerm != NULL )
		this->upperValue = STRDUP_TtoT(upperTerm);
	else
		this->upperValue = NULL;
	this->includeLower = includeLower;
	this->includeUpper = includeUpper;
}


/**
 * Constructs a filter for field <code>fieldName</code> matching
 * less than or equal to <code>upperTerm</code>.
 */
RangeFilter* RangeFilter::Less( TCHAR* fieldName, TCHAR* upperTerm ) {
	return new RangeFilter( fieldName, NULL, upperTerm, false, true );
}


/**
* Constructs a filter for field <code>fieldName</code> matching
* more than or equal to <code>lowerTerm</code>.
*/
RangeFilter* RangeFilter::More( TCHAR* fieldName, TCHAR* lowerTerm ) {
	return new RangeFilter( fieldName, lowerTerm, NULL, true, false );
}


RangeFilter::~RangeFilter()
{
	_CLDELETE_CARRAY( lowerValue );
	_CLDELETE_CARRAY( field );
	_CLDELETE_CARRAY( upperValue );
}


RangeFilter::RangeFilter( const RangeFilter& copy ) : 
	field( STRDUP_TtoT(copy.field) ),
	lowerValue( STRDUP_TtoT(copy.lowerValue) ), 
	upperValue( STRDUP_TtoT(copy.upperValue) ),
	includeLower( copy.includeLower ),
	includeUpper( copy.includeUpper )
{
}


Filter* RangeFilter::clone() const {
	return _CLNEW RangeFilter(*this );
}


TCHAR* RangeFilter::toString()
{
	size_t len = (field ? _tcslen(field) : 0) + (lowerValue ? _tcslen(lowerValue) : 0) + (upperValue ? _tcslen(upperValue) : 0) + 8;
	TCHAR* ret = _CL_NEWARRAY( TCHAR, len );
	ret[0] = 0;
	_sntprintf( ret, len, _T("%s: [%s-%s]"), field, (lowerValue?lowerValue:_T("")), (upperValue?upperValue:_T("")) );
	
	return ret;
}


/** Returns a BitSet with true for documents which should be permitted in
search results, and false for those that should not. */
BitSet* RangeFilter::bits( IndexReader* reader )
{
	BitSet* bts = _CLNEW BitSet( reader->maxDoc() );
	Term* term = NULL;
	
	Term* t = _CLNEW Term( field, (lowerValue ? lowerValue : _T("")), false );
	TermEnum* enumerator = reader->terms( t );	// get enumeration of all terms after lowerValue
	_CLDECDELETE( t );
	
	if( enumerator->term(false) == NULL ) {
		_CLDELETE( enumerator );
		return bts;
	}
	
	bool checkLower = false;
	if( !includeLower ) // make adjustments to set to exclusive
		checkLower = true;
	
	TermDocs* termDocs = reader->termDocs();
	
	try
	{
		do
		{
			term = enumerator->term();
			
			if( term == NULL || _tcscmp(term->field(), field) )
				break;
			
			if( !checkLower || lowerValue == NULL || _tcscmp(term->text(), lowerValue) > 0 )
			{
				checkLower = false;
				if( upperValue != NULL )
				{
					int compare = _tcscmp( upperValue, term->text() );
					
					/* if beyond the upper term, or is exclusive and
					 * this is equal to the upper term, break out */
					if( (compare < 0) || (!includeUpper && compare == 0) )
						break;
				}
				
				termDocs->seek( enumerator->term(false) );
				while( termDocs->next() ) {
					bts->set( termDocs->doc() );
				}
			}
			
			_CLDECDELETE( term );
		}
		while( enumerator->next() );
	}
	_CLFINALLY
	(
		_CLDECDELETE( term );
		termDocs->close();
		_CLVDELETE( termDocs );
		enumerator->close();
		_CLDELETE( enumerator );
	);
	
	return bts;
}

CL_NS_END
