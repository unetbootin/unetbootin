/*
 * Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
 *
 * Distributable under the terms of either the Apache License (Version 2.0) or 
 * the GNU Lesser General Public License, as specified in the COPYING file.
 *
 * Changes are Copyright(C) 2007, 2008 by Nokia Corporation and/or its subsidiary(-ies), all rights reserved.
*/
#ifndef _lucene_index_FieldInfos_
#define _lucene_index_FieldInfos_

#if defined(_LUCENE_PRAGMA_ONCE)
#   pragma once
#endif

#include <QtCore/QString>

#include "CLucene/store/Directory.h"
#include "CLucene/document/Document.h"
#include "CLucene/util/VoidMap.h"
#include "CLucene/util/VoidList.h"

CL_NS_DEF(index)

class FieldInfo : LUCENE_BASE
{
  public:
	//name of the field
	const TCHAR* name;

    //Is field indexed? true = yes false = no
	bool isIndexed;

	//field number
	const int32_t number;

	// true if term vector for this field should be stored
	bool storeTermVector;
	bool storeOffsetWithTermVector;
	bool storePositionWithTermVector;

	bool omitNorms; // omit norms associated with indexed fields

	//Func - Constructor
	//       Initialises FieldInfo.
	//       na holds the name of the field
	//       tk indicates whether this field is indexed or not
	//       nu indicates its number
	//Pre  - na != NULL and holds the name of the field
	//       tk is true or false
	//       number >= 0
	//Post - The FieldInfo instance has been created and initialized.
	//       name holds the duplicated string of na
	//       isIndexed = tk
	//       number = nu  
	FieldInfo(const TCHAR* fieldName, bool isIndexed, int32_t fieldNumber, 
		bool storeTermVector, bool storeOffsetWithTermVector, 
        bool storePositionWithTermVector, bool omitNorms);

    //Func - Destructor
	//Pre  - true
	//Post - The instance has been destroyed
	~FieldInfo();
};

/* Access to the Field Info file that describes document fields and whether or
 * not they are indexed. Each segment has a separate Field Info file. Objects
 * of this class are thread-safe for multiple readers, but only one thread can
 * be adding documents at a time, with no other reader or writer threads
 * accessing this object.
*/
class FieldInfos : LUCENE_BASE
{
private:
	// we now use internd field names, so we can use the voidCompare to
    // directly compare the strings
	typedef CL_NS(util)::CLHashMap<const TCHAR*, FieldInfo*,
	    CL_NS(util)::Compare::TChar,CL_NS(util)::Equals::TChar > defByName;
	defByName byName;
		
	CL_NS(util)::CLArrayList<FieldInfo*,
        CL_NS(util)::Deletor::Object<FieldInfo> > byNumber;
		
public:
	enum {
		IS_INDEXED                      = 0x1,
		STORE_TERMVECTOR                = 0x2,
		STORE_POSITIONS_WITH_TERMVECTOR = 0x4,
		STORE_OFFSET_WITH_TERMVECTOR    = 0x8,
		OMIT_NORMS                      = 0x10
	};

	FieldInfos();
	~FieldInfos();

    // Construct a FieldInfos object using the directory and the name of the
    // file IndexInput
    // @param d The directory to open the IndexInput from
    // @param name Name of the file to open the IndexInput from in the Directory
    // @throws IOException
    // @see #read
	FieldInfos(CL_NS(store)::Directory* d, const QString& name);

	int32_t fieldNumber(const TCHAR* fieldName)const;
	
    // Return the fieldinfo object referenced by the fieldNumber.
    // @param fieldNumber
    // @return the FieldInfo object or null when the given fieldNumber
    // doesn't exist.
	FieldInfo* fieldInfo(const TCHAR* fieldName) const;
	
    // Return the fieldName identified by its number.
    // @param fieldNumber
    // @return the fieldName or an empty string when the field
    // with the given number doesn't exist.
	const TCHAR* fieldName(const int32_t fieldNumber) const;

	FieldInfo* fieldInfo(const int32_t fieldNumber) const;

	int32_t size()const;

  	bool hasVectors() const;

	// Adds field info for a Document. 
	void add(const CL_NS(document)::Document* doc);

	// Merges in information from another FieldInfos. 
	void add(FieldInfos* other);
	
	
	/** If the field is not yet known, adds it. If it is known, checks to make
	*  sure that the isIndexed flag is the same as was given previously for this
	*  field. If not - marks it as being indexed.  Same goes for the TermVector
	* parameters.
	* 
	* @param name The name of the field
	* @param isIndexed true if the field is indexed
	* @param storeTermVector true if the term vector should be stored
	* @param storePositionWithTermVector true if the term vector with positions should be stored
	* @param storeOffsetWithTermVector true if the term vector with offsets should be stored
	*/
	void add(const TCHAR* name, bool isIndexed, bool storeTermVector = false,
        bool storePositionWithTermVector = false,
        bool storeOffsetWithTermVector = false, bool omitNorms = false);
	
	/**
	* Assumes the fields are not storing term vectors 
	* @param names The names of the fields
	* @param isIndexed true if the field is indexed
	* @param storeTermVector true if the term vector should be stored
	* 
	* @see #add(String, boolean)
	*/
	void add(const TCHAR** names, bool isIndexed, bool storeTermVector = false,
        bool storePositionWithTermVector = false,
        bool storeOffsetWithTermVector = false, bool omitNorms = false);

	void write(CL_NS(store)::Directory* d, const QString& name) const;
	void write(CL_NS(store)::IndexOutput* output) const;

private:
	void read(CL_NS(store)::IndexInput* input);
	void addInternal(const TCHAR* name, bool isIndexed, bool storeTermVector,
		bool storePositionWithTermVector, bool storeOffsetWithTermVector,
        bool omitNorms);

};

CL_NS_END

#endif
