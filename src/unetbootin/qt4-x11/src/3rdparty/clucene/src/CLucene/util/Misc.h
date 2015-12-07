/*
 * Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
 *
 * Distributable under the terms of either the Apache License (Version 2.0) or 
 * the GNU Lesser General Public License, as specified in the COPYING file.
 *
 * Changes are Copyright(C) 2007, 2008 by Nokia Corporation and/or its subsidiary(-ies), all rights reserved.
*/
#ifndef _lucene_util_Misc_H
#define _lucene_util_Misc_H

#if defined(_LUCENE_PRAGMA_ONCE)
#   pragma once
#endif

#include <QtCore/QString>

CL_NS_DEF(util)

class Misc
{
public:
    static uint64_t currentTimeMillis();

    static size_t ahashCode(const char* str);
    static size_t ahashCode(const char* str, size_t len);
    static char* ajoin(const char* a, const char* b, const char* c = NULL,
        const char* d = NULL, const char* e = NULL, const char* f = NULL);
    static char* segmentname(const char* segment, const char* ext, int32_t x = -1);
    static void segmentname(char* buffer, int32_t bufferLen, const char* segment,
        const char* ext, int32_t x = -1);

    static size_t qhashCode(const QString& str);
    static size_t qhashCode(const QString& str, size_t len);
    static QString qjoin(const QString& a, const QString& b,
        const QString& c = QString(), const QString& d = QString(),
        const QString& e = QString(), const QString& f = QString());
    static QString segmentname(const QString& segment, const QString& ext,
        int32_t x = -1 );
    static void segmentname(QString& buffer, int32_t bufferLen,
        const QString& Segment, const QString& ext, int32_t x = -1);

    // Compares two strings, character by character, and returns the
    // first position where the two strings differ from one another.
    //
    // @param s1 The first string to compare
    // @param s1Len The length of the first string to compare
    // @param s2 The second string to compare
    // @param s2Len The length of the second string to compare
    // @return The first position where the two strings differ.
    static int32_t stringDifference(const TCHAR* s1, int32_t s1Len,
        const TCHAR* s2, int32_t s2Len);
    static TCHAR* join (const TCHAR* a, const TCHAR* b, const TCHAR* c = NULL,
        const TCHAR* d = NULL, const TCHAR* e = NULL, const TCHAR* f = NULL );

#ifdef _UCS2
    static size_t whashCode(const wchar_t* str);
    static size_t whashCode(const wchar_t* str, size_t len);

#   define thashCode whashCode

    static char* _wideToChar(const wchar_t* s CL_FILELINEPARAM);
    static void _cpywideToChar(const wchar_t* s, char* d, size_t len);

    static wchar_t* _charToWide(const char* s CL_FILELINEPARAM);
    static void _cpycharToWide(const char* s, wchar_t* d, size_t len);

#else
#   define thashCode ahashCode
#endif
};

CL_NS_END

#endif
