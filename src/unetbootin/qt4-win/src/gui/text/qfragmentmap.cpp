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

#include <private/qtools_p.h>

#include "qfragmentmap_p.h"

#include <stdlib.h>
#include <new>

#define F(x) (*fragment(x))
#define X (*fragment(x))
#define Y (*fragment(y))
#define Z (*fragment(z))
#define W (*fragment(w))
#define N (*fragment(n))
#define P (*fragment(p))
#define PP (*fragment(pp))

QT_BEGIN_NAMESPACE

#ifdef QT_QMAP_DEBUG
#define PMDEBUG qDebug
void QFragmentMap::inorder(uint x, int level) {
    if (X.left)
        inorder(X.left, level + 1);
    for (int i = 0; i < level; ++i)
        std::cout << "    ";
    std::cout << "this=" << x << " Key=" << key(x) << " size_left=" << X.size_left
              << " size=" << X.size
              << " textPos=" << X.position << (X.color == Red ? " Red" : " Black")
              << std::endl;
    if (X.right)
        inorder(X.right, level + 1);
}
void QFragmentMap::check()
{
    Q_ASSERT((head->node_count == 0 && head->root == 0)
             || (head->node_count != 0 && head->root != 0 && F(head->root).parent == 0));

    ConstIterator it = begin();
    int key = 0;
    for (; it != end(); ++it) {
        Q_ASSERT(key == it.key());
        key += (*it)->size;
    }
}
#else
#define PMDEBUG if(0)qDebug
static inline void inorder() {}
static inline void check() {}
#endif

#define TAG(a,b,c,d) (((quint32)a) << 24) | (((quint32)b) << 16) | (((quint32)c) << 8) | d;

QFragmentMapData::QFragmentMapData(uint fs)
    : fragmentSize(qMax<uint>(fs, sizeof(Header)))
{
    init();
}

void QFragmentMapData::init()
{
    fragments = (char *)malloc(64*fragmentSize);
    head->tag = TAG('p', 'm', 'a', 'p');
    head->root = 0;
    head->freelist = 1;
    head->node_count = 0;
    head->allocated = 64;
    // mark all items to the right as unused
    F(head->freelist).right = 0;
#ifndef QT_NO_DEBUG
    for (uint i = 1; i < head->allocated; ++i)
        F(i).parent = 0xdeadbeef;
#endif
}

QFragmentMapData::~QFragmentMapData()
{
    free(head);
}

uint QFragmentMapData::createFragment()
{
    Q_ASSERT(head->freelist <= head->allocated);

    uint freePos = head->freelist;
    if (freePos == head->allocated) {
        // need to create some free space
        uint needed = qAllocMore((freePos+1)*fragmentSize, 0);
        Q_ASSERT(needed/fragmentSize > head->allocated);
        fragments = (char *)realloc(fragments, needed);
        head->allocated = needed/fragmentSize;
        F(freePos).right = 0;
#ifndef QT_NO_DEBUG
        for (uint i = freePos; i < head->allocated; ++i)
            F(i).parent = 0xdeadbeef;
#endif
    }

    uint nextPos = F(freePos).right;
    if (!nextPos) {
        nextPos = freePos+1;
        if (nextPos < head->allocated)
            F(nextPos).right = 0;
    }

    head->freelist = nextPos;

#ifndef QT_NO_DEBUG
    Q_ASSERT(F(freePos).parent == 0xdeadbeef);
    F(freePos).parent = 0;
    if (nextPos < head->allocated) {
        Q_ASSERT(F(nextPos).parent == 0xdeadbeef);
    }
#endif

    ++head->node_count;

    PMDEBUG("===> createFragment at %d", freePos);
    return freePos;
}

void QFragmentMapData::freeFragment(uint i)
{
    PMDEBUG("===> freeFragment at %d", i);
#ifndef QT_NO_DEBUG
    Q_ASSERT(F(i).parent != 0xdeadbeef);
    if (head->freelist < head->allocated) {
        Q_ASSERT(F(head->freelist).parent == 0xdeadbeef);
    }
    F(i).parent = 0xdeadbeef;
#endif
    F(i).right = head->freelist;
    head->freelist = i;

    --head->node_count;
}


uint QFragmentMapData::next(uint n) const {
    Q_ASSERT(n);
    if (N.right) {
        n = N.right;
        while (N.left)
            n = N.left;
    } else {
        uint y = N.parent;
        while (N.parent && n == Y.right) {
            n = y;
            y = Y.parent;
        }
        n = y;
    }
    return n;
}

uint QFragmentMapData::previous(uint n) const {
    if (!n)
        return maximum(root());

    if (N.left) {
        n = N.left;
        while (N.right)
            n = N.right;
    } else {
        uint y = N.parent;
        while (N.parent && n == Y.left) {
            n = y;
            y = Y.parent;
        }
        n = y;
    }
    return n;
}


/*
     x              y
      \            / \
       y    -->   x   b
      / \          \
     a   b          a
*/
void QFragmentMapData::rotateLeft(uint x)
{
    uint p = X.parent;
    uint y = X.right;
    PMDEBUG("    rotateLeft on x=%d (y=%d, p=%d)", x, y, p);


    if (y) {
        X.right = Y.left;
        if (Y.left)
            F(Y.left).parent = x;
        Y.left = x;
        Y.parent = p;
    } else {
        X.right = 0;
    }
    if (!p) {
        Q_ASSERT(head->root == x);
        head->root = y;
    }
    else if (x == P.left)
        P.left = y;
    else
        P.right = y;
    X.parent = y;
    Y.size_left += X.size_left + X.size;
    Y.weight_left += X.weight_left + 1;

    inorder();
    check();
}


/*
         x          y
        /          / \
       y    -->   a   x
      / \            /
     a   b          b
*/
void QFragmentMapData::rotateRight(uint x)
{
    uint y = X.left;
    uint p = X.parent;
    PMDEBUG("    rotateRight on x=%d (y=%d, p=%d)", x, y, p);

    if (y) {
        X.left = Y.right;
        if (Y.right)
            F(Y.right).parent = x;
        Y.right = x;
        Y.parent = p;
    } else {
        X.left = 0;
    }
    if (!p) {
        Q_ASSERT(head->root == x);
        head->root = y;
    }
    else if (x == P.right)
        P.right = y;
    else
        P.left = y;
    X.parent = y;
    X.size_left -= Y.size_left + Y.size;
    X.weight_left -= Y.weight_left + 1;

    inorder();
    check();
}


void QFragmentMapData::rebalance(uint x)
{
    X.color = Red;

    PMDEBUG("  -> rebalance x=%d", x);
    inorder();

    while (X.parent && F(X.parent).color == Red) {
        uint p = X.parent;
        uint pp = P.parent;
        Q_ASSERT(pp);
        if (p == PP.left) {
            uint y = PP.right;
            if (y && Y.color == Red) {
                P.color = Black;
                Y.color = Black;
                PP.color = Red;
                x = pp;
            } else {
                if (x == P.right) {
                    x = p;
                    rotateLeft(x);
                    p = X.parent;
                    pp = P.parent;
                }
                P.color = Black;
                if (pp) {
                    PP.color = Red;
                    rotateRight(pp);
                }
            }
        } else {
            uint y = PP.left;
            if (y && Y.color == Red) {
                P.color = Black;
                Y.color = Black;
                PP.color = Red;
                x = pp;
            } else {
                if (x == P.left) {
                    x = p;
                    rotateRight(x);
                    p = X.parent;
                    pp = P.parent;
                }
                P.color = Black;
                if (pp) {
                    PP.color = Red;
                    rotateLeft(pp);
                }
            }
        }
    }
    F(root()).color = Black;
    check();
}


uint QFragmentMapData::erase_single(uint z)
{
    uint w = previous(z);
    uint y = z;
    uint x;
    uint p;

    if (!Y.left) {
        x = Y.right;
    } else if (!Y.right) {
        x = Y.left;
    } else {
        y = Y.right;
        while (Y.left)
            y = Y.left;
        x = Y.right;
    }

    PMDEBUG("removeAndRebalance on %d (x=%d, y=%d)", z, x, y);
    inorder();

    if (y != z) {
        F(Z.left).parent = y;
        Y.left = Z.left;
        Y.size_left = Z.size_left;
        Y.weight_left = Z.weight_left;
        if (y != Z.right) {
            /*
                     z                y
                    / \              / \
                   a   b            a   b
                      /                /
                    ...     -->      ...
                    /                /
                   y                x
                  / \
                 0   x
             */
            p = Y.parent;
            if (x)
                X.parent = p;
            P.left = x;
            Y.right = Z.right;
            F(Z.right).parent = y;
            uint n = p;
            while (n != y) {
                N.size_left -= Y.size;
                N.weight_left -= 1;
                n = N.parent;
            }
        } else {
            /*
                     z                y
                    / \              / \
                   a   y     -->    a   x
                      / \
                     0   x
             */
            p = y;
        }
        uint zp = Z.parent;
        if (!zp) {
            Q_ASSERT(head->root == z);
            head->root = y;
        } else if (F(zp).left == z) {
            F(zp).left = y;
            F(zp).size_left -= Z.size;
            F(zp).weight_left -= 1;
        } else {
            F(zp).right = y;
        }
        Y.parent = zp;
        // Swap the colors
        uint c = Y.color;
        Y.color = Z.color;
        Z.color = c;
        y = z;
    } else {
        /*
                p          p            p          p
               /          /              \          \
              z    -->   x                z  -->     x
              |                           |
              x                           x
         */
        p = Z.parent;
        if (x)
            X.parent = p;
        if (!p) {
            Q_ASSERT(head->root == z);
            head->root = x;
        } else if (P.left == z) {
            P.left = x;
            P.size_left -= Z.size;
            P.weight_left -= 1;
        } else {
            P.right = x;
        }
    }
    uint n = z;
    while (N.parent) {
        uint p = N.parent;
        if (P.left == n) {
            PMDEBUG("reducing size_left of %d by %d", N.parent, Z.size);
            P.size_left -= Z.size;
            P.weight_left -= 1;
        }
        n = p;
    }

    freeFragment(z);
    PMDEBUG("after removal");
    inorder();
    check();


    if (Y.color != Red) {
        while (X.parent && (x == 0 || X.color == Black)) {
            if (x == P.left) {
                uint w = P.right;
                if (W.color == Red) {
                    W.color = Black;
                    P.color = Red;
                    rotateLeft(p);
                    w = P.right;
                }
                if ((W.left == 0 || F(W.left).color == Black) &&
                    (W.right == 0 || F(W.right).color == Black)) {
                    W.color = Red;
                    x = p;
                    p = X.parent;
                } else {
                    if (W.right == 0 || F(W.right).color == Black) {
                        if (W.left)
                            F(W.left).color = Black;
                        W.color = Red;
                        rotateRight(P.right);
                        w = P.right;
                    }
                    W.color = P.color;
                    P.color = Black;
                    if (W.right)
                        F(W.right).color = Black;
                    rotateLeft(p);
                    break;
                }
            } else {
                uint w = P.left;
                if (W.color == Red) {
                    W.color = Black;
                    P.color = Red;
                    rotateRight(p);
                    w = P.left;
                }
                if ((W.right == 0 || F(W.right).color == Black) &&
                    (W.left == 0 || F(W.left).color == Black)) {
                    W.color = Red;
                    x = p;
                    p = X.parent;
                } else {
                    if (W.left == 0 || F(W.left).color == Black) {
                        if (W.right)
                            F(W.right).color = Black;
                        W.color = Red;
                        rotateLeft(P.left);
                        w = P.left;
                    }
                    W.color = P.color;
                    P.color = Black;
                    if (W.left)
                        F(W.left).color = Black;
                    rotateRight(p);
                    break;
                }
            }
        }
        if (x)
            X.color = Black;
    }

    PMDEBUG("after rebalance");
    inorder();
    check();

    return w;
}

uint QFragmentMapData::findNode(int k) const
{
    uint x = root();

    uint s = k;
    while (x) {
        if (sizeLeft(x) <= s) {
            if (s < sizeLeft(x) + size(x))
                return x;
            s -= sizeLeft(x) + size(x);
            x = X.right;
        } else {
            x = X.left;
        }
    }
    return 0;
}

uint QFragmentMapData::findNodeByIndex(int k) const
{
    uint x = root();

    uint s = k;
    while (x) {
        if (weightLeft(x) <= s) {
            if (s <= weightLeft(x))
                return x;
            s -= weightLeft(x) + 1;
            x = X.right;
        } else {
            x = X.left;
        }
    }
    return 0;
}

uint QFragmentMapData::insert_single(int key, uint length)
{
    Q_ASSERT(!findNode(key) || (int)this->position(findNode(key)) == key);

    uint z = createFragment();

    Z.size = length;
    Z.left = 0;
    Z.right = 0;
    Z.size_left = 0;
    Z.weight_left = 0;

    PMDEBUG("inserting with key %d", key);
    uint y = 0;
    uint x = root();

    Q_ASSERT(!x || X.parent == 0);

    uint s = key;
//     inorder();
    bool right = false;
    while (x) {
        y = x;
//          PMDEBUG("x=%p: x->size_left=%d, key=%d, s=%d", x, x->size_left, x->key(), s);
        if (s <= X.size_left) {
            x = X.left;
            right = false;
        } else {
            s -= X.size_left + X.size;
            x = X.right;
            right = true;
        }
    }
//     if (y)
//         PMDEBUG("  y=%p: y->size_left=%d, y->key=%d s=%d", y, y->size_left, y ? y->key() : -1, s);

    Z.parent = y;
    if (!y) {
        head->root = z;
    } else if (!right) {
//          PMDEBUG("inserting left");
        Y.left = z;
        Y.size_left = Z.size;
        Y.weight_left = 1;
    } else {
//          PMDEBUG("inserting right");
        Y.right = z;
    }
    while (y && Y.parent) {
        uint p = Y.parent;
        if (P.left == y) {
            P.size_left += Z.size;
            P.weight_left += 1;
        }
        y = p;
    }
//     PMDEBUG("before rebalance");
//     inorder();
    rebalance(z);

    inorder();
    PMDEBUG("end insert\n");

    return z;
}


int QFragmentMapData::length() const {
    uint root = this->root();
    return root ? sizeLeft(root) + size(root) + sizeRight(root) : 0;
}


QT_END_NAMESPACE
