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

#include "q3gcache.h"
#include "q3ptrlist.h"
#include "q3dict.h"
#include "qstring.h"

QT_BEGIN_NAMESPACE

/*!
  \class Q3GCache
  \reentrant
  \brief The Q3GCache class is an internal class for implementing Q3Cache
  template classes.

  \internal

  Q3GCache is a strictly internal class that acts as a base class for the
  \link collection.html collection classes\endlink Q3Cache and QIntCache.
*/


/*****************************************************************************
  Q3GCacheItem class (internal cache item)
 *****************************************************************************/

struct Q3CacheItem
{
    Q3CacheItem(void *k, Q3PtrCollection::Item d, int c, short p)
	: priority(p), skipPriority(p), cost(c), key(k), data(d), node(0) {}
    short	priority;
    short	skipPriority;
    int		cost;
    void       *key;
    Q3PtrCollection::Item data;
    Q3LNode     *node;
};


/*****************************************************************************
  Q3CList class (internal list of cache items)
 *****************************************************************************/

class Q3CList : private Q3PtrList<Q3CacheItem>
{
friend class Q3GCacheIterator;
friend class Q3CListIt;
public:
    Q3CList()	{}
   ~Q3CList();

    void	insert(Q3CacheItem *);		// insert according to priority
    void	insert(int, Q3CacheItem *);
    void	take(Q3CacheItem *);
    void	reference(Q3CacheItem *);

    void	setAutoDelete(bool del) { Q3PtrCollection::setAutoDelete(del); }

    bool	removeFirst()	{ return Q3PtrList<Q3CacheItem>::removeFirst(); }
    bool	removeLast()	{ return Q3PtrList<Q3CacheItem>::removeLast(); }

    Q3CacheItem *first()		{ return Q3PtrList<Q3CacheItem>::first(); }
    Q3CacheItem *last()		{ return Q3PtrList<Q3CacheItem>::last(); }
    Q3CacheItem *prev()		{ return Q3PtrList<Q3CacheItem>::prev(); }
    Q3CacheItem *next()		{ return Q3PtrList<Q3CacheItem>::next(); }

#if defined(QT_DEBUG)
    int		inserts;			// variables for statistics
    int		insertCosts;
    int		insertMisses;
    int		finds;
    int		hits;
    int		hitCosts;
    int		dumps;
    int		dumpCosts;
#endif
};


Q3CList::~Q3CList()
{
#if defined(QT_DEBUG)
    Q_ASSERT(count() == 0);
#endif
}


void Q3CList::insert(Q3CacheItem *ci)
{
    Q3CacheItem *item = first();
    while(item && item->skipPriority > ci->priority) {
	item->skipPriority--;
	item = next();
    }
    if (item)
	Q3PtrList<Q3CacheItem>::insert(at(), ci);
    else
	append(ci);
#if defined(QT_DEBUG)
    Q_ASSERT(ci->node == 0);
#endif
    ci->node = currentNode();
}

inline void Q3CList::insert(int i, Q3CacheItem *ci)
{
    Q3PtrList<Q3CacheItem>::insert(i, ci);
#if defined(QT_DEBUG)
    Q_ASSERT(ci->node == 0);
#endif
    ci->node = currentNode();
}


void Q3CList::take(Q3CacheItem *ci)
{
    if (ci) {
#if defined(QT_DEBUG)
	Q_ASSERT(ci->node != 0);
#endif
	takeNode(ci->node);
	ci->node = 0;
    }
}


inline void Q3CList::reference(Q3CacheItem *ci)
{
#if defined(QT_DEBUG)
    Q_ASSERT(ci != 0 && ci->node != 0);
#endif
    ci->skipPriority = ci->priority;
    relinkNode(ci->node);			// relink as first item
}


class Q3CListIt: public Q3PtrListIterator<Q3CacheItem>
{
public:
    Q3CListIt(const Q3CList *p): Q3PtrListIterator<Q3CacheItem>(*p) {}
    Q3CListIt(const Q3CListIt *p): Q3PtrListIterator<Q3CacheItem>(*p) {}
};


/*****************************************************************************
  Q3CDict class (internal dictionary of cache items)
 *****************************************************************************/

//
// Since we need to decide if the dictionary should use an int or const
// char * key (the "bool trivial" argument in the constructor below)
// we cannot use the macro/template dict, but inherit directly from Q3GDict.
//

class Q3CDict : public Q3GDict
{
public:
    Q3CDict(uint size, uint kt, bool caseSensitive, bool copyKeys)
	: Q3GDict(size, (KeyType)kt, caseSensitive, copyKeys) {}
    ~Q3CDict();

    void clear() { Q3GDict::clear(); }

    Q3CacheItem *find_string(const QString &key) const
	{ return (Q3CacheItem*)((Q3CDict*)this)->look_string(key, 0, 0); }
    Q3CacheItem *find_ascii(const char *key) const
	{ return (Q3CacheItem*)((Q3CDict*)this)->look_ascii(key, 0, 0); }
    Q3CacheItem *find_int(long key) const
	{ return (Q3CacheItem*)((Q3CDict*)this)->look_int(key, 0, 0); }

    Q3CacheItem *take_string(const QString &key)
	{ return (Q3CacheItem*)Q3GDict::take_string(key); }
    Q3CacheItem *take_ascii(const char *key)
	{ return (Q3CacheItem*)Q3GDict::take_ascii(key); }
    Q3CacheItem *take_int(long key)
	{ return (Q3CacheItem*)Q3GDict::take_int(key); }

    bool  insert_string(const QString &key, const Q3CacheItem *ci)
	{ return Q3GDict::look_string(key,(Item)ci,1)!=0;}
    bool  insert_ascii(const char *key, const Q3CacheItem *ci)
	{ return Q3GDict::look_ascii(key,(Item)ci,1)!=0;}
    bool  insert_int(long key, const Q3CacheItem *ci)
	{ return Q3GDict::look_int(key,(Item)ci,1)!=0;}

    bool  remove_string(Q3CacheItem *item)
	{ return Q3GDict::remove_string(*((QString*)(item->key)),item); }
    bool  remove_ascii(Q3CacheItem *item)
	{ return Q3GDict::remove_ascii((const char *)item->key,item); }
    bool  remove_int(Q3CacheItem *item)
	{ return Q3GDict::remove_int((long)item->key,item);}

    void  statistics()			{ Q3GDict::statistics(); }

private:
    void deleteItem(void *item)
	{ if (del_item) { Q3CacheItem *d = (Q3CacheItem*)item; delete d; } }
};

inline Q3CDict::~Q3CDict()
{
    clear();
}

/*****************************************************************************
  Q3GDict member functions
 *****************************************************************************/

/*!
  Constructs a cache.
  The maximum cost of the cache is given by \a maxCost and the size by \a
  size. The key type is \a kt which may be \c StringKey, \c AsciiKey,
  \c IntKey or \c PtrKey. The case-sensitivity of lookups is set with
  \a caseSensitive. Keys are copied if \a copyKeys is true.
*/

Q3GCache::Q3GCache(int maxCost, uint size, KeyType kt, bool caseSensitive,
		  bool copyKeys)
{
    keytype = kt;
    lruList = new Q3CList;
    Q_CHECK_PTR(lruList);
    lruList->setAutoDelete(true);
    copyk   = ((keytype == AsciiKey) && copyKeys);
    dict    = new Q3CDict(size, kt, caseSensitive, false);
    Q_CHECK_PTR(dict);
    mCost   = maxCost;
    tCost   = 0;
#if defined(QT_DEBUG)
    lruList->inserts	  = 0;
    lruList->insertCosts  = 0;
    lruList->insertMisses = 0;
    lruList->finds	  = 0;
    lruList->hits	  = 0;
    lruList->hitCosts	  = 0;
    lruList->dumps	  = 0;
    lruList->dumpCosts	  = 0;
#endif
}

/*!
  Cannot copy a cache.
*/

Q3GCache::Q3GCache(const Q3GCache &)
    : Q3PtrCollection()
{
#if defined(QT_CHECK_NULL)
    qFatal("Q3GCache::Q3GCache(Q3GCache &): Cannot copy a cache");
#endif
}

/*!
  Removes all items from the cache and destroys it.
*/

Q3GCache::~Q3GCache()
{
    clear();
    delete dict;
    delete lruList;
}

/*!
  Cannot assign a cache.
*/

Q3GCache &Q3GCache::operator=(const Q3GCache &)
{
#if defined(QT_CHECK_NULL)
    qFatal("Q3GCache::operator=: Cannot copy a cache");
#endif
    return *this;
}


/*!
  Returns the number of items in the cache.
*/

uint Q3GCache::count() const
{
    return dict->count();
}

/*!
  Returns the size of the hash array.
*/

uint Q3GCache::size() const
{
    return dict->size();
}

/*!
  \fn int Q3GCache::maxCost() const

  Returns the maximum cache cost.
*/

/*!
  \fn int Q3GCache::totalCost() const

  Returns the total cache cost.
*/

/*!
  Sets the maximum cache cost to \a maxCost.
*/

void Q3GCache::setMaxCost(int maxCost)
{
    if (maxCost < tCost) {
	if (!makeRoomFor(tCost - maxCost))	// remove excess cost
	    return;
    }
    mCost = maxCost;
}


/*!
    Inserts an item with data \a data into the cache using key \a key.
    The item has cost \a cost and priority \a priority.

  \warning If this function returns false, you must delete \a data
  yourself.  Additionally, be very careful about using \a data after
  calling this function, as any other insertions into the cache, from
  anywhere in the application, or within Qt itself, could cause the
  data to be discarded from the cache, and the pointer to become
  invalid.
*/

bool Q3GCache::insert_string(const QString &key, Q3PtrCollection::Item data,
			     int cost, int priority)
{
    if (tCost + cost > mCost) {
	if (!makeRoomFor(tCost + cost - mCost, priority)) {
#if defined(QT_DEBUG)
	    lruList->insertMisses++;
#endif
	    return false;
	}
    }
#if defined(QT_DEBUG)
    Q_ASSERT(keytype == StringKey);
    lruList->inserts++;
    lruList->insertCosts += cost;
#endif
    if (priority < -32768)
	priority = -32768;
    else if (priority > 32767)
	priority = 32677;
    Q3CacheItem *ci = new Q3CacheItem(new QString(key), newItem(data),
				     cost, (short)priority);
    Q_CHECK_PTR(ci);
    lruList->insert(0, ci);
    dict->insert_string(key, ci);
    tCost += cost;
    return true;
}

bool Q3GCache::insert_other(const char *key, Q3PtrCollection::Item data,
			    int cost, int priority)
{
    if (tCost + cost > mCost) {
	if (!makeRoomFor(tCost + cost - mCost, priority)) {
#if defined(QT_DEBUG)
	    lruList->insertMisses++;
#endif
	    return false;
	}
    }
#if defined(QT_DEBUG)
    Q_ASSERT(keytype != StringKey);
    lruList->inserts++;
    lruList->insertCosts += cost;
#endif
    if (keytype == AsciiKey && copyk)
	key = qstrdup(key);
    if (priority < -32768)
	priority = -32768;
    else if (priority > 32767)
	priority = 32677;
    Q3CacheItem *ci = new Q3CacheItem((void*)key, newItem(data), cost,
				     (short)priority);
    Q_CHECK_PTR(ci);
    lruList->insert(0, ci);
    if (keytype == AsciiKey)
	dict->insert_ascii(key, ci);
    else
	dict->insert_int((long)key, ci);
    tCost += cost;
    return true;
}


/*!
  Removes the item with key \a key from the cache. Returns true if the
  item was removed; otherwise returns false.
*/

bool Q3GCache::remove_string(const QString &key)
{
    Item d = take_string(key);
    if (d)
	deleteItem(d);
    return d != 0;
}

bool Q3GCache::remove_other(const char *key)
{
    Item d = take_other(key);
    if (d)
	deleteItem(d);
    return d != 0;
}


/*!
  Takes the item with key \a key out of the cache. The item is not
  deleted. If no item has this \a key 0 is returned.
*/

Q3PtrCollection::Item Q3GCache::take_string(const QString &key)
{
    Q3CacheItem *ci = dict->take_string(key);	// take from dict
    Item d;
    if (ci) {
	d = ci->data;
	tCost -= ci->cost;
	lruList->take(ci);			// take from list
	delete (QString*)ci->key;
	delete ci;
    } else {
	d = 0;
    }
    return d;
}

/*!
  Takes the item with key \a key out of the cache. The item is not
  deleted. If no item has this \a key 0 is returned.
*/

Q3PtrCollection::Item Q3GCache::take_other(const char *key)
{
    Q3CacheItem *ci;
    if (keytype == AsciiKey)
	ci = dict->take_ascii(key);
    else
	ci = dict->take_int((long)key);
    Item d;
    if (ci) {
	d = ci->data;
	tCost -= ci->cost;
	lruList->take(ci);			// take from list
	if (copyk)
	    delete [] (char *)ci->key;
	delete ci;
    } else {
	d = 0;
    }
    return d;
}


/*!
  Clears the cache.
*/

void Q3GCache::clear()
{
    Q3CacheItem *ci;
    while ((ci = lruList->first())) {
	switch (keytype) {
	    case StringKey:
		dict->remove_string(ci);
		delete (QString*)ci->key;
		break;
	    case AsciiKey:
		dict->remove_ascii(ci);
		if (copyk)
		    delete [] (char*)ci->key;
		break;
	    case IntKey:
		dict->remove_int(ci);
		break;
	    case PtrKey:			// unused
		break;
	}
	deleteItem(ci->data);			// delete data
	lruList->removeFirst();			// remove from list
    }
    tCost = 0;
}


/*!
  Finds an item for \a key in the cache and adds a reference if \a ref is true.
*/

Q3PtrCollection::Item Q3GCache::find_string(const QString &key, bool ref) const
{
    Q3CacheItem *ci = dict->find_string(key);
#if defined(QT_DEBUG)
    lruList->finds++;
#endif
    if (ci) {
#if defined(QT_DEBUG)
	lruList->hits++;
	lruList->hitCosts += ci->cost;
#endif
	if (ref)
	    lruList->reference(ci);
	return ci->data;
    }
    return 0;
}


/*!
  Finds an item for \a key in the cache and adds a reference if \a ref is true.
*/

Q3PtrCollection::Item Q3GCache::find_other(const char *key, bool ref) const
{
    Q3CacheItem *ci = keytype == AsciiKey ? dict->find_ascii(key)
					 : dict->find_int((long)key);
#if defined(QT_DEBUG)
    lruList->finds++;
#endif
    if (ci) {
#if defined(QT_DEBUG)
	lruList->hits++;
	lruList->hitCosts += ci->cost;
#endif
	if (ref)
	    lruList->reference(ci);
	return ci->data;
    }
    return 0;
}


/*!
  Allocates cache space for one or more items.
*/

bool Q3GCache::makeRoomFor(int cost, int priority)
{
    if (cost > mCost)				// cannot make room for more
	return false;				//   than maximum cost
    if (priority == -1)
	priority = 32767;
    register Q3CacheItem *ci = lruList->last();
    int cntCost = 0;
    int dumps	= 0;				// number of items to dump
    while (cntCost < cost && ci && ci->skipPriority <= priority) {
	cntCost += ci->cost;
	ci	 = lruList->prev();
	dumps++;
    }
    if (cntCost < cost)			// can enough cost be dumped?
	return false;				// no
#if defined(QT_DEBUG)
    Q_ASSERT(dumps > 0);
#endif
    while (dumps--) {
	ci = lruList->last();
#if defined(QT_DEBUG)
	lruList->dumps++;
	lruList->dumpCosts += ci->cost;
#endif
	switch (keytype) {
	    case StringKey:
		dict->remove_string(ci);
		delete (QString*)ci->key;
		break;
	    case AsciiKey:
		dict->remove_ascii(ci);
		if (copyk)
		    delete [] (char *)ci->key;
		break;
	    case IntKey:
		dict->remove_int(ci);
		break;
	    case PtrKey:			// unused
		break;
	}
	deleteItem(ci->data);			// delete data
	lruList->removeLast();			// remove from list
    }
    tCost -= cntCost;
    return true;
}


/*!
  Outputs debug statistics.
*/

void Q3GCache::statistics() const
{
#if defined(QT_DEBUG)
    QString line;
    line.fill(QLatin1Char('*'), 80);
    qDebug("%s", line.ascii());
    qDebug("CACHE STATISTICS:");
    qDebug("cache contains %d item%s, with a total cost of %d",
	   count(), count() != 1 ? "s" : "", tCost);
    qDebug("maximum cost is %d, cache is %d%% full.",
	   mCost, (200*tCost + mCost) / (mCost*2));
    qDebug("find() has been called %d time%s",
	   lruList->finds, lruList->finds != 1 ? "s" : "");
    qDebug("%d of these were hits, items found had a total cost of %d.",
	   lruList->hits,lruList->hitCosts);
    qDebug("%d item%s %s been inserted with a total cost of %d.",
	   lruList->inserts,lruList->inserts != 1 ? "s" : "",
	   lruList->inserts != 1 ? "have" : "has", lruList->insertCosts);
    qDebug("%d item%s %s too large or had too low priority to be inserted.",
	   lruList->insertMisses, lruList->insertMisses != 1 ? "s" : "",
	   lruList->insertMisses != 1 ? "were" : "was");
    qDebug("%d item%s %s been thrown away with a total cost of %d.",
	   lruList->dumps, lruList->dumps != 1 ? "s" : "",
	   lruList->dumps != 1 ? "have" : "has", lruList->dumpCosts);
    qDebug("Statistics from internal dictionary class:");
    dict->statistics();
    qDebug("%s", line.ascii());
#endif
}


/*****************************************************************************
  Q3GCacheIterator member functions
 *****************************************************************************/

/*!
  \class Q3GCacheIterator qgcache.h
  \reentrant
  \brief The Q3GCacheIterator class is an internal class for implementing Q3CacheIterator and
  QIntCacheIterator.

  \internal

  Q3GCacheIterator is a strictly internal class that does the heavy work for
  Q3CacheIterator and QIntCacheIterator.
*/

/*!
  Constructs an iterator that operates on the cache \a c.
*/

Q3GCacheIterator::Q3GCacheIterator(const Q3GCache &c)
{
    it = new Q3CListIt(c.lruList);
#if defined(QT_DEBUG)
    Q_ASSERT(it != 0);
#endif
}

/*!
  Constructs an iterator that operates on the same cache as \a ci.
*/

Q3GCacheIterator::Q3GCacheIterator(const Q3GCacheIterator &ci)
{
    it = new Q3CListIt(ci.it);
#if defined(QT_DEBUG)
    Q_ASSERT(it != 0);
#endif
}

/*!
  Destroys the iterator.
*/

Q3GCacheIterator::~Q3GCacheIterator()
{
    delete it;
}

/*!
  Assigns the iterator \a ci to this cache iterator.
*/

Q3GCacheIterator &Q3GCacheIterator::operator=(const Q3GCacheIterator &ci)
{
    *it = *ci.it;
    return *this;
}

/*!
  Returns the number of items in the cache.
*/

uint Q3GCacheIterator::count() const
{
    return it->count();
}

/*!
  Returns true if the iterator points to the first item.
*/

bool  Q3GCacheIterator::atFirst() const
{
    return it->atFirst();
}

/*!
  Returns true if the iterator points to the last item.
*/

bool Q3GCacheIterator::atLast() const
{
    return it->atLast();
}

/*!
  Sets the list iterator to point to the first item in the cache.
*/

Q3PtrCollection::Item Q3GCacheIterator::toFirst()
{
    Q3CacheItem *item = it->toFirst();
    return item ? item->data : 0;
}

/*!
  Sets the list iterator to point to the last item in the cache.
*/

Q3PtrCollection::Item Q3GCacheIterator::toLast()
{
    Q3CacheItem *item = it->toLast();
    return item ? item->data : 0;
}

/*!
  Returns the current item.
*/

Q3PtrCollection::Item Q3GCacheIterator::get() const
{
    Q3CacheItem *item = it->current();
    return item ? item->data : 0;
}

/*!
  Returns the key of the current item.
*/

QString Q3GCacheIterator::getKeyString() const
{
    Q3CacheItem *item = it->current();
    return item ? *((QString*)item->key) : QString();
}

/*!
  Returns the key of the current item, as a \0-terminated C string.
*/

const char *Q3GCacheIterator::getKeyAscii() const
{
    Q3CacheItem *item = it->current();
    return item ? (const char *)item->key : 0;
}

/*!
  Returns the key of the current item, as a long.
*/

long Q3GCacheIterator::getKeyInt() const
{
    Q3CacheItem *item = it->current();
    return item ? (long)item->key : 0;
}

/*!
  Moves to the next item (postfix).
*/

Q3PtrCollection::Item Q3GCacheIterator::operator()()
{
    Q3CacheItem *item = it->operator()();
    return item ? item->data : 0;
}

/*!
  Moves to the next item (prefix).
*/

Q3PtrCollection::Item Q3GCacheIterator::operator++()
{
    Q3CacheItem *item = it->operator++();
    return item ? item->data : 0;
}

/*!
  Moves \a jump positions forward.
*/

Q3PtrCollection::Item Q3GCacheIterator::operator+=(uint jump)
{
    Q3CacheItem *item = it->operator+=(jump);
    return item ? item->data : 0;
}

/*!
  Moves to the previous item (prefix).
*/

Q3PtrCollection::Item Q3GCacheIterator::operator--()
{
    Q3CacheItem *item = it->operator--();
    return item ? item->data : 0;
}

/*!
  Moves \a jump positions backward.
*/

Q3PtrCollection::Item Q3GCacheIterator::operator-=(uint jump)
{
    Q3CacheItem *item = it->operator-=(jump);
    return item ? item->data : 0;
}

QT_END_NAMESPACE
