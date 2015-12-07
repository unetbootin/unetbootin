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
