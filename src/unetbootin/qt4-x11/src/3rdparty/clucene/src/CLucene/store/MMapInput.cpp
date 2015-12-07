/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#include "CLucene/StdHeader.h"
#if defined(LUCENE_FS_MMAP)

#include "FSDirectory.h"
#include "CLucene/util/Misc.h"
#include "CLucene/debug/condition.h"

#ifndef _CLCOMPILER_MSVC
	#include <sys/mman.h>
#endif

CL_NS_DEF(store)
CL_NS_USE(util)

	FSDirectory::MMapIndexInput::MMapIndexInput(const char* path):
		pos(0),
		data(NULL),
		_length(0),
		isClone(false)
	{
	//Func - Constructor.
	//       Opens the file named path
	//Pre  - path != NULL
	//Post - if the file could not be opened  an exception is thrown.

	  CND_PRECONDITION(path != NULL, "path is NULL");

#ifdef _CLCOMPILER_MSVC
	  mmaphandle = NULL;
	  fhandle = CreateFileA(path,GENERIC_READ,FILE_SHARE_READ, 0,OPEN_EXISTING,0,0);
	  
	  //Check if a valid fhandle was retrieved
	  if (fhandle < 0){
		DWORD err = GetLastError();
        if ( err == ERROR_FILE_NOT_FOUND )
		    _CLTHROWA(CL_ERR_IO, "File does not exist");
        else if ( err == EACCES )
            _CLTHROWA(ERROR_ACCESS_DENIED, "File Access denied");
        else if ( err == ERROR_TOO_MANY_OPEN_FILES )
            _CLTHROWA(CL_ERR_IO, "Too many open files");
		else
			_CLTHROWA(CL_ERR_IO, "File IO Error");
	  }

	  DWORD dummy=0;
	  _length = GetFileSize(fhandle,&dummy);

	  if ( _length > 0 ){
		mmaphandle = CreateFileMappingA(fhandle,NULL,PAGE_READONLY,0,0,NULL);
		if ( mmaphandle != NULL ){
			void* address = MapViewOfFile(mmaphandle,FILE_MAP_READ,0,0,0);
			if ( address != NULL ){
				data = (uint8_t*)address;
				return; //SUCCESS!
			}
		}
		CloseHandle(mmaphandle);

		char* lpMsgBuf=0;
		DWORD dw = GetLastError(); 

		FormatMessageA(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			dw,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			lpMsgBuf,
			0, NULL );

		char* errstr = _CL_NEWARRAY(char, strlen(lpMsgBuf)+40); 
		sprintf(errstr, "MMapIndexInput::MMapIndexInput failed with error %d: %s", dw, lpMsgBuf); 
		LocalFree(lpMsgBuf);
	    
		_CLTHROWA_DEL(CL_ERR_IO,errstr);
	  }

#else //_CLCOMPILER_MSVC
	 fhandle = ::open (path, O_RDONLY);
  	 if (fhandle < 0){
		_CLTHROWA(CL_ERR_IO,strerror(errno));	
  	 }else{
		// stat it
		struct stat sb;
		if (::fstat (fhandle, &sb)){
			_CLTHROWA(CL_ERR_IO,strerror(errno));
		}else{
			// get length from stat
			_length = sb.st_size;
			
			// mmap the file
			void* address = ::mmap(0, _length, PROT_READ, MAP_SHARED, fhandle, 0);
			if (address == MAP_FAILED){
				_CLTHROWA(CL_ERR_IO,strerror(errno));
			}else{
				data = (uint8_t*)address;
			}
		}
  	 }
#endif
  }

  FSDirectory::MMapIndexInput::MMapIndexInput(const MMapIndexInput& clone): IndexInput(clone){
  //Func - Constructor
  //       Uses clone for its initialization
  //Pre  - clone is a valide instance of FSIndexInput
  //Post - The instance has been created and initialized by clone
  
#ifdef _CLCOMPILER_MSVC
	  mmaphandle = NULL;
	  fhandle = NULL;
#endif

	  data = clone.data;
	  pos = clone.pos;

	  //clone the file length
	  _length  = clone._length;
	  //Keep in mind that this instance is a clone
	  isClone = true;
  }

  uint8_t FSDirectory::MMapIndexInput::readByte(){
	  return *(data+(pos++));
  }

  void FSDirectory::MMapIndexInput::readBytes(uint8_t* b, const int32_t len){
	memcpy(b, data+pos, len);
	pos+=len;
  }
  int32_t FSDirectory::MMapIndexInput::readVInt(){
	  uint8_t b = *(data+(pos++));
	  int32_t i = b & 0x7F;
	  for (int shift = 7; (b & 0x80) != 0; shift += 7) {
	    b = *(data+(pos++));
	    i |= (b & 0x7F) << shift;
	  }
	  return i;
  }
  int64_t FSDirectory::MMapIndexInput::getFilePointer() const{
	return pos;
  }
  void FSDirectory::MMapIndexInput::seek(const int64_t pos){
	  this->pos=pos;
  }

  FSDirectory::MMapIndexInput::~MMapIndexInput(){
  //Func - Destructor
  //Pre  - True
  //Post - The file for which this instance is responsible has been closed.
  //       The instance has been destroyed

	  close();
  }

  IndexInput* FSDirectory::MMapIndexInput::clone() const
  {
    return _CLNEW FSDirectory::MMapIndexInput(*this);
  }
  void FSDirectory::MMapIndexInput::close()  {
    //IndexInput::close();

	if ( !isClone ){
#ifdef _CLCOMPILER_MSVC
		if ( data != NULL ){
			if ( ! UnmapViewOfFile(data) ){
				CND_PRECONDITION( false, "UnmapViewOfFile(data) failed"); //todo: change to rich error
			}
		}

		if ( mmaphandle != NULL ){
			if ( ! CloseHandle(mmaphandle) ){
				CND_PRECONDITION( false, "CloseHandle(mmaphandle) failed");
			}
		}
		if ( fhandle != NULL ){
			if ( !CloseHandle(fhandle) ){
				CND_PRECONDITION( false, "CloseHandle(fhandle) failed");
			}
		}
		mmaphandle = NULL;
		fhandle = NULL;
#else
		if ( data != NULL )
	  		::munmap(data, _length);
	  	if ( fhandle > 0 )
	  		::close(fhandle);
	  	fhandle = 0;
#endif
	}
	data = NULL;
	pos = 0;
  }


CL_NS_END
#endif
