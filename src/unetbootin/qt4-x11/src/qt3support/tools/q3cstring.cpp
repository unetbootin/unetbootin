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

#include "q3cstring.h"
#include "qregexp.h"
#include "qdatastream.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>

QT_BEGIN_NAMESPACE

/*****************************************************************************
  Q3CString member functions
 *****************************************************************************/

/*!
    \class Q3CString q3cstring.h
    \reentrant
    \brief The Q3CString class provides an abstraction of the classic C
    zero-terminated char array (char *).

    \compat

    Q3CString tries to behave like a more convenient \c{const char *}.
    The price of doing this is that some algorithms will perform
    badly. For example, append() is O(length()) since it scans for a
    null terminator. Although you might use Q3CString for text that is
    never exposed to the user, for most purposes, and especially for
    user-visible text, you should use QString. QString provides
    implicit sharing, Unicode and other internationalization support,
    and is well optimized.

    Note that for the Q3CString methods that take a \c{const char *}
    parameter the \c{const char *} must either be 0 (null) or not-null
    and '\0' (NUL byte) terminated; otherwise the results are
    undefined.

    A Q3CString that has not been assigned to anything is \e null, i.e.
    both the length and the data pointer is 0. A Q3CString that
    references the empty string ("", a single '\0' char) is \e empty.
    Both null and empty Q3CStrings are legal parameters to the methods.
    Assigning \c{const char *} 0 to Q3CString produces a null Q3CString.

    The length() function returns the length of the string; resize()
    resizes the string and truncate() truncates the string. A string
    can be filled with a character using fill(). Strings can be left
    or right padded with characters using leftJustify() and
    rightJustify(). Characters, strings and regular expressions can be
    searched for using find() and findRev(), and counted using
    contains().

    Strings and characters can be inserted with insert() and appended
    with append(). A string can be prepended with prepend().
    Characters can be removed from the string with remove() and
    replaced with replace().

    Portions of a string can be extracted using left(), right() and
    mid(). Whitespace can be removed using stripWhiteSpace() and
    simplifyWhiteSpace(). Strings can be converted to uppercase or
    lowercase with upper() and lower() respectively.

    Strings that contain numbers can be converted to numbers with
    toShort(), toInt(), toLong(), toULong(), toFloat() and toDouble().
    Numbers can be converted to strings with setNum().

    Many operators are overloaded to work with Q3CStrings. Q3CString
    also supports some more obscure functions, e.g. sprintf(),
    setStr() and setExpand().

    \sidebar Note on Character Comparisons

    In Q3CString the notion of uppercase and lowercase and of which
    character is greater than or less than another character is locale
    dependent. This affects functions which support a case insensitive
    option or which compare or lowercase or uppercase their arguments.
    Case insensitive operations and comparisons will be accurate if
    both strings contain only ASCII characters. (If \c $LC_CTYPE is
    set, most Unix systems do "the right thing".) Functions that this
    affects include contains(), find(), findRev(), \l operator<(), \l
    operator<=(), \l operator>(), \l operator>=(), lower() and
    upper().

    This issue does not apply to \l{QString}s since they represent
    characters using Unicode.
    \endsidebar

    Performance note: The Q3CString methods for QRegExp searching are
    implemented by converting the Q3CString to a QString and performing
    the search on that. This implies a deep copy of the Q3CString data.
    If you are going to perform many QRegExp searches on a large
    Q3CString, you will get better performance by converting the
    Q3CString to a QString yourself, and then searching in the QString.
*/

/*!
    \fn Q3CString Q3CString::left(uint len)  const

    \internal
*/

/*!
    \fn Q3CString Q3CString::right(uint len) const

    \internal
*/

/*!
    \fn Q3CString Q3CString::mid(uint index, uint len) const

    \internal
*/

/*!
    \fn Q3CString  Q3CString::lower() const

    Use QByteArray::toLower() instead.
*/

/*!
    \fn Q3CString  Q3CString::upper() const

    Use QByteArray::toUpper() instead.
*/

/*!
    \fn Q3CString  Q3CString::stripWhiteSpace() const

    Use QByteArray::trimmed() instead.
*/

/*!
    \fn Q3CString  Q3CString::simplifyWhiteSpace() const

    Use QByteArray::simplified() instead.
*/

/*!
    \fn Q3CString& Q3CString::insert(uint index, const char *c)

    \internal
*/

/*!
    \fn Q3CString& Q3CString::insert(uint index, char c)

    \internal
*/

/*!
    \fn Q3CString& Q3CString::prepend(const char *c)

    \internal
*/

/*!
    \fn Q3CString& Q3CString::remove(uint index, uint len)

    \internal
*/

/*!
    \fn Q3CString& Q3CString::replace(uint index, uint len, const char *c)

    \internal
*/

/*!
    \fn Q3CString& Q3CString::replace(char c, const Q3CString &after)

    \internal
*/

/*!
    \fn Q3CString& Q3CString::replace(char c, const char *after)

    \internal
*/

/*!
    \fn Q3CString& Q3CString::replace(const Q3CString &b, const Q3CString &a)

    \internal
*/

/*!
    \fn Q3CString& Q3CString::replace(const char *b, const char *a)

    \internal
*/

/*!
    \fn Q3CString& Q3CString::replace(char b, char a)

    \internal
*/

/*!
    \fn Q3CString::Q3CString()

    Constructs a null string.

    \sa isNull()
*/

/*!
    \fn Q3CString::Q3CString(const QByteArray &ba)

    Constructs a copy of \a ba.
*/

/*!
    \fn Q3CString::Q3CString(const Q3CString &s)

    Constructs a shallow copy \a s.
*/

/*! \fn Q3CString::Q3CString(int size)
    Constructs a string with room for \a size characters, including
    the '\0'-terminator. Makes a null string if \a size == 0.

    If \a size \> 0, then the first and last characters in the string
    are initialized to '\0'. All other characters are uninitialized.

    \sa resize(), isNull()
*/

/*! \fn Q3CString::Q3CString(const char *str)
    Constructs a string that is a deep copy of \a str.

    If \a str is 0 a null string is created.

    \sa isNull()
*/


/*! \fn Q3CString::Q3CString(const char *str, uint maxsize)

    Constructs a string that is a deep copy of \a str. The copy will
    be at most \a maxsize bytes long including the '\0'-terminator.

    Example:
    \snippet doc/src/snippets/code/src_qt3support_tools_q3cstring.cpp 0

    If \a str contains a 0 byte within the first \a maxsize bytes, the
    resulting Q3CString will be terminated by this 0. If \a str is 0 a
    null string is created.

    \sa isNull()
*/

/*!
    \fn Q3CString &Q3CString::operator=(const QByteArray &ba)

    Assigns byte array \a ba to this Q3CString.
*/

/*!
    \fn Q3CString &Q3CString::operator=(const Q3CString &s)

    Assigns a shallow copy of \a s to this string and returns a
    reference to this string.
*/

/*!
    \fn Q3CString &Q3CString::operator=(const char *str)
    \overload

    Assigns a deep copy of \a str to this string and returns a
    reference to this string.

    If \a str is 0 a null string is created.

    \sa isNull()
*/

/*
    \fn bool Q3CString::isNull() const

    Returns true if the string is null, i.e. if data() == 0; otherwise
    returns false. A null string is also an empty string.

    Example:
    \snippet doc/src/snippets/code/src.qt3support.tools.q3cstring.cpp 1

    \sa isEmpty(), length(), size()
*/

/*
    \fn bool Q3CString::isEmpty() const

    Returns true if the string is empty, i.e. if length() == 0;
    otherwise returns false. An empty string is not always a null
    string.

    See example in isNull().

    \sa isNull(), length(), size()
*/

/*
    \fn uint Q3CString::length() const

    Returns the length of the string, excluding the '\0'-terminator.
    Equivalent to calling \c strlen(data()).

    Null strings and empty strings have zero length.

    \sa size(), isNull(), isEmpty()
*/

/*
    \fn bool Q3CString::truncate(uint pos)

    Truncates the string at position \a pos.

    Equivalent to calling \c resize(pos+1).

    Example:
    \snippet doc/src/snippets/code/src.qt3support.tools.q3cstring.cpp 2

    \sa resize()
*/



/*!
    Implemented as a call to the native vsprintf() (see the manual for
    your C library).

    If the string is shorter than 256 characters, this sprintf() calls
    resize(256) to decrease the chance of memory corruption. The
    string is resized back to its actual length before sprintf()
    returns.

    Example:
    \snippet doc/src/snippets/code/src_qt3support_tools_q3cstring.cpp 3

    \warning All vsprintf() implementations will write past the end of
    the target string (*this) if the \a format specification and
    arguments happen to be longer than the target string, and some
    will also fail if the target string is longer than some arbitrary
    implementation limit.

    Giving user-supplied arguments to sprintf() is risky: Sooner or
    later someone will paste a huge line into your application.
*/

Q3CString &Q3CString::sprintf(const char *format, ...)
{
    detach();
    va_list ap;
    va_start(ap, format);
    if (size() < 256)
        resize(256);                // make string big enough
    qvsnprintf(data(), size(), format, ap);
    resize(qstrlen(constData()));
    va_end(ap);
    return *this;
}



/*!
    \fn Q3CString Q3CString::copy() const

    Returns a deep copy of this string.
*/


/*!
    Returns a string of length \a width (plus one for the terminating
    '\0') that contains this string padded with the \a fill character.

    If the length of the string exceeds \a width and \a truncate is
    false (the default), then the returned string is a copy of the
    string. If the length of the string exceeds \a width and \a
    truncate is true, then the returned string is a left(\a width).

    Example:
    \snippet doc/src/snippets/code/src_qt3support_tools_q3cstring.cpp 4

    \sa rightJustify()
*/

Q3CString Q3CString::leftJustify(uint width, char fill, bool truncate) const
{
    Q3CString result;
    int len = qstrlen(constData());
    int padlen = width - len;
    if (padlen > 0) {
        result.resize(len+padlen);
        memcpy(result.data(), constData(), len);
        memset(result.data()+len, fill, padlen);
    } else {
        if (truncate)
            result = left(width);
        else
            result = *this;
    }
    return result;
}

/*!
    Returns a string of length \a width (plus one for the terminating
    '\0') that contains zero or more of the \a fill character followed
    by this string.

    If the length of the string exceeds \a width and \a truncate is
    false (the default), then the returned string is a copy of the
    string. If the length of the string exceeds \a width and \a
    truncate is true, then the returned string is a left(\a width).

    Example:
    \snippet doc/src/snippets/code/src_qt3support_tools_q3cstring.cpp 5

    \sa leftJustify()
*/

Q3CString Q3CString::rightJustify(uint width, char fill, bool truncate) const
{
    Q3CString result;
    int len = qstrlen(constData());
    int padlen = width - len;
    if (padlen > 0) {
        result.resize(len+padlen);
        memset(result.data(), fill, padlen);
        memcpy(result.data()+padlen, constData(), len);
    } else {
        if (truncate)
            result = left(width);
        else
            result = *this;
    }
    return result;
}

/*!
    Returns the string converted to a \c long value.

    If \a ok is not 0: *\a ok is set to false if the string is not a
    number, or if it has trailing garbage; otherwise *\a ok is set to
    true.
*/

long Q3CString::toLong(bool *ok) const
{
    const char *p = constData();
    long val=0;
    const long max_mult = 214748364;
    bool is_ok = false;
    int neg = 0;
    if (!p)
        goto bye;
    while (isspace((uchar) *p))                // skip leading space
        p++;
    if (*p == '-') {
        p++;
        neg = 1;
    } else if (*p == '+') {
        p++;
    }
    if (!isdigit((uchar) *p))
        goto bye;
    while (isdigit((uchar) *p)) {
        if (val > max_mult || (val == max_mult && (*p-'0') > 7+neg))
            goto bye;
        val = 10*val + (*p++ - '0');
    }
    if (neg)
        val = -val;
    while (isspace((uchar) *p))                // skip trailing space
        p++;
    if (*p == '\0')
        is_ok = true;
bye:
    if (ok)
        *ok = is_ok;
    return is_ok ? val : 0;
}

/*!
    Returns the string converted to an \c{unsigned long} value.

    If \a ok is not 0: *\a ok is set to false if the string is not a
    number, or if it has trailing garbage; otherwise *\a ok is set to
    true.
*/

ulong Q3CString::toULong(bool *ok) const
{
    const char *p = constData();
    ulong val=0;
    const ulong max_mult = 429496729;
    bool is_ok = false;
    if (!p)
        goto bye;
    while (isspace((uchar) *p))                // skip leading space
        p++;
    if (*p == '+')
        p++;
    if (!isdigit((uchar) *p))
        goto bye;
    while (isdigit((uchar) *p)) {
        if (val > max_mult || (val == max_mult && (*p-'0') > 5))
            goto bye;
        val = 10*val + (*p++ - '0');
    }
    while (isspace((uchar) *p))                // skip trailing space
        p++;
    if (*p == '\0')
        is_ok = true;
bye:
    if (ok)
        *ok = is_ok;
    return is_ok ? val : 0;
}

/*!
    Returns the string converted to a \c{short} value.

    If \a ok is not 0: *\a ok is set to false if the string is not a
    number, is out of range, or if it has trailing garbage; otherwise
    *\a ok is set to true.
*/

short Q3CString::toShort(bool *ok) const
{
    long v = toLong(ok);
    if (ok && *ok && (v < -32768 || v > 32767))
        *ok = false;
    return (short)v;
}

/*!
    Returns the string converted to an \c{unsigned short} value.

    If \a ok is not 0: *\a ok is set to false if the string is not a
    number, is out of range, or if it has trailing garbage; otherwise
    *\a ok is set to true.
*/

ushort Q3CString::toUShort(bool *ok) const
{
    ulong v = toULong(ok);
    if (ok && *ok && (v > 65535))
        *ok = false;
    return (ushort)v;
}


/*!
    Returns the string converted to a \c{int} value.

    If \a ok is not 0: *\a ok is set to false if the string is not a
    number, or if it has trailing garbage; otherwise *\a ok is set to
    true.
*/

int Q3CString::toInt(bool *ok) const
{
    return (int)toLong(ok);
}

/*!
    Returns the string converted to an \c{unsigned int} value.

    If \a ok is not 0: *\a ok is set to false if the string is not a
    number, or if it has trailing garbage; otherwise *\a ok is set to
    true.
*/

uint Q3CString::toUInt(bool *ok) const
{
    return (uint)toULong(ok);
}

/*!
    Returns the string converted to a \c{double} value.

    If \a ok is not 0: *\a ok is set to false if the string is not a
    number, or if it has trailing garbage; otherwise *\a ok is set to
    true.
*/

double Q3CString::toDouble(bool *ok) const
{
    char *end;
    double val = strtod(constData() ? constData() : "", &end);
    if (ok)
        *ok = (constData() && *constData() && (end == 0 || *end == '\0'));
    return val;
}

/*!
    Returns the string converted to a \c{float} value.

    If \a ok is not 0: *\a ok is set to false if the string is not a
    number, or if it has trailing garbage; otherwise *\a ok is set to
    true.
*/

float Q3CString::toFloat(bool *ok) const
{
    return (float)toDouble(ok);
}


/*! \fn Q3CString &Q3CString::setStr(const char *str)
    Makes a deep copy of \a str. Returns a reference to the string.
*/

/*!
    \overload

    Sets the string to the string representation of the number \a n
    and returns a reference to the string.
*/

Q3CString &Q3CString::setNum(long n)
{
    data();
    char buf[20];
    register char *p = &buf[19];
    bool neg;
    if (n < 0) {
        neg = true;
        n = -n;
    } else {
        neg = false;
    }
    *p = '\0';
    do {
        *--p = ((int)(n%10)) + '0';
        n /= 10;
    } while (n);
    if (neg)
        *--p = '-';
    *this = p;
    return *this;
}

/*!
    \overload

    Sets the string to the string representation of the number \a n
    and returns a reference to the string.
*/

Q3CString &Q3CString::setNum(ulong n)
{
    data();
    char buf[20];
    register char *p = &buf[19];
    *p = '\0';
    do {
        *--p = ((int)(n%10)) + '0';
        n /= 10;
    } while (n);
    *this = p;
    return *this;
}

/*!
    \fn Q3CString &Q3CString::setNum(int n)
    \overload

    Sets the string to the string representation of the number \a n
    and returns a reference to the string.
*/

/*!
    \fn Q3CString &Q3CString::setNum(uint n)
    \overload

    Sets the string to the string representation of the number \a n
    and returns a reference to the string.
*/

/*!
    \fn Q3CString &Q3CString::setNum(short n)
    \overload

    Sets the string to the string representation of the number \a n
    and returns a reference to the string.
*/

/*!
    \fn Q3CString &Q3CString::setNum(ushort n)
    \overload

    Sets the string to the string representation of the number \a n
    and returns a reference to the string.
*/

/*!
    Sets the string to the string representation of the number \a n
    and returns a reference to the string.

    The format of the string representation is specified by the format
    character \a f, and the precision (number of digits after the
    decimal point) is specified with \a prec.

    The valid formats for \a f are 'e', 'E', 'f', 'g' and 'G'. The
    formats are the same as for sprintf(); they are explained in \l
    QString::arg().
*/

Q3CString &Q3CString::setNum(double n, char f, int prec)
{
#ifndef QT_NO_DEBUG
    if (!(f=='f' || f=='F' || f=='e' || f=='E' || f=='g' || f=='G'))
        qWarning("Q3CString::setNum: Invalid format char '%c'", f);
#endif
    char format[20];
    register char *fs = format;                        // generate format string
    *fs++ = '%';                                //   "%.<prec>l<f>"
    if (prec > 99)
        prec = 99;
    *fs++ = '.';
    if (prec >= 10) {
        *fs++ = prec / 10 + '0';
        *fs++ = prec % 10 + '0';
    } else {
        *fs++ = prec + '0';
    }
    *fs++ = 'l';
    *fs++ = f;
    *fs = '\0';
    return sprintf(format, n);
}

/*! \fn Q3CString &Q3CString::setNum(float n, char f, int prec)
    \overload
*/

/*!
    Sets the character at position \a index to \a c and expands the
    string if necessary, padding with spaces.

    Returns false if \a index was out of range and the string could
    not be expanded; otherwise returns true.
*/

bool Q3CString::setExpand(uint index, char c)
{
    uint oldlen = length();
    if (index >= oldlen) {
        resize(index+1);
        if (index > oldlen)
            memset(data() + oldlen, ' ', index - oldlen);
    }
    *(data() + index) = c;
    return true;
}


/*
    \fn Q3CString::operator const char *() const

    Returns the string data.
*/


/*!
    \fn Q3CString& Q3CString::append(const char *str)

    Appends string \a str to the string and returns a reference to the
    string. Equivalent to operator+=().
*/



#ifndef QT_NO_DATASTREAM
/*! \fn QDataStream &operator<<(QDataStream &s, const Q3CString &str)
    \relates Q3CString

    Writes string \a str to the stream \a s.

    \sa \link datastreamformat.html Format of the QDataStream operators \endlink
*/
QDataStream &operator<<(QDataStream &d, const Q3CString &s)
{
    if (d.version() >= QDataStream::Qt_4_0)
        return operator<<(d, static_cast<const QByteArray &>(s));

    // we need to add a NUL to keep compatibility with Qt 3's QByteArray
    QByteArray copy = s;
    copy.append('\0');
    return operator<<(d, copy);
}

/*!
  \fn QDataStream &operator>>(QDataStream &s, Q3CString &str)
    \relates Q3CString

    Reads a string into \a str from the stream \a s.

    \sa \link datastreamformat.html Format of the QDataStream operators \endlink
*/
QDataStream &operator>>(QDataStream &d, Q3CString &s) {
    operator>>(d, static_cast<QByteArray &>(s));
    if (d.version() < QDataStream::Qt_4_0 && s.endsWith('\0'))
        s.chop(1);                  // ending NUL
    return d;
}
#endif

/*****************************************************************************
  Documentation for related functions
 *****************************************************************************/

/*!
    \fn bool operator==(const Q3CString &s1, const Q3CString &s2)

    \relates Q3CString

    Returns true if \a s1 and \a s2 are equal; otherwise returns false.

    Equivalent to qstrcmp(\a s1, \a s2) == 0.
*/

/*!
    \fn bool operator==(const Q3CString &s1, const char *s2)
    \overload

    \relates Q3CString

    Returns true if \a s1 and \a s2 are equal; otherwise returns false.

    Equivalent to qstrcmp(\a s1, \a s2) == 0.
*/

/*!
    \fn bool operator==(const char *s1, const Q3CString &s2)
    \overload

    \relates Q3CString

    Returns true if \a s1 and \a s2 are equal; otherwise returns false.

    Equivalent to qstrcmp(\a s1, \a s2) == 0.
*/

/*!
    \fn bool operator!=(const Q3CString &s1, const Q3CString &s2)

    \relates Q3CString

    Returns true if \a s1 and \a s2 are different; otherwise returns false.

    Equivalent to qstrcmp(\a s1, \a s2) != 0.
*/

/*!
    \fn bool operator!=(const Q3CString &s1, const char *s2)
    \overload

    \relates Q3CString

    Returns true if \a s1 and \a s2 are different; otherwise returns false.

    Equivalent to qstrcmp(\a s1, \a s2) != 0.
*/

/*!
    \fn bool operator!=(const char *s1, const Q3CString &s2)
    \overload

    \relates Q3CString

    Returns true if \a s1 and \a s2 are different; otherwise returns false.

    Equivalent to qstrcmp(\a s1, \a s2) != 0.
*/

/*!
    \fn bool operator<(const Q3CString &s1, const char *s2)

    \relates Q3CString

    Returns true if \a s1 is less than \a s2; otherwise returns false.

    Equivalent to qstrcmp(\a s1, \a s2) \< 0.
*/

/*!
    \fn bool operator<(const char *s1, const Q3CString &s2)
    \overload

    \relates Q3CString

    Returns true if \a s1 is less than \a s2; otherwise returns false.

    Equivalent to qstrcmp(\a s1, \a s2) \< 0.
*/

/*!
    \fn bool operator<=(const Q3CString &s1, const char *s2)

    \relates Q3CString

    Returns true if \a s1 is less than or equal to \a s2; otherwise
    returns false.

    Equivalent to qstrcmp(\a s1, \a s2) \<= 0.
*/

/*!
    \fn bool operator<=(const char *s1, const Q3CString &s2)
    \overload

    \relates Q3CString

    Returns true if \a s1 is less than or equal to \a s2; otherwise
    returns false.

    Equivalent to qstrcmp(\a s1, \a s2) \<= 0.
*/

/*!
    \fn bool operator>(const Q3CString &s1, const char *s2)

    \relates Q3CString

    Returns true if \a s1 is greater than \a s2; otherwise returns false.

    Equivalent to qstrcmp(\a s1, \a s2) \> 0.
*/

/*!
    \fn bool operator>(const char *s1, const Q3CString &s2)
    \overload

    \relates Q3CString

    Returns true if \a s1 is greater than \a s2; otherwise returns false.

    Equivalent to qstrcmp(\a s1, \a s2) \> 0.
*/

/*!
    \fn bool operator>=(const Q3CString &s1, const char *s2)

    \relates Q3CString

    Returns true if \a s1 is greater than or equal to \a s2; otherwise
    returns false.

    Equivalent to qstrcmp(\a s1, \a s2) \>= 0.
*/

/*!
    \fn bool operator>=(const char *s1, const Q3CString &s2)
    \overload

    \relates Q3CString

    Returns true if \a s1 is greater than or equal to \a s2; otherwise
    returns false.

    Equivalent to qstrcmp(\a s1, \a s2) \>= 0.
*/

/*!
    \fn const Q3CString operator+(const Q3CString &s1, const Q3CString &s2)

    \relates Q3CString

    Returns a string which consists of the concatenation of \a s1 and
    \a s2.
*/

/*!
    \fn const Q3CString operator+(const Q3CString &s1, const char *s2)
    \overload

    \relates Q3CString

    Returns a string which consists of the concatenation of \a s1 and \a s2.
*/

/*!
    \fn const Q3CString operator+(const char *s1, const Q3CString &s2)
    \overload

    \relates Q3CString

    Returns a string which consists of the concatenation of \a s1 and \a s2.
*/

/*!
    \fn const Q3CString operator+(const Q3CString &s, char c)
    \overload

    \relates Q3CString

    Returns a string which consists of the concatenation of \a s and \a c.
*/

/*!
    \fn const Q3CString operator+(char c, const Q3CString &s)
    \overload

    \relates Q3CString
    Returns a string which consists of the concatenation of \a c and \a s.
*/

QT_END_NAMESPACE
