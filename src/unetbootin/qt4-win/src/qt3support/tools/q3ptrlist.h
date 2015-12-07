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

#ifndef Q3PTRLIST_H
#define Q3PTRLIST_H

#include <Qt3Support/q3glist.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3SupportLight)

template<class type>
class Q3PtrListStdIterator : public Q3GListStdIterator
{
public:
    inline Q3PtrListStdIterator( Q3LNode* n ): Q3GListStdIterator(n) {}
    type *operator*() { return node ? (type *)node->getData() : 0; }
    inline Q3PtrListStdIterator<type> operator++()
    { node = next(); return *this; }
    inline Q3PtrListStdIterator<type> operator++(int)
    { Q3LNode* n = node; node = next(); return Q3PtrListStdIterator<type>( n ); }
    inline bool operator==( const Q3PtrListStdIterator<type>& it ) const { return node == it.node; }
    inline bool operator!=( const Q3PtrListStdIterator<type>& it ) const { return node != it.node; }
};


template<class type>
class Q3PtrList
#ifdef qdoc
	: public Q3PtrCollection
#else
	: public Q3GList
#endif
{
public:

    Q3PtrList()				{}
    Q3PtrList( const Q3PtrList<type> &l ) : Q3GList(l) {}
    ~Q3PtrList()				{ clear(); }
    Q3PtrList<type> &operator=(const Q3PtrList<type> &l)
			{ return (Q3PtrList<type>&)Q3GList::operator=(l); }
    bool operator==( const Q3PtrList<type> &list ) const
    { return Q3GList::operator==( list ); }
    bool operator!=( const Q3PtrList<type> &list ) const
    { return !Q3GList::operator==( list ); }
    uint  count()   const		{ return Q3GList::count(); }
    bool  isEmpty() const		{ return Q3GList::count() == 0; }
    bool  insert( uint i, const type *d){ return Q3GList::insertAt(i,(Q3PtrCollection::Item)d); }
    void  inSort( const type *d )	{ Q3GList::inSort((Q3PtrCollection::Item)d); }
    void  prepend( const type *d )	{ Q3GList::insertAt(0,(Q3PtrCollection::Item)d); }
    void  append( const type *d )	{ Q3GList::append((Q3PtrCollection::Item)d); }
    bool  remove( uint i )		{ return Q3GList::removeAt(i); }
    bool  remove()			{ return Q3GList::remove((Q3PtrCollection::Item)0); }
    bool  remove( const type *d )	{ return Q3GList::remove((Q3PtrCollection::Item)d); }
    bool  removeRef( const type *d )	{ return Q3GList::removeRef((Q3PtrCollection::Item)d); }
    void  removeNode( Q3LNode *n )	{ Q3GList::removeNode(n); }
    bool  removeFirst()			{ return Q3GList::removeFirst(); }
    bool  removeLast()			{ return Q3GList::removeLast(); }
    type *take( uint i )		{ return (type *)Q3GList::takeAt(i); }
    type *take()			{ return (type *)Q3GList::take(); }
    type *takeNode( Q3LNode *n )		{ return (type *)Q3GList::takeNode(n); }
    void  clear()			{ Q3GList::clear(); }
    void  sort()			{ Q3GList::sort(); }
    int	  find( const type *d )		{ return Q3GList::find((Q3PtrCollection::Item)d); }
    int	  findNext( const type *d )	{ return Q3GList::find((Q3PtrCollection::Item)d,false); }
    int	  findRef( const type *d )	{ return Q3GList::findRef((Q3PtrCollection::Item)d); }
    int	  findNextRef( const type *d ){ return Q3GList::findRef((Q3PtrCollection::Item)d,false);}
    uint  contains( const type *d ) const { return Q3GList::contains((Q3PtrCollection::Item)d); }
    uint  containsRef( const type *d ) const
					{ return Q3GList::containsRef((Q3PtrCollection::Item)d); }
    bool replace( uint i, const type *d ) { return Q3GList::replaceAt( i, (Q3PtrCollection::Item)d ); }
    type *at( uint i )			{ return (type *)Q3GList::at(i); }
    int	  at() const			{ return Q3GList::at(); }
    type *current()  const		{ return (type *)Q3GList::get(); }
    Q3LNode *currentNode()  const	{ return Q3GList::currentNode(); }
    type *getFirst() const		{ return (type *)Q3GList::cfirst(); }
    type *getLast()  const		{ return (type *)Q3GList::clast(); }
    type *first()			{ return (type *)Q3GList::first(); }
    type *last()			{ return (type *)Q3GList::last(); }
    type *next()			{ return (type *)Q3GList::next(); }
    type *prev()			{ return (type *)Q3GList::prev(); }
    void  toVector( Q3GVector *vec )const{ Q3GList::toVector(vec); }


    // standard iterators
    typedef Q3PtrListStdIterator<type> Iterator;
    typedef Q3PtrListStdIterator<type> ConstIterator;
    inline Iterator begin() { return Q3GList::begin(); }
    inline ConstIterator begin() const { return Q3GList::begin(); }
    inline ConstIterator constBegin() const { return Q3GList::begin(); }
    inline Iterator end() { return Q3GList::end(); }
    inline ConstIterator end() const { return Q3GList::end(); }
    inline ConstIterator constEnd() const { return Q3GList::end(); }
    inline Iterator erase( Iterator it ) { return Q3GList::erase( it ); }
    // stl syntax compatibility
    typedef Iterator iterator;
    typedef ConstIterator const_iterator;


#ifdef qdoc
protected:
    virtual int compareItems( Q3PtrCollection::Item, Q3PtrCollection::Item );
    virtual QDataStream& read( QDataStream&, Q3PtrCollection::Item& );
    virtual QDataStream& write( QDataStream&, Q3PtrCollection::Item ) const;
#endif

private:
    void  deleteItem( Item d );
};

#if !defined(Q_BROKEN_TEMPLATE_SPECIALIZATION)
template<> inline void Q3PtrList<void>::deleteItem( Q3PtrCollection::Item )
{
}
#endif

template<class type> inline void Q3PtrList<type>::deleteItem( Q3PtrCollection::Item d )
{
    if ( del_item ) delete (type *)d;
}

template<class type>
class Q3PtrListIterator : public Q3GListIterator
{
public:
    Q3PtrListIterator(const Q3PtrList<type> &l) :Q3GListIterator((Q3GList &)l) {}
   ~Q3PtrListIterator()	      {}
    uint  count()   const     { return list->count(); }
    bool  isEmpty() const     { return list->count() == 0; }
    bool  atFirst() const     { return Q3GListIterator::atFirst(); }
    bool  atLast()  const     { return Q3GListIterator::atLast(); }
    type *toFirst()	      { return (type *)Q3GListIterator::toFirst(); }
    type *toLast()	      { return (type *)Q3GListIterator::toLast(); }
    operator type *() const   { return (type *)Q3GListIterator::get(); }
    type *operator*()         { return (type *)Q3GListIterator::get(); }

    // No good, since Q3PtrList<char> (ie. QStrList fails...
    //
    // MSVC++ gives warning
    // Sunpro C++ 4.1 gives error
    //    type *operator->()        { return (type *)Q3GListIterator::get(); }

    type *current()   const   { return (type *)Q3GListIterator::get(); }
    type *operator()()	      { return (type *)Q3GListIterator::operator()();}
    type *operator++()	      { return (type *)Q3GListIterator::operator++(); }
    type *operator+=(uint j)  { return (type *)Q3GListIterator::operator+=(j);}
    type *operator--()	      { return (type *)Q3GListIterator::operator--(); }
    type *operator-=(uint j)  { return (type *)Q3GListIterator::operator-=(j);}
    Q3PtrListIterator<type>& operator=(const Q3PtrListIterator<type>&it)
			      { Q3GListIterator::operator=(it); return *this; }
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // Q3PTRLIST_H
