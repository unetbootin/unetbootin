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

#ifndef Q3MEMARRAY_H
#define Q3MEMARRAY_H

#include <Qt3Support/q3garray.h>
#include <QtCore/qvector.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3SupportLight)

template<class type>
class Q3MemArray : public Q3GArray
{
public:
    typedef type* Iterator;
    typedef const type* ConstIterator;
    typedef type ValueType;

protected:
    Q3MemArray(int, int) : Q3GArray(0, 0) {}

public:
    Q3MemArray() {}
    Q3MemArray(int size) : Q3GArray(size*sizeof(type)) {}
    Q3MemArray(const Q3MemArray<type> &a) : Q3GArray(a) {}
    Q3MemArray(const QVector<type> &vector);
   ~Q3MemArray() {}
    Q3MemArray<type> &operator=(const Q3MemArray<type> &a)
				{ return (Q3MemArray<type>&)Q3GArray::assign(a); }
    type *data()    const	{ return (type *)Q3GArray::data(); }
    uint  nrefs()   const	{ return Q3GArray::nrefs(); }
    uint  size()    const	{ return Q3GArray::size()/sizeof(type); }
    uint  count()   const	{ return size(); }
    bool  isEmpty() const	{ return Q3GArray::size() == 0; }
    bool  isNull()  const	{ return Q3GArray::data() == 0; }
    bool  resize(uint size)	{ return Q3GArray::resize(size*sizeof(type)); }
    bool  resize(uint size, Optimization optim) { return Q3GArray::resize(size*sizeof(type), optim); }
    bool  truncate(uint pos)	{ return Q3GArray::resize(pos*sizeof(type)); }
    bool  fill(const type &d, int size = -1)
	{ return Q3GArray::fill((char*)&d,size,sizeof(type)); }
    void  detach()		{ Q3GArray::detach(); }
    Q3MemArray<type>   copy() const
	{ Q3MemArray<type> tmp; return tmp.duplicate(*this); }
    Q3MemArray<type>& assign(const Q3MemArray<type>& a)
	{ return (Q3MemArray<type>&)Q3GArray::assign(a); }
    Q3MemArray<type>& assign(const type *a, uint n)
	{ return (Q3MemArray<type>&)Q3GArray::assign((char*)a,n*sizeof(type)); }
    Q3MemArray<type>& duplicate(const Q3MemArray<type>& a)
	{ return (Q3MemArray<type>&)Q3GArray::duplicate(a); }
    Q3MemArray<type>& duplicate(const type *a, uint n)
	{ return (Q3MemArray<type>&)Q3GArray::duplicate((char*)a,n*sizeof(type)); }
    Q3MemArray<type>& setRawData(const type *a, uint n)
	{ return (Q3MemArray<type>&)Q3GArray::setRawData((char*)a,
						     n*sizeof(type)); }
    void resetRawData(const type *a, uint n)
	{ Q3GArray::resetRawData((char*)a,n*sizeof(type)); }
    int	 find(const type &d, uint i=0) const
	{ return Q3GArray::find((char*)&d,i,sizeof(type)); }
    int	 contains(const type &d) const
	{ return Q3GArray::contains((char*)&d,sizeof(type)); }
    void sort() { Q3GArray::sort(sizeof(type)); }
    int  bsearch(const type &d) const
	{ return Q3GArray::bsearch((const char*)&d,sizeof(type)); }
    // ### Qt 4.0: maybe provide uint overload as work-around for MSVC bug
    type& operator[](int i) const
	{ return (type &)(*(type *)Q3GArray::at(i*sizeof(type))); }
    type& at(uint i) const
	{ return (type &)(*(type *)Q3GArray::at(i*sizeof(type))); }
	 operator const type*() const { return (const type *)Q3GArray::data(); }
    bool operator==(const Q3MemArray<type> &a) const { return isEqual(a); }
    bool operator!=(const Q3MemArray<type> &a) const { return !isEqual(a); }
    Iterator begin() { return data(); }
    Iterator end() { return data() + size(); }
    ConstIterator begin() const { return data(); }
    ConstIterator end() const { return data() + size(); }

    operator QVector<type>() const;
};

template<class type>
Q_OUTOFLINE_TEMPLATE Q3MemArray<type>::Q3MemArray(const QVector<type> &vector)
    : Q3GArray(vector.size()*sizeof(type))
{
    for (int i = 0; i < vector.size(); ++i)
        at(i) = vector.at(i);
}

template<class type>
Q_OUTOFLINE_TEMPLATE Q3MemArray<type>::operator QVector<type>() const
{
    QVector<type> vector;
    for (int i = 0; i < size(); ++i)
        vector.append(at(i));
    return vector;
}

QT_END_NAMESPACE

QT_END_HEADER

#endif // Q3MEMARRAY_H
