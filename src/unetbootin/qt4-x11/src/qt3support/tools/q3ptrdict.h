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

#ifndef Q3PTRDICT_H
#define Q3PTRDICT_H

#include <Qt3Support/q3gdict.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3SupportLight)

template<class type>
class Q3PtrDict
#ifdef qdoc
	: public Q3PtrCollection
#else
	: public Q3GDict
#endif
{
public:
    Q3PtrDict(int size=17) : Q3GDict(size,PtrKey,0,0) {}
    Q3PtrDict( const Q3PtrDict<type> &d ) : Q3GDict(d) {}
   ~Q3PtrDict()				{ clear(); }
    Q3PtrDict<type> &operator=(const Q3PtrDict<type> &d)
			{ return (Q3PtrDict<type>&)Q3GDict::operator=(d); }
    uint  count()   const		{ return Q3GDict::count(); }
    uint  size()    const		{ return Q3GDict::size(); }
    bool  isEmpty() const		{ return Q3GDict::count() == 0; }
    void  insert( void *k, const type *d )
					{ Q3GDict::look_ptr(k,(Item)d,1); }
    void  replace( void *k, const type *d )
					{ Q3GDict::look_ptr(k,(Item)d,2); }
    bool  remove( void *k )		{ return Q3GDict::remove_ptr(k); }
    type *take( void *k )		{ return (type*)Q3GDict::take_ptr(k); }
    type *find( void *k ) const
		{ return (type *)((Q3GDict*)this)->Q3GDict::look_ptr(k,0,0); }
    type *operator[]( void *k ) const
		{ return (type *)((Q3GDict*)this)->Q3GDict::look_ptr(k,0,0); }
    void  clear()			{ Q3GDict::clear(); }
    void  resize( uint n )		{ Q3GDict::resize(n); }
    void  statistics() const		{ Q3GDict::statistics(); }

#ifdef qdoc
protected:
    virtual QDataStream& read( QDataStream &, Q3PtrCollection::Item & );
    virtual QDataStream& write( QDataStream &, Q3PtrCollection::Item ) const;
#endif

private:
    void  deleteItem( Item d );
};

#if !defined(Q_BROKEN_TEMPLATE_SPECIALIZATION)
template<> inline void Q3PtrDict<void>::deleteItem( Q3PtrCollection::Item )
{
}
#endif

template<class type>
inline void Q3PtrDict<type>::deleteItem( Q3PtrCollection::Item d )
{
    if ( del_item ) delete (type *)d;
}

template<class type>
class Q3PtrDictIterator : public Q3GDictIterator
{
public:
    Q3PtrDictIterator(const Q3PtrDict<type> &d) :Q3GDictIterator((Q3GDict &)d) {}
   ~Q3PtrDictIterator()	      {}
    uint  count()   const     { return dict->count(); }
    bool  isEmpty() const     { return dict->count() == 0; }
    type *toFirst()	      { return (type *)Q3GDictIterator::toFirst(); }
    operator type *()  const  { return (type *)Q3GDictIterator::get(); }
    type *current()    const  { return (type *)Q3GDictIterator::get(); }
    void *currentKey() const  { return Q3GDictIterator::getKeyPtr(); }
    type *operator()()	      { return (type *)Q3GDictIterator::operator()(); }
    type *operator++()	      { return (type *)Q3GDictIterator::operator++(); }
    type *operator+=(uint j)  { return (type *)Q3GDictIterator::operator+=(j);}
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // Q3PTRDICT_H
