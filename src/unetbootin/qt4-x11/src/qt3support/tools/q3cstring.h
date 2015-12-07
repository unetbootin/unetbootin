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
Q_COMPAT_EXPORT QDataStream &operator<<(QDataStream &d, const Q3CString &s);
Q_COMPAT_EXPORT QDataStream &operator>>(QDataStream &d, Q3CString &s);
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
