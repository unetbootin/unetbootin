/*
 * Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
 *
 * Distributable under the terms of either the Apache License (Version 2.0) or 
 * the GNU Lesser General Public License, as specified in the COPYING file.
 *
 * Changes are Copyright(C) 2007, 2008 by Nokia Corporation and/or its subsidiary(-ies), all rights reserved.
*/
#ifndef _lucene_index_DocumentWriter_
#define _lucene_index_DocumentWriter_

#if defined(_LUCENE_PRAGMA_ONCE)
#   pragma once
#endif

#include <QtCore/QString>

#include "CLucene/analysis/AnalysisHeader.h"
#include "CLucene/document/Document.h"
#include "CLucene/store/Directory.h"
#include "FieldInfos.h"
#include "IndexWriter.h"
#include "CLucene/util/VoidMap.h"
#include "CLucene/document/Field.h"
#include "TermInfo.h"
#include "CLucene/search/Similarity.h"
#include "TermInfosWriter.h"
#include "FieldsWriter.h"
#include "Term.h"

CL_NS_DEF(index)

class DocumentWriter : LUCENE_BASE
{
public:	
    // info about a Term in a doc
    class Posting : LUCENE_BASE
    {
	public:
		Term* term;					  // the Term
		int32_t freq;					  // its frequency in doc
		Array<int32_t> positions;				  // positions it occurs at
		Array<TermVectorOffsetInfo> offsets;
		
		Posting(Term* t, const int32_t position, TermVectorOffsetInfo* offset);
		~Posting();
	};

private:
	CL_NS(analysis)::Analyzer* analyzer;
	CL_NS(store)::Directory* directory;
	FieldInfos* fieldInfos; //array
	const int32_t maxFieldLength;
	CL_NS(search)::Similarity* similarity;
	int32_t termIndexInterval;

	// Keys are Terms, values are Postings.
	// Used to buffer a document before it is written to the index.
	typedef CL_NS(util)::CLHashtable<Term*, Posting*, Term::Compare,
        Term::Equals> PostingTableType;
    PostingTableType postingTable;
	int32_t* fieldLengths; //array
	int32_t* fieldPositions; //array
	int32_t* fieldOffsets; //array
	qreal* fieldBoosts; //array

	Term* termBuffer;
public:
	/** This ctor used by test code only.
	*
	* @param directory The directory to write the document information to
	* @param analyzer The analyzer to use for the document
	* @param similarity The Similarity function
	* @param maxFieldLength The maximum number of tokens a field may have
	*/ 
	DocumentWriter(CL_NS(store)::Directory* d, CL_NS(analysis)::Analyzer* a,
        CL_NS(search)::Similarity* similarity, const int32_t maxFieldLength);
	
	DocumentWriter(CL_NS(store)::Directory* directory,
        CL_NS(analysis)::Analyzer* analyzer, IndexWriter* writer);
	~DocumentWriter();

	void addDocument(const QString& segment, CL_NS(document)::Document* doc);


private:
	// Tokenizes the fields of a document into Postings.
	void invertDocument(const CL_NS(document)::Document* doc);

	void addPosition(const TCHAR* field, const TCHAR* text,
        const int32_t position, TermVectorOffsetInfo* offset);

	void sortPostingTable(Posting**& array, int32_t& arraySize);

	static void quickSort(Posting**& postings, const int32_t lo, const int32_t hi);

	void writePostings(Posting** postings, const int32_t postingsLength,
        const QString& segment);

	void writeNorms(const QString& segment);

	void clearPostingTable();
};

CL_NS_END

#endif
