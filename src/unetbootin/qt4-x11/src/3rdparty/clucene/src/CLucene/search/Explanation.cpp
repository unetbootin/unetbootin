/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#include "CLucene/StdHeader.h"
#include "Explanation.h"
#include "CLucene/util/StringBuffer.h"

CL_NS_USE(util)
CL_NS_DEF(search)


Explanation::Explanation(qreal value, const TCHAR* description) {
 this->value = value;
 _tcsncpy(this->description,description,LUCENE_SEARCH_EXPLANATION_DESC_LEN);
}

Explanation::Explanation() {
 this->value = 0;
 this->description[0]=0;
}

Explanation::Explanation(const Explanation& copy){
    set(copy);
}
void Explanation::set(const Explanation& copy){
    this->value = copy.value;
    STRCPY_TtoT(description,copy.description,LUCENE_SEARCH_EXPLANATION_DESC_LEN);

    details.clear();
    typedef CL_NS(util)::Deletor::Object<Explanation> Deletor; 
    CL_NS(util)::CLArrayList<Explanation*, Deletor>::const_iterator itr;
    itr = copy.details.begin();
    while ( itr != copy.details.end() ){
        details.push_back( (*itr)->clone() );
        ++itr;
    }
}

Explanation::~Explanation(){
}

void Explanation::setDescription(const TCHAR* description) {
   _tcsncpy(this->description,description,LUCENE_SEARCH_EXPLANATION_DESC_LEN);
}


Explanation* Explanation::clone() const{ 
   return _CLNEW Explanation(*this); 
}

qreal Explanation::getValue() const{ 
   return value; 
}
  
void Explanation::setValue(qreal value) { 
   this->value = value; 
}

const TCHAR* Explanation::getDescription() const { 
   return description; 
}

///todo: mem leaks
TCHAR* Explanation::toString(int32_t depth) {
 StringBuffer buffer;
 for (int32_t i = 0; i < depth; i++) {
   buffer.append(_T("  "));
 }
 buffer.appendFloat(getValue(),2);
 buffer.append(_T(" = "));
 buffer.append(getDescription());
 buffer.append(_T("\n"));

 for ( uint32_t j=0;j<details.size();j++ ){
   TCHAR* tmp = details[j]->toString(depth+1);
   buffer.append(tmp);
   _CLDELETE_CARRAY(tmp);
 }
 return buffer.toString();
}

int Explanation::getDetailsLength(){
    return details.size();
}
Explanation* Explanation::getDetail(int i){
    return details[i];
}
/** The sub-nodes of this explanation node. */
void Explanation::getDetails(Explanation** ret) {
    uint32_t size = details.size();
    for ( uint32_t i=0;i<size;i++ ){
        ret[i] = details[i]->clone();
    }
    ret[size] = NULL;
}

/** Adds a sub-node to this explanation node. */
void Explanation::addDetail(Explanation* detail) {
   details.push_back(detail);
}

/** Render an explanation as text. */
TCHAR* Explanation::toString() {
 return toString(0);
}

/** Render an explanation as HTML. */
///todo: mem leaks
TCHAR* Explanation::toHtml() {
 StringBuffer buffer;
 TCHAR* tmp;
 buffer.append(_T("<ul>\n"));

 buffer.append(_T("<li>"));
 buffer.appendFloat(getValue(),2);
 buffer.append(_T(" = "));
 
 buffer.append(getDescription());
 buffer.append(_T("</li>\n"));

 for ( uint32_t i=0;i<details.size();i++ ){
   tmp = details[i]->toHtml();
    buffer.append(tmp);
    _CLDELETE_CARRAY(tmp);
 }
 buffer.append(_T("</ul>\n"));

 return buffer.toString();
}
CL_NS_END
