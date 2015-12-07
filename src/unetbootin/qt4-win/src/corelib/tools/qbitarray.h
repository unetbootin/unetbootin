/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License versions 2.0 or 3.0 as published by the Free Software
** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file.  Alternatively you may (at
** your option) use any later version of the GNU General Public
** License if such license has been publicly approved by Trolltech ASA
** (or its successors, if any) and the KDE Free Qt Foundation. In
** addition, as a special exception, Trolltech gives you certain
** additional rights. These rights are described in the Trolltech GPL
** Exception version 1.2, which can be found at
** http://www.trolltech.com/products/qt/gplexception/ and in the file
** GPL_EXCEPTION.txt in this package.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/. If
** you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** In addition, as a special exception, Trolltech, as the sole
** copyright holder for Qt Designer, grants users of the Qt/Eclipse
** Integration plug-in the right for the Qt/Eclipse Integration to
** link to functionality provided by Qt Designer and its related
** libraries.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not expressly
** granted herein.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef QBITARRAY_H
#define QBITARRAY_H

#include <QtCore/qbytearray.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Core)

class QBitRef;
class Q_CORE_EXPORT QBitArray
{
    friend Q_CORE_EXPORT QDataStream &operator<<(QDataStream &, const QBitArray &);
    friend Q_CORE_EXPORT QDataStream &operator>>(QDataStream &, QBitArray &);
    friend Q_CORE_EXPORT uint qHash(const QBitArray &key);
    QByteArray d;

public:
    inline QBitArray() {}
    explicit QBitArray(int size, bool val = false);
    QBitArray(const QBitArray &other) : d(other.d) {}
    inline QBitArray &operator=(const QBitArray &other) { d = other.d; return *this; }

    inline int size() const { return (d.size() << 3) - *d.constData(); }
    inline int count() const { return (d.size() << 3) - *d.constData(); }
    int count(bool on) const;
    // ### Qt 5: Store the number of set bits separately

    inline bool isEmpty() const { return d.isEmpty(); }
    inline bool isNull() const { return d.isNull(); }

    void resize(int size);

    inline void detach() { d.detach(); }
    inline bool isDetached() const { return d.isDetached(); }
    inline void clear() { d.clear(); }

    bool testBit(int i) const;
    void setBit(int i);
    void setBit(int i, bool val);
    void clearBit(int i);
    bool toggleBit(int i);

    bool at(int i) const;
    QBitRef operator[](int i);
    bool operator[](int i) const;
    QBitRef operator[](uint i);
    bool operator[](uint i) const;

    QBitArray& operator&=(const QBitArray &);
    QBitArray& operator|=(const QBitArray &);
    QBitArray& operator^=(const QBitArray &);
    QBitArray  operator~() const;

    inline bool operator==(const QBitArray& a) const { return d == a.d; }
    inline bool operator!=(const QBitArray& a) const { return d != a.d; }

    inline bool fill(bool val, int size = -1);
    void fill(bool val, int first, int last);

    inline void truncate(int pos) { if (pos < size()) resize(pos); }

public:
    typedef QByteArray::DataPtr DataPtr;
    inline DataPtr &data_ptr() { return d.data_ptr(); }
};

inline bool QBitArray::fill(bool aval, int asize)
{ *this = QBitArray((asize < 0 ? this->size() : asize), aval); return true; }

Q_CORE_EXPORT QBitArray operator&(const QBitArray &, const QBitArray &);
Q_CORE_EXPORT QBitArray operator|(const QBitArray &, const QBitArray &);
Q_CORE_EXPORT QBitArray operator^(const QBitArray &, const QBitArray &);

inline bool QBitArray::testBit(int i) const
{ Q_ASSERT(i >= 0 && i < size());
 return (*(reinterpret_cast<const uchar*>(d.constData())+1+(i>>3)) & (1 << (i & 7))) != 0; }

inline void QBitArray::setBit(int i)
{ Q_ASSERT(i >= 0 && i < size());
 *(reinterpret_cast<uchar*>(d.data())+1+(i>>3)) |= (1 << (i & 7)); }

inline void QBitArray::clearBit(int i)
{ Q_ASSERT(i >= 0 && i < size());
 *(reinterpret_cast<uchar*>(d.data())+1+(i>>3)) &= ~(1 << (i & 7)); }

inline void QBitArray::setBit(int i, bool val)
{ if (val) setBit(i); else clearBit(i); }

inline bool QBitArray::toggleBit(int i)
{ Q_ASSERT(i >= 0 &&  i < size());
 uchar b = 1<< (i&7); uchar* p = reinterpret_cast<uchar*>(d.data())+1+(i>>3);
 uchar c = *p&b; *p^=b; return c!=0; }

inline bool QBitArray::operator[](int i) const { return testBit(i); }
inline bool QBitArray::operator[](uint i) const { return testBit(i); }
inline bool QBitArray::at(int i) const { return testBit(i); }

class Q_CORE_EXPORT QBitRef
{
private:
    QBitArray& a;
    int i;
    inline QBitRef(QBitArray& array, int idx) : a(array), i(idx) {}
    friend class QBitArray;
public:
    inline operator bool() const { return a.testBit(i); }
    inline bool operator!() const { return !a.testBit(i); }
    QBitRef& operator=(const QBitRef& val) { a.setBit(i, val); return *this; }
    QBitRef& operator=(bool val) { a.setBit(i, val); return *this; }
};

inline QBitRef QBitArray::operator[](int i)
{ Q_ASSERT(i >= 0); return QBitRef(*this, i); }
inline QBitRef QBitArray::operator[](uint i)
{ return QBitRef(*this, i); }


#ifndef QT_NO_DATASTREAM
Q_CORE_EXPORT QDataStream &operator<<(QDataStream &, const QBitArray &);
Q_CORE_EXPORT QDataStream &operator>>(QDataStream &, QBitArray &);
#endif

Q_DECLARE_TYPEINFO(QBitArray, Q_MOVABLE_TYPE);
Q_DECLARE_SHARED(QBitArray)

QT_END_NAMESPACE

QT_END_HEADER

#endif // QBITARRAY_H
