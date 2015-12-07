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

#ifndef Q3GDICT_H
#define Q3GDICT_H

#include <Qt3Support/q3ptrcollection.h>
#include <QtCore/qstring.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3SupportLight)

class Q3GDictIterator;
class Q3GDItList;


class Q3BaseBucket				// internal dict node
{
public:
    Q3PtrCollection::Item	 getData()			{ return data; }
    Q3PtrCollection::Item	 setData( Q3PtrCollection::Item d ) { return data = d; }
    Q3BaseBucket		*getNext()			{ return next; }
    void		 setNext( Q3BaseBucket *n)	{ next = n; }
protected:
    Q3BaseBucket( Q3PtrCollection::Item d, Q3BaseBucket *n ) : data(d), next(n) {}
    Q3PtrCollection::Item	 data;
    Q3BaseBucket		*next;
};

class Q3StringBucket : public Q3BaseBucket
{
public:
    Q3StringBucket( const QString &k, Q3PtrCollection::Item d, Q3BaseBucket *n )
	: Q3BaseBucket(d,n), key(k)		{}
    const QString  &getKey() const		{ return key; }
private:
    QString	    key;
};

class Q3AsciiBucket : public Q3BaseBucket
{
public:
    Q3AsciiBucket( const char *k, Q3PtrCollection::Item d, Q3BaseBucket *n )
	: Q3BaseBucket(d,n), key(k) {}
    const char *getKey() const { return key; }
private:
    const char *key;
};

class Q3IntBucket : public Q3BaseBucket
{
public:
    Q3IntBucket( long k, Q3PtrCollection::Item d, Q3BaseBucket *n )
	: Q3BaseBucket(d,n), key(k) {}
    long  getKey() const { return key; }
private:
    long  key;
};

class Q3PtrBucket : public Q3BaseBucket
{
public:
    Q3PtrBucket( void *k, Q3PtrCollection::Item d, Q3BaseBucket *n )
	: Q3BaseBucket(d,n), key(k) {}
    void *getKey() const { return key; }
private:
    void *key;
};


class Q_COMPAT_EXPORT Q3GDict : public Q3PtrCollection	// generic dictionary class
{
public:
    uint	count() const	{ return numItems; }
    uint	size()	const	{ return vlen; }
    Q3PtrCollection::Item look_string( const QString& key, Q3PtrCollection::Item,
				   int );
    Q3PtrCollection::Item look_ascii( const char *key, Q3PtrCollection::Item, int );
    Q3PtrCollection::Item look_int( long key, Q3PtrCollection::Item, int );
    Q3PtrCollection::Item look_ptr( void *key, Q3PtrCollection::Item, int );
#ifndef QT_NO_DATASTREAM
    QDataStream &read( QDataStream & );
    QDataStream &write( QDataStream & ) const;
#endif
protected:
    enum KeyType { StringKey, AsciiKey, IntKey, PtrKey };

    Q3GDict( uint len, KeyType kt, bool cs, bool ck );
    Q3GDict( const Q3GDict & );
   ~Q3GDict();

    Q3GDict     &operator=( const Q3GDict & );

    bool	remove_string( const QString &key, Q3PtrCollection::Item item=0 );
    bool	remove_ascii( const char *key, Q3PtrCollection::Item item=0 );
    bool	remove_int( long key, Q3PtrCollection::Item item=0 );
    bool	remove_ptr( void *key, Q3PtrCollection::Item item=0 );
    Q3PtrCollection::Item take_string( const QString &key );
    Q3PtrCollection::Item take_ascii( const char *key );
    Q3PtrCollection::Item take_int( long key );
    Q3PtrCollection::Item take_ptr( void *key );

    void	clear();
    void	resize( uint );

    int		hashKeyString( const QString & );
    int		hashKeyAscii( const char * );

    void	statistics() const;

#ifndef QT_NO_DATASTREAM
    virtual QDataStream &read( QDataStream &, Q3PtrCollection::Item & );
    virtual QDataStream &write( QDataStream &, Q3PtrCollection::Item ) const;
#endif
private:
    Q3BaseBucket **vec;
    uint	vlen;
    uint	numItems;
    uint	keytype	: 2;
    uint	cases	: 1;
    uint	copyk	: 1;
    Q3GDItList  *iterators;
    void	   unlink_common( int, Q3BaseBucket *, Q3BaseBucket * );
    Q3StringBucket *unlink_string( const QString &,
				  Q3PtrCollection::Item item = 0 );
    Q3AsciiBucket  *unlink_ascii( const char *, Q3PtrCollection::Item item = 0 );
    Q3IntBucket    *unlink_int( long, Q3PtrCollection::Item item = 0 );
    Q3PtrBucket    *unlink_ptr( void *, Q3PtrCollection::Item item = 0 );
    void	init( uint, KeyType, bool, bool );
    friend class Q3GDictIterator;
};


class Q_COMPAT_EXPORT Q3GDictIterator			// generic dictionary iterator
{
friend class Q3GDict;
public:
    Q3GDictIterator( const Q3GDict & );
    Q3GDictIterator( const Q3GDictIterator & );
    Q3GDictIterator &operator=( const Q3GDictIterator & );
   ~Q3GDictIterator();

    Q3PtrCollection::Item toFirst();

    Q3PtrCollection::Item get()	     const;
    QString	      getKeyString() const;
    const char	     *getKeyAscii()  const;
    long	      getKeyInt()    const;
    void	     *getKeyPtr()    const;

    Q3PtrCollection::Item operator()();
    Q3PtrCollection::Item operator++();
    Q3PtrCollection::Item operator+=(uint);

protected:
    Q3GDict	     *dict;

private:
    Q3BaseBucket      *curNode;
    uint	      curIndex;
};

inline Q3PtrCollection::Item Q3GDictIterator::get() const
{
    return curNode ? curNode->getData() : 0;
}

inline QString Q3GDictIterator::getKeyString() const
{
    return curNode ? ((Q3StringBucket*)curNode)->getKey() : QString();
}

inline const char *Q3GDictIterator::getKeyAscii() const
{
    return curNode ? ((Q3AsciiBucket*)curNode)->getKey() : 0;
}

inline long Q3GDictIterator::getKeyInt() const
{
    return curNode ? ((Q3IntBucket*)curNode)->getKey() : 0;
}

inline void *Q3GDictIterator::getKeyPtr() const
{
    return curNode ? ((Q3PtrBucket*)curNode)->getKey() : 0;
}

QT_END_NAMESPACE

QT_END_HEADER

#endif // Q3GDICT_H
