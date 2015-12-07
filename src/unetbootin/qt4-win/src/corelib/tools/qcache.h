/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#ifndef QCACHE_H
#define QCACHE_H

#include <QtCore/qhash.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Core)

template <class Key, class T>
class QCache
{
    struct Node {
        inline Node() : keyPtr(0) {}
        inline Node(T *data, int cost)
            : keyPtr(0), t(data), c(cost), p(0), n(0) {}
        const Key *keyPtr; T *t; int c; Node *p,*n;
    };
    Node *f, *l;
    QHash<Key, Node> hash;
    void *unused;
    int mx, total;

    inline void unlink(Node &n) {
        if (n.p) n.p->n = n.n;
        if (n.n) n.n->p = n.p;
        if (l == &n) l = n.p;
        if (f == &n) f = n.n;
        total -= n.c;
        delete n.t;
        hash.remove(*n.keyPtr);
    }
    inline T *relink(const Key &key) {
        typename QHash<Key, Node>::iterator i = hash.find(key);
        if (typename QHash<Key, Node>::const_iterator(i) == hash.constEnd())
            return 0;

        Node &n = *i;
        if (f != &n) {
            if (n.p) n.p->n = n.n;
            if (n.n) n.n->p = n.p;
            if (l == &n) l = n.p;
            n.p = 0;
            n.n = f;
            f->p = &n;
            f = &n;
        }
        return n.t;
    }

    Q_DISABLE_COPY(QCache)

public:
    inline explicit QCache(int maxCost = 100);
#ifdef QT3_SUPPORT
    inline QT3_SUPPORT_CONSTRUCTOR QCache(int maxCost, int /* dummy */)
        : f(0), l(0), mx(maxCost), total(0) {}
#endif
    inline ~QCache() { clear(); }

    inline int maxCost() const { return mx; }
    void setMaxCost(int m);
    inline int totalCost() const { return total; }

    inline int size() const { return hash.size(); }
    inline int count() const { return hash.size(); }
    inline bool isEmpty() const { return hash.isEmpty(); }
    inline QList<Key> keys() const { return hash.keys(); }

    void clear();

    bool insert(const Key &key, T *object, int cost = 1);
    T *object(const Key &key) const;
    inline bool contains(const Key &key) const { return hash.contains(key); }
    T *operator[](const Key &key) const;

    bool remove(const Key &key);
    T *take(const Key &key);

private:
    void trim(int m);

#ifdef QT3_SUPPORT
    inline QT3_SUPPORT T *find(const Key &key) const { return object(key); }
#endif

};

template <class Key, class T>
inline QCache<Key, T>::QCache(int amaxCost)
    : f(0), l(0), mx(amaxCost), total(0) {}

template <class Key, class T>
inline void QCache<Key,T>::clear()
{ while (f) { delete f->t; f = f->n; }
 hash.clear(); l = 0; total = 0; }

template <class Key, class T>
inline void QCache<Key,T>::setMaxCost(int m)
{ mx = m; trim(mx); }

template <class Key, class T>
inline T *QCache<Key,T>::object(const Key &key) const
{ return const_cast<QCache<Key,T>*>(this)->relink(key); }

template <class Key, class T>
inline T *QCache<Key,T>::operator[](const Key &key) const
{ return object(key); }

template <class Key, class T>
inline bool QCache<Key,T>::remove(const Key &key)
{
    typename QHash<Key, Node>::iterator i = hash.find(key);
    if (typename QHash<Key, Node>::const_iterator(i) == hash.constEnd()) {
        return false;
    } else {
        unlink(*i);
        return true;
    }
}

template <class Key, class T>
inline T *QCache<Key,T>::take(const Key &key)
{
    typename QHash<Key, Node>::iterator i = hash.find(key);
    if (i == hash.end())
        return 0;

    Node &n = *i;
    T *t = n.t;
    n.t = 0;
    unlink(n);
    return t;
}

template <class Key, class T>
bool QCache<Key,T>::insert(const Key &akey, T *aobject, int acost)
{
    remove(akey);
    if (acost > mx) {
        delete aobject;
        return false;
    }
    trim(mx - acost);
    Node sn(aobject, acost);
    typename QHash<Key, Node>::iterator i = hash.insert(akey, sn);
    total += acost;
    Node *n = &i.value();
    n->keyPtr = &i.key();
    if (f) f->p = n;
    n->n = f;
    f = n;
    if (!l) l = f;
    return true;
}

template <class Key, class T>
void QCache<Key,T>::trim(int m)
{
    Node *n = l;
    while (n && total > m) {
        Node *u = n;
        n = n->p;
        if (qIsDetached(*u->t))
            unlink(*u);
    }
}

QT_END_NAMESPACE

QT_END_HEADER

#endif // QCACHE_H
