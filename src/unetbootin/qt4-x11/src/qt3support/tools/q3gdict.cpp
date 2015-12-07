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

#include "q3gdict.h"
#include "q3ptrlist.h"
#include "qstring.h"
#include "qdatastream.h"
#include <ctype.h>

QT_BEGIN_NAMESPACE

/*!
  \class Q3GDict
  \reentrant
  \brief The Q3GDict class is an internal class for implementing QDict template classes.

  \internal

  Q3GDict is a strictly internal class that acts as a base class for the
  \link collection.html collection classes\endlink QDict and QIntDict.

  Q3GDict has some virtual functions that can be reimplemented to customize
  the subclasses.
  \list
  \i read() reads a collection/dictionary item from a QDataStream.
  \i write() writes a collection/dictionary item to a QDataStream.
  \endlist
  Normally, you do not have to reimplement any of these functions.
*/

static const int op_find = 0;
static const int op_insert = 1;
static const int op_replace = 2;


class Q3GDItList : public Q3PtrList<Q3GDictIterator>
{
public:
    Q3GDItList() : Q3PtrList<Q3GDictIterator>() {}
    Q3GDItList(const Q3GDItList &list) : Q3PtrList<Q3GDictIterator>(list) {}
    ~Q3GDItList() { clear(); }
    Q3GDItList &operator=(const Q3GDItList &list)
        { return (Q3GDItList&)Q3PtrList<Q3GDictIterator>::operator=(list); }
};


/*****************************************************************************
  Default implementation of special and virtual functions
 *****************************************************************************/

/*!
  Returns the hash key for \a key, when key is a string.
*/

int Q3GDict::hashKeyString(const QString &key)
{
#if defined(QT_CHECK_NULL)
    if (key.isNull())
        qWarning("Q3GDict::hashKeyString: Invalid null key");
#endif
    int i;
    register uint h=0;
    uint g;
    const QChar *p = key.unicode();
    if (cases) {                                // case sensitive
        for (i=0; i<(int)key.length(); i++) {
            h = (h<<4) + p[i].cell();
            if ((g = h & 0xf0000000))
                h ^= g >> 24;
            h &= ~g;
        }
    } else {                                    // case insensitive
        for (i=0; i<(int)key.length(); i++) {
            h = (h<<4) + p[i].lower().cell();
            if ((g = h & 0xf0000000))
                h ^= g >> 24;
            h &= ~g;
        }
    }
    int index = h;
    if (index < 0)                              // adjust index to table size
        index = -index;
    return index;
}

/*!
  Returns the hash key for \a key, which is a C string.
*/

int Q3GDict::hashKeyAscii(const char *key)
{
#if defined(QT_CHECK_NULL)
    if (key == 0)
        qWarning("Q3GDict::hashAsciiKey: Invalid null key");
#endif
    register const char *k = key;
    register uint h=0;
    uint g;
    if (cases) {                                // case sensitive
        while (*k) {
            h = (h<<4) + *k++;
            if ((g = h & 0xf0000000))
                h ^= g >> 24;
            h &= ~g;
        }
    } else {                                    // case insensitive
        while (*k) {
            h = (h<<4) + tolower((uchar) *k);
            if ((g = h & 0xf0000000))
                h ^= g >> 24;
            h &= ~g;
            k++;
        }
    }
    int index = h;
    if (index < 0)                              // adjust index to table size
        index = -index;
    return index;
}

#ifndef QT_NO_DATASTREAM

/*!
    \overload
  Reads a collection/dictionary item from the stream \a s and returns a
  reference to the stream.

  The default implementation sets \a item to 0.

  \sa write()
*/

QDataStream& Q3GDict::read(QDataStream &s, Q3PtrCollection::Item &item)
{
    item = 0;
    return s;
}

/*!
    \overload
  Writes a collection/dictionary item to the stream \a s and returns a
  reference to the stream.

  \sa read()
*/

QDataStream& Q3GDict::write(QDataStream &s, Q3PtrCollection::Item) const
{
    return s;
}
#endif //QT_NO_DATASTREAM

/*****************************************************************************
  Q3GDict member functions
 *****************************************************************************/

/*!
  Constructs a dictionary.

  \a len is the initial size of the dictionary.
  The key type is \a kt which may be \c StringKey, \c AsciiKey,
  \c IntKey or \c PtrKey. The case-sensitivity of lookups is set with
  \a caseSensitive. Keys are copied if \a copyKeys is true.
*/

Q3GDict::Q3GDict(uint len, KeyType kt, bool caseSensitive, bool copyKeys)
{
    init(len, kt, caseSensitive, copyKeys);
}


void Q3GDict::init(uint len, KeyType kt, bool caseSensitive, bool copyKeys)
{
    vlen = len ? len : 17;
    vec = new Q3BaseBucket *[ vlen ];

    Q_CHECK_PTR(vec);
    memset((char*)vec, 0, vlen*sizeof(Q3BaseBucket*));
    numItems  = 0;
    iterators = 0;
    // The caseSensitive and copyKey options don't make sense for
    // all dict types.
    switch ((keytype = (uint)kt)) {
        case StringKey:
            cases = caseSensitive;
            copyk = false;
            break;
        case AsciiKey:
            cases = caseSensitive;
            copyk = copyKeys;
            break;
        default:
            cases = false;
            copyk = false;
            break;
    }
}


/*!
  Constructs a copy of \a dict.
*/

Q3GDict::Q3GDict(const Q3GDict & dict)
    : Q3PtrCollection(dict)
{
    init(dict.vlen, (KeyType)dict.keytype, dict.cases, dict.copyk);
    Q3GDictIterator it(dict);
    while (it.get()) {                  // copy from other dict
        switch (keytype) {
            case StringKey:
                look_string(it.getKeyString(), it.get(), op_insert);
                break;
            case AsciiKey:
                look_ascii(it.getKeyAscii(), it.get(), op_insert);
                break;
            case IntKey:
                look_int(it.getKeyInt(), it.get(), op_insert);
                break;
            case PtrKey:
                look_ptr(it.getKeyPtr(), it.get(), op_insert);
                break;
        }
        ++it;
    }
}


/*!
  Removes all items from the dictionary and destroys it.
*/

Q3GDict::~Q3GDict()
{
    clear();                                    // delete everything
    delete [] vec;
    if (!iterators)                             // no iterators for this dict
        return;
    Q3GDictIterator *i = iterators->first();
    while (i) {                         // notify all iterators that
        i->dict = 0;                            // this dict is deleted
        i = iterators->next();
    }
    delete iterators;
}


/*!
  Assigns \a dict to this dictionary.
*/

Q3GDict &Q3GDict::operator=(const Q3GDict &dict)
{
    if (&dict == this)
        return *this;
    clear();
    Q3GDictIterator it(dict);
    while (it.get()) {                  // copy from other dict
        switch (keytype) {
            case StringKey:
                look_string(it.getKeyString(), it.get(), op_insert);
                break;
            case AsciiKey:
                look_ascii(it.getKeyAscii(), it.get(), op_insert);
                break;
            case IntKey:
                look_int(it.getKeyInt(), it.get(), op_insert);
                break;
            case PtrKey:
                look_ptr(it.getKeyPtr(), it.get(), op_insert);
                break;
        }
        ++it;
    }
    return *this;
}

/*!
  \fn uint Q3GDict::count() const

  Returns the number of items in the dictionary.
*/

/*!
  \fn uint Q3GDict::size() const

  Returns the size of the hash array.
*/

/*!
  The do-it-all function; \a op is one of op_find, op_insert, op_replace.
  The key is \a key and the item is \a d.
*/

Q3PtrCollection::Item Q3GDict::look_string(const QString &key, Q3PtrCollection::Item d,
                                       int op)
{
    Q3StringBucket *n = 0;
    int index = hashKeyString(key) % vlen;
    if (op == op_find) {                        // find
        if (cases) {
            n = (Q3StringBucket*)vec[index];
            while(n != 0) {
                if (key == n->getKey())
                    return n->getData();        // item found
                n = (Q3StringBucket*)n->getNext();
            }
        } else {
            QString k = key.lower();
            n = (Q3StringBucket*)vec[index];
            while(n != 0) {
                if (k == n->getKey().lower())
                    return n->getData();        // item found
                n = (Q3StringBucket*)n->getNext();
            }
        }
        return 0;                               // not found
    }
    if (op == op_replace) {                     // replace
        if (vec[index] != 0)                    // maybe something there
            remove_string(key);
    }
    // op_insert or op_replace
    n = new Q3StringBucket(key,newItem(d),vec[index]);
    Q_CHECK_PTR(n);
#if defined(QT_CHECK_NULL)
    if (n->getData() == 0)
        qWarning("QDict: Cannot insert null item");
#endif
    vec[index] = n;
    numItems++;
    return n->getData();
}

Q3PtrCollection::Item Q3GDict::look_ascii(const char *key, Q3PtrCollection::Item d, int op)
{
    Q3AsciiBucket *n;
    int index = hashKeyAscii(key) % vlen;
    if (op == op_find) {                        // find
        if (cases) {
            for (n=(Q3AsciiBucket*)vec[index]; n;
                  n=(Q3AsciiBucket*)n->getNext()) {
                if (qstrcmp(n->getKey(),key) == 0)
                    return n->getData();        // item found
            }
        } else {
            for (n=(Q3AsciiBucket*)vec[index]; n;
                  n=(Q3AsciiBucket*)n->getNext()) {
                if (qstricmp(n->getKey(),key) == 0)
                    return n->getData();        // item found
            }
        }
        return 0;                               // not found
    }
    if (op == op_replace) {                     // replace
        if (vec[index] != 0)                    // maybe something there
            remove_ascii(key);
    }
    // op_insert or op_replace
    n = new Q3AsciiBucket(copyk ? qstrdup(key) : key,newItem(d),vec[index]);
    Q_CHECK_PTR(n);
#if defined(QT_CHECK_NULL)
    if (n->getData() == 0)
        qWarning("QAsciiDict: Cannot insert null item");
#endif
    vec[index] = n;
    numItems++;
    return n->getData();
}

Q3PtrCollection::Item Q3GDict::look_int(long key, Q3PtrCollection::Item d, int op)
{
    Q3IntBucket *n;
    int index = (int)((ulong)key % vlen);       // simple hash
    if (op == op_find) {                        // find
        for (n=(Q3IntBucket*)vec[index]; n;
              n=(Q3IntBucket*)n->getNext()) {
            if (n->getKey() == key)
                return n->getData();            // item found
        }
        return 0;                               // not found
    }
    if (op == op_replace) {                     // replace
        if (vec[index] != 0)                    // maybe something there
            remove_int(key);
    }
    // op_insert or op_replace
    n = new Q3IntBucket(key,newItem(d),vec[index]);
    Q_CHECK_PTR(n);
#if defined(QT_CHECK_NULL)
    if (n->getData() == 0)
        qWarning("QIntDict: Cannot insert null item");
#endif
    vec[index] = n;
    numItems++;
    return n->getData();
}

Q3PtrCollection::Item Q3GDict::look_ptr(void *key, Q3PtrCollection::Item d, int op)
{
    Q3PtrBucket *n;
    int index = (int)((ulong)key % vlen);       // simple hash
    if (op == op_find) {                        // find
        for (n=(Q3PtrBucket*)vec[index]; n;
              n=(Q3PtrBucket*)n->getNext()) {
            if (n->getKey() == key)
                return n->getData();            // item found
        }
        return 0;                               // not found
    }
    if (op == op_replace) {                     // replace
        if (vec[index] != 0)                    // maybe something there
            remove_ptr(key);
    }
    // op_insert or op_replace
    n = new Q3PtrBucket(key,newItem(d),vec[index]);
    Q_CHECK_PTR(n);
#if defined(QT_CHECK_NULL)
    if (n->getData() == 0)
        qWarning("Q3PtrDict: Cannot insert null item");
#endif
    vec[index] = n;
    numItems++;
    return n->getData();
}


/*!
  Changes the size of the hashtable to \a newsize.
  The contents of the dictionary are preserved,
  but all iterators on the dictionary become invalid.
*/
void Q3GDict::resize(uint newsize)
{
    // Save old information
    Q3BaseBucket **old_vec = vec;
    uint old_vlen  = vlen;
    bool old_copyk = copyk;

    vec = new Q3BaseBucket *[vlen = newsize];
    Q_CHECK_PTR(vec);
    memset((char*)vec, 0, vlen*sizeof(Q3BaseBucket*));
    numItems = 0;
    copyk = false;

    // Reinsert every item from vec, deleting vec as we go
    for (uint index = 0; index < old_vlen; index++) {
        switch (keytype) {
            case StringKey:
                {
                    Q3StringBucket *n=(Q3StringBucket *)old_vec[index];
                    while (n) {
                        look_string(n->getKey(), n->getData(), op_insert);
                        Q3StringBucket *t=(Q3StringBucket *)n->getNext();
                        delete n;
                        n = t;
                    }
                }
                break;
            case AsciiKey:
                {
                    Q3AsciiBucket *n=(Q3AsciiBucket *)old_vec[index];
                    while (n) {
                        look_ascii(n->getKey(), n->getData(), op_insert);
                        Q3AsciiBucket *t=(Q3AsciiBucket *)n->getNext();
                        delete n;
                        n = t;
                    }
                }
                break;
            case IntKey:
                {
                    Q3IntBucket *n=(Q3IntBucket *)old_vec[index];
                    while (n) {
                        look_int(n->getKey(), n->getData(), op_insert);
                        Q3IntBucket *t=(Q3IntBucket *)n->getNext();
                        delete n;
                        n = t;
                    }
                }
                break;
            case PtrKey:
                {
                    Q3PtrBucket *n=(Q3PtrBucket *)old_vec[index];
                    while (n) {
                        look_ptr(n->getKey(), n->getData(), op_insert);
                        Q3PtrBucket *t=(Q3PtrBucket *)n->getNext();
                        delete n;
                        n = t;
                    }
                }
                break;
        }
    }
    delete [] old_vec;

    // Restore state
    copyk = old_copyk;

    // Invalidate all iterators, since order is lost
    if (iterators && iterators->count()) {
        Q3GDictIterator *i = iterators->first();
        while (i) {
            i->toFirst();
            i = iterators->next();
        }
    }
}

/*!
  Unlinks the bucket with the specified key (and specified data pointer,
  if it is set).
*/

void Q3GDict::unlink_common(int index, Q3BaseBucket *node, Q3BaseBucket *prev)
{
    if (iterators && iterators->count()) {      // update iterators
        Q3GDictIterator *i = iterators->first();
        while (i) {                             // invalidate all iterators
            if (i->curNode == node)             // referring to pending node
                i->operator++();
            i = iterators->next();
        }
    }
    if (prev)                                   // unlink node
        prev->setNext(node->getNext());
    else
        vec[index] = node->getNext();
    numItems--;
}

Q3StringBucket *Q3GDict::unlink_string(const QString &key, Q3PtrCollection::Item d)
{
    if (numItems == 0)                  // nothing in dictionary
        return 0;
    Q3StringBucket *n;
    Q3StringBucket *prev = 0;
    int index = hashKeyString(key) % vlen;
    if (cases) {
        for (n=(Q3StringBucket*)vec[index]; n;
              n=(Q3StringBucket*)n->getNext()) {
            bool found = (key == n->getKey());
            if (found && d)
                found = (n->getData() == d);
            if (found) {
                unlink_common(index,n,prev);
                return n;
            }
            prev = n;
        }
    } else {
        QString k = key.lower();
        for (n=(Q3StringBucket*)vec[index]; n;
              n=(Q3StringBucket*)n->getNext()) {
            bool found = (k == n->getKey().lower());
            if (found && d)
                found = (n->getData() == d);
            if (found) {
                unlink_common(index,n,prev);
                return n;
            }
            prev = n;
        }
    }
    return 0;
}

Q3AsciiBucket *Q3GDict::unlink_ascii(const char *key, Q3PtrCollection::Item d)
{
    if (numItems == 0)                  // nothing in dictionary
        return 0;
    Q3AsciiBucket *n;
    Q3AsciiBucket *prev = 0;
    int index = hashKeyAscii(key) % vlen;
    for (n=(Q3AsciiBucket *)vec[index]; n; n=(Q3AsciiBucket *)n->getNext()) {
        bool found = (cases ? qstrcmp(n->getKey(),key)
                       : qstricmp(n->getKey(),key)) == 0;
        if (found && d)
            found = (n->getData() == d);
        if (found) {
            unlink_common(index,n,prev);
            return n;
        }
        prev = n;
    }
    return 0;
}

Q3IntBucket *Q3GDict::unlink_int(long key, Q3PtrCollection::Item d)
{
    if (numItems == 0)                  // nothing in dictionary
        return 0;
    Q3IntBucket *n;
    Q3IntBucket *prev = 0;
    int index = (int)((ulong)key % vlen);
    for (n=(Q3IntBucket *)vec[index]; n; n=(Q3IntBucket *)n->getNext()) {
        bool found = (n->getKey() == key);
        if (found && d)
            found = (n->getData() == d);
        if (found) {
            unlink_common(index,n,prev);
            return n;
        }
        prev = n;
    }
    return 0;
}

Q3PtrBucket *Q3GDict::unlink_ptr(void *key, Q3PtrCollection::Item d)
{
    if (numItems == 0)                  // nothing in dictionary
        return 0;
    Q3PtrBucket *n;
    Q3PtrBucket *prev = 0;
    int index = (int)((ulong)key % vlen);
    for (n=(Q3PtrBucket *)vec[index]; n; n=(Q3PtrBucket *)n->getNext()) {
        bool found = (n->getKey() == key);
        if (found && d)
            found = (n->getData() == d);
        if (found) {
            unlink_common(index,n,prev);
            return n;
        }
        prev = n;
    }
    return 0;
}


/*!
  Removes the item with the specified \a key.  If \a item is not null,
  the remove will match the \a item as well (used to remove an
  item when several items have the same key).
*/

bool Q3GDict::remove_string(const QString &key, Q3PtrCollection::Item item)
{
    Q3StringBucket *n = unlink_string(key, item);
    if (n) {
        deleteItem(n->getData());
        delete n;
        return true;
    } else {
        return false;
    }
}

bool Q3GDict::remove_ascii(const char *key, Q3PtrCollection::Item item)
{
    Q3AsciiBucket *n = unlink_ascii(key, item);
    if (n) {
        if (copyk)
            delete [] (char *)n->getKey();
        deleteItem(n->getData());
        delete n;
    }
    return n != 0;
}

bool Q3GDict::remove_int(long key, Q3PtrCollection::Item item)
{
    Q3IntBucket *n = unlink_int(key, item);
    if (n) {
        deleteItem(n->getData());
        delete n;
    }
    return n != 0;
}

bool Q3GDict::remove_ptr(void *key, Q3PtrCollection::Item item)
{
    Q3PtrBucket *n = unlink_ptr(key, item);
    if (n) {
        deleteItem(n->getData());
        delete n;
    }
    return n != 0;
}

Q3PtrCollection::Item Q3GDict::take_string(const QString &key)
{
    Q3StringBucket *n = unlink_string(key);
    Item d;
    if (n) {
        d = n->getData();
        delete n;
    } else {
        d = 0;
    }
    return d;
}

Q3PtrCollection::Item Q3GDict::take_ascii(const char *key)
{
    Q3AsciiBucket *n = unlink_ascii(key);
    Item d;
    if (n) {
        if (copyk)
            delete [] (char *)n->getKey();
        d = n->getData();
        delete n;
    } else {
        d = 0;
    }
    return d;
}

Q3PtrCollection::Item Q3GDict::take_int(long key)
{
    Q3IntBucket *n = unlink_int(key);
    Item d;
    if (n) {
        d = n->getData();
        delete n;
    } else {
        d = 0;
    }
    return d;
}

Q3PtrCollection::Item Q3GDict::take_ptr(void *key)
{
    Q3PtrBucket *n = unlink_ptr(key);
    Item d;
    if (n) {
        d = n->getData();
        delete n;
    } else {
        d = 0;
    }
    return d;
}

/*!
  Removes all items from the dictionary.
*/
void Q3GDict::clear()
{
    if (!numItems)
        return;
    numItems = 0;                               // disable remove() function
    for (uint j=0; j<vlen; j++) {               // destroy hash table
        if (vec[j]) {
            switch (keytype) {
                case StringKey:
                    {
                        Q3StringBucket *n=(Q3StringBucket *)vec[j];
                        while (n) {
                            Q3StringBucket *next = (Q3StringBucket*)n->getNext();
                            deleteItem(n->getData());
                            delete n;
                            n = next;
                        }
                    }
                    break;
                case AsciiKey:
                    {
                        Q3AsciiBucket *n=(Q3AsciiBucket *)vec[j];
                        while (n) {
                            Q3AsciiBucket *next = (Q3AsciiBucket*)n->getNext();
                            if (copyk)
                                delete [] (char *)n->getKey();
                            deleteItem(n->getData());
                            delete n;
                            n = next;
                        }
                    }
                    break;
                case IntKey:
                    {
                        Q3IntBucket *n=(Q3IntBucket *)vec[j];
                        while (n) {
                            Q3IntBucket *next = (Q3IntBucket*)n->getNext();
                            deleteItem(n->getData());
                            delete n;
                            n = next;
                        }
                    }
                    break;
                case PtrKey:
                    {
                        Q3PtrBucket *n=(Q3PtrBucket *)vec[j];
                        while (n) {
                            Q3PtrBucket *next = (Q3PtrBucket*)n->getNext();
                            deleteItem(n->getData());
                            delete n;
                            n = next;
                        }
                    }
                    break;
            }
            vec[j] = 0;                         // detach list of buckets
        }
    }
    if (iterators && iterators->count()) {      // invalidate all iterators
        Q3GDictIterator *i = iterators->first();
        while (i) {
            i->curNode = 0;
            i = iterators->next();
        }
    }
}

/*!
  Outputs debug statistics.
*/
void Q3GDict::statistics() const
{
#if defined(QT_DEBUG)
    QString line;
    line.fill(QLatin1Char('-'), 60);
    double real, ideal;
    qDebug("%s", line.ascii());
    qDebug("DICTIONARY STATISTICS:");
    if (count() == 0) {
        qDebug("Empty!");
        qDebug("%s", line.ascii());
        return;
    }
    real = 0.0;
    ideal = (float)count()/(2.0*size())*(count()+2.0*size()-1);
    uint i = 0;
    while (i<size()) {
        Q3BaseBucket *n = vec[i];
        int b = 0;
        while (n) {                             // count number of buckets
            b++;
            n = n->getNext();
        }
        real = real + (double)b * ((double)b+1.0)/2.0;
        char buf[80], *pbuf;
        if (b > 78)
            b = 78;
        pbuf = buf;
        while (b--)
            *pbuf++ = '*';
        *pbuf = '\0';
        qDebug("%s", buf);
        i++;
    }
    qDebug("Array size = %d", size());
    qDebug("# items    = %d", count());
    qDebug("Real dist  = %g", real);
    qDebug("Rand dist  = %g", ideal);
    qDebug("Real/Rand  = %g", real/ideal);
    qDebug("%s", line.ascii());
#endif // QT_DEBUG
}


/*****************************************************************************
  Q3GDict stream functions
 *****************************************************************************/
#ifndef QT_NO_DATASTREAM
QDataStream &operator>>(QDataStream &s, Q3GDict &dict)
{
    return dict.read(s);
}

QDataStream &operator<<(QDataStream &s, const Q3GDict &dict)
{
    return dict.write(s);
}

#if defined(Q_CC_DEC) && defined(__alpha) && (__DECCXX_VER-0 >= 50190001)
#pragma message disable narrowptr
#endif

/*!
  Reads a dictionary from the stream \a s.
*/

QDataStream &Q3GDict::read(QDataStream &s)
{
    uint num;
    s >> num;                                   // read number of items
    clear();                                    // clear dict
    while (num--) {                             // read all items
        Item d;
        switch (keytype) {
            case StringKey:
                {
                    QString k;
                    s >> k;
                    read(s, d);
                    look_string(k, d, op_insert);
                }
                break;
            case AsciiKey:
                {
                    char *k;
                    s >> k;
                    read(s, d);
                    look_ascii(k, d, op_insert);
                    if (copyk)
                        delete [] k;
                }
                break;
            case IntKey:
                {
                    Q_UINT32 k;
                    s >> k;
                    read(s, d);
                    look_int(k, d, op_insert);
                }
                break;
            case PtrKey:
                {
                    Q_UINT32 k;
                    s >> k;
                    read(s, d);
                    // ### cannot insert 0 - this renders the thing
                    // useless since all pointers are written as 0,
                    // but hey, serializing pointers?  can it be done
                    // at all, ever?
                    if (k)
                        look_ptr((void *)(ulong)k, d, op_insert);
                }
                break;
        }
    }
    return s;
}

/*!
  Writes the dictionary to the stream \a s.
*/

QDataStream& Q3GDict::write(QDataStream &s) const
{
    s << count();                               // write number of items
    uint i = 0;
    while (i<size()) {
        Q3BaseBucket *n = vec[i];
        while (n) {                             // write all buckets
            switch (keytype) {
                case StringKey:
                    s << ((Q3StringBucket*)n)->getKey();
                    break;
                case AsciiKey:
                    s << ((Q3AsciiBucket*)n)->getKey();
                    break;
                case IntKey:
                    s << (Q_UINT32)((Q3IntBucket*)n)->getKey();
                    break;
                case PtrKey:
                    s << (Q_UINT32)0; // ### cannot serialize a pointer
                    break;
            }
            write(s, n->getData());             // write data
            n = n->getNext();
        }
        i++;
    }
    return s;
}
#endif //QT_NO_DATASTREAM

/*****************************************************************************
  Q3GDictIterator member functions
 *****************************************************************************/

/*!
  \class Q3GDictIterator qgdict.h
  \reentrant
  \brief The Q3GDictIterator class is an internal class for implementing QDictIterator and QIntDictIterator.

  \internal

  Q3GDictIterator is a strictly internal class that does the heavy work for
  QDictIterator and QIntDictIterator.
*/

/*!
  Constructs an iterator that operates on the dictionary \a d.
*/

Q3GDictIterator::Q3GDictIterator(const Q3GDict &d)
{
    dict = (Q3GDict *)&d;                       // get reference to dict
    toFirst();                                  // set to first noe
    if (!dict->iterators) {
        dict->iterators = new Q3GDItList;       // create iterator list
        Q_CHECK_PTR(dict->iterators);
    }
    dict->iterators->append(this);              // attach iterator to dict
}

/*!
  Constructs a copy of the iterator \a it.
*/

Q3GDictIterator::Q3GDictIterator(const Q3GDictIterator &it)
{
    dict = it.dict;
    curNode = it.curNode;
    curIndex = it.curIndex;
    if (dict)
        dict->iterators->append(this);  // attach iterator to dict
}

/*!
  Assigns a copy of the iterator \a it and returns a reference to this
  iterator.
*/

Q3GDictIterator &Q3GDictIterator::operator=(const Q3GDictIterator &it)
{
    if (dict)                                   // detach from old dict
        dict->iterators->removeRef(this);
    dict = it.dict;
    curNode = it.curNode;
    curIndex = it.curIndex;
    if (dict)
        dict->iterators->append(this);  // attach to new list
    return *this;
}

/*!
  Destroys the iterator.
*/

Q3GDictIterator::~Q3GDictIterator()
{
    if (dict)                                   // detach iterator from dict
        dict->iterators->removeRef(this);
}


/*!
  Sets the iterator to point to the first item in the dictionary.
*/

Q3PtrCollection::Item Q3GDictIterator::toFirst()
{
    if (!dict) {
#if defined(QT_CHECK_NULL)
        qWarning("Q3GDictIterator::toFirst: Dictionary has been deleted");
#endif
        return 0;
    }
    if (dict->count() == 0) {                   // empty dictionary
        curNode = 0;
        return 0;
    }
    register uint i = 0;
    register Q3BaseBucket **v = dict->vec;
    while (!(*v++))
        i++;
    curNode = dict->vec[i];
    curIndex = i;
    return curNode->getData();
}


/*!
  Moves to the next item (postfix).
*/

Q3PtrCollection::Item Q3GDictIterator::operator()()
{
    if (!dict) {
#if defined(QT_CHECK_NULL)
        qWarning("Q3GDictIterator::operator(): Dictionary has been deleted");
#endif
        return 0;
    }
    if (!curNode)
        return 0;
    Q3PtrCollection::Item d = curNode->getData();
    this->operator++();
    return d;
}

/*!
  Moves to the next item (prefix).
*/

Q3PtrCollection::Item Q3GDictIterator::operator++()
{
    if (!dict) {
#if defined(QT_CHECK_NULL)
        qWarning("Q3GDictIterator::operator++: Dictionary has been deleted");
#endif
        return 0;
    }
    if (!curNode)
        return 0;
    curNode = curNode->getNext();
    if (!curNode) {                             // no next bucket
        register uint i = curIndex + 1;         // look from next vec element
        register Q3BaseBucket **v = &dict->vec[i];
        while (i < dict->size() && !(*v++))
            i++;
        if (i == dict->size()) {                // nothing found
            curNode = 0;
            return 0;
        }
        curNode = dict->vec[i];
        curIndex = i;
    }
    return curNode->getData();
}

/*!
  Moves \a jumps positions forward.
*/

Q3PtrCollection::Item Q3GDictIterator::operator+=(uint jumps)
{
    while (curNode && jumps--)
        operator++();
    return curNode ? curNode->getData() : 0;
}

QT_END_NAMESPACE
