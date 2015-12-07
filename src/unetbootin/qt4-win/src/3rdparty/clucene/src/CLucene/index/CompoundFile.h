/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
*
* Changes are Copyright(C) 2007 by Trolltech ASA, all rights reserved.
------------------------------------------------------------------------------*/
#ifndef _lucene_index_compoundfile_h
#define _lucene_index_compoundfile_h

#if defined(_LUCENE_PRAGMA_ONCE)
# pragma once
#endif

#include "CLucene/store/IndexInput.h"
#include "CLucene/store/IndexOutput.h"
#include "CLucene/store/Directory.h"
#include "CLucene/store/Lock.h"
#include "CLucene/util/VoidList.h"
#include "CLucene/util/VoidMap.h"


CL_NS_DEF(index)


/**
 * Class for accessing a compound stream.
 * This class implements a directory, but is limited to only read operations.
 * Directory methods that would normally modify data throw an exception.
 *
 */
class CompoundFileReader: public CL_NS(store)::Directory {
private:
	/** Implementation of an IndexInput that reads from a portion of the
     *  compound file. The visibility is left as "package" *only* because
     *  this helps with testing since JUnit test cases in a different class
     *  can then access package fields of this class.
     */
    class CSIndexInput:public CL_NS(store)::BufferedIndexInput {
	private:
		CL_NS(store)::IndexInput* base;
		int64_t fileOffset;
		int64_t _length;
	protected:
		/** Expert: implements buffer refill.  Reads uint8_ts from the current
		*  position in the input.
		* @param b the array to read uint8_ts into
		* @param length the number of uint8_ts to read
		*/
		void readInternal(uint8_t* b, const int32_t len);
		void seekInternal(const int64_t pos)
		{
		}

	public:
		CSIndexInput(CL_NS(store)::IndexInput* base, const int64_t fileOffset, const int64_t length);
		CSIndexInput(const CSIndexInput& clone);
		~CSIndexInput();

		/** Closes the stream to futher operations. */
		void close();
		CL_NS(store)::IndexInput* clone() const;

		int64_t length(){ return _length; }
		
		const char* getDirectoryType() const{ return CompoundFileReader::DirectoryType(); }
	};

	class FileEntry:LUCENE_BASE {
	public:
		int64_t offset;
		int64_t length;
		FileEntry(){
			offset=0;
			length=0;
		}
		~FileEntry(){
		}
	};

	// Base info
	CL_NS(store)::Directory* directory;
	char fileName[CL_MAX_PATH];

	CL_NS(store)::IndexInput* stream;

    typedef CL_NS(util)::CLHashMap<const char*,FileEntry*,
		CL_NS(util)::Compare::Char,
		CL_NS(util)::Equals::Char,
		CL_NS(util)::Deletor::acArray,
		CL_NS(util)::Deletor::Object<FileEntry> > EntriesType;
	EntriesType entries;
protected:
	/** Removes an existing file in the directory-> */
	bool doDeleteFile(const char* name);

public:
	CompoundFileReader(CL_NS(store)::Directory* dir, char* name);
	~CompoundFileReader();
	CL_NS(store)::Directory* getDirectory();
	const char* getName() const;

	void close();
	CL_NS(store)::IndexInput* openInput(const char* id);

	/** Returns an array of strings, one for each file in the directory-> */
	void fileList(vector<string>* names) const;
	/** Returns true iff a file with the given name exists. */
	bool fileExists(const char* name) const;
	/** Returns the time the named file was last modified. */
	int64_t fileModified(const char* name) const;
	/** Set the modified time of an existing file to now. */
	void touchFile(const char* name);
	/** Renames an existing file in the directory->
	If a file already exists with the new name, then it is replaced.
	This replacement should be atomic. */
	void renameFile(const char* from, const char* to);
	/** Returns the length of a file in the directory.
     * @throws IOException if the file does not exist */
    int64_t fileLength(const char* name) const;
	/** Not implemented
     * @throws UnsupportedOperationException */
    CL_NS(store)::IndexOutput* createOutput(const char* name);
	/** Not implemented
     * @throws UnsupportedOperationException */
	CL_NS(store)::LuceneLock* makeLock(const char* name);

	TCHAR* toString() const;
	
	static const char* DirectoryType(){ return "CFS"; }
	const char* getDirectoryType() const{ return DirectoryType(); }
};


/**
 * Combines multiple files into a single compound file.
 * The file format:<br>
 * <ul>
 *     <li>VInt fileCount</li>
 *     <li>{Directory}
 *         fileCount entries with the following structure:</li>
 *         <ul>
 *             <li>int64_t dataOffset</li>
 *             <li>UTFString extension</li>
 *         </ul>
 *     <li>{File Data}
 *         fileCount entries with the raw data of the corresponding file</li>
 * </ul>
 *
 * The fileCount integer indicates how many files are contained in this compound
 * file. The {directory} that follows has that many entries. Each directory entry
 * contains an encoding identifier, an int64_t pointer to the start of this file's
 * data section, and a UTF String with that file's extension.
 *
 */
class CompoundFileWriter:LUCENE_BASE {
	class WriterFileEntry:LUCENE_BASE {
	public:
		WriterFileEntry(){
			directoryOffset=0;
			dataOffset=0;
		}
		~WriterFileEntry(){
		}
		/** source file */
		char file[CL_MAX_PATH];

		/** temporary holder for the start of directory entry for this file */
		int64_t directoryOffset;

		/** temporary holder for the start of this file's data section */
		int64_t dataOffset;

	};

	CL_NS(store)::Directory* directory;
	char fileName[CL_MAX_PATH];
	CL_NS(util)::CLHashSet<const char*,
		CL_NS(util)::Compare::Char,CL_NS(util)::Deletor::acArray> ids;
	CL_NS(util)::CLLinkedList<WriterFileEntry*,
		CL_NS(util)::Deletor::Object<WriterFileEntry> > entries;
	bool merged;
	/** Copy the contents of the file with specified extension into the
	*  provided output stream. Use the provided buffer for moving data
	*  to reduce memory allocation.
	*/
	void copyFile(WriterFileEntry* source, CL_NS(store)::IndexOutput* os, uint8_t* buffer, int32_t bufferLength);
public:
	/** Create the compound stream in the specified file. The file name is the
	*  entire name (no extensions are added).
	*/
	CompoundFileWriter(CL_NS(store)::Directory* dir, const char* name);
	~CompoundFileWriter();
	/** Returns the directory of the compound file. */
	CL_NS(store)::Directory* getDirectory();
	const char* getName() const ;
	/** Add a source stream. <code>file</code> is the string by which the 
	*  sub-stream will be known in the compound stream.
	* 
	*  @throws IllegalStateException if this writer is closed
	*  @throws NullPointerException if <code>file</code> is null
	*  @throws IllegalArgumentException if a file with the same name
	*   has been added already
	*/
	void addFile(const char* file);
	/** Merge files with the extensions added up to now.
	*  All files with these extensions are combined sequentially into the
	*  compound stream. After successful merge, the source files
    *  @throws IllegalStateException if close() had been called before or
    *   if no file has been added to this object
	*  are deleted.
	*/
	void close();
};

CL_NS_END
#endif
