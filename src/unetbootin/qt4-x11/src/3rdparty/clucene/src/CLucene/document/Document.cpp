/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#include "CLucene/StdHeader.h"
#include "Document.h"
#include "Field.h"
#include "CLucene/util/StringBuffer.h"

CL_NS_USE(util)
CL_NS_DEF(document) 

    DocumentFieldEnumeration::DocumentFieldList::DocumentFieldList(Field* f, DocumentFieldList* n ) { 
    //Func - Constructor
	//Pre  - f != NULL
	//       n may be NULL
	//Post - Instance has been created
	    CND_PRECONDITION(f != NULL, "f is NULL");

		field = f;
		next  = n;
	}
	DocumentFieldEnumeration::DocumentFieldList::~DocumentFieldList(){
    //Func - Destructor
	//Pre  - true
	//Post - Instance has been destroyed

		// Instead of recursively deleting the field list we do
		// it iteratively to avoid stack overflows when
		// dealing with several thousands of fields.

		if (!field) {
            return; // nothing to do; deleted by different invocation of dtor
        }
        
		DocumentFieldList* cur = next;	
		while (cur != NULL)
		{
			DocumentFieldList* temp = cur->next;
			cur->next = NULL;
			
			_CLDELETE(cur);
			cur = temp;
		}
		_CLDELETE(field);
	}

	
	DocumentFieldEnumeration::DocumentFieldEnumeration(const DocumentFieldList* fl){
    //Func - Constructor
	//Pre  - fl may be NULL
	//Post - Instance has been created

		fields = fl;
	}

	DocumentFieldEnumeration::~DocumentFieldEnumeration(){
    //Func - Destructor
	//Pre  - true
	//Post - Instance has been destroyed
	}

	bool DocumentFieldEnumeration::hasMoreElements() const {
		return fields == NULL ? false : true;
	}

	Field* DocumentFieldEnumeration::nextElement() {
    //Func - Return the next element in the enumeration
	//Pre  - true
	//Post - The next element is returned or NULL


		Field* result = NULL;
		//Check if fields is still valid
        if (fields){
			result = fields->field;
		    fields = fields->next;
		    }
		return result;
	}

  /** Constructs a new document with no fields. */
    Document::Document(){
    //Func - Constructor
	//Pre  - true
	//Post - Instance has been created
        boost = 1.0f;
	    fieldList = NULL; 
	}

	Document::~Document(){
    //Func - Destructor
	//Pre  - true
	//Post - Instance has been destroyed
        boost = 1.0f;
		_CLDELETE(fieldList);
	}
	
	void Document::clear(){
		_CLDELETE(fieldList);
	}

	void Document::add(Field& field) {
		fieldList = _CLNEW DocumentFieldEnumeration::DocumentFieldList(&field, fieldList);
	}

   void Document::setBoost(qreal boost) {
      this->boost = boost;
   }

   qreal Document::getBoost() const {
      return boost;
   }


	 Field* Document::getField(const TCHAR* name)  const{
	    CND_PRECONDITION(name != NULL, "name is NULL");

		for (DocumentFieldEnumeration::DocumentFieldList* list = fieldList; list != NULL; list = list->next)
		   //cannot use interning here, because name is probably not interned
			if ( _tcscmp(list->field->name(), name) == 0 ){ 
				return list->field;
			}
		
		return NULL;
	}

	const TCHAR* Document::get(const TCHAR* field) const {
	    CND_PRECONDITION(field != NULL, "field is NULL");
		Field *f = getField(field);
		if (f!=NULL)
			return f->stringValue(); //this returns null it is a binary(reader)
		else
			return NULL;
	}

	DocumentFieldEnumeration* Document::fields() const {
		return _CLNEW DocumentFieldEnumeration(fieldList);
	}


	TCHAR* Document::toString() const {
		StringBuffer ret(_T("Document<"));
		for (DocumentFieldEnumeration::DocumentFieldList* list = fieldList; list != NULL; list = list->next) {
    		TCHAR* tmp = list->field->toString();
			ret.append( tmp );
    		if (list->next != NULL)
    		    ret.append(_T(" "));
			_CLDELETE_ARRAY( tmp ); 
		}
		ret.append(_T(">"));
		return ret.toString();
	} 



   void Document::removeField(const TCHAR* name) {
	  CND_PRECONDITION(name != NULL, "name is NULL");

      DocumentFieldEnumeration::DocumentFieldList* previous = NULL;
      DocumentFieldEnumeration::DocumentFieldList* current = fieldList;
      while (current != NULL) {
         //cannot use interning here, because name is probably not interned
         if ( _tcscmp(current->field->name(),name) == 0 ){
            if (previous){
               previous->next = current->next;
            }else
               fieldList = current->next;
            current->next=NULL; //ensure fieldlist destructor doesnt delete it
            _CLDELETE(current);
            return;
         }
		 		previous = current;
         current = current->next;
      }
   }
   
   void Document::removeFields(const TCHAR* name) {
	  CND_PRECONDITION(name != NULL, "name is NULL");

      DocumentFieldEnumeration::DocumentFieldList* previous = NULL;
      DocumentFieldEnumeration::DocumentFieldList* current = fieldList;
      while (current != NULL) {
         //cannot use interning here, because name is probably not interned
         if ( _tcscmp(current->field->name(),name) == 0 ){
            if (previous){
               previous->next = current->next;
            }else
               fieldList = current->next;

            current->next=NULL; //ensure fieldlist destructor doesnt delete it
            _CLDELETE(current);
			
			if ( previous )
				current = previous->next;
			else
				current = fieldList;
		 }else{
			previous = current;
			current = current->next;
		 }
      }
   }

   TCHAR** Document::getValues(const TCHAR* name) {
      DocumentFieldEnumeration* it = fields();
      int32_t count = 0;
      while ( it->hasMoreElements() ){
      	Field* f = it->nextElement();
         //cannot use interning here, because name is probably not interned
         if ( _tcscmp(f->name(),name) == 0 && f->stringValue() != NULL )
            count++;
      }
      _CLDELETE(it);
      it = fields();

      //todo: there must be a better way of doing this, we are doing two iterations of the fields
      TCHAR** ret = NULL;
	    if ( count > 0 ){
         //start again
         ret = _CL_NEWARRAY(TCHAR*,count+1);
         int32_t i=0;
         while ( it->hasMoreElements() ){
            Field* fld=it->nextElement();
            if ( _tcscmp(fld->name(),name)== 0 && fld->stringValue() != NULL ){
               ret[i] = stringDuplicate(fld->stringValue());
               i++;
            }
         }
         ret[count]=NULL;
	  }
     _CLDELETE(it);
     return ret;
   }
CL_NS_END
