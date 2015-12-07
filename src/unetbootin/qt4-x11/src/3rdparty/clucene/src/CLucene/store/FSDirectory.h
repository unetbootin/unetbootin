/*
 * Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
 *
 * Distributable under the terms of either the Apache License (Version 2.0) or 
 * the GNU Lesser General Public License, as specified in the COPYING file.
 *
 * Changes are Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
*/
#ifndef _lucene_store_FSDirectory_
#define _lucene_store_FSDirectory_

#if defined(_LUCENE_PRAGMA_ONCE)
#   pragma once
#endif

#include <QtCore/QFile>
#include <QtCore/QString>
#include <QtCore/QStringList>

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
class FSDirectory : public Directory
{
public:
    // Destructor - only call this if you are sure the directory
    // is not being used anymore. Otherwise use the ref-counting
    // facilities of _CLDECDELETE
    ~FSDirectory();

    // Get a list of strings, one for each file in the directory. 
    QStringList list() const;

    // Returns true iff a file with the given name exists. 
    bool fileExists(const QString& name) const;

    // Returns the text name of the directory
    QString getDirName() const; ///<returns reference

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
    static FSDirectory* getDirectory(const QString& file, const bool create);

    // Returns the time the named file was last modified.
    int64_t fileModified(const QString& name) const;

    //static
    // Returns the time the named file was last modified.
    static int64_t fileModified(const QString& dir, const QString& name);

    // static
    // Returns the length in bytes of a file in the directory. 
    int64_t fileLength(const QString& name) const;

    // Returns a stream reading an existing file. 
    IndexInput* openInput(const QString& name);
    IndexInput* openInput(const QString& name, int32_t bufferSize);

    // Renames an existing file in the directory. 
    void renameFile(const QString& from, const QString& to);

    // Set the modified time of an existing file to now.
    void touchFile(const QString& name);

    // Creates a new, empty file in the directory with the given name.
    // Returns a stream writing this file.
    IndexOutput* createOutput(const QString& name);

    // Construct a {@link Lock}.
    // @param name the name of the lock file
    LuceneLock* makeLock(const QString& name);

    // Decrease the ref-count to the directory by one. If the object is no
    // longer needed, then the object is removed from the directory pool. 
    void close();

    // If MMap is available, this can disable use of mmap reading.
    void setUseMMap(bool value) { useMMap = value; }

    // Gets whether the directory is using MMap for inputstreams.
    bool getUseMMap() const { return useMMap; }

    QString toString() const;

    static QString DirectoryType() { return QLatin1String("FS"); }
    QString getDirectoryType() const { return QLatin1String("FS"); }

    // Set whether Lucene's use of lock files is disabled. By default, 
    // lock files are enabled. They should only be disabled if the index
    // is on a read-only medium like a CD-ROM.
    static void setDisableLocks(bool doDisableLocks)
    { disableLocks = doDisableLocks; }

    // Returns whether Lucene's use of lock files is disabled.
    // @return true if locks are disabled, false if locks are enabled.
    static bool getDisableLocks() { return disableLocks; }

protected:
    FSDirectory(const QString& path, const bool createDir);
    // Removes an existing file in the directory. 
    bool doDeleteFile(const QString& name);

private:
    class FSLock : public LuceneLock {
    public:
        FSLock (const QString& lockDir, const QString& name);
        ~FSLock();

        bool obtain();
        void release();
        bool isLocked();
        QString toString() const;

        QString lockDir;
        QString lockFile;
    };
    friend class FSDirectory::FSLock;

    class FSIndexInput : public BufferedIndexInput {
    public:
        FSIndexInput(const QString& path, int32_t bufferSize =
            CL_NS(store)::BufferedIndexOutput::BUFFER_SIZE);
        ~FSIndexInput();

        void close();
        IndexInput* clone() const;
                
        int64_t length()
        { return handle->_length; }

        QString getDirectoryType() const
        { return FSDirectory::DirectoryType(); }

    protected:
        FSIndexInput(const FSIndexInput& clone);
        // Random-access methods 
        void seekInternal(const int64_t position);
        // IndexInput methods 
        void readInternal(uint8_t* b, const int32_t len);

    private:
        // We used a shared handle between all the fsindexinput clones.
        // This reduces number of file handles we need, and it means
        // we dont have to use file tell (which is slow) before doing a read.
        class SharedHandle : LUCENE_REFBASE {
        public:
            SharedHandle();
            ~SharedHandle();

            int64_t _fpos;
            int64_t _length;
            
            QFile fhandle;
            DEFINE_MUTEX(THIS_LOCK)
        };
        SharedHandle* handle;
        int64_t _pos;
    };
    friend class FSDirectory::FSIndexInput;

    class FSIndexOutput : public BufferedIndexOutput {
    public:
        FSIndexOutput(const QString& path);
        ~FSIndexOutput();

        void close();
        int64_t length();
        void seek(const int64_t pos);

    protected:
        void flushBuffer(const uint8_t* b, const int32_t size);

    private:
        QFile fhandle;
    };
    friend class FSDirectory::FSIndexOutput;

private:
    QString directory;
    int refCount;
    void create();

    QString lockDir;
    QString getLockPrefix() const;
    static bool disableLocks;

    void priv_getFN(QString& buffer, const QString& name) const;
    bool useMMap;
};

CL_NS_END

#endif
