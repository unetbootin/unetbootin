/*
 * Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
 *
 * Distributable under the terms of either the Apache License (Version 2.0) or 
 * the GNU Lesser General Public License, as specified in the COPYING file.
 *
 * Changes are Copyright(C) 2007, 2008 by Nokia Corporation and/or its subsidiary(-ies), all rights reserved.
*/
#include "CLucene/StdHeader.h"
#include "IndexModifier.h"

#include "Term.h"
#include "IndexWriter.h"
#include "IndexReader.h"

CL_NS_DEF(index)
CL_NS_USE(util)
CL_NS_USE(store)
CL_NS_USE(analysis)
CL_NS_USE(document)

IndexModifier::IndexModifier(Directory* directory, Analyzer* analyzer, bool create) {
	init(directory, analyzer, create);
}

IndexModifier::IndexModifier(const QString& dirName, Analyzer* analyzer, bool create) {
	Directory* dir = FSDirectory::getDirectory(dirName, create);
	init(dir, analyzer, create);
}

void IndexModifier::init(Directory* directory, Analyzer* analyzer, bool create) {
	indexWriter = NULL;
	indexReader = NULL;
	this->analyzer = analyzer;
	open = false;

	useCompoundFile = true;
	int32_t maxBufferedDocs = IndexWriter::DEFAULT_MAX_BUFFERED_DOCS;
	this->maxFieldLength = IndexWriter::DEFAULT_MAX_FIELD_LENGTH;
	int32_t mergeFactor = IndexWriter::DEFAULT_MERGE_FACTOR;
	
	this->directory = _CL_POINTER(directory);
	createIndexReader();
	open = true;
}

IndexModifier::~IndexModifier(){
	close();
}

void IndexModifier::assureOpen() const{
	if (!open) {
		_CLTHROWA(CL_ERR_IllegalState,"Index is closed");
	}
}

void IndexModifier::createIndexWriter() {
	if (indexWriter == NULL) {
		if (indexReader != NULL) {
			indexReader->close();
			_CLDELETE(indexReader);
		}
		indexWriter = _CLNEW IndexWriter(directory, analyzer, false);
		indexWriter->setUseCompoundFile(useCompoundFile);
		//indexWriter->setMaxBufferedDocs(maxBufferedDocs);
		indexWriter->setMaxFieldLength(maxFieldLength);
		//indexWriter->setMergeFactor(mergeFactor);
	}
}

void IndexModifier::createIndexReader() {
	if (indexReader == NULL) {
		if (indexWriter != NULL) {
			indexWriter->close();
			_CLDELETE(indexWriter);
		}
		indexReader = IndexReader::open(directory);
	}
}

void IndexModifier::flush() {
	SCOPED_LOCK_MUTEX(directory->THIS_LOCK)
	assureOpen();
	if (indexWriter != NULL) {
		indexWriter->close();
		_CLDELETE(indexWriter);
		createIndexWriter();
	} else {
		indexReader->close();
		_CLDELETE(indexReader);
		createIndexReader();
	}
}

void IndexModifier::addDocument(Document* doc, Analyzer* docAnalyzer) {
	SCOPED_LOCK_MUTEX(directory->THIS_LOCK) 
	assureOpen();
	createIndexWriter();
	if (docAnalyzer != NULL)
		indexWriter->addDocument(doc, docAnalyzer);
	else
		indexWriter->addDocument(doc);
}

int32_t IndexModifier::deleteDocuments(Term* term) {
	SCOPED_LOCK_MUTEX(directory->THIS_LOCK)
	assureOpen();
	createIndexReader();
	return indexReader->deleteDocuments(term);
}

void IndexModifier::deleteDocument(int32_t docNum) {
	SCOPED_LOCK_MUTEX(directory->THIS_LOCK)
	assureOpen();
	createIndexReader();
	indexReader->deleteDocument(docNum);
}

int32_t IndexModifier::docCount() {
	SCOPED_LOCK_MUTEX(directory->THIS_LOCK)
	assureOpen();
	if (indexWriter != NULL)
		return indexWriter->docCount();
	else
		return indexReader->numDocs();
}

void IndexModifier::optimize() {
	SCOPED_LOCK_MUTEX(directory->THIS_LOCK)
	assureOpen();
	createIndexWriter();
	indexWriter->optimize();
}

void IndexModifier::setUseCompoundFile(bool useCompoundFile) {
	SCOPED_LOCK_MUTEX(directory->THIS_LOCK)
	assureOpen();
	if (indexWriter != NULL) 
		indexWriter->setUseCompoundFile(useCompoundFile);
	this->useCompoundFile = useCompoundFile;
}

bool IndexModifier::getUseCompoundFile() {
	SCOPED_LOCK_MUTEX(directory->THIS_LOCK)
	assureOpen();
	createIndexWriter();
	return indexWriter->getUseCompoundFile();
}

void IndexModifier::setMaxFieldLength(int32_t maxFieldLength) {
	SCOPED_LOCK_MUTEX(directory->THIS_LOCK)
	assureOpen();
	if (indexWriter != NULL)
		indexWriter->setMaxFieldLength(maxFieldLength);
	this->maxFieldLength = maxFieldLength;
}

int32_t IndexModifier::getMaxFieldLength() {
	SCOPED_LOCK_MUTEX(directory->THIS_LOCK)
	assureOpen();
	createIndexWriter();
	return indexWriter->getMaxFieldLength();
}

void IndexModifier::setMaxBufferedDocs(int32_t maxBufferedDocs) {
	SCOPED_LOCK_MUTEX(directory->THIS_LOCK)
	assureOpen();
	if (indexWriter != NULL)
		indexWriter->setMaxBufferedDocs(maxBufferedDocs);
	this->maxBufferedDocs = maxBufferedDocs;
}

int32_t IndexModifier::getMaxBufferedDocs() {
	SCOPED_LOCK_MUTEX(directory->THIS_LOCK)
	assureOpen();
	createIndexWriter();
	return indexWriter->getMaxBufferedDocs();
}
void IndexModifier::setMergeFactor(int32_t mergeFactor) {
	SCOPED_LOCK_MUTEX(directory->THIS_LOCK)
	assureOpen();
	if (indexWriter != NULL)
		indexWriter->setMergeFactor(mergeFactor);
	this->mergeFactor = mergeFactor;
}

int32_t IndexModifier::getMergeFactor() {
	SCOPED_LOCK_MUTEX(directory->THIS_LOCK)
	assureOpen();
	createIndexWriter();
	return indexWriter->getMergeFactor();
}

void IndexModifier::close() {
	SCOPED_LOCK_MUTEX(directory->THIS_LOCK)
	if (indexWriter != NULL) {
		indexWriter->close();
		_CLDELETE(indexWriter);
	} else {
		indexReader->close();
		_CLDELETE(indexReader);
	}
	_CLDECDELETE(directory)
	open = false;
}

QString IndexModifier::toString() const
{
	QString ret(QLatin1String("Index@"));
	return ret.append(directory->toString());
}



int64_t IndexModifier::getCurrentVersion() const{
	SCOPED_LOCK_MUTEX(directory->THIS_LOCK)
	return IndexReader::getCurrentVersion(directory);
}
	
TermDocs* IndexModifier::termDocs(Term* term){
	SCOPED_LOCK_MUTEX(directory->THIS_LOCK)
	assureOpen();
	createIndexReader();
	return indexReader->termDocs(term);
}

TermEnum* IndexModifier::terms(Term* term){
	SCOPED_LOCK_MUTEX(directory->THIS_LOCK)
	assureOpen();
	createIndexReader();
	if ( term != NULL )
		return indexReader->terms(term);
	else
		return indexReader->terms();
}


  CL_NS(document)::Document* IndexModifier::document(const int32_t n){
    Document* ret = _CLNEW Document;
    if (!document(n,ret) )
        _CLDELETE(ret);
    return ret;
  }
bool IndexModifier::document(int32_t n, CL_NS(document)::Document* doc){
	SCOPED_LOCK_MUTEX(directory->THIS_LOCK)
	assureOpen();
	createIndexReader();
	return indexReader->document(n, doc);
}
CL_NS(store)::Directory* IndexModifier::getDirectory(){
	return directory;
}

CL_NS_END
