/*
 * Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
 *
 * Distributable under the terms of either the Apache License (Version 2.0) or 
 * the GNU Lesser General Public License, as specified in the COPYING file.
 *
 * Changes are Copyright(C) 2007, 2008 by Nokia Corporation and/or its subsidiary(-ies), all rights reserved.
*/
#include "CLucene/StdHeader.h"
#include "IndexWriter.h"

#include "CLucene/document/Document.h"
#include "CLucene/store/Directory.h"
#include "CLucene/store/Lock.h"
#include "CLucene/util/VoidList.h"
#include "DocumentWriter.h"
#include "SegmentInfos.h"
#include "SegmentMerger.h"

CL_NS_USE(store)
CL_NS_USE(util)
CL_NS_USE(document)
CL_NS_USE(analysis)
CL_NS_DEF(index)

const QLatin1String IndexWriter::WRITE_LOCK_NAME("write.lock");
const QLatin1String IndexWriter::COMMIT_LOCK_NAME("commit.lock");

IndexWriter::IndexWriter(const QString& path, Analyzer* a, const bool create,
    const bool _closeDir)
    : directory(FSDirectory::getDirectory(path, create))
    , analyzer(a)
    , segmentInfos(true)
    , closeDir(_closeDir)
{
    //Func - Constructor
    //       Constructs an IndexWriter for the index in path.
    //Pre  - path != NULL and contains a named directory path
    //       a holds a valid reference to an analyzer and analyzes the text to
    //       be indexed create indicates if the indexWriter must create a new
    //       index located at path or just open it
    //Post - If create is true, then a new, empty index has been created in
    //       path, replacing the index already there, if any. The named
    //       directory path is owned by this Instance

    CND_PRECONDITION(!path.isEmpty(), "path is NULL");

    //Continue initializing the instance by _IndexWriter
    _IndexWriter(create);
}

IndexWriter::IndexWriter(Directory* d, Analyzer* a, const bool create,
    const bool _closeDir)
    : directory(_CL_POINTER(d))
    , analyzer(a)
    , segmentInfos(true)
    , closeDir(_closeDir)
{
    //Func - Constructor
    //       Constructs an IndexWriter for the index in path.
    //Pre  - d contains a valid reference to a directory
    //       a holds a valid reference to an analyzer and analyzes the text to
    //       be indexed create indicates if the indexWriter must create a new
    //       index located at path or just open it
    //Post - If create is true, then a new, empty index has been created in
    //       path, replacing the index already there, if any. The directory d
    //       is not owned by this Instance

    //Continue initializing the instance by _IndexWriter
    _IndexWriter ( create );
}

void IndexWriter::_IndexWriter(const bool create)
{
    //Func - Initialises the instances
    //Pre  - create indicates if the indexWriter must create a new index
    //       located at path or just open it

    similarity = CL_NS(search)::Similarity::getDefault();

    useCompoundFile = true;
    if ( directory->getDirectoryType() == RAMDirectory::DirectoryType() )
        useCompoundFile = false;

    //Create a ramDirectory
    ramDirectory = _CLNEW TransactionalRAMDirectory;

    CND_CONDITION(ramDirectory != NULL, "ramDirectory is NULL");

    //Initialize the writeLock to
    writeLock  = NULL;

    //initialise the settings...
    maxFieldLength = DEFAULT_MAX_FIELD_LENGTH;
    mergeFactor = DEFAULT_MERGE_FACTOR;
    maxMergeDocs = DEFAULT_MAX_MERGE_DOCS;
    writeLockTimeout = WRITE_LOCK_TIMEOUT;
    commitLockTimeout = COMMIT_LOCK_TIMEOUT;
    minMergeDocs = DEFAULT_MAX_BUFFERED_DOCS;
    termIndexInterval = DEFAULT_TERM_INDEX_INTERVAL;

    //Create a new lock using the name "write.lock"
    LuceneLock* newLock = directory->makeLock(IndexWriter::WRITE_LOCK_NAME);

    //Condition check to see if newLock has been allocated properly
    CND_CONDITION(newLock != NULL,
        "No memory could be allocated for LuceneLock newLock");

    //Try to obtain a write lock
    if (!newLock->obtain(writeLockTimeout)){
        //Write lock could not be obtained so delete it
        _CLDELETE(newLock);
        //Reset the instance
        _finalize();
        //throw an exception because no writelock could be created or obtained
        _CLTHROWA(CL_ERR_IO, "Index locked for write or no write access." );
    }

    //The Write Lock has been obtained so save it for later use
    this->writeLock = newLock;

    //Create a new lock using the name "commit.lock"
    LuceneLock* lock = directory->makeLock(IndexWriter::COMMIT_LOCK_NAME);

    //Condition check to see if lock has been allocated properly
    CND_CONDITION(lock != NULL, "No memory could be allocated for LuceneLock lock");

    LockWith2 with(lock, commitLockTimeout, this, NULL, create);
    {
        SCOPED_LOCK_MUTEX(directory->THIS_LOCK) // in- & inter-process sync
        with.run();
    }

    //Release the commit lock
    _CLDELETE(lock);

    isOpen = true;
}

IndexWriter::~IndexWriter()
{
    //Func - Destructor
    //Pre  - true
    //Post - The instance has been destroyed
    close();
    _finalize();
}

void IndexWriter::close()
{
    //Func - Flushes all changes to an index, closes all associated files, and
    //       closes the directory that the index is stored in.
    //Pre  - closeDir indicates if the directory must be closed or not
    //Post - All the changes have been flushed to disk and the write lock has
    //       been released. The ramDirectory has also been closed. The 
    //       directory has been closed if the reference count of the directory
    //       reaches zero

    SCOPED_LOCK_MUTEX(THIS_LOCK)
    if (isOpen) {
        //Flush the Ram Segments
        flushRamSegments();
        //Close the ram directory
        if (ramDirectory != NULL) {
            ramDirectory->close();
            _CLDECDELETE(ramDirectory);
        }

        //Check if this instance must close the directory
        if (closeDir)
            directory->close();
        _CLDECDELETE(directory);

        // release write lock
        if (writeLock != NULL) {
            writeLock->release();
            _CLDELETE(writeLock);
        }
        isOpen = false;
    }
}

void IndexWriter::_finalize()
{
    //Func - Releases all the resources of the instance
    //Pre  - true
    //Post - All the releases have been released

    if(writeLock != NULL) {
        //release write lock
        writeLock->release();
        _CLDELETE( writeLock );
    }

    //Delete the ramDirectory
    if (ramDirectory != NULL) {
        ramDirectory->close();
        _CLDECDELETE(ramDirectory);
    }
}

int32_t IndexWriter::docCount()
{
    //Func - Counts the number of documents in the index
    //Pre  - true
    //Post - The number of documents have been returned

    SCOPED_LOCK_MUTEX(THIS_LOCK)

    //Initialize count
    int32_t count = 0;

    //Iterate through all segmentInfos
    for (int32_t i = 0; i < segmentInfos.size(); i++) {
        //Get the i-th SegmentInfo
        SegmentInfo* si = segmentInfos.info(i);
        //Retrieve the number of documents of the segment and add it to count
        count += si->docCount;
    }
    return count;
}

void IndexWriter::addDocument(Document* doc, Analyzer* analyzer)
{
    //Func - Adds a document to the index
    //Pre  - doc contains a valid reference to a document
    //       ramDirectory != NULL
    //Post - The document has been added to the index of this IndexWriter
    CND_PRECONDITION(ramDirectory != NULL, "ramDirectory is NULL");

    if (analyzer == NULL)
        analyzer = this->analyzer;

    ramDirectory->transStart();
    try {
        QString segmentName = newSegmentName();
        CND_CONDITION(!segmentName.isEmpty(), "segmentName is NULL");
        try {
            //Create the DocumentWriter using a ramDirectory and analyzer
            // supplied by the IndexWriter (this).
            DocumentWriter* dw = _CLNEW DocumentWriter(ramDirectory, analyzer,
                this );
            CND_CONDITION(dw != NULL, "dw is NULL");
            try {
                //Add the client-supplied document to the new segment.
                dw->addDocument(segmentName, doc);
            } _CLFINALLY (
                _CLDELETE(dw);
            );

            //Create a new SegmentInfo instance about this new segment.
            SegmentInfo* si = _CLNEW SegmentInfo(segmentName, 1, ramDirectory);
            CND_CONDITION(si != NULL, "Si is NULL");
            {
                SCOPED_LOCK_MUTEX(THIS_LOCK)

                //Add the info object for this particular segment to the list
                // of all segmentInfos->
                segmentInfos.add(si);

                //Check to see if the segments must be merged
                maybeMergeSegments();
            }
        } _CLFINALLY()
    } catch (...) {
        ramDirectory->transAbort();
        throw;
}
    ramDirectory->transCommit();
}

void IndexWriter::optimize()
{
    //Func - Optimizes the index for which this Instance is responsible
    //Pre  - true
    //Post -
    SCOPED_LOCK_MUTEX(THIS_LOCK)

    //Flush the RamSegments to disk
    flushRamSegments();
    while (segmentInfos.size() > 1
        || (segmentInfos.size() == 1
            && (SegmentReader::hasDeletions(segmentInfos.info(0))
            || segmentInfos.info(0)->getDir()!=directory
            || (useCompoundFile 
            && (!SegmentReader::usesCompoundFile(segmentInfos.info(0))
                || SegmentReader::hasSeparateNorms(segmentInfos.info(0))))))) {
            int32_t minSegment = segmentInfos.size() - mergeFactor;
            mergeSegments(minSegment < 0 ? 0 : minSegment);
    }
}


QString IndexWriter::newSegmentName()
{
    SCOPED_LOCK_MUTEX(THIS_LOCK)
    return QLatin1Char('_') + QString::number(segmentInfos.counter++, 36);
}

void IndexWriter::flushRamSegments()
{
    //Func - Merges all RAM-resident segments.
    //Pre  - ramDirectory != NULL
    //Post - The RAM-resident segments have been merged to disk

    CND_PRECONDITION(ramDirectory != NULL, "ramDirectory is NULL");

    int32_t minSegment = segmentInfos.size()-1; //don't make this unsigned...
    CND_CONDITION(minSegment >= -1, "minSegment must be >= -1");

    int32_t docCount = 0;
    //Iterate through all the segements and check if the directory is a ramDirectory
    while (minSegment >= 0 &&
        segmentInfos.info(minSegment)->getDir() == ramDirectory) {
            docCount += segmentInfos.info(minSegment)->docCount;
            minSegment--;
    }
    if (minSegment < 0 ||			  // add one FS segment?
        (docCount + segmentInfos.info(minSegment)->docCount) > mergeFactor ||
        !(segmentInfos.info(segmentInfos.size()-1)->getDir() == ramDirectory))
        minSegment++;

    CND_CONDITION(minSegment >= 0, "minSegment must be >= 0");
    if (minSegment >= segmentInfos.size())
        return;					  // none to merge
    mergeSegments(minSegment);
}

void IndexWriter::maybeMergeSegments() {
    //Func - Incremental Segment Merger
    //Pre  -
    //Post -

    int64_t targetMergeDocs = minMergeDocs;

    // find segments smaller than current target size
    while (targetMergeDocs <= maxMergeDocs) {
        int32_t minSegment = segmentInfos.size();
        int32_t mergeDocs = 0;

        while (--minSegment >= 0) {
            SegmentInfo* si = segmentInfos.info(minSegment);
            if (si->docCount >= targetMergeDocs)
                break;
            mergeDocs += si->docCount;
        }

        if (mergeDocs >= targetMergeDocs){
            // found a merge to do
            mergeSegments(minSegment+1);
        }else
            break;

        //increase target size
        targetMergeDocs *= mergeFactor;
    }
}

void IndexWriter::mergeSegments(const uint32_t minSegment)
{
    mergeSegments(minSegment, segmentInfos.size());
}

void IndexWriter::mergeSegments(const uint32_t minSegment, const uint32_t end)
{
    CLVector<SegmentReader*> segmentsToDelete(false);
    QString mergedName = newSegmentName();
#ifdef _CL_DEBUG_INFO
    fprintf(_CL_DEBUG_INFO, "merging segments\n");
#endif
    SegmentMerger merger(this, mergedName);
    for (size_t i = minSegment; i < end; i++) {
        SegmentInfo* si = segmentInfos.info(i);
#ifdef _CL_DEBUG_INFO
        fprintf(_CL_DEBUG_INFO, " %s (%d docs)\n", 
            si->name.toLocal8Bit().constData(), si->docCount);
#endif
        SegmentReader* reader = _CLNEW SegmentReader(si);
        merger.add(reader);
        // if we own the directory
        if ((reader->getDirectory() == this->directory)
            || (reader->getDirectory() == this->ramDirectory)) {
                // queue segment for deletion
                segmentsToDelete.push_back(reader);	  
        }
    }

    int32_t mergedDocCount = merger.merge();

#ifdef _CL_DEBUG_INFO
    fprintf(_CL_DEBUG_INFO, "\n into %s (%d docs)\n",
        mergedName.toLocal8Bit().constData(), mergedDocCount);
#endif

    segmentInfos.clearto(minSegment);// remove old infos & add new
    segmentInfos.add(_CLNEW SegmentInfo(mergedName, mergedDocCount, directory));

    // close readers before we attempt to delete now-obsolete segments
    merger.closeReaders();

    LuceneLock* lock = directory->makeLock(IndexWriter::COMMIT_LOCK_NAME);
    LockWith2 with (lock, commitLockTimeout, this, &segmentsToDelete, true);
    {
        SCOPED_LOCK_MUTEX(directory->THIS_LOCK) // in- & inter-process sync
        with.run();
    }
    _CLDELETE( lock );

    if (useCompoundFile) {
        QStringList filesToDelete;
        merger.createCompoundFile(mergedName + QLatin1String(".tmp"), filesToDelete);

        LuceneLock* lock = directory->makeLock(IndexWriter::COMMIT_LOCK_NAME);
        LockWithCFS with(lock, commitLockTimeout, directory, this, mergedName,
            filesToDelete);
        {
            SCOPED_LOCK_MUTEX(directory->THIS_LOCK) // in- & inter-process sync
            with.run();
        }
        _CLDELETE(lock);
    }
}

void IndexWriter::deleteSegments(CLVector<SegmentReader*>* segments)
{
    QStringList deletable;

    {//scope delete deleteArray object
        QStringList deleteArray;
        readDeleteableFiles(deleteArray);
        deleteFiles(deleteArray, deletable); // try to delete deleteable
    }

    QStringList files;
    for (uint32_t i = 0; i < segments->size(); i++) {
        SegmentReader* reader = (*segments)[i];
        files.clear();
        reader->files(files);
        if (reader->getDirectory() == this->directory)
            deleteFiles(files, deletable);	  // try to delete our files
        else
            deleteFiles(files, reader->getDirectory()); // delete, eg, RAM files
    }

    writeDeleteableFiles(deletable);		  // note files we can't delete
}

void IndexWriter::deleteFiles(const QStringList& files)
{
    QStringList currentDeletable;
    readDeleteableFiles(currentDeletable);

    // try to delete deleteable
    QStringList deletable;
    deleteFiles(currentDeletable, deletable);
    
    // try to delete our files
    deleteFiles(files, deletable);

    // note files we can't delete
    writeDeleteableFiles(deletable);
}

void IndexWriter::readDeleteableFiles(QStringList& result)
{
    if (!directory->fileExists(QLatin1String("deletable")))
        return;

    IndexInput* input = directory->openInput(QLatin1String("deletable"));
    try {
        // read file names
        TCHAR tname[CL_MAX_PATH];
        for (int32_t i = input->readInt(); i > 0; i--) {
            int32_t read = input->readString(tname, CL_MAX_PATH);
            result.push_back(QString::fromWCharArray(tname, read));
        }
    } _CLFINALLY (
        input->close();
        _CLDELETE(input);
    );
}

void IndexWriter::deleteFiles(const QStringList& files, QStringList& deletable)
{
    QStringList::const_iterator itr;
    for (itr = files.begin(); itr != files.end(); ++itr) {
        if (!getDirectory()->fileExists((*itr)))
            continue;

        if (!getDirectory()->deleteFile((*itr), false)) {
            if (directory->fileExists((*itr))) {
#ifdef _CL_DEBUG_INFO
                fprintf(_CL_DEBUG_INFO, "%s; Will re-try later.\n", err.what());
#endif
                // add to deletable
                deletable.push_back((*itr));
            }
        }
    }
}

void IndexWriter::deleteFiles(const QStringList& files, Directory* directory)
{
    QStringList::const_iterator itr;
    for (itr = files.begin(); itr != files.end(); ++itr)
        directory->deleteFile((*itr), true);
}

void IndexWriter::writeDeleteableFiles(const QStringList& files)
{
    IndexOutput* output = directory->createOutput(QLatin1String("deleteable.new"));
    try {
        output->writeInt(files.size());

        TCHAR tfile[CL_MAX_PATH];
        QStringList::const_iterator itr;
        for (itr  = files.begin(); itr != files.end(); ++itr) {
            tfile[(*itr).toWCharArray(tfile)] = '\0';
            output->writeString(tfile, _tcslen(tfile));
        }
    } _CLFINALLY (
        output->close();
        _CLDELETE(output);
    );

    directory->renameFile(QLatin1String("deleteable.new"),
        QLatin1String("deletable"));
}

void IndexWriter::addIndexes(Directory** dirs)
{
    //Func - Add several indexes located in different directories into the current
    //       one managed by this instance
    //Pre  - dirs != NULL and contains directories of several indexes
    //       dirsLength > 0 and contains the number of directories
    //Post - The indexes located in the directories in dirs have been merged with
    //       the pre(current) index. The Resulting index has also been optimized

    SCOPED_LOCK_MUTEX(THIS_LOCK)

    CND_PRECONDITION(dirs != NULL, "dirs is NULL");

    // start with zero or 1 seg so optimize the current
    optimize();

    int32_t start = segmentInfos.size();

    //Iterate through the directories
    for (int32_t i = 0; dirs[i] != NULL; ++i) {
        // DSR: Changed SegmentInfos constructor arg (see bug discussion below).
        SegmentInfos sis(false);
        sis.read(dirs[i]);
        for (int32_t j = 0; j < sis.size(); j++)
            segmentInfos.add(sis.info(j));	  // add each info
    }

    // commented out by tbusch to solve a bug and to be conform with
    // java lucene

    // merge newly added segments in log(n) passes
    //while (segmentInfos.size() > start + mergeFactor) {
    //    for (int32_t base = start; base < segmentInfos.size(); base++) {
    //        int32_t end = min(segmentInfos.size(), base + mergeFactor);
    //        if (end - base > 1)
    //            mergeSegments(base, end);
    //    }
    //}

    // cleanup
    optimize();
}


void IndexWriter::addIndexes(IndexReader** readers)
{
    SCOPED_LOCK_MUTEX(THIS_LOCK)
    optimize();					  // start with zero or 1 seg

    QString mergedName = newSegmentName();
    SegmentMerger merger(this, mergedName);

    CLVector<SegmentReader*> segmentsToDelete;
    SegmentReader* sReader = NULL;
    if (segmentInfos.size() == 1) { // add existing index, if any
        sReader = _CLNEW SegmentReader(segmentInfos.info(0));
        merger.add(sReader);
        segmentsToDelete.push_back(sReader);   // queue segment for deletion
    }

    int32_t readersLength = 0;
    while (readers[readersLength] != NULL)
        merger.add(readers[readersLength++]);

    int32_t docCount = merger.merge();                // merge 'em

    // pop old infos & add new
    segmentInfos.clearto(0);
    segmentInfos.add(_CLNEW SegmentInfo(mergedName, docCount, directory));

    if (sReader != NULL) {
        sReader->close();
        _CLDELETE(sReader);
    }

    LuceneLock* lock = directory->makeLock(IndexWriter::COMMIT_LOCK_NAME);
    LockWith2 with(lock, commitLockTimeout, this, &segmentsToDelete, true);
    {
        // in- & inter-process sync
        SCOPED_LOCK_MUTEX(directory->THIS_LOCK)
        with.run();
    }
    _CLDELETE(lock);

    if (useCompoundFile) {
        QStringList filesToDelete;
        merger.createCompoundFile(mergedName + QLatin1String(".tmp"),
            filesToDelete);

        LuceneLock* cfslock = directory->makeLock(IndexWriter::COMMIT_LOCK_NAME);
        LockWithCFS with(cfslock, commitLockTimeout, directory, this, mergedName,
            filesToDelete);
        {
            // in- & inter-process sync
            SCOPED_LOCK_MUTEX(directory->THIS_LOCK)
            with.run();
        }
        _CLDELETE(cfslock);
    }
}

// #pragma mark -- IndexWriter::LockWith2

IndexWriter::LockWith2::LockWith2(CL_NS(store)::LuceneLock* lock,
                                  int64_t lockWaitTimeout,
                                  IndexWriter* indexWriter,
                                  CL_NS(util)::CLVector<SegmentReader*>* std,
                                  bool _create)
    : CL_NS(store)::LuceneLockWith<void>(lock, lockWaitTimeout)
    , create(_create)
    , writer(indexWriter)
    , segmentsToDelete(std)
{
}

void IndexWriter::LockWith2::doBody()
{
    //Func - Writes segmentInfos to or reads  segmentInfos from disk
    //Pre  - writer != NULL
    //Post - if create is true then segementInfos has been written to disk 
    //       otherwise segmentInfos has been read from disk

    CND_PRECONDITION(writer != NULL, "writer is NULL");

    if (create) {
        writer->segmentInfos.write(writer->getDirectory());
        // delete now-unused segments
        if (segmentsToDelete != NULL)
            writer->deleteSegments(segmentsToDelete);
    } else {
        writer->segmentInfos.read(writer->getDirectory());
    }
}

// #pragma mark -- IndexWriter::LockWithCFS

IndexWriter::LockWithCFS::LockWithCFS(CL_NS(store)::LuceneLock* lock,
                                      int64_t lockWaitTimeout,
                                      CL_NS(store)::Directory* dir,
                                      IndexWriter* indexWriter,
                                      const QString& segmentName,
                                      const QStringList& ftd)
    : CL_NS(store)::LuceneLockWith<void>(lock, lockWaitTimeout)
    , segName(segmentName)
    , writer(indexWriter)
    , directory(dir)
    , filesToDelete(ftd)
{
}

void IndexWriter::LockWithCFS::doBody()
{
    //Func - Writes segmentInfos to or reads  segmentInfos from disk
    //Pre  - writer != NULL
    //Post - if create is true then segementInfos has been written to disk
    //       otherwise segmentInfos has been read from disk

    CND_PRECONDITION(directory != NULL, "directory is NULL");
    CND_PRECONDITION(!segName.isEmpty(), "mergedName is NULL");

    // make compound file visible for SegmentReaders
    directory->renameFile(segName + QLatin1String(".tmp"),
        segName + QLatin1String(".cfs"));
    // delete now unused files of segment 
    writer->deleteFiles(filesToDelete);   
}

CL_NS_END
