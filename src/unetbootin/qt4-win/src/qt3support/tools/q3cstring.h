/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt3Support module of the Qt Toolkit.
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

#ifndef Q3CSTRING_H
#define Q3CSTRING_H

#include <QtCore/qbytearray.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3SupportLight)

/*****************************************************************************
  QCString class
 *****************************************************************************/

class QRegExp;

class Q_COMPAT_EXPORT Q3CString : public QByteArray
{
public:
    Q3CString() {}
    Q3CString(int size) : QByteArray(size, '\0') {}
    Q3CString(const Q3CString &s) : QByteArray(s) {}
    Q3CString(const QByteArray &ba) : QByteArray(ba) {}
    Q3CString(const char *str) : QByteArray(str) {}
    Q3CString(const char *str, uint maxlen) : QByteArray(str, qMin(qstrlen(str), maxlen - 1)) {}

    Q3CString    &operator=(const Q3CString &s) {
        QByteArray::operator=(s); return *this;
    }
    Q3CString    &operator=(const char *str) {
        QByteArray::operator=(str); return *this;
    }
    Q3CString    &operator=(const QByteArray &ba) {
        QByteArray::operator=(ba); return *this;
    }

    Q3CString        copy()        const { return *this; }
    Q3CString    &sprintf(const char *format, ...);

    Q3CString        left(uint len)  const { return QByteArray::left(len); }
    Q3CString        right(uint len) const { return QByteArray::right(len); }
    Q3CString        mid(uint index, uint len=0xffffffff) const { return QByteArray::mid(index, len); }

    Q3CString        leftJustify(uint width, char fill=' ', bool trunc=false)const;
    Q3CString        rightJustify(uint width, char fill=' ',bool trunc=false)const;

    Q3CString        lower() const { return QByteArray::toLower(); }
    Q3CString        upper() const { return QByteArray::toUpper(); }

    Q3CString        stripWhiteSpace()        const { return QByteArray::trimmed(); }
    Q3CString        simplifyWhiteSpace()        const { return QByteArray::simplified(); }

    Q3CString    &insert(uint index, const char *c) { QByteArray::insert(index, c); return *this; }
    Q3CString    &insert(uint index, char c) { QByteArray::insert(index, c); return *this; }
    Q3CString    &append(const char *c) { QByteArray::append(c); return *this; }
    Q3CString    &prepend(const char *c) { QByteArray::prepend(c); return *this; }
    Q3CString    &remove(uint index, uint len) { QByteArray::remove(index, len); return *this; }
    Q3CString    &replace(uint index, uint len, const char *c)
    { QByteArray::replace(index, len, c); return *this; }
    Q3CString    &replace(char c, const Q3CString &after) { return replace(c, after.constData()); }
    Q3CString    &replace(char c, const char *after) { QByteArray::replace(c, after); return *this; }
    Q3CString    &replace(const Q3CString &b, const Q3CString &a)
    { return replace(b.constData(), a.constData()); }
    Q3CString    &replace(const char *b, const char *a) { QByteArray::replace(b, a); return *this; }
    Q3CString    &replace(char b, char a) { QByteArray::replace(b, a); return *this; }

    short        toShort(bool *ok=0)        const;
    ushort        toUShort(bool *ok=0)        const;
    int                toInt(bool *ok=0)        const;
    uint        toUInt(bool *ok=0)        const;
    long        toLong(bool *ok=0)        const;
    ulong        toULong(bool *ok=0)        const;
    float        toFloat(bool *ok=0)        const;
    double        toDouble(bool *ok=0)        const;

    Q3CString    &setStr(const char *s) { *this = s; return *this; }
    Q3CString    &setNum(short);
    Q3CString    &setNum(ushort);
    Q3CString    &setNum(int);
    Q3CString    &setNum(uint);
    Q3CString    &setNum(long);
    Q3CString    &setNum(ulong);
    Q3CString    &setNum(float, char f='g', int prec=6);
    Q3CString    &setNum(double, char f='g', int prec=6);

    bool        setExpand(uint index, char c);

};


/*****************************************************************************
  Q3CString stream functions
 *****************************************************************************/
#ifndef QT_NO_DATASTREAM
inline Q_COMPAT_EXPORT QDataStream &operator<<(QDataStream &d, const Q3CString &s) {
    return operator<<(d, static_cast<const QByteArray &>(s));
}
inline Q_COMPAT_EXPORT QDataStream &operator>>(QDataStream &d, Q3CString &s) {
    return operator>>(d, static_cast<QByteArray &>(s));
}
#endif

/*****************************************************************************
  Q3CString inline functions
 *****************************************************************************/

inline Q3CString &Q3CString::setNum(short n)
{ return setNum(long(n)); }

inline Q3CString &Q3CString::setNum(ushort n)
{ return setNum(ulong(n)); }

inline Q3CString &Q3CString::setNum(int n)
{ return setNum(long(n)); }

inline Q3CString &Q3CString::setNum(uint n)
{ return setNum(ulong(n)); }

inline Q3CString &Q3CString::setNum(float n, char f, int prec)
{ return setNum(double(n),f,prec); }

/*****************************************************************************
  Q3CString non-member operators
 *****************************************************************************/

Q_COMPAT_EXPORT inline bool operator==(const Q3CString &s1, const Q3CString &s2)
{ return qstrcmp(s1, s2) == 0; }

Q_COMPAT_EXPORT inline bool operator==(const Q3CString &s1, const char *s2)
{ return qstrcmp(s1, s2) == 0; }

Q_COMPAT_EXPORT inline bool operator==(const char *s1, const Q3CString &s2)
{ return qstrcmp(s1, s2) == 0; }

Q_COMPAT_EXPORT inline bool operator!=(const Q3CString &s1, const Q3CString &s2)
{ return qstrcmp(s1, s2) != 0; }

Q_COMPAT_EXPORT inline bool operator!=(const Q3CString &s1, const char *s2)
{ return qstrcmp(s1, s2) != 0; }

Q_COMPAT_EXPORT inline bool operator!=(const char *s1, const Q3CString &s2)
{ return qstrcmp(s1, s2) != 0; }

Q_COMPAT_EXPORT inline bool operator<(const Q3CString &s1, const Q3CString& s2)
{ return qstrcmp(s1, s2) < 0; }

Q_COMPAT_EXPORT inline bool operator<(const Q3CString &s1, const char *s2)
{ return qstrcmp(s1, s2) < 0; }

Q_COMPAT_EXPORT inline bool operator<(const char *s1, const Q3CString &s2)
{ return qstrcmp(s1, s2) < 0; }

Q_COMPAT_EXPORT inline bool operator<=(const Q3CString &s1, const Q3CString &s2)
{ return qstrcmp(s1, s2) <= 0; }

Q_COMPAT_EXPORT inline bool operator<=(const Q3CString &s1, const char *s2)
{ return qstrcmp(s1, s2) <= 0; }

Q_COMPAT_EXPORT inline bool operator<=(const char *s1, const Q3CString &s2)
{ return qstrcmp(s1, s2) <= 0; }

Q_COMPAT_EXPORT inline bool operator>(const Q3CString &s1, const Q3CString &s2)
{ return qstrcmp(s1, s2) > 0; }

Q_COMPAT_EXPORT inline bool operator>(const Q3CString &s1, const char *s2)
{ return qstrcmp(s1, s2) > 0; }

Q_COMPAT_EXPORT inline bool operator>(const char *s1, const Q3CString &s2)
{ return qstrcmp(s1, s2) > 0; }

Q_COMPAT_EXPORT inline bool operator>=(const Q3CString &s1, const Q3CString& s2)
{ return qstrcmp(s1, s2) >= 0; }

Q_COMPAT_EXPORT inline bool operator>=(const Q3CString &s1, const char *s2)
{ return qstrcmp(s1, s2) >= 0; }

Q_COMPAT_EXPORT inline bool operator>=(const char *s1, const Q3CString &s2)
{ return qstrcmp(s1, s2) >= 0; }

Q_COMPAT_EXPORT inline const Q3CString operator+(const Q3CString &s1,
                                          const Q3CString &s2)
{
    Q3CString tmp(s1);
    tmp += s2;
    return tmp;
}
Q_COMPAT_EXPORT inline const Q3CString operator+(const Q3CString &s1,
                                          const QByteArray &s2)
{
    QByteArray tmp(s1);
    tmp += s2;
    return tmp;
}
Q_COMPAT_EXPORT inline const Q3CString operator+(const QByteArray &s1,
                                          const Q3CString &s2)
{
    QByteArray tmp(s1);
    tmp += s2;
    return tmp;
}

Q_COMPAT_EXPORT inline const Q3CString operator+(const Q3CString &s1, const char *s2)
{
    Q3CString tmp(s1);
    tmp += s2;
    return tmp;
}

Q_COMPAT_EXPORT inline const Q3CString operator+(const char *s1, const Q3CString &s2)
{
    Q3CString tmp(s1);
    tmp += s2;
    return tmp;
}

Q_COMPAT_EXPORT inline const Q3CString operator+(const Q3CString &s1, char c2)
{
    Q3CString tmp(s1);
    tmp += c2;
    return tmp;
}

Q_COMPAT_EXPORT inline const Q3CString operator+(char c1, const Q3CString &s2)
{
    Q3CString tmp;
    tmp += c1;
    tmp += s2;
    return tmp;
}

QT_END_NAMESPACE

QT_END_HEADER

#endif // Q3CSTRING_H
