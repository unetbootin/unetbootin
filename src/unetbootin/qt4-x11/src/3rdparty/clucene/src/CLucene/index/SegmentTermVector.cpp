/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#include "CLucene/StdHeader.h"
#include "TermVector.h"
#include "CLucene/util/StringBuffer.h"

CL_NS_USE(util)
CL_NS_DEF(index)

Array<int32_t> SegmentTermPositionVector::EMPTY_TERM_POS;

SegmentTermVector::SegmentTermVector(const TCHAR* field, TCHAR** terms, Array<int32_t>* termFreqs) {
	this->field = STRDUP_TtoT(field);
	this->terms = terms;
	this->termsLen = -1; //lazily get the size of the terms
	this->termFreqs = termFreqs;
}

SegmentTermVector::~SegmentTermVector(){
  _CLDELETE_CARRAY(field);
  _CLDELETE_CARRAY_ALL(terms);
  
  _CLDELETE_ARRAY(termFreqs->values);
  _CLDELETE(termFreqs);
}
TermPositionVector* SegmentTermVector::__asTermPositionVector(){
	return NULL;
}

const TCHAR* SegmentTermVector::getField() {
return field;
}

TCHAR* SegmentTermVector::toString() const{
StringBuffer sb;
sb.appendChar('{');
sb.append(field);
sb.append(_T(": "));

int32_t i=0;
while ( terms && terms[i] != NULL ){
  if (i>0) 
	  sb.append(_T(", "));
  sb.append(terms[i]);
  sb.appendChar('/');

  sb.appendInt((*termFreqs)[i]);
}
sb.appendChar('}');
return sb.toString();
}

int32_t SegmentTermVector::size() {
if ( terms == NULL )
	return 0;

if ( termsLen == -1 ){
	termsLen=0;
	while ( terms[termsLen] != 0 )
		termsLen++;
}
return termsLen;
}

const TCHAR** SegmentTermVector::getTerms() {
	return (const TCHAR**)terms;
}

const Array<int32_t>* SegmentTermVector::getTermFrequencies() {
	return termFreqs;
}

int32_t SegmentTermVector::binarySearch(TCHAR** a, const int32_t arraylen, const TCHAR* key) const
{
	int32_t low = 0;
	int32_t hi = arraylen - 1;
	int32_t mid = 0;
	while (low <= hi)
	{
		mid = (low + hi) >> 1;

		int32_t c = _tcscmp(a[mid],key);
		if (c==0)
			return mid;
		else if (c > 0)
			hi = mid - 1;
		else // This gets the insertion point right on the last loop.
			low = ++mid;
	}
	return -mid - 1;
}

int32_t SegmentTermVector::indexOf(const TCHAR* termText) {
	if(terms == NULL)
		return -1;
	int32_t res = binarySearch(terms, size(), termText);
	return res >= 0 ? res : -1;
}

void SegmentTermVector::indexesOf(const TCHAR** termNumbers, const int32_t start, const int32_t len, Array<int32_t>& ret) {
	// TODO: there must be a more efficient way of doing this.
	//       At least, we could advance the lower bound of the terms array
	//       as we find valid indexes. Also, it might be possible to leverage
	//       this even more by starting in the middle of the termNumbers array
	//       and thus dividing the terms array maybe in half with each found index.
	ret.length = len;
	ret.values = _CL_NEWARRAY(int32_t,len);
	for (int32_t i=0; i<len; ++i) {
	  ret.values[i] = indexOf(termNumbers[start+ i]);
	}
}



    
SegmentTermPositionVector::SegmentTermPositionVector(const TCHAR* field, TCHAR** terms, Array<int32_t>* termFreqs, Array< Array<int32_t> >* positions, Array< Array<TermVectorOffsetInfo> >* offsets):
	SegmentTermVector(field,terms,termFreqs)
{
	this->offsets = offsets;
	this->positions = positions;
}

SegmentTermPositionVector::~SegmentTermPositionVector(){
	if ( offsets ){
		for (size_t i=0;i<offsets->length;i++){
			if ( offsets->values != NULL ){
				Array<TermVectorOffsetInfo>& offs = offsets->values[i];
				for ( size_t j=0;j<offs.length;j++ ){
					_CLDELETE_ARRAY(offs.values);
				}
			}
		}
		_CLDELETE_ARRAY(offsets->values);
        _CLDELETE(offsets);
	}
	if ( positions ){
		for (size_t i=0;i<positions->length;i++){
			if ( positions->values != NULL ){
				Array<int32_t>& pos = positions->values[i];
				for ( size_t j=0;j<pos.length;j++ ){
					_CLDELETE_ARRAY(pos.values);
				}
			}
		}
        _CLDELETE_ARRAY(positions->values);
		_CLDELETE(positions);
	}
}

TermPositionVector* SegmentTermPositionVector::__asTermPositionVector(){
	return this;
}
/**
* Returns an array of TermVectorOffsetInfo in which the term is found.
*
* @param index The position in the array to get the offsets from
* @return An array of TermVectorOffsetInfo objects or the empty list
* @see org.apache.lucene.analysis.Token
*/
Array<TermVectorOffsetInfo>* SegmentTermPositionVector::getOffsets(int32_t index) {
	if(offsets == NULL)
		return NULL;
	if (index >=0 && index < offsets->length)
		return &offsets->values[index];
	else
		return &TermVectorOffsetInfo::EMPTY_OFFSET_INFO;
}

/**
* Returns an array of positions in which the term is found.
* Terms are identified by the index at which its number appears in the
* term String array obtained from the <code>indexOf</code> method.
*/
Array<int32_t>* SegmentTermPositionVector::getTermPositions(int32_t index) {
	if(positions == NULL)
		return NULL;

	if (index >=0 && index < positions->length)
		return &positions->values[index];
	else
		return &EMPTY_TERM_POS;
}
CL_NS_END

