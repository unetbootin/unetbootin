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

#ifndef QATOMIC_BFIN_H
#define QATOMIC_BFIN_H

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

#define Q_ATOMIC_INT_REFERENCE_COUNTING_IS_NOT_NATIVE

inline bool QBasicAtomicInt::isReferenceCountingNative()
{ return false; }
inline bool QBasicAtomicInt::isReferenceCountingWaitFree()
{ return false; }

#define Q_ATOMIC_INT_TEST_AND_SET_IS_NOT_NATIVE

inline bool QBasicAtomicInt::isTestAndSetNative()
{ return false; }
inline bool QBasicAtomicInt::isTestAndSetWaitFree()
{ return false; }

#define Q_ATOMIC_INT_FETCH_AND_STORE_IS_NOT_NATIVE

inline bool QBasicAtomicInt::isFetchAndStoreNative()
{ return false; }
inline bool QBasicAtomicInt::isFetchAndStoreWaitFree()
{ return false; }

#define Q_ATOMIC_INT_FETCH_AND_ADD_IS_NOT_NATIVE

inline bool QBasicAtomicInt::isFetchAndAddNative()
{ return false; }
inline bool QBasicAtomicInt::isFetchAndAddWaitFree()
{ return false; }

#define Q_ATOMIC_POINTER_TEST_AND_SET_IS_NOT_NATIVE

template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::isTestAndSetNative()
{ return false; }
template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::isTestAndSetWaitFree()
{ return false; }

#define Q_ATOMIC_POINTER_FETCH_AND_STORE_IS_NOT_NATIVE

template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::isFetchAndStoreNative()
{ return false; }
template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::isFetchAndStoreWaitFree()
{ return false; }

#define Q_ATOMIC_POINTER_FETCH_AND_ADD_IS_NOT_NATIVE

template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::isFetchAndAddNative()
{ return false; }
template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::isFetchAndAddWaitFree()
{ return false; }

#if defined(Q_OS_LINUX) && defined(Q_CC_GNU)

QT_BEGIN_INCLUDE_NAMESPACE
#include <asm/fixed_code.h>
QT_END_INCLUDE_NAMESPACE

inline bool QBasicAtomicInt::ref()
{
    int ret;
    asm volatile("R0 = 1;\n\t"
		 "P0 = %3;\n\t"
                 "CALL (%2);\n\t"
                 "%0 = R0;"
                 : "=da" (ret), "=m" (_q_value)
                 : "a" (ATOMIC_ADD32), "da" (&_q_value), "m" (_q_value)
                 : "R0", "R1", "P0", "RETS", "memory");
    return ret != 0;
}

inline bool QBasicAtomicInt::deref()
{
    int ret;
    asm volatile("R0 = 1;\n\t"
		 "P0 = %3;\n\t"
                 "CALL (%2);\n\t"
                 "%0 = R0;"
                 : "=da" (ret), "=m" (_q_value)
                 : "a" (ATOMIC_SUB32), "da" (&_q_value), "m" (_q_value)
                 : "R0", "R1", "P0", "RETS", "memory");
    return ret != 0;
}

inline int QBasicAtomicInt::testAndSetOrdered(int expectedValue, int newValue)
{
    long int readval;
    asm volatile ("P0 = %2;\n\t"
		  "R1 = %3;\n\t"
		  "R2 = %4;\n\t"
		  "CALL (%5);\n\t"
		  "%0 = R0;\n\t"
		  : "=da" (readval), "=m" (_q_value)
		  : "da" (&_q_value),
		  "da" (expectedValue),
		  "da" (newValue),
		  "a" (ATOMIC_CAS32),
		  "m" (_q_value)
		  : "P0", "R0", "R1", "R2", "RETS", "memory", "cc");
    return readval == expectedValue;
}

inline int QBasicAtomicInt::fetchAndStoreOrdered(int newValue)
{
    asm volatile("R1 = %2;\n\t"
		 "P0 = %4;\n\t"
                 "CALL (%3);\n\t"
                 "%0 = R0;"
                 : "=da" (newValue), "=m" (_q_value)
                 : "da" (newValue), "a" (ATOMIC_XCHG32), "da" (&_q_value), "m" (_q_value)
                 : "R0", "R1", "P0", "RETS", "memory");
    return newValue;
}

#error "QBasicAtomicInt::fetchAndAddOrdered() not implemented"

template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::testAndSetOrdered(T *expectedValue, T *newValue)
{
    T *readval;
    asm volatile ("P0 = %2;\n\t"
		  "R1 = %3;\n\t"
		  "R2 = %4;\n\t"
		  "CALL (%5);\n\t"
		  "%0 = R0;\n\t"
		  : "=da" (readval), "=m" (_q_value)
		  : "da" (&_q_value),
		  "da" (expectedValue),
		  "da" (newValue),
		  "a" (ATOMIC_CAS32),
		  "m" (_q_value)
		  : "P0", "R0", "R1", "R2", "RETS", "memory", "cc");
    return readval == expectedValue;
}

template <typename T>
Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndStoreOrdered(T *newValue)
{
    asm volatile("R1 = %2;\n\t"
		 "P0 = %4;\n\t"
                 "CALL (%3);\n\t"
                 "%0 = R0;"
                 : "=da" (newValue), "=m" (_q_value)
                 : "da" (newValue), "a" (ATOMIC_XCHG32), "da" (&_q_value), "m" (_q_value)
                 : "R0", "R1", "P0", "RETS", "memory");
    return newValue;
}

#error "QBasicAtomicPointer<T>::fetchAndAddOrdered() not implemented"

#endif // Q_OS_LINUX && Q_CC_GNU

// Test and set for integers

inline bool QBasicAtomicInt::testAndSetRelaxed(int expectedValue, int newValue)
{
    return testAndSetOrdered(expectedValue, newValue);
}

inline bool QBasicAtomicInt::testAndSetAcquire(int expectedValue, int newValue)
{
    return testAndSetOrdered(expectedValue, newValue);
}

inline bool QBasicAtomicInt::testAndSetRelease(int expectedValue, int newValue)
{
    return testAndSetOrdered(expectedValue, newValue);
}

// Fetch and store for integers

inline int QBasicAtomicInt::fetchAndStoreRelaxed(int newValue)
{
    return fetchAndStoreOrdered(newValue);
}

inline int QBasicAtomicInt::fetchAndStoreAcquire(int newValue)
{
    return fetchAndStoreOrdered(newValue);
}

inline int QBasicAtomicInt::fetchAndStoreRelease(int newValue)
{
    return fetchAndStoreOrdered(newValue);
}

// Fetch and add for integers

inline int QBasicAtomicInt::fetchAndAddRelaxed(int valueToAdd)
{
    return fetchAndAddOrdered(valueToAdd);
}

inline int QBasicAtomicInt::fetchAndAddAcquire(int valueToAdd)
{
    return fetchAndAddOrdered(valueToAdd);
}

inline int QBasicAtomicInt::fetchAndAddRelease(int valueToAdd)
{
    return fetchAndAddOrdered(valueToAdd);
}

// Test and set for pointers

template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::testAndSetRelaxed(T *expectedValue, T *newValue)
{
    return testAndSetOrdered(expectedValue, newValue);
}

template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::testAndSetAcquire(T *expectedValue, T *newValue)
{
    return testAndSetOrdered(expectedValue, newValue);
}

template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::testAndSetRelease(T *expectedValue, T *newValue)
{
    return testAndSetOrdered(expectedValue, newValue);
}

// Fetch and store for pointers

template <typename T>
Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndStoreRelaxed(T *newValue)
{
    return fetchAndStoreOrdered(newValue);
}

template <typename T>
Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndStoreAcquire(T *newValue)
{
    return fetchAndStoreOrdered(newValue);
}

template <typename T>
Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndStoreRelease(T *newValue)
{
    return fetchAndStoreOrdered(newValue);
}

// Fetch and add for pointers

template <typename T>
Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndAddRelaxed(qptrdiff valueToAdd)
{
    return fetchAndAddOrdered(valueToAdd);
}

template <typename T>
Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndAddAcquire(qptrdiff valueToAdd)
{
    return fetchAndAddOrdered(valueToAdd);
}

template <typename T>
Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndAddRelease(qptrdiff valueToAdd)
{
    return fetchAndAddOrdered(valueToAdd);
}

QT_END_NAMESPACE

QT_END_HEADER

#endif // QATOMIC_BFIN_H
