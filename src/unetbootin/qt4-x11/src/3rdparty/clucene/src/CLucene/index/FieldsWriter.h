/*
 * Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
 *
 * Distributable under the terms of either the Apache License (Version 2.0) or 
 * the GNU Lesser General Public License, as specified in the COPYING file.
 *
 * Changes are Copyright(C) 2007, 2008 by Nokia Corporation and/or its subsidiary(-ies), all rights reserved.
*/
#ifndef _lucene_index_FieldsWriter_
#define _lucene_index_FieldsWriter_

#if defined(_LUCENE_PRAGMA_ONCE)
#   pragma once
#endif

#include <QtCore/QString>

#include "CLucene/document/Document.h"
#include "CLucene/store/Directory.h"
#include "CLucene/store/IndexOutput.h"

CL_NS_DEF(index)

class FieldInfos;

class FieldsWriter : LUCENE_BASE
{
private:
	FieldInfos* fieldInfos;

	CL_NS(store)::IndexOutput* fieldsStream;
	CL_NS(store)::IndexOutput* indexStream;

public:
	LUCENE_STATIC_CONSTANT(uint8_t, FIELD_IS_TOKENIZED = 0x1);
	LUCENE_STATIC_CONSTANT(uint8_t, FIELD_IS_BINARY = 0x2);
	LUCENE_STATIC_CONSTANT(uint8_t, FIELD_IS_COMPRESSED = 0x4);

	FieldsWriter(CL_NS(store)::Directory* d, const QString& segment, FieldInfos* fn);
	~FieldsWriter();

	void close();

	void addDocument(CL_NS(document)::Document* doc);
};

CL_NS_END

#endif
