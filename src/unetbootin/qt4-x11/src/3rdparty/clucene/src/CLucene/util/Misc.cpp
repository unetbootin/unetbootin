/*
 * Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
 *
 * Distributable under the terms of either the Apache License (Version 2.0) or 
 * the GNU Lesser General Public License, as specified in the COPYING file.
 *
 * Changes are Copyright(C) 2007, 2008 by Nokia Corporation and/or its subsidiary(-ies), all rights reserved.
*/
#include "CLucene/StdHeader.h"
#include "Misc.h"

#ifdef _CL_TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# if defined(_CL_HAVE_SYS_TIME_H)
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif

#ifdef _CL_HAVE_SYS_TIMEB_H
# include <sys/timeb.h>
#endif

CL_NS_DEF(util)

uint64_t Misc::currentTimeMillis()
{
#if defined(_CLCOMPILER_MSVC) || defined(__MINGW32__) || defined(__BORLANDC__)
    struct _timeb tstruct;
    _ftime(&tstruct);

    return (((uint64_t) tstruct.time) * 1000) + tstruct.millitm;
#else
    struct timeval tstruct;
    if (gettimeofday(&tstruct, NULL) < 0) {
        _CLTHROWA(CL_ERR_Runtime,"Error in gettimeofday call.");
    }

    return (((uint64_t) tstruct.tv_sec) * 1000) + tstruct.tv_usec / 1000;
#endif
}

// #pragma mark -- char related utils

size_t Misc::ahashCode(const char* str)
{
	// Compute the hash code using a local variable to be reentrant.
	size_t hashCode = 0;
	while (*str != 0)
		hashCode = hashCode * 31 + *str++;
	return hashCode;
}

size_t Misc::ahashCode(const char* str, size_t len)
{
	// Compute the hash code using a local variable to be reentrant.
	size_t count = len;
    size_t hashCode = 0;
	for (size_t i = 0; i < count; i++)
		hashCode = hashCode * 31 + *str++;
	return hashCode;
}

char* Misc::ajoin(const char* a, const char* b, const char* c, const char* d,
    const char* e, const char* f)
{
#define aLEN(x) (x == NULL ? 0 : strlen(x))
    const size_t totalLen = aLEN(a) + aLEN(b) + aLEN(c) + aLEN(d) + aLEN(e)
        + aLEN(f) + sizeof(char); /* Space for terminator. */

    char* buf = _CL_NEWARRAY(char, totalLen);
    buf[0] = 0;
    if (a != NULL)
        strcat(buf, a);

    if (b != NULL)
        strcat(buf, b);

    if (c != NULL)
        strcat(buf, c);

    if (d != NULL)
        strcat(buf, d);

    if (e != NULL)
        strcat(buf, e);

    if (f != NULL)
        strcat(buf, f);

    return buf;
}

char* Misc::segmentname(const char* segment, const char* ext, int32_t x)
{
	CND_PRECONDITION(ext != NULL, "ext is NULL");

	char* buf = _CL_NEWARRAY(char, CL_MAX_PATH);
	if (x == -1)
		_snprintf(buf, CL_MAX_PATH, "%s%s", segment, ext);
	else
		_snprintf(buf, CL_MAX_PATH, "%s%s%d", segment, ext, x);
	return buf;
}

void Misc::segmentname(char* buffer, int32_t bufferLen, const char* segment,
    const char* ext, int32_t x)
{
	CND_PRECONDITION(buffer  != NULL, "buffer is NULL");
	CND_PRECONDITION(segment != NULL, "segment is NULL");
	CND_PRECONDITION(ext     != NULL, "extention is NULL");

	if (x == -1)
		_snprintf(buffer, bufferLen, "%s%s", segment, ext);
	else
		_snprintf(buffer, bufferLen, "%s%s%d", segment, ext, x);
}

// #pragma mark -- qt related utils

size_t Misc::qhashCode(const QString& str)
{
	size_t hashCode = 0;
	for (int i = 0; i < str.count(); ++i)
		hashCode = hashCode * 31 + str.at(i).unicode();
	return hashCode;
}

size_t Misc::qhashCode(const QString& str, size_t len)
{
    size_t count = len;
	size_t hashCode = 0;
	for (size_t i = 0; i < count; ++i)
		hashCode = hashCode * 31 + str.at(i).unicode();
	return hashCode;
}

QString Misc::qjoin(const QString &a, const QString &b, const QString &c,
    const QString &d, const QString &e, const QString &f)
{
    QString buffer;

    if (!a.isNull() && !a.isEmpty())
        buffer.append(a);

    if (!b.isNull() && !b.isEmpty())
        buffer.append(b);

    if (!c.isNull() && !c.isEmpty())
        buffer.append(c);

    if (!d.isNull() && !d.isEmpty())
        buffer.append(d);

    if (!e.isNull() && !e.isEmpty())
        buffer.append(e);

    if (!f.isNull() && !f.isEmpty())
        buffer.append(f);

    return buffer;
}

QString Misc::segmentname(const QString& segment, const QString& ext, int32_t x)
{
	CND_PRECONDITION(!ext.isEmpty(), "ext is NULL");
	
    if (x == -1)
        return QString(segment + ext);
	
    QString buf(QLatin1String("%1%2%3"));
	return buf.arg(segment).arg(ext).arg(x);
}

void Misc::segmentname(QString& buffer, int32_t bufferLen,
    const QString& segment, const QString& ext, int32_t x)
{
	CND_PRECONDITION(!segment.isEmpty(), "segment is NULL");
	CND_PRECONDITION(!ext.isEmpty(), "extention is NULL");

    buffer.clear();
    if (x == -1) {
		buffer = QString(segment + ext);
    } else {
        buffer = QString(QLatin1String("%1%2%3")).arg(segment).arg(ext).arg(x);
    }
}

// #pragma mark -- TCHAR related utils

int32_t Misc::stringDifference(const TCHAR* s1, int32_t len1, const TCHAR* s2,
    int32_t len2)
{
	int32_t len = len1 < len2 ? len1 : len2;
	for (int32_t i = 0; i < len; i++)
		if (s1[i] != s2[i])
			return i;
	return len;
}

/* DSR:CL_BUG: (See comment for join method in Misc.h): */
TCHAR* Misc::join (const TCHAR* a, const TCHAR* b, const TCHAR* c,
                   const TCHAR* d, const TCHAR* e, const TCHAR* f)
{
#define LEN(x) (x == NULL ? 0 : _tcslen(x))
    const size_t totalLen = LEN(a) + LEN(b) + LEN(c) + LEN(d) + LEN(e) + LEN(f)
        + sizeof(TCHAR); /* Space for terminator. */

    TCHAR* buf = _CL_NEWARRAY(TCHAR, totalLen);
    buf[0] = 0;
    if (a != NULL)
        _tcscat(buf, a);

    if (b != NULL)
        _tcscat(buf, b);

    if (c != NULL)
        _tcscat(buf, c);
    
    if (d != NULL)
        _tcscat(buf, d);

    if (e != NULL)
        _tcscat(buf, e);

    if (f != NULL)
        _tcscat(buf, f);

    return buf;
}

#ifdef _UCS2

size_t Misc::whashCode(const wchar_t* str)
{
	// Compute the hash code using a local variable to be reentrant.
	size_t hashCode = 0;
	while (*str != 0)
		hashCode = hashCode * 31 + *str++;
	return hashCode;
}

size_t Misc::whashCode(const wchar_t* str, size_t len)
{
	// Compute the hash code using a local variable to be reentrant.
    size_t count = len;
	size_t hashCode = 0;
	for (size_t i = 0; i < count; i++)
		hashCode = hashCode * 31 + *str++;
	return hashCode;
}

char* Misc::_wideToChar(const wchar_t* s CL_FILELINEPARAM)
{
   size_t len = _tcslen(s);
   char* msg = _CL_NEWARRAY(char, len + 1);
   _cpywideToChar(s, msg, len + 1);
   return msg;
}

void Misc::_cpywideToChar(const wchar_t* s, char* d, size_t len)
{
    size_t sLen = wcslen(s);
    for (uint32_t i = 0; i < len && i < sLen + 1; i++)
        d[i] = LUCENE_OOR_CHAR(s[i]);
}

wchar_t* Misc::_charToWide(const char* s CL_FILELINEPARAM)
{
   size_t len = strlen(s);
   wchar_t* msg = _CL_NEWARRAY(wchar_t, len + 1);
   _cpycharToWide(s, msg, len + 1);
   return msg;
}

void Misc::_cpycharToWide(const char* s, wchar_t* d, size_t len)
{
    size_t sLen = strlen(s);
    for (uint32_t i = 0; i < len && i < sLen + 1; i++)
      d[i] = s[i];
}

#endif

CL_NS_END
