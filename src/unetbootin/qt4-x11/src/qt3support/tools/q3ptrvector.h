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

#ifndef Q3PTRVECTOR_H
#define Q3PTRVECTOR_H

#include <Qt3Support/q3gvector.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3SupportLight)

template<class type>
class Q3PtrVector
#ifdef qdoc
	: public Q3PtrCollection
#else
	: public Q3GVector
#endif
{
public:
    Q3PtrVector()				{ }
    Q3PtrVector( uint size ) : Q3GVector(size) { }
    Q3PtrVector( const Q3PtrVector<type> &v ) : Q3GVector( v ) { }
    ~Q3PtrVector()				{ clear(); }
    Q3PtrVector<type> &operator=(const Q3PtrVector<type> &v)
			{ return (Q3PtrVector<type>&)Q3GVector::operator=(v); }
    bool operator==( const Q3PtrVector<type> &v ) const { return Q3GVector::operator==(v); }
    type **data()   const		{ return (type **)Q3GVector::data(); }
    uint  size()    const		{ return Q3GVector::size(); }
    uint  count()   const		{ return Q3GVector::count(); }
    bool  isEmpty() const		{ return Q3GVector::count() == 0; }
    bool  isNull()  const		{ return Q3GVector::size() == 0; }
    bool  resize( uint size )		{ return Q3GVector::resize(size); }
    bool  insert( uint i, const type *d){ return Q3GVector::insert(i,(Item)d); }
    bool  remove( uint i )		{ return Q3GVector::remove(i); }
    type *take( uint i )		{ return (type *)Q3GVector::take(i); }
    void  clear()			{ Q3GVector::clear(); }
    bool  fill( const type *d, int size=-1 )
					{ return Q3GVector::fill((Item)d,size);}
    void  sort()			{ Q3GVector::sort(); }
    int	  bsearch( const type *d ) const{ return Q3GVector::bsearch((Item)d); }
    int	  findRef( const type *d, uint i=0 ) const
					{ return Q3GVector::findRef((Item)d,i);}
    int	  find( const type *d, uint i= 0 ) const
					{ return Q3GVector::find((Item)d,i); }
    uint  containsRef( const type *d ) const
				{ return Q3GVector::containsRef((Item)d); }
    uint  contains( const type *d ) const
					{ return Q3GVector::contains((Item)d); }
    type *operator[]( int i ) const	{ return (type *)Q3GVector::at(i); }
    type *at( uint i ) const		{ return (type *)Q3GVector::at(i); }
    void  toList( Q3GList *list ) const	{ Q3GVector::toList(list); }

#ifdef qdoc
protected:
    virtual int compareItems( Q3PtrCollection::Item d1, Q3PtrCollection::Item d2 );
    virtual QDataStream& read( QDataStream &s, Q3PtrCollection::Item &d );
    virtual QDataStream& write( QDataStream &s, Q3PtrCollection::Item d ) const;
#endif

private:
    void  deleteItem( Item d );
};

#if !defined(Q_BROKEN_TEMPLATE_SPECIALIZATION)
template<> inline void Q3PtrVector<void>::deleteItem( Q3PtrCollection::Item )
{
}
#endif

template<class type> inline void Q3PtrVector<type>::deleteItem( Q3PtrCollection::Item d )
{
    if ( del_item ) delete (type *)d;
}

QT_END_NAMESPACE

QT_END_HEADER

#endif // Q3PTRVECTOR_H
