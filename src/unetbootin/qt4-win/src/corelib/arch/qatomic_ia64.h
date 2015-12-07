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

#ifndef QATOMIC_IA64_H
#define QATOMIC_IA64_H

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

#define Q_ATOMIC_INT_REFERENCE_COUNTING_IS_ALWAYS_NATIVE

inline bool QBasicAtomicInt::isReferenceCountingNative()
{ return true; }
inline bool QBasicAtomicInt::isReferenceCountingWaitFree()
{ return false; }

#define Q_ATOMIC_INT_TEST_AND_SET_IS_ALWAYS_NATIVE
#define Q_ATOMIC_INT_TEST_AND_SET_IS_WAIT_FREE

inline bool QBasicAtomicInt::isTestAndSetNative()
{ return true; }
inline bool QBasicAtomicInt::isTestAndSetWaitFree()
{ return true; }

#define Q_ATOMIC_INT_FETCH_AND_STORE_IS_ALWAYS_NATIVE

inline bool QBasicAtomicInt::isFetchAndStoreNative()
{ return true; }
inline bool QBasicAtomicInt::isFetchAndStoreWaitFree()
{ return false; }

#define Q_ATOMIC_INT_FETCH_AND_ADD_IS_ALWAYS_NATIVE

inline bool QBasicAtomicInt::isFetchAndAddNative()
{ return true; }
inline bool QBasicAtomicInt::isFetchAndAddWaitFree()
{ return false; }

#define Q_ATOMIC_POINTER_TEST_AND_SET_IS_ALWAYS_NATIVE
#define Q_ATOMIC_POINTER_TEST_AND_SET_IS_WAIT_FREE

template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::isTestAndSetNative()
{ return true; }
template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::isTestAndSetWaitFree()
{ return true; }

#define Q_ATOMIC_POINTER_FETCH_AND_STORE_IS_ALWAYS_NATIVE

template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::isFetchAndStoreNative()
{ return true; }
template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::isFetchAndStoreWaitFree()
{ return false; }

#define Q_ATOMIC_POINTER_FETCH_AND_ADD_IS_ALWAYS_NATIVE

template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::isFetchAndAddNative()
{ return true; }
template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::isFetchAndAddWaitFree()
{ return false; }

#if defined(Q_CC_INTEL)

// intrinsics provided by the Intel C++ Compiler
#include <ia64intrin.h>

inline bool QBasicAtomicInt::testAndSetRelaxed(int expectedValue, int newValue)

    return (static_cast<int>(_InterlockedCompareExchange(&static_cast<unsigned int>(_q_value),
                                                         newValue,
                                                         expectedValue))
            == expectedValue);
}

inline bool QBasicAtomicInt::testAndSetAcquire(int expectedValue, int newValue)
{
    return (static_cast<int>(_InterlockedCompareExchange_acq(&static_cast<unsigned int>(_q_value),
                                                             newValue,
                                                             expectedValue))
            == expectedValue);
}

inline bool QBasicAtomicInt::testAndSetRelease(int expectedValue, int newValue)
{
    return (static_cast<int>(_InterlockedCompareExchange_rel(&static_cast<unsigned int>(_q_value),
                                                             newValue,
                                                             expectedValue))
            == expectedValue);
}

inline bool QBasicAtomicInt::testAndSetOrdered(int expectedValue, int newValue)
{
    __memory_barrier();
    return testAndSetAcquire(expectedValue, newValue);
}

template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::testAndSetRelaxed(T *expectedValue, T *newValue)
{
    return _InterlockedCompareExchangePointer(reinterpret_cast<void * volatile*>(ptr),
                                              newval, expected) == expected;
}

template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::testAndSetAcquire(T *expectedValue, T *newValue)
{
    return (_InterlockedCompareExchange64_acq(&reinterpret_cast<quintptr>(_q_value),
                                              quintptr(newValue),
                                              quintptr(expectedValue))
            == quintptr(expected));
}

template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::testAndSetRelease(T *expectedValue, T *newValue)
{
    return (_InterlockedCompareExchange64_rel(&reinterpret_cast<quintptr>(_q_value),
                                              quintptr(newValue),
                                              quintptr(expectedValue))
            == quintptr(expected));
}

template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::testAndSetOrdered(T *expectedValue, T *newValue)
{
    __memory_barrier();
    return testAndSetAcquire(expectedValue, newValue);
}

#else // !Q_CC_INTEL

#  if defined(Q_CC_GNU)

inline bool QBasicAtomicInt::testAndSetAcquire(int expectedValue, int newValue)
{
    int ret;
    asm volatile("mov ar.ccv=%2\n"
                 ";;\n"
                 "cmpxchg4.acq %0=%1,%3,ar.ccv\n"
                 : "=r" (ret), "+m" (_q_value)
                 : "r" (expectedValue), "r" (newValue)
                 : "memory");
    return ret == expectedValue;
}

inline bool QBasicAtomicInt::testAndSetRelease(int expectedValue, int newValue)
{
    int ret;
    asm volatile("mov ar.ccv=%2\n"
                 ";;\n"
                 "cmpxchg4.rel %0=%1,%3,ar.ccv\n"
                 : "=r" (ret), "+m" (_q_value)
                 : "r" (expectedValue), "r" (newValue)
                 : "memory");
    return ret == expectedValue;
}

template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::testAndSetAcquire(T *expectedValue, T *newValue)
{
    T *ret;
    asm volatile("mov ar.ccv=%2\n"
                 ";;\n"
                 "cmpxchg8.acq %0=%1,%3,ar.ccv\n"
                 : "=r" (ret), "+m" (_q_value)
                 : "r" (expectedValue), "r" (newValue)
                 : "memory");
    return ret == expectedValue;
}

template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::testAndSetRelease(T *expectedValue, T *newValue)
{
    T *ret;
    asm volatile("mov ar.ccv=%2\n"
                 ";;\n"
                 "cmpxchg8.rel %0=%1,%3,ar.ccv\n"
                 : "=r" (ret), "+m" (_q_value)
                 : "r" (expectedValue), "r" (newValue)
                 : "memory");
    return ret == expectedValue;
}

#elif defined Q_CC_HPACC

QT_BEGIN_INCLUDE_NAMESPACE
#include <ia64/sys/inline.h>
QT_END_INCLUDE_NAMESPACE

#define FENCE (_Asm_fence)(_UP_CALL_FENCE | _UP_SYS_FENCE | _DOWN_CALL_FENCE | _DOWN_SYS_FENCE)

inline bool QBasicAtomicInt::testAndSetAcquire(int expectedValue, int newValue)
{
    _Asm_mov_to_ar((_Asm_app_reg)_AREG_CCV, (unsigned)expectedValue, FENCE);
    int ret = _Asm_cmpxchg((_Asm_sz)_SZ_W, (_Asm_sem)_SEM_ACQ,
                           &_q_value, (unsigned)newValue, (_Asm_ldhint)_LDHINT_NONE);
    return ret == expectedValue;
}

inline bool QBasicAtomicInt::testAndSetRelease(int expectedValue, int newValue)
{
    _Asm_mov_to_ar((_Asm_app_reg)_AREG_CCV, (unsigned)expectedValue, FENCE);
    int ret = _Asm_cmpxchg((_Asm_sz)_SZ_W, (_Asm_sem)_SEM_REL,
                           &_q_value, newValue, (_Asm_ldhint)_LDHINT_NONE);
    return ret == expectedValue;
}

template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::testAndSetAcquire(T *expectedValue, T *newValue)
{
#ifdef __LP64__
    _Asm_mov_to_ar((_Asm_app_reg)_AREG_CCV, (quint64)expectedValue, FENCE);
    T *ret = (T *)_Asm_cmpxchg((_Asm_sz)_SZ_D, (_Asm_sem)_SEM_ACQ,
                               &_q_value, (quint64)newValue, (_Asm_ldhint)_LDHINT_NONE);
#else
    _Asm_mov_to_ar((_Asm_app_reg)_AREG_CCV, (quint32)expectedValue, FENCE);
    T *ret = (T *)_Asm_cmpxchg((_Asm_sz)_SZ_W, (_Asm_sem)_SEM_ACQ,
                               &_q_value, (quint32)newValue, (_Asm_ldhint)_LDHINT_NONE);
#endif
    return ret == expectedValue;
}

template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::testAndSetRelease(T *expectedValue, T *newValue)
{
#ifdef __LP64__
    _Asm_mov_to_ar((_Asm_app_reg)_AREG_CCV, (quint64)expectedValue, FENCE);
    T *ret = (T *)_Asm_cmpxchg((_Asm_sz)_SZ_D, (_Asm_sem)_SEM_REL,
                               &_q_value, (quint64)newValue, (_Asm_ldhint)_LDHINT_NONE);
#else
    _Asm_mov_to_ar((_Asm_app_reg)_AREG_CCV, (quint32)expectedValue, FENCE);
    T *ret = (T *)_Asm_cmpxchg((_Asm_sz)_SZ_W, (_Asm_sem)_SEM_REL,
                               &_q_value, (quint32)newValue, (_Asm_ldhint)_LDHINT_NONE);
#endif
    return ret == expectedValue;
}

#else

extern "C" {
    Q_CORE_EXPORT int q_atomic_test_and_set_int(volatile int *ptr, int expected, int newval);
    Q_CORE_EXPORT int q_atomic_test_and_set_ptr(volatile void *ptr, void *expected, void *newval);
} // extern "C"

#endif

inline bool QBasicAtomicInt::testAndSetRelaxed(int expectedValue, int newValue)
{
    return testAndSetAcquire(expectedValue, newValue);
}

inline bool QBasicAtomicInt::testAndSetOrdered(int expectedValue, int newValue)
{
    return testAndSetAcquire(expectedValue, newValue);
}

template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::testAndSetRelaxed(T *expectedValue, T *newValue)
{
    return testAndSetAcquire(expectedValue, newValue);
}

template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::testAndSetOrdered(T *expectedValue, T *newValue)
{
    return testAndSetAcquire(expectedValue, newValue);
}

#endif // Q_CC_INTEL

inline int QBasicAtomicInt::fetchAndStoreRelaxed(int newValue)
{
    int returnValue;
    for (;;) {
        returnValue = _q_value;
        if (testAndSetRelaxed(returnValue, newValue))
            break;
    }
    return returnValue;
}

inline int QBasicAtomicInt::fetchAndStoreAcquire(int newValue)
{
    int returnValue;
    for (;;) {
        returnValue = _q_value;
        if (testAndSetAcquire(returnValue, newValue))
            break;
    }
    return returnValue;
}

inline int QBasicAtomicInt::fetchAndStoreRelease(int newValue)
{
    int returnValue;
    for (;;) {
        returnValue = _q_value;
        if (testAndSetRelease(returnValue, newValue))
            break;
    }
    return returnValue;
}

inline int QBasicAtomicInt::fetchAndStoreOrdered(int newValue)
{
    int returnValue;
    for (;;) {
        returnValue = _q_value;
        if (testAndSetOrdered(returnValue, newValue))
            break;
    }
    return returnValue;
}

inline int QBasicAtomicInt::fetchAndAddRelaxed(int valueToAdd)
{
    int returnValue;
    for (;;) {
        returnValue = _q_value;
        if (testAndSetRelaxed(returnValue, _q_value + valueToAdd))
            break;
    }
    return returnValue;
}

inline int QBasicAtomicInt::fetchAndAddAcquire(int valueToAdd)
{
    int returnValue;
    for (;;) {
        returnValue = _q_value;
        if (testAndSetAcquire(returnValue, _q_value + valueToAdd))
            break;
    }
    return returnValue;
}

inline int QBasicAtomicInt::fetchAndAddRelease(int valueToAdd)
{
    int returnValue;
    for (;;) {
        returnValue = _q_value;
        if (testAndSetRelease(returnValue, _q_value + valueToAdd))
            break;
    }
    return returnValue;
}

inline int QBasicAtomicInt::fetchAndAddOrdered(int valueToAdd)
{
    int returnValue;
    for (;;) {
        returnValue = _q_value;
        if (testAndSetOrdered(returnValue, _q_value + valueToAdd))
            break;
    }
    return returnValue;
}

inline bool QBasicAtomicInt::ref()
{
    return fetchAndAddRelaxed(1) != -1;
}

inline bool QBasicAtomicInt::deref()
{
    return fetchAndAddRelaxed(-1) != 1;
}

template <typename T>
Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndStoreRelaxed(T *newValue)
{
    T *returnValue;
    for (;;) {
        returnValue = (_q_value);
        if (testAndSetRelaxed(returnValue, newValue))
            break;
    }
    return returnValue;
}

template <typename T>
Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndStoreAcquire(T *newValue)
{
    T *returnValue;
    for (;;) {
        returnValue = (_q_value);
        if (testAndSetAcquire(returnValue, newValue))
            break;
    }
    return returnValue;
}

template <typename T>
Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndStoreRelease(T *newValue)
{
    T *returnValue;
    for (;;) {
        returnValue = (_q_value);
        if (testAndSetRelease(returnValue, newValue))
            break;
    }
    return returnValue;
}

template <typename T>
Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndStoreOrdered(T *newValue)
{
    T *returnValue;
    for (;;) {
        returnValue = (_q_value);
        if (testAndSetOrdered(returnValue, newValue))
            break;
    }
    return returnValue;
}

template <typename T>
Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndAddRelaxed(qptrdiff valueToAdd)
{
    T *returnValue;
    for (;;) {
        returnValue = (_q_value);
        if (testAndSetRelaxed(returnValue, returnValue + valueToAdd))
            break;
    }
    return returnValue;
}

template <typename T>
Q_INLINE_TEMPLATE
T *QBasicAtomicPointer<T>::fetchAndAddAcquire(qptrdiff valueToAdd)
{
    T *returnValue;
    for (;;) {
        returnValue = (_q_value);
        if (testAndSetAcquire(returnValue, returnValue + valueToAdd))
            break;
    }
    return returnValue;
}

template <typename T>
Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndAddRelease(qptrdiff valueToAdd)
{
    T *returnValue;
    for (;;) {
        returnValue = (_q_value);
        if (testAndSetRelease(returnValue, returnValue + valueToAdd))
            break;
    }
    return returnValue;
}

template <typename T>
Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndAddOrdered(qptrdiff valueToAdd)
{
    T *returnValue;
    for (;;) {
        returnValue = (_q_value);
        if (testAndSetOrdered(returnValue, returnValue + valueToAdd))
            break;
    }
    return returnValue;
}

QT_END_NAMESPACE

QT_END_HEADER

#endif // QATOMIC_IA64_H
