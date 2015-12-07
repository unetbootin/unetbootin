/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#include "CLucene/StdHeader.h"
#include "SegmentHeader.h"

#include "Terms.h"

CL_NS_USE(util)
CL_NS_DEF(index)

SegmentTermPositions::SegmentTermPositions(const SegmentReader* _parent):
  SegmentTermDocs(_parent){
//Func - Constructor
//Pre  - Parent != NULL
//Post - The instance has been created

    CND_PRECONDITION(_parent != NULL, "Parent is NULL");
    
    proxStream = _parent->proxStream->clone();
    
    CND_CONDITION(proxStream != NULL,"proxStream is NULL");
    
    position  = 0;
    proxCount = 0;
}

SegmentTermPositions::~SegmentTermPositions() {
//Func - Destructor
//Pre  - true
//Post - The intance has been closed
    close();
}

TermDocs* SegmentTermPositions::__asTermDocs(){
    return (TermDocs*) this;
}
TermPositions* SegmentTermPositions::__asTermPositions(){
    return (TermPositions*) this;
}

void SegmentTermPositions::seek(const TermInfo* ti) {
    SegmentTermDocs::seek(ti);
    if (ti != NULL)
        proxStream->seek(ti->proxPointer);
    proxCount = 0;
}

void SegmentTermPositions::close() {
//Func - Frees the resources
//Pre  - true
//Post - The resources  have been freed

    SegmentTermDocs::close();
    //Check if proxStream still exists
    if(proxStream){
        proxStream->close();         
        _CLDELETE( proxStream );
    }
}

int32_t SegmentTermPositions::nextPosition() {
    /* DSR:CL_BUG: Should raise exception if proxCount == 0 at the
    ** beginning of this method, as in
    **   if (--proxCount == 0) throw ...;
    ** The JavaDocs for TermPositions.nextPosition declare this constraint,
    ** but CLucene doesn't enforce it. */
    proxCount--;
    return position += proxStream->readVInt();
}

bool SegmentTermPositions::next() {
    for (int32_t f = proxCount; f > 0; f--)		  // skip unread positions
        proxStream->readVInt();
    
    if (SegmentTermDocs::next()) {				  // run super
        proxCount = _freq;				  // note frequency
        position = 0;				  // reset position
        return true;
    }
    return false;
}

int32_t SegmentTermPositions::read(int32_t* docs, int32_t* freqs, int32_t length) {
    _CLTHROWA(CL_ERR_InvalidState,"TermPositions does not support processing multiple documents in one call. Use TermDocs instead.");
}

void SegmentTermPositions::skippingDoc() {
    for (int32_t f = _freq; f > 0; f--)		  // skip all positions
        proxStream->readVInt();
}

void SegmentTermPositions::skipProx(int64_t proxPointer){
    proxStream->seek(proxPointer);
    proxCount = 0;
}

CL_NS_END
