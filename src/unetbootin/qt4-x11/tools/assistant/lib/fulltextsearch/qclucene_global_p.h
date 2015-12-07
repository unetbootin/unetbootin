/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QCLucene library and is distributable under
** the terms of the LGPL license as specified in the license.txt file.
**
****************************************************************************/

#ifndef QCLUCENE_GLOBAL_P_H
#define QCLUCENE_GLOBAL_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API. It exists for the convenience
// of the help generator tools. This header file may change from version
// to version without notice, or even be removed.
//
// We mean it.
//

#if !defined(_MSC_VER)
#   include "qclucene-config_p.h"
#endif

#include <QtCore/QChar>
#include <QtCore/QString>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

#if !defined(QT_SHARED) && !defined(QT_DLL)
#   define QHELP_EXPORT
#elif defined(QHELP_LIB)
#   define QHELP_EXPORT Q_DECL_EXPORT
#else
#   define QHELP_EXPORT Q_DECL_IMPORT
#endif

//
//  W A R N I N G
//  -------------
//
// adjustments here, need to be done in 
// QTDIR/src/3rdparty/clucene/src/CLucene/StdHeader.h as well
//
#if defined(_LUCENE_DONTIMPLEMENT_NS_MACROS)

#elif !defined(DISABLE_NAMESPACE)
#   ifdef QT_NAMESPACE
#       define CL_NS_DEF(sub) namespace QT_NAMESPACE { namespace lucene{ namespace sub{
#       define CL_NS_DEF2(sub,sub2) namespace QT_NAMESPACE { namespace lucene{ namespace sub{ namespace sub2 {

#       define CL_NS_END }}}
#       define CL_NS_END2 }}}}

#       define CL_NS_USE(sub) using namespace QT_NAMESPACE::lucene::sub;
#       define CL_NS_USE2(sub,sub2) using namespace QT_NAMESPACE::lucene::sub::sub2;

#       define CL_NS(sub) QT_NAMESPACE::lucene::sub
#       define CL_NS2(sub,sub2) QT_NAMESPACE::lucene::sub::sub2
#   else
#       define CL_NS_DEF(sub) namespace lucene{ namespace sub{
#       define CL_NS_DEF2(sub,sub2) namespace lucene{ namespace sub{ namespace sub2 {

#       define CL_NS_END }}
#       define CL_NS_END2 }}}

#       define CL_NS_USE(sub) using namespace lucene::sub;
#       define CL_NS_USE2(sub,sub2) using namespace lucene::sub::sub2;

#       define CL_NS(sub) lucene::sub
#       define CL_NS2(sub,sub2) lucene::sub::sub2
#   endif
#else
#   define CL_NS_DEF(sub)
#   define CL_NS_DEF2(sub, sub2)
#   define CL_NS_END
#   define CL_NS_END2
#   define CL_NS_USE(sub)
#   define CL_NS_USE2(sub,sub2)
#   define CL_NS(sub)
#   define CL_NS2(sub,sub2)
#endif

#if !defined(_MSC_VER) && defined(_CL_HAVE_WCHAR_H) && defined(_CL_HAVE_WCHAR_T)
#   if !defined(TCHAR)
#       define TCHAR wchar_t
#   endif
#else
#   include <windows.h>
#endif

namespace {
    TCHAR* QStringToTChar(const QString &str)
    {
        TCHAR *string = new TCHAR[(str.length() +1) * sizeof(TCHAR)];
        memset(string, 0, (str.length() +1) * sizeof(TCHAR));
        #if defined(UNICODE) || defined(_CL_HAVE_WCHAR_H) && defined(_CL_HAVE_WCHAR_T)
            str.toWCharArray(string);
        #else
            const QByteArray ba = str.toAscii();
            strcpy(string, ba.constData());
        #endif
        return string;
    }

    QString TCharToQString(const TCHAR *string)
    {
        #if defined(UNICODE) || defined(_CL_HAVE_WCHAR_H) && defined(_CL_HAVE_WCHAR_T)
            QString retValue = QString::fromWCharArray(string);
            return retValue;
        #else
            return QString(QLatin1String(string));
        #endif
    }
}

QT_END_NAMESPACE

QT_END_HEADER

#endif // QCLUCENE_GLOBAL_P_H
