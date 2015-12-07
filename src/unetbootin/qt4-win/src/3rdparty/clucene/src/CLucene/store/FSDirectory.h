/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
*
* Changes are Copyright(C) 2007 by Trolltech ASA, all rights reserved.
------------------------------------------------------------------------------*/
#ifndef _lucene_store_FSDirectory_
#define _lucene_store_FSDirectory_

#if defined(_LUCENE_PRAGMA_ONCE)
# pragma once
#endif

#include "Directory.h"
#include "Lock.h"
#include "CLucene/util/VoidMap.h"
#include "CLucene/util/StringBuffer.h"

   CL_NS_DEF(store)

   /**
   * Straightforward implementation of {@link Directory} as a directory of files.
   * <p>If the system property 'disableLuceneLocks' has the String value of
   * "true", lock creation will be disabled.
   *
   * @see Directory
   */
	class FSDirectory:public Directory{
	private:
		class FSLock: public LuceneLock{
		public:
			// const char* fname;
			char lockFile[CL_MAX_PATH];
			char* lockDir;
			FSLock ( const char* lockDir, const char* name );
			~FSLock();
			bool obtain();
			void release();
			bool isLocked();
			TCHAR* toString();
		};
		friend class FSDirectory::FSLock;

#if defined(LUCENE_FS_MMAP)
		class MMapIndexInput: public IndexInput{
			uint8_t* data;
			int64_t pos;
#ifdef _CLCOMPILER_MSVC
			HANDLE mmaphandle;
			HANDLE fhandle;
#else
			int fhandle;
#endif
    		bool isClone;
			int64_t _length;

			MMapIndexInput(const MMapIndexInput& clone);
		public:
    		MMapIndexInput(const char* path);
			~MMapIndexInput();
			IndexInput* clone() const;

			inline uint8_t readByte();
			int32_t readVInt();
			void readBytes(uint8_t* b, const int32_t len);
			void close();
			int64_t getFilePointer() const;
			void seek(const int64_t pos);
			int64_t length(){ return _length; }
			
			static const char* DirectoryType(){ return "MMAP"; }
			const char* getDirectoryType() const{ return DirectoryType(); }
		};
#endif

    	class FSIndexInput:public BufferedIndexInput {
			/**
			* We used a shared handle between all the fsindexinput clones.
			* This reduces number of file handles we need, and it means
			* we dont have to use file tell (which is slow) before doing
			* a read.
			*/
			class SharedHandle: LUCENE_REFBASE{
			public:
    			int32_t fhandle;
				int64_t _length;
				int64_t _fpos;
    			DEFINE_MUTEX(THIS_LOCK)
    			char path[CL_MAX_DIR]; //todo: this is only used for cloning, better to get information from the fhandle
    			SharedHandle();
				~SharedHandle() throw(CLuceneError&);
			};
			SharedHandle* handle;
			int64_t _pos;
    	protected:
    		FSIndexInput(const FSIndexInput& clone);
    	public:
			FSIndexInput(const char* path, int32_t bufferSize=CL_NS(store)::BufferedIndexOutput::BUFFER_SIZE);
    		~FSIndexInput();
    
    		IndexInput* clone() const;
    		void close();
			int64_t length(){ return handle->_length; }
			
			const char* getDirectoryType() const{ return FSDirectory::DirectoryType(); }
    	protected:
    		// Random-access methods 
    		void seekInternal(const int64_t position);
    		// IndexInput methods 
    		void readInternal(uint8_t* b, const int32_t len);
    	};
		friend class FSDirectory::FSIndexInput;

    	class FSIndexOutput: public BufferedIndexOutput {
    	private:
    		int32_t fhandle;
    	protected:
    		// output methods: 
    		void flushBuffer(const uint8_t* b, const int32_t size);
    	public:
    		FSIndexOutput(const char* path);
    		~FSIndexOutput();

    		// output methods:
    		void close();
    
    		// Random-access methods 
    		void seek(const int64_t pos);
    		int64_t length();
		};
		friend class FSDirectory::FSIndexOutput;

	protected:
		FSDirectory(const char* path, const bool createDir);
	private:
		char directory[CL_MAX_PATH];
		int refCount;
		void create();
		
		static const char* LOCK_DIR;
		static const char* getLockDir();
		char lockDir[CL_MAX_PATH];
		char* getLockPrefix() const;
		static bool disableLocks;

		void priv_getFN(char* buffer, const char* name) const;
		bool useMMap;

	protected:
		/// Removes an existing file in the directory. 
		bool doDeleteFile(const char* name);

	public:
	  ///Destructor - only call this if you are sure the directory
	  ///is not being used anymore. Otherwise use the ref-counting
	  ///facilities of _CLDECDELETE
		~FSDirectory();

		/// Get a list of strings, one for each file in the directory. 
		void fileList(vector<string>* names) const;

		/// Returns true iff a file with the given name exists. 
		bool fileExists(const char* name) const;

      /// Returns the text name of the directory
		const char* getDirName() const; ///<returns reference
		

		/**
         Returns the directory instance for the named location.
		   
         Do not delete this instance, only use close, otherwise other instances
         will lose this instance.

		   <p>Directories are cached, so that, for a given canonical path, the same
		   FSDirectory instance will always be returned.  This permits
		   synchronization on directories.
		   
		   @param file the path to the directory.
		   @param create if true, create, or erase any existing contents.
		   @return the FSDirectory for the named file.
        */
		static FSDirectory* getDirectory(const char* file, const bool create);

		/// Returns the time the named file was last modified.
		int64_t fileModified(const char* name) const;
		       
		//static
		/// Returns the time the named file was last modified.
		static int64_t fileModified(const char* dir, const char* name);

		//static
		/// Returns the length in bytes of a file in the directory. 
		int64_t fileLength(const char* name) const;

		/// Returns a stream reading an existing file. 
		IndexInput* openInput(const char* name);
		IndexInput* openInput(const char* name, int32_t bufferSize);

#ifdef LUCENE_FS_MMAP
		IndexInput* openMMapFile(const char* name, int32_t bufferSize=LUCENE_STREAM_BUFFER_SIZE);
#endif

		/// Renames an existing file in the directory. 
		void renameFile(const char* from, const char* to);

      	/** Set the modified time of an existing file to now. */
      	void touchFile(const char* name);

		/// Creates a new, empty file in the directory with the given name.
		///	Returns a stream writing this file. 
		IndexOutput* createOutput(const char* name);

		/// Construct a {@link Lock}.
		/// @param name the name of the lock file
		LuceneLock* makeLock(const char* name);

		  ///Decrease the ref-count to the directory by one. If
		  ///the object is no longer needed, then the object is
		  ///removed from the directory pool. 
      void close();

	  /** 
	  * If MMap is available, this can disable use of
	  * mmap reading.
	  */
	  void setUseMMap(bool value){ useMMap = value; }
	  /** 
	  * Gets whether the directory is using MMap for inputstreams.
	  */
	  bool getUseMMap() const{ return useMMap; }

	  TCHAR* toString() const;

	  static const char* DirectoryType(){ return "FS"; }
	  const char* getDirectoryType() const{ return "FS"; }
	
	/**
	* Set whether Lucene's use of lock files is disabled. By default, 
	* lock files are enabled. They should only be disabled if the index
	* is on a read-only medium like a CD-ROM.
	*/
	static void setDisableLocks(bool doDisableLocks) { disableLocks = doDisableLocks; }
	
	/**
	* Returns whether Lucene's use of lock files is disabled.
	* @return true if locks are disabled, false if locks are enabled.
	*/
	static bool getDisableLocks() { return disableLocks; }

  };

CL_NS_END
#endif
