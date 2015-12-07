/*
 * Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
 *
 * Distributable under the terms of either the Apache License (Version 2.0) or 
 * the GNU Lesser General Public License, as specified in the COPYING file.
 *
 * Changes are Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
*/
#ifndef _lucene_index_compoundfile_h
#define _lucene_index_compoundfile_h

#if defined(_LUCENE_PRAGMA_ONCE)
#   pragma once
#endif

#include <QtCore/QString>
#include <QtCore/QStringList>

#include "CLucene/store/IndexInput.h"
#include "CLucene/store/IndexOutput.h"
#include "CLucene/store/Directory.h"
#include "CLucene/store/Lock.h"
#include "CLucene/util/VoidList.h"
#include "CLucene/util/VoidMap.h"

CL_NS_DEF(index)

// Class for accessing a compound stream.
// This class implements a directory, but is limited to only read operations.
// Directory methods that would normally modify data throw an exception.
class CompoundFileReader : public CL_NS(store)::Directory
{
private:
    /** Implementation of an IndexInput that reads from a portion of the
    *  compound file. The visibility is left as "package" *only* because
    *  this helps with testing since JUnit test cases in a different class
    *  can then access package fields of this class.
    */
    class CSIndexInput : public CL_NS(store)::BufferedIndexInput
    {
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
        void seekInternal(const int64_t pos) {}

    public:
        CSIndexInput(CL_NS(store)::IndexInput* base, const int64_t fileOffset,
            const int64_t length);
        CSIndexInput(const CSIndexInput& clone);
        ~CSIndexInput();

        /** Closes the stream to futher operations. */
        void close();
        CL_NS(store)::IndexInput* clone() const;

        int64_t length() { return _length; }

        QString getDirectoryType() const {
            return CompoundFileReader::DirectoryType(); }
    };

    class FileEntry : LUCENE_BASE
    {
    public:
        FileEntry()
            : offset(0), length(0) {}
        FileEntry(int64_t _offset)
            : offset(_offset), length(0) {}
        ~FileEntry() {}

        int64_t offset;
        int64_t length;
    };

    // Base info
    CL_NS(store)::Directory* directory;
    QString fileName;

    CL_NS(store)::IndexInput* stream;

    typedef CL_NS(util)::CLHashMap<QString, FileEntry*,
        CL_NS(util)::Compare::Qstring,
        CL_NS(util)::Equals::Qstring,
        CL_NS(util)::Deletor::DummyQString,
        CL_NS(util)::Deletor::Object<FileEntry> > EntriesType;
    EntriesType entries;
protected:
    // Removes an existing file in the directory->
    bool doDeleteFile(const QString& name);

public:
    CompoundFileReader(CL_NS(store)::Directory* dir, const QString& name);
    ~CompoundFileReader();
    CL_NS(store)::Directory* getDirectory();
    QString getName() const;

    void close();
    CL_NS(store)::IndexInput* openInput(const QString& id);

    /** Returns an array of strings, one for each file in the directory-> */
    QStringList list() const;
    /** Returns true iff a file with the given name exists. */
    bool fileExists(const QString& name) const;
    /** Returns the time the named file was last modified. */
    int64_t fileModified(const QString& name) const;
    /** Set the modified time of an existing file to now. */
    void touchFile(const QString& name);
    /** Renames an existing file in the directory->
    If a file already exists with the new name, then it is replaced.
    This replacement should be atomic. */
    void renameFile(const QString& from, const QString& to);
    /** Returns the length of a file in the directory.
    * @throws IOException if the file does not exist */
    int64_t fileLength(const QString& name) const;
    /** Not implemented
    * @throws UnsupportedOperationException */
    CL_NS(store)::IndexOutput* createOutput(const QString& name);
    /** Not implemented
    * @throws UnsupportedOperationException */
    CL_NS(store)::LuceneLock* makeLock(const QString& name);

    QString toString() const;

    static QString DirectoryType() { return QLatin1String("CFS"); }
    QString getDirectoryType() const { return DirectoryType(); }
};



// Combines multiple files into a single compound file.
// The file format:<br>
// <ul>
//    <li>VInt fileCount</li>
//    <li>{Directory}
//        fileCount entries with the following structure:</li>
//        <ul>
//            <li>int64_t dataOffset</li>
//            <li>UTFString extension</li>
//        </ul>
//    <li>{File Data}
//        fileCount entries with the raw data of the corresponding file</li>
// </ul>
// The fileCount integer indicates how many files are contained in this compound
// file. The {directory} that follows has that many entries. Each directory entry
// contains an encoding identifier, an int64_t pointer to the start of this file's
// data section, and a UTF String with that file's extension.
class CompoundFileWriter : LUCENE_BASE
{
    class WriterFileEntry : LUCENE_BASE {
    public:
        WriterFileEntry()
            : dataOffset(0), directoryOffset(0) {}
        WriterFileEntry(const QString& _file)
            : file(_file), dataOffset(0), directoryOffset(0) {}
        ~WriterFileEntry() {}

        QString file;
        // temporary holder for the start of this file's data section
        int64_t dataOffset;
        // temporary holder for the start of directory entry for this file
        int64_t directoryOffset;
    };

    bool merged;
    QString fileName;
    CL_NS(store)::Directory* directory;
    
    CL_NS(util)::CLHashSet<QString, CL_NS(util)::Compare::Qstring,
        CL_NS(util)::Deletor::DummyQString> ids;

    CL_NS(util)::CLLinkedList<WriterFileEntry*,
        CL_NS(util)::Deletor::Object<WriterFileEntry> > entries;
    
    // Copy the contents of the file with specified extension into the
    // provided output stream. Use the provided buffer for moving data
    // to reduce memory allocation.
    void copyFile(WriterFileEntry* source, CL_NS(store)::IndexOutput* os,
        uint8_t* buffer, int32_t bufferLength);

public:
    // Create the compound stream in the specified file. The file name is the
    //  entire name (no extensions are added).
    CompoundFileWriter(CL_NS(store)::Directory* dir, const QString& name);
    ~CompoundFileWriter();
    
    // Returns the directory of the compound file.
    CL_NS(store)::Directory* getDirectory();
    
    QString getName() const ;
    /* Add a source stream. <code>file</code> is the string by which the 
    *  sub-stream will be known in the compound stream.
    * 
    *  @throws IllegalStateException if this writer is closed
    *  @throws NullPointerException if <code>file</code> is null
    *  @throws IllegalArgumentException if a file with the same name
    *   has been added already
    */
    void addFile(const QString& file);
    /* Merge files with the extensions added up to now.
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
