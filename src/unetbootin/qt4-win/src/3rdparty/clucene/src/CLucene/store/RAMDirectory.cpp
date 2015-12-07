/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
*
* Changes are Copyright(C) 2007 by Trolltech ASA, all rights reserved.
------------------------------------------------------------------------------*/
#include "CLucene/StdHeader.h"
#include "RAMDirectory.h"

#include "Lock.h"
#include "Directory.h"
#include "FSDirectory.h"
#include "CLucene/index/IndexReader.h"
#include "CLucene/util/VoidMap.h"
#include "CLucene/util/Misc.h"
#include "CLucene/debug/condition.h"

CL_NS_USE(util)
CL_NS_DEF(store)

  RAMFile::RAMFile()
  {
     length = 0;
     lastModified = Misc::currentTimeMillis();
  }
  RAMFile::~RAMFile(){
  }


  RAMDirectory::RAMLock::RAMLock(const char* name, RAMDirectory* dir):
    directory(dir)
  {
  	fname = STRDUP_AtoA(name);
  }
  RAMDirectory::RAMLock::~RAMLock()
  {
    _CLDELETE_LCaARRAY( fname );
    directory = NULL;
  }
  TCHAR* RAMDirectory::RAMLock::toString(){
	  return STRDUP_TtoT(_T("LockFile@RAM"));
  }
  bool RAMDirectory::RAMLock::isLocked() {
   return directory->fileExists(fname);
  }
  bool RAMDirectory::RAMLock::obtain(){
    SCOPED_LOCK_MUTEX(directory->files_mutex);
    if (!directory->fileExists(fname)) {
        IndexOutput* tmp = directory->createOutput(fname);
        tmp->close();
        _CLDELETE(tmp);

      return true;
    }
    return false;
  }

  void RAMDirectory::RAMLock::release(){
    directory->deleteFile(fname);
  }


  RAMIndexOutput::~RAMIndexOutput(){
	if ( deleteFile ){
        _CLDELETE(file);
	}else
     	file = NULL;
  }
  RAMIndexOutput::RAMIndexOutput(RAMFile* f):file(f) {
    pointer = 0;
    deleteFile = false;
  }
  
  RAMIndexOutput::RAMIndexOutput():
     file(_CLNEW RAMFile)
  {
     pointer = 0;
     deleteFile = true;
  }

  void RAMIndexOutput::writeTo(IndexOutput* out){
    flush();
    int64_t end = file->length;
    int64_t pos = 0;
    int32_t p = 0;
    while (pos < end) {
      int32_t length = CL_NS(store)::BufferedIndexOutput::BUFFER_SIZE;
      int64_t nextPos = pos + length;
      if (nextPos > end) {                        // at the last buffer
        length = (int32_t)(end - pos);
      }
      out->writeBytes((uint8_t*)file->buffers[p++], length);
      pos = nextPos;
    }
  }

  void RAMIndexOutput::reset(){
	seek(_ILONGLONG(0));
    file->length = _ILONGLONG(0);
  }

  void RAMIndexOutput::flushBuffer(const uint8_t* src, const int32_t len) {
    uint8_t* b = NULL;
    int32_t bufferPos = 0;
    while (bufferPos != len) {
	    uint32_t bufferNumber = pointer/CL_NS(store)::BufferedIndexOutput::BUFFER_SIZE;
	    int32_t bufferOffset = pointer%CL_NS(store)::BufferedIndexOutput::BUFFER_SIZE;
	    int32_t bytesInBuffer = CL_NS(store)::BufferedIndexOutput::BUFFER_SIZE - bufferOffset;
	    int32_t remainInSrcBuffer = len - bufferPos;
      	int32_t bytesToCopy = bytesInBuffer >= remainInSrcBuffer ? remainInSrcBuffer : bytesInBuffer;
	
		if (bufferNumber == file->buffers.size()){
		  b = _CL_NEWARRAY(uint8_t, CL_NS(store)::BufferedIndexOutput::BUFFER_SIZE);
	      file->buffers.push_back( b );
		}else{
		  b = file->buffers[bufferNumber];	
		}
		memcpy(b+bufferOffset, src+bufferPos, bytesToCopy * sizeof(uint8_t));
		bufferPos += bytesToCopy;
        pointer += bytesToCopy;
	}
    if (pointer > file->length)
      file->length = pointer;

    file->lastModified = Misc::currentTimeMillis();
  }

  void RAMIndexOutput::close() {
    BufferedIndexOutput::close();
  }

  /** Random-at methods */
  void RAMIndexOutput::seek(const int64_t pos){
    BufferedIndexOutput::seek(pos);
    pointer = (int32_t)pos;
  }
  int64_t RAMIndexOutput::length() {
    return file->length;
  }


  RAMIndexInput::RAMIndexInput(RAMFile* f):
  	file(f) {
    pointer = 0;
    _length = f->length;
  }
  RAMIndexInput::RAMIndexInput(const RAMIndexInput& other):
    BufferedIndexInput(other)
  {
  	file = other.file;
    pointer = other.pointer;
    _length = other._length;
  }
  RAMIndexInput::~RAMIndexInput(){
      RAMIndexInput::close();
  }
  IndexInput* RAMIndexInput::clone() const
  {
    RAMIndexInput* ret = _CLNEW RAMIndexInput(*this);
    return ret;
  }
  int64_t RAMIndexInput::length() {
    return _length;
  }
  const char* RAMIndexInput::getDirectoryType() const{ 
	  return RAMDirectory::DirectoryType(); 
  }
  void RAMIndexInput::readInternal(uint8_t* dest, const int32_t len) {
    const int64_t bytesAvailable = file->length - pointer;
    int64_t remainder = len <= bytesAvailable ? len : bytesAvailable;
    int32_t start = pointer;
    int32_t destOffset = 0;
    while (remainder != 0) {
      int32_t bufferNumber = start / CL_NS(store)::BufferedIndexOutput::BUFFER_SIZE;
      int32_t bufferOffset = start % CL_NS(store)::BufferedIndexOutput::BUFFER_SIZE;
      int32_t bytesInBuffer = CL_NS(store)::BufferedIndexOutput::BUFFER_SIZE - bufferOffset;

	  /* The buffer's entire length (bufferLength) is defined by IndexInput.h
      ** as int32_t, so obviously the number of bytes in a given segment of the
      ** buffer won't exceed the the capacity of int32_t.  Therefore, the
      ** int64_t->int32_t cast on the next line is safe. */
      int32_t bytesToCopy = bytesInBuffer >= remainder ? static_cast<int32_t>(remainder) : bytesInBuffer;
      uint8_t* b = file->buffers[bufferNumber];
	  memcpy(dest+destOffset,b+bufferOffset,bytesToCopy * sizeof(uint8_t));

      destOffset += bytesToCopy;
      start += bytesToCopy;
      remainder -= bytesToCopy;
      pointer += bytesToCopy;
    }
  }

  void RAMIndexInput::close() {
    BufferedIndexInput::close();
  }

  void RAMIndexInput::seekInternal(const int64_t pos) {
	  CND_PRECONDITION(pos>=0 &&pos<this->_length,"Seeking out of range")
    pointer = (int32_t)pos;
  }






  void RAMDirectory::fileList(vector<string>* names) const{
    SCOPED_LOCK_MUTEX(files_mutex);

	FileMap::const_iterator itr = files.begin();
    while (itr != files.end()){
        names->push_back(itr->first);
        ++itr;
    }
  }

  RAMDirectory::RAMDirectory():
   Directory(),files(true,true)
  {
  }
  
  RAMDirectory::~RAMDirectory(){
   //todo: should call close directory?
  }

  void RAMDirectory::_copyFromDir(Directory* dir, bool closeDir)
  {
  	vector<string> names;
    dir->fileList(&names);
    uint8_t buf[CL_NS(store)::BufferedIndexOutput::BUFFER_SIZE];

    for (size_t i=0;i<names.size();++i ){
		if ( !CL_NS(index)::IndexReader::isLuceneFile(names[i].c_str()))
            continue;
            
        // make place on ram disk
        IndexOutput* os = createOutput(names[i].c_str());
        // read current file
        IndexInput* is = dir->openInput(names[i].c_str());
        // and copy to ram disk
        //todo: this could be a problem when copying from big indexes... 
        int64_t len = is->length();
        int64_t readCount = 0;
        while (readCount < len) {
            int32_t toRead = (int32_t)(readCount + CL_NS(store)::BufferedIndexOutput::BUFFER_SIZE > len ? len - readCount : CL_NS(store)::BufferedIndexOutput::BUFFER_SIZE);
            is->readBytes(buf, toRead);
            os->writeBytes(buf, toRead);
            readCount += toRead;
        }
        
        // graceful cleanup
        is->close();
        _CLDELETE(is);
        os->close();
        _CLDELETE(os);
    }
    if (closeDir)
       dir->close();
  }
  RAMDirectory::RAMDirectory(Directory* dir):
   Directory(),files(true,true)
  {
    _copyFromDir(dir,false);
    
  }
  
   RAMDirectory::RAMDirectory(const char* dir):
      Directory(),files(true,true)
   {
      Directory* fsdir = FSDirectory::getDirectory(dir,false);
      try{
         _copyFromDir(fsdir,false);
      }_CLFINALLY(fsdir->close();_CLDECDELETE(fsdir););

   }

  bool RAMDirectory::fileExists(const char* name) const {
    SCOPED_LOCK_MUTEX(files_mutex);
    return files.exists(name);
  }

  int64_t RAMDirectory::fileModified(const char* name) const {
	  SCOPED_LOCK_MUTEX(files_mutex);
	  const RAMFile* f = files.get(name);
	  return f->lastModified;
  }

  int64_t RAMDirectory::fileLength(const char* name) const{
	  SCOPED_LOCK_MUTEX(files_mutex);
	  RAMFile* f = files.get(name);
      return f->length;
  }


  IndexInput* RAMDirectory::openInput(const char* name) {
    SCOPED_LOCK_MUTEX(files_mutex);
    RAMFile* file = files.get(name);
    if (file == NULL) { /* DSR:PROPOSED: Better error checking. */
      _CLTHROWA(CL_ERR_IO,"[RAMDirectory::open] The requested file does not exist.");
    }
    return _CLNEW RAMIndexInput( file );
  }

  void RAMDirectory::close(){
      SCOPED_LOCK_MUTEX(files_mutex);
      files.clear();
  }

  bool RAMDirectory::doDeleteFile(const char* name) {
    SCOPED_LOCK_MUTEX(files_mutex);
    files.remove(name);
    return true;
  }

  void RAMDirectory::renameFile(const char* from, const char* to) {
	SCOPED_LOCK_MUTEX(files_mutex);
	FileMap::iterator itr = files.find(from);

    /* DSR:CL_BUG_LEAK:
    ** If a file named $to already existed, its old value was leaked.
    ** My inclination would be to prevent this implicit deletion with an
    ** exception, but it happens routinely in CLucene's internals (e.g., during
    ** IndexWriter.addIndexes with the file named 'segments'). */
    if (files.exists(to)) {
      files.remove(to);
    }
	if ( itr == files.end() ){
		char tmp[1024];
		_snprintf(tmp,1024,"cannot rename %s, file does not exist",from);
		_CLTHROWT(CL_ERR_IO,tmp);
	}
	CND_PRECONDITION(itr != files.end(), "itr==files.end()")
	RAMFile* file = itr->second;
    files.removeitr(itr,false,true);
    files.put(STRDUP_AtoA(to), file);
  }

  
  void RAMDirectory::touchFile(const char* name) {
    RAMFile* file = NULL;
    {
      SCOPED_LOCK_MUTEX(files_mutex);
      file = files.get(name);
	}
    uint64_t ts1 = file->lastModified;
    uint64_t ts2 = Misc::currentTimeMillis();

	//make sure that the time has actually changed
    while ( ts1==ts2 ) {
        _LUCENE_SLEEP(1);
        ts2 = Misc::currentTimeMillis();
    };

    file->lastModified = ts2;
  }

  IndexOutput* RAMDirectory::createOutput(const char* name) {
    /* Check the $files VoidMap to see if there was a previous file named
    ** $name.  If so, delete the old RAMFile object, but reuse the existing
    ** char buffer ($n) that holds the filename.  If not, duplicate the
    ** supplied filename buffer ($name) and pass ownership of that memory ($n)
    ** to $files. */

    SCOPED_LOCK_MUTEX(files_mutex);

    const char* n = files.getKey(name);
    if (n != NULL) {
	   RAMFile* rf = files.get(name);
      _CLDELETE(rf);
    } else {
      n = STRDUP_AtoA(name);
    }

    RAMFile* file = _CLNEW RAMFile();
    #ifdef _DEBUG
      file->filename = n;
    #endif
    files[n] = file;

    return _CLNEW RAMIndexOutput(file);
  }

  LuceneLock* RAMDirectory::makeLock(const char* name) {
    return _CLNEW RAMLock(name,this);
  }

  TCHAR* RAMDirectory::toString() const{
	return STRDUP_TtoT( _T("RAMDirectory") );
  }
CL_NS_END
