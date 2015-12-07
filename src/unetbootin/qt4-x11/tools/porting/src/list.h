/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the qt3to4 porting application of the Qt Toolkit.
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

#ifndef LIST_H
#define LIST_H

#include "smallobject.h"
#include <QtGlobal>

QT_BEGIN_NAMESPACE

template <typename T>
class List
{
    struct Data
    {
        int alloc, size;
        T array[1];
    };
    pool *p;
    Data *d;

public:
    inline List(pool *_pool) : p(_pool), d(0) { d = malloc(16); d->size = 0; d->alloc = 16; }

    inline List(const List &v) : d(0) { operator=(v); }
    inline ~List() { d = 0; }
    List &operator=(const List &v);

    bool operator==(const List &v) const;
    inline bool operator!=(const List &v) const { return !(*this == v); }

    inline int size() const { return d->size; }
    inline bool isEmpty() const { return d->size == 0; }

    inline int capacity() const { return d->alloc; }
    void reserve(int alloc);

    inline T* data() { return d->array; }
    inline const T* data() const { return d->array; }
    void clear();

    const T &at(int i) const;
    T &operator[](int i);
    const T &operator[](int i) const;
    void append(const T &t);
    void prepend(const T &t);
    void insert(int i, const T &t);
    void insert(int i, int n, const T &t);
    void replace(int i, const T &t);
    void remove(int i);
    void remove(int i, int n);

    List &fill(const T &t, int size = -1);

    int indexOf(const T &t, int from = 0) const;
    int lastIndexOf(const T &t, int from = -1) const;
    bool contains(const T &t) const;
    int count(const T &t) const;

    // STL-style
    typedef T* iterator;
    typedef const T* const_iterator;
    inline iterator begin() { return d->array; }
    inline const_iterator begin() const { return d->array; }
    inline iterator end() { return d->array + d->size; }
    inline const_iterator end() const { return d->array + d->size; }
    iterator insert(iterator before, int n, const T &x);
    inline iterator insert(iterator before, const T &x) { return insert(before, 1, x); }
    iterator erase(iterator begin, iterator end);
    inline iterator erase(iterator pos) { return erase(pos, pos+1); }

    // more Qt
    inline int count() const { return d->size; }
    inline T& first() { Q_ASSERT(!isEmpty()); return *begin(); }
    inline const T& first() const { Q_ASSERT(!isEmpty()); return *begin(); }
    inline T& last() { Q_ASSERT(!isEmpty()); return *(end()-1); }
    inline const T& last() const { Q_ASSERT(!isEmpty()); return *(end()-1); }

    T value(int i) const;
    T value(int i, const T &defaultValue) const;

    // STL compatibility
    typedef T value_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;
#ifndef QT_NO_STL
    typedef ptrdiff_t difference_type;
#else
    typedef int difference_type;
#endif
    typedef iterator Iterator;
    typedef const_iterator ConstIterator;
    typedef int size_type;
    inline void push_back(const T &t) { append(t); }
    inline void push_front(const T &t) { prepend(t); }
    void pop_back() { Q_ASSERT(!isEmpty()); erase(end()-1); }
    void pop_front() { Q_ASSERT(!isEmpty()); erase(begin()); }
    inline bool empty() const
    { return d->size == 0; }
    inline T& front() { return first(); }
    inline const_reference front() const { return first(); }
    inline reference back() { return last(); }
    inline const_reference back() const { return last(); }

    //comfort
    List &operator+=(const List &l);
    inline List operator+(const List &l) const
    { List n = *this; n += l; return n; }
    inline void operator+=(const T &t)
    { append(t); }
    inline List &operator<< (const T &t)
    { append(t); return *this; }

private:
    Data *malloc(int alloc);
};

template <typename T>
inline void List<T>::clear()
{ d->size = 0; }

template <typename T>
inline const T &List<T>::at(int i) const
{ Q_ASSERT_X(i >= 0 && i < d->size, "List<T>::at", "index out of range");
  return d->array[i]; }
template <typename T>
inline const T &List<T>::operator[](int i) const
{ Q_ASSERT_X(i >= 0 && i < d->size, "List<T>::operator[]", "index out of range");
  return d->array[i]; }
template <typename T>
inline T &List<T>::operator[](int i)
{ Q_ASSERT_X(i >= 0 && i < d->size, "List<T>::operator[]", "index out of range");
  return d->array[i]; }
template <typename T>
inline void List<T>::insert(int i, const T &t)
{ Q_ASSERT_X(i >= 0 && i <= d->size, "List<T>::insert", "index out of range");
  insert(begin()+i, 1, t); }
template <typename T>
inline void List<T>::insert(int i, int n, const T &t)
{ Q_ASSERT_X(i >= 0 && i <= d->size, "List<T>::insert", "index out of range");
  insert(begin() + i, n, t); }
template <typename T>
inline void List<T>::remove(int i, int n)
{ Q_ASSERT_X(i >= 0 && i + n <= d->size, "List<T>::remove", "index out of range");
  erase(begin() + i, begin() + i + n); }
template <typename T>
inline void List<T>::remove(int i)
{ Q_ASSERT_X(i >= 0 && i < d->size, "List<T>::remove", "index out of range");
  erase(begin() + i, begin() + i + 1); }
template <typename T>
inline void List<T>::prepend(const T &t)
{ insert(begin(), 1, t); }
template <typename T>
inline void List<T>::replace(int i, const T &t)
{ Q_ASSERT_X(i >= 0 && i < d->size, "List<T>::replace", "index out of range");
  data()[i] = t; }

template <typename T>
List<T> &List<T>::operator=(const List<T> &v)
{
    p = v.p;
    d = malloc(v.d->alloc);
    memcpy(d, v.d, sizeof(Data) + (v.d->size - 1) * sizeof(T));
    return *this;
}

template <typename T>
inline typename List<T>::Data *List<T>::malloc(int alloc)
{
    return static_cast<Data *>(p->allocate(sizeof(Data) + (alloc - 1) * sizeof(T)));
}

template <typename T>
void List<T>::reserve(int alloc)
{
    if (alloc <= d->alloc)
        return;
    alloc <<= 2;
    d = static_cast<Data *>(p->reallocate(d, sizeof(Data) + d->alloc * sizeof(T),
                                          sizeof(Data) + (alloc - 1) * sizeof(T)));
    d->alloc = alloc;
}

template<typename T>
T List<T>::value(int i) const
{
    if(i < 0 || i >= d->size) {
        return T();
    }
    return d->array[i];
}
template<typename T>
T List<T>::value(int i, const T& defaultValue) const
{
    return ((i < 0 || i >= d->size) ? defaultValue : d->array[i]);
}

template <typename T>
void List<T>::append(const T &t)
{
    reserve(d->size + 1);
    d->array[d->size++] = t;
}


template <typename T>
typename List<T>::iterator List<T>::insert(iterator before, size_type n, const T& t)
{
    int p = before - d->array;
    if (n != 0) {
        reserve(d->size + n);
        T *b = d->array+p;
        T *i = b+n;
        memmove(i, b, (d->size-p)*sizeof(T));
        while (i != b)
            *(--i) = t;
    }
    d->size += n;
    return d->array+p;
}

template <typename T>
typename List<T>::iterator List<T>::erase(iterator begin, iterator end)
{
    int f = begin - d->array;
    int l = end - d->array;
    int n = l - f;
    memmove(d->array + f, d->array + l, (d->size-l)*sizeof(T));
    d->size -= n;
    return d->array + f;
}

template <typename T>
bool List<T>::operator==(const List<T> &v) const
{
    if (d->size != v.d->size)
        return false;
    T* b = d->array;
    T* i = b + d->size;
    T* j = v.d->array + d->size;
    while (i != b)
        if (!(*--i == *--j))
            return false;
    return true;
}

template <typename T>
List<T> &List<T>::fill(const T &t, int size)
{
    resize(size < 0 ? d->size : size);
    if (d->size) {
        T* i = d->array + d->size;
        T* b = d->array;
        while (i != b)
            *--i = t;
    }
    return *this;
}

template <typename T>
List<T> &List<T>::operator+=(const List &l)
{
    int newSize = d->size + l.d->size;
    reserve(newSize);

    T *w = d->array + newSize;
    T *i = l.d->array + l.d->size;
    T *b = l.d->array;
    while (i != b)
        *--w = *--i;
    d->size = newSize;
    return *this;
}

template <typename T>
int List<T>::indexOf(const T &t, int from) const
{
    if (from < 0)
        from = qMax(from + d->size, 0);
    if (from < d->size) {
        T* n = d->array + from - 1;
        T* e = d->array + d->size;
        while (++n != e)
            if (*n == t)
                return n - d->array;
    }
    return -1;
}

template <typename T>
int List<T>::lastIndexOf(const T &t, int from) const
{
    if (from < 0)
        from += d->size;
    else if (from >= d->size)
        from = d->size-1;
    if (from >= 0) {
        T* b = d->array;
        T* n = d->array + from + 1;
        while (n != b) {
            if (*--n == t)
                return n - b;
        }
    }
    return -1;
}

template <typename T>
bool List<T>::contains(const T &t) const
{
    T* b = d->array;
    T* i = d->array + d->size;
    while (i != b)
        if (*--i == t)
            return true;
    return false;
}

template <typename T>
int List<T>::count(const T &t) const
{
    int c = 0;
    T* b = d->array;
    T* i = d->array + d->size;
    while (i != b)
        if (*--i == t)
            ++c;
    return c;
}

QT_END_NAMESPACE

#endif // LIST_H
