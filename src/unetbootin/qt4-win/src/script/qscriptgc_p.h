/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtScript module of the Qt Toolkit.
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

#ifndef QSCRIPTGC_P_H
#define QSCRIPTGC_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtCore/qglobal.h>

#ifndef QT_NO_SCRIPT

#include <QtCore/QtDebug>
#include <new>

#include "qscriptmemorypool_p.h"

QT_BEGIN_NAMESPACE

namespace QScript {

class GCBlock
{
public:
    GCBlock *next;

    union {
        int generation;
        uint flags;
    };

public:
    inline GCBlock(GCBlock *n):
        next(n), flags(0) {}

    inline void *data()
    { return reinterpret_cast<char *>(this) + sizeof(GCBlock); }

    inline static GCBlock *get(void *ptr)
    {
        char *where = reinterpret_cast<char *>(ptr);
        return reinterpret_cast<GCBlock *>(where - sizeof(GCBlock));
    }
};

template <typename _Tp, typename _FinalizerArg>
class GCAlloc
{
private:
    int m_new_allocated_blocks;
    int m_free_blocks;
    int m_new_allocated_extra_bytes;
    GCBlock *m_head;
    GCBlock *m_current;
    GCBlock *m_free;
    bool m_blocked_gc;
    bool m_force_gc;
    bool m_sweeping;
    MemoryPool pool;
    _Tp trivial;

public:
    enum { MaxNumberOfBlocks = 1 << 14 };
    enum { MaxNumberOfExtraBytes = 0x800000 };

public:
    inline GCAlloc():
        m_new_allocated_blocks(0),
        m_free_blocks(0),
        m_new_allocated_extra_bytes(0),
        m_head(0),
        m_current(0),
        m_free(0),
        m_blocked_gc(false),
        m_force_gc(false),
        m_sweeping(false) {}

    inline ~GCAlloc() {
    }

    inline void destruct(_FinalizerArg farg) {
        m_sweeping = true;
        GCBlock *blk = m_free;

        if (! blk) {
            blk = m_head;
            m_head = 0;
        }

        while (blk) {
            GCBlock *was = blk;
            blk = blk->next;

            Q_ASSERT(was->data());
            _Tp *data = reinterpret_cast<_Tp*>(was->data());
            data->finalize(farg);
            data->~_Tp();
            blk->~GCBlock();

            if (! blk && m_head) {
                blk = m_head;
                m_head = 0;
            }
        }
        m_sweeping = false;
    }

    inline int newAllocatedBlocks() const { return m_new_allocated_blocks; }
    inline int freeBlocks() const { return m_free_blocks; }

    inline _Tp *operator()()
    {
        GCBlock *previous = m_current;
        void *where = 0;

        if (! m_free) {
            Q_ASSERT (m_free_blocks == 0);
            where = pool.allocate(sizeof(GCBlock) + sizeof(_Tp));
            ++m_new_allocated_blocks;
            (void) new (reinterpret_cast<char*>(where) + sizeof(GCBlock)) _Tp();
        } else {
            --m_free_blocks;
            where = m_free;
            m_free = m_free->next;

            if (! m_free)
                m_force_gc = true;
        }

        m_current = new (where) GCBlock(0);

        if (! previous) {
            Q_ASSERT(! m_head);
            m_head = m_current;
        } else {
            previous->next = m_current;
            m_current->generation = m_head->generation;
        }

        return reinterpret_cast<_Tp*> (m_current->data());
    }

    inline void release(_Tp *ptr)
    {
        GCBlock::get(ptr)->released = true;
    }

    inline bool blocked() const
    {
        return m_blocked_gc;
    }

    inline bool sweeping() const
    {
        return m_sweeping;
    }

    inline bool blockGC(bool block)
    {
        bool was = m_blocked_gc;
        m_blocked_gc = block;
        return was;
    }

    inline void requestGC()
    {
        m_force_gc = true;
    }

    inline void adjustBytesAllocated(int bytes)
    { m_new_allocated_extra_bytes += bytes; }

    inline bool poll()
    {
        if (m_blocked_gc || ! m_head)
            return false;

        else if (m_force_gc) {
            m_force_gc = false;
            return true;
        }

        else if (m_free && ! m_free->next)
            return true;

        return (m_new_allocated_blocks >= MaxNumberOfBlocks)
            || ((m_new_allocated_extra_bytes >= MaxNumberOfExtraBytes)
                && (m_new_allocated_blocks > 0));
    }

    inline int generation(_Tp *ptr) const
    { return GCBlock::get(ptr)->generation; }

    inline GCBlock *head() const
    { return m_head; }

    void sweep(int generation, _FinalizerArg farg)
    {
        m_sweeping = true;
        GCBlock *blk = m_head;
        m_current = 0;

        m_new_allocated_blocks = 0;
        m_new_allocated_extra_bytes = 0;

        while (blk != 0) {
            if (blk->generation != generation) {
                if (m_current)
                    m_current->next = blk->next;

                GCBlock *tmp = blk;
                blk = blk->next;    // advance the pointer

                tmp->next = m_free; // prepend the node to the free list...
                m_free = tmp;
                ++m_free_blocks;

                if (m_free == m_head)
                    m_head = 0;

                _Tp *data = reinterpret_cast<_Tp *>(tmp->data());
                data->finalize(farg); // we need it
                tmp->~GCBlock();
            } else {
                m_current = blk;
                blk = blk->next;
            }
        }

        if (! m_current)
            m_head = m_current;
        m_sweeping = false;
    }

    class const_iterator
    {
    public:
        typedef _Tp value_type;
        typedef const _Tp *pointer;
        typedef const _Tp &reference;
        inline const_iterator() : i(0) { }
        inline const_iterator(GCBlock *block) : i(block) { }
        inline const_iterator(const const_iterator &o)
        { i = reinterpret_cast<const const_iterator &>(o).i; }

        inline const _Tp *data() const { return reinterpret_cast<_Tp*>(i->data()); }
        inline const _Tp &value() const { return *reinterpret_cast<_Tp*>(i->data()); }
        inline const _Tp &operator*() const { return *reinterpret_cast<_Tp*>(i->data()); }
        inline const _Tp *operator->() const { return reinterpret_cast<_Tp*>(i->data()); }
        inline bool operator==(const const_iterator &o) const { return i == o.i; }
        inline bool operator!=(const const_iterator &o) const { return i != o.i; }

        inline const_iterator &operator++() {
            i = i->next;
            return *this;
        }
    private:
        GCBlock *i;
    };
    friend class const_iterator;

    inline const_iterator constBegin() const { return const_iterator(m_head); }
    inline const_iterator constEnd() const { return const_iterator(0); }
    
private:
    Q_DISABLE_COPY(GCAlloc)
};

} // namespace QScript

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT
#endif // QSCRIPT_GC_H
