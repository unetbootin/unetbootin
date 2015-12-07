/*
 * Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
 *
 * Distributable under the terms of either the Apache License (Version 2.0) or 
 * the GNU Lesser General Public License, as specified in the COPYING file.
 *
 * Changes are Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
*/
#ifndef _lucene_store_RAMDirectory_
#define _lucene_store_RAMDirectory_

#if defined(_LUCENE_PRAGMA_ONCE)
#   pragma once
#endif

#include <QtCore/QString>
#include <QtCore/QStringList>

#include "Lock.h"
#include "Directory.h"
#include "CLucene/util/VoidMap.h"
#include "CLucene/util/Arrays.h"

CL_NS_DEF(store)

class RAMFile : LUCENE_BASE
{
public:
    CL_NS(util)::CLVector<uint8_t*, CL_NS(util)::Deletor::Array<uint8_t> > buffers;
    int64_t length;
    uint64_t lastModified;

#ifdef _DEBUG
    QString filename;
#endif

    RAMFile();
    ~RAMFile();
};

class RAMIndexOutput : public BufferedIndexOutput
{
protected:
    RAMFile* file;
    int32_t pointer;
    bool deleteFile;

    // output methods: 
    void flushBuffer(const uint8_t* src, const int32_t len);

public:
    RAMIndexOutput(RAMFile* f);
    RAMIndexOutput();
    /** Construct an empty output buffer. */
    virtual ~RAMIndexOutput();

    virtual void close();

    // Random-at methods
    virtual void seek(const int64_t pos);
    int64_t length();
    /** Resets this to an empty buffer. */
    void reset();
    /** Copy the current contents of this buffer to the named output. */
    void writeTo(IndexOutput* output);
};

class RAMIndexInput : public BufferedIndexInput
{
private:
    RAMFile* file;
    int32_t pointer;
    int64_t _length;

protected:
    /** IndexInput methods */
    RAMIndexInput(const RAMIndexInput& clone);
    void readInternal(uint8_t *dest, const int32_t len);

    /** Random-at methods */
    void seekInternal(const int64_t pos);

public:
    RAMIndexInput(RAMFile* f);
    ~RAMIndexInput();
    IndexInput* clone() const;

    void close();
    int64_t length();
    QString getDirectoryType() const;
};


/**
* A memory-resident {@link Directory} implementation.
*/
class RAMDirectory : public Directory
{
    class RAMLock : public LuceneLock
    {
    private:
        RAMDirectory* directory;
        QString fname;
    public:
        RAMLock(const QString& name, RAMDirectory* dir);
        virtual ~RAMLock();
        bool obtain();
        void release();
        bool isLocked();
        virtual QString toString() const;
    };

    typedef CL_NS(util)::CLHashMap<QString, RAMFile*,
        CL_NS(util)::Compare::Qstring, CL_NS(util)::Equals::Qstring,
        CL_NS(util)::Deletor::DummyQString,
        CL_NS(util)::Deletor::Object<RAMFile> > FileMap;

protected:
    /// Removes an existing file in the directory. 
    virtual bool doDeleteFile(const QString& name);

    /**
    * Creates a new <code>RAMDirectory</code> instance from a different
    * <code>Directory</code> implementation.  This can be used to load
    * a disk-based index into memory.
    * <P>
    * This should be used only with indices that can fit into memory.
    *
    * @param dir a <code>Directory</code> value
    * @exception IOException if an error occurs
    */
    void _copyFromDir(Directory* dir, bool closeDir);
    FileMap files; // unlike the java Hashtable, FileMap is not synchronized, and all access must be protected by a lock

public:
#ifndef _CL_DISABLE_MULTITHREADING //do this so that the mutable keyword still works without mt enabled
    mutable DEFINE_MUTEX(files_mutex) // mutable: const methods must also be able to synchronize properly
#endif

    // Returns a null terminated array of strings, one for each file in the directory. 
    QStringList list() const;

    /** Constructs an empty {@link Directory}. */
    RAMDirectory();

    // Destructor - only call this if you are sure the directory
    // is not being used anymore. Otherwise use the ref-counting
    // facilities of dir->close
    virtual ~RAMDirectory();
    RAMDirectory(Directory* dir);

    /**
    * Creates a new <code>RAMDirectory</code> instance from the {@link FSDirectory}.
    *
    * @param dir a <code>String</code> specifying the full index directory path
    */
    RAMDirectory(const QString& dir);

    /// Returns true iff the named file exists in this directory. 
    bool fileExists(const QString& name) const;

    /// Returns the time the named file was last modified. 
    int64_t fileModified(const QString& name) const;

    /// Returns the length in bytes of a file in the directory. 
    int64_t fileLength(const QString& name) const;

    /// Removes an existing file in the directory. 
    virtual void renameFile(const QString& from, const QString& to);

    /** Set the modified time of an existing file to now. */
    void touchFile(const QString& name);

    /// Creates a new, empty file in the directory with the given name.
    ///	Returns a stream writing this file. 
    virtual IndexOutput* createOutput(const QString& name);

    /// Construct a {@link Lock}.
    /// @param name the name of the lock file
    LuceneLock* makeLock(const QString& name);

    /// Returns a stream reading an existing file. 
    IndexInput* openInput(const QString& name);

    virtual void close();

    QString toString() const;

    static QString DirectoryType() { return QLatin1String("RAM"); }
    QString getDirectoryType() const { return DirectoryType(); }
};

CL_NS_END

#endif
