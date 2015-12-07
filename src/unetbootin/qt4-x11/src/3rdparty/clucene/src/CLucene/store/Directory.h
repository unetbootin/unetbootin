/*
 * Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
 *
 * Distributable under the terms of either the Apache License (Version 2.0) or 
 * the GNU Lesser General Public License, as specified in the COPYING file.
 *
 * Changes are Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
*/
#ifndef _lucene_store_Directory
#define _lucene_store_Directory

#if defined(_LUCENE_PRAGMA_ONCE)
# pragma once
#endif

#include <QtCore/QStringList>

#include "CLucene/store/Lock.h"
#include "CLucene/util/VoidList.h"
#include "CLucene/util/Misc.h"

#include "IndexInput.h"
#include "IndexOutput.h"

CL_NS_DEF(store)

/** A Directory is a flat list of files.  Files may be written once, when they
* are created.  Once a file is created it may only be opened for read, or
* deleted.  Random access is permitted both when reading and writing.
*
* <p> Direct i/o is not used directly, but rather all i/o is
* through this API.  This permits things such as: <ul>
* <li> implementation of RAM-based indices;
* <li> implementation indices stored in a database, via a database;
* <li> implementation of an index as a single file;
* </ul>
*
*/
class Directory : LUCENE_REFBASE
{
protected:
    Directory() {}
    // Removes an existing file in the directory. 
    virtual bool doDeleteFile(const QString& name) = 0;

public:
    DEFINE_MUTEX(THIS_LOCK)

    virtual ~Directory() {};

    // Returns an array of strings, one for each file in the directory.
    virtual QStringList list() const = 0;

    // Returns true iff a file with the given name exists. 
    virtual bool fileExists(const QString& name) const = 0;

    // Returns the time the named file was last modified. 
    virtual int64_t fileModified(const QString& name) const = 0;

    // Returns the length of a file in the directory. 
    virtual int64_t fileLength(const QString& name) const = 0;

    // Returns a stream reading an existing file. 
    virtual IndexInput* openInput(const QString& name) = 0;
    virtual IndexInput* openInput(const QString& name, int32_t bufferSize)
    { 
        // didnt overload bufferSize
        return openInput(name);
    }

    // Set the modified time of an existing file to now.
    virtual void touchFile(const QString& name) = 0;

    // Removes an existing file in the directory. 
    virtual bool deleteFile(const QString& name, const bool throwError = true) {
        bool ret = doDeleteFile(name);
        if (!ret && throwError) {
            char buffer[200];
            _snprintf(buffer, 200, "couldn't delete file %s",
                name.toLocal8Bit().constData());
            _CLTHROWA(CL_ERR_IO, buffer);
        }
        return ret;
    }

    // Renames an existing file in the directory.
    // If a file already exists with the new name, then it is replaced.
    virtual void renameFile(const QString& from, const QString& to) = 0;

    // Creates a new, empty file in the directory with the given name.
    // Returns a stream writing this file. 
    virtual IndexOutput* createOutput(const QString& name) = 0;

    // Construct a {@link Lock}.
    // @param name the name of the lock file
    virtual LuceneLock* makeLock(const QString& name) = 0;

    // Closes the store. 
    virtual void close() = 0;

    virtual QString toString() const = 0;

    virtual QString getDirectoryType() const = 0;
};

CL_NS_END

#endif
