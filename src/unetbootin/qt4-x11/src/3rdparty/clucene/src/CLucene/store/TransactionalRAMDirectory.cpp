/*
 * Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
 *
 * Distributable under the terms of either the Apache License (Version 2.0) or 
 * the GNU Lesser General Public License, as specified in the COPYING file.
 *
 * Changes are Copyright(C) 2007, 2008 by Nokia Corporation and/or its subsidiary(-ies), all rights reserved.
*/
#include "CLucene/StdHeader.h"
#include "TransactionalRAMDirectory.h"

CL_NS_DEF(store)
CL_NS_USE(util)

TransactionalRAMDirectory::TransactionalRAMDirectory()
    : RAMDirectory()
    , filesToRestoreOnAbort(false, true)
{
    transOpen = false;
}

TransactionalRAMDirectory::~TransactionalRAMDirectory()
{
}

bool TransactionalRAMDirectory::archiveOrigFileIfNecessary(const QString& name)
{
    // If a file named $name was present when the transaction started and the
    // original RAMFile object has not been archived for restoration upon
    // transaction abort, then do so, and return true.
    // In any other case, return false.
    if (fileExists(name) && filesToRemoveOnAbort.find(name) == filesToRemoveOnAbort.end()) {
        // The file exists, but isn't recorded as having been created after the
        // start of the transaction, so it must've been present at the start of
        // the transaction.

        // Transfer memory ownership of both the key and the value from files to
        // filesToRestoreOnAbort.
        QString origName = files.getKey(name);
        RAMFile* origFile = files.get(name);
        files.remove(name, true, true);
        filesToRestoreOnAbort.put(origName, origFile);

        CND_CONDITION(!fileExists(name),
            "File should not exist immediately after archival.");
        return true;
    }

    return false;
}

void TransactionalRAMDirectory::unarchiveOrigFile(const QString& name)
{
    QString origName = filesToRestoreOnAbort.getKey(name);
    if (origName.isEmpty()) {
        _CLTHROWA(CL_ERR_RAMTransaction,
            "File submitted for unarchival was not archived.");
    }
    RAMFile* origFile = filesToRestoreOnAbort.get(name);
    // Transfer memory ownership back to files from filesToRestoreOnAbort.
    filesToRestoreOnAbort.remove(name, true, true);
    files.put(origName, origFile);
}

bool TransactionalRAMDirectory::transIsOpen() const
{
    return transOpen;
}

void TransactionalRAMDirectory::transStart()
{
    if (transOpen) {
        _CLTHROWA(CL_ERR_RAMTransaction,
            "Must resolve previous transaction before starting another.");
    }

    CND_CONDITION(filesToRemoveOnAbort.size() == 0,
        "filesToRemoveOnAbort should have been cleared by either its"
        " constructor or transResolved.");

    CND_CONDITION(filesToRestoreOnAbort.size() == 0,
        "filesToRestoreOnAbort should have been cleared by either its"
        " constructor or transResolved.");

    transOpen = true;
}

void TransactionalRAMDirectory::transResolved()
{
    // This method implements actions common to both forms of transaction
    // resolution.
    filesToRemoveOnAbort.clear();
    filesToRestoreOnAbort.clear();
    transOpen = false;
}

void TransactionalRAMDirectory::transCommit()
{
    if (!transOpen)
        _CLTHROWA(CL_ERR_RAMTransaction, "There is no open transaction.");

    // All storage is in memory, so commit is ultra-simple.
    transResolved();
}

void TransactionalRAMDirectory::transAbort()
{
    if (!transOpen)
        _CLTHROWA(CL_ERR_RAMTransaction, "There is no open transaction.");

    // Delete each file in filesToRemoveOnAbort.
    FilenameSet::const_iterator itrDel = filesToRemoveOnAbort.begin();
    for ( ; itrDel != filesToRemoveOnAbort.end(); ++itrDel) {
        size_t nameLength = itrDel->first.length();

        // Special exception: Refrain from deleting a lock's flag file, as that
        // would interfere with the operation of the lock.
        if (!(nameLength >= 5
            && itrDel->first.rightRef(5) == QLatin1String(".lock"))) {
                RAMDirectory::deleteFile(itrDel->first);
        }
    }
    // Ownership of the memory of both the key and the value never left files,
    // so there's no need for a special directive to filesToRemoveOnAbort.
    filesToRemoveOnAbort.clear();

    // Now that any new-since-trans-start files with the same names as
    // already-present-at-trans-start files are out of the way, restore each
    // file in filesToRestoreOnAbort.
    TransFileMap::const_iterator itr = filesToRestoreOnAbort.begin();
    for ( ; itr != filesToRestoreOnAbort.end(); ++itr) {
        files.put(itr->first, itr->second);
        filesToRestoreOnAbort.remove(itr->first);
    }

    CND_CONDITION(filesToRestoreOnAbort.size() == 0,
        "filesToRestoreOnAbort should be empty.");

    transResolved();
}

bool TransactionalRAMDirectory::doDeleteFile(const QString& name)
{
    if (!transOpen)
        return RAMDirectory::doDeleteFile(name);

    bool wasOriginalAndWasArchived = archiveOrigFileIfNecessary(name);
    if (!wasOriginalAndWasArchived) {
        // The file to be deleted wasn't present at transaction start, so instead
        // of archiving it, we delete it the conventional way, making sure to
        // erase its record in filesToRemoveOnAbort if it was listed there.
        filesToRemoveOnAbort.remove(name);
        return RAMDirectory::doDeleteFile(name);
    }
    return true;
}

void TransactionalRAMDirectory::renameFile(const QString& from, const QString& to)
{
    // During the review on 2005.03.18, decided not to implement transactional
    // renameFile for two reasons:
    //  a) It's not needed in the limited scenario for which
    //     TransactionalRAMDirectory was designed (IndexWriter::addDocument and
    //     subcode).
    //  b) Supporting renaming during a transaction would add considerable
    //     bookkeeping overhead, reducing the performance of the overwhelmingly
    //     typical case (commit) in order to support the rare case (abort).
    //
    // This was not a thinly disguised punt due to the complication of
    // implementing renameFile transactionally; rather, several implementations
    // were considered, but it seemed wrongheaded to degrade the performance of
    // the typical case based on the mere potential need to support renameFile
    // at some future point for the benefit of the atypical case.
    if (transOpen) {
        _CLTHROWA(CL_ERR_RAMTransaction,
            "TransactionalRAMDirectory disallows renameFile during a transaction.");
    }
    RAMDirectory::renameFile(from, to);
}

IndexOutput* TransactionalRAMDirectory::createOutput(const QString& name)
{
    if (!transOpen)
        return RAMDirectory::createOutput(name);

    bool wasOriginalAndWasArchived = archiveOrigFileIfNecessary(name);
    try {
        IndexOutput* ret = RAMDirectory::createOutput(name);
        // Importantly, we store a pointer to the filename memory managed by
        // files, rather than that passed in by the client (name).  We don't make
        // an additional copy of the filename's memory because the transactional
        // metadata container filesToRemoveOnAbort is not at risk of outliving
        // files.
        filesToRemoveOnAbort.put(files.getKey(name), NULL);
        return ret;
    } catch (...) {
        if (wasOriginalAndWasArchived) {
            unarchiveOrigFile(name);
        }
        throw;
    }
}

void TransactionalRAMDirectory::close()
{
    if (transOpen)
        transAbort();
    
    RAMDirectory::close();
}

CL_NS_END
