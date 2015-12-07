/****************************************************************************
**
** Copyright (C) 2004-2008 Trolltech ASA. All rights reserved.
** Copyright (C) 2001-2004 Roberto Raggi
**
** This file is part of the qt3to4 porting application of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License versions 2.0 or 3.0 as published by the Free Software
** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file.  Alternatively you may (at
** your option) use any later version of the GNU General Public
** License if such license has been publicly approved by Trolltech ASA
** (or its successors, if any) and the KDE Free Qt Foundation. In
** addition, as a special exception, Trolltech gives you certain
** additional rights. These rights are described in the Trolltech GPL
** Exception version 1.2, which can be found at
** http://www.trolltech.com/products/qt/gplexception/ and in the file
** GPL_EXCEPTION.txt in this package.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/. If
** you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** In addition, as a special exception, Trolltech, as the sole
** copyright holder for Qt Designer, grants users of the Qt/Eclipse
** Integration plug-in the right for the Qt/Eclipse Integration to
** link to functionality provided by Qt Designer and its related
** libraries.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not expressly
** granted herein.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef SMALLOBJECT_H
#define SMALLOBJECT_H

#include <QList>

#include <assert.h>
#include <stdlib.h>
#include <string.h>

QT_BEGIN_NAMESPACE

struct block_t
{
    static int N;

    block_t *chain;
    char *data;
    char *ptr;
    char *end;

    inline void init(int block_size = 256)
    {
        ++N;
        chain = 0;
        data = (char*) malloc(block_size);
        ptr = data;
        end = data + block_size;
    }

    inline void init0(int block_size)
    {
        init();
        memset(data, '0', block_size);
    }

    inline void destroy()
    {
        --N;
        if (chain) {
            chain->destroy();
            free(chain);
        }

        free(data);
    }

    inline void *allocate(size_t size, block_t **right_most)
    {
        const unsigned int block_size = 1 << 16;
        if (end < ptr + size) {
            assert( size < block_size );

            if (!chain) {
                chain = (block_t*) malloc(sizeof(block_t));
                Q_ASSERT(chain);
                chain->init(block_size);
            }

            return chain->allocate(size, right_most);
        }

        char *r = ptr;
        ptr += size;

        if (right_most)
            *right_most = this;

        return r;
    }

};

// a stupid memory pool
struct pool
{
    block_t blk;
    block_t *right_most;

    inline pool() { blk.init(); right_most = &blk; }
    inline ~pool() { blk.destroy(); }

    inline void *allocate(size_t size)
    {
        void *ptr = right_most->allocate(size, &right_most);
        Q_ASSERT(ptr);
        return ptr;

    }
    inline void *reallocate(void *old, size_t old_size, size_t size)
    {
        void *alloc = right_most->allocate(size, &right_most);
        memcpy(alloc, old, old_size);
        return alloc;
    }
private:
   Q_DISABLE_COPY(pool)
};

/*
    A memory pool that calls the destructor for each stored item when memory is
    freed, at the expence storing one BaseType* per item.

    Note that allocations for types that does not inherit BaseType
    is not allowed and will result in a crash when the memory is freed.
*/
template <typename BaseType>
class TypedPool
{
public:
    inline TypedPool() { blk.init(); right_most = &blk; }
    inline ~TypedPool()
    {
        foreach(BaseType *item, allocated)
            item->~BaseType();
        blk.destroy();
    }

    inline void *allocate(size_t size)
    {
        void *memory = right_most->allocate(size, &right_most);
        allocated.append(reinterpret_cast<BaseType *>(memory));
        Q_ASSERT(memory);
        return memory;
    }
private:
    Q_DISABLE_COPY(TypedPool)
    block_t blk;
    block_t *right_most;
    QList<BaseType *> allocated;
};

struct SmallObject
{
    static void *operator new(size_t size);
    static void operator delete(void *p);
};

QT_END_NAMESPACE

#endif
