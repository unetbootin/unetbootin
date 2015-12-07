/*
 * Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
 *
 * Distributable under the terms of either the Apache License (Version 2.0) or 
 * the GNU Lesser General Public License, as specified in the COPYING file.
 *
 * Changes are Copyright(C) 2007, 2008 by Nokia Corporation and/or its subsidiary(-ies), all rights reserved.
*/
#include "CLucene/StdHeader.h"
#include "TermVector.h"
#include "CLucene/util/StringBuffer.h"

CL_NS_USE(util)
CL_NS_DEF(index)

TermVectorsReader::TermVectorsReader(CL_NS(store)::Directory* d,
    const QString& segment, FieldInfos* fieldInfos)
{
	if (d->fileExists(segment + TermVectorsWriter::LUCENE_TVX_EXTENSION)) {
      tvx = d->openInput(segment + TermVectorsWriter::LUCENE_TVX_EXTENSION);
      checkValidFormat(tvx);
	  
	  tvd = d->openInput(segment + TermVectorsWriter::LUCENE_TVD_EXTENSION);
      tvdFormat = checkValidFormat(tvd);
	  
	  tvf = d->openInput(segment + TermVectorsWriter::LUCENE_TVF_EXTENSION);
      tvfFormat = checkValidFormat(tvf);

      _size = tvx->length() / 8;
	}else{
	  tvx = NULL;
	  tvd = NULL;
	  tvf = NULL;
	  _size = 0;
	}

    this->fieldInfos = fieldInfos;
}

TermVectorsReader::TermVectorsReader(const TermVectorsReader& copy)
{
    tvx = copy.tvx->clone();
    tvd = copy.tvd->clone();
    tvf = copy.tvf->clone();
	
    tvdFormat = copy.tvdFormat;
    tvfFormat = copy.tvfFormat;
    _size = copy._size;
    fieldInfos = copy.fieldInfos;
}
TermVectorsReader* TermVectorsReader::clone() const{
	if (tvx == NULL || tvd == NULL || tvf == NULL)
		return NULL;
	return _CLNEW TermVectorsReader(*this);
}

TermVectorsReader::~TermVectorsReader(){
	close();
}

void TermVectorsReader::close(){
	// why don't we trap the exception and at least make sure that
    // all streams that we can close are closed?
	CLuceneError keep(0,"",false);
	bool thrown = false;

	if (tvx != NULL){
		try{
			tvx->close();
		}catch(CLuceneError& err){
			if ( err.number() == CL_ERR_IO ){
				keep = err;
				thrown = true;
			}else
				throw err;
		}
		_CLDELETE(tvx);//delete even  if error thrown
	}
    if (tvd != NULL){
		try{
			tvd->close();
		}catch(CLuceneError& err){
			if ( err.number() == CL_ERR_IO ){
				keep = err;
				thrown = true;
			}else
				throw err;
		}
		_CLDELETE(tvd);
	}
    if (tvf != NULL){
		try{
			tvf->close();
		}catch(CLuceneError& err){
			if ( err.number() == CL_ERR_IO ){
				keep = err;
				thrown = true;
			}else
				throw err;
		}
		_CLDELETE(tvf);
	}

	if ( thrown )
		throw keep;
}

TermFreqVector* TermVectorsReader::get(const int32_t docNum, const TCHAR* field){
	// Check if no term vectors are available for this segment at all
    int32_t fieldNumber = fieldInfos->fieldNumber(field);
    TermFreqVector* result = NULL;
    if (tvx != NULL) {
		//We need to account for the FORMAT_SIZE at when seeking in the tvx
		//We don't need to do this in other seeks because we already have the
		// file pointer
		//that was written in another file
        tvx->seek((docNum * 8L) + TermVectorsWriter::FORMAT_SIZE);
        int64_t position = tvx->readLong();

        tvd->seek(position);
        int32_t fieldCount = tvd->readVInt();
        // There are only a few fields per document. We opt for a full scan
        // rather then requiring that they be ordered. We need to read through
        // all of the fields anyway to get to the tvf pointers.
        int32_t number = 0;
        int32_t found = -1;
        for (int32_t i = 0; i < fieldCount; ++i) {
			if(tvdFormat == TermVectorsWriter::FORMAT_VERSION)
				number = tvd->readVInt();
			else
				number += tvd->readVInt();
          if (number == fieldNumber) 
			  found = i;
        }
  
		// This field, although valid in the segment, was not found in this
		// document
		if (found != -1) {
          // Compute position in the tvf file
          position = 0;
          for (int32_t i = 0; i <= found; ++i)
            position += tvd->readVLong();
          result = readTermVector(field, position);
        }
    }
    return result;
}


bool TermVectorsReader::get(int32_t docNum, Array<TermFreqVector*>& result){
    // Check if no term vectors are available for this segment at all
    if (tvx != NULL) {
        //We need to offset by
		tvx->seek((docNum * 8L) + TermVectorsWriter::FORMAT_SIZE);
        int64_t position = tvx->readLong();

        tvd->seek(position);
        int32_t fieldCount = tvd->readVInt();

        // No fields are vectorized for this document
        if (fieldCount != 0) {
            int32_t number = 0;
            const TCHAR** fields = _CL_NEWARRAY(const TCHAR*,fieldCount+1);
    		
			{ //msvc6 scope fix
				for (int32_t i = 0; i < fieldCount; ++i) {
					if(tvdFormat == TermVectorsWriter::FORMAT_VERSION)
						number = tvd->readVInt();
					else
						number += tvd->readVInt();
				    fields[i] = fieldInfos->fieldName(number);
				}
			}
			fields[fieldCount]=NULL;
		  
		    // Compute position in the tvf file
		    position = 0;
		    int64_t* tvfPointers = _CL_NEWARRAY(int64_t,fieldCount);
			{ //msvc6 scope fix
				for (int32_t i = 0; i < fieldCount; ++i) {
				    position += tvd->readVLong();
				    tvfPointers[i] = position;
				}
			}

			readTermVectors(fields, tvfPointers, fieldCount, result);
            _CLDELETE_ARRAY(tvfPointers);
            _CLDELETE_ARRAY(fields);
        }
		return true;
    }
	return false;
}


int32_t TermVectorsReader::checkValidFormat(CL_NS(store)::IndexInput* in)
{
	int32_t format = in->readInt();
	if (format > TermVectorsWriter::FORMAT_VERSION)
	{
		CL_NS(util)::StringBuffer err;
		err.append(_T("Incompatible format version: "));
		err.appendInt(format);
		err.append(_T(" expected "));
		err.appendInt(TermVectorsWriter::FORMAT_VERSION);
		err.append(_T(" or less"));
		_CLTHROWT(CL_ERR_Runtime,err.getBuffer());
	}
	return format;
}

void TermVectorsReader::readTermVectors(const TCHAR** fields,
    const int64_t* tvfPointers, const int32_t len, Array<TermFreqVector*>& result)
{
	result.length = len;
	result.values = _CL_NEWARRAY(TermFreqVector*,len);
    for (int32_t i = 0; i < len; ++i) {
      result.values[i] = readTermVector(fields[i], tvfPointers[i]);
    }
}

SegmentTermVector* TermVectorsReader::readTermVector(const TCHAR* field,
    const int64_t tvfPointer)
{
	// Now read the data from specified position. We don't need to offset by
    // the FORMAT here since the pointer already includes the offset
    tvf->seek(tvfPointer);

    int32_t numTerms = tvf->readVInt();
    // If no terms - return a constant empty termvector. However, this should never occur!
    if (numTerms == 0) 
		return _CLNEW SegmentTermVector(field, NULL, NULL);

	bool storePositions;
    bool storeOffsets;

	if(tvfFormat == TermVectorsWriter::FORMAT_VERSION){
		uint8_t bits = tvf->readByte();
		storePositions = (bits & TermVectorsWriter::STORE_POSITIONS_WITH_TERMVECTOR) != 0;
		storeOffsets = (bits & TermVectorsWriter::STORE_OFFSET_WITH_TERMVECTOR) != 0;
	}
	else{
		tvf->readVInt();
		storePositions = false;
		storeOffsets = false;
	}

    TCHAR** terms = _CL_NEWARRAY(TCHAR*,numTerms+1);
    Array<int32_t>* termFreqs = _CLNEW Array<int32_t>(numTerms);

    //  we may not need these, but declare them
    Array< Array<int32_t> >* positions = NULL;
	Array< Array<TermVectorOffsetInfo> >* offsets = NULL;
	if(storePositions){
		Array<int32_t>* tmp = _CL_NEWARRAY(Array<int32_t>,numTerms);
		positions = _CLNEW Array< Array<int32_t> >(tmp, numTerms);
	}
	if(storeOffsets){
		Array<TermVectorOffsetInfo>* tmp = _CL_NEWARRAY(Array<TermVectorOffsetInfo>,numTerms);
		offsets = _CLNEW Array< Array<TermVectorOffsetInfo> >(tmp, numTerms);
	}

    int32_t start = 0;
    int32_t deltaLength = 0;
    int32_t totalLength = 0;
	int32_t bufferLen=10; // init the buffer with a length of 10 character
	TCHAR* buffer = (TCHAR*)malloc(bufferLen * sizeof(TCHAR));
	
    for (int32_t i = 0; i < numTerms; ++i) {
		start = tvf->readVInt();
		deltaLength = tvf->readVInt();
		totalLength = start + deltaLength;
		if (bufferLen < totalLength) // increase buffer
		{
			buffer=(TCHAR*)realloc(buffer,totalLength * sizeof(TCHAR));
			bufferLen = totalLength;
		}

		//read the term
		tvf->readChars(buffer, start, deltaLength);
		terms[i] = _CL_NEWARRAY(TCHAR,totalLength+1);
		_tcsncpy(terms[i],buffer,totalLength);
		terms[i][totalLength] = '\0'; //null terminate term

		//read the frequency
		int32_t freq = tvf->readVInt();
		termFreqs->values[i] = freq;

		if (storePositions) { //read in the positions
			Array<int32_t>& pos = positions->values[i];
			pos.length = freq;
			pos.values = _CL_NEWARRAY(int32_t,freq);

			int32_t prevPosition = 0;
			for (int32_t j = 0; j < freq; ++j)
			{
				pos.values[j] = prevPosition + tvf->readVInt();
				prevPosition = pos.values[j];
			}
		}

		if (storeOffsets) {
			Array<TermVectorOffsetInfo>& offs = offsets->values[i];
			offs.length = freq;
			offs.values = _CL_NEWARRAY(TermVectorOffsetInfo,freq);

			int32_t prevOffset = 0;
			for (int32_t j = 0; j < freq; ++j) {
				int32_t startOffset = prevOffset + tvf->readVInt();
				int32_t endOffset = startOffset + tvf->readVInt();
				offs.values[j].setStartOffset(startOffset);
				offs.values[j].setEndOffset(endOffset);
				prevOffset = endOffset;
			}
		}
    }
    free(buffer);
	terms[numTerms]=NULL; //null terminate terms array

	SegmentTermVector* tv = NULL;
	if (storePositions || storeOffsets){
	  return _CLNEW SegmentTermPositionVector(field, terms, termFreqs, positions, offsets);
	}else {
	  return _CLNEW SegmentTermVector(field, terms, termFreqs);
	}
}

int64_t TermVectorsReader::size()
{
    return _size;
}
 



Array<TermVectorOffsetInfo> TermVectorOffsetInfo::EMPTY_OFFSET_INFO;

TermVectorOffsetInfo::TermVectorOffsetInfo()
{
	startOffset = 0;
	endOffset=0;
}

TermVectorOffsetInfo::~TermVectorOffsetInfo()
{
}

TermVectorOffsetInfo::TermVectorOffsetInfo(int32_t startOffset, int32_t endOffset)
{
	this->endOffset = endOffset;
	this->startOffset = startOffset;
}

int32_t TermVectorOffsetInfo::getEndOffset() const
{
	return endOffset;
}

void TermVectorOffsetInfo::setEndOffset(int32_t endOffset)
{
	this->endOffset = endOffset;
}

int32_t TermVectorOffsetInfo::getStartOffset() const
{
	return startOffset;
}

void TermVectorOffsetInfo::setStartOffset(int32_t startOffset)
{
	this->startOffset = startOffset;
}

bool TermVectorOffsetInfo::equals(TermVectorOffsetInfo* termVectorOffsetInfo)
{
	if (this == termVectorOffsetInfo) 
		return true;

	if (endOffset != termVectorOffsetInfo->endOffset) return false;
	if (startOffset != termVectorOffsetInfo->startOffset) return false;

	return true;
}

size_t TermVectorOffsetInfo::hashCode() const
{
	size_t result;
	result = startOffset;
	result = 29 * result + endOffset;
	return result;
}

CL_NS_END
