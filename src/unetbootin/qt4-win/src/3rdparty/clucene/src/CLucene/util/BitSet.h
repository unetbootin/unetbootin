/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#ifndef _lucene_util_BitSet_
#define _lucene_util_BitSet_

#if defined(_LUCENE_PRAGMA_ONCE)
# pragma once
#endif

#include "CLucene/store/Directory.h"

CL_NS_DEF(util)
class BitSet:LUCENE_BASE {
	int32_t _size;
	int32_t _count;
	uint8_t *bits;
	
protected:
	BitSet( const BitSet& copy );

public:
	///Create a bitset with the specified size
	BitSet ( int32_t size );
	BitSet(CL_NS(store)::Directory* d, const char* name);
	void write(CL_NS(store)::Directory* d, const char* name);
	
	///Destructor for the bit set
	~BitSet();
	
	///get the value of the specified bit
	inline bool get(const int32_t bit) const{
		return (bits[bit >> 3] & (1 << (bit & 7))) != 0;
	}
	
	///set the value of the specified bit
	void set(const int32_t bit, bool val=true);
	
	///returns the size of the bitset
	int32_t size() const;
	
	/// Returns the total number of one bits in this BitSet.  This is efficiently
	///	computed and cached, so that, if the BitSet is not changed, no
	///	recomputation is done for repeated calls. 
	int32_t count();
	BitSet *clone() const;
};
CL_NS_END
#endif
