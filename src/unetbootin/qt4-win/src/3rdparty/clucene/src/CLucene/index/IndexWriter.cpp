/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
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


  const char* IndexWriter::WRITE_LOCK_NAME = "write.lock";
  const char* IndexWriter::COMMIT_LOCK_NAME = "commit.lock";

  IndexWriter::IndexWriter(const char* path, Analyzer* a, const bool create, const bool _closeDir):
		directory( FSDirectory::getDirectory(path, create) ),
		analyzer(a),
		segmentInfos (_CLNEW SegmentInfos),
		closeDir(_closeDir){
  //Func - Constructor
  //       Constructs an IndexWriter for the index in path.
  //Pre  - path != NULL and contains a named directory path
  //       a holds a valid reference to an analyzer and analyzes the text to be indexed
  //       create indicates if the indexWriter must create a new index located at path or just open it
  //Post - If create is true, then a new, empty index has been created in path, replacing the index
  //       already there, if any. The named directory path is owned by this Instance

	  CND_PRECONDITION(path != NULL, "path is NULL");

	  //Continue initializing the instance by _IndexWriter
	  _IndexWriter ( create );
  }

  IndexWriter::IndexWriter(Directory* d, Analyzer* a, const bool create, const bool _closeDir):
	  directory(_CL_POINTER(d)),
	  analyzer(a),
	  segmentInfos (_CLNEW SegmentInfos),
      closeDir(_closeDir)
  {
  //Func - Constructor
  //       Constructs an IndexWriter for the index in path.
  //Pre  - d contains a valid reference to a directory
  //       a holds a valid reference to an analyzer and analyzes the text to be indexed
  //       create indicates if the indexWriter must create a new index located at path or just open it
  //Post - If create is true, then a new, empty index has been created in path, replacing the index
  //       already there, if any. The directory d is not owned by this Instance

	  //Continue initializing the instance by _IndexWriter
	  _IndexWriter ( create );
  }

  void IndexWriter::_IndexWriter(const bool create){
  //Func - Initialises the instances
  //Pre  - create indicates if the indexWriter must create a new index located at path or just open it
  //Post -

	similarity = CL_NS(search)::Similarity::getDefault();

	useCompoundFile = true;
	if ( directory->getDirectoryType() == RAMDirectory::DirectoryType() )
	    useCompoundFile = false;

	//Create a ramDirectory
	ramDirectory = _CLNEW TransactionalRAMDirectory;

	CND_CONDITION(ramDirectory != NULL,"ramDirectory is NULL");

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
	CND_CONDITION(newLock != NULL, "No memory could be allocated for LuceneLock newLock");

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

	LockWith2 with ( lock,commitLockTimeout,this, NULL, create );
	{
		SCOPED_LOCK_MUTEX(directory->THIS_LOCK) // in- & inter-process sync
		with.run();
	}

	//Release the commit lock
	_CLDELETE(lock);

   isOpen = true;
  }

  void IndexWriter::_finalize(){
  //Func - Releases all the resources of the instance
  //Pre  - true
  //Post - All the releases have been released

	  if(writeLock != NULL){
		  //release write lock
		  writeLock->release();
		  _CLDELETE( writeLock );
	  }

	  //Delete the ramDirectory
	  if ( ramDirectory != NULL ){
			ramDirectory->close();
			_CLDECDELETE(ramDirectory);
	  }

	  //clear segmentInfos and delete it
	  _CLDELETE(segmentInfos);

  }

  IndexWriter::~IndexWriter() {
  //Func - Destructor
  //Pre  - true
  //Post - The instance has been destroyed
     close();
	 _finalize();
  }


  void IndexWriter::close( ) {
  //Func - Flushes all changes to an index, closes all associated files, and closes
  //       the directory that the index is stored in.
  //Pre  - closeDir indicates if the directory must be closed or not
  //Post - All the changes have been flushed to disk and the write lock has been released
  //       The ramDirectory has also been closed. The directory has been closed
  //       if the reference count of the directory reaches zero

	 SCOPED_LOCK_MUTEX(THIS_LOCK)
     if ( isOpen ){
	   //Flush the Ram Segments
	   flushRamSegments();
	   //Close the ram directory
	   if ( ramDirectory != NULL ){
		  ramDirectory->close();
		  _CLDECDELETE(ramDirectory);
	   }

	   //Check if this instance must close the directory
	   if ( closeDir ){
		   directory->close();
	   }
	   _CLDECDELETE(directory);

      // release write lock
	   if (writeLock != NULL){
		   writeLock->release();
		   _CLDELETE( writeLock );
	   }

       isOpen = false;
     }
  }


  int32_t IndexWriter::docCount(){
  //Func - Counts the number of documents in the index
  //Pre  - true
  //Post - The number of documents have been returned

	  SCOPED_LOCK_MUTEX(THIS_LOCK)
	  
	  //Initialize count
	  int32_t count = 0;

	  //Iterate through all segmentInfos
	  for (int32_t i = 0; i < segmentInfos->size(); i++) {
		  //Get the i-th SegmentInfo
		  SegmentInfo* si = segmentInfos->info(i);
		  //Retrieve the number of documents of the segment and add it to count
		  count += si->docCount;
      }
	  return count;
  }

  void IndexWriter::addDocument(Document* doc, Analyzer* analyzer) {
  //Func - Adds a document to the index
  //Pre  - doc contains a valid reference to a document
  //       ramDirectory != NULL
  //Post - The document has been added to the index of this IndexWriter
	CND_PRECONDITION(ramDirectory != NULL,"ramDirectory is NULL");

	if ( analyzer == NULL )
		analyzer = this->analyzer;

	ramDirectory->transStart();
	try {
		char* segmentName = newSegmentName();
		CND_CONDITION(segmentName != NULL, "segmentName is NULL");
		try {
			//Create the DocumentWriter using a ramDirectory and analyzer
			// supplied by the IndexWriter (this).
			DocumentWriter* dw = _CLNEW DocumentWriter(
				ramDirectory, analyzer, this );
			CND_CONDITION(dw != NULL, "dw is NULL");
			try {
				//Add the client-supplied document to the new segment.
				dw->addDocument(segmentName, doc);
			} _CLFINALLY(
				_CLDELETE(dw);
			);

			//Create a new SegmentInfo instance about this new segment.
			SegmentInfo* si = _CLNEW SegmentInfo(segmentName, 1, ramDirectory);
			CND_CONDITION(si != NULL, "Si is NULL");

			{
				SCOPED_LOCK_MUTEX(THIS_LOCK)

   				//Add the info object for this particular segment to the list
   				// of all segmentInfos->
   				segmentInfos->add(si);
	   			
          		//Check to see if the segments must be merged
          		maybeMergeSegments();
			}
		} _CLFINALLY(
			_CLDELETE_CaARRAY(segmentName);
		);
		
	} catch (...) {
		ramDirectory->transAbort();
		throw;
	}
	ramDirectory->transCommit();
  }


  void IndexWriter::optimize() {
  //Func - Optimizes the index for which this Instance is responsible
  //Pre  - true
  //Post -
    SCOPED_LOCK_MUTEX(THIS_LOCK)
	//Flush the RamSegments to disk
    flushRamSegments();
    while (segmentInfos->size() > 1 ||
		(segmentInfos->size() == 1 &&
			(SegmentReader::hasDeletions(segmentInfos->info(0)) ||
            segmentInfos->info(0)->getDir()!=directory ||
            (useCompoundFile &&
              (!SegmentReader::usesCompoundFile(segmentInfos->info(0)) ||
                SegmentReader::hasSeparateNorms(segmentInfos->info(0))))))) {

		int32_t minSegment = segmentInfos->size() - mergeFactor;

		mergeSegments(minSegment < 0 ? 0 : minSegment);
	}
  }


  char* IndexWriter::newSegmentName() {
    SCOPED_LOCK_MUTEX(THIS_LOCK)

    TCHAR buf[9];
    _i64tot(segmentInfos->counter++,buf,36); //36 is RADIX of 10 digits and 26 numbers

	int32_t rlen = _tcslen(buf) + 2;
	char* ret = _CL_NEWARRAY(char,rlen);
	strcpy(ret,"_");
	STRCPY_TtoA(ret+1,buf,rlen-1); //write at 2nd character, for a maximum of 9 characters
    return ret;
  }

  void IndexWriter::flushRamSegments() {
  //Func - Merges all RAM-resident segments.
  //Pre  - ramDirectory != NULL
  //Post - The RAM-resident segments have been merged to disk

	CND_PRECONDITION(ramDirectory != NULL, "ramDirectory is NULL");

    int32_t minSegment = segmentInfos->size()-1; //don't make this unsigned...
	CND_CONDITION(minSegment >= -1, "minSegment must be >= -1");

	int32_t docCount = 0;
	//Iterate through all the segements and check if the directory is a ramDirectory
	while (minSegment >= 0 &&
	  segmentInfos->info(minSegment)->getDir() == ramDirectory) {
	  docCount += segmentInfos->info(minSegment)->docCount;
	  minSegment--;
	}
	if (minSegment < 0 ||			  // add one FS segment?
		(docCount + segmentInfos->info(minSegment)->docCount) > mergeFactor ||
		!(segmentInfos->info(segmentInfos->size()-1)->getDir() == ramDirectory))
	  minSegment++;

	CND_CONDITION(minSegment >= 0, "minSegment must be >= 0");
	if (minSegment >= segmentInfos->size())
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
		int32_t minSegment = segmentInfos->size();
		int32_t mergeDocs = 0;

		while (--minSegment >= 0) {
			SegmentInfo* si = segmentInfos->info(minSegment);
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

  void IndexWriter::mergeSegments(const uint32_t minSegment) {
    mergeSegments(minSegment, segmentInfos->size());
  }

  void IndexWriter::mergeSegments(const uint32_t minSegment, const uint32_t end) {
    CLVector<SegmentReader*> segmentsToDelete(false);
    const char* mergedName = newSegmentName();
#ifdef _CL_DEBUG_INFO
	fprintf(_CL_DEBUG_INFO, "merging segments\n");
#endif
    SegmentMerger merger(this, mergedName);
    for (size_t i = minSegment; i < end; i++) {
      SegmentInfo* si = segmentInfos->info(i);
#ifdef _CL_DEBUG_INFO
	  fprintf(_CL_DEBUG_INFO, " %s (%d docs)\n",si->name,si->docCount);
#endif
      SegmentReader* reader = _CLNEW SegmentReader(si);
      merger.add(reader);
      if ((reader->getDirectory() == this->directory) || // if we own the directory
		(reader->getDirectory() == this->ramDirectory)){
        segmentsToDelete.push_back(reader);	  // queue segment for deletion
	  }
    }

    int32_t mergedDocCount = merger.merge();

#ifdef _CL_DEBUG_INFO
	 fprintf(_CL_DEBUG_INFO,"\n into %s (%d docs)\n",mergedName, mergedDocCount);
#endif

	segmentInfos->clearto(minSegment);// remove old infos & add new
    segmentInfos->add( _CLNEW SegmentInfo(mergedName, mergedDocCount, directory) );

    // close readers before we attempt to delete now-obsolete segments
    merger.closeReaders();

	LuceneLock* lock = directory->makeLock(IndexWriter::COMMIT_LOCK_NAME);
	LockWith2 with ( lock, commitLockTimeout,this, &segmentsToDelete, true );

    {
    	SCOPED_LOCK_MUTEX(directory->THIS_LOCK) // in- & inter-process sync
    	with.run();
    }
    _CLDELETE( lock );


	
    if (useCompoundFile) {
		char cmpdTmpName[CL_MAX_PATH];
		strcpy(cmpdTmpName,mergedName);
		strcat(cmpdTmpName,".tmp");

		AStringArrayWithDeletor filesToDelete;
		merger.createCompoundFile(cmpdTmpName, filesToDelete);

		LuceneLock* lock = directory->makeLock(IndexWriter::COMMIT_LOCK_NAME);
		LockWithCFS with ( lock,commitLockTimeout,directory, this, mergedName, &filesToDelete);
		{
			SCOPED_LOCK_MUTEX(directory->THIS_LOCK) // in- & inter-process sync
			with.run();
		}
		_CLDELETE(lock);
    }

    _CLDELETE_CaARRAY( mergedName );
  }

  void IndexWriter::deleteSegments(CLVector<SegmentReader*>* segments) {
    AStringArrayWithDeletor deletable;

	{//scope delete deleteArray object
		AStringArrayWithDeletor deleteArray;
		readDeleteableFiles(deleteArray);
		deleteFiles(deleteArray, deletable); // try to delete deleteable
	}

	AStringArrayWithDeletor files;
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

  void IndexWriter::readDeleteableFiles(AStringArrayWithDeletor& result) {
    if (!directory->fileExists("deletable"))
      return;

    IndexInput* input = directory->openInput("deletable");
    try {
		TCHAR tname[CL_MAX_PATH];
		for (int32_t i = input->readInt(); i > 0; i--){	  // read file names
			input->readString(tname,CL_MAX_PATH);
			result.push_back(STRDUP_TtoA(tname));
		}
    } _CLFINALLY(
        input->close();
        _CLDELETE(input);
    );
  }

  void IndexWriter::writeDeleteableFiles(AStringArrayWithDeletor& files) {
    IndexOutput* output = directory->createOutput("deleteable.new");
    try {
      output->writeInt(files.size());
	  TCHAR tfile[CL_MAX_PATH]; //temporary space for tchar file name
	  for (uint32_t i = 0; i < files.size(); i++){
		STRCPY_AtoT(tfile,files[i],CL_MAX_PATH);
        output->writeString( tfile, _tcslen(tfile) );
	  }
    } _CLFINALLY(
        output->close();
        _CLDELETE(output);
    );

    directory->renameFile("deleteable.new", "deletable");
  }

  void IndexWriter::deleteFiles(AStringArrayWithDeletor& files){
	AStringArrayWithDeletor deletable;
	AStringArrayWithDeletor currDeletable;
	readDeleteableFiles(currDeletable);
	deleteFiles(currDeletable, deletable); // try to delete deleteable
	deleteFiles(files, deletable);     // try to delete our files
	writeDeleteableFiles(deletable);        // note files we can't delete
  }

  void IndexWriter::deleteFiles(AStringArrayWithDeletor& files, Directory* directory) {
	AStringArrayWithDeletor::iterator itr = files.begin();
	while ( itr != files.end() ){
		directory->deleteFile( *itr, true );
		++itr;
	}
  }

  void IndexWriter::deleteFiles(AStringArrayWithDeletor& files, AStringArrayWithDeletor& deletable) {
	  AStringArrayWithDeletor::iterator itr=files.begin();
	  while ( itr != files.end() ){
		const char* file = *itr;
		if ( getDirectory()->fileExists(file) ){
			if ( !getDirectory()->deleteFile(file, false) ){
				if (directory->fileExists(file)) {
					#ifdef _CL_DEBUG_INFO
					fprintf(_CL_DEBUG_INFO,"%s; Will re-try later.\n", err.what());
					#endif
					deletable.push_back(STRDUP_AtoA(file));		  // add to deletable
				}
			}
		}
		++itr;
	 }
  }


  
  void IndexWriter::addIndexes(Directory** dirs) {
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
	  
	  int32_t start = segmentInfos->size();

	  //Iterate through the directories
      int32_t i = 0;
	  while ( dirs[i] != NULL ) {
		  // DSR: Changed SegmentInfos constructor arg (see bug discussion below).
		  SegmentInfos sis(false);
		  sis.read( dirs[i]);

		  for (int32_t j = 0; j < sis.size(); j++) {
			segmentInfos->add(sis.info(j));	  // add each info
		  }
        i++;
	}
	
	// merge newly added segments in log(n) passes
    while (segmentInfos->size() > start+mergeFactor) {
      for (int32_t base = start; base < segmentInfos->size(); base++) {
        int32_t end = min(segmentInfos->size(), base+mergeFactor);
        if (end-base > 1)
          mergeSegments(base, end);
      }
    }

	optimize();					  // cleanup
  }


  void IndexWriter::addIndexes(IndexReader** readers){
	SCOPED_LOCK_MUTEX(THIS_LOCK)
    optimize();					  // start with zero or 1 seg

    char* mergedName = newSegmentName();
    SegmentMerger merger(this, mergedName);

    CLVector<SegmentReader*> segmentsToDelete;
	SegmentReader* sReader = NULL;
    if (segmentInfos->size() == 1){ // add existing index, if any
        sReader = _CLNEW SegmentReader(segmentInfos->info(0));
		merger.add(sReader);
        segmentsToDelete.push_back(sReader);   // queue segment for deletion
    }

    int32_t readersLength = 0;
    while ( readers[readersLength] != NULL )
      merger.add(readers[readersLength++]);

    int32_t docCount = merger.merge();                // merge 'em

    // pop old infos & add new
	segmentInfos->clearto(0);
    segmentInfos->add(_CLNEW SegmentInfo(mergedName, docCount, directory));

	if ( sReader != NULL ){
		sReader->close();
		_CLDELETE(sReader);
	}

	LuceneLock* lock = directory->makeLock(IndexWriter::COMMIT_LOCK_NAME);
    LockWith2 with ( lock,commitLockTimeout,this, &segmentsToDelete, true);
	{
		SCOPED_LOCK_MUTEX(directory->THIS_LOCK) // in- & inter-process sync
	   	with.run();
	}
    _CLDELETE(lock);

	if (useCompoundFile) {
		char cmpdTmpName[CL_MAX_PATH];
		strcpy(cmpdTmpName,mergedName);
		strcat(cmpdTmpName,".tmp");

		AStringArrayWithDeletor filesToDelete;
		merger.createCompoundFile(cmpdTmpName, filesToDelete);

		LuceneLock* cfslock = directory->makeLock(IndexWriter::COMMIT_LOCK_NAME);
		LockWithCFS with ( lock,commitLockTimeout,directory, this, mergedName, &filesToDelete);
		{
			SCOPED_LOCK_MUTEX(directory->THIS_LOCK) // in- & inter-process sync
			with.run();
		}
		_CLDELETE(cfslock);
    }
    _CLDELETE_CaARRAY(mergedName);
  }


    IndexWriter::LockWith2::LockWith2(CL_NS(store)::LuceneLock* lock, int64_t lockWaitTimeout,
            IndexWriter* wr, 
            CL_NS(util)::CLVector<SegmentReader*>* std,
            bool create):
        CL_NS(store)::LuceneLockWith<void>(lock,lockWaitTimeout)
    {
        this->writer = wr;
        this->segmentsToDelete = std;
        this->create = create;
    }
  
  void IndexWriter::LockWith2::doBody() {
  //Func - Writes segmentInfos to or reads  segmentInfos from disk
  //Pre  - writer != NULL
  //Post - if create is true then segementInfos has been written to disk otherwise
  //       segmentInfos has been read from disk

	  CND_PRECONDITION(writer != NULL, "writer is NULL");

	  if (create){
		  writer->segmentInfos->write(writer->getDirectory());
		  if ( segmentsToDelete != NULL )
			writer->deleteSegments(segmentsToDelete);  // delete now-unused segments
	  }else
		  writer->segmentInfos->read(writer->getDirectory());
  }

    IndexWriter::LockWithCFS::LockWithCFS(CL_NS(store)::LuceneLock* lock, int64_t lockWaitTimeout, 
            CL_NS(store)::Directory* dir, 
            IndexWriter* wr, 
            const char* segName, 
            CL_NS(util)::AStringArrayWithDeletor* ftd):
        CL_NS(store)::LuceneLockWith<void>(lock,lockWaitTimeout)
    {
        this->segName = segName;
        this->directory = dir;
        this->writer = wr;
        this->filesToDelete = ftd;
    }
  void IndexWriter::LockWithCFS::doBody() {
  //Func - Writes segmentInfos to or reads  segmentInfos from disk
  //Pre  - writer != NULL
  //Post - if create is true then segementInfos has been written to disk otherwise
  //       segmentInfos has been read from disk

		CND_PRECONDITION(directory != NULL, "directory is NULL");
		CND_PRECONDITION(segName != NULL, "mergedName is NULL");

		char from[CL_MAX_PATH];
		char nu[CL_MAX_PATH];

		strcpy(from,segName);
		strcat(from,".tmp");
		strcpy(nu,segName);
		strcat(nu,".cfs");

		// make compound file visible for SegmentReaders
		directory->renameFile(from, nu);
		// delete now unused files of segment 
		writer->deleteFiles(*filesToDelete);   
  }

CL_NS_END
