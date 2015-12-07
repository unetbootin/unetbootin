/*
 * Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
 *
 * Distributable under the terms of either the Apache License (Version 2.0) or 
 * the GNU Lesser General Public License, as specified in the COPYING file.
 *
 * Changes are Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
*/
#include "CLucene/StdHeader.h"
#include "SegmentHeader.h"

#include "FieldInfos.h"
#include "FieldsReader.h"
#include "IndexReader.h"
#include "TermInfosReader.h"
#include "Terms.h"
#include "CLucene/search/Similarity.h"

CL_NS_USE(util)
CL_NS_USE(store)
CL_NS_USE(document)
CL_NS_USE(search)
CL_NS_DEF(index)

SegmentReader::Norm::Norm(IndexInput* instrm, int32_t n, SegmentReader* r,
    const QString& seg)
    : number(n)
    , reader(r)
    , segment(seg)
    , in(instrm)
    , bytes(NULL)
    , dirty(false)
{
    //Func - Constructor
    //Pre  - instrm is a valid reference to an IndexInput
    //Post - A Norm instance has been created with an empty bytes array
    bytes = NULL;
    dirty = false;
}

SegmentReader::Norm::~Norm()
{
    //Func - Destructor
    //Pre  - true
    //Post - The IndexInput in has been deleted (and closed by its destructor) 
    //       and the array too.

    //Close and destroy the inputstream in-> The inputstream will be closed
    // by its destructor. Note that the IndexInput 'in' actually is a pointer!!!!!  
    _CLDELETE(in);

    //Delete the bytes array
    _CLDELETE_ARRAY(bytes);

}

void SegmentReader::Norm::reWrite()
{
    QString buf(segment + QLatin1String(".tmp"));

    // NOTE: norms are re-written in regular directory, not cfs
    IndexOutput* out = reader->getDirectory()->createOutput(buf);
    try {
        out->writeBytes(bytes, reader->maxDoc());
    } _CLFINALLY (
        out->close();
        _CLDELETE(out)
        );

    QString fileName(segment);
    if (reader->cfsReader == NULL)
        fileName.append(QLatin1String(".f%1")).arg(number);
    else // use a different file name if we have compound format
        fileName.append(QLatin1String(".s%1")).arg(number);

    reader->getDirectory()->renameFile(buf, fileName);
    this->dirty = false;
}

SegmentReader::SegmentReader(SegmentInfo* si)
    : IndexReader(si->getDir())
    , _norms(false, false)
{ 
    initialize(si);
}

SegmentReader::SegmentReader(SegmentInfos* sis, SegmentInfo* si)
    : IndexReader(si->getDir(), sis, false)
    , _norms(false, false)
{ 
    initialize(si);
}

void SegmentReader::initialize(SegmentInfo* si)
{
    //Pre  - si-> is a valid reference to SegmentInfo instance
    //       identified by si->
    //Post - All files of the segment have been read

    deletedDocs      = NULL;
    ones			   = NULL;
    //There are no documents yet marked as deleted
    deletedDocsDirty = false;

    normsDirty=false;
    undeleteAll=false;

    //Duplicate the name of the segment from SegmentInfo to segment
    segment          = si->name;
    // make sure that all index files have been read or are kept open
    // so that if an index update removes them we'll still have them
    freqStream       = NULL;
    proxStream       = NULL;

    //instantiate a buffer large enough to hold a directory path
    QString buf;

    // Use compound file directory for some files, if it exists
    Directory* cfsDir = getDirectory();
    SegmentName(buf, CL_MAX_PATH, QLatin1String(".cfs"));
    if (cfsDir->fileExists(buf)) {
        cfsReader = _CLNEW CompoundFileReader(cfsDir, buf);
        cfsDir = cfsReader;
    }else
        cfsReader = NULL;

    // Create the name of the field info file with suffix .fnm in buf
    SegmentName(buf, CL_MAX_PATH, QLatin1String(".fnm"));
    fieldInfos = _CLNEW FieldInfos(cfsDir, buf );

    // Condition check to see if fieldInfos points to a valid instance
    CND_CONDITION(fieldInfos != NULL,
        "No memory could be allocated for fieldInfos");

    // Create the name of the frequence file with suffix .frq in buf
    SegmentName(buf ,CL_MAX_PATH, QLatin1String(".frq"));

    // Open an IndexInput freqStream to the frequency file
    freqStream = cfsDir->openInput( buf );

    // Condition check to see if freqStream points to a valid instance and was
    // able to open the frequency file
    CND_CONDITION(freqStream != NULL,
        "IndexInput freqStream could not open the frequency file");

    // Create the name of the prox file with suffix .prx in buf
    SegmentName(buf, CL_MAX_PATH, QLatin1String(".prx"));

    // Open an IndexInput proxStream to the prox file
    proxStream = cfsDir->openInput( buf );

    // Condition check to see if proxStream points to a valid instance and was
    // able to open the prox file
    CND_CONDITION(proxStream != NULL,
        "IndexInput proxStream could not open proximity file");

    // Instantiate a FieldsReader for reading the Field Info File
    fieldsReader = _CLNEW FieldsReader(cfsDir, segment, fieldInfos);

    // Condition check to see if fieldsReader points to a valid instance 
    CND_CONDITION(fieldsReader != NULL,
        "No memory could be allocated for fieldsReader");

    //Instantiate a TermInfosReader for reading the Term Dictionary .tis file
    tis = _CLNEW TermInfosReader(cfsDir, segment, fieldInfos);

    //Condition check to see if tis points to a valid instance 
    CND_CONDITION(tis != NULL,"No memory could be allocated for tis");

    // Check if the segment has deletion according to the SegmentInfo instance
    // si-> NOTE: the bitvector is stored using the regular directory, not cfs
    if (hasDeletions(si)) {
        //Create a deletion file with suffix .del          
        SegmentName(buf, CL_MAX_PATH, QLatin1String(".del"));
        // Instantiate a BitVector that manages which documents have been deleted
        deletedDocs = _CLNEW BitSet(getDirectory(), buf);
    }

    // Open the norm file. There's a norm file for each indexed field with a
    // byte for each document. The .f[0-9]* file contains, for each document,
    // a byte that encodes a value that is multiplied into the score for hits
    // on that field
    openNorms(cfsDir);

    termVectorsReaderOrig = NULL;
    if (fieldInfos->hasVectors()) // open term vector files only as needed
        termVectorsReaderOrig = _CLNEW TermVectorsReader(cfsDir, segment, fieldInfos); 
}

SegmentReader::~SegmentReader()
{
    //Func - Destructor.
    //Pre  - doClose has been invoked!
    //Post - the instance has been destroyed

    doClose(); //this means that index reader doesn't need to be closed manually

    _CLDELETE(fieldInfos);
    _CLDELETE(fieldsReader);
    _CLDELETE(tis);	      
    _CLDELETE(freqStream);
    _CLDELETE(proxStream);
    _CLDELETE(deletedDocs);
    _CLDELETE_ARRAY(ones);
    _CLDELETE(termVectorsReaderOrig);
    _CLDECDELETE(cfsReader);
}

void SegmentReader::doCommit()
{
    QString bufdel(segment + QLatin1String(".del"));

    if (deletedDocsDirty) {               // re-write deleted 
        QString buftmp(segment + QLatin1String(".tmp"));
        deletedDocs->write(getDirectory(), buftmp);
        getDirectory()->renameFile(buftmp, bufdel);
    }

    if(undeleteAll && getDirectory()->fileExists(bufdel))
        getDirectory()->deleteFile(bufdel, true);

    if (normsDirty) {               // re-write norms 
        NormsType::iterator itr = _norms.begin();
        while (itr != _norms.end()) {
            Norm* norm = itr->second;
            if (norm->dirty) {
                norm->reWrite();
            }
            ++itr;
        }
    }
    deletedDocsDirty = false;
    normsDirty = false;
    undeleteAll = false;
}

void SegmentReader::doClose()
{
    //Func - Closes all streams to the files of a single segment
    //Pre  - fieldsReader != NULL
    //       tis != NULL
    //Post - All streams to files have been closed

    CND_PRECONDITION(fieldsReader != NULL, "fieldsReader is NULL");
    CND_PRECONDITION(tis != NULL, "tis is NULL");

    //Close the fieldsReader
    fieldsReader->close();
    //Close the TermInfosReader
    tis->close();

    //Close the frequency stream
    if (freqStream != NULL){
        freqStream->close();
    }
    //Close the prox stream
    if (proxStream != NULL){
        proxStream->close();
    }

    //Close the norm file
    closeNorms();

    if (termVectorsReaderOrig != NULL) 
        termVectorsReaderOrig->close();

    if (cfsReader != NULL)
        cfsReader->close();
}

bool SegmentReader::hasDeletions() const
{
    return deletedDocs != NULL;
}

//static 
bool SegmentReader::usesCompoundFile(SegmentInfo* si)
{
    return si->getDir()->fileExists(si->name + QLatin1String(".cfs"));
}

//static
bool SegmentReader::hasSeparateNorms(SegmentInfo* si)
{
    QString pattern(si->name);
    pattern.append(QLatin1String(".s"));
    size_t patternLength = pattern.length();

    QStringList names = si->getDir()->list();
    foreach (const QString& name, names) {
        int length = name.length();
        if (length > patternLength && name.left(patternLength) == pattern) {
            if (name.at(patternLength) >= QLatin1Char('0')
                && name.at(patternLength) <= QLatin1Char('9')) {
                return true;
            }
        }
    }
    return false;
}

bool SegmentReader::hasDeletions(const SegmentInfo* si)
{
    //Func - Static method
    //       Checks if a segment managed by SegmentInfo si-> has deletions
    //Pre  - si-> holds a valid reference to an SegmentInfo instance
    //Post - if the segement contains deleteions true is returned otherwise flas

    //Check if the deletion file exists and return the result
    QString f;
    Misc::segmentname(f, CL_MAX_PATH, si->name, QLatin1String(".del"), -1);
    return si->getDir()->fileExists(f);
}

//synchronized
void SegmentReader::doDelete(const int32_t docNum)
{
    //Func - Marks document docNum as deleted
    //Pre  - docNum >=0 and DocNum < maxDoc() 
    //       docNum contains the number of the document that must be 
    //       marked deleted
    //Post - The document identified by docNum has been marked deleted

    SCOPED_LOCK_MUTEX(THIS_LOCK)

    CND_PRECONDITION(docNum >= 0, "docNum is a negative number");
    CND_PRECONDITION(docNum < maxDoc(),
        "docNum is bigger than the total number of documents");

    //Check if deletedDocs exists
    if (deletedDocs == NULL) {
        deletedDocs = _CLNEW BitSet(maxDoc());

        //Condition check to see if deletedDocs points to a valid instance
        CND_CONDITION(deletedDocs != NULL,
            "No memory could be allocated for deletedDocs");
    }
    //Flag that there are documents marked deleted
    deletedDocsDirty = true;
    undeleteAll = false;
    //Mark document identified by docNum as deleted
    deletedDocs->set(docNum);

}

void SegmentReader::doUndeleteAll()
{
    _CLDELETE(deletedDocs);
    deletedDocsDirty = false;
    undeleteAll = true;
}

void SegmentReader::files(QStringList& retarray)
{
    //Func - Returns all file names managed by this SegmentReader
    //Pre  - segment != NULL
    //Post - All filenames managed by this SegmentRead have been returned

    CND_PRECONDITION(segment != NULL, "segment is NULL");

    QString temp;
    #define _ADD_SEGMENT(ext) \
        temp = SegmentName(ext); \
        if (getDirectory()->fileExists(temp)) \
            retarray.push_back(temp);

    //Add the name of the Field Info file
    _ADD_SEGMENT(QLatin1String(".cfs"));
    _ADD_SEGMENT(QLatin1String(".fnm"));
    _ADD_SEGMENT(QLatin1String(".fdx"));
    _ADD_SEGMENT(QLatin1String(".fdt"));
    _ADD_SEGMENT(QLatin1String(".tii"));
    _ADD_SEGMENT(QLatin1String(".tis"));
    _ADD_SEGMENT(QLatin1String(".frq"));
    _ADD_SEGMENT(QLatin1String(".prx"));
    _ADD_SEGMENT(QLatin1String(".del"));
    _ADD_SEGMENT(QLatin1String(".tvx"));
    _ADD_SEGMENT(QLatin1String(".tvd"));
    _ADD_SEGMENT(QLatin1String(".tvf"));
    _ADD_SEGMENT(QLatin1String(".tvp"));

    //iterate through the field infos
    for (int32_t i = 0; i < fieldInfos->size(); ++i) {
        //Get the field info for the i-th field   
        FieldInfo* fi = fieldInfos->fieldInfo(i);
        //Check if the field has been indexed
        if (fi->isIndexed && !fi->omitNorms) {
            QString name;
            if (cfsReader == NULL)
                name = SegmentName(QLatin1String(".f"), i);
            else
                name = SegmentName(QLatin1String(".s"), i);
            
            //The field has been indexed so add its norm file
            if (getDirectory()->fileExists(name))
                retarray.push_back(name);
        }
    }
}

TermEnum* SegmentReader::terms() const
{
    //Func - Returns an enumeration of all the Terms and TermInfos in the set. 
    //Pre  - tis != NULL
    //Post - An enumeration of all the Terms and TermInfos in the set has been returned

    CND_PRECONDITION(tis != NULL, "tis is NULL");

    return tis->terms();
}

TermEnum* SegmentReader::terms(const Term* t) const
{
    //Func - Returns an enumeration of terms starting at or after the named term t 
    //Pre  - t != NULL
    //       tis != NULL
    //Post - An enumeration of terms starting at or after the named term t 

    CND_PRECONDITION(t   != NULL, "t is NULL");
    CND_PRECONDITION(tis != NULL, "tis is NULL");

    return tis->terms(t);
}

bool SegmentReader::document(int32_t n, Document* doc)
{
    //Func - Returns a document identified by n
    //Pre  - n >=0 and identifies the document n
    //Post - if the document has been deleted then an exception has been thrown
    //       otherwise a reference to the found document has been returned

    SCOPED_LOCK_MUTEX(THIS_LOCK)

    CND_PRECONDITION(n >= 0, "n is a negative number");

    //Check if the n-th document has been marked deleted
    if (isDeleted(n))
        _CLTHROWA(CL_ERR_InvalidState, "attempt to access a deleted document" );

    //Retrieve the n-th document
    return fieldsReader->doc(n, doc);
}

bool SegmentReader::isDeleted(const int32_t n)
{
    //Func - Checks if the n-th document has been marked deleted
    //Pre  - n >=0 and identifies the document n
    //Post - true has been returned if document n has been deleted otherwise fralse

    SCOPED_LOCK_MUTEX(THIS_LOCK)

    CND_PRECONDITION(n >= 0, "n is a negative number");

    //Is document n deleted
    return (deletedDocs != NULL && deletedDocs->get(n));
}

TermDocs* SegmentReader::termDocs() const
{
    //Func - Returns an unpositioned TermDocs enumerator. 
    //Pre  - true
    //Post - An unpositioned TermDocs enumerator has been returned

    return _CLNEW SegmentTermDocs(this);
}

TermPositions* SegmentReader::termPositions() const
{
    //Func - Returns an unpositioned TermPositions enumerator. 
    //Pre  - true
    //Post - An unpositioned TermPositions enumerator has been returned

    return _CLNEW SegmentTermPositions(this);
}

int32_t SegmentReader::docFreq(const Term* t) const
{
    //Func - Returns the number of documents which contain the term t
    //Pre  - t holds a valid reference to a Term
    //Post - The number of documents which contain term t has been returned

    //Get the TermInfo ti for Term  t in the set
    TermInfo* ti = tis->get(t);
    //Check if an TermInfo has been returned
    if (ti) {
        //Get the frequency of the term
        int32_t ret = ti->docFreq;
        //TermInfo ti is not needed anymore so delete it
        _CLDELETE( ti );
        //return the number of documents which containt term t
        return ret;
    }
        
    //No TermInfo returned so return 0
    return 0;
}

int32_t SegmentReader::numDocs()
{
    //Func - Returns the actual number of documents in the segment
    //Pre  - true
    //Post - The actual number of documents in the segments

    //Get the number of all the documents in the segment including the ones that have 
    //been marked deleted
    int32_t n = maxDoc();

    //Check if there any deleted docs
    if (deletedDocs != NULL)
        //Substract the number of deleted docs from the number returned by maxDoc
        n -= deletedDocs->count();

    //return the actual number of documents in the segment
    return n;
}

int32_t SegmentReader::maxDoc() const
{
    //Func - Returns the number of  all the documents in the segment including
    //       the ones that have been marked deleted
    //Pre  - true
    //Post - The total number of documents in the segment has been returned

    return fieldsReader->size();
}

void SegmentReader::getFieldNames(FieldOption fldOption,
    StringArrayWithDeletor& retarray)
{
    size_t len = fieldInfos->size();
    for (size_t i = 0; i < len; i++) {
        bool v = false;
        FieldInfo* fi = fieldInfos->fieldInfo(i);
        if (fldOption & IndexReader::ALL) {
            v = true;
        } else {
            if (!fi->isIndexed && (fldOption & IndexReader::UNINDEXED)) {
                v = true;
            }
            
            if (fi->isIndexed && (fldOption & IndexReader::INDEXED)) {
                v = true;
            }

            if (fi->isIndexed && fi->storeTermVector == false
                && (fldOption & IndexReader::INDEXED_NO_TERMVECTOR)) {
                v = true;
            }
            
            if ((fldOption & IndexReader::TERMVECTOR)
                && fi->storeTermVector == true
                && fi->storePositionWithTermVector == false
                && fi->storeOffsetWithTermVector == false) {
                v = true;
            }

            if (fi->isIndexed && fi->storeTermVector
                && (fldOption & IndexReader::INDEXED_WITH_TERMVECTOR)) {
                v = true;
            }

            if (fi->storePositionWithTermVector
                && fi->storeOffsetWithTermVector == false
                && (fldOption & IndexReader::TERMVECTOR_WITH_POSITION)) {
                v = true;
            }

            if (fi->storeOffsetWithTermVector
                && fi->storePositionWithTermVector == false
                && (fldOption & IndexReader::TERMVECTOR_WITH_OFFSET)) {
                v = true;
            }

            if ((fi->storeOffsetWithTermVector && fi->storePositionWithTermVector)
                && (fldOption & IndexReader::TERMVECTOR_WITH_POSITION_OFFSET)) {
                v = true;
            }
        }

        if (v)
            retarray.push_back(STRDUP_TtoT(fi->name));
    }
}

bool SegmentReader::hasNorms(const TCHAR* field) const
{
    return _norms.find(field) != _norms.end();
}


void SegmentReader::norms(const TCHAR* field, uint8_t* bytes)
{
    //Func - Reads the Norms for field from disk starting at offset in the inputstream
    //Pre  - field != NULL
    //       bytes != NULL is an array of bytes which is to be used to read the norms into.
    //       it is advisable to have bytes initalized by zeroes!
    //Post - The if an inputstream to the norm file could be retrieved the bytes have been read
    //       You are never sure whether or not the norms have been read into bytes properly!!!!!!!!!!!!!!!!!

    CND_PRECONDITION(field != NULL, "field is NULL");
    CND_PRECONDITION(bytes != NULL, "field is NULL");

    SCOPED_LOCK_MUTEX(THIS_LOCK)

    Norm* norm = _norms.get(field);
    if ( norm == NULL ){
        memcpy(bytes, fakeNorms(), maxDoc());
        return;
    }
    
    if (norm->bytes != NULL) { // can copy from cache
        memcpy(bytes, norm->bytes, maxDoc());
        return;
    }

    IndexInput* _normStream = norm->in->clone();
    CND_PRECONDITION(_normStream != NULL, "normStream==NULL")

    // read from disk
    try { 
        _normStream->seek(0);
        _normStream->readBytes(bytes, maxDoc());
    } _CLFINALLY (
        //Have the normstream closed
        _normStream->close();
        //Destroy the normstream
        _CLDELETE( _normStream );
    );	
}

uint8_t* SegmentReader::createFakeNorms(int32_t size)
{
    uint8_t* ones = _CL_NEWARRAY(uint8_t,size);
    memset(ones, DefaultSimilarity::encodeNorm(1.0f), size);
    return ones;
}

uint8_t* SegmentReader::fakeNorms()
{
    if (ones == NULL) 
        ones = createFakeNorms(maxDoc());
    return ones;
}

// can return null if norms aren't stored
uint8_t* SegmentReader::getNorms(const TCHAR* field)
{
    SCOPED_LOCK_MUTEX(THIS_LOCK)
    Norm* norm = _norms.get(field);
    if (norm == NULL) 
        return NULL;  // not indexed, or norms not stored

    if (norm->bytes == NULL) {                     // value not yet read
        uint8_t* bytes = _CL_NEWARRAY(uint8_t, maxDoc());
        norms(field, bytes);
        norm->bytes = bytes;                         // cache it
    }
    return norm->bytes;
}

uint8_t* SegmentReader::norms(const TCHAR* field)
{
    //Func - Returns the bytes array that holds the norms of a named field
    //Pre  - field != NULL and contains the name of the field for which the norms 
    //       must be retrieved
    //Post - If there was norm for the named field then a bytes array has been allocated 
    //       and returned containing the norms for that field. If the named field is unknown NULL is returned.

    CND_PRECONDITION(field != NULL, "field is NULL");
    
    SCOPED_LOCK_MUTEX(THIS_LOCK)

    uint8_t* bytes = getNorms(field);
    if (bytes == NULL) 
        bytes = fakeNorms();
    return bytes;
}

void SegmentReader::doSetNorm(int32_t doc, const TCHAR* field, uint8_t value)
{
    Norm* norm = _norms.get(field);
    if (norm == NULL)                             // not an indexed field
        return;

    norm->dirty = true;                            // mark it dirty
    normsDirty = true;

    uint8_t* bits = norms(field);
    bits[doc] = value;                    // set the value
}

QString SegmentReader::SegmentName(const QString& ext, const int32_t x)
{
    //Func - Returns an allocated buffer in which it creates a filename by 
    //       concatenating segment with ext and x
    //Pre    ext != NULL and holds the extension
    //       x contains a number
    //Post - A buffer has been instantiated an when x = -1 buffer contains the concatenation of 
    //       segment and ext otherwise buffer contains the contentation of segment, ext and x

    CND_PRECONDITION(!ext.isEmpty(), "ext is NULL");

    QString buf;
    SegmentName(buf, CL_MAX_PATH, ext, x);
    return buf;
}

void SegmentReader::SegmentName(QString& buffer, int32_t bufferLen,
    const QString& ext, const int32_t x)
{
    //Func - Creates a filename in buffer by concatenating segment with ext and x
    //Pre  - buffer != NULL
    //       ext    != NULL
    //       x contains a number
    //Post - When x = -1 buffer contains the concatenation of segment and ext otherwise
    //       buffer contains the contentation of segment, ext and x

    CND_PRECONDITION(!segment.isEmpty(), "Segment is NULL");

    Misc::segmentname(buffer, bufferLen, segment, ext, x);
}

void SegmentReader::openNorms(Directory* cfsDir)
{
    //Func - Open all norms files for all fields
    //       Creates for each field a norm Instance with an open inputstream to 
    //       a corresponding norm file ready to be read
    //Pre  - true
    //Post - For each field a norm instance has been created with an open inputstream to
    //       a corresponding norm file ready to be read

    //Iterate through all the fields
    for (int32_t i = 0; i < fieldInfos->size(); i++) {
        //Get the FieldInfo for the i-th field
        FieldInfo* fi = fieldInfos->fieldInfo(i);
        //Check if the field is indexed
        if (fi->isIndexed && !fi->omitNorms ) {
            //Allocate a buffer
            QString fileName;

            // look first if there are separate norms in compound format
            SegmentName(fileName, CL_MAX_PATH, QLatin1String(".s"), fi->number);
            Directory* d = getDirectory();
            if(!d->fileExists(fileName)){
                SegmentName(fileName, CL_MAX_PATH, QLatin1String(".f"), fi->number);
                d = cfsDir;
            }

            _norms.put(fi->name, _CLNEW Norm(d->openInput(fileName),
                fi->number, this, segment));
        }
    }
}

void SegmentReader::closeNorms()
{
    //Func - Close all the norms stored in norms
    //Pre  - true
    //Post - All the norms have been destroyed

    SCOPED_LOCK_MUTEX(_norms.THIS_LOCK)
    
    //Create an interator initialized at the beginning of norms
    NormsType::iterator itr = _norms.begin();
    //Iterate through all the norms
    while (itr != _norms.end()) {
        // Get, delete the norm
        _CLDELETE(itr->second);
        // Move the interator to the next norm in the norms collection.
        // Note ++ is an overloaded operator
        ++itr;
    }
    _norms.clear(); //bvk: they're deleted, so clear them so that they are not re-used
}

TermVectorsReader* SegmentReader::getTermVectorsReader()
{
    TermVectorsReader* tvReader = termVectorsLocal.get();
    if (tvReader == NULL) {
        tvReader = termVectorsReaderOrig->clone();
        termVectorsLocal.set(tvReader);
    }
    return tvReader;
}

TermFreqVector* SegmentReader::getTermFreqVector(int32_t docNumber,
    const TCHAR* field)
{
    if (field) {
        FieldInfo* fi = fieldInfos->fieldInfo(field);
        // Check if this field is invalid or has no stored term vector
        if (fi == NULL || !fi->storeTermVector || termVectorsReaderOrig == NULL) 
            return NULL;
    }

    TermVectorsReader* termVectorsReader = getTermVectorsReader();
    if (termVectorsReader == NULL)
        return NULL;

    return termVectorsReader->get(docNumber, field);
}

bool SegmentReader::getTermFreqVectors(int32_t docNumber,
    Array<TermFreqVector*>& result)
{
    if (termVectorsReaderOrig == NULL)
        return false;

    TermVectorsReader* termVectorsReader = getTermVectorsReader();
    if (termVectorsReader == NULL)
        return false;

    return termVectorsReader->get(docNumber, result);
}

CL_NS_END
