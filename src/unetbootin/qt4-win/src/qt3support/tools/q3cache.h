/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt3Support module of the Qt Toolkit.
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

#ifndef Q3CACHE_H
#define Q3CACHE_H

#include <Qt3Support/q3gcache.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3SupportLight)

template<class type>
class Q3Cache
#ifdef qdoc
        : public Q3PtrCollection
#else
        : public Q3GCache
#endif
{
public:
    Q3Cache(const Q3Cache<type> &c) : Q3GCache(c) {}
    Q3Cache(int maxCost=100, int size=17, bool caseSensitive=true)
        : Q3GCache(maxCost, size, StringKey, caseSensitive, false) {}
   ~Q3Cache()                           { clear(); }
    Q3Cache<type> &operator=(const Q3Cache<type> &c)
                        { return (Q3Cache<type>&)Q3GCache::operator=(c); }
    int   maxCost()   const             { return Q3GCache::maxCost(); }
    int   totalCost() const             { return Q3GCache::totalCost(); }
    void  setMaxCost(int m)           { Q3GCache::setMaxCost(m); }
    uint  count()     const             { return Q3GCache::count(); }
    uint  size()      const             { return Q3GCache::size(); }
    bool  isEmpty()   const             { return Q3GCache::count() == 0; }
    void  clear()                       { Q3GCache::clear(); }
    bool  insert(const QString &k, const type *d, int c=1, int p=0)
                        { return Q3GCache::insert_string(k,(Item)d,c,p);}
    bool  remove(const QString &k)
                        { return Q3GCache::remove_string(k); }
    type *take(const QString &k)
                        { return (type *)Q3GCache::take_string(k); }
    type *find(const QString &k, bool ref=true) const
                        { return (type *)Q3GCache::find_string(k,ref);}
    type *operator[](const QString &k) const
                        { return (type *)Q3GCache::find_string(k);}
    void  statistics() const          { Q3GCache::statistics(); }
private:
    void  deleteItem(Item d);
};

#if !defined(Q_BROKEN_TEMPLATE_SPECIALIZATION)
template<> inline void Q3Cache<void>::deleteItem(Q3PtrCollection::Item)
{
}
#endif

template<class type> inline void Q3Cache<type>::deleteItem(Q3PtrCollection::Item d)
{
    if (del_item) delete (type *)d;
}

template<class type>
class Q3CacheIterator : public Q3GCacheIterator
{
public:
    Q3CacheIterator(const Q3Cache<type> &c):Q3GCacheIterator((Q3GCache &)c) {}
    Q3CacheIterator(const Q3CacheIterator<type> &ci)
                                : Q3GCacheIterator((Q3GCacheIterator &)ci) {}
    Q3CacheIterator<type> &operator=(const Q3CacheIterator<type>&ci)
        { return (Q3CacheIterator<type>&)Q3GCacheIterator::operator=(ci); }
    uint  count()   const     { return Q3GCacheIterator::count(); }
    bool  isEmpty() const     { return Q3GCacheIterator::count() == 0; }
    bool  atFirst() const     { return Q3GCacheIterator::atFirst(); }
    bool  atLast()  const     { return Q3GCacheIterator::atLast(); }
    type *toFirst()           { return (type *)Q3GCacheIterator::toFirst(); }
    type *toLast()            { return (type *)Q3GCacheIterator::toLast(); }
    operator type *() const   { return (type *)Q3GCacheIterator::get(); }
    type *current()   const   { return (type *)Q3GCacheIterator::get(); }
    QString currentKey() const{ return Q3GCacheIterator::getKeyString(); }
    type *operator()()        { return (type *)Q3GCacheIterator::operator()();}
    type *operator++()        { return (type *)Q3GCacheIterator::operator++(); }
    type *operator+=(uint j)  { return (type *)Q3GCacheIterator::operator+=(j);}
    type *operator--()        { return (type *)Q3GCacheIterator::operator--(); }
    type *operator-=(uint j)  { return (type *)Q3GCacheIterator::operator-=(j);}
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // Q3CACHE_H
