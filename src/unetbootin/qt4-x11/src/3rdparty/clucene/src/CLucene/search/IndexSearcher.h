/*
 * Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
 *
 * Distributable under the terms of either the Apache License (Version 2.0) or 
 * the GNU Lesser General Public License, as specified in the COPYING file.
 *
 * Changes are Copyright(C) 2007, 2008 by Nokia Corporation and/or its subsidiary(-ies), all rights reserved.
*/
#ifndef _lucene_search_IndexSearcher_
#define _lucene_search_IndexSearcher_

#if defined(_LUCENE_PRAGMA_ONCE)
#   pragma once
#endif

#include <QtCore/QString>

#include "SearchHeader.h"
#include "CLucene/store/Directory.h"
#include "CLucene/document/Document.h"
#include "CLucene/index/IndexReader.h"
#include "CLucene/index/Term.h"
#include "CLucene/util/BitSet.h"
#include "HitQueue.h"
#include "FieldSortedHitQueue.h"

CL_NS_DEF(search)
/** Implements search over a single IndexReader.
*
* <p>Applications usually need only call the inherited {@link search(Query*)}
* or {@link search(Query*,Filter*)} methods.
*/
class IndexSearcher:public Searcher{
	CL_NS(index)::IndexReader* reader;
	bool readerOwner;

public:
	/// Creates a searcher searching the index in the named directory.
	IndexSearcher(const QString& path);
  
    /// Creates a searcher searching the index in the specified directory.
    IndexSearcher(CL_NS(store)::Directory* directory);

	/// Creates a searcher searching the provided index. 
	IndexSearcher(CL_NS(index)::IndexReader* r);

	~IndexSearcher();
	    
	/// Frees resources associated with this Searcher. 
	void close();

	int32_t docFreq(const CL_NS(index)::Term* term) const;

	bool doc(int32_t i, CL_NS(document)::Document* document);

	int32_t maxDoc() const;

	TopDocs* _search(Query* query, Filter* filter, const int32_t nDocs);
	TopFieldDocs* _search(Query* query, Filter* filter, const int32_t nDocs,
        const Sort* sort);

	void _search(Query* query, Filter* filter, HitCollector* results);

	CL_NS(index)::IndexReader* getReader() {
		return reader; }

	Query* rewrite(Query* original);
	void explain(Query* query, int32_t doc, Explanation* ret);
};

CL_NS_END

#endif
