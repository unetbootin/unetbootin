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
#include "CLucene/util/Misc.h"

CL_NS_USE(util)
CL_NS_DEF(index)

const QLatin1String TermVectorsWriter::LUCENE_TVX_EXTENSION(".tvx");
const QLatin1String TermVectorsWriter::LUCENE_TVD_EXTENSION(".tvd");
const QLatin1String TermVectorsWriter::LUCENE_TVF_EXTENSION(".tvf");

TermVectorsWriter::TermVectorsWriter(CL_NS(store)::Directory* directory, 
    const QString& segment,FieldInfos* fieldInfos)
{
    // Open files for TermVector storage
    tvx = directory->createOutput(segment + LUCENE_TVX_EXTENSION);
    tvx->writeInt(FORMAT_VERSION);

    tvd = directory->createOutput(segment + LUCENE_TVD_EXTENSION);
    tvd->writeInt(FORMAT_VERSION);

    tvf = directory->createOutput(segment + LUCENE_TVF_EXTENSION);
    tvf->writeInt(FORMAT_VERSION);

    this->fieldInfos = fieldInfos;

    currentField = NULL;
    currentDocPointer = -1;
}

TermVectorsWriter::~TermVectorsWriter()
{
    if (tvx != NULL) {
        tvx->close();
        _CLDELETE(tvx);
    }

    if (tvd != NULL) {
        tvd->close();
        _CLDELETE(tvd);
    }

    if (tvf != NULL){
        tvf->close();
        _CLDELETE(tvf);
    }
}

void TermVectorsWriter::openDocument()
{
    closeDocument();
    currentDocPointer = tvd->getFilePointer();
}

void TermVectorsWriter::closeDocument()
{
    if (isDocumentOpen()) {
        closeField();
        writeDoc();
        fields.clear();
        currentDocPointer = -1;
    }
}

bool TermVectorsWriter::isDocumentOpen() const
{
    return currentDocPointer != -1;
}


void TermVectorsWriter::openField(int32_t fieldNumber,
    bool storePositionWithTermVector, bool storeOffsetWithTermVector)
{
    if (!isDocumentOpen())
        _CLTHROWA(CL_ERR_InvalidState,"Cannot open field when no document is open.");

    closeField();
    currentField = _CLNEW TVField(fieldNumber, storePositionWithTermVector,
        storeOffsetWithTermVector);
}

void TermVectorsWriter::openField(const TCHAR* field)
{
    FieldInfo* fieldInfo = fieldInfos->fieldInfo(field);
    openField(fieldInfo->number, fieldInfo->storePositionWithTermVector,
        fieldInfo->storeOffsetWithTermVector);
}

void TermVectorsWriter::closeField()
{
    if (isFieldOpen()) {
        /* DEBUG */
        //System.out.println("closeField()");
        /* DEBUG */

        // save field and terms
        writeField();
        fields.push_back(currentField);
        terms.clear();
        currentField = NULL;
    }
}

bool TermVectorsWriter::isFieldOpen() const
{
    return currentField != NULL;
}

void TermVectorsWriter::addTerm(const TCHAR* termText, int32_t freq, 
    Array<int32_t>* positions, Array<TermVectorOffsetInfo>* offsets)
{
    if (!isDocumentOpen()) 
        _CLTHROWA(CL_ERR_InvalidState, "Cannot add terms when document is not open");
    
    if (!isFieldOpen())
        _CLTHROWA(CL_ERR_InvalidState, "Cannot add terms when field is not open");

    addTermInternal(termText, freq, positions, offsets);
}

void TermVectorsWriter::addTermInternal(const TCHAR* termText, int32_t freq,
    Array<int32_t>* positions, Array<TermVectorOffsetInfo>* offsets)
{
    TVTerm* term = _CLNEW TVTerm();
    term->setTermText(termText);
    term->freq = freq;
    term->positions = positions;
    term->offsets = offsets;
    terms.push_back(term);
}

void TermVectorsWriter::addAllDocVectors(Array<TermFreqVector*>& vectors)
{
    openDocument();

    for (int32_t i = 0; i < vectors.length; ++i) {
        bool storePositionWithTermVector = false;
        bool storeOffsetWithTermVector = false;

        if ( vectors[i]->__asTermPositionVector() != NULL ) {
            TermPositionVector* tpVector = vectors[i]->__asTermPositionVector();

            if (tpVector->size() > 0 && tpVector->getTermPositions(0) != NULL)
                storePositionWithTermVector = true;
            if (tpVector->size() > 0 && tpVector->getOffsets(0) != NULL)
                storeOffsetWithTermVector = true;

            FieldInfo* fieldInfo = fieldInfos->fieldInfo(tpVector->getField());
            openField(fieldInfo->number, storePositionWithTermVector, storeOffsetWithTermVector);

            for (int32_t j = 0; j < tpVector->size(); ++j)
                addTermInternal(tpVector->getTerms()[j], 
                (*tpVector->getTermFrequencies())[j], 
                tpVector->getTermPositions(j),
                tpVector->getOffsets(j));

            closeField();

        } else {
            TermFreqVector* tfVector = vectors[i];

            FieldInfo* fieldInfo = fieldInfos->fieldInfo(tfVector->getField());
            openField(fieldInfo->number, storePositionWithTermVector, storeOffsetWithTermVector);

            for (int32_t j = 0; j < tfVector->size(); ++j)
                addTermInternal(tfVector->getTerms()[j], 
                (*tfVector->getTermFrequencies())[j], NULL, NULL);

            closeField();
        }
    }

    closeDocument();
}


void TermVectorsWriter::close()
{
    try {
        closeDocument();

        // make an effort to close all streams we can but remember and re-throw
        // the first exception encountered in this process
#define _DOTVWCLOSE(x)                                              \
        if (x != NULL) {                                            \
            try {                                                   \
                x->close();                                         \
                _CLDELETE(x)                                        \
            } catch (CLuceneError& e) {                             \
                if ( e.number() != CL_ERR_IO )                      \
                    throw e;                                        \
                if (ikeep == 0)                                     \
                    ikeep = e.number();                             \
                if (keep[0] == 0)                                   \
                    strcpy(keep,e.what());                          \
            } catch (...) {                                         \
                if (keep[0] == 0)                                   \
                    strcpy(keep, "Unknown error while closing " #x);\
            }                                                       \
        }
    } _CLFINALLY ( \
        char keep[200];
        int32_t ikeep = 0;
        keep[0] = 0;
        _DOTVWCLOSE(tvx);
        _DOTVWCLOSE(tvd);
        _DOTVWCLOSE(tvf);
        if (keep[0] != 0)
            _CLTHROWA(ikeep, keep);
    );
}

void TermVectorsWriter::writeField()
{
    // remember where this field is written
    currentField->tvfPointer = tvf->getFilePointer();
    //System.out.println("Field Pointer: " + currentField.tvfPointer);
    int32_t size = terms.size();

    tvf->writeVInt(size);

    bool storePositions = currentField->storePositions;
    bool storeOffsets = currentField->storeOffsets;
    uint8_t bits = 0x0;
    if (storePositions) 
        bits |= STORE_POSITIONS_WITH_TERMVECTOR;
    if (storeOffsets) 
        bits |= STORE_OFFSET_WITH_TERMVECTOR;
    tvf->writeByte(bits);

    const TCHAR* lastTermText = LUCENE_BLANK_STRING;
    int32_t lastTermTextLen = 0;

    for (int32_t i = 0; i < size; ++i) {
        TVTerm* term = terms[i];
        int32_t start = CL_NS(util)::Misc::stringDifference(lastTermText,
            lastTermTextLen, term->getTermText(),term->getTermTextLen());
        int32_t length = term->getTermTextLen() - start;
        tvf->writeVInt(start);			  // write shared prefix length
        tvf->writeVInt(length);			  // write delta length
        tvf->writeChars(term->getTermText(), start, length);  // write delta chars
        tvf->writeVInt(term->freq);

        lastTermText = term->getTermText();
        lastTermTextLen = term->getTermTextLen();

        if (storePositions) {
            if(term->positions == NULL)
                _CLTHROWA(CL_ERR_IllegalState, "Trying to write positions that are NULL!");

            // use delta encoding for positions
            int32_t position = 0;
            for (int32_t j = 0; j < term->freq; ++j){
                tvf->writeVInt((*term->positions)[j] - position);
                position = (*term->positions)[j];
            }
        }

        if (storeOffsets) {
            if(term->offsets == NULL)
                _CLTHROWA(CL_ERR_IllegalState, "Trying to write offsets that are NULL!");

            // use delta encoding for offsets
            int32_t position = 0;
            for (int32_t j = 0; j < term->freq; ++j) {
                tvf->writeVInt((*term->offsets)[j].getStartOffset() - position);
                 //Save the diff between the two.
                tvf->writeVInt((*term->offsets)[j].getEndOffset() -
                    (*term->offsets)[j].getStartOffset());
                position = (*term->offsets)[j].getEndOffset();
            }
        }
    }
}

void TermVectorsWriter::writeDoc()
{
    if (isFieldOpen()) {
        _CLTHROWA(CL_ERR_InvalidState,
            "Field is still open while writing document");
    }

    // write document index record
    tvx->writeLong(currentDocPointer);

    // write document data record
    int32_t size = fields.size();

    // write the number of fields
    tvd->writeVInt(size);

    // write field numbers
    for (int32_t j = 0; j < size; ++j) {
        tvd->writeVInt(fields[j]->number);
    }

    // write field pointers
    int64_t lastFieldPointer = 0;
    for (int32_t i = 0; i < size; ++i) {
        TVField* field = (TVField*) fields[i];
        tvd->writeVLong(field->tvfPointer - lastFieldPointer);

        lastFieldPointer = field->tvfPointer;
    }
}

const TCHAR* TermVectorsWriter::TVTerm::getTermText() const
{
    return termText;
}

size_t TermVectorsWriter::TVTerm::getTermTextLen()
{
    if (termTextLen==-1)
        termTextLen = _tcslen(termText);
    return termTextLen; 
}

void TermVectorsWriter::TVTerm::setTermText(const TCHAR* val)
{
    _CLDELETE_CARRAY(termText);
    termText = STRDUP_TtoT(val);
    termTextLen = -1;

}

TermVectorsWriter::TVTerm::TVTerm()
    : freq(0)
    , positions(NULL)
    , offsets(NULL)
{
    termText=NULL;  
    termTextLen=-1;
}

TermVectorsWriter::TVTerm::~TVTerm()
{
    _CLDELETE_CARRAY(termText)
}

CL_NS_END
