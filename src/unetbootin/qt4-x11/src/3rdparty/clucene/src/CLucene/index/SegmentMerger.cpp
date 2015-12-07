/*
 * Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
 *
 * Distributable under the terms of either the Apache License (Version 2.0) or 
 * the GNU Lesser General Public License, as specified in the COPYING file.
 *
 * Changes are Copyright(C) 2007, 2008 by Nokia Corporation and/or its subsidiary(-ies), all rights reserved.
*/
#include "CLucene/StdHeader.h"
#include "SegmentMerger.h"

CL_NS_USE(util)
CL_NS_USE(document)
CL_NS_USE(store)
CL_NS_DEF(index)

// File extensions of old-style index files
int COMPOUND_EXTENSIONS_LENGTH = 7;
const char* COMPOUND_EXTENSIONS = "fnm\0" "frq\0" "prx\0" "fdx\0" "fdt\0" "tii\0" "tis\0";

int VECTOR_EXTENSIONS_LENGTH = 3;
const char* VECTOR_EXTENSIONS = "tvx\0" "tvd\0" "tvf\0";

SegmentMerger::SegmentMerger(IndexWriter* writer, const QString& name)
{
    //Func - Constructor
    //Pre  - dir holds a valid reference to a Directory
    //       name != NULL
    //Post - Instance has been created

    CND_PRECONDITION(!name.isEmpty(), "name is NULL");

    freqOutput       = NULL;
    proxOutput       = NULL;
    termInfosWriter  = NULL;
    queue            = NULL;
    fieldInfos       = NULL;
    useCompoundFile  = writer->getUseCompoundFile();
    skipBuffer       = _CLNEW CL_NS(store)::RAMIndexOutput();

    segment          = name;
    directory		 = writer->getDirectory();
    termIndexInterval = writer->getTermIndexInterval();

    lastSkipDoc=0;
    lastSkipFreqPointer=0;
    lastSkipProxPointer=0;
    skipInterval=0;
}

SegmentMerger::~SegmentMerger()
{
    //Func - Destructor
    //Pre  - true
    //Post - The instance has been destroyed

    //Clear the readers set
    readers.clear();

    //Delete field Infos
    _CLDELETE(fieldInfos);     
    //Close and destroy the IndexOutput to the Frequency File
    if (freqOutput != NULL) { 
        freqOutput->close(); 
        _CLDELETE(freqOutput); 
    }
    //Close and destroy the IndexOutput to the Prox File
    if (proxOutput != NULL) {
        proxOutput->close(); 
        _CLDELETE(proxOutput); 
    }
    //Close and destroy the termInfosWriter
    if (termInfosWriter != NULL) {
        termInfosWriter->close(); 
        _CLDELETE(termInfosWriter); 
    }
    //Close and destroy the queue
    if (queue != NULL) {
        queue->close(); 
        _CLDELETE(queue);
    }
    //close and destory the skipBuffer
    if (skipBuffer != NULL) {
        skipBuffer->close();
        _CLDELETE(skipBuffer);
    }
}

void SegmentMerger::add(IndexReader* reader)
{
    //Func - Adds a IndexReader to the set of readers
    //Pre  - reader contains a valid reference to a IndexReader
    //Post - The SegementReader reader has been added to the set of readers

    readers.push_back(reader);
}

IndexReader* SegmentMerger::segmentReader(const int32_t i)
{
    //Func - Returns a reference to the i-th IndexReader
    //Pre  - 0 <= i < readers.size()
    //Post - A reference to the i-th IndexReader has been returned

    CND_PRECONDITION(i >= 0, "i is a negative number");
    CND_PRECONDITION((size_t)i < readers.size(),
        "i is bigger than the number of IndexReader instances");

    //Retrieve the i-th IndexReader
    IndexReader* ret = readers[i];
    CND_CONDITION(ret != NULL, "No IndexReader found");

    return ret;
}

int32_t SegmentMerger::merge()
{
    int32_t value = mergeFields();
    mergeTerms();
    mergeNorms();

    if (fieldInfos->hasVectors())
        mergeVectors();

    return value;
}

void SegmentMerger::closeReaders()
{
    for (uint32_t i = 0; i < readers.size(); i++) {
        // close readers
        IndexReader* reader = readers[i];
        reader->close();
    }
}

void SegmentMerger::createCompoundFile(const QString& filename, QStringList& files)
{
    CompoundFileWriter* cfsWriter = _CLNEW CompoundFileWriter(directory, filename);

    { //msvc6 scope fix
        // Basic files
        for (int32_t i = 0; i < COMPOUND_EXTENSIONS_LENGTH; i++) {
            files.push_back(Misc::qjoin(segment, QLatin1String("."),
                QLatin1String(COMPOUND_EXTENSIONS+(i*4))));
        }
    }

    { //msvc6 scope fix
        // Field norm files
        for (int32_t i = 0; i < fieldInfos->size(); i++) {
            FieldInfo* fi = fieldInfos->fieldInfo(i);
            if (fi->isIndexed && !fi->omitNorms) {
                TCHAR tbuf[10];
                char abuf[10];
                _i64tot(i, tbuf, 10);
                STRCPY_TtoA(abuf, tbuf, 10);

                files.push_back(Misc::qjoin(segment, QLatin1String(".f"),
                    QLatin1String(abuf)));
            }
        }
    }

    // Vector files
    if (fieldInfos->hasVectors()) {
        for (int32_t i = 0; i < VECTOR_EXTENSIONS_LENGTH; i++) {
            files.push_back(Misc::qjoin(segment, QLatin1String("."),
                QLatin1String(VECTOR_EXTENSIONS+(i*4))));
        }
    }

    { //msvc6 scope fix
        // Now merge all added files
        for (size_t i=0;i<files.size();i++) {
            cfsWriter->addFile(files[i]);
        }
    }

    // Perform the merge
    cfsWriter->close();
    _CLDELETE(cfsWriter);
}

void SegmentMerger::addIndexed(IndexReader* reader, FieldInfos* fieldInfos,
    StringArrayWithDeletor& names, bool storeTermVectors,
    bool storePositionWithTermVector, bool storeOffsetWithTermVector)
{
    StringArrayWithDeletor::const_iterator itr = names.begin();
    while (itr != names.end()) {
        fieldInfos->add(*itr, true, 
            storeTermVectors, storePositionWithTermVector, 
            storeOffsetWithTermVector, !reader->hasNorms(*itr));
        ++itr;
    }
}

int32_t SegmentMerger::mergeFields()
{
    //Func - Merge the fields of all segments 
    //Pre  - true
    //Post - The field infos and field values of all segments have been merged.

    //Create a new FieldInfos
    fieldInfos = _CLNEW FieldInfos();		  // merge field names

    //Condition check to see if fieldInfos points to a valid instance
    CND_CONDITION(fieldInfos != NULL, "Memory allocation for fieldInfos failed");

    IndexReader* reader = NULL;

    int32_t docCount = 0;

    //Iterate through all readers
    for (uint32_t i = 0; i < readers.size(); i++) {
        //get the i-th reader
        reader = readers[i];
        //Condition check to see if reader points to a valid instance
        CND_CONDITION(reader != NULL,"No IndexReader found");

        StringArrayWithDeletor tmp;

        tmp.clear();
        reader->getFieldNames(IndexReader::TERMVECTOR_WITH_POSITION_OFFSET, tmp);
        addIndexed(reader, fieldInfos, tmp, true, true, true);

        tmp.clear();
        reader->getFieldNames(IndexReader::TERMVECTOR_WITH_POSITION, tmp);
        addIndexed(reader, fieldInfos, tmp, true, true, false);

        tmp.clear();
        reader->getFieldNames(IndexReader::TERMVECTOR_WITH_OFFSET, tmp);
        addIndexed(reader, fieldInfos, tmp, true, false, true);

        tmp.clear();
        reader->getFieldNames(IndexReader::TERMVECTOR, tmp);
        addIndexed(reader, fieldInfos, tmp, true, false, false);

        tmp.clear();
        reader->getFieldNames(IndexReader::INDEXED, tmp);
        addIndexed(reader, fieldInfos, tmp, false, false, false);

        tmp.clear();
        reader->getFieldNames(IndexReader::UNINDEXED, tmp);
        if (tmp.size() > 0) {
            TCHAR** arr = _CL_NEWARRAY(TCHAR*,tmp.size()+1);
            tmp.toArray(arr);
            fieldInfos->add((const TCHAR**)arr, false);
            _CLDELETE_ARRAY(arr);
            //no need to delete the contents, since tmp is responsible for it
        }
    }

    //Create the filename of the new FieldInfos file
    QString buf = Misc::segmentname(segment, QLatin1String(".fnm"));
    //Write the new FieldInfos file to the directory
    fieldInfos->write(directory, buf);

    // merge field values
    // Instantiate Fieldswriter which will write in directory for the segment
    // name segment using the new merged fieldInfos
    FieldsWriter* fieldsWriter = _CLNEW FieldsWriter(directory, segment, fieldInfos);

    //Condition check to see if fieldsWriter points to a valid instance
    CND_CONDITION(fieldsWriter != NULL, "Memory allocation for fieldsWriter failed");

    try {  
        IndexReader* reader = NULL;
        int32_t maxDoc          = 0;
        //Iterate through all readers
        for (uint32_t i = 0; i < readers.size(); i++) {
            // get the i-th reader
            reader = readers[i];


            // Condition check to see if reader points to a valid instance
            CND_CONDITION(reader != NULL, "No IndexReader found");

            // Get the total number documents including the documents that have
            // been marked deleted
            int32_t maxDoc = reader->maxDoc();

            //document buffer
            Document doc;

            //Iterate through all the documents managed by the current reader
            for (int32_t j = 0; j < maxDoc; j++) {
                //Check if the j-th document has been deleted, if so skip it
                if (!reader->isDeleted(j)) { 
                    //Get the document
                    if (reader->document(j, &doc)) {
                        //Add the document to the new FieldsWriter
                        fieldsWriter->addDocument(&doc);
                        docCount++;
                        //doc is cleard for re-use
                        doc.clear();
                    }
                }
            }
        }
    } _CLFINALLY (
        //Close the fieldsWriter
        fieldsWriter->close();
        //And have it deleted as it not used any more
        _CLDELETE(fieldsWriter);
    );

    return docCount;
}

void SegmentMerger::mergeVectors()
{
    TermVectorsWriter* termVectorsWriter = 
        _CLNEW TermVectorsWriter(directory, segment, fieldInfos);

    try {
        for (uint32_t r = 0; r < readers.size(); r++) {
            IndexReader* reader = readers[r];
            int32_t maxDoc = reader->maxDoc();
            for (int32_t docNum = 0; docNum < maxDoc; docNum++) {
                // skip deleted docs
                if (reader->isDeleted(docNum))
                    continue;

                Array<TermFreqVector*> tmp;
                if (reader->getTermFreqVectors(docNum, tmp))
                    termVectorsWriter->addAllDocVectors(tmp);
                tmp.deleteAll();
            }
        }
    } _CLFINALLY (
        _CLDELETE(termVectorsWriter);
    );
}


void SegmentMerger::mergeTerms()
{
    //Func - Merge the terms of all segments
    //Pre  - fieldInfos != NULL
    //Post - The terms of all segments have been merged

    CND_PRECONDITION(fieldInfos != NULL, "fieldInfos is NULL");

    try{
        //create a filename for the new Frequency File for segment
        QString buf = Misc::segmentname(segment, QLatin1String(".frq"));
        //Open an IndexOutput to the new Frequency File
        freqOutput = directory->createOutput(buf);

        //create a filename for the new Prox File for segment
        buf = Misc::segmentname(segment, QLatin1String(".prx"));
        //Open an IndexOutput to the new Prox File
        proxOutput = directory->createOutput(buf);

        //Instantiate  a new termInfosWriter which will write in directory
        //for the segment name segment using the new merged fieldInfos
        termInfosWriter = _CLNEW TermInfosWriter(directory, segment, fieldInfos,
            termIndexInterval);  

        //Condition check to see if termInfosWriter points to a valid instance
        CND_CONDITION(termInfosWriter != NULL,
            "Memory allocation for termInfosWriter failed");

        skipInterval = termInfosWriter->skipInterval;
        queue = _CLNEW SegmentMergeQueue(readers.size());

        //And merge the Term Infos
        mergeTermInfos();	      
    } _CLFINALLY (
        //Close and destroy the IndexOutput to the Frequency File
        if (freqOutput != NULL) {
            freqOutput->close(); _CLDELETE(freqOutput);
        }

        //Close and destroy the IndexOutput to the Prox File
        if (proxOutput != NULL)
        {
            proxOutput->close();
            _CLDELETE(proxOutput);
        }

        //Close and destroy the termInfosWriter
        if (termInfosWriter != NULL) {
            termInfosWriter->close();
            _CLDELETE(termInfosWriter);
        }
        
        //Close and destroy the queue
        if (queue != NULL) {
            queue->close();
            _CLDELETE(queue);
        }
    );
}

void SegmentMerger::mergeTermInfos()
{
    //Func - Merges all TermInfos into a single segment
    //Pre  - true
    //Post - All TermInfos have been merged into a single segment

    //Condition check to see if queue points to a valid instance
    CND_CONDITION(queue != NULL, "Memory allocation for queue failed");

    //base is the id of the first document in a segment
    int32_t base = 0;

    IndexReader* reader = NULL;
    SegmentMergeInfo* smi = NULL;

    //iterate through all the readers
    for (uint32_t i = 0; i < readers.size(); i++) {
        //Get the i-th reader
        reader = readers[i];

        //Condition check to see if reader points to a valid instance
        CND_CONDITION(reader != NULL, "No IndexReader found");

        //Get the term enumeration of the reader
        TermEnum* termEnum = reader->terms();
        //Instantiate a new SegmentMerginfo for the current reader and enumeration
        smi = _CLNEW SegmentMergeInfo(base, termEnum, reader);

        //Condition check to see if smi points to a valid instance
        CND_CONDITION(smi != NULL, "Memory allocation for smi failed")	;

        //Increase the base by the number of documents that have not been marked deleted
        //so base will contain a new value for the first document of the next iteration
        base += reader->numDocs();
        //Get the next current term
        if (smi->next()) {
            //Store the SegmentMergeInfo smi with the initialized SegmentTermEnum TermEnum
            //into the queue
            queue->put(smi);
        } else {
            //Apparently the end of the TermEnum of the SegmentTerm has been reached so
            //close the SegmentMergeInfo smi
            smi->close();
            //And destroy the instance and set smi to NULL (It will be used later in this method)
            _CLDELETE(smi);
        }
    }

    //Instantiate an array of SegmentMergeInfo instances called match
    SegmentMergeInfo** match = _CL_NEWARRAY(SegmentMergeInfo*,readers.size()+1);

    //Condition check to see if match points to a valid instance
    CND_CONDITION(match != NULL, "Memory allocation for match failed")	;

    SegmentMergeInfo* top = NULL;

    //As long as there are SegmentMergeInfo instances stored in the queue
    while (queue->size() > 0) {
        int32_t matchSize = 0;			  

        // pop matching terms

        //Pop the first SegmentMergeInfo from the queue
        match[matchSize++] = queue->pop();
        //Get the Term of match[0]
        Term* term = match[0]->term;

        //Condition check to see if term points to a valid instance
        CND_CONDITION(term != NULL,"term is NULL")	;

        //Get the current top of the queue
        top = queue->top();

        //For each SegmentMergInfo still in the queue 
        //Check if term matches the term of the SegmentMergeInfo instances in the queue
        while (top != NULL && term->equals(top->term)) {
            //A match has been found so add the matching SegmentMergeInfo to the match array
            match[matchSize++] = queue->pop();
            //Get the next SegmentMergeInfo
            top = queue->top();
        }
        match[matchSize]=NULL;

        //add new TermInfo
        mergeTermInfo(match); //matchSize  

        //Restore the SegmentTermInfo instances in the match array back into the queue
        while (matchSize > 0) {
            smi = match[--matchSize];

            //Condition check to see if smi points to a valid instance
            CND_CONDITION(smi != NULL, "smi is NULL");

            //Move to the next term in the enumeration of SegmentMergeInfo smi
            if (smi->next()) {
                //There still are some terms so restore smi in the queue
                queue->put(smi);

            } else {
                //Done with a segment
                //No terms anymore so close this SegmentMergeInfo instance
                smi->close();				  
                _CLDELETE(smi);
            }
        }
    }

    _CLDELETE_ARRAY(match);
}

void SegmentMerger::mergeTermInfo(SegmentMergeInfo** smis)
{
    //Func - Merge the TermInfo of a term found in one or more segments. 
    //Pre  - smis != NULL and it contains segments that are positioned at the same term.
    //       n is equal to the number of SegmentMergeInfo instances in smis
    //       freqOutput != NULL
    //       proxOutput != NULL
    //Post - The TermInfo of a term has been merged

    CND_PRECONDITION(smis != NULL, "smis is NULL");
    CND_PRECONDITION(freqOutput != NULL, "freqOutput is NULL");
    CND_PRECONDITION(proxOutput != NULL, "proxOutput is NULL");

    //Get the file pointer of the IndexOutput to the Frequency File
    int64_t freqPointer = freqOutput->getFilePointer();
    //Get the file pointer of the IndexOutput to the Prox File
    int64_t proxPointer = proxOutput->getFilePointer();

    //Process postings from multiple segments all positioned on the same term.
    int32_t df = appendPostings(smis);  

    int64_t skipPointer = writeSkip();

    //df contains the number of documents across all segments where this term was found
    if (df > 0) {
        //add an entry to the dictionary with pointers to prox and freq files
        termInfo.set(df, freqPointer, proxPointer, (int32_t)(skipPointer - freqPointer));
        //Precondition check for to be sure that the reference to
        //smis[0]->term will be valid
        CND_PRECONDITION(smis[0]->term != NULL, "smis[0]->term is NULL");
        //Write a new TermInfo
        termInfosWriter->add(smis[0]->term, &termInfo);
    }
}


int32_t SegmentMerger::appendPostings(SegmentMergeInfo** smis)
{
    //Func - Process postings from multiple segments all positioned on the
    //       same term. Writes out merged entries into freqOutput and
    //       the proxOutput streams.
    //Pre  - smis != NULL and it contains segments that are positioned at the same term.
    //       n is equal to the number of SegmentMergeInfo instances in smis
    //       freqOutput != NULL
    //       proxOutput != NULL
    //Post - Returns number of documents across all segments where this term was found

    CND_PRECONDITION(smis != NULL, "smis is NULL");
    CND_PRECONDITION(freqOutput != NULL, "freqOutput is NULL");
    CND_PRECONDITION(proxOutput != NULL, "proxOutput is NULL");

    int32_t lastDoc = 0;
    int32_t df = 0;       //Document Counter

    resetSkip();
    SegmentMergeInfo* smi = NULL;

    //Iterate through all SegmentMergeInfo instances in smis
    int32_t i = 0;
    while ((smi=smis[i]) != NULL) {
        //Get the i-th SegmentMergeInfo 

        //Condition check to see if smi points to a valid instance
        CND_PRECONDITION(smi != NULL, "	 is NULL");

        //Get the term positions 
        TermPositions* postings = smi->getPositions();
        //Get the base of this segment
        int32_t base = smi->base;
        //Get the docMap so we can see which documents have been deleted
        int32_t* docMap = smi->getDocMap();
        //Seek the termpost
        postings->seek(smi->termEnum);
        while (postings->next()) {
            int32_t doc = postings->doc();
            //Check if there are deletions
            if (docMap != NULL)
                doc = docMap[doc]; // map around deletions
            doc += base;                              // convert to merged space

            //Condition check to see doc is eaqual to or bigger than lastDoc
            CND_CONDITION(doc >= lastDoc,"docs out of order");

            //Increase the total frequency over all segments
            df++;

            if ((df % skipInterval) == 0) {
                bufferSkip(lastDoc);
            }

            //Calculate a new docCode 
            //use low bit to flag freq=1
            int32_t docCode = (doc - lastDoc) << 1;	  
            lastDoc = doc;

            //Get the frequency of the Term
            int32_t freq = postings->freq();
            if (freq == 1) {
                //write doc & freq=1
                freqOutput->writeVInt(docCode | 1);	  
            } else {
                //write doc
                freqOutput->writeVInt(docCode);	  
                //write frequency in doc
                freqOutput->writeVInt(freq);		  
            }

            int32_t lastPosition = 0;			  
            // write position deltas
            for (int32_t j = 0; j < freq; j++) {
                //Get the next position
                int32_t position = postings->nextPosition();
                //Write the difference between position and the last position
                proxOutput->writeVInt(position - lastPosition);			  
                lastPosition = position;
            }
        }

        i++;
    }

    //Return total number of documents across all segments where term was found		
    return df;
}

void SegmentMerger::resetSkip()
{
    skipBuffer->reset();
    lastSkipDoc = 0;
    lastSkipFreqPointer = freqOutput->getFilePointer();
    lastSkipProxPointer = proxOutput->getFilePointer();
}

void SegmentMerger::bufferSkip(int32_t doc)
{
    int64_t freqPointer = freqOutput->getFilePointer();
    int64_t proxPointer = proxOutput->getFilePointer();

    skipBuffer->writeVInt(doc - lastSkipDoc);
    skipBuffer->writeVInt((int32_t) (freqPointer - lastSkipFreqPointer));
    skipBuffer->writeVInt((int32_t) (proxPointer - lastSkipProxPointer));

    lastSkipDoc = doc;
    lastSkipFreqPointer = freqPointer;
    lastSkipProxPointer = proxPointer;
}

int64_t SegmentMerger::writeSkip()
{
    int64_t skipPointer = freqOutput->getFilePointer();
    skipBuffer->writeTo(freqOutput);
    return skipPointer;
}

// Func - Merges the norms for all fields
// Pre  - fieldInfos != NULL
// Post - The norms for all fields have been merged
void SegmentMerger::mergeNorms()
{
    CND_PRECONDITION(fieldInfos != NULL, "fieldInfos is NULL");

    //iterate through all the Field Infos instances
    for (int32_t i = 0; i < fieldInfos->size(); i++) {
        //Get the i-th FieldInfo
        FieldInfo* fi = fieldInfos->fieldInfo(i);
        //Is this Field indexed?
        if (fi->isIndexed && !fi->omitNorms) {
            //Create and Instantiate an IndexOutput to that norm file
            QString buf = Misc::segmentname(segment, QLatin1String(".f"), i);
            IndexOutput* output = directory->createOutput(buf);

            //Condition check to see if output points to a valid instance
            CND_CONDITION(output != NULL, "No Outputstream retrieved");

            uint8_t* input = NULL;
            try {
                for (uint32_t j = 0; j < readers.size(); ++j) {
                    // get the next index reader + condition check
                    IndexReader* reader = readers[j];
                    CND_CONDITION(reader != NULL, "No reader found");

                    // Get the total number of documents including the documents
                    // that have been marked deleted
                    int32_t maxDoc = reader->maxDoc();
                    if (maxDoc > 0) {
                        // if there are docs, allocate buffer to read it's norms
                        uint8_t* data = (uint8_t*)realloc(input, maxDoc *
                            sizeof(uint8_t));
                        if (data) {
                            input = data;
                            memset(input, 0, maxDoc * sizeof(uint8_t));
                            // Get an IndexInput to the norm file for this
                            // field in this segment
                            reader->norms(fi->name, input);

                            //Iterate through all the documents
                            for(int32_t k = 0; k < maxDoc; k++) {
                                //Check if document k is deleted
                                if (!reader->isDeleted(k)) {
                                    //write the new norm
                                    output->writeByte(input[k]);
                                }
                            }
                        }
                    }
                }
            } _CLFINALLY (
                if (output != NULL) {
                    output->close();
                    _CLDELETE(output);
                }
                free(input);
            );
        }
    }
}

CL_NS_END
