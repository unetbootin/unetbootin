/*
 * Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
 *
 * Distributable under the terms of either the Apache License (Version 2.0) or 
 * the GNU Lesser General Public License, as specified in the COPYING file.
 *
 * Changes are Copyright(C) 2007, 2008 by Nokia Corporation and/or its subsidiary(-ies), all rights reserved.
*/
#include "CLucene/StdHeader.h"
#include "BitSet.h"
#include "CLucene/store/Directory.h"

CL_NS_USE(store)
CL_NS_DEF(util)

BitSet::BitSet(const BitSet& copy)
    : _size(copy._size)
    , _count(-1)
{
    int32_t len = (_size >> 3) + 1;
    bits = _CL_NEWARRAY(uint8_t, len);
    memcpy(bits, copy.bits, len);
}

BitSet::BitSet(int32_t size)
    : _size(size)
    , _count(-1)
{
    int32_t len = (_size >> 3) + 1;
    bits = _CL_NEWARRAY(uint8_t, len);
    memset(bits, 0, len);
}

BitSet::BitSet(CL_NS(store)::Directory* d, const QString& name)
{
    _count = -1;
    CL_NS(store)::IndexInput* input = d->openInput(name);
    try {
        _size = input->readInt();			  // read size
        _count = input->readInt();			  // read count

        bits = _CL_NEWARRAY(uint8_t,(_size >> 3) + 1);		  // allocate bits
        input->readBytes(bits, (_size >> 3) + 1);	  // read bits
    } _CLFINALLY (
        input->close();
        _CLDELETE(input );
    );
}

void BitSet::write(CL_NS(store)::Directory* d, const QString& name)
{
    CL_NS(store)::IndexOutput* output = d->createOutput(name);
    try {
        output->writeInt(size());			  // write size
        output->writeInt(count());			  // write count
        output->writeBytes(bits, (_size >> 3) + 1);	  // write bits
    } _CLFINALLY (
        output->close();
        _CLDELETE(output);
    );
}

BitSet::~BitSet()
{
    _CLDELETE_ARRAY(bits);
}

void BitSet::set(int32_t bit, bool val)
{
    if (val)
        bits[bit >> 3] |= 1 << (bit & 7);
    else
        bits[bit >> 3] &= ~(1 << (bit & 7));

    _count = -1;
}

int32_t BitSet::size() const
{
    return _size;
}

int32_t BitSet::count()
{
    // if the BitSet has been modified
    if (_count == -1) {
        static const uint8_t BYTE_COUNTS[] = {  
            0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
            1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
            1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
            2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
            1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
            2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
            2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
            3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
            1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
            2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
            2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
            3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
            2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
            3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
            3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
            4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8};

            int32_t c = 0;
            int32_t end = (_size >> 3) + 1;
            for (int32_t i = 0; i < end; i++)
                c += BYTE_COUNTS[bits[i]];	  // sum bits per uint8_t
            _count = c;
    }
    return _count;
}

BitSet* BitSet::clone() const
{
    return _CLNEW BitSet(*this);
}

CL_NS_END
