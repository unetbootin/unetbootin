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

#ifndef Q3GCACHE_H
#define Q3GCACHE_H

#include <Qt3Support/q3ptrcollection.h>
#include <Qt3Support/q3glist.h>
#include <Qt3Support/q3gdict.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3SupportLight)

class Q3CList;					// internal classes
class Q3CListIt;
class Q3CDict;

class Q_COMPAT_EXPORT Q3GCache : public Q3PtrCollection	// generic LRU cache
{
friend class Q3GCacheIterator;
protected:
    enum KeyType { StringKey, AsciiKey, IntKey, PtrKey };
      // identical to Q3GDict's, but PtrKey is not used at the moment

    Q3GCache(int maxCost, uint size, KeyType kt, bool caseSensitive,
	     bool copyKeys);
    Q3GCache(const Q3GCache &);			// not allowed, calls fatal()
   ~Q3GCache();
    Q3GCache &operator=(const Q3GCache &);	// not allowed, calls fatal()

    uint    count()	const;
    uint    size()	const;
    int	    maxCost()	const	{ return mCost; }
    int	    totalCost() const	{ return tCost; }
    void    setMaxCost(int maxCost);
    void    clear();

    bool    insert_string(const QString &key, Q3PtrCollection::Item,
			   int cost, int priority);
    bool    insert_other(const char *key, Q3PtrCollection::Item,
			  int cost, int priority);
    bool    remove_string(const QString &key);
    bool    remove_other(const char *key);
    Q3PtrCollection::Item take_string(const QString &key);
    Q3PtrCollection::Item take_other(const char *key);

    Q3PtrCollection::Item find_string(const QString &key, bool ref=true) const;
    Q3PtrCollection::Item find_other(const char *key, bool ref=true) const;

    void    statistics() const;

private:
    bool    makeRoomFor(int cost, int priority = -1);
    KeyType keytype;
    Q3CList *lruList;
    Q3CDict *dict;
    int	    mCost;
    int	    tCost;
    bool    copyk;
};


class Q_COMPAT_EXPORT Q3GCacheIterator			// generic cache iterator
{
protected:
    Q3GCacheIterator(const Q3GCache &);
    Q3GCacheIterator(const Q3GCacheIterator &);
   ~Q3GCacheIterator();
    Q3GCacheIterator &operator=(const Q3GCacheIterator &);

    uint	      count()   const;
    bool	      atFirst() const;
    bool	      atLast()  const;
    Q3PtrCollection::Item toFirst();
    Q3PtrCollection::Item toLast();

    Q3PtrCollection::Item get() const;
    QString	      getKeyString() const;
    const char       *getKeyAscii()  const;
    long	      getKeyInt()    const;

    Q3PtrCollection::Item operator()();
    Q3PtrCollection::Item operator++();
    Q3PtrCollection::Item operator+=(uint);
    Q3PtrCollection::Item operator--();
    Q3PtrCollection::Item operator-=(uint);

protected:
    Q3CListIt *it;				// iterator on cache list
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // Q3GCACHE_H
