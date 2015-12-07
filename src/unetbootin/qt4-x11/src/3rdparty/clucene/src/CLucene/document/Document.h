/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#ifndef _lucene_document_Document_
#define _lucene_document_Document_

#if defined(_LUCENE_PRAGMA_ONCE)
# pragma once
#endif

#include "Field.h"

///todo: jlucene has change from using DocumentFieldList/Enumeration
///to using a java List... do we want to do this too?
CL_NS_DEF(document)

class Document; //predefine
class DocumentFieldEnumeration :LUCENE_BASE{
    class DocumentFieldList :LUCENE_BASE{
    public:
    	DocumentFieldList(Field* f, DocumentFieldList* n);
    	~DocumentFieldList();
    	Field* field;
    	DocumentFieldList* next;
    };
	friend class Document;
private:
	const DocumentFieldList* fields;
public:
	DocumentFieldEnumeration(const DocumentFieldList* fl);
	~DocumentFieldEnumeration();
	bool hasMoreElements() const;
	Field* nextElement();
};

/** Documents are the unit of indexing and search.
*
* A Document is a set of fields.  Each field has a name and a textual value.
* A field may be {@link Field#isStored() stored} with the document, in which
* case it is returned with search hits on the document.  Thus each document
* should typically contain one or more stored fields which uniquely identify
* it.
*
* <p>Note that fields which are <i>not</i> {@link Field#isStored() stored} are
* <i>not</i> available in documents retrieved from the index, e.g. with {@link
* Hits#doc(int32_t, Document*)}, {@link Searcher#doc(int32_t, Document*)} or {@link
* IndexReader#document(int32_t, Document*)}.
*/
class Document:LUCENE_BASE {
private:
	DocumentFieldEnumeration::DocumentFieldList* fieldList;
	qreal boost;
public:
	Document();
	~Document();

	/**
	* <p>Adds a field to a document.  Several fields may be added with
	* the same name.  In this case, if the fields are indexed, their text is
	* treated as though appended for the purposes of search.</p>
	* <p> Note that add like the removeField(s) methods only makes sense 
	* prior to adding a document to an index. These methods cannot
	* be used to change the content of an existing index! In order to achieve this,
	* a document has to be deleted from an index and a new changed version of that
	* document has to be added.</p>
	*
	*/
	void add(Field& field);
	/** Returns a field with the given name if any exist in this document, or
	* null.  If multiple fields exists with this name, this method returns the
	* first value added. 
	* Note: name is case sensitive
	*/
	Field* getField(const TCHAR* name) const;
	
	/** Returns the string value of the field with the given name if any exist in
	* this document, or null.  If multiple fields exist with this name, this
	* method returns the first value added. If only binary fields with this name
	* exist, returns null.
	* Note: name is case sensitive
	*/
	const TCHAR* get(const TCHAR* field) const;

	/** Returns an Enumeration of all the fields in a document. */
	DocumentFieldEnumeration* fields() const;
	/** Prints the fields of a document for human consumption. */
	TCHAR* toString() const;

	/** Sets a boost factor for hits on any field of this document.  This value
	* will be multiplied into the score of all hits on this document.
	*
	* <p>Values are multiplied into the value of {@link Field#getBoost()} of
	* each field in this document.  Thus, this method in effect sets a default
	* boost for the fields of this document.
	*
	* @see Field#setBoost(qreal)
	*/
	void setBoost(qreal boost);
  
	/** Returns the boost factor for hits on any field of this document.
	*
	* <p>The default value is 1.0.
	*
	* <p>Note: This value is not stored directly with the document in the index.
	* Documents returned from {@link IndexReader#document(int32_t, Document*)} and
	* {@link Hits#doc(int32_t, Document*)} may thus not have the same value present as when
	* this document was indexed.
	*
	* @see #setBoost(qreal)
	*/
	qreal getBoost() const;


	/**
	* <p>Removes field with the specified name from the document.
	* If multiple fields exist with this name, this method removes the first field that has been added.
	* If there is no field with the specified name, the document remains unchanged.</p>
	* <p> Note that the removeField(s) methods like the add method only make sense 
	* prior to adding a document to an index. These methods cannot
	* be used to change the content of an existing index! In order to achieve this,
	* a document has to be deleted from an index and a new changed version of that
	* document has to be added.</p>
	* Note: name is case sensitive
	*/
	void removeField(const TCHAR* name);
        
	/**
	* <p>Removes all fields with the given name from the document.
	* If there is no field with the specified name, the document remains unchanged.</p>
	* <p> Note that the removeField(s) methods like the add method only make sense 
	* prior to adding a document to an index. These methods cannot
	* be used to change the content of an existing index! In order to achieve this,
	* a document has to be deleted from an index and a new changed version of that
	* document has to be added.</p>
	* Note: name is case sensitive
	*/
	void removeFields(const TCHAR* name);

	/**
	* Returns an array of values of the field specified as the method parameter.
	* This method can return <code>null</code>.
	* Note: name is case sensitive
	*
	* @param name the name of the field
	* @return a <code>String[]</code> of field values
	*/
	TCHAR** getValues(const TCHAR* name);
	
	/**
	* Empties out the document so that it can be reused
	*/
	void clear();
};
CL_NS_END
#endif
