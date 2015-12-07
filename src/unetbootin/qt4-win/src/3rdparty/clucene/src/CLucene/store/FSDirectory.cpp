/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
*
* Changes are Copyright(C) 2007 by Trolltech ASA, all rights reserved.
------------------------------------------------------------------------------*/

#if defined(_BUILD_FOR_QT_)
#   include <QtCore/QDir>
#   include <QtCore/QString>
#   include <QtCore/QFileInfo>
#   include <QtCore/QLatin1String>
#   include <QtCore/QCryptographicHash>

QT_USE_NAMESPACE
#else
#   include "CLucene/util/dirent.h" //if we have dirent, then the native one will be used
#endif

#include "CLucene/StdHeader.h"
#include "FSDirectory.h"
#include "CLucene/index/IndexReader.h"
#include "CLucene/util/Misc.h"
#include "CLucene/debug/condition.h"

CL_NS_DEF(store)
CL_NS_USE(util)

   /** This cache of directories ensures that there is a unique Directory
   * instance per path, so that synchronization on the Directory can be used to
   * synchronize access between readers and writers.
   */
	static CL_NS(util)::CLHashMap<const char*,FSDirectory*,CL_NS(util)::Compare::Char,CL_NS(util)::Equals::Char> DIRECTORIES(false,false);

	bool FSDirectory::disableLocks=false;

	FSDirectory::FSIndexInput::FSIndexInput(const char* path, int32_t __bufferSize):
		BufferedIndexInput(__bufferSize)	
    {
	//Func - Constructor.
	//       Opens the file named path
	//Pre  - path != NULL
	//Post - if the file could not be opened  an exception is thrown.

	  CND_PRECONDITION(path != NULL, "path is NULL");

	  handle = _CLNEW SharedHandle();
	  strcpy(handle->path,path);

	  //Open the file
	  handle->fhandle  = _open(path, O_BINARY | O_RDONLY | O_RANDOM, _S_IREAD );
	  
	  //Check if a valid handle was retrieved
	  if (handle->fhandle < 0){
		int err = errno;
        if ( err == ENOENT )
		    _CLTHROWA(CL_ERR_IO, "File does not exist");
        else if ( err == EACCES )
            _CLTHROWA(CL_ERR_IO, "File Access denied");
        else if ( err == EMFILE )
            _CLTHROWA(CL_ERR_IO, "Too many open files");
	  }

	  //Store the file length
	  handle->_length = fileSize(handle->fhandle);
	  handle->_fpos = 0;
	  this->_pos = 0;
  }

  FSDirectory::FSIndexInput::FSIndexInput(const FSIndexInput& other): BufferedIndexInput(other){
  //Func - Constructor
  //       Uses clone for its initialization
  //Pre  - clone is a valide instance of FSIndexInput
  //Post - The instance has been created and initialized by clone
	if ( other.handle == NULL )
		_CLTHROWA(CL_ERR_NullPointer, "other handle is null");

	SCOPED_LOCK_MUTEX(other.handle->THIS_LOCK)
	handle = _CL_POINTER(other.handle);
	_pos = other.handle->_fpos; //note where we are currently...
  }

  FSDirectory::FSIndexInput::SharedHandle::SharedHandle(){
  	fhandle = 0;
    _length = 0;
    _fpos = 0;
    path[0]=0;
  }
  FSDirectory::FSIndexInput::SharedHandle::~SharedHandle() throw(CLuceneError&){
    if ( fhandle >= 0 ){
      if ( _close(fhandle) != 0 )
        _CLTHROWA(CL_ERR_IO, "File IO Close error");
      else
        fhandle = -1;
    }
  }

  FSDirectory::FSIndexInput::~FSIndexInput(){
  //Func - Destructor
  //Pre  - True
  //Post - The file for which this instance is responsible has been closed.
  //       The instance has been destroyed

	  FSIndexInput::close();
  }

  IndexInput* FSDirectory::FSIndexInput::clone() const
  {
    return _CLNEW FSDirectory::FSIndexInput(*this);
  }
  void FSDirectory::FSIndexInput::close()  {
	BufferedIndexInput::close();
	_CLDECDELETE(handle);
  }

  void FSDirectory::FSIndexInput::seekInternal(const int64_t position)  {
	CND_PRECONDITION(position>=0 &&position<handle->_length,"Seeking out of range")
	_pos = position;
  }

/** IndexInput methods */
void FSDirectory::FSIndexInput::readInternal(uint8_t* b, const int32_t len) {
	SCOPED_LOCK_MUTEX(handle->THIS_LOCK)
	CND_PRECONDITION(handle!=NULL,"shared file handle has closed");
	CND_PRECONDITION(handle->fhandle>=0,"file is not open");

	if ( handle->_fpos != _pos ){
		if ( fileSeek(handle->fhandle,_pos,SEEK_SET) != _pos ){
			_CLTHROWA( CL_ERR_IO, "File IO Seek error");
		}
		handle->_fpos = _pos;
	}

	bufferLength = _read(handle->fhandle,b,len); // 2004.10.31:SF 1037836
	if (bufferLength == 0){
		_CLTHROWA(CL_ERR_IO, "read past EOF");
	}
	if (bufferLength == -1){
		//if (EINTR == errno) we could do something else... but we have
		//to guarantee some return, or throw EOF
		
		_CLTHROWA(CL_ERR_IO, "read error");
	}
	_pos+=bufferLength;
	handle->_fpos=_pos;
}

  FSDirectory::FSIndexOutput::FSIndexOutput(const char* path){
	//O_BINARY - Opens file in binary (untranslated) mode
	//O_CREAT - Creates and opens new file for writing. Has no effect if file specified by filename exists
	//O_RANDOM - Specifies that caching is optimized for, but not restricted to, random access from disk.
	//O_WRONLY - Opens file for writing only;
	if ( Misc::dir_Exists(path) )
	  fhandle = _open( path, O_BINARY | O_RDWR | O_RANDOM | O_TRUNC, _S_IREAD | _S_IWRITE);
	else // added by JBP
	  fhandle = _open( path, O_BINARY | O_RDWR | O_RANDOM | O_CREAT, _S_IREAD | _S_IWRITE);

	if ( fhandle < 0 ){
        int err = errno;
        if ( err == ENOENT )
    	    _CLTHROWA(CL_ERR_IO, "File does not exist");
        else if ( err == EACCES )
            _CLTHROWA(CL_ERR_IO, "File Access denied");
        else if ( err == EMFILE )
            _CLTHROWA(CL_ERR_IO, "Too many open files");
    }
  }
  FSDirectory::FSIndexOutput::~FSIndexOutput(){
	if ( fhandle >= 0 ){
	  try {
        FSIndexOutput::close();
	  }catch(CLuceneError& err){
	    //ignore IO errors...
	    if ( err.number() != CL_ERR_IO )
	        throw;
	  }
	}
  }

  /** output methods: */
  void FSDirectory::FSIndexOutput::flushBuffer(const uint8_t* b, const int32_t size) {
	  CND_PRECONDITION(fhandle>=0,"file is not open");
      if ( size > 0 && _write(fhandle,b,size) != size )
        _CLTHROWA(CL_ERR_IO, "File IO Write error");
  }
  void FSDirectory::FSIndexOutput::close() {
    try{
      BufferedIndexOutput::close();
    }catch(CLuceneError& err){
	    //ignore IO errors...
	    if ( err.number() != CL_ERR_IO )
	        throw;
    }

    if ( _close(fhandle) != 0 )
      _CLTHROWA(CL_ERR_IO, "File IO Close error");
    else
      fhandle = -1; //-1 now indicates closed
  }

  void FSDirectory::FSIndexOutput::seek(const int64_t pos) {
    CND_PRECONDITION(fhandle>=0,"file is not open");
    BufferedIndexOutput::seek(pos);
	int64_t ret = fileSeek(fhandle,pos,SEEK_SET);
	if ( ret != pos ){
      _CLTHROWA(CL_ERR_IO, "File IO Seek error");
	}
  }
  int64_t FSDirectory::FSIndexOutput::length(){
	  CND_PRECONDITION(fhandle>=0,"file is not open");
	  return fileSize(fhandle);
  }


	const char* FSDirectory::LOCK_DIR=NULL;
	const char* FSDirectory::getLockDir(){
		#ifdef LUCENE_LOCK_DIR
		LOCK_DIR = LUCENE_LOCK_DIR	
		#else	
			#ifdef LUCENE_LOCK_DIR_ENV_1
			if ( LOCK_DIR == NULL )
				LOCK_DIR = getenv(LUCENE_LOCK_DIR_ENV_1);
			#endif			
			#ifdef LUCENE_LOCK_DIR_ENV_2
			if ( LOCK_DIR == NULL )
				LOCK_DIR = getenv(LUCENE_LOCK_DIR_ENV_2);
			#endif
			#ifdef LUCENE_LOCK_DIR_ENV_FALLBACK
			if ( LOCK_DIR == NULL )
				LOCK_DIR=LUCENE_LOCK_DIR_ENV_FALLBACK;
			#endif
			if ( LOCK_DIR == NULL )
				_CLTHROWA(CL_ERR_IO, "Couldn't get determine lock dir");
		#endif
		
		return LOCK_DIR;
	}

  FSDirectory::FSDirectory(const char* path, const bool createDir):
   Directory(),
   refCount(0),
   useMMap(false)
  {
  	_realpath(path,directory);//set a realpath so that if we change directory, we can still function
  	if ( !directory || !*directory ){
  		strcpy(directory,path);	
  	}
    
#if defined(_BUILD_FOR_QT_)
    strcpy(lockDir, directory);

    QString lockDirPath = QLatin1String(lockDir);
    QDir dir(lockDirPath);
    if (!dir.exists()) {
        if (!dir.mkpath(QLatin1String(lockDir)))
            _CLTHROWA_DEL(CL_ERR_IO, "Cannot create temp directory");
    }

    QFileInfo info(lockDirPath);
    if (info.isFile() || info.isSymLink())
        _CLTHROWA(CL_ERR_IO, "Found regular file where directory expected");
#else
	const char* tmplockdir = getLockDir();
    if ( tmplockdir == NULL ) {
      strcpy(lockDir,directory);
    } else {
      strcpy(lockDir,tmplockdir);
    }

    // Ensure that lockDir exists and is a directory.
	struct fileStat fstat;
    if ( fileStat(tmplockdir,&fstat) != 0 ) {
		//todo: should construct directory using _mkdirs... have to write replacement
		if ( _mkdir(directory) == -1 ){
			_CLTHROWA(CL_ERR_IO,"Cannot create temp directory"); //todo: make richer error
		}
	}
	if ( !(fstat.st_mode & S_IFDIR) ){
		_CLTHROWA(CL_ERR_IO, "Found regular file where directory expected"); //todo: make richer error: " + lockDir);
    }
#endif

    if (createDir) {
      create();
    }

    if (!Misc::dir_Exists(directory)){
		char* err = _CL_NEWARRAY(char,19+strlen(path)+1); //19: len of " is not a directory"
		strcpy(err,path);
		strcat(err," is not a directory");
        _CLTHROWA_DEL(CL_ERR_IO, err );
    }
  }


  void FSDirectory::create()
  {
    SCOPED_LOCK_MUTEX(THIS_LOCK)
#if defined(_BUILD_FOR_QT_)
    bool clear = false;
    QString fsDirPath = QLatin1String(directory);
    QDir dir(fsDirPath);
    if (!dir.exists()) {
        if (!dir.mkpath(fsDirPath)) {
            char* err = _CL_NEWARRAY(char, 27 + strlen(directory)+1); //27: len of "Couldn't create directory: "
            strcpy(err, "Couldn't create directory: ");
            strcat(err, directory);
            _CLTHROWA_DEL(CL_ERR_IO, err);
        }
    } else {
        clear = true;
    }

    QFileInfo info(fsDirPath);
    if (info.isFile() || info.isSymLink()) {
        char tmp[1024];
        _snprintf(tmp, 1024, "%s not a directory", directory);
        _CLTHROWA(CL_ERR_IO, tmp);
    }

    if (clear) {
        dir.setPath(fsDirPath);

        // clear probably existing lucene index files 
        QStringList fileList = dir.entryList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
        foreach(const QString file, fileList) {
            const QByteArray tmpPath = file.toLocal8Bit();
            if (CL_NS(index)::IndexReader::isLuceneFile(tmpPath.constData())) {
                if (!dir.remove(file))
                    _CLTHROWA(CL_ERR_IO, "Couldn't delete file "); //todo: make richer error
            }
        }

        // clear probably existing file locks
        QString fsLockDirName = QLatin1String(lockDir);
        QFileInfo dirInfo(fsLockDirName);
        if (dirInfo.exists() && dirInfo.isReadable() && dirInfo.isWritable() && !dirInfo.isFile() && !dirInfo.isSymLink()) {
            char* lockPrefix = getLockPrefix();
            QDir lockDirectory(fsLockDirName);
            fileList = dir.entryList(QStringList() << (QLatin1String(lockPrefix) + QLatin1String("*")),
                QDir::Files | QDir::Hidden | QDir::NoSymLinks);

            foreach(const QString file, fileList) {
                if (!lockDirectory.remove(file)) {
                    _CLDELETE_CaARRAY(lockPrefix);
                    _CLTHROWA(CL_ERR_IO, "Couldn't delete file "); //todo: make richer error
                }
            }

            _CLDELETE_CaARRAY(lockPrefix);
        }
        else {
            _CLTHROWA(CL_ERR_IO, "Cannot read lock directory"); //todo: richer error: + lockDir.getAbsolutePath());
        }
    }   
#else
    struct fileStat fstat;
    if ( fileStat(directory,&fstat) != 0 ) {
	  //todo: should construct directory using _mkdirs... have to write replacement
      if ( _mkdir(directory) == -1 ){
		  char* err = _CL_NEWARRAY(char,27+strlen(directory)+1); //27: len of "Couldn't create directory: "
		  strcpy(err,"Couldn't create directory: ");
		  strcat(err,directory);
		  _CLTHROWA_DEL(CL_ERR_IO, err );
      }
	}
	if ( !(fstat.st_mode & S_IFDIR) ){
      char tmp[1024];
      _snprintf(tmp,1024,"%s not a directory", directory);
      _CLTHROWA(CL_ERR_IO,tmp);
	}

	  //clear old files
    DIR* dir = opendir(directory);
    struct dirent* fl = readdir(dir);
    struct fileStat buf;

    char path[CL_MAX_DIR];
    while ( fl != NULL ){
	  if ( CL_NS(index)::IndexReader::isLuceneFile(fl->d_name) ){
		_snprintf(path,CL_MAX_DIR,"%s/%s",directory,fl->d_name);
		int32_t ret = fileStat(path,&buf);
		if ( ret==0 && !(buf.st_mode & S_IFDIR) ) {
			if ( (strcmp(fl->d_name, ".")) && (strcmp(fl->d_name, "..")) ) {
				if ( _unlink( path ) == -1 ) {
				  closedir(dir);
				  _CLTHROWA(CL_ERR_IO, "Couldn't delete file "); //todo: make richer error
				}
			}
		}
	  }
	  fl = readdir(dir);
    }
    closedir(dir);

	char* lockPrefix = getLockPrefix(); // clear old locks
	size_t lockPrefixLen = strlen(lockPrefix);
	
    dir = opendir(lockDir);
	if ( dir == NULL )
		_CLTHROWA(CL_ERR_IO, "Cannot read lock directory"); //todo: richer error: + lockDir.getAbsolutePath());
	fl = readdir(dir);
    while ( fl != NULL ){
		if (strncmp(fl->d_name,lockPrefix,lockPrefixLen)==0){
			_snprintf(path,CL_MAX_DIR,"%s/%s",lockDir,fl->d_name);
			if ( _unlink( path ) == -1 ) {
				closedir(dir);
				_CLDELETE_CaARRAY(lockPrefix);
				_CLTHROWA(CL_ERR_IO, "Couldn't delete file "); //todo: make richer error
			}
		}
      fl = readdir(dir);
    }
    closedir(dir);
    _CLDELETE_CaARRAY(lockPrefix);
#endif
  }

  void FSDirectory::priv_getFN(char* buffer, const char* name) const{
      buffer[0] = 0;
      strcpy(buffer,directory);
      strcat(buffer, PATH_DELIMITERA );
      strcat(buffer,name);
  }

  FSDirectory::~FSDirectory(){
  }

  //todo: fix this, ugly!!!
  void FSDirectory::fileList(vector<string>* names) const
  {
    CND_PRECONDITION(directory[0] != 0, "directory is not open");

#if defined(_BUILD_FOR_QT_)
    QDir dir;
    dir.setPath(QLatin1String(directory));
    QStringList fileList = dir.entryList(QDir::Files | QDir::Hidden);
    foreach(const QString name, fileList) {
        const QByteArray tmpPath = name.toLocal8Bit();
        names->push_back(tmpPath.constData());
    }
#else
    DIR* dir = opendir(directory);
    
    struct dirent* fl = readdir(dir);
    struct fileStat buf;

    char path[CL_MAX_DIR];
	strncpy(path,directory,CL_MAX_DIR);
    strcat(path,PATH_DELIMITERA);
    char* pathP = path + strlen(path);

    while ( fl != NULL ){
      strcpy(pathP,fl->d_name);
      fileStat(path,&buf);
      if ( !(buf.st_mode & S_IFDIR) ) {
        names->push_back( fl->d_name );
      }
      fl = readdir(dir);
    }
    closedir(dir);
#endif
  }

  bool FSDirectory::fileExists(const char* name) const {
	  CND_PRECONDITION(directory[0]!=0,"directory is not open");
    char fl[CL_MAX_DIR];
    priv_getFN(fl, name);
    return Misc::dir_Exists( fl );
  }

  const char* FSDirectory::getDirName() const{
    return directory;
  }

  //static
  FSDirectory* FSDirectory::getDirectory(const char* file, const bool _create){
    FSDirectory* dir = NULL;
	{
		if ( !file || !*file )
			_CLTHROWA(CL_ERR_IO,"Invalid directory");

		SCOPED_LOCK_MUTEX(DIRECTORIES.THIS_LOCK)
		dir = DIRECTORIES.get(file);
		if ( dir == NULL  ){
			dir = _CLNEW FSDirectory(file,_create);
			DIRECTORIES.put( dir->directory, dir);
		}else if ( _create ){
	    	dir->create();
		}
	}

	{
		SCOPED_LOCK_MUTEX(dir->THIS_LOCK)
		dir->refCount++;
	}

    return _CL_POINTER(dir);
  }

  int64_t FSDirectory::fileModified(const char* name) const {
	CND_PRECONDITION(directory[0]!=0,"directory is not open");
    struct fileStat buf;
    char buffer[CL_MAX_DIR];
    priv_getFN(buffer,name);
    if (fileStat( buffer, &buf ) == -1 )
      return 0;
    else
      return buf.st_mtime;
  }

  //static
  int64_t FSDirectory::fileModified(const char* dir, const char* name){
    struct fileStat buf;
    char buffer[CL_MAX_DIR];
	_snprintf(buffer,CL_MAX_DIR,"%s%s%s",dir,PATH_DELIMITERA,name);
    fileStat( buffer, &buf );
    return buf.st_mtime;
  }

  void FSDirectory::touchFile(const char* name){
	  CND_PRECONDITION(directory[0]!=0,"directory is not open");
    char buffer[CL_MAX_DIR];
    _snprintf(buffer,CL_MAX_DIR,"%s%s%s",directory,PATH_DELIMITERA,name);
	
    int32_t r = _open(buffer, O_RDWR, _S_IWRITE);
	if ( r < 0 )
		_CLTHROWA(CL_ERR_IO,"IO Error while touching file");
	_close(r);
  }

  int64_t FSDirectory::fileLength(const char* name) const {
	  CND_PRECONDITION(directory[0]!=0,"directory is not open");
    struct fileStat buf;
    char buffer[CL_MAX_DIR];
    priv_getFN(buffer,name);
    if ( fileStat( buffer, &buf ) == -1 )
      return 0;
    else
      return buf.st_size;
  }

  IndexInput* FSDirectory::openInput(const char* name ) {
  	return openInput(name, CL_NS(store)::BufferedIndexOutput::BUFFER_SIZE);
  }
  
#ifdef LUCENE_FS_MMAP
  IndexInput* FSDirectory::openMMapFile(const char* name, int32_t bufferSize){
    char fl[CL_MAX_DIR];
    priv_getFN(fl, name);
	if ( Misc::file_Size(fl) < LUCENE_INT32_MAX_SHOULDBE ) //todo: would this be bigger on 64bit systems?. i suppose it would be...test first
		return _CLNEW MMapIndexInput( fl );
	else
		return _CLNEW FSIndexInput( fl, bufferSize );
  }
#endif

  IndexInput* FSDirectory::openInput(const char* name, int32_t bufferSize ){
	CND_PRECONDITION(directory[0]!=0,"directory is not open")
    char fl[CL_MAX_DIR];
    priv_getFN(fl, name);
#ifdef LUCENE_FS_MMAP
	//todo: do some tests here... like if the file
	//is >2gb, then some system cannot mmap the file
	//also some file systems mmap will fail?? could detect here too
	if ( useMMap && Misc::file_Size(fl) < LUCENE_INT32_MAX_SHOULDBE ) //todo: would this be bigger on 64bit systems?. i suppose it would be...test first
		return _CLNEW MMapIndexInput( fl );
	else
#endif
	return _CLNEW FSIndexInput( fl, bufferSize );
  }
		
  void FSDirectory::close(){
    SCOPED_LOCK_MUTEX(DIRECTORIES.THIS_LOCK)
    {
	    SCOPED_LOCK_MUTEX(THIS_LOCK)
	
	    CND_PRECONDITION(directory[0]!=0,"directory is not open");
	
	    if (--refCount <= 0 ) {//refcount starts at 1
	        Directory* dir = DIRECTORIES.get(getDirName());
	        if(dir){
	            DIRECTORIES.remove( getDirName() ); //this will be removed in ~FSDirectory
	            _CLDECDELETE(dir);
	        }
	    }
	}
   }

   /**
   * So we can do some byte-to-hexchar conversion below
   */
	char HEX_DIGITS[] =
	{'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};

	char* FSDirectory::getLockPrefix() const{
		char dirName[CL_MAX_PATH]; // name to be hashed
		if ( _realpath(directory,dirName) == NULL ){
			_CLTHROWA(CL_ERR_Runtime,"Invalid directory path");
		}

		//to make a compatible name with jlucene, we need to make some changes...
		if ( dirName[1] == ':' )
			dirName[0] = (char)_totupper((char)dirName[0]);

        QByteArray b = QCryptographicHash::hash(dirName, QCryptographicHash::Md5);	
		char* ret=_CL_NEWARRAY(char,32+7+1); //32=2*16, 7=strlen("lucene-")
		strcpy(ret, "lucene-");
		strcat(ret, b.toHex().constData());
	    return ret; 
  }

  bool FSDirectory::doDeleteFile(const char* name)  {
	CND_PRECONDITION(directory[0]!=0,"directory is not open");
    char fl[CL_MAX_DIR];
    priv_getFN(fl, name);
	return _unlink(fl) != -1;
  }
  
  void FSDirectory::renameFile(const char* from, const char* to){
	CND_PRECONDITION(directory[0]!=0,"directory is not open");
    SCOPED_LOCK_MUTEX(THIS_LOCK)
    char old[CL_MAX_DIR];
    priv_getFN(old, from);

    char nu[CL_MAX_DIR];
    priv_getFN(nu, to);

    /* This is not atomic.  If the program crashes between the call to
    delete() and the call to renameTo() then we're screwed, but I've
    been unable to figure out how else to do this... */

    if ( Misc::dir_Exists(nu) )
      if( _unlink(nu) != 0 ){
	    char* err = _CL_NEWARRAY(char,16+strlen(to)+1); //16: len of "couldn't delete "
		strcpy(err,"couldn't delete ");
		strcat(err,to);
        _CLTHROWA_DEL(CL_ERR_IO, err );
      }
    if ( _rename(old,nu) != 0 ){
       //todo: jlucene has some extra rename code - if the rename fails, it copies
       //the whole file to the new file... might want to implement that if renaming
       //fails on some platforms
        char buffer[20+CL_MAX_PATH+CL_MAX_PATH];
        strcpy(buffer,"couldn't rename ");
        strcat(buffer,from);
        strcat(buffer," to ");
        strcat(buffer,nu);
      _CLTHROWA(CL_ERR_IO, buffer );
    }
  }

  IndexOutput* FSDirectory::createOutput(const char* name) {
	CND_PRECONDITION(directory[0]!=0,"directory is not open");
    char fl[CL_MAX_DIR];
    priv_getFN(fl, name);
	if ( Misc::dir_Exists(fl) ){
		if ( _unlink(fl) != 0 ){
			char tmp[1024];
			strcpy(tmp, "Cannot overwrite: ");
			strcat(tmp, name);
			_CLTHROWA(CL_ERR_IO, tmp);
		}
	}
    return _CLNEW FSIndexOutput( fl );
  }

  LuceneLock* FSDirectory::makeLock(const char* name) {
	CND_PRECONDITION(directory[0]!=0,"directory is not open");

	char* tmp = getLockPrefix();
	char* lockFile = _CL_NEWARRAY(char,strlen(tmp)+strlen(name)+2);
	strcpy(lockFile,tmp);
	strcat(lockFile,"-");
	strcat(lockFile,name);
	_CLDELETE_CaARRAY(tmp);

    // create a lock file
    LuceneLock* ret = _CLNEW FSLock( lockDir, lockFile );
	_CLDELETE_CaARRAY(lockFile);
	return ret;
  }



  FSDirectory::FSLock::FSLock ( const char* _lockDir, const char* name )
  {
	  this->lockDir = STRDUP_AtoA(_lockDir);
	  strcpy(lockFile,_lockDir);
	  strcat(lockFile,PATH_DELIMITERA);
	  strcat(lockFile,name);
  }
  FSDirectory::FSLock::~FSLock(){
    _CLDELETE_LCaARRAY( lockDir );
  }
  TCHAR* FSDirectory::FSLock::toString(){
	  size_t lflen = strlen(lockFile);
	  TCHAR* ret = _CL_NEWARRAY(TCHAR,lflen+6);
	  _tcscpy(ret,_T("Lock@"));
	  STRCPY_AtoT(ret+5,lockFile,lflen+1);
	  return ret;
  }
  bool FSDirectory::FSLock::obtain() {
   if (disableLocks)
      return true;

	if ( Misc::dir_Exists(lockFile) )
	  return false;

	if ( !Misc::dir_Exists(lockDir) ){
       //todo: should construct directory using _mkdirs... have to write replacement
      if ( _mkdir(lockDir) == -1 ){
		  char* err = _CL_NEWARRAY(char,34+strlen(lockDir)+1); //34: len of "Couldn't create lock directory: "
		  strcpy(err,"Couldn't create lock directory: ");
		  strcat(err,lockDir);
		  _CLTHROWA_DEL(CL_ERR_IO, err );
      }
    }
    int32_t r = _open(lockFile,  O_RDWR | O_CREAT | O_RANDOM , _S_IREAD | _S_IWRITE); //must do this or file will be created Read only
	if ( r < 0 )
	  return false;
	else{
	  _close(r);
	  return true;
	}

  }
  bool FSDirectory::FSLock::isLocked(){
     if (disableLocks)
          return false;

     return Misc::dir_Exists(lockFile);
  }
  void FSDirectory::FSLock::release() {
    if (disableLocks)
          return;
    _unlink( lockFile );
  }

  TCHAR* FSDirectory::toString() const{
	  TCHAR* ret = _CL_NEWARRAY(TCHAR, strlen(this->directory) + 13); //strlen("FSDirectory@")
	  _tcscpy(ret,_T("FSDirectory@"));
	  STRCPY_AtoT(ret+12,directory,strlen(directory)+1);

	  return ret;
  }

CL_NS_END
