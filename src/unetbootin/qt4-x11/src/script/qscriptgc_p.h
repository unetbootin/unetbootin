/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtScript module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
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

template <typename _Tp>
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
        m_sweeping(false) {
        trivial.reset();
    }

    inline ~GCAlloc() {
    }

    inline void destruct() {
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

    inline _Tp *operator()(int generation)
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
        }
        m_current->generation = generation;

        return reinterpret_cast<_Tp*> (m_current->data());
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

    void sweep(int generation)
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
                    m_head = blk;

                _Tp *data = reinterpret_cast<_Tp *>(tmp->data());
                data->finalize();
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
