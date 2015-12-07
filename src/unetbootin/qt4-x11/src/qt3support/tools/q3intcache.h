/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt3Support module of the Qt Toolkit.
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

#ifndef Q3INTCACHE_H
#define Q3INTCACHE_H

#include <Qt3Support/q3gcache.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3SupportLight)

template<class type>
class Q3IntCache
#ifdef qdoc
	: public Q3PtrCollection
#else
	: public Q3GCache
#endif
{
public:
    Q3IntCache( const Q3IntCache<type> &c ) : Q3GCache(c) {}
    Q3IntCache( int maxCost=100, int size=17 )
	: Q3GCache( maxCost, size, IntKey, false, false ) {}
   ~Q3IntCache()		{ clear(); }
    Q3IntCache<type> &operator=( const Q3IntCache<type> &c )
			{ return (Q3IntCache<type>&)Q3GCache::operator=(c); }
    int	  maxCost()   const	{ return Q3GCache::maxCost(); }
    int	  totalCost() const	{ return Q3GCache::totalCost(); }
    void  setMaxCost( int m)	{ Q3GCache::setMaxCost(m); }
    uint  count()     const	{ return Q3GCache::count(); }
    uint  size()      const	{ return Q3GCache::size(); }
    bool  isEmpty()   const	{ return Q3GCache::count() == 0; }
    bool  insert( long k, const type *d, int c=1, int p=0 )
		{ return Q3GCache::insert_other((const char*)k,(Item)d,c,p); }
    bool  remove( long k )
		{ return Q3GCache::remove_other((const char*)k); }
    type *take( long k )
		{ return (type *)Q3GCache::take_other((const char*)k);}
    void  clear()		{ Q3GCache::clear(); }
    type *find( long k, bool ref=true ) const
		{ return (type *)Q3GCache::find_other( (const char*)k,ref);}
    type *operator[]( long k ) const
		{ return (type *)Q3GCache::find_other( (const char*)k); }
    void  statistics() const { Q3GCache::statistics(); }
private:
	void  deleteItem( Item d );
};

#if !defined(Q_BROKEN_TEMPLATE_SPECIALIZATION)
template<> inline void Q3IntCache<void>::deleteItem( Q3PtrCollection::Item )
{
}
#endif

template<class type> inline void Q3IntCache<type>::deleteItem( Q3PtrCollection::Item d )
{
    if ( del_item ) delete (type *)d;
}

template<class type>
class Q3IntCacheIterator : public Q3GCacheIterator
{
public:
    Q3IntCacheIterator( const Q3IntCache<type> &c )
	: Q3GCacheIterator( (Q3GCache &)c ) {}
    Q3IntCacheIterator( const Q3IntCacheIterator<type> &ci )
			      : Q3GCacheIterator((Q3GCacheIterator &)ci) {}
    Q3IntCacheIterator<type> &operator=( const Q3IntCacheIterator<type>&ci )
	{ return ( Q3IntCacheIterator<type>&)Q3GCacheIterator::operator=( ci );}
    uint  count()   const     { return Q3GCacheIterator::count(); }
    bool  isEmpty() const     { return Q3GCacheIterator::count() == 0; }
    bool  atFirst() const     { return Q3GCacheIterator::atFirst(); }
    bool  atLast()  const     { return Q3GCacheIterator::atLast(); }
    type *toFirst()	      { return (type *)Q3GCacheIterator::toFirst(); }
    type *toLast()	      { return (type *)Q3GCacheIterator::toLast(); }
    operator type *()  const  { return (type *)Q3GCacheIterator::get(); }
    type *current()    const  { return (type *)Q3GCacheIterator::get(); }
    long  currentKey() const  { return (long)Q3GCacheIterator::getKeyInt();}
    type *operator()()	      { return (type *)Q3GCacheIterator::operator()();}
    type *operator++()	      { return (type *)Q3GCacheIterator::operator++(); }
    type *operator+=(uint j)  { return (type *)Q3GCacheIterator::operator+=(j);}
    type *operator--()	      { return (type *)Q3GCacheIterator::operator--(); }
    type *operator-=(uint j)  { return (type *)Q3GCacheIterator::operator-=(j);}
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // Q3INTCACHE_H
