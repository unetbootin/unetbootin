/*
 * Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
 *
 * Distributable under the terms of either the Apache License (Version 2.0) or 
 * the GNU Lesser General Public License, as specified in the COPYING file.
 *
 * Changes are Copyright(C) 2007, 2008 by Nokia Corporation and/or its subsidiary(-ies), all rights reserved.
*/
#ifndef _lucene_store_TransactionalRAMDirectory_
#define _lucene_store_TransactionalRAMDirectory_

#if defined(_LUCENE_PRAGMA_ONCE)
#   pragma once
#endif

#include <QtCore/QString>

#include "RAMDirectory.h"
#include "CLucene/util/VoidList.h"

CL_NS_DEF(store)

/***
This transactional in-memory Directory was created to address a specific
situation, and was deliberately pared down to the simplest viable
implementation.  For the sake of simplicity, this implementation imposes
restrictions on what operations can be performed in the directory while a
transaction is in progress (documented in TransactionalRAMDirectory.cpp).

Because the Lucene Directory interface itself is rather simplistic, it
would not be difficult to expand TransactionalRAMDirectory so that it
provided fully general transactionality.  However, the developer of this
original implementation was of the opinion that the last thing CLucene
needs is gratuitous features that exceed their required complexity and
haven't been rigorously tested.
*/
class TransactionalRAMDirectory : public RAMDirectory
{
private:
    typedef CL_NS(util)::CLSet<QString, void*, CL_NS(util)::Compare::Qstring,
        CL_NS(util)::Deletor::DummyQString> FilenameSet;
    FilenameSet filesToRemoveOnAbort;

    typedef CL_NS(util)::CLSet<QString, RAMFile*, CL_NS(util)::Compare::Qstring,
        CL_NS(util)::Deletor::DummyQString,
        CL_NS(util)::Deletor::Object<RAMFile> > TransFileMap;
    TransFileMap filesToRestoreOnAbort;

    bool transOpen;

    void transResolved();
    bool archiveOrigFileIfNecessary(const QString& name);
    void unarchiveOrigFile(const QString& name);

protected:
    bool doDeleteFile(const QString& name);

public:
    TransactionalRAMDirectory();
    virtual ~TransactionalRAMDirectory();

    bool transIsOpen() const;
    void transStart();
    void transCommit();
    void transAbort();

    // Constrained operations:
    void renameFile(const QString& from, const QString& to);
    IndexOutput* createOutput(const QString& name);

    void close();
};

CL_NS_END

#endif
