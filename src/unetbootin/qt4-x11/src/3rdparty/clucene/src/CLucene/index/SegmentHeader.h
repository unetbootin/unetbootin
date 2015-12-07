/*
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
*
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
*
* Changes are Copyright(C) 2007, 2008 by Nokia Corporation and/or its subsidiary(-ies), all rights reserved.
*/
#ifndef _lucene_index_SegmentHeader_
#define _lucene_index_SegmentHeader_

#if defined(_LUCENE_PRAGMA_ONCE)
#   pragma once
#endif

#include <QtCore/QString>
#include <QtCore/QStringList>

#include "SegmentInfos.h"
#include "CLucene/util/BitSet.h"
#include "CLucene/util/VoidMap.h"
#include "Term.h"
#include "FieldInfos.h"
#include "FieldsReader.h"
#include "IndexReader.h"
#include "TermInfosReader.h"
#include "CompoundFile.h"
#include "CLucene/util/ThreadLocal.h"

CL_NS_DEF(index)

class SegmentReader;

class SegmentTermDocs : public virtual TermDocs
{
    int32_t _doc;
    int32_t skipInterval;
    int32_t numSkips;
    int32_t skipCount;
    CL_NS(store)::IndexInput* skipStream;
    int32_t skipDoc;
    int64_t freqPointer;
    int64_t proxPointer;
    int64_t skipPointer;
    bool haveSkipped;

protected:
    // SegmentReader parent
    const SegmentReader* parent;
    CL_NS(store)::IndexInput* freqStream;
    int32_t count;
    int32_t df;
    int32_t _freq;
    CL_NS(util)::BitSet* deletedDocs;
public:
    virtual ~SegmentTermDocs();

    virtual void seek(TermEnum* termEnum);
    virtual void seek(Term* term);
    virtual void seek(const TermInfo* ti);

    virtual void close();
    virtual int32_t doc()const;
    virtual int32_t freq()const;

    virtual bool next();

    /** Optimized implementation. */
    virtual int32_t read(int32_t* docs, int32_t* freqs, int32_t length);

    /** Optimized implementation. */
    virtual bool skipTo(const int32_t target);

    virtual TermPositions* __asTermPositions();

    // \param Parent must be a segment reader
    SegmentTermDocs( const SegmentReader* Parent);
protected:
    virtual void skippingDoc(){}
    virtual void skipProx(int64_t proxPointer){}
};


class SegmentTermPositions : public SegmentTermDocs, public TermPositions
{
private:
    CL_NS(store)::IndexInput* proxStream;
    int32_t proxCount;
    int32_t position;

public:
    // \param Parent must be a segment reader
    SegmentTermPositions(const SegmentReader* Parent);
    ~SegmentTermPositions();

    void seek(const TermInfo* ti);
    void close();
    int32_t nextPosition();
    bool next();
    int32_t read(int32_t* docs, int32_t* freqs, int32_t length);
    virtual TermDocs* __asTermDocs();
    virtual TermPositions* __asTermPositions();

    //resolve SegmentTermDocs/TermPositions ambiguity
    void seek(Term* term){ SegmentTermDocs::seek(term); }
    void seek(TermEnum* termEnum){ SegmentTermDocs::seek(termEnum); }
    int32_t doc() const{ return SegmentTermDocs::doc(); }
    int32_t freq() const{ return SegmentTermDocs::freq(); }
    bool skipTo(const int32_t target){ return SegmentTermDocs::skipTo(target); }

protected:
    void skippingDoc();
    /** Called by super.skipTo(). */
    void skipProx(int64_t proxPointer);
};

// An IndexReader responsible for reading 1 segment of an index
class SegmentReader : public IndexReader
{
    /**
    * The class Norm represents the normalizations for a field.
    * These normalizations are read from an IndexInput in into an array of bytes called bytes
    */
    class Norm : LUCENE_BASE
    {
        int32_t number;
        SegmentReader* reader;
        QString segment; // segment name
    
    public:
        CL_NS(store)::IndexInput* in;
        uint8_t* bytes;
        bool dirty;
        //Constructor
        Norm(CL_NS(store)::IndexInput* instrm, int32_t number,
            SegmentReader* reader, const QString& segment);
        //Destructor
        ~Norm();

        void reWrite();
    };
    friend class SegmentReader::Norm;

    //Holds the name of the segment that is being read
    QString segment;

    //Indicates if there are documents marked as deleted
    bool deletedDocsDirty;
    bool normsDirty;
    bool undeleteAll;

    //Holds all norms for all fields in the segment
    typedef CL_NS(util)::CLHashtable<const TCHAR*,
        Norm*,CL_NS(util)::Compare::TChar, CL_NS(util)::Equals::TChar> NormsType;
    NormsType _norms; 

    uint8_t* ones;
    uint8_t* fakeNorms();

    // Compound File Reader when based on a compound file segment
    CompoundFileReader* cfsReader;
    // Reads the Field Info file
    FieldsReader* fieldsReader;
    TermVectorsReader* termVectorsReaderOrig;
    CL_NS(util)::ThreadLocal<TermVectorsReader*,
        CL_NS(util)::Deletor::Object<TermVectorsReader> >termVectorsLocal;

    void initialize(SegmentInfo* si);

    // Create a clone from the initial TermVectorsReader and store it in the
    // ThreadLocal. @return TermVectorsReader
    TermVectorsReader* getTermVectorsReader();

protected:
    // Marks document docNum as deleted
    void doDelete(const int32_t docNum);
    void doUndeleteAll();
    void doCommit();
    void doSetNorm(int32_t doc, const TCHAR* field, uint8_t value);

    // can return null if norms aren't stored
    uint8_t* getNorms(const TCHAR* field);

public:
    /**
    Func - Constructor.
    Opens all files of a segment
    .fnm     -> Field Info File
    Field names are stored in the field info file, with suffix .fnm.
    .frq     -> Frequency File
    The .frq file contains the lists of documents which contain 
    each term, along with the frequency of the term in that document.
    .prx     -> Prox File
    The prox file contains the lists of positions that each term occurs
    at within documents.
    .tis     -> Term Info File
    This file is sorted by Term. Terms are ordered first lexicographically 
    by the term's field name, and within that lexicographically by the term's text.
    .del     -> Deletion File
    The .del file is optional, and only exists when a segment contains deletions
    .f[0-9]* -> Norm File
    Contains s, for each document, a byte that encodes a value that is 
    multiplied into the score for hits on that field:
    */
    SegmentReader(SegmentInfo* si);

    SegmentReader(SegmentInfos* sis, SegmentInfo* si);
    // Destructor.
    virtual ~SegmentReader();

    // Closes all streams to the files of a single segment
    void doClose();

    // Checks if a segment managed by SegmentInfo si has deletions
    static bool hasDeletions(const SegmentInfo* si);
    bool hasDeletions() const;
    bool hasNorms(const TCHAR* field) const;

    // Returns all file names managed by this SegmentReader
    void files(QStringList& retarray);
    // Returns an enumeration of all the Terms and TermInfos in the set.
    TermEnum* terms() const;
    // Returns an enumeration of terms starting at or after the named term t
    TermEnum* terms(const Term* t) const;

    // Gets the document identified by n
    bool document(int32_t n, CL_NS(document)::Document* doc);

    // Checks if the n-th document has been marked deleted
    bool isDeleted(const int32_t n);

    // Returns an unpositioned TermDocs enumerator.
    TermDocs* termDocs() const;
    // Returns an unpositioned TermPositions enumerator.
    TermPositions* termPositions() const;

    // Returns the number of documents which contain the term t
    int32_t docFreq(const Term* t) const;

    // Returns the actual number of documents in the segment
    int32_t numDocs();
    // Returns the number of  all the documents in the segment including the
    // ones that have been marked deleted
    int32_t maxDoc() const;

    // Returns the bytes array that holds the norms of a named field.
    // Returns fake norms if norms aren't available
    uint8_t* norms(const TCHAR* field);

    // Reads the Norms for field from disk
    void norms(const TCHAR* field, uint8_t* bytes);

    // concatenating segment with ext and x
    QString SegmentName(const QString& ext, const int32_t x = -1);
    // Creates a filename in buffer by concatenating segment with ext and x
    void SegmentName(QString& buffer, int32_t bufferLen, const QString& ext,
        const int32_t x = -1);

    /**
    * @see IndexReader#getFieldNames(IndexReader.FieldOption fldOption)
    */
    void getFieldNames(FieldOption fldOption, CL_NS(util)::StringArrayWithDeletor& retarray);

    static bool usesCompoundFile(SegmentInfo* si);

    /** Return a term frequency vector for the specified document and field. The
    *  vector returned contains term numbers and frequencies for all terms in
    *  the specified field of this document, if the field had storeTermVector
    *  flag set.  If the flag was not set, the method returns null.
    * @throws IOException
    */
    TermFreqVector* getTermFreqVector(int32_t docNumber, const TCHAR* field = NULL);

    /** Return an array of term frequency vectors for the specified document.
    *  The array contains a vector for each vectorized field in the document.
    *  Each vector vector contains term numbers and frequencies for all terms
    *  in a given vectorized field.
    *  If no such fields existed, the method returns null.
    * @throws IOException
    */
    bool getTermFreqVectors(int32_t docNumber, Array<TermFreqVector*>& result);

private:
    //Open all norms files for all fields
    void openNorms(CL_NS(store)::Directory* cfsDir);
    //Closes all norms files
    void closeNorms();

    // a bitVector that manages which documents have been deleted
    CL_NS(util)::BitSet* deletedDocs;
    // an IndexInput to the frequency file
    CL_NS(store)::IndexInput* freqStream;
    // For reading the fieldInfos file
    FieldInfos* fieldInfos;
    // For reading the Term Dictionary .tis file
    TermInfosReader* tis;
    // an IndexInput to the prox file
    CL_NS(store)::IndexInput* proxStream;

    static bool hasSeparateNorms(SegmentInfo* si);
    static uint8_t* createFakeNorms(int32_t size);

    // allow various classes to access the internals of this. this allows us
    // to have a more tight idea of the package
    friend class IndexReader;
    friend class IndexWriter;
    friend class SegmentTermDocs;
    friend class SegmentTermPositions;
    friend class MultiReader;
};

CL_NS_END

#endif
