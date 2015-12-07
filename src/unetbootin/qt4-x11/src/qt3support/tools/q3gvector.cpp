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

#include "qglobal.h"
#if defined(Q_CC_BOR)
// needed for qsort() because of a std namespace problem on Borland
#include "qplatformdefs.h"
#endif

#define	 Q3GVECTOR_CPP
#include "q3gvector.h"
#include "q3glist.h"
#include "qstring.h"
#include "qdatastream.h"
#include <stdlib.h>

#ifndef QT_NO_THREAD
#  include "private/qmutexpool_p.h"
#endif

QT_BEGIN_NAMESPACE

#define USE_MALLOC				// comment to use new/delete

#undef NEW
#undef DELETE

#if defined(USE_MALLOC)
#define NEW(type,size)	((type*)malloc(size*sizeof(type)))
#define DELETE(array)	(free((char*)array))
#else
#define NEW(type,size)	(new type[size])
#define DELETE(array)	(delete[] array)
#define DONT_USE_REALLOC			// comment to use realloc()
#endif

/*!
  \class Q3GVector
  \reentrant

  \brief The Q3GVector class is an internal class for implementing Qt
  collection classes.

  \internal

  Q3GVector is an internal class that acts as a base class for the
  Q3PtrVector collection class.

  Q3GVector has some virtual functions that may be reimplemented in
  subclasses to customize behavior.

  \list
  \i compareItems() compares two collection/vector items.
  \i read() reads a collection/vector item from a QDataStream.
  \i write() writes a collection/vector item to a QDataStream.
  \endlist
*/

/*****************************************************************************
  Default implementation of virtual functions
 *****************************************************************************/

/*!
  This virtual function compares two list items.

  Returns:
  <ul>
  <li> 0 if \a d1 == \a d2
  <li> non-zero if \a d1 != \a d2
  </ul>

  This function returns \e int rather than \e bool so that
  reimplementations can return one of three values and use it to sort
  by:
  <ul>
  <li> 0 if \a d1 == \a d2
  <li> \> 0 (positive integer) if \a d1 \> \a d2
  <li> \< 0 (negative integer) if \a d1 \< \a d2
  </ul>

  The Q3PtrVector::sort() and Q3PtrVector::bsearch() functions require that
  compareItems() is implemented as described here.

  This function should not modify the vector because some const
  functions call compareItems().
*/

int Q3GVector::compareItems( Item d1, Item d2 )
{
    return d1 != d2;				// compare pointers
}

#ifndef QT_NO_DATASTREAM
/*!
  Reads a collection/vector item from the stream \a s and returns a reference
  to the stream.

  The default implementation sets \a d to 0.

  \sa write()
*/

QDataStream &Q3GVector::read( QDataStream &s, Item &d )
{						// read item from stream
    d = 0;
    return s;
}

/*!
  Writes a collection/vector item to the stream \a s and returns a reference
  to the stream.

  The default implementation does nothing.

  \sa read()
*/

QDataStream &Q3GVector::write( QDataStream &s, Item ) const
{						// write item to stream
    return s;
}
#endif // QT_NO_DATASTREAM

/*****************************************************************************
  Q3GVector member functions
 *****************************************************************************/

Q3GVector::Q3GVector()				// create empty vector
{
    vec = 0;
    len = numItems = 0;
}

Q3GVector::Q3GVector( uint size )			// create vectors with nullptrs
{
    len = size;
    numItems = 0;
    if ( len == 0 ) {				// zero length
	vec = 0;
	return;
    }
    vec = NEW(Item,len);
    Q_CHECK_PTR( vec );
    memset( (void*)vec, 0, len*sizeof(Item) );	// fill with nulls
}

Q3GVector::Q3GVector( const Q3GVector &a )		// make copy of other vector
    : Q3PtrCollection( a )
{
    len = a.len;
    numItems = a.numItems;
    if ( len == 0 ) {
	vec = 0;
	return;
    }
    vec = NEW( Item, len );
    Q_CHECK_PTR( vec );
    for ( uint i = 0; i < len; i++ ) {
	if ( a.vec[i] ) {
	    vec[i] = newItem( a.vec[i] );
	    Q_CHECK_PTR( vec[i] );
	} else {
	    vec[i] = 0;
	}
    }
}

Q3GVector::~Q3GVector()
{
    clear();
}

Q3GVector& Q3GVector::operator=( const Q3GVector &v )
{
    if ( &v == this )
	return *this;

    clear();
    len = v.len;
    numItems = v.numItems;
    if ( len == 0 ) {
	vec = 0;
	return *this;
    }
    vec = NEW( Item, len );
    Q_CHECK_PTR( vec );
    for ( uint i = 0; i < len; i++ ) {
	if ( v.vec[i] ) {
	    vec[i] = newItem( v.vec[i] );
	    Q_CHECK_PTR( vec[i] );
	} else {
	    vec[i] = 0;
	}
    }
    return *this;
}


bool Q3GVector::insert( uint index, Item d )	// insert item at index
{
#if defined(QT_CHECK_RANGE)
    if ( index >= len ) {			// range error
	qWarning( "Q3GVector::insert: Index %d out of range", index );
	return false;
    }
#endif
    if ( vec[index] ) {				// remove old item
	deleteItem( vec[index] );
	numItems--;
    }
    if ( d ) {
	vec[index] = newItem( d );
	Q_CHECK_PTR( vec[index] );
	numItems++;
	return vec[index] != 0;
    } else {
	vec[index] = 0;				// reset item
    }
    return true;
}

bool Q3GVector::remove( uint index )		// remove item at index
{
#if defined(QT_CHECK_RANGE)
    if ( index >= len ) {			// range error
	qWarning( "Q3GVector::remove: Index %d out of range", index );
	return false;
    }
#endif
    if ( vec[index] ) {				// valid item
	deleteItem( vec[index] );		// delete it
	vec[index] = 0;				// reset pointer
	numItems--;
    }
    return true;
}

Q3PtrCollection::Item Q3GVector::take( uint index )		// take out item
{
#if defined(QT_CHECK_RANGE)
    if ( index >= len ) {			// range error
	qWarning( "Q3GVector::take: Index %d out of range", index );
	return 0;
    }
#endif
    Item d = vec[index];				// don't delete item
    if ( d )
	numItems--;
    vec[index] = 0;
    return d;
}

void Q3GVector::clear()				// clear vector
{
    if ( vec ) {
	for ( uint i=0; i<len; i++ ) {		// delete each item
	    if ( vec[i] )
		deleteItem( vec[i] );
	}
	DELETE(vec);
	vec = 0;
	len = numItems = 0;
    }
}

bool Q3GVector::resize( uint newsize )		// resize array
{
    if ( newsize == len )			// nothing to do
	return true;
    if ( vec ) {				// existing data
	if ( newsize < len ) {			// shrink vector
	    uint i = newsize;
	    while ( i < len ) {			// delete lost items
		if ( vec[i] ) {
		    deleteItem( vec[i] );
		    numItems--;
		}
		i++;
	    }
	}
	if ( newsize == 0 ) {			// vector becomes empty
	    DELETE(vec);
	    vec = 0;
	    len = numItems = 0;
	    return true;
	}
#if defined(DONT_USE_REALLOC)
	if ( newsize == 0 ) {
	    DELETE(vec);
	    vec = 0;
	    return false;
	}
	Item *newvec = NEW(Item,newsize);		// manual realloc
	memcpy( newvec, vec, (len < newsize ? len : newsize)*sizeof(Item) );
	DELETE(vec);
	vec = newvec;
#else
	vec = (Item*)realloc( (char *)vec, newsize*sizeof(Item) );
#endif
    } else {					// create new vector
	vec = NEW(Item,newsize);
	len = numItems = 0;
    }
    Q_CHECK_PTR( vec );
    if ( !vec )					// no memory
	return false;
    if ( newsize > len )			// init extra space added
	memset( (void*)&vec[len], 0, (newsize-len)*sizeof(Item) );
    len = newsize;
    return true;
}


bool Q3GVector::fill( Item d, int flen )		// resize and fill vector
{
    if ( flen < 0 )
	flen = len;				// default: use vector length
    else if ( !resize( flen ) )
	return false;
    for ( uint i=0; i<(uint)flen; i++ )		// insert d at every index
	insert( i, d );
    return true;
}


static Q3GVector *sort_vec=0;			// current sort vector


#if defined(Q_C_CALLBACKS)
extern "C" {
#endif

#ifdef Q_OS_WINCE
static int _cdecl cmp_vec( const void *n1, const void *n2 )
#else
static int cmp_vec( const void *n1, const void *n2 )
#endif
{
    return sort_vec->compareItems( *((Q3PtrCollection::Item*)n1), *((Q3PtrCollection::Item*)n2) );
}

#if defined(Q_C_CALLBACKS)
}
#endif


void Q3GVector::sort()				// sort vector
{
    if ( count() == 0 )				// no elements
	return;
    register Item *start = &vec[0];
    register Item *end	= &vec[len-1];
    Item tmp;
    for (;;) {				// put all zero elements behind
	while ( start < end && *start != 0 )
	    start++;
	while ( end > start && *end == 0 )
	    end--;
	if ( start < end ) {
	    tmp = *start;
	    *start = *end;
	    *end = tmp;
	} else {
	    break;
	}
    }

#ifndef QT_NO_THREAD
    QMutexLocker locker(QMutexPool::globalInstanceGet(&sort_vec));
#endif

    sort_vec = (Q3GVector*)this;
    qsort( vec, count(), sizeof(Item), cmp_vec );
    sort_vec = 0;
}

int Q3GVector::bsearch( Item d ) const		// binary search; when sorted
{
    if ( !len )
	return -1;
    if ( !d ) {
#if defined(QT_CHECK_NULL)
	qWarning( "Q3GVector::bsearch: Cannot search for null object" );
#endif
	return -1;
    }
    int n1 = 0;
    int n2 = len - 1;
    int mid = 0;
    bool found = false;
    while ( n1 <= n2 ) {
	int  res;
	mid = (n1 + n2)/2;
	if ( vec[mid] == 0 )			// null item greater
	    res = -1;
	else
	    res = ((Q3GVector*)this)->compareItems( d, vec[mid] );
	if ( res < 0 )
	    n2 = mid - 1;
	else if ( res > 0 )
	    n1 = mid + 1;
	else {					// found it
	    found = true;
	    break;
	}
    }
    if ( !found )
	return -1;
    // search to first of equal items
    while ( (mid - 1 >= 0) && !((Q3GVector*)this)->compareItems(d, vec[mid-1]) )
	mid--;
    return mid;
}

int Q3GVector::findRef( Item d, uint index) const // find exact item in vector
{
#if defined(QT_CHECK_RANGE)
    if ( index > len ) {			// range error
	qWarning( "Q3GVector::findRef: Index %d out of range", index );
	return -1;
    }
#endif
    for ( uint i=index; i<len; i++ ) {
	if ( vec[i] == d )
	    return i;
    }
    return -1;
}

int Q3GVector::find( Item d, uint index ) const	// find equal item in vector
{
#if defined(QT_CHECK_RANGE)
    if ( index >= len ) {			// range error
	qWarning( "Q3GVector::find: Index %d out of range", index );
	return -1;
    }
#endif
    for ( uint i=index; i<len; i++ ) {
	if ( vec[i] == 0 && d == 0 )		// found null item
	    return i;
	if ( vec[i] && ((Q3GVector*)this)->compareItems( vec[i], d ) == 0 )
	    return i;
    }
    return -1;
}

uint Q3GVector::containsRef( Item d ) const	// get number of exact matches
{
    uint count = 0;
    for ( uint i=0; i<len; i++ ) {
	if ( vec[i] == d )
	    count++;
    }
    return count;
}

uint Q3GVector::contains( Item d ) const		// get number of equal matches
{
    uint count = 0;
    for ( uint i=0; i<len; i++ ) {
	if ( vec[i] == 0 && d == 0 )		// count null items
	    count++;
	if ( vec[i] && ((Q3GVector*)this)->compareItems( vec[i], d ) == 0 )
	    count++;
    }
    return count;
}

bool Q3GVector::insertExpand( uint index, Item d )// insert and grow if necessary
{
    if ( index >= len ) {
	if ( !resize( index+1 ) )		// no memory
	    return false;
    }
    insert( index, d );
    return true;
}

void Q3GVector::toList( Q3GList *list ) const	// store items in list
{
    list->clear();
    for ( uint i=0; i<len; i++ ) {
	if ( vec[i] )
	    list->append( vec[i] );
    }
}


void Q3GVector::warningIndexRange( uint i )
{
#if defined(QT_CHECK_RANGE)
    qWarning( "Q3GVector::operator[]: Index %d out of range", i );
#else
    Q_UNUSED( i )
#endif
}


/*****************************************************************************
  Q3GVector stream functions
 *****************************************************************************/
#ifndef QT_NO_DATASTREAM
QDataStream &operator>>( QDataStream &s, Q3GVector &vec )
{						// read vector
    return vec.read( s );
}

QDataStream &operator<<( QDataStream &s, const Q3GVector &vec )
{						// write vector
    return vec.write( s );
}

QDataStream &Q3GVector::read( QDataStream &s )	// read vector from stream
{
    uint num;
    s >> num;					// read number of items
    clear();					// clear vector
    resize( num );
    for (uint i=0; i<num; i++) {		// read all items
	Item d;
	read( s, d );
	Q_CHECK_PTR( d );
	if ( !d )				// no memory
	    break;
	vec[i] = d;
    }
    return s;
}

QDataStream &Q3GVector::write( QDataStream &s ) const
{						// write vector to stream
    uint num = count();
    s << num;					// number of items to write
    num = size();
    for (uint i=0; i<num; i++) {		// write non-null items
	if ( vec[i] )
	    write( s, vec[i] );
    }
    return s;
}

/* Returns whether v equals this vector or not */

bool Q3GVector::operator==( const Q3GVector &v ) const
{
    if ( size() != v.size() )
	return false;
    if ( count() != v.count() )
	return false;
    for ( int i = 0; i < (int)size(); ++i ) {
	if ( ( (Q3GVector*)this )->compareItems( at( i ), v.at( i ) ) != 0 )
	    return false;
    }
    return true;
}

#endif // QT_NO_DATASTREAM

QT_END_NAMESPACE
