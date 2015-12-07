/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#ifndef QFRAGMENTMAP_P_H
#define QFRAGMENTMAP_P_H

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

#include "QtCore/qglobal.h"
#include <stdlib.h>

QT_BEGIN_NAMESPACE

template <class T> class QFragmentMap;

class QFragment
{
public:
    quint32 size_left;
    quint32 parent;
    quint32 left;
    quint32 right;
    quint32 color : 1;
    quint32 size : 31;
    quint32 weight_left;
};

class Q_AUTOTEST_EXPORT QFragmentMapData
{
    enum Color { Red, Black };
public:
    QFragmentMapData(uint fs);
    ~QFragmentMapData();

    void init();

    class Header
    {
    public:
        quint32 tag;
        quint32 root; // this relies on being at the same position as parent in the fragment struct for now.
        quint32 unused1;
        quint32 unused2;
        quint32 size; // 32*allocated
        quint32 freelist;
        quint32 node_count;
        quint32 allocated;
    };


    int length() const;


    inline QFragment *fragment(uint index) {
        return (QFragment *)(fragments + index*fragmentSize);
    }
    inline const QFragment *fragment(uint index) const {
        return (QFragment *)(fragments + index*fragmentSize);
    }

    inline bool isRoot(uint index) const {
        return !fragment(index)->parent;
    }

    inline uint position(uint node) const {
        const QFragment *f = fragment(node);
        uint offset = f->size_left;
        while (f->parent) {
            uint p = f->parent;
            f = fragment(p);
            if (f->right == node)
                offset += f->size_left + f->size;
            node = p;
        }
        return offset;
    }
    inline uint sizeRight(uint node) const {
        uint sr = 0;
        const QFragment *f = fragment(node);
        node = f->right;
        while (node) {
            f = fragment(node);
            sr += f->size_left + f->size;
            node = f->right;
        }
        return sr;
    }
    inline uint sizeLeft(uint node) const {
        return fragment(node)->size_left;
    }

    inline uint index(uint node) const {
        const QFragment *f = fragment(node);
        uint offset = f->weight_left;
        while (f->parent) {
            uint p = f->parent;
            f = fragment(p);
            if (f->right == node)
                offset += f->weight_left + 1;
            node = p;
        }
        return offset;
    }
    inline uint weightRight(uint node) const {
        uint wr = 0;
        const QFragment *f = fragment(node);
        node = f->right;
        while (node) {
            f = fragment(node);
            wr += f->weight_left + 1;
            node = f->right;
        }
        return wr;
    }
    inline uint weightLeft(uint node) const {
        return fragment(node)->weight_left;
    }

    inline uint size(uint node) const { return fragment(node)->size; }

    inline void setSize(uint node, int new_size) {
        QFragment *f = fragment(node);
        int diff = new_size - f->size;
        f->size = new_size;
        while (f->parent) {
            uint p = f->parent;
            f = fragment(p);
            if (f->left == node)
                f->size_left += diff;
            node = p;
        }
    }


    uint findNode(int k) const;
    uint findNodeByIndex(int k) const;

    uint insert_single(int key, uint length);
    uint erase_single(uint f);

#ifdef QT_QMAP_DEBUG
    void inorder() {
        std::cout << ">>>> inorder >>>>" << std::endl;
        uint x = head->root;
        if (!x) {
            std::cout << "empty" << std::endl;
        } else {
            inorder(x);
        }
        std::cout << "<<<< inorder <<<<" << std::endl;
    }
    void inorder(uint x, int level = 0);
    void check();
#endif
    uint minimum(uint n) const {
        while (n && fragment(n)->left)
            n = fragment(n)->left;
        return n;
    }

    uint maximum(uint n) const {
        while (n && fragment(n)->right)
            n = fragment(n)->right;
        return n;
    }

    uint next(uint n) const;
    uint previous(uint n) const;

    inline uint root() const {
        Q_ASSERT(!head->root || !fragment(head->root)->parent);
        return head->root;
    }
    inline void setRoot(uint new_root) {
        Q_ASSERT(!head->root || !fragment(new_root)->parent);
        head->root = new_root;
    }

    void rotateLeft(uint x);
    void rotateRight(uint x);
    void rebalance(uint x);
    void removeAndRebalance(uint z);

    uint createFragment();
    void freeFragment(uint f);

    union {
        Header *head;
        char *fragments;
    };
    const int fragmentSize;
};

template <class T>
class QFragmentMap
{
public:
    class Iterator
    {
    public:
        QFragmentMap *pt;
        quint32 n;

        Iterator() : pt(0), n(0) {}
        Iterator(QFragmentMap *p, int node) : pt(p), n(node) {}
        Iterator(const Iterator& it) : pt(it.pt), n(it.n) {}

        inline bool atEnd() const { return !n; }

        bool operator==(const Iterator& it) const { return pt == it.pt && n == it.n; }
        bool operator!=(const Iterator& it) const { return pt != it.pt || n != it.n; }
        bool operator<(const Iterator &it) const { return position() < it.position(); }

        T *operator*() { Q_ASSERT(!atEnd()); return pt->fragment(n); }
        const T *operator*() const { Q_ASSERT(!atEnd()); return pt->fragment(n); }
        T *operator->() { Q_ASSERT(!atEnd()); return pt->fragment(n); }
        const T *operator->() const { Q_ASSERT(!atEnd()); return pt->fragment(n); }

        int position() const { Q_ASSERT(!atEnd()); return pt->data.position(n); }
        const T *value() const { Q_ASSERT(!atEnd()); return pt->fragment(n); }
        T *value() { Q_ASSERT(!atEnd()); return pt->fragment(n); }

        Iterator& operator++() {
            n = pt->data.next(n);
            return *this;
        }
        Iterator& operator--() {
            n = pt->data.previous(n);
            return *this;
        }

    };


    class ConstIterator
    {
    public:
        const QFragmentMap *pt;
        quint32 n;

        /**
         * Functions
         */
        ConstIterator() : pt(0), n(0) {}
        ConstIterator(const QFragmentMap *p, int node) : pt(p), n(node) {}
        ConstIterator(const ConstIterator& it) : pt(it.pt), n(it.n) {}
        ConstIterator(const Iterator& it) : pt(it.pt), n(it.n) {}

        inline bool atEnd() const { return !n; }

        bool operator==(const ConstIterator& it) const { return pt == it.pt && n == it.n; }
        bool operator!=(const ConstIterator& it) const { return pt != it.pt || n != it.n; }
        bool operator<(const ConstIterator &it) const { return position() < it.position(); }

        const T *operator*()  const { Q_ASSERT(!atEnd()); return pt->fragment(n); }
        const T *operator->()  const { Q_ASSERT(!atEnd()); return pt->fragment(n); }

        int position() const { Q_ASSERT(!atEnd()); return pt->data.position(n); }
        int size() const { Q_ASSERT(!atEnd()); return pt->data.size(n); }
        const T *value() const { Q_ASSERT(!atEnd()); return pt->fragment(n); }

        ConstIterator& operator++() {
            n = pt->data.next(n);
            return *this;
        }
        ConstIterator& operator--() {
            n = pt->data.previous(n);
            return *this;
        }
    };


    QFragmentMap() : data(sizeof(T)) {}
    ~QFragmentMap()
    {
        for (Iterator it = begin(); !it.atEnd(); ++it)
            it.value()->free();
    }

    inline void clear() {
        for (Iterator it = begin(); !it.atEnd(); ++it)
            it.value()->free();
        ::free(data.head);
        data.init();
    }

    inline Iterator begin() { return Iterator(this, data.minimum(data.root())); }
    inline Iterator end() { return Iterator(this, 0); }
    inline ConstIterator begin() const { return ConstIterator(this, data.minimum(data.root())); }
    inline ConstIterator end() const { return ConstIterator(this, 0); }

    inline ConstIterator last() const { return ConstIterator(this, data.maximum(data.root())); }

    inline bool isEmpty() const { return data.head->node_count == 0; }
    inline int numNodes() const { return data.head->node_count; }
    int length() const { return data.length(); }

    Iterator find(int k) { return Iterator(this, data.findNode(k)); }
    ConstIterator find(int k) const { return ConstIterator(this, data.findNode(k)); }

    uint findNode(int k) const { return data.findNode(k); }
    uint findNodeByIndex(int k) const { return data.findNodeByIndex(k); }

    uint insert_single(int key, uint length)
    {
        uint f = data.insert_single(key, length);
        if (f != 0) {
            T *frag = fragment(f);
            Q_ASSERT(frag);
            frag->initialize();
        }
        return f;
    }
    uint erase_single(uint f)
    {
      if (f != 0) {
          T *frag = fragment(f);
          Q_ASSERT(frag);
          frag->free();
      }
      return data.erase_single(f);
    }

    inline T *fragment(uint index) {
        Q_ASSERT(index != 0);
        return static_cast<T *>(data.fragment(index));
    }
    inline const T *fragment(uint index) const {
        Q_ASSERT(index != 0);
        return static_cast<const T *>(data.fragment(index));
    }
    inline uint position(uint node) const { return data.position(node); }
    inline uint index(uint node) const { return data.index(node); }
    inline uint next(uint n) const { return data.next(n); }
    inline uint previous(uint n) const { return data.previous(n); }
    inline uint size(uint node) const { return data.size(node); }
    inline void setSize(uint node, int new_size)
    { data.setSize(node, new_size);
      if (node != 0) {
          T *frag = fragment(node);
          Q_ASSERT(frag);
          frag->invalidate();
      }
    }

    inline int firstNode() const { return data.minimum(data.root()); }

private:
    friend class Iterator;
    friend class ConstIterator;

    QFragmentMapData data;

    QFragmentMap(const QFragmentMap& m);
    QFragmentMap& operator= (const QFragmentMap& m);
};

QT_END_NAMESPACE

#endif // QFRAGMENTMAP_P_H
