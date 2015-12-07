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

#ifndef Q3GVECTOR_H
#define Q3GVECTOR_H

#include <Qt3Support/q3ptrcollection.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3SupportLight)

class Q_COMPAT_EXPORT Q3GVector : public Q3PtrCollection	// generic vector
{
friend class Q3GList;				// needed by Q3GList::toVector
public:
#ifndef QT_NO_DATASTREAM
    QDataStream &read( QDataStream & );		// read vector from stream
    QDataStream &write( QDataStream & ) const;	// write vector to stream
#endif
    virtual int compareItems( Item, Item );

protected:
    Q3GVector();					// create empty vector
    Q3GVector( uint size );			// create vector with nullptrs
    Q3GVector( const Q3GVector &v );		// make copy of other vector
   ~Q3GVector();

    Q3GVector &operator=( const Q3GVector &v );	// assign from other vector
    bool operator==( const Q3GVector &v ) const;

    Item	 *data()    const	{ return vec; }
    uint  size()    const	{ return len; }
    uint  count()   const	{ return numItems; }

    bool  insert( uint index, Item );		// insert item at index
    bool  remove( uint index );			// remove item
    Item	  take( uint index );			// take out item

    void  clear();				// clear vector
    bool  resize( uint newsize );		// resize vector

    bool  fill( Item, int flen );		// resize and fill vector

    void  sort();				// sort vector
    int	  bsearch( Item ) const;			// binary search (when sorted)

    int	  findRef( Item, uint index ) const;	// find exact item in vector
    int	  find( Item, uint index ) const;	// find equal item in vector
    uint  containsRef( Item ) const;		// get number of exact matches
    uint  contains( Item ) const;		// get number of equal matches

    Item	  at( uint index ) const		// return indexed item
    {
#if defined(QT_CHECK_RANGE)
	if ( index >= len )
	    warningIndexRange( index );
#endif
	return vec[index];
    }

    bool insertExpand( uint index, Item );	// insert, expand if necessary

    void toList( Q3GList * ) const;		// put items in list

#ifndef QT_NO_DATASTREAM
    virtual QDataStream &read( QDataStream &, Item & );
    virtual QDataStream &write( QDataStream &, Item ) const;
#endif
private:
    Item	 *vec;
    uint  len;
    uint  numItems;

    static void warningIndexRange( uint );
};


/*****************************************************************************
  Q3GVector stream functions
 *****************************************************************************/

#ifndef QT_NO_DATASTREAM
Q_COMPAT_EXPORT QDataStream &operator>>( QDataStream &, Q3GVector & );
Q_COMPAT_EXPORT QDataStream &operator<<( QDataStream &, const Q3GVector & );
#endif

QT_END_NAMESPACE

QT_END_HEADER

#endif // Q3GVECTOR_H
